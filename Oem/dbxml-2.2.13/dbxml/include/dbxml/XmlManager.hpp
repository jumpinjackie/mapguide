//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlManager.hpp,v 1.21 2005/10/10 18:24:20 gmf Exp $
//

#ifndef __XMLMANAGER_HPP
#define	__XMLMANAGER_HPP

#include <string>

#include "DbXmlFwd.hpp"
#include "XmlContainer.hpp"
#include "XmlQueryContext.hpp"
#include "XmlIndexLookup.hpp"
#include "db_cxx.h"

namespace DbXml
{

class Manager;

class DBXML_EXPORT XmlManager
{
public:
	/** @name Public Constructors */
	/**
	 * It's assumed that the DbEnv is already open. If adopted, the DbEnv
	 * will be both closed and deleted by the XmlManager when it is
	 * destroyed.  The DbEnv is adopted if flags include DBXML_ADOPT_DBENV.
	 *
	 * Valid flags: DBXML_ADOPT_DBENV, DBXML_ALLOW_EXTERNAL_ACCESS,
	 *  DBXML_ALLOW_AUTO_OPEN
	 */
	XmlManager(DbEnv *dbEnv, u_int32_t flags = 0);
	/**
	 * Valid flags: DBXML_ALLOW_EXTERNAL_ACCESS, DBXML_ALLOW_AUTO_OPEN
	 */
	XmlManager(u_int32_t flags);
	/**
	 * Default state: no external access, no auto open
	 */
	XmlManager();

	/** @name For Reference Counting */
	XmlManager(const XmlManager &o);
	XmlManager &operator=(const XmlManager &o);
	~XmlManager();
	
	/** @name XmlManager Configuration for XmlContainer construction */
	/**
	 * Flags for containers:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_TRANSACTIONAL, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	void setDefaultContainerFlags(u_int32_t flags);
	u_int32_t getDefaultContainerFlags();
	// Berkeley DB page size for containers
	void setDefaultPageSize(u_int32_t pageSize);
	u_int32_t getDefaultPageSize();
	// Berkeley DB unique number increment
	void setDefaultSequenceIncrement(u_int32_t incr);
	u_int32_t getDefaultSequenceIncrement();
	// Container type -- defaults to XmlContainer::WholedocContainer
	void setDefaultContainerType(XmlContainer::ContainerType type);
	XmlContainer::ContainerType getDefaultContainerType();

	/** @name For resolving external entities */
	void registerResolver(const XmlResolver &resolver);
	
	/** @name Container Methods */

	// returns 0 if not a container; otherwise returns format version
	int existsContainer(const std::string &name);
	
	/// Uses the default flags and ContainerType, implies DB_CREATE
	XmlContainer createContainer(const std::string &name);
	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_TRANSACTIONAL, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer createContainer(
		const std::string &name, u_int32_t flags,
		XmlContainer::ContainerType type,
		int mode = 0);
	/// Uses the default flags
	XmlContainer openContainer(const std::string &name);
	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_TRANSACTIONAL, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer openContainer(const std::string &name, u_int32_t flags);

	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer openContainer(const std::string &name,
				   u_int32_t flags,
				   XmlContainer::ContainerType type,
				   int mode = 0);
	
	
	/** @name Factory methods for XmlInputStream classes */
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createStdInInputStream() const;
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createLocalFileInputStream(
		const std::string &filename) const;
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		const char *const bufId,
		const bool adoptBuffer = false) const;

	/// XmlInputStream must be deleted by user.
	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		const bool copyBuffer) const;
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId, 
					    const std::string &publicId) const;
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId) const;
	
	
	/** @name Container Methods (Transacted) */
	
	/// Uses the default flags, implies DB_CREATE
	XmlContainer createContainer(XmlTransaction &txn,
				     const std::string &name);
	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer createContainer(XmlTransaction &txn,
				     const std::string &name, u_int32_t flags,
				     XmlContainer::ContainerType type,
				     int mode = 0);
	/// Uses the default flags
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name);
	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name,
				   u_int32_t flags);

	/**
	 * Valid flags:
	 * DB_XA_CREATE, DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DB_DIRTY_READ
	 * DB_NOMMAP, DB_THREAD, DBXML_CHKSUM, DBXML_ENCRYPT, DB_CREATE,
	 * DB_EXCL, DB_RDONLY, DBXML_ALLOW_VALIDATION,
	 * DB_TXN_NOT_DURABLE
	 */
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name,
				   u_int32_t flags,
				   XmlContainer::ContainerType type,
				   int mode = 0);
	
	/** @name Container Administration Methods */

	void removeContainer(const std::string &name);
	void renameContainer(const std::string &oldName,
			     const std::string &newName);
	void dumpContainer(const std::string &name,
			   std::ostream *out);
	void loadContainer(const std::string &name,
			   std::istream *in, unsigned long *lineno,
			   XmlUpdateContext &context);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES
	 */
	void reindexContainer(const std::string &name,
			      XmlUpdateContext &context,
			      u_int32_t flags = 0);
	
	/**
	 * Valid flags:
	 * DB_SALVAGE, DB_AGGRESSIVE
	 */
	void verifyContainer(const std::string &name,
			     std::ostream *out, u_int32_t flags = 0);
	void upgradeContainer(const std::string &name,
			      XmlUpdateContext &context);

	/** @name Container Administration Methods (Transacted) */

	void removeContainer(XmlTransaction &txn, const std::string &name);
	void renameContainer(XmlTransaction &txn, const std::string &oldName,
			     const std::string &newName);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES
	 */
	void reindexContainer(XmlTransaction &txn,
			      const std::string &name,
			      XmlUpdateContext &context,
			      u_int32_t flags = 0);

	/** @name Document Methods */
	
	XmlDocument createDocument();
	
	/** @name Context Methods */
	
	XmlQueryContext createQueryContext(
		XmlQueryContext::ReturnType rt =
		   XmlQueryContext::LiveValues,
		XmlQueryContext::EvaluationType et = XmlQueryContext::Eager);
	XmlUpdateContext createUpdateContext();

	/** @name Query Expression Methods */

	XmlQueryExpression prepare(const std::string &queryString,
				   XmlQueryContext &context);
	XmlQueryExpression prepare(XmlTransaction &txn,
				   const std::string &queryString,
				   XmlQueryContext &context);
	/**
	 * Valid flags:
	 * DB_DIRTY_READ, DB_RMW, DB_DEGREE_2, DBXML_LAZY_DOCS
	 */
	XmlResults query(const std::string &queryString,
			 XmlQueryContext &context, u_int32_t flags = 0);
	/**
	 * Valid flags:
	 * DB_DIRTY_READ, DB_RMW, DB_DEGREE_2, DBXML_LAZY_DOCS
	 */
	XmlResults query(XmlTransaction &txn, const std::string &queryString,
			 XmlQueryContext &context, u_int32_t flags = 0);

	/** @name Results Methods */
	XmlResults createResults();
	
	/** @name Modify Methods */
	
	XmlModify createModify();
	
	/** @name Transaction Methods */
	
	XmlTransaction createTransaction(DbTxn *toUse);
	/**
	 * Valid flags:
	 * DB_DIRTY_READ, DB_TXN_NOSYNC, DB_TXN_NOWAIT, DB_DEGREE_2, DB_TXN_SYNC
	 */
	XmlTransaction createTransaction(u_int32_t flags = 0);

	/** @name Direct Index Lookup */

	XmlIndexLookup createIndexLookup(
		XmlContainer &container,
		const std::string &uri, const std::string &name,
		const std::string &index,
		const XmlValue &value = XmlValue(),
		XmlIndexLookup::Operation op = XmlIndexLookup::EQ);
	
	/** @name Berkeley DB Compatibility */
	
	DbEnv *getDbEnv();
	const std::string &getHome() const;
	
	
	/** @name Private Methods (for internal use) */
	// @{

	XmlManager(Manager &db);
	operator Manager &() {
		return *impl_;
	}
	operator const Manager &() const {
		return *impl_;
	}
	operator Manager *() {
		return impl_;
	}
	
	//@}

private:
	Manager *impl_;
};

}

#endif
