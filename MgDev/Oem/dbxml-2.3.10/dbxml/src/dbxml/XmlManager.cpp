//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlManager.cpp,v 1.43.2.1 2006/12/13 16:11:59 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlManager.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "TransactedContainer.hpp"
#include "UTF8.hpp"
#include "Manager.hpp"
#include "LocalFileInputStream.hpp"
#include "MemBufInputStream.hpp"
#include "URLInputStream.hpp"
#include "QueryExpression.hpp"
#include "StdInInputStream.hpp"
#include "Transaction.hpp"
#include "Document.hpp"
#include "QueryContext.hpp"
#include "Modify.hpp"
#include "IndexLookup.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "UpdateContext.hpp"
#include "Results.hpp"
#include "dataItem/DbXmlURIResolver.hpp"

#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/exceptions/XQillaException.hpp>

#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

#ifdef DEBUG
static void dbFlagsCollision(const char *flag)
{
	std::string msg = "Collision with DB flags: " + (std::string)flag;
//	throw XmlException(XmlException::INTERNAL_ERROR, msg);
	std::cout << msg << std::endl;
}

#endif

XmlManager::XmlManager(DbEnv *dbEnv, u_int32_t flags)
	: impl_(new Manager(dbEnv, flags))
{
#ifdef DEBUG
	int ncollisions = 0;
	// Do sanity check on potential DB flags collisions.
	// This will only ever fail if DB flags are changed
	// in a way that collides with DBXML_ flags
	// New DBXML_ and DB_ flags should be added here as appropriate
	int dbTxnFlags = DB_READ_COMMITTED|DB_READ_UNCOMMITTED|DB_MULTIVERSION|DB_TXN_SNAPSHOT|DB_TXN_SYNC|DB_TXN_NOWAIT|DB_RMW;
	int dbDbFlags = DB_XA_CREATE| DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|DB_CREATE|DB_EXCL|DB_RDONLY|DB_TXN_NOT_DURABLE|DB_MULTIVERSION|DB_TRUNCATE;
	int dbXmlFlags = DBXML_TRANSACTIONAL|DBXML_ALLOW_VALIDATION|DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES|DBXML_CHKSUM|DBXML_ENCRYPT;
	if ((DBXML_LAZY_DOCS & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_LAZY_DOCS");
		++ncollisions;
	}
	if ((DBXML_REVERSE_ORDER & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_REVERSE_ORDER");
		++ncollisions;
	}
	if ((DBXML_INDEX_VALUES & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_INDEX_VALUES");
		++ncollisions;
	}
	if ((DBXML_NO_INDEX_NODES & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_NO_INDEX_NODES");
		++ncollisions;
	}
	if ((DBXML_CACHE_DOCUMENTS & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_CACHE_DOCUMENTS");
		++ncollisions;
	}
	if ((DBXML_WELL_FORMED_ONLY & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_WELL_FORMED_ONLY");
		++ncollisions;
	}
	if ((dbDbFlags & dbXmlFlags) != 0) {
		dbFlagsCollision("MISC_FLAGS");
		++ncollisions;
	}
	if (ncollisions)
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "One or more collisions with DB flags");
#endif
	impl_->acquire();
}

XmlManager::XmlManager(u_int32_t flags)
	: impl_(new Manager(flags))
{
	impl_->acquire();
}

XmlManager::XmlManager()
	: impl_(new Manager((u_int32_t)0))
{
	impl_->acquire();
}

XmlManager::~XmlManager()
{
	impl_->release();
}

XmlManager::XmlManager(Manager &db)
	: impl_(&db)
{
	impl_->acquire();
}

XmlManager::XmlManager(const XmlManager &o)
	: impl_(o.impl_)
{
	impl_->acquire();
}

XmlManager &XmlManager::operator=(const XmlManager &o)
{
	if(impl_ != o.impl_) {
		impl_->release();
		impl_ = o.impl_;
		impl_->acquire();
	}
	return *this;
}

DbEnv *XmlManager::getDbEnv()
{
	return impl_->getDbEnv();
}

const std::string &XmlManager::getHome() const
{
	return impl_->getHome();
}

void XmlManager::setDefaultContainerFlags(u_int32_t flags)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "setDefaultContainerFlags()",
		flags, DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DB_CREATE|DB_EXCL|
		DB_RDONLY|DBXML_ALLOW_VALIDATION|DB_TXN_NOT_DURABLE|
		DB_MULTIVERSION);
	impl_->setDefaultContainerFlags(flags);
}

u_int32_t XmlManager::getDefaultContainerFlags()
{
	return impl_->getDefaultContainerFlags();
}

u_int32_t XmlManager::getFlags() const
{
	return impl_->getFlags();
}

void XmlManager::setDefaultPageSize(u_int32_t pageSize)
{
	impl_->setDefaultPageSize(pageSize);
}

u_int32_t XmlManager::getDefaultPageSize()
{
	return impl_->getDefaultPageSize();
}

void XmlManager::setDefaultSequenceIncrement(u_int32_t incr)
{
	impl_->setDefaultSequenceIncrement(incr);
}

u_int32_t XmlManager::getDefaultSequenceIncrement()
{
	return impl_->getDefaultSequenceIncrement();
}

void XmlManager::setDefaultContainerType(XmlContainer::ContainerType type)
{
	impl_->setDefaultContainerType(type);
}

XmlContainer::ContainerType XmlManager::getDefaultContainerType()
{
	return impl_->getDefaultContainerType();
}

void XmlManager::registerResolver(const XmlResolver &resolver)
{
	impl_->getResolverStore().registerResolver(resolver);
}

int XmlManager::getImplicitTimezone() const
{
	// Convert from seconds based to minutes based
	return impl_->getImplicitTimezone() / 60;
}

void XmlManager::setImplicitTimezone(int tz)
{
	// Convert from minutes based to seconds based
	impl_->setImplicitTimezone(tz * 60);
}

int XmlManager::existsContainer(const std::string &name)
{
	return Container::checkContainer(name, *getDbEnv());
}

XmlContainer XmlManager::createContainer(const std::string &name)
{
	return impl_->openContainer(
		name, 0,
		impl_->getDefaultContainerFlags() | DB_CREATE | DB_EXCL,
		impl_->getDefaultContainerType(), 0);
}

XmlContainer XmlManager::createContainer(
	const std::string &name, u_int32_t flags,
	XmlContainer::ContainerType type, int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "createContainer()",
		flags, DB_XA_CREATE|DB_CREATE|DBXML_NO_INDEX_NODES|
		DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE|DB_EXCL|DB_MULTIVERSION);
	return impl_->openContainer(name, 0, flags | DB_CREATE | DB_EXCL,
				    type, mode);
}

XmlContainer XmlManager::createContainer(
	XmlTransaction &txn, const std::string &name)
{
	return impl_->openContainer(name, txn, impl_->getDefaultContainerFlags() |
				    DB_CREATE | DB_EXCL,
				    impl_->getDefaultContainerType(), 0);
}

XmlContainer XmlManager::createContainer(
	XmlTransaction &txn, const std::string &name,
	u_int32_t flags, XmlContainer::ContainerType type, int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "createContainer()",
		flags, DB_CREATE|DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE|DB_EXCL|DB_MULTIVERSION);
	
	return impl_->openContainer(name, txn, flags | DB_CREATE | DB_EXCL,
				    type, mode);
}

XmlInputStream *XmlManager::createStdInInputStream() const {
	return new StdInInputStream();
}

XmlInputStream *XmlManager::createLocalFileInputStream(const std::string &filename) const {
	XmlInputStream *newStream = 0;
	try {
		newStream = new LocalFileInputStream(filename);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create FileInputStream for file: ";
		s << filename;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

XmlInputStream *XmlManager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	const char *const bufId,
	const bool adoptBuffer) const {
	XmlInputStream *newStream = 0;
	try {
		newStream = new MemBufInputStream(srcDocBytes,
						  byteCount, bufId,
						  adoptBuffer);
	}
	catch (...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Failed to create MemBufInputStream");
	}
	return newStream;
}

// always adopts buffer
XmlInputStream *XmlManager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	const bool copyBuffer) const {
	XmlInputStream *newStream = 0;
	try {
		const char *buf = srcDocBytes;
		if (copyBuffer) {
			char *newBytes = new char[byteCount];
			if (!newBytes) {
				throw XmlException(
					XmlException::NO_MEMORY_ERROR,
					"Failed to allocate memory");
			}
			memcpy(newBytes, srcDocBytes, byteCount);
			buf = (const char *)newBytes;
		}
		newStream = new MemBufInputStream(buf,
						  byteCount, "",
						  (buf != srcDocBytes));
	}
	catch (...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Failed to create MemBufInputStream");
	}
	return newStream;
}

XmlInputStream *XmlManager::createURLInputStream(const std::string &baseId, 
						 const std::string &systemId, 
						 const std::string &publicId) const
{
	XmlInputStream *newStream = 0;
	try {
		newStream = new URLInputStream(baseId, systemId, publicId);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create URLInputStream, baseId: ";
		s << baseId;
		s << ", systemId: ";
		s << systemId;
		s << ", publicId: ";
		s << publicId;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

XmlInputStream *XmlManager::createURLInputStream(const std::string &baseId, 
						 const std::string &systemId) const
{
	XmlInputStream *newStream = 0;
	try {
		newStream = new URLInputStream(baseId, systemId);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create URLInputStream, baseId: ";
		s << baseId;
		s << ", systemId: ";
		s << systemId;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

XmlContainer XmlManager::openContainer(const std::string &name)
{
	return impl_->openContainer(name, 0, impl_->getDefaultContainerFlags(),
				    impl_->getDefaultContainerType(), 0);
}

XmlContainer XmlManager::openContainer(const std::string &name, u_int32_t flags)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, 0, flags,
				    impl_->getDefaultContainerType(), 0);
}

XmlContainer XmlManager::openContainer(const std::string &name,
				       u_int32_t flags,
				       XmlContainer::ContainerType type,
				       int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, 0, flags, type, mode);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn,
				       const std::string &name,
				       u_int32_t flags,
				       XmlContainer::ContainerType type,
				       int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, txn, flags, type, mode);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn, const std::string &name)
{
	return impl_->openContainer(name, txn,
				    impl_->getDefaultContainerFlags(),
				    impl_->getDefaultContainerType(), 0);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn, const std::string &name, u_int32_t flags)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DB_XA_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, txn, flags,
				    impl_->getDefaultContainerType(), 0);
}

XmlDocument XmlManager::createDocument()
{
	return new Document(*this);
}

static void checkOpenContainer(Manager &impl, const std::string &name,
			       const char *method)
{
	// getOpenContainer() call has no side effects
	TransactedContainer *c = impl.getOpenContainer(name);
	if (c) {
		ostringstream s;
		s << "XmlManager::";
		s << method;
		s << "(): requires a closed container";
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
}

void XmlManager::removeContainer(const string &name)
{
	checkOpenContainer(*impl_, name, "removeContainer");
	impl_->removeContainer(0, name);
}

void XmlManager::renameContainer(const string &oldName,
				 const string &newName)
{
	checkOpenContainer(*impl_, oldName, "renameContainer");
	checkOpenContainer(*impl_, newName, "renameContainer");
	impl_->renameContainer(0, oldName, newName);
}

void XmlManager::dumpContainer(const string &name, ostream *out)
{
	checkOpenContainer(*impl_, name, "dumpContainer");
	Container::dump(*impl_, name, out);
}

void XmlManager::loadContainer(const string &name, istream *in,
			       unsigned long *lineno,
			       XmlUpdateContext &context)
{
	Container::load(*impl_, name, in, lineno, context);
}

void XmlManager::verifyContainer(const string &name, ostream *out,
				 u_int32_t flags)
{
	impl_->checkFlags(Log::misc_flag_info, "verifyContainer()", flags,
			  DB_SALVAGE|DB_AGGRESSIVE);
	checkOpenContainer(*impl_, name, "verifyContainer");
	Container::verify(*impl_, name, out, flags);
}

void XmlManager::upgradeContainer(const string &name,
				  XmlUpdateContext &context)
{
	checkOpenContainer(*impl_, name, "upgradeContainer");
	Container::upgradeContainer(name, *impl_, context);
}

void XmlManager::removeContainer(XmlTransaction &txn, const string &name)
{
	checkOpenContainer(*impl_, name, "removeContainer");
	impl_->removeContainer(txn, name);
}

void XmlManager::renameContainer(XmlTransaction &txn,
				 const string &oldName,
				 const string &newName)
{
	checkOpenContainer(*impl_, oldName, "renameContainer");
	checkOpenContainer(*impl_, newName, "renameContainer");
	impl_->renameContainer(txn, oldName, newName);
}

void XmlManager::reindexContainer(const std::string &name,
				  XmlUpdateContext &context,
				  u_int32_t flags)
{
	checkOpenContainer(*impl_, name, "reindexContainer");
	Container::reindexContainer(0, *impl_, name, context, flags);
}

void XmlManager::reindexContainer(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context,
				  u_int32_t flags)
{
	checkOpenContainer(*impl_, name, "reindexContainer");
	Container::reindexContainer(txn, *impl_, name, context, flags);
}

void XmlManager::truncateContainer(const std::string &name,
				   XmlUpdateContext &context,
				   u_int32_t /* flags */)
{
	checkOpenContainer(*impl_, name, "truncateContainer");
	Container::truncateContainer(0, *impl_, name, context);
}

void XmlManager::truncateContainer(XmlTransaction &txn,
				   const std::string &name,
				   XmlUpdateContext &context,
				   u_int32_t /* flags */)
{
	checkOpenContainer(*impl_, name, "truncateContainer");
	Container::truncateContainer(txn, *impl_, name, context);
}

void XmlManager::compactContainer(const std::string &name,
				  XmlUpdateContext &context,
				  u_int32_t /* flags */)
{
	// While compaction is supported as of 4.4, there's a bug
	// that causes it to fail for BDB XML containers, so require
	// 4.5 for compaction
#if DBVER > 44
	checkOpenContainer(*impl_, name, "compactContainer");
	Container::compactContainer(0, *impl_, name, context);
#else
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlManager::compactContainer is not supported in Berkeley DB versions before 4.5");
#endif
}

void XmlManager::compactContainer(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context,
				  u_int32_t /* flags */)
{
#if DBVER > 44
	checkOpenContainer(*impl_, name, "compactContainer");
	Container::compactContainer(txn, *impl_, name, context);
#else
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlManager::compactContainer is not supported in Berkeley DB versions before 4.5");
#endif
}

XmlQueryContext XmlManager::createQueryContext(XmlQueryContext::ReturnType rt,
					       XmlQueryContext::EvaluationType et)
{
	return new QueryContext(*this, rt, et);
}

XmlUpdateContext XmlManager::createUpdateContext()
{
	return new UpdateContext(*this);
}

XmlQueryExpression XmlManager::prepare(const std::string &xquery,
				       XmlQueryContext &context)
{
	return new QueryExpression(xquery, context, 0);
}

XmlQueryExpression XmlManager::prepare(XmlTransaction &txn,
				       const std::string &xquery,
				       XmlQueryContext &context)
{
	return new QueryExpression(xquery, context, txn);
}

XmlResults XmlManager::query(const std::string &xquery,
			     XmlQueryContext &context, u_int32_t flags)
{
	// flags are checked in execute
	return prepare(xquery, context).execute(context, flags);
}

XmlResults XmlManager::query(XmlTransaction &txn, const std::string &xquery,
			     XmlQueryContext &context, u_int32_t flags)
{
	// flags are checked in execute
	return prepare(txn, xquery, context).execute(txn, context, flags);
}

XmlResults XmlManager::createResults()
{
	return new ValueResults();
}

XmlModify XmlManager::createModify()
{
	return new Modify(*this);
}

XmlTransaction XmlManager::createTransaction(DbTxn *toUse)
{
	if (!toUse)
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlManager::createTransaction(DbTxn*) requires a valid DbTxn object");

	if (impl_->isTransactedEnv() || impl_->isCDBEnv())
		return new Transaction(*this, toUse);
	
	throw XmlException(XmlException::INVALID_VALUE,
			   "Cannot call XmlManager::createTransaction when transactions are not initialized");
}

XmlTransaction XmlManager::createTransaction(u_int32_t flags)
{
	impl_->checkFlags(Log::misc_flag_info, "createTransaction()", flags,
			  DB_READ_UNCOMMITTED|DB_TXN_NOSYNC|DB_TXN_NOWAIT|
			  DB_TXN_SYNC|DB_READ_COMMITTED|DB_TXN_SNAPSHOT);
	if (impl_->isTransactedEnv())
		return new Transaction(*this, flags);
	throw XmlException(XmlException::INVALID_VALUE,
			   "Cannot call XmlManager::createTransaction when transactions are not initialized");
	
}

XmlIndexLookup XmlManager::createIndexLookup(
	XmlContainer &cont, const std::string &uri, const std::string &name,
	const std::string &index, const XmlValue &value,
	XmlIndexLookup::Operation op)
{
	return XmlIndexLookup(new IndexLookup(cont, uri, name, index,
					      value, op));
}


DBXML_EXPORT const char *DbXml::dbxml_version(int *majorp, int *minorp, 
					      int *patchp)
{
	if (majorp != NULL)
		*majorp = DBXML_VERSION_MAJOR;
	if (minorp != NULL)
		*minorp = DBXML_VERSION_MINOR;
	if (patchp != NULL)
		*patchp = DBXML_VERSION_PATCH;
	return DBXML_VERSION_STRING;
}

DBXML_EXPORT const char *DbXml::dbxml_db_version(int *majorp, int *minorp, 
						 int *patchp)
{
	return DbEnv::version(majorp, minorp, patchp);
}

DBXML_EXPORT void DbXml::setLogLevel(LogLevel level, bool enabled)
{
	Log::setLogLevel((Log::ImplLogLevel)level, enabled);
}

DBXML_EXPORT void DbXml::setLogCategory(LogCategory category, bool enabled)
{
	Log::setLogCategory((Log::ImplLogCategory)category, enabled);
}
