//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlURIResolver.cpp,v 1.21 2005/11/15 12:56:27 gmf Exp $
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

#include <pathan/Sequence.hpp>
#include <pathan/exceptions/XMLParseException.hpp>

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
	if(uri.isValid()) {
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
	if(uri.isValid()) {
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
    // TODO
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
	// XQuery module imports are of type UnKnown
	case XMLResourceIdentifier::UnKnown:
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
	scoped_ptr<XmlTransaction> txn;
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
	scoped_ptr<XmlTransaction> txn;
	if(txn_) txn.reset(new XmlTransaction(txn_));

	ResolverStore::const_iterator end = ((Manager&)mgr_).getResolverStore().end();
	for(ResolverStore::const_iterator it = ((Manager&)mgr_).getResolverStore().begin(); it != end; ++it) {
		if((*it)->resolveCollection(txn.get(), mgr_, uri, result)) {
			return true;
		}
	}
	return false;
}

XmlInputStream *DbXmlURIResolver::resolveSchema(
	const std::string &schemaLocation, const std::string &nameSpace) const
{
	scoped_ptr<XmlTransaction> txn;
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
	scoped_ptr<XmlTransaction> txn;
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

bool DbXmlURIResolver::resolveDocKnownSchemes(const std::string &uri,
	XmlValue &value, ReferenceMinder *minder)
{
	bool retVal = false;

	// Check in the ReferenceMinder for the document first
	Document *doc = minder->findDocument(uri);
	if(doc != 0) {
		value = XmlValue(XmlDocument(doc));
		retVal = true;
	}
	else if (uri.find("http:") == 0) {
		XmlDocument doc = mgr_.createDocument();
		XmlInputStream *is = mgr_.createURLInputStream("", uri);
		doc.setContentAsXmlInputStream(is);
		doc.setName(uri);
		((Document&)doc).setIdentifier(uri);
		value = XmlValue(doc);
		retVal = true;
	} else if (uri.find("file:") == 0) {
		std::string local;
		local.assign(uri, 5, uri.size());
		XmlDocument doc = mgr_.createDocument();
		XmlInputStream *is = mgr_.createLocalFileInputStream(local);
		doc.setContentAsXmlInputStream(is);
		doc.setName(uri);
		((Document&)doc).setIdentifier(uri);
		value = XmlValue(doc);
		retVal = true;

	}
	// TBD: others...
	return retVal;
}
