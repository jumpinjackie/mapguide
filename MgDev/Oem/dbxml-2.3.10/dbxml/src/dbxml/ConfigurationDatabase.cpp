//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ConfigurationDatabase.cpp,v 1.35.2.1 2007/01/09 22:32:49 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "ConfigurationDatabase.hpp"

#include <sstream>
#include <dbxml/XmlException.hpp>

#include "Cursor.hpp"
#include "Manager.hpp"
#include "Buffer.hpp"
#include "Container.hpp"
#include "TypeConversions.hpp"

using namespace DbXml;
using namespace std;

static const char *configuration_name = "configuration";
static const char *sequence_name = "sequence";

// DbSequence is associated with the configuration database because it's
// always present.  It requires a database without duplicates.
//
// DbSequence usage notes:
//  numeric sequences start at the value, 2, and can go to
//  (uint32)0xffffffff.  After that, the DbSequence.get() call will
//  throw an exception.
// Sequence numbers are NEVER reused.  This limits the number of
// documents in a container to not the current count, but the count
// of documents *ever* created in that container.
// It's actually smaller than that, because, in the interest of performance,
// The cache value is set to 5 (see code below).  Cached sequence numbers
// are never returned, so it's possible to skip unused numbers from the cache.
//
// TBD: eventually, the simplest solution to limits is to make Document IDs
// 64-bit numbers (db_seq_t).
//

static DbSequence * initDbSequence(Transaction *txn, Db &db, u_int32_t incr,
				   u_int32_t flags)
{
	Dbt seqKey((void*)"dbxmlID", 7);
	static db_seq_t initValue = 2;
	static db_seq_t maxValue = (db_seq_t)(unsigned int)0xffffffff;
	DbTxn *dbtxn = 0;
	size_t cacheSize = (txn ? 0 : incr);

	if (txn)
		dbtxn = txn->getDbTxn();

	// always set DB_CREATE and DB_THREAD
	flags &= (DB_TXN_NOSYNC);
	flags |= DB_CREATE|DB_THREAD;
	DbSequence *seq =  new DbSequence(&db, 0);
	// no-op if already initialized
	seq->initial_value(initValue);
	seq->set_range(initValue, maxValue);
	seq->set_cachesize(cacheSize);
	int err = seq->open(dbtxn, &seqKey, flags);
	if (err)
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "Could not create DbSequence object");
	return seq;
}

ConfigurationDatabase::ConfigurationDatabase(
	DbEnv *env, Transaction *txn, const std::string &name,
	XmlContainer::ContainerType &type,
	u_int32_t pageSize, u_int32_t seqIncr, u_int32_t flags, int mode,
	bool &indexNodes, bool doVersionCheck)
	: environment_(env),
	  name_(name),
	  database_(env, name, "secondary_", configuration_name,
		    pageSize, flags & DB_XA_CREATE),
	  seqDatabase_(env, name, "secondary_", sequence_name,
		       pageSize, flags & DB_XA_CREATE),
	  seq_(0),
	  seqIncr_(seqIncr),
	  creatingTxn_(txn)
{
	flags &= ~DB_XA_CREATE;
	int err = 0;
	try {
		bool rdonly = (flags & DB_RDONLY) != 0;

		// Open the Db objects
		err = database_.open(txn, DB_BTREE, flags, mode);
		// Check version before attempting to open sequence DB,
		// which will fail for pre-2.2 containers
		if (err == 0 && doVersionCheck)
			err = checkVersion(txn, rdonly);
		if (err == 0)
			err = seqDatabase_.open(txn, DB_BTREE, flags, mode);

		if (err == 0)
			seq_ = initDbSequence(txn, seqDatabase_.getDb(),
					      seqIncr_, flags);

		// Add the default index specification, if there
		// isn't one already
		if(err == 0 && !rdonly)
			err = checkIndexSpecification(txn);

		// Add, or set the container type.  If this is
		// a create, it won't be there. "type" is by ref
		if(err == 0)
			type = checkContainerType(txn, database_, type, rdonly);

		// Add indexNodes, if it's not there already, otherwise
		// retrieve it.
		if(err == 0)
			indexNodes = checkIndexNodes(txn, indexNodes, rdonly);
	} catch (DbException &e) {
		err = e.get_errno();
		// fall through
	}
	catch (XmlException &) {
		if (txn)
			txn->abort();
		throw;
	}

	if (err != 0) {
		if (txn)
			txn->abort();
		string msg = name;
		if (err == EEXIST) {
			msg += ": container exists";
			throw XmlException(
				XmlException::CONTAINER_EXISTS, msg);
		} else if (err == ENOENT) {
			msg += ": container file not found, or not a container";
			throw XmlException(XmlException::CONTAINER_NOT_FOUND,
					   msg);
		}
		throw XmlException(err);
	}
}

ConfigurationDatabase::~ConfigurationDatabase()
{
	if (seq_) {
		seq_->close(0);
		delete seq_;
	}
}

int ConfigurationDatabase::generateID(Transaction *txn, DocID &newId)
{
	int err = 0;
	db_seq_t newValue = 0;
	u_int32_t flags = 0;
	DbTxn *dbtxn = 0;
	if (txn) {
		if (txn == creatingTxn_) {
			dbtxn = txn->getDbTxn();
		} else {
			flags |= DB_AUTO_COMMIT|DB_TXN_NOSYNC;
		}
	}
	try {
		err = seq_->get(dbtxn, 1, &newValue, flags);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	newId = (docId_t)newValue;
	return err;
}

void ConfigurationDatabase::resetSequenceCache()
{
	creatingTxn_ = 0;
	try {
		seq_->set_cachesize(seqIncr_);
	} catch (DbException &e) {
		throw XmlException(e);
	}
}

int ConfigurationDatabase::getConfigurationItem(
	Transaction *txn, const char *key, size_t keyLength, Buffer &b,
	bool lock) const
{
	// lock means that we acquire a write lock for reading the
	// configuration item. We use this when we're going to change
	// an item and we want to block all the readers and writers
	// until we're done.
	//
	DbtIn k((void*)key, keyLength);
	DbtOut v;
#if 1
	u_int32_t flags = (txn ? (lock ? DB_RMW : DB_READ_COMMITTED) : 0);
	int err = database_.get(txn, &k, &v, flags);
#else
	Cursor cursor(database_, txn, CURSOR_READ);
	int err = cursor.get(&k, &v, DB_SET | (txn && lock ? DB_RMW : 0));
#endif
	if (err == 0) {
		b.write(v.get_data(), v.get_size());
	}
	return err;
}

int ConfigurationDatabase::putConfigurationItem(
	Transaction *txn, const char *key, const Buffer &b)
{
	DbtIn k((void*)key, strlen(key) + 1);
	DbtIn v(b.getBuffer(), b.getOccupancy());
	return database_.put(txn, &k, &v, 0);
}

XmlContainer::ContainerType ConfigurationDatabase::checkContainerType(
	Transaction *txn, DbWrapper &db, XmlContainer::ContainerType type,
	bool isReadonly)
{
	static const char *ctypeKey = "ctype";
	static const char *ctypeDoc = "doc";
	static const char *ctypeNode = "node";
	// Get container type from DB
	DbtIn keydbt((void *) ctypeKey, 6); // include null in key
	DbtOut datadbt;
	int err = db.get(txn, &keydbt, &datadbt, 0);
	if (!err) {
		// existing DB.
		const char *ttype = (const char*)datadbt.get_data();
		if (::strcmp(ttype, ctypeNode) == 0)
			type = XmlContainer::NodeContainer;
		else if (::strcmp(ttype, ctypeDoc) == 0)
			type = XmlContainer::WholedocContainer;
		else
			throw XmlException(XmlException::INTERNAL_ERROR,
			   "Unknown container type in configuration database");
	} else if (err == DB_NOTFOUND) {
		if (isReadonly) {
			throw XmlException(XmlException::INVALID_VALUE,
				   "Cannot set type on read-only Container");
		}
		// set type
		if ((type != XmlContainer::WholedocContainer) &&
		    (type != XmlContainer::NodeContainer))
			throw XmlException(XmlException::INVALID_VALUE,
					   "Unknown container typer");
		const char *ttype =
			((type == XmlContainer::WholedocContainer) ?
			 ctypeDoc : ctypeNode);
		datadbt.set((void*)ttype, ::strlen(ttype) + 1);
		err = db.put(txn, &keydbt, &datadbt, 0);

		if (err)
			throw XmlException(XmlException::DATABASE_ERROR,
			  "Unexpected error from DB setting container type");

	} else {
		throw XmlException(XmlException::DATABASE_ERROR,
			   "Unexpected error from DB getting container type");
	}
	return type;
}

bool ConfigurationDatabase::checkIndexNodes(Transaction *txn, bool indexNodes,
					    bool isReadonly)
{
	static const char *indexNodesKey = "indexNodes";
	// Get container type from DB
	DbtIn keydbt((void *) indexNodesKey, strlen(indexNodesKey) + 1); // include null in key
	DbtOut datadbt;
	int err = database_.get(txn, &keydbt, &datadbt, 0);
	if (!err) {
		// existing DB.
		uint8_t val = *(const uint8_t*)datadbt.get_data();
		indexNodes = (val == 1);
	} else if (err == DB_NOTFOUND) {
		if (isReadonly) {
			throw XmlException(XmlException::INVALID_VALUE,
				   "Cannot write configuration on read-only Container");
		}
		datadbt.set(0, 1); // reserve space
		*(uint8_t*)datadbt.get_data() = indexNodes ? 1 : 0;
		err = database_.put(txn, &keydbt, &datadbt, 0);

		if (err)
			throw XmlException(XmlException::DATABASE_ERROR,
			  "Unexpected error from DB writing configuration");

	} else {
		throw XmlException(XmlException::DATABASE_ERROR,
			  "Unexpected error from DB reading configuration");
	}
	return indexNodes;
}

int ConfigurationDatabase::checkVersion(Transaction *txn, bool rdonly)
{
	int err= 0;
	//
	// Get/Put the version number into the configuration database.
	//
	unsigned int saved_version= 0;
	err = getVersion(txn, saved_version);
	if (err == DB_NOTFOUND) {
		if (!rdonly)
			err = putVersion(txn, database_, Container::version);
	} else {
		if (Container::version != saved_version) {
			ostringstream s;
			s << "Container version '";
			s << saved_version;
			s << "' does not match the current dbxml library version '";
			s << Container::version;
			s << "'.";
			if((Container::version > saved_version)) {
				if (saved_version == VERSION_121)
					s << "Upgrade is not supported from release 1.2.x to release 2.x.";
				else
					s << "Upgrade is required. Run XmlManager::upgradeContainer()";
			} else {
				s << " Use a more recent release of the dbxml library to open this container.";
			}
			throw XmlException(XmlException::VERSION_MISMATCH, s.str());
		}
	}
	return err;
}

int ConfigurationDatabase::getVersion(Transaction *txn, unsigned int &version) const
{
	// Get version number from configuration database
	//
	DbtIn keydbt((void *)"version", strlen("version") + 1);
	DbtOut datadbt;
	int err = database_.get(txn, &keydbt, &datadbt, 0);
	version =
		(err!=0 ? 0 : DbXml::stringToInt((char *)datadbt.get_data()));
	return err;
}

int ConfigurationDatabase::getVersions(
	Transaction *txn, unsigned int &current_version,
	unsigned int &saved_version) const
{
	current_version = Container::version;
	return getVersion(txn, saved_version);
}

int ConfigurationDatabase::checkIndexSpecification(Transaction *txn)
{
	static const char *key = "index";
	static size_t keyLength = strlen(key) + 1;

	Buffer b;
	int err = getConfigurationItem(txn, key, keyLength, b, /*lock=*/false);
	if(err == DB_NOTFOUND) {
		// Write the default index specification
		err = IndexSpecification().write(this, txn);
	}

	return err;
}

int ConfigurationDatabase::getIndexSpecification(
	Transaction *txn, IndexSpecification &index, bool lock) const
{
	return index.read(this, txn, lock);
}

int ConfigurationDatabase::putIndexSpecification(
	Transaction *txn, const IndexSpecification &index)
{
	return index.write(this, txn);
}

void ConfigurationDatabase::setIndexNodes(Transaction *txn, bool indexNodes)
{
	static const char *indexNodesKey = "indexNodes";
	DbtIn keydbt((void *) indexNodesKey, strlen(indexNodesKey) + 1); // include null in key
	DbtOut datadbt(0, 1);
	*(uint8_t*)datadbt.get_data() = indexNodes ? 1 : 0;
	int err = database_.put(txn, &keydbt, &datadbt, 0);
	if (err)
		throw XmlException(
			XmlException::DATABASE_ERROR,
			"Unexpected error from DB writing configuration");
}

//static
XmlContainer::ContainerType
ConfigurationDatabase::readContainerType(DbEnv *env,
					 const std::string &name)
{
	DbWrapper database(env, name, "secondary_",
			   configuration_name, 0, 0);
	int err = database.open(0, DB_BTREE, 0, 0);
	if (err != 0)
		throw XmlException(XmlException::DATABASE_ERROR,
				   "Unexpected error opening Configuration DB");
	// type is ignored -- it's initialized to suppress warnings
	XmlContainer::ContainerType type = XmlContainer::WholedocContainer;
	return checkContainerType(0, database, type, true);
}

int ConfigurationDatabase::dump(
	DbEnv *env, const std::string &name, std::ostream *out)
{
	DbWrapper  database(env, name, "secondary_", configuration_name, 0, 0);
	DbWrapper seqDatabase(env, name, "secondary_", sequence_name, 0, 0);

	int err = Container::writeHeader(database.getDatabaseName(), out);
	if(err == 0) err = database.dump(out);

	// sequence DB
	if (err == 0) {
		err = Container::writeHeader(seqDatabase.getDatabaseName(), out);
		if(err == 0) err = seqDatabase.dump(out);
	}

	return err;
}

int ConfigurationDatabase::load(
	DbEnv *env, const std::string &name,
	std::istream *in, unsigned long *lineno)
{
	DbWrapper  database(env, name, "secondary_", configuration_name, 0, 0);
	DbWrapper seqDatabase(env, name, "secondary_", sequence_name, 0, 0);

	int err = Container::verifyHeader(database.getDatabaseName(), in);
	if(err != 0) {
		ostringstream oss;
		oss << "ConfigurationDatabase::load() invalid database dump file loading '" << name << "'";
		Log::log(env, Log::C_DICTIONARY, Log::L_ERROR, oss.str().c_str());
	} else {
		err = database.load(in, lineno);
	}

	// sequence DB
	if (err == 0) {
		err = Container::verifyHeader(seqDatabase.getDatabaseName(), in);
		if(err != 0) {
			ostringstream oss;
			oss << "ConfigurationDatabase::load() (sequenceDb) invalid database dump file loading '" << name << "'";
			Log::log(env, Log::C_DICTIONARY, Log::L_ERROR,
				 oss.str().c_str());
		} else {
			err = seqDatabase.load(in, lineno);
		}
	}

	return err;
}

int ConfigurationDatabase::verify(
	DbEnv *env, const std::string &name, std::ostream *out,
	u_int32_t flags)
{
	DbWrapper  database(env, name, "secondary_", configuration_name, 0, 0);
	DbWrapper seqDatabase(env, name, "secondary_", sequence_name, 0, 0);

	int err = 0;
	if(flags & DB_SALVAGE)
		err = Container::writeHeader(database.getDatabaseName(), out);
	if(err == 0)
		err = database.verify(out, flags);

	// sequence DB
	if (err == 0) {
		if(flags & DB_SALVAGE)
			err = Container::writeHeader(seqDatabase.getDatabaseName(),
						     out);
		if(err == 0)
			err = seqDatabase.verify(out, flags);
	}

	return err;
}

// static
int ConfigurationDatabase::putVersion(Transaction *txn,
				      DbWrapper &db,
				      unsigned int version)
{
	// Put version number into configuration database
	// key = "version\0", data = int (ASCII-encoded int)
	//
	DbtIn keydbt((void *)"version", strlen("version") + 1);
	DbtOut datadbt;
	string s(DbXml::toString(version));
	datadbt.set((void*)s.c_str(), s.length() + 1);
	return db.put(txn, &keydbt, &datadbt, 0);
}

// static
void ConfigurationDatabase::upgrade(const std::string &name, Manager &mgr,
				    int old_version, int current_version)
{
	DBXML_ASSERT(old_version < current_version);
	// do upgrade to 2.2 if necessary, no upgrade from 2.2 to 2.3
	if (current_version >= VERSION_22) {
		// if (pre-2.2)
		//   o  create sequence database, and move
		//   o move sequence record from configuration db
		// o upgrade version to current_version
		int err;
		DbWrapper database(mgr.getDbEnv(), name, "secondary_",
			configuration_name, 0, 0);
		err = database.open(0, DB_BTREE, 0, 0);
		if (old_version < VERSION_22) {
			// need to upgrade to 2.2 format, with a separate
			// sequence database
			DbWrapper seqDatabase(mgr.getDbEnv(), name, "secondary_",
				sequence_name, database.getPageSize(), 0);
			if (err == 0)
				err = seqDatabase.open(0, DB_BTREE,
						       DB_CREATE|DB_EXCL, 0);
			if (err == 0) {
				Dbt seqKey((void*)"dbxmlID", 7);
				DbtOut datadbt;
				err = database.get(0, &seqKey, &datadbt, 0);
				if (err == 0) {
					err = seqDatabase.put(0, &seqKey,
							      &datadbt, 0);
					if (err == 0)
						err = database.del(0, &seqKey, 0);
				}
			}
		}

		// Upgrade the index specification
		if(err == 0) {
			IndexSpecification is;
			err = is.upgrade(database, old_version, current_version);
		}
		if (err == 0)
			err = putVersion(0, database, current_version);
		if (err != 0)
			throw XmlException(XmlException::DATABASE_ERROR,
				   "Unexpected error upgrading Configuration DB");
	}
}
