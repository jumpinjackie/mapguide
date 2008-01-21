//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SyntaxDatabase.cpp,v 1.34 2006/11/17 18:18:27 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "SyntaxDatabase.hpp"

#include <sstream>
#include "Syntax.hpp"
#include "Key.hpp"
#include "OperationContext.hpp"
#include "KeyStatistics.hpp"
#include "Cursor.hpp"
#include "Container.hpp"
#include "IndexEntry.hpp"

using namespace DbXml;
using namespace std;

static const string index_name("document_index_");
static const string statistics_name("document_statistics_");

SyntaxDatabase::SyntaxDatabase(const Syntax *syntax, DbEnv *env, Transaction *txn, const std::string &name,
                               bool nodesIndexed, u_int32_t pageSize, u_int32_t flags, int mode)
	: syntax_(syntax),
	  environment_(env),
	  containerName_(name),
	  index_(new IndexDatabase(env, name, index_name + syntax->getName(), syntax,
			 pageSize, flags & DB_XA_CREATE)),
	  statistics_(new IndexDatabase(env, name, statistics_name + syntax->getName(), syntax,
			      pageSize, flags & DB_XA_CREATE))
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {
		// Open the Db objects
		err = index_->open(txn, /*duplicates*/true, nodesIndexed,
				   flags, mode);
		if(err == 0)
			err = statistics_->open(txn, /*duplicates*/false,
						nodesIndexed, flags, mode);
	}
	catch (XmlException &xe) {
		err = xe.getExceptionCode();
	}
	if (err != 0) {
		if (err == EINVAL)
			err = ENOENT; // 4.4 will return EINVAL for
		                       // non-existent in-memory DBs
		if (err != ENOENT && txn)
			txn->abort();
		// unconditionally cleanup -- ignore errors
		try { index_->close(0); } catch (...) {}
		if(err == EEXIST)
			throw XmlException(
				XmlException::CONTAINER_EXISTS,
				db_strerror(err));
		throw XmlException(err); // this creates a DB error
	}
}

SyntaxDatabase::~SyntaxDatabase()
{
	// nothing to do
}

int SyntaxDatabase::getIndexData(OperationContext &context, IndexData::Ptr &data,
				 SecondaryDatabase::Operation op1, const Key &k1,
				 SecondaryDatabase::Operation op2, const Key &k2) const
{
	DBXML_ASSERT(k1.getSyntaxType() == syntax_->getType());
	DBXML_ASSERT(op2 == DbWrapper::NONE || k2.getSyntaxType() == syntax_->getType());

	ScopedPtr<IndexCursor> cursor;
	int err;
	if(op2 == DbWrapper::NONE) {
		cursor.reset(const_cast<IndexDatabase*>(
				     getIndexDB())->createCursor(
					     context.txn(), op1, &k1, false));
	} else {
		cursor.reset(const_cast<IndexDatabase*>(
				     getIndexDB())->createCursor(
					     context.txn(), op1,
					     &k1, op2, &k2, false));
	}
	err = cursor->error();

	if(err == 0) {
		if(k1.getIndex().getKey() == Index::KEY_EQUALITY) {
			IndexEntry::Ptr ie(new IndexEntry());
			err = cursor->first(*ie); // could throw
			while(err == 0 && ie->getDocID() != 0) {
				data->insert(ie);
				ie.reset(new IndexEntry());
				err = cursor->next(*ie); // could throw
			}
		} else {
			vector<IndexEntry::Ptr> tmpStore;
			tmpStore.reserve(256);

			IndexEntry::Ptr ie(new IndexEntry());
			err = cursor->first(*ie); // could throw
			if(err == 0 && ie->getDocID() != 0) {
				IndexEntry::Ptr last = ie;
				tmpStore.push_back(ie);
				ie.reset(new IndexEntry());
				while((err = cursor->next(*ie)) == 0 && // could throw
					ie->getDocID() != 0) {

					if(*ie < *last) {
						data->insert(tmpStore.begin(), tmpStore.end());
						tmpStore.clear();
					}

					last = ie;
					tmpStore.push_back(ie);
					ie.reset(new IndexEntry());
				}
			}

			data->insert(tmpStore.begin(), tmpStore.end());
		}
	}

	return err;
}

int SyntaxDatabase::updateStatistics(OperationContext &context, DbtIn &key, const KeyStatistics &statistics)
{
	Cursor cursor(*statistics_.get(), context.txn(), CURSOR_WRITE);
	int err = cursor.get(&key, &context.data(), DB_SET | (context.txn() ? DB_RMW : 0)); // could throw on error
	if (err == 0) {
		KeyStatistics existing;
		existing.setThisFromDbt(context.data());
		existing.add(statistics);
		existing.setDbtFromThis(context.data());
		err = cursor.put(&key, &context.data(), DB_CURRENT); // could throw on error
	} else if (err == DB_NOTFOUND) {
		statistics.setDbtFromThis(context.data());
		err = cursor.put(&key, &context.data(), DB_KEYFIRST); // could throw on error
	}
	return err;
}

// use DB prefix iterator to remove all index entries for the specified index.
// do both index and statistics databases.
// The prefix is <indexKeyPrefix>[<nameId>]
int SyntaxDatabase::removeIndex(OperationContext &context, const Index &index,
				const NameID &id)
{
	// key is <indexType><name ID>, which is one byte plus a marshaled integer.
	unsigned char keybuf[10];
	keybuf[0] = index.getKeyPrefix();
	u_int32_t sz = 1;
	if (id != 0) {
		// not a default index
		sz += id.marshal(keybuf+1);
	}
	int err = removeIndexEntries(context, *index_.get(), keybuf, sz);
	if (err == 0)
		err = removeIndexEntries(context, *statistics_.get(), keybuf, sz);
	return err;
}

int SyntaxDatabase::removeIndexEntries(OperationContext &context, DbWrapper &db,
				       void *buf, u_int32_t bufsize)
{
	unsigned char keybuf[10];
	// assert bufsize < 10;
	memcpy(keybuf, buf, bufsize);
	DbtIn key(keybuf, bufsize);
	key.set_ulen(bufsize);
	key.set_dlen(bufsize);
	key.set_doff(0);
	key.set_flags(DB_DBT_PARTIAL);
	DbtIn data(0,0);
	data.set_flags(DB_DBT_PARTIAL);
	data.set_dlen(0);
		
	u_int32_t flags = (context.txn() ? DB_RMW : 0);
	int err;

	// remove from index database
	Cursor cursor(db, context.txn(), CURSOR_WRITE);
	err = cursor.get(&key, &data, flags|DB_SET_RANGE);
	while ((err == 0) && (memcmp(key.get_data(), buf, bufsize) == 0)) {
		cursor.del(0);
		err = cursor.get(&key, &data, flags|DB_NEXT);
		if (err == DB_NOTFOUND)
			break;
	}
	if (err == DB_NOTFOUND)
		err = 0;
	return err;
}


int SyntaxDatabase::dump(const Syntax *syntax, DbEnv *env, const std::string &name, std::ostream *out)
{
	IndexDatabase::Ptr index(new IndexDatabase(env, name, index_name + syntax->getName(), syntax, 0, 0));
	IndexDatabase::Ptr statistics(new IndexDatabase(env, name, statistics_name + syntax->getName(), syntax, 0, 0));

	int err = Container::writeHeader(index->getDatabaseName(), out);
	if(err == 0) err = index->dump(out);

	if(err == 0) err = Container::writeHeader(statistics->getDatabaseName(), out);
	if(err == 0) err = statistics->dump(out);

	return err;
}

int SyntaxDatabase::load(const Syntax *syntax, DbEnv *env, const std::string &name, std::istream *in, unsigned long *lineno)
{
	IndexDatabase::Ptr index(new IndexDatabase(env, name, index_name + syntax->getName(), syntax, 0, 0));
	IndexDatabase::Ptr statistics(new IndexDatabase(env, name, statistics_name + syntax->getName(), syntax, 0, 0));

	// Load index
	int err = Container::verifyHeader(index->getDatabaseName(), in);
	if(err != 0) {
		ostringstream oss;
		oss << "SyntaxDatabase::load() invalid database dump file loading '" << name << "'";
		Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
	}
	else {
		err = index->load(in, lineno);
	}

	// Load statistics
	if(err == 0) {
		err = Container::verifyHeader(statistics->getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "SyntaxDatabase::load() invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
		}
		else {
			err = statistics->load(in, lineno);
		}
	}

	return err;
}

int SyntaxDatabase::verify(const Syntax *syntax, DbEnv *env, const std::string &name,
	std::ostream *out, u_int32_t flags)
{
	IndexDatabase::Ptr index(
		new IndexDatabase(env, name, index_name + syntax->getName(),
			syntax, 0, 0));
	// not all index DBs may exist, handle this by
	// attempting to open
	int err = index->open(0, /*duplicates*/true, /*nodesIndexed*/true, 0, 0);

	if(err != 0) {
		if(err == ENOENT)
			return 0; // doesn't exist, nothing to do
		throw XmlException(err);
	}
	// open worked -- DB exists.  Now close it for verify.
	index.reset(
		new IndexDatabase(env, name, index_name + syntax->getName(),
			syntax, 0, 0));
	IndexDatabase::Ptr statistics(
		new IndexDatabase(env, name, statistics_name + syntax->getName(),
			syntax, 0, 0));
	if(flags & DB_SALVAGE) {
		err = Container::writeHeader(index->getDatabaseName(), out);
	}
	if(err == 0) {
		err = index->verify(out, flags);
	}
	
	if(err == 0 && (flags & DB_SALVAGE)) {
		err = Container::writeHeader(statistics->getDatabaseName(), out);
	}
	if(err == 0) {
		err = statistics->verify(out, flags);
	}
	return err;
}
