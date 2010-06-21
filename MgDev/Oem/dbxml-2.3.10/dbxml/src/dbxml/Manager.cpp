//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Manager.cpp,v 1.38.2.1 2007/01/09 22:32:49 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "Manager.hpp"
#include "dbxml/XmlManager.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlException.hpp"
#include "Transaction.hpp"
#include "TransactedContainer.hpp"
#include "UTF8.hpp"
#include "Globals.hpp"
#include "SyntaxManager.hpp"
#include "dataItem/DbXmlURIResolver.hpp"
#include "DictionaryDatabase.hpp"

#include <xqilla/utils/ContextUtils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

// Default environment cache size to 50MB
// This beats DB's default of 256K, and should not
// consume excessive resource
static const u_int32_t db_cachesize_default = 50 * 1024 * 1024;

// SET_DB_ERRCALL is never defined.  This is for debugging only
#ifdef SET_DB_ERRCALL
static void errcall(const DbEnv *dbenv, const char *errpfx, const char *msg)
{
	cout << "message: " << (errpfx ? errpfx : "") << ": " << msg << endl;
}

#endif

static void throwContainerNotFound(const std::string &name)
{
	ostringstream oss;
	oss << "Container '" << name << "' does not exist.";
	throw XmlException(XmlException::CONTAINER_NOT_FOUND,
			   oss.str());
}

Manager::Manager(DbEnv *dbEnv, u_int32_t flags)
	: dbEnv_(dbEnv),
	  dbEnvAdopted_((flags & DBXML_ADOPT_DBENV)!=0),
	  dbEnvOpenFlags_(0),
	  flags_(flags),
	  autoOpen_((flags & DBXML_ALLOW_AUTO_OPEN)!=0),
	  defaultContainerFlags_(0),
	  defaultPageSize_(0),
	  defaultSequenceIncr_(5),
	  defaultContainerType_(XmlContainer::NodeContainer),
	  openContainers_(),
	  dictionary_(0)
{
	checkFlags(construct_manager_flag_info, "Manager()",
		   flags, DBXML_ADOPT_DBENV|
		   DBXML_ALLOW_EXTERNAL_ACCESS|DBXML_ALLOW_AUTO_OPEN);
	if(dbEnv_ == 0) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Null DbEnv pointer passed as parameter to XmlManager.");
	}
	const char *dbhome = 0;
	dbEnv_->get_home(&dbhome);
	if (dbhome && *dbhome)
		dbHome_ = dbhome;
	dbEnv_->get_open_flags(&dbEnvOpenFlags_);
	if (dbEnvOpenFlags_ & DB_AUTO_COMMIT) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"A DbEnv using DB_AUTO_COMMIT cannot be used to construct an XmlManager object.");
	}
	if (dbEnvOpenFlags_ & DB_INIT_CDB) {
		u_int32_t envFlags = 0;
		dbEnv_->get_flags(&envFlags);
		if (!(envFlags & DB_CDB_ALLDB)) {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"A DbEnv opened with DB_INIT_CDB must set the DB_CDB_ALLDB flag when used with Berkeley DB XML.");
		}
	}
#ifdef DB_SET_ERRCALL
	dbEnv_->set_errcall(errcall);
#endif
	resolvers_ = new ResolverStore();
	resolvers_->setSecure((flags & DBXML_ALLOW_EXTERNAL_ACCESS) == 0);
	initialize(dbEnv_);
	timezone_ = ContextUtils::getTimezone();
}

Manager::Manager(u_int32_t flags)
	: dbEnv_(new DbEnv(0)),
	  dbEnvAdopted_(true),
	  dbEnvOpenFlags_(0),
	  flags_(flags),
	  autoOpen_((flags & DBXML_ALLOW_AUTO_OPEN)!=0),
	  defaultContainerFlags_(0),
	  defaultPageSize_(0),
	  defaultSequenceIncr_(5),
	  defaultContainerType_(XmlContainer::NodeContainer),
	  openContainers_(),
	  dictionary_(0)
{
	checkFlags(construct_manager_flag_info, "Manager()",
		   flags, DBXML_ALLOW_EXTERNAL_ACCESS|DBXML_ALLOW_AUTO_OPEN);
	resolvers_ = new ResolverStore();
	resolvers_->setSecure((flags & DBXML_ALLOW_EXTERNAL_ACCESS) == 0);
	initialize(dbEnv_); // initializes Xerces and XQilla
	timezone_ = ContextUtils::getTimezone();
	// init DB defaults
	dbEnv_->set_cachesize(0, db_cachesize_default, 1);
	dbEnv_->set_errpfx("BDB XML");
	dbEnv_->set_error_stream(&cerr);
#ifdef DB_SET_ERRCALL
	dbEnv_->set_errcall(errcall);
#endif
	try {
		dbEnv_->open(0, DB_PRIVATE|DB_CREATE|DB_INIT_MPOOL, 0);
		dbEnv_->get_open_flags(&dbEnvOpenFlags_);
	}
	catch(DbException &e) {
		terminate();
		throw XmlException(e);
	}
	catch(std::exception &e) {
		terminate();
		std::string error("Error opening default database environment");
		throw XmlException(
			XmlException::INTERNAL_ERROR, error,
			__FILE__, __LINE__);
	}
}

Manager::~Manager()
{
	openContainers_.releaseRegisteredContainers();

	try {
		if(dbEnvAdopted_) {
			dbEnv_->close(0);
			delete dbEnv_;
		}
	}
	catch(...) {
		// Destructors don't throw.
	}
	terminate();
	delete resolvers_;
	if (dictionary_)
		delete dictionary_;
}

void Manager::initialize(DbEnv *env)
{
	// Initialize the global variables
	Globals::initialize(env);
}

void Manager::terminate()
{
	// terminate the global variables
	Globals::terminate();
}

TransactedContainer *Manager::openContainer(
	const std::string &name, Transaction *txn, u_int32_t flags,
	XmlContainer::ContainerType type,
	int mode, bool doVersionCheck)
{
	if(flags & ~(DB_XA_CREATE|DB_CREATE|DB_READ_UNCOMMITTED|
		     DB_EXCL|DB_NOMMAP|DB_RDONLY|DB_THREAD|DBXML_CHKSUM|
		     DBXML_ENCRYPT|DBXML_INDEX_NODES|DBXML_ALLOW_VALIDATION|
		     DBXML_TRANSACTIONAL|DB_MULTIVERSION)) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Invalid flags to method XmlManager::openContainer");
	}

	return openContainers_.findContainer(
		*this, name, txn, flags,
		type, defaultPageSize_, defaultSequenceIncr_, mode,
		doVersionCheck);
}

TransactedContainer *Manager::getOpenContainer(const std::string &name)
{
	return openContainers_.findOpenContainer(name);
}

void Manager::removeContainer(Transaction *txn, const std::string &name)
{
	int err = 0;
	try {
		err = dbEnv_->dbremove(Transaction::toDbTxn(txn), name.c_str(), 0, 0);
	}
	catch(DbException &e) {
		if (e.get_errno() == ENOENT)
			err = ENOENT;
		else
			throw XmlException(e);
	}
	if (err) {
		if (err == ENOENT)
			throwContainerNotFound(name);
		else
			throw XmlException(err);
	} else {
		ostringstream oss;
		oss << "Container '" << name << "' removed.";
		log(C_CONTAINER, L_DEBUG, oss);
	}
}

void Manager::renameContainer(Transaction *txn, const std::string &oldName,
			      const std::string &newName)
{
	int err = 0;
	try {
		err = dbEnv_->dbrename(Transaction::toDbTxn(txn),
				       oldName.c_str(), 0, newName.c_str(), 0);
	}
	catch(DbException &e) {
		if (e.get_errno() == ENOENT)
			err = ENOENT;
		else
			throw XmlException(e);
	}
	if (err) {
		if (err == ENOENT)
			throwContainerNotFound(oldName);
		else
			throw XmlException(err);
	} else {
		ostringstream oss;
		oss << "Container '" << oldName <<
			"' renamed to '" << newName << "'.";
		log(C_CONTAINER, L_DEBUG, oss);
	}
}

void Manager::checkFlags(const FlagInfo *flag_info, const char *function,
			 u_int32_t flags, u_int32_t mask) const
{
	Log::checkFlags(dbEnv_, C_MANAGER, 0, function,
			flag_info, flags, mask);
}

void Manager::log(ImplLogCategory c, ImplLogLevel l,
		   const ostringstream &s) const
{
	Log::log(dbEnv_, c, l, s.str().c_str());
}

void Manager::log(ImplLogCategory c, ImplLogLevel l, const string &s) const
{
	Log::log(dbEnv_, c, l, s.c_str());
}

Manager::ContainerStore::ContainerStore()
	: mutex_(XMLPlatformUtils::makeMutex(Globals::defaultMemoryManager))
{
}

Manager::ContainerStore::~ContainerStore()
{
	XMLPlatformUtils::closeMutex(mutex_);
}

int Manager::ContainerStore::closeContainer(TransactedContainer *container,
					     u_int32_t flags)
{
	int err = 0;
	try {
		MutexLock lock(mutex_);

		// remove name
		Map::iterator i = store_.find(container->getName());
		if(i != store_.end() && container == i->second) {
			store_.erase(i);
		}
	}
	catch (const XMLException &e) {
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			string("Error during Xerces-C mutex actions: ") +
			XMLChToUTF8(e.getMessage()).str(), __FILE__, __LINE__);
	}
	return err;
}

bool Manager::ContainerStore::addAlias(const std::string &alias,
				       TransactedContainer *container)
{
	MutexLock lock(mutex_);

	Map::iterator i = store_.find(alias);
	if(i == store_.end()) {
		store_[alias] = container;
		return true;
	}
	return false;
}

// only remove if the name matches the container
bool Manager::ContainerStore::removeAlias(const std::string &alias,
					  TransactedContainer *container)
{
	MutexLock lock(mutex_);

	Map::iterator i = store_.find(alias);
	if(i != store_.end() && container == i->second) {
		store_.erase(i);
		return true;
	}
	return false;
}

TransactedContainer *Manager::ContainerStore::findContainer(
	Manager &mgr, const std::string &name, Transaction *txn,
	u_int32_t flags, XmlContainer::ContainerType type,
	u_int32_t pagesize, u_int32_t seqIncr, int mode, bool doVersionCheck)
{
	try {
		MutexLock lock(mutex_);

		TransactedContainer *result = 0;
		Map::iterator i = store_.find(name);
		if(i != store_.end()) {
			// Found one already open
			result = i->second;
		} else {
			// Have to open one ourselves...
			result = new TransactedContainer(
				mgr, name, txn, flags,
				pagesize, seqIncr, mode, type, doVersionCheck);
			store_[result->getName()] = result;
			if (result && mgr.isCDBEnv())
				result->setUseCDB();
		}

		return result;
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
	catch (const XMLException &e) {
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			string("XMLException opening a container: ") +
			XMLChToUTF8(e.getMessage()).str(), __FILE__, __LINE__);
	}

	// Never happens
	return 0;
}

TransactedContainer *Manager::ContainerStore::findOpenContainer(
	const std::string &name)
{
	TransactedContainer *result = NULL;
	MutexLock lock(mutex_);

	Map::iterator i = store_.find(name);
	if(i != store_.end()) {
		// Found one already open
		result = i->second;
	}
	return result;
}

void Manager::ContainerStore::releaseRegisteredContainers()
{
	try {
		MutexLock lock(mutex_);
		store_.clear();
  }
	catch (const XMLException &e) {
		throw XmlException(
			XmlException::INTERNAL_ERROR,
			string("Error during Xerces-C mutex actions: ") +
			XMLChToUTF8(e.getMessage()).str(), __FILE__, __LINE__);
	}
}

DictionaryDatabase *Manager::getDictionary()
{
	lock();
	try {
		if (!dictionary_) {
			dictionary_ = new DictionaryDatabase(
				0, // no env
				0, // no txn
				"", // no name
				0, // default page size
				DB_THREAD|DB_CREATE, // thread-safe
				0 // default mode
				);
			// note: DictionaryDatabase will set its cache
			// sizes small (less than the 256k default) if
			// no DbEnv passed
		}
	} catch (...) {
		unlock();
		throw;
	}
	unlock();
	return dictionary_;
}
