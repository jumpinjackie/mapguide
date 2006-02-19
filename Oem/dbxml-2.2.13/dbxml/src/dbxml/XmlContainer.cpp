//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlContainer.cpp,v 1.130 2005/11/28 17:42:26 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlContainer.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "dbxml/XmlIndexSpecification.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlStatistics.hpp"
#include "dbxml/XmlIndexLookup.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "TransactedContainer.hpp"
#include "UpdateContext.hpp"
#include "QueryExpression.hpp"
#include "UTF8.hpp"
#include "Manager.hpp"
#include "Document.hpp"
#include "Results.hpp"
#include "IndexLookup.hpp"

#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/XPath2MemoryManager.hpp>

#include <db_cxx.h>
#include <cerrno>

using namespace DbXml;
using namespace std;

static const char *className = "XmlContainer";
#define CHECK_POINTER checkNullPointer(container_,className)

// XmlContainer

XmlContainer::XmlContainer()
	: container_(0)
{
}

XmlContainer::XmlContainer(TransactedContainer *container)
	: container_(container)
{
	if (container_)
		container_->acquire();
}

XmlContainer::~XmlContainer()
{
	if (container_)
		container_->release();
}

XmlContainer::XmlContainer(const XmlContainer &o)
	: container_(o.container_)
{
	if (container_)
		container_->acquire();
}

XmlContainer &XmlContainer::operator=(const XmlContainer &o)
{
	if(container_ != o.container_) {
		if (container_)
			container_->release();
		container_ = o.container_;
		if (container_)
			container_->acquire();
	}

	return *this;
}

void XmlContainer::close()
{
	if (container_)
		container_->close();
}

void XmlContainer::sync()
{
	if (container_)
		container_->sync();
}

XmlManager &XmlContainer::getManager()
{
	CHECK_POINTER;
	return container_->getManager();
}

void XmlContainer::setIndexSpecification(XmlTransaction &txn, 
					 const XmlIndexSpecification &index,
					 XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->setIndexSpecification(txn, index, 
							context);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
}

void XmlContainer::setIndexSpecification(const XmlIndexSpecification &index, 
					 XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->setIndexSpecification(0, index, context);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
}

XmlIndexSpecification XmlContainer::getIndexSpecification
(XmlTransaction &txn, u_int32_t flags) const
{
	CHECK_POINTER;
	XmlIndexSpecification index;
	int err = 0;
	bool lock = (flags & DB_RMW) ? true : false;
	try {
		err = container_->getConfigurationDB()->getIndexSpecification
			(txn, index, lock);
		// we've just overwritten the new index and the
		// built-in iterator is pointing to the old memory
		index.reset();
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
	return index;
}

XmlIndexSpecification XmlContainer::getIndexSpecification() const
{
	CHECK_POINTER;
	XmlIndexSpecification index;
	int err = 0;
	try {
		err = container_->getConfigurationDB()->getIndexSpecification(0, index);
		// we've just overwritten the new index and the
		// built-in iterator is pointing to the old memory
		index.reset();
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
	return index;
}

void XmlContainer::addIndex(XmlTransaction &txn, const std::string &uri,
			    const std::string &name, const std::string &index,
			    XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.addIndex(uri, name, index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::addIndex(const std::string &uri, const std::string &name,
			    const std::string &index, XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.addIndex(uri, name, index);
	setIndexSpecification(is, context);
}

void XmlContainer::addIndex(XmlTransaction &txn,
			    const std::string &uri, const std::string &name,
			    XmlIndexSpecification::Type indexType,
			    XmlValue::Type syntaxType,
			    XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.addIndex(uri, name, indexType, syntaxType);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::addIndex(const std::string &uri, const std::string &name,
			    XmlIndexSpecification::Type indexType,
			    XmlValue::Type syntaxType,
			    XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.addIndex(uri, name, indexType, syntaxType);
	setIndexSpecification(is, context);
}

void XmlContainer::deleteIndex(XmlTransaction &txn, const std::string &uri,
			       const std::string &name, const std::string &index,
			       XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.deleteIndex(uri, name, index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::deleteIndex(const std::string &uri, const std::string &name,
			       const std::string &index, 
			       XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.deleteIndex(uri, name, index);
	setIndexSpecification(is, context);
}

void XmlContainer::replaceIndex(XmlTransaction &txn, const std::string &uri,
				const std::string &name, 
				const std::string &index,
				XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.replaceIndex(uri, name, index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::replaceIndex(const std::string &uri, const std::string &name,
				const std::string &index, 
				XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.replaceIndex(uri, name, index);
	setIndexSpecification(is, context);
}

void XmlContainer::addDefaultIndex(XmlTransaction &txn, const std::string &index,
				   XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.addDefaultIndex(index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::addDefaultIndex(const std::string &index, XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.addDefaultIndex(index);
	setIndexSpecification(is, context);
}

void XmlContainer::deleteDefaultIndex(XmlTransaction &txn, const std::string &index,
				      XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.deleteDefaultIndex(index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::deleteDefaultIndex(const std::string &index, XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.deleteDefaultIndex(index);
	setIndexSpecification(is, context);
}

void XmlContainer::replaceDefaultIndex(XmlTransaction &txn, const std::string &index,
				       XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification(txn);
	is.replaceDefaultIndex(index);
	setIndexSpecification(txn, is, context);
}

void XmlContainer::replaceDefaultIndex(const std::string &index, XmlUpdateContext &context)
{
	XmlIndexSpecification is = getIndexSpecification();
	is.replaceDefaultIndex(index);
	setIndexSpecification(is, context);
}

const std::string &XmlContainer::getName() const
{
	CHECK_POINTER;
	return container_->getName();
}

bool XmlContainer::addAlias(const std::string &alias)
{
	CHECK_POINTER;
	return container_->addAlias(alias);
}

bool XmlContainer::removeAlias(const std::string &alias)
{
	CHECK_POINTER;
	return container_->removeAlias(alias);
}

XmlContainer::ContainerType XmlContainer::getContainerType() const
{
	CHECK_POINTER;
	return container_->getContainerType();
}

bool XmlContainer::getIndexNodes() const
{
	CHECK_POINTER;
	return container_->getIndexNodes();
}

u_int32_t XmlContainer::getPageSize() const
{
	CHECK_POINTER;
	return container_->getPageSize();
}

std::string XmlContainer::putDocument(const std::string &name,
				      XmlInputStream *input,
				      XmlUpdateContext &context,
				      u_int32_t flags)
{
	CHECK_POINTER;
	XmlDocument document = container_->getManager().createDocument();
	document.setName(name);
	document.setContentAsXmlInputStream(input);
	putDocument(document, context, flags);
	return document.getName();
}

std::string XmlContainer::putDocument(XmlTransaction &txn,
				      const std::string &name,
				      XmlInputStream *input,
				      XmlUpdateContext &context,
				      u_int32_t flags)
{
	CHECK_POINTER;
	XmlDocument document = container_->getManager().createDocument();
	document.setName(name);
	document.setContentAsXmlInputStream(input);
	putDocument(txn, document, context, flags);
	return document.getName();
}

void XmlContainer::putDocument(XmlTransaction &txn, XmlDocument &document,
			       XmlUpdateContext &context, u_int32_t flags)
{
	CHECK_POINTER;
	container_->checkFlags(Log::misc_flag_info, "putDocument()", flags, DBXML_GEN_NAME);
	int err = 0;
	try {
		err = container_->addDocument(txn, document, context, flags);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
}

void XmlContainer::putDocument(XmlDocument &document,
			       XmlUpdateContext &context,
			       u_int32_t flags)
{
	CHECK_POINTER;
	container_->checkFlags(Log::misc_flag_info, "putDocument()", flags, DBXML_GEN_NAME);
	int err = 0;
	try {
		err = container_->addDocument(0, document, context, flags);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);
}

string XmlContainer::putDocument(XmlTransaction &txn,
				 const std::string &name,
				 const std::string &contents, 
				 XmlUpdateContext &context, u_int32_t flags)
{
	CHECK_POINTER;
	XmlDocument document = container_->getManager().createDocument();
	document.setName(name);
	document.setContent(contents);
	putDocument(txn, document, context, flags);
	return document.getName();
}

string XmlContainer::putDocument(const std::string &name,
				 const std::string &contents,
				 XmlUpdateContext &context, u_int32_t flags)
{
	CHECK_POINTER;
	XmlDocument document = container_->getManager().createDocument();
	document.setName(name);
	document.setContent(contents);
	putDocument(document, context, flags);
	return document.getName();
}

static void throwDocNotFound(const std::string &name)
{
	ostringstream s;
	std::string tname(name);
	if (tname.size() == 0)
		tname = "<empty name>";
	s << "Document not found: " << tname;
	throw XmlException(XmlException::DOCUMENT_NOT_FOUND, 
			   s.str().c_str());

}

XmlDocument XmlContainer::getDocument(XmlTransaction &txn, 
				      const std::string &name, u_int32_t flags)
{
	CHECK_POINTER;
	container_->checkFlags(Log::misc_flag_info, "getDocument()", flags,
			       DB_DIRTY_READ|DB_RMW|DB_DEGREE_2|DBXML_LAZY_DOCS);
	XmlDocument r;
	int err = 0;
	try {
		OperationContext oc(txn);
		err = container_->getDocument(oc, name, r, flags);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(name);
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(name);
	else if (err != 0)
		throw XmlException(err);

	return r;
}

XmlDocument XmlContainer::getDocument(const std::string &name, u_int32_t flags)
{
	CHECK_POINTER;
	container_->checkFlags(Log::misc_flag_info, "getDocument()", flags, DB_DIRTY_READ|DB_RMW|DB_DEGREE_2|DBXML_LAZY_DOCS);
	XmlDocument r;
	int err = 0;
	try {
		OperationContext oc;
		err = container_->getDocument(oc, name, r, flags);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(name);
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(name);
	else if (err != 0)
		throw XmlException(err);

	return r;
}

void XmlContainer::deleteDocument(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->deleteDocument(txn, name, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(name);
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(name);
	else if (err != 0)
		throw XmlException(err);
}

void XmlContainer::deleteDocument(const std::string &name,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->deleteDocument(0, name, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(name);
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(name);
	else if (err != 0)
		throw XmlException(err);
}

void XmlContainer::deleteDocument(XmlTransaction &txn, XmlDocument &document,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->deleteDocument(txn, document, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(document.getName());
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(document.getName());
	else if (err != 0)
		throw XmlException(err);
}

void XmlContainer::deleteDocument(XmlDocument &document,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->deleteDocument(0, document, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			throwDocNotFound(document.getName());
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(document.getName());
	else if (err != 0)
		throw XmlException(err);
}

void XmlContainer::updateDocument(XmlTransaction &txn, XmlDocument &document,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->updateDocument(txn, document, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			err = DB_NOTFOUND;
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(document.getName());
	else if (err != 0)
		throw XmlException(err);
}

void XmlContainer::updateDocument(XmlDocument &document,
				  XmlUpdateContext &context)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = container_->updateDocument(0, document, context);
	} catch (DbException &e) {
		if (e.get_errno() == DB_NOTFOUND)
			err = DB_NOTFOUND;
		else
			throw XmlException(e);
	}
	if (err == DB_NOTFOUND)
		throwDocNotFound(document.getName());
	else if (err != 0)
		throw XmlException(err);
}

// common function for getAllDocuments() and getNumDocuments variants.
// Always use lazy evaluation
static Results *getDocs(XmlContainer &container,
			Transaction *txn,
			bool lazy,
			u_int32_t flags)
{
	XmlQueryContext context = container.getManager().createQueryContext();
	if (lazy)
		context.setEvaluationType(XmlQueryContext::Lazy);
	if (flags == 0)
		flags = DBXML_LAZY_DOCS;

	XmlIndexLookup il = container.getManager().createIndexLookup(container,
		metaDataNamespace_uri, metaDataName_name,
		"unique-metadata-equality-string");
	return ((IndexLookup&)il).execute(txn, context, flags);
}

XmlResults XmlContainer::getAllDocuments(u_int32_t flags)
{
	CHECK_POINTER;
	Results *res = getDocs(*this, NULL, true, flags);
	return res;
}

XmlResults XmlContainer::getAllDocuments(XmlTransaction &txn,
					 u_int32_t flags)
{
	CHECK_POINTER;
	Results *res = getDocs(*this, txn, true, flags);
	return res;
}

size_t XmlContainer::getNumDocuments()
{
	CHECK_POINTER;
	Results *res = getDocs(*this, NULL, false, 0);
	size_t num = res->size();
	delete res;
	return num;
}

size_t XmlContainer::getNumDocuments(XmlTransaction &txn)
{
	CHECK_POINTER;
	Results *res = getDocs(*this, txn, false, 0);
	size_t num = res->size();
	delete res;
	return num;
}

//
// all lookupIndex methods are deprecated in favor of
// createIndexLookup.
//
XmlResults XmlContainer::lookupIndex(XmlQueryContext &context,
				     const std::string &uri,
				     const std::string &name,
				     const std::string &index,
				     const XmlValue &value, u_int32_t flags)
{
	// NOTE: Operation is always EQ for compatibility interfaces
	XmlIndexLookup il = getManager().createIndexLookup(
		*this, uri, name, index, value,
		XmlIndexLookup::EQ);
	return ((IndexLookup&)il).execute(0,context, flags);
}

XmlResults XmlContainer::lookupIndex(XmlQueryContext &context,
				     const std::string &uri,
				     const std::string &name,
				     const std::string &parent_uri,
				     const std::string &parent_name,
				     const std::string &index,
				     const XmlValue &value, u_int32_t flags)
{
	// NOTE: Operation is always EQ for compatibility interfaces
	XmlIndexLookup il = getManager().createIndexLookup(
		*this, uri, name, index, value, XmlIndexLookup::EQ);
	il.setParent(parent_uri, parent_name);
	return ((IndexLookup&)il).execute(0,context, flags);
}

XmlResults XmlContainer::lookupIndex(XmlTransaction &txn,
				     XmlQueryContext &context,
				     const std::string &uri,
				     const std::string &name,
				     const std::string &index,
				     const XmlValue &value,
				     u_int32_t flags)
{
	// NOTE: Operation is always EQ for compatibility interfaces
	XmlIndexLookup il = getManager().createIndexLookup(
		*this, uri, name, index, value, XmlIndexLookup::EQ);

	return ((IndexLookup&)il).execute(txn,context, flags);
}

XmlResults XmlContainer::lookupIndex(XmlTransaction &txn,
				     XmlQueryContext &context,
				     const std::string &uri,
				     const std::string &name,
				     const std::string &parent_uri,
				     const std::string &parent_name,
				     const std::string &index,
				     const XmlValue &value,
				     u_int32_t flags)
{
	// NOTE: Operation is always EQ for compatibility interfaces
	XmlIndexLookup il = getManager().createIndexLookup(
		*this, uri, name, index, value, XmlIndexLookup::EQ);

	il.setParent(parent_uri, parent_name);
	return ((IndexLookup&)il).execute(txn,context, flags);
}

XmlStatistics XmlContainer::lookupStatistics(const std::string &uri,
					     const std::string &name,
					     const std::string &index,
					     const XmlValue &value)
{
	CHECK_POINTER;
	Name child(uri, name);
	return container_->lookupStatistics(NULL, // txn
					    &child,
					    NULL, // parent
					    index, value);
}

XmlStatistics XmlContainer::lookupStatistics(
	const std::string &uri,
	const std::string &name,
	const std::string &parent_uri,
	const std::string &parent_name,
	const std::string &index,
	const XmlValue &value)
{
	CHECK_POINTER;
	Name child(uri, name);
	Name parent(parent_uri, parent_name);
	return container_->lookupStatistics(NULL, // txn
					    &child,
					    &parent,
					    index, value);
}

XmlStatistics XmlContainer::lookupStatistics(XmlTransaction &txn,
					     const std::string &uri,
					     const std::string &name,
					     const std::string &index,
					     const XmlValue &value)
{
	CHECK_POINTER;
	Name child(uri, name);
	return container_->lookupStatistics(txn,
					    &child,
					    NULL, // parent
					    index, value);
}

XmlStatistics XmlContainer::lookupStatistics(
	XmlTransaction &txn,
	const std::string &uri,
	const std::string &name,
	const std::string &parent_uri,
	const std::string &parent_name,
	const std::string &index,
	const XmlValue &value)
{
	CHECK_POINTER;
	Name child(uri, name);
	Name parent(parent_uri, parent_name);
	return container_->lookupStatistics(txn,
					    &child,
					    &parent,
					    index, value);
}

XmlContainer::operator Container *()
{
	CHECK_POINTER;
	return container_;
}

XmlContainer::operator Container &()
{
	CHECK_POINTER;
	return *container_;
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

DBXML_EXPORT void DbXml::setLogLevel(LogLevel level, bool enabled)
{
	Log::setLogLevel((Log::ImplLogLevel)level, enabled);
}

DBXML_EXPORT void DbXml::setLogCategory(LogCategory category, bool enabled)
{
	Log::setLogCategory((Log::ImplLogCategory)category, enabled);
}
