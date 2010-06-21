//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlDynamicContextImpl.cpp,v 1.42.2.1 2007/01/03 19:21:13 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include <iostream>
#include <sstream>
#include "DbXmlDynamicContextImpl.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../QueryContext.hpp"
#include "../ReferenceMinder.hpp"
#include "dbxml/XmlException.hpp"
#include "../UTF8.hpp"

#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/TypeNotFoundException.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/items/DatatypeLookup.hpp>
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>
#include <xqilla/dom-api/impl/XQillaNSResolverImpl.hpp>
#include <xqilla/context/URIResolver.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/utils/ContextUtils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlDynamicContextImpl::DbXmlDynamicContextImpl(QueryContext &qc, Transaction *txn, const DbXmlContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
	: DbXmlContext(qc, txn),
	  _staticContext(staticContext),
	  _createdWith(memMgr),
	  _internalMM(memMgr),
	  _nsResolver(staticContext->getNSResolver()),
	  _defaultCollation(0),
	  _ordering(staticContext->getNodeSetOrdering()),
	  _defaultElementNS(staticContext->getDefaultElementAndTypeNS()),
	  _resolvers(XQillaAllocator<URIResolver*>(&_internalMM)),
	  // This is created with the _createdWith memory manager,
	  // since a bug in xerces means we can't use a non-thread-safe
	  // memory manager - jpcs
	  _docCache(staticContext->getDocumentCache()->createDerivedCache(memMgr))
{
	time(&_currentTime);
	_memMgr = &_internalMM;
	_varStore = _internalMM.createVariableStore();
	_itemFactory = new (&_internalMM) DbXmlFactoryImpl(_docCache, &_internalMM);

	m_pDebugCallback = NULL;
	m_bEnableDebugging = false;

	// DbXml specific
	registerURIResolver(&resolver_);
	setModuleResolver(&resolver_);
	setXMLEntityResolver(&resolver_);
}

DbXmlDynamicContextImpl::~DbXmlDynamicContextImpl()
{
	_varStore->clear();

	delete _itemFactory;
	delete _docCache;
}

XERCES_CPP_NAMESPACE_QUALIFIER
DOMDocument *DbXmlDynamicContextImpl::createNewDocument() const
{
	DBXML_ASSERT(false);
	return 0;
}

void DbXmlDynamicContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
	DBXML_ASSERT(false);
}

DynamicContext *DbXmlDynamicContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
	return _staticContext->createDynamicContext(memMgr);
}

void DbXmlDynamicContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
	_memMgr = memMgr;
}

void DbXmlDynamicContextImpl::setDebugCallback(XQDebugCallback* callback)
{
	m_pDebugCallback=callback;
}

XQDebugCallback* DbXmlDynamicContextImpl::getDebugCallback() const
{
	return m_pDebugCallback;
}

void DbXmlDynamicContextImpl::enableDebugging(bool enable/*=true*/)
{
	m_bEnableDebugging=enable;
}

bool DbXmlDynamicContextImpl::isDebuggingEnabled() const
{
	return m_bEnableDebugging;
}

void DbXmlDynamicContextImpl::clearDynamicContext()
{
	_nsResolver = _staticContext->getNSResolver();
	_varStore->clear();
	_resolvers.clear();
	time(&_currentTime);
}

const DocumentCache *DbXmlDynamicContextImpl::getDocumentCache() const
{
	return _docCache;
}

void DbXmlDynamicContextImpl::setDocumentCache(DocumentCache* docCache)
{
	_docCache = docCache;
}

VariableStore* DbXmlDynamicContextImpl::getVariableStore()
{
	return _varStore;
}

time_t DbXmlDynamicContextImpl::getCurrentTime() const
{
	return _currentTime;
}

void DbXmlDynamicContextImpl::setCurrentTime(time_t newTime)
{
	_currentTime=newTime;
}

XPath2MemoryManager* DbXmlDynamicContextImpl::getMemoryManager() const
{
	return _memMgr;
}

const XMLCh* DbXmlDynamicContextImpl::getDefaultElementAndTypeNS() const
{
	return _defaultElementNS;
}

void DbXmlDynamicContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS) {
	_defaultElementNS = newNS;
}

void DbXmlDynamicContextImpl::registerURIResolver(URIResolver *resolver)
{
	if(resolver != 0) {
		_resolvers.push_back(resolver);
	}
}

Sequence DbXmlDynamicContextImpl::resolveDocument(const XMLCh* uri, const LocationInfo *location)
{
	bool found = false;
	Sequence result(getMemoryManager());
	std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
	for(std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin();
	    !found && i != end; ++i) {
		found = (*i)->resolveDocument(result, uri, this);
	}

	if(!found) {
		XMLBuffer errMsg;
		errMsg.set(X("Cannot resolve document: "));
		errMsg.append(uri);
		errMsg.append(X(" [err:FODC0002]"));
		XQThrow3(XMLParseException,X("DbXmlDynamicContextImpl::resolveDocument"),
			errMsg.getRawBuffer(), location);
	}

	return result;
}

Sequence DbXmlDynamicContextImpl::resolveCollection(const XMLCh* uri, const LocationInfo *location)
{
	bool found = false;
	Sequence result(getMemoryManager());
	std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
	for(std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin();
	    !found && i != end; ++i) {
		found = (*i)->resolveCollection(result, uri, this);
	}

	if(!found) {
		XMLBuffer errMsg;
		errMsg.set(X("Cannot resolve collection: "));
		errMsg.append(uri);
		errMsg.append(X(" [err:FODC0004]"));
		XQThrow3(XMLParseException,X("DbXmlDynamicContextImpl::resolveCollection"),
			errMsg.getRawBuffer(), location);
	}

	return result;
}

Sequence DbXmlDynamicContextImpl::resolveDefaultCollection()
{
  Sequence result(getMemoryManager());
  std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
  for(std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin(); i != end; ++i) {
    if((*i)->resolveDefaultCollection(result, this)) {
      break;
    }
  }
  return result;
}

Node::Ptr DbXmlDynamicContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
	DBXML_ASSERT(false);
	return 0;
}

ItemFactory *DbXmlDynamicContextImpl::getItemFactory() const
{
  return _itemFactory;
}

void DbXmlDynamicContextImpl::setItemFactory(ItemFactory *factory)
{
  _itemFactory = factory;
}

void DbXmlDynamicContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
	((XQillaNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void DbXmlDynamicContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
	_nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* DbXmlDynamicContextImpl::getNSResolver() const {
	return _nsResolver;
}

const XMLCh* DbXmlDynamicContextImpl::getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const
{
	const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\' [err:XPST0081]"), getMemoryManager());
		XQThrow3(NamespaceLookupException, X("DbXmlDynamicContextImpl::getUriBoundToPrefix"), msg, location);
	}

	return uri;
}

const XMLCh* DbXmlDynamicContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
	return _nsResolver->lookupPrefix(uri);
}

void DbXmlDynamicContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
	_docCache->setXMLEntityResolver(handler);
}

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* DbXmlDynamicContextImpl::getXMLEntityResolver() const
{
	return _docCache->getXMLEntityResolver();
}

void DbXmlDynamicContextImpl::setDefaultCollation(const XMLCh* URI)
{
	_defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* DbXmlDynamicContextImpl::getDefaultCollation(const LocationInfo *location) const
{
	if(_defaultCollation == 0) {
		return _staticContext->getDefaultCollation(location);
	}
	else {
		return getCollation(_defaultCollation, location);
	}
}

StaticContext::NodeSetOrdering DbXmlDynamicContextImpl::getNodeSetOrdering() const
{
	return _ordering;
}

void DbXmlDynamicContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{
	_ordering=newOrder;
}
