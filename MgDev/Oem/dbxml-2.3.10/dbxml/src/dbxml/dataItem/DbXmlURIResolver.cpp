//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlURIResolver.cpp,v 1.32 2006/11/09 14:25:04 jsnelson Exp $
//

#include <dbxml/XmlValue.hpp>
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlResolver.hpp>
#include <dbxml/XmlTransaction.hpp>
#include <dbxml/XmlData.hpp>
#include "DbXmlURIResolver.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlUri.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../Results.hpp"
#include "../Manager.hpp"
#include "../Document.hpp"
#include "../XmlInputStreamWrapper.hpp"
#include "../nodeStore/NsUtil.hpp"

#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>

#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <sstream>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static void _throwResolutionError(const std::string &uri, const char *resource)
{
	ostringstream oss;
	oss << "External access not allowed.  Cannot resolve\n\t" << resource << ": " <<
		uri;
	throw XmlException(XmlException::XPATH_EVALUATION_ERROR,
			   oss.str());
}

XmlInputStream *DbXmlSchemeResolver::resolveSchema(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &schemaLocation,
	const std::string &nameSpace) const
{
	DbXmlUri uri(schemaLocation, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		Transaction *t = 0;
		if (txn)
			t = *txn;
		XmlDocument doc(uri.openDocument(mgr, t));
		// Can't use getContentAsInputStream(), as the XmlInputStream
		// returned does not hold a reference to the XmlDocument, and
		// therefore becomes invalid when the XmlDocument goes out of
		// scope - jpcs
		XmlData content(doc.getContent());
		char *copy = new char[content.get_size()];
		memcpy(copy, content.get_data(), content.get_size());
		return mgr.createMemBufInputStream(copy, content.get_size(),
						   schemaLocation.c_str(),
						   /*adoptBuffer*/true);
	}
	return 0;
}

XmlInputStream *DbXmlSchemeResolver::resolveEntity(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &systemId,
	const std::string &publicId) const
{
	DbXmlUri uri(systemId, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		Transaction *t = 0;
		if (txn)
			t = *txn;
		XmlDocument doc(uri.openDocument(mgr, t));
		// Can't use getContentAsInputStream(), as the XmlInputStream
		// returned does not hold a reference to the XmlDocument, and
		// therefore becomes invalid when the XmlDocument goes out of
		// scope - jpcs
		XmlData content(doc.getContent());
		char *copy = new char[content.get_size()];
		memcpy(copy, content.get_data(), content.get_size());
		return mgr.createMemBufInputStream(copy, content.get_size(),
						   systemId.c_str(),
						   /*adoptBuffer*/true);
	}
	return 0;
}

XmlInputStream *DbXmlSchemeResolver::resolveModule(
	XmlTransaction *txn, XmlManager &mgr,
	const std::string &moduleLocation,
	const std::string &nameSpace) const
{
	DbXmlUri uri(moduleLocation, /*documentUri*/true);
	if(uri.isDbXmlScheme()) {
		Transaction *t = 0;
		if (txn)
			t = *txn;
		XmlDocument doc(uri.openDocument(mgr, t));
		// Can't use getContentAsInputStream(), as the XmlInputStream
		// returned does not hold a reference to the XmlDocument, and
		// therefore becomes invalid when the XmlDocument goes out of
		// scope - jpcs
		XmlData content(doc.getContent());
		char *copy = new char[content.get_size()];
		memcpy(copy, content.get_data(), content.get_size());
		return mgr.createMemBufInputStream(copy, content.get_size(),
						   moduleLocation.c_str(),
						   /*adoptBuffer*/true);
	}
	return 0;
}

ResolverStore::ResolverStore()
	: secure_(false)
{
 	registerResolver(dbxmlResolver_);
}

void ResolverStore::registerResolver(const XmlResolver &resolver)
{
	resolvers_.push_back(&resolver);
}

ResolverStore::const_iterator ResolverStore::begin() const
{
	return resolvers_.begin();
}

ResolverStore::const_iterator ResolverStore::end() const
{
	return resolvers_.end();
}

DbXmlURIResolver::DbXmlURIResolver(XmlManager &mgr, Transaction *txn)
	: mgr_(mgr),
	  txn_(txn)
{
}

static std::string _createURI(const XMLCh *base, const std::string &uri)
{
	if (!base) {
		return uri;
	}
	std::string retVal;
	XMLChToUTF8 base8(base);
	retVal = base8.str();
	retVal += uri;
	// trust the result, since setBaseURI validates the base
	// as having the form, xxxx:*
	return retVal;
}

bool DbXmlURIResolver::resolveDocument(Sequence &result, const XMLCh* uri,
				       DynamicContext* context)
{
	DbXmlContext *dbContext = CAST_TO_DBXMLCONTEXT(context);

	QueryContext &qc = dbContext->getQueryContext();

	// Concat base URI (if present) and uri
	// TBD: consider using XMLUri class to validate the result
	bool retVal = false;
	XMLChToUTF8 uri8(uri);
	XmlValue value;
	std::string sysId;

	// Look for known schemes without Base first
	if (resolveDocKnownSchemes(uri8.str(), value, qc.getMinder())) {
		retVal = true;
		sysId = uri8.str();
	} else {
		sysId = _createURI(context->getBaseURI(), uri8.str());
		if(resolveDocument(sysId, value)) {
			retVal = true;
		} else if(((Manager&)mgr_).getResolverStore().
			  getSecure()) {
			_throwResolutionError(sysId, "document");
		} else {
			// try to find known URI schemes using base
			retVal = resolveDocKnownSchemes(sysId, value, qc.getMinder());
		}
	}
	if(retVal && !value.isNull()) {
		result.addItem(Value::convertToItem(value, context));
	}
	return retVal;
}

bool DbXmlURIResolver::resolveCollection(Sequence &result, const XMLCh* uri,
					 DynamicContext* context)
{

	XMLChToUTF8 uri8(uri);
	// Concat base URI (if present) and uri
	// TBD: consider using XMLUri class to validate the result
	std::string sysId = _createURI(context->getBaseURI(), uri8.str());
	bool retVal = true;
	XmlResults results = new ValueResults();
	if(resolveCollection(sysId, results)) {
		results.reset();
		XmlValue value;
		while(results.next(value)) {
			result.addItem(Value::convertToItem(value,
							    context));
		}
	} else if(((Manager&)mgr_).getResolverStore().getSecure()) {
		_throwResolutionError(sysId, "collection");
	} else {
		retVal = false;
	}
	return retVal;
}

bool DbXmlURIResolver::resolveDefaultCollection(Sequence &result, DynamicContext* context)
{
    // Handled in QueryPlanFunction
    return false;
}

bool DbXmlURIResolver::resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri,
	const StaticContext* context)
{
	XmlResults results = new ValueResults();
	if(resolveModuleLocation(XMLChToUTF8(nsUri).str(), results)) {
		results.reset();
		XmlValue value;
		while(results.next(value)) {
			result->push_back(context->getMemoryManager()->
				getPooledString(value.asString().c_str()));
		}
		return true;
	}
	return false;
}

InputSource* DbXmlURIResolver::resolveEntity(XMLResourceIdentifier *ri)
{
	XmlInputStream *result = 0;
	switch(ri->getResourceIdentifierType()) {
	case XMLResourceIdentifier::SchemaGrammar:
	case XMLResourceIdentifier::SchemaImport:
	case XMLResourceIdentifier::SchemaInclude:
	case XMLResourceIdentifier::SchemaRedefine: {
		result = resolveSchema(XMLChToUTF8(ri->getSchemaLocation()).str(),
				       XMLChToUTF8(ri->getNameSpace()).str());
		break;
	}
	case XMLResourceIdentifier::UnKnown: {
		// XQuery module imports are of type UnKnown
		result = resolveModule(XMLChToUTF8(ri->getSystemId()).str(),
			XMLChToUTF8(ri->getNameSpace()).str());
		if(result == 0) {
			// Preserve old behaviour for compatibility
			// TBD This should be deprecated and taken out at a later date - jpcs
			result = resolveEntity(XMLChToUTF8(ri->getSystemId()).str(),
				XMLChToUTF8(ri->getNameSpace()).str());
		}
		break;
	}
	case XMLResourceIdentifier::ExternalEntity: {
		result = resolveEntity(XMLChToUTF8(ri->getSystemId()).str(),
				       XMLChToUTF8(ri->getPublicId()).str());
		break;
	}
	default: {
		// Stop compiler warnings
		break;
	}
	}

	if(result != 0) {
		XmlInputStreamWrapper *res = new XmlInputStreamWrapper(&result);
		res->setPublicId(ri->getPublicId());
		res->setSystemId(ri->getSystemId());
		return res;
	} else if(((Manager&)mgr_).getResolverStore().getSecure()) {
		_throwResolutionError(
			XMLChToUTF8(ri->getSystemId()).str(),
			"entity");
	}
	return 0;
}

bool DbXmlURIResolver::resolveDocument(const std::string &uri,
				       XmlValue &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = ((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it = ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveDocument(txn.get(), mgr_, uri, result)) {
			return true;
		}
	}
	return false;
}

bool DbXmlURIResolver::resolveCollection(const std::string &uri,
					 XmlResults &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = ((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it = ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveCollection(txn.get(), mgr_, uri, result)) {
			return true;
		}
	}
	return false;
}

bool DbXmlURIResolver::resolveModuleLocation(const std::string &nameSpace,
	XmlResults &result) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = ((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it = ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveModuleLocation(txn.get(), mgr_, nameSpace, result)) {
			return true;
		}
	}
	return false;
}

XmlInputStream *DbXmlURIResolver::resolveSchema(
	const std::string &schemaLocation, const std::string &nameSpace) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end = ((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it = ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveSchema(txn.get(), mgr_, schemaLocation, nameSpace);
		if(result != 0) break;
	}
	return result;
}

XmlInputStream *DbXmlURIResolver::resolveEntity(
	const std::string &systemId, const std::string &publicId) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end =
		((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it =
		    ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveEntity(txn.get(), mgr_, systemId,
					      publicId);
		if(result != 0) break;
	}
	return result;
}

XmlInputStream *DbXmlURIResolver::resolveModule(const std::string &location,
	const std::string &nameSpace) const
{
	ScopedPtr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	XmlInputStream *result = 0;
	ResolverStore::const_iterator end =
		((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it =
		    ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		result = (*it)->resolveModule(txn.get(), mgr_, location,
					      nameSpace);
		if(result != 0) break;
	}
	return result;
}

bool DbXmlURIResolver::resolveDocKnownSchemes(const std::string &uri,
	XmlValue &value, ReferenceMinder *minder)
{
	bool retVal = false;

	// Check in the ReferenceMinder for the document first
	UTF8ToXMLCh uri16(uri);
	Document *doc = minder->findDocument(uri16.str());
	if(doc != 0) {
		value = XmlValue(XmlDocument(doc));
		retVal = true;
	}
	else if (uri.find("http:") == 0) {
		XmlDocument doc = mgr_.createDocument();
		XmlInputStream *is = mgr_.createURLInputStream("", uri);
		doc.setContentAsXmlInputStream(is);
		doc.setName(uri);
		((Document&)doc).setDocumentURI(uri16.str());

		// This is enough to check that the resource exists
		try {
			is->curPos();
			minder->addDocument(doc);
			value = XmlValue(doc);
			retVal = true;
		}
		catch(XmlException &e) {}
	} else if (uri.find("file:") == 0) {
		std::string local;
		local.assign(uri, 5, uri.size());
		XmlDocument doc = mgr_.createDocument();
		XmlInputStream *is = mgr_.createLocalFileInputStream(local);
		doc.setContentAsXmlInputStream(is);
		doc.setName(uri);
		((Document&)doc).setDocumentURI(uri16.str());

		// This is enough to check that the resource exists
		try {
			is->curPos();
			minder->addDocument(doc);
			value = XmlValue(doc);
			retVal = true;
		}
		catch(XmlException &e) {}
	}
	// TBD: others...
	return retVal;
}
