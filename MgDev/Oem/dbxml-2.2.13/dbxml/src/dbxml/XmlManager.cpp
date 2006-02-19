//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlManager.cpp,v 1.33 2005/10/12 12:20:17 jsnelson Exp $
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

#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/PathanException.hpp>

#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

XmlManager::XmlManager(DbEnv *dbEnv, u_int32_t flags)
	: impl_(new Manager(dbEnv, flags))
{
#ifdef DEBUG
	// Do sanity check on potential DB flags collisions.
	// This will only ever fail if DB flags are changed
	// in a way that collides with DBXML_ flags
	// New DBXML_ flags should be added here as appropriate
	int dbTxnFlags = DB_DIRTY_READ|DB_RMW|DB_DEGREE_2;
	int dbDbFlags = DB_XA_CREATE| DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|DB_CREATE|DB_EXCL|DB_RDONLY|DB_TXN_NOT_DURABLE;
	int dbXmlFlags = DBXML_TRANSACTIONAL|DBXML_ALLOW_VALIDATION|DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES|DBXML_CHKSUM|DBXML_ENCRYPT;
	if ((DBXML_LAZY_DOCS & dbTxnFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML_LAZY_DOCS collision with DB flags");
	}
	if ((DBXML_REVERSE_ORDER & dbTxnFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML_REVERSE_ORDER collision with DB flags");
	}
	if ((DBXML_INDEX_VALUES & dbTxnFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML_INDEX_VALUES collision with DB flags");
	}
	if ((DBXML_NO_INDEX_NODES & dbTxnFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML_NO_INDEX_NODES collision with DB flags");
	}
	if ((DBXML_CACHE_DOCUMENTS & dbTxnFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML_CACHE_DOCUMENTS collision with DB flags");
	}
	if ((dbDbFlags & dbXmlFlags) != 0) {
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "DBXML container flags collision with DB flags");
	}
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DB_CREATE|DB_EXCL|
		DB_RDONLY|DBXML_ALLOW_VALIDATION|DB_TXN_NOT_DURABLE);
	impl_->setDefaultContainerFlags(flags);
}

u_int32_t XmlManager::getDefaultContainerFlags()
{
	return impl_->getDefaultContainerFlags();
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE);
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE);
	
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE);
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE);
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE);
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
		DBXML_TRANSACTIONAL|DB_DIRTY_READ|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE);
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
	impl_->checkFlags(Log::misc_flag_info, "query()", flags,
		   DB_DIRTY_READ|DB_RMW|DB_DEGREE_2|DBXML_LAZY_DOCS);
	return prepare(xquery, context).execute(context, flags);
}

XmlResults XmlManager::query(XmlTransaction &txn, const std::string &xquery,
			     XmlQueryContext &context, u_int32_t flags)
{
	impl_->checkFlags(Log::misc_flag_info, "query()", flags,
		   DB_DIRTY_READ|DB_RMW|DB_DEGREE_2|DBXML_LAZY_DOCS);
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

	if (impl_->isTransactedEnv())
		return new Transaction(*this, toUse);
	
	throw XmlException(XmlException::INVALID_VALUE,
			   "Cannot call XmlManager::createTransaction when transactions are not initialized");

}

XmlTransaction XmlManager::createTransaction(u_int32_t flags)
{
	impl_->checkFlags(Log::misc_flag_info, "createTransaction()", flags,
			  DB_DIRTY_READ|DB_TXN_NOSYNC|DB_TXN_NOWAIT|
			  DB_TXN_SYNC|DB_DEGREE_2);
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

