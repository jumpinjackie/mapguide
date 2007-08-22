//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbWrapper.cpp,v 1.47 2006/10/30 17:45:52 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "DbWrapper.hpp"
#include "Cursor.hpp"
#include "Key.hpp"
#include "NameID.hpp"
#include "DocID.hpp"
#include "Log.hpp"
#include "OperationContext.hpp"
#include "IndexEntry.hpp"
#include "db_rdbt.h"
#include "nodeStore/NsUtil.hpp"

#include <cassert>
#include <cerrno>
#include <sstream>

using namespace std;
using namespace DbXml;

// DbWrapper
#define DBXML_DEFAULT_PAGESIZE (8 * 1024)

DbWrapper::DbWrapper(DbEnv *environment, const std::string &name,
		     const std::string &prefixName,
		     const std::string &databaseName,
		     u_int32_t pageSize, u_int32_t flags)
	: needsToBeClosed_(false),
	  name_(name),
	  prefixName_(prefixName),
	  databaseName_(databaseName),
	  pageSize_(pageSize),
	  db_(environment, flags),
	  environment_(environment)
{
}

DbWrapper::~DbWrapper()
{
	if (needsToBeClosed_) {
		try {
			close(0);
		}
		catch (DbException &e) {
			ostringstream oss;
			oss << "DB exception during database close: " <<
				e.what();
			Log::log(environment_, Log::C_CONTAINER,
				 Log::L_ERROR, oss.str().c_str());
		}
		catch (...) {
			// Exceptions thrown from destructors are a bad thing.
			//DBXML_ASSERT(0);
			Log::log(environment_, Log::C_CONTAINER, Log::L_ERROR,
				 "Unknown exception during database close: ");
		}
	}
}

int DbWrapper::open(Transaction *txn, DBTYPE type, u_int32_t flags, int mode)
{
	int err = 0;
	if (pageSize_ > 0)
		db_.set_pagesize(pageSize_);

	if(flags&DBXML_CHKSUM) {
#if (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR < 2)
		db_.set_flags(DB_CHKSUM_SHA1);
#else
		db_.set_flags(DB_CHKSUM);
#endif
		flags &= ~DBXML_CHKSUM;
	}

	if(flags&DBXML_ENCRYPT) {
		db_.set_flags(DB_ENCRYPT);
		flags &= ~DBXML_ENCRYPT;
	}

	if(flags&DB_TXN_NOT_DURABLE) {
		db_.set_flags(DB_TXN_NOT_DURABLE);
		flags &= ~DB_TXN_NOT_DURABLE;
	}

	string dbname(getDatabaseName());

	// The file is opened in the directory specified by the environment.
	const char *fileName= 0;
	const char *databaseName = 0;
	if(name_.length() > 0) {
#ifdef DBXML_USE_SEPARATE_DBS
		// this is for DEBUG use only because it only
		// allows one container for a given directory/environment
		// it's used internally., only for 
		fileName = dbname.c_str();
#else
		fileName= name_.c_str();
		databaseName = dbname.c_str();
#endif
	} else {
		// creating an in-memory container -- DB_CREATE keeps
		// DB quiet about this
		flags |= DB_CREATE;
	}
	try {
		err = db_.open(Transaction::toDbTxn(txn), fileName, databaseName, type, flags, mode);
	}
	catch(DbException &de) {
		err = de.get_errno();
		if (err == DB_LOCK_DEADLOCK)
			throw XmlException(de);
	}
	if (err == 0) {
		needsToBeClosed_ = true;
		if (pageSize_ == 0) {
			// Find out the page size of the underlying database.
			// We don't use stat because it will fail with
			// 'Invalid argument' if the open is within a txn.
			//
			pageSize_ = db_.get_DB()->pgsize;
		}
	}
	return err;
}

int DbWrapper::close(u_int32_t flags)
{
	// We are forgiving and allow close to be called even if the db
	// isn't actually open. This is for the error handling code in
	// Container. It calls close on all the DbWrapper objects to make
	// sure they are all in the closed state before they are destroyed.
	//
	int err = 0;
	if (needsToBeClosed_) {
		needsToBeClosed_ = false; // db.close kills the db handle even if it fails.
		try {
			err = db_.close(flags);
		}
		catch(DbException &de) {
			err = de.get_errno();
		}
	}
	return err;
}

/*
 * pr_callback - C++ callback function for using pr_* functions from C++.
 */
extern "C"
{
	static int pr_callback(void *handle, const void *str_arg) {
		std::ostream &out = *(std::ostream *)handle;
		const char *str = (const char *)str_arg;

		out << str;
		return (0);
	}
} /* extern "C" */

int DbWrapper::dump(std::ostream *out)
{
	int ret, err = 0;

	if ((ret = open(NULL, DB_UNKNOWN, DB_RDONLY, 0)) == 0) {
		DB *dbp = db_.get_DB();
		err = dbp->dump(dbp, NULL, pr_callback, out, 0, 1);
		ret = close(0);
	}
	if (ret == 0)
		ret = err;
	return (ret);
}

extern "C"
{
	/*
	 * Implementation of READ_FN for reading from a C++ istream.
	 * Reads at most 'len' characters into 'buf' up to the first 'until' characater
	 * (if non-zero). The terminator (if found) is discarded, and the string is nul
	 * terminated if 'len' > 1.
	 * Returns: zero on success, DB_NOTFOUND if 'until' != 0 and not found, or
	 * EOF on EOF
	 */
	static int
	read_callback(char *buf, size_t len, char until, void *handle) {
		std::istream &in = *(std::istream *)handle;
		char *p = buf;
		char c;
		size_t bytes_read;

		for (bytes_read = 0; bytes_read < len; bytes_read++) {
			if (!in.get(c) || (until != 0 && c == until))
				break;
			else
				*p++ = c;
		}

		if (bytes_read < len)
			*p = '\0';

		if (!in && bytes_read == 0)
			return (EOF);
		else if (until != 0 && c != until)
			return (DB_NOTFOUND);
		else
			return (0);
	}
} /* extern "C" */

int DbWrapper::load(std::istream *in, unsigned long *lineno)
{
	int version, ret, t_ret;
	DBTYPE dbtype;
	char *subdb;
	u_int32_t read_flags;
	Dbt key, data;
	db_recno_t recno, datarecno;
	DB_ENV *dbenv = environment_ ? environment_->get_DB_ENV() : 0;

	if ((ret = __db_rheader(dbenv, db_.get_DB(), &dbtype,
				&subdb, &version, &read_flags, read_callback, in, lineno)) != 0)
		goto err;

	/* We always print with keys */
	if (!(read_flags & DB_READ_HASKEYS)) {
		db_.errx("Invalid DbXml dump: keys missing");
		ret = EINVAL;
		goto err;
	}

	if ((ret = open(NULL, dbtype, DB_CREATE|DB_EXCL, 0)) != 0)
		goto err;

	/* Initialize the key/data pair. */
	if (dbtype == DB_RECNO || dbtype == DB_QUEUE) {
		key.set_size(sizeof(recno));
		key.set_data(&datarecno);
	} else {
		key.set_ulen(1024);
		key.set_data((void *)malloc(key.get_ulen()));
	}
	data.set_ulen(1024);
	data.set_data((void *)malloc(data.get_ulen()));
	if (key.get_data() == NULL || data.get_data() == NULL) {
		db_.err(ENOMEM, NULL);
		goto err;
	}

	/* Get each key/data pair and add them to the database. */
	for (recno = 1;; ++recno) {
		if ((ret = __db_rdbt(dbenv, key.get_DBT(), data.get_DBT(),
				     read_flags, read_callback, in, lineno)) != 0) {
			if (ret == EOF)
				ret = 0;
			break;
		}

		switch (ret = db_.put(NULL, &key, &data, 0)) {
		case 0:
			break;
		case DB_KEYEXIST:
			db_.errx("line %d: key already exists, not loaded:", *lineno);
			dbenv->prdbt(key.get_DBT(), read_flags & DB_READ_PRINTABLE,
				     0, &std::cerr, pr_callback, 0);
			break;
		default:
			db_.err(ret, NULL);
			goto err;
		}
	}

 err:	/* Close the database. */
	if ((t_ret = close(0)) != 0 && ret == 0)
		ret = t_ret;

	/* Free allocated memory. */
	if (subdb != NULL)
		free(subdb);
	if (dbtype != DB_RECNO && dbtype != DB_QUEUE)
		free(key.get_data());
	if (data.get_data() != NULL)
		free(data.get_data());

	return (ret);
}

int DbWrapper::verify(std::ostream *out, u_int32_t flags)
{
#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 2)
	needsToBeClosed_ = false; // db.verify kills the db handle even if it fails.
#endif
	try {
		return db_.verify(
			name_.length() == 0 ? 0 : name_.c_str(),
			getDatabaseName().c_str(), out, flags);
	}
	catch(DbException &de) {
		return de.get_errno();
	}
}

const char *DbWrapper::operationToString(DbWrapper::Operation operation)
{
	switch (operation) {
	case NONE:
		return "none";
	case ALL:
		return "all";
	case EQUALITY:
		return "=";
	case LTX:
		return "<";
	case LTE:
		return "<=";
	case GTX:
		return ">";
	case GTE:
		return ">=";
	case RANGE:
		return "range";
	case PREFIX:
		return "prefix";
	case SUBSTRING:
		return "substring";
	}
	return "unknown";
}

const char *DbWrapper::operationToWord(DbWrapper::Operation operation)
{
	switch (operation) {
	case NONE:
		return "none";
	case ALL:
		return "all";
	case EQUALITY:
		return "eq";
	case LTX:
		return "lt";
	case LTE:
		return "lte";
	case GTX:
		return "gt";
	case GTE:
		return "gte";
	case RANGE:
		return "range";
	case PREFIX:
		return "prefix";
	case SUBSTRING:
		return "substring";
	}
	return "unknown";
}

u_int32_t DbWrapper::getPageSize() const
{
	return pageSize_;
}

unsigned long DbWrapper::getNumberOfPages() const
{
	return 0xFFFF; // JCM - Let's just assume that a sequential scan is very expensive.
}

// PrimaryDatabase

PrimaryDatabase::PrimaryDatabase(DbEnv *environment, const std::string &name,
				 const std::string& databaseName,
				 u_int32_t pageSize, u_int32_t flags)
	: DbWrapper(environment, name, "primary_",
		    databaseName, pageSize, flags)
{}

PrimaryDatabase::~PrimaryDatabase()
{}

int PrimaryDatabase::open(Transaction *txn, u_int32_t flags, int mode)
{
	return DbWrapper::open(txn, DB_RECNO, flags, mode);
}

int PrimaryDatabase::getPrimary(OperationContext &context, const NameID &id,
				DbtOut *data, u_int32_t flags) const
{
	id.setDbtFromThis(context.key());
	return get(context.txn(), &context.key(), data, flags);
}

int PrimaryDatabase::putPrimary(OperationContext &context, const NameID &id,
				const Dbt *data, u_int32_t flags)
{
	id.setDbtFromThis(context.key());
	return put(context.txn(), &context.key(), const_cast<Dbt*>(data), flags);
}

int PrimaryDatabase::appendPrimary(OperationContext &context, NameID &id,
				   const Dbt *data, u_int32_t flags)
{
	int err = put(context.txn(), &context.key(), const_cast<Dbt*>(data),
		      flags | DB_APPEND);
	if (err == 0)
		id.setThisFromDbtAsId(context.key());
	return err;
}

int PrimaryDatabase::deletePrimary(OperationContext &context,
				   const NameID &id, u_int32_t flags)
{
	id.setDbtFromThis(context.key());
	return del(context.txn(), &context.key(), flags);
}

// SecondaryDatabase

SecondaryDatabase::SecondaryDatabase(DbEnv *environment,
				     const std::string &name,
				     const std::string& databaseName,
				     u_int32_t pageSize, u_int32_t flags)
	: DbWrapper(environment, name, "secondary_",
		    databaseName, pageSize, flags)
{
}

int SecondaryDatabase::open(Transaction *txn, bool duplicates,
			    u_int32_t flags, int mode)
{
	if (duplicates) {
		db_.set_flags(DB_DUP|DB_DUPSORT);
	}
	return DbWrapper::open(txn, DB_BTREE, flags, mode);
}

// IndexDatabase

extern "C" {
int
index_duplicate_compare(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	const xmlbyte_t *p1 = (const xmlbyte_t *)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t *)dbt2->data;
	++p1; ++p2; // Get past format byte

	// Compare the DocID first
	int res = DocID::compareMarshaled(p1, p2);
	if(res != 0) return res;

	// Compare the node ID by using a lexicographical
	// comparison, stopping at the terminating null.
	//
	// (If no node ID is stored, a null is stored instead,
	//  so that we can simplify this sort algorithm.)

	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}
};

IndexDatabase::IndexDatabase(DbEnv *environment, const std::string &name,
			     const std::string& databaseName,
			     const Syntax *syntax, u_int32_t pageSize,
			     u_int32_t flags)
	: DbWrapper(environment, name, "secondary_",
		    databaseName, pageSize, flags),
	  syntax_(syntax)
{
	DBXML_ASSERT(syntax != 0);

	DbWrapper::bt_compare_fn fn = syntax->get_bt_compare();
	if(fn != 0) {
		db_.set_bt_compare(fn);
	}
}

int IndexDatabase::open(Transaction *txn, bool duplicates, bool nodesIndexed, u_int32_t flags, int mode)
{
	if(duplicates) {
		db_.set_flags(DB_DUPSORT);
		// Don't need a custom comparator unless nodes are indexed
		db_.set_dup_compare(index_duplicate_compare);
	}
	return DbWrapper::open(txn, DB_BTREE, flags, mode);
}

// We assume that key has the correct endianness.
int IndexDatabase::getIndexEntry(OperationContext &context, const Dbt &key, IndexEntry &ie) const
{
	u_int32_t flags = (context.txn()) ? DB_READ_COMMITTED : 0;
	int err = get(context.txn(), &(Dbt&)key, &context.data(), flags);
	if(err == 0) {
		ie.setThisFromDbt(context.data());
	}
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::putIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data)
{
	int err = put(context.txn(), &(Dbt&)key, &(Dbt&)data, DB_NODUPDATA);
	if(err == DB_KEYEXIST) err= 0; // Suppress duplicate data error, it's OK really.
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::updateIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data)
{
	Cursor cursor(*this, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if(err == 0) {
		// We want to do a partial retrieval of 0 bytes,
		// to get the cursor in the right position
		Dbt tmp;
		tmp.set_data(data.get_data());
		tmp.set_size(data.get_size());
		tmp.set_doff(0); 
		tmp.set_dlen(0);
		tmp.set_flags(DB_DBT_PARTIAL);

		// Find the existing entry
		err = cursor.get(const_cast<Dbt*>(&key), &tmp, DB_GET_BOTH);

		if(err == 0) {
			err = cursor.del( /*no flags*/0);
		} else if(err == DB_NOTFOUND) {
			// If we can't find the index entry already, just add it
			err = 0;
		}

		if(err == 0) {
			err = cursor.put(const_cast<Dbt*>(&key), const_cast<Dbt*>(&data), DB_NODUPDATA);
			if(err == DB_KEYEXIST) err = 0; // Suppress duplicate data error, it's OK really.
		}
	}
	return err;
}

// We assume that key has the correct endianness.
int IndexDatabase::delIndexEntry(OperationContext &context, const Dbt &key, const Dbt &data)
{
	Cursor cursor(*this, context.txn(), CURSOR_WRITE);
	int err = cursor.error();
	if (err == 0) {
		err = cursor.get(&(Dbt&)key, &(Dbt&)data, DB_GET_BOTH);
		if (err == 0) {
			err = cursor.del( /*no flags*/0);
		}
	}
	return err;
}

// We assume that key has the correct endianness.
bool IndexDatabase::exists(Transaction *txn, const Dbt &key) const
{
	// Check if the key already exists.
	// DB_DBT_PARTIAL and len=0 prevents retrieval of the data.
	DbtIn dbt;
	dbt.set_flags(DB_DBT_PARTIAL);
	int err = get(txn, &(Dbt&)key, &dbt, /*no flags*/0);
	return (err != DB_NOTFOUND);
}

void IndexDatabase::getMinKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyMin(0);
	if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
		keyMin.set(key, 0, 0);
	}
	else {
		keyMin.set(key);
	}
	keyMin.setDbtFromThis(dbt);
}

void IndexDatabase::getMaxKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyMin(0);
	if(key.getIndex().getKey() == Index::KEY_EQUALITY) {
		keyMin.set(key, 0, 0);
	}
	else {
		keyMin.set(key);
	}
	getNextKeyDbt(keyMin, dbt);
}

void IndexDatabase::getNextKeyDbt(const Key &key, DbtOut &dbt) const
{
	Key keyNext(0);
	keyNext.set(key);
	keyNext.setDbtFromThis(dbt);

	// We want to increment the dbt value to the next key,
	// but if incrementing the last byte results in a carry
	// (value overflows and goes back to zero), then we have
	// to increment the last-but-one byte - and so on.
	char *maxPtr = (char *)dbt.get_data();
	u_int32_t maxSize = dbt.get_size();
	char *pos = maxPtr + maxSize;
	for(--pos; pos >= maxPtr; --pos) {
		++(*pos);
		if(*pos != 0) break;
	}
}

double IndexDatabase::percentage(OperationContext &context, Operation operation, Operation gto, Operation lto, const Key &key1, const Key &key2) const
{
	DbtOut &dbt1 = context.key();
	DbtOut &dbt2 = context.data();

	DB_KEY_RANGE krMin;
	getMinKeyDbt(key1, dbt1);
	key_range(context.txn(), &dbt1, &krMin, 0);

	DB_KEY_RANGE krMax;
	getMaxKeyDbt(key1, dbt1);
	key_range(context.txn(), &dbt1, &krMax, 0);

	// range is the % of the database keys that the keys for this index occupy.
	double range = krMax.less - krMin.less;
	double extent = 0.0;

	if (range > 0.0) {
		// extent is the % of the database keys that the keys for this index match this operation.
		DB_KEY_RANGE kr1;
		DB_KEY_RANGE kr2;

		switch(operation) {
		case DbWrapper::PREFIX: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			getNextKeyDbt(key1, dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - kr1.less;
			break;
		}
		case DbWrapper::LTX:
		case DbWrapper::LTE: {
			key1.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - krMin.less + (operation == DbWrapper::LTE ? kr2.equal : 0);
			break;
		}
		case DbWrapper::GTX:
		case DbWrapper::GTE: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			extent = krMax.less + krMax.equal - kr1.less + (operation == DbWrapper::GTX ? kr1.equal : 0);
			break;
		}
		case DbWrapper::RANGE: {
			key1.setDbtFromThis(dbt1);
			key_range(context.txn(), &dbt1, &kr1, 0);
			key2.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.less - kr1.less + (lto == DbWrapper::LTE ? kr2.equal : 0) + (gto == DbWrapper::GTX ? kr1.equal : 0);
			break;
		}
		case DbWrapper::EQUALITY: {
			key1.setDbtFromThis(dbt2);
			key_range(context.txn(), &dbt2, &kr2, 0);
			extent = kr2.equal;
			break;
		}
		case DbWrapper::ALL: {
			extent = range;
			break;
		}
		default: {
			break;
		}
		}
	}

	// extent/range is the % of keys within this index that match this operation.
	return (range == 0 ? 0 : extent / range);
}

IndexCursor *IndexDatabase::createCursor(Transaction *txn,
					 DbWrapper::Operation operation,
					 const Key *key, bool reverse)
{
	switch(operation) {
	case DbWrapper::EQUALITY:
		// For Equals, reverse makes no sense
		return new EqualsIndexCursor(*this, txn, key);
		break;
	case DbWrapper::PREFIX:
		if (!reverse)
			return new PrefixIndexCursor(*this, txn, key);
		else
			return new ReversePrefixIndexCursor(*this, txn, key);
		break;
	case DbWrapper::LTX:
	case DbWrapper::LTE:
	case DbWrapper::GTX:
	case DbWrapper::GTE:
	case DbWrapper::ALL:
		if (!reverse)
			return new InequalityIndexCursor(
				*this, txn, operation,
				key, syntax_);
		else
			return new ReverseInequalityIndexCursor(
				*this, txn, operation,
				key, syntax_);
		break;
	case DbWrapper::NONE:
	case DbWrapper::RANGE:
		DBXML_ASSERT(0);
		break;
	default: break;
	}
	return NULL;
}

IndexCursor *IndexDatabase::createCursor(
	Transaction *txn, DbWrapper::Operation gto,
	const Key *gtk, DbWrapper::Operation lto,
	const Key *ltk, bool reverse)
{
	if (!reverse)
		return new InequalityIndexCursor(*this, txn, gto, gtk,
						 lto, ltk, syntax_);
	else
		return new ReverseInequalityIndexCursor(
			*this, txn, gto, gtk, lto, ltk, syntax_);
}

NsIterator *IndexDatabase::createIterator(Transaction *txn, const Key *key)
{
	return new EqualsIndexIterator(*this, txn, key);
}
