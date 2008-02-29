//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlURIResolver.hpp,v 1.17 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLURIRESOLVER_HPP
#define	__DBXMLURIRESOLVER_HPP

#include <string>
#include <vector>

#include <dbxml/XmlResolver.hpp>

#include <xqilla/context/URIResolver.hpp>
#include <xqilla/schema/DocumentCache.hpp>
#include <xqilla/context/ModuleResolver.hpp>

#include <xercesc/util/XMLEntityResolver.hpp>

class DynamicContext;

namespace DbXml
{

class XmlValue;
class Value;
class XmlResults;
class XmlManager;
class Transaction;
class XmlInputStream;
class ReferenceMinder;

class DbXmlSchemeResolver : public XmlResolver
{
public:
	virtual XmlInputStream *resolveSchema(
		XmlTransaction *txn, XmlManager &mgr,
		const std::string &schemaLocation,
		const std::string &nameSpace) const;
	virtual XmlInputStream *resolveEntity(
		XmlTransaction *txn, XmlManager &mgr,
		const std::string &systemId,
		const std::string &publicId) const;
	virtual XmlInputStream *resolveModule(
		XmlTransaction *txn, XmlManager &mgr,
		const std::string &moduleLocation,
		const std::string &nameSpace) const;
};

class ResolverStore {
public:
	typedef std::vector<const XmlResolver*> VectorOfResolvers;
	typedef VectorOfResolvers::const_iterator const_iterator;

	ResolverStore();

	bool getSecure() const { return secure_; }
	void setSecure(bool secure) { secure_ = secure; }

	void registerResolver(const XmlResolver &resolver);
	const_iterator begin() const;
	const_iterator end() const;

private:
	bool secure_;
	DbXmlSchemeResolver dbxmlResolver_;
	VectorOfResolvers resolvers_;
};

class DbXmlURIResolver : public URIResolver,
			 public ModuleResolver,
			 public XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver
{
public:
	DbXmlURIResolver(XmlManager &mgr, Transaction *txn);

	Transaction *getTransaction() const {
		return txn_;
	}
	void setTransaction(Transaction *txn) {
		txn_ = txn;
	}

	/// From URIResolver
	virtual bool resolveDocument(Sequence &result, const XMLCh* uri,
		DynamicContext* context);

	/// From URIResolver
	virtual bool resolveCollection(Sequence &result, const XMLCh* uri,
		DynamicContext* context);

	/// From URIResolver
	virtual bool resolveDefaultCollection(Sequence &result,
		DynamicContext* context);

	/// From ModuleResolver
	virtual bool resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri,
		const StaticContext* context);

	/// From XMLEntityResolver
	virtual XERCES_CPP_NAMESPACE_QUALIFIER InputSource* resolveEntity(
		XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier*
		resourceIdentifier);

	bool resolveDocument(const std::string &uri, XmlValue &result) const;
	bool resolveCollection(const std::string &uri,
		XmlResults &result) const;
	XmlInputStream *resolveSchema(const std::string &schemaLocation,
		const std::string &nameSpace) const;
	XmlInputStream *resolveEntity(const std::string &systemId,
		const std::string &publicId) const;
	bool resolveModuleLocation(const std::string &nameSpace,
		XmlResults &result) const;
	XmlInputStream *resolveModule(const std::string &location,
		const std::string &nameSpace) const;
private:
	bool resolveDocKnownSchemes(const std::string &uri, XmlValue &value,
		ReferenceMinder *minder);
	
	XmlManager &mgr_;
	Transaction *txn_;
};

}

#endif
