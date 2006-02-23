//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlDynamicContextImpl.cpp,v 1.28 2005/10/12 12:20:18 jsnelson Exp $
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

#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>
#include <pathan/exceptions/TypeNotFoundException.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/Node.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/functionAPI/DatatypeLookup.hpp>
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/internal/context/DocumentCacheImpl.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include <pathan/URIResolver.hpp>
#include <pathan/exceptions/XMLParseException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/internal/utils/ContextUtils.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

static void _throwResolutionError(const XMLCh *uri, const char *resource)
{
	ostringstream oss;
	XMLChToUTF8 uuri(uri);
	oss << "Cannot resolve " << resource << ": " <<
		uuri.str();
	throw XmlException(XmlException::XPATH_EVALUATION_ERROR,
			   oss.str());
}

DbXmlDynamicContextImpl::DbXmlDynamicContextImpl(QueryContext &qc, Transaction *txn, const XQContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
	: DbXmlContext(qc, txn),
	  _staticContext(staticContext),
	  _createdWith(memMgr),
	  _internalMM(memMgr),
	  _nsResolver(staticContext->getNSResolver()),
	  _defaultCollation(0),
	  _ordering(staticContext->getNodeSetOrdering()),
	  _contextItem(0),
	  _contextPosition(1),
	  _contextSize(1),
	  _implicitTimezone(0),
	  _resolvers(PathanAllocator<URIResolver*>(&_internalMM)),
	  // This is created with the _createdWith memory manager,
	  // since a bug in xerces means we can't use a non-thread-safe
	  // memory manager - jpcs
	  _docCache(staticContext->getDocumentCache()->createDerivedCache(memMgr))
{
	time(&_currentTime);
	_memMgr = &_internalMM;
	_varStore = _internalMM.createVariableStore();
	_pathanFactory = new (&_internalMM) DbXmlFactoryImpl(_docCache, &_internalMM);

	m_pDebugCallback = NULL;
	m_bEnableDebugging = false;

	// DbXml specific
	registerURIResolver(&resolver_);
	setXMLEntityResolver(&resolver_);
}

DbXmlDynamicContextImpl::~DbXmlDynamicContextImpl()
{
	_varStore->clear();
	_contextItem = 0;
	_implicitTimezone = 0;

	((XQFactory*)_pathanFactory)->release();
	_docCache->release();
}

void DbXmlDynamicContextImpl::release()
{
	this->~DbXmlDynamicContextImpl();
	_createdWith->deallocate(this);
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
	if(memMgr == 0) {
		return 0;
	}
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
	_contextItem = 0;
	_contextSize = 1;
	_contextPosition = 1;
	_varStore->clear();
	_implicitTimezone = 0;
	_resolvers.clear();
	time(&_currentTime);
}

const DocumentCache *DbXmlDynamicContextImpl::getDocumentCache() const
{
	return _docCache;
}

void DbXmlDynamicContextImpl::setContextItem(const Item::Ptr &item)
{
	_contextItem = item;
}

void DbXmlDynamicContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	DBXML_ASSERT(false);
}

void DbXmlDynamicContextImpl::setContextSize(unsigned int size)
{
	_contextSize = size;
}

void DbXmlDynamicContextImpl::setContextPosition(unsigned int pos)
{
	_contextPosition = pos;
}

const Item::Ptr &DbXmlDynamicContextImpl::getContextItem() const
{
	return _contextItem;
}

unsigned int DbXmlDynamicContextImpl::getContextSize() const
{
	return _contextSize;
}

unsigned int DbXmlDynamicContextImpl::getContextPosition() const
{
	return _contextPosition;
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

const ATDurationOrDerived::Ptr &DbXmlDynamicContextImpl::getImplicitTimezone() const {
	if(_implicitTimezone == NULLRCP) {
		// validate tzone
		Timezone tzone(ContextUtils::getTimezone());
		// c-style the const away since we are in a const method
		((DbXmlDynamicContextImpl*)this)->_implicitTimezone = tzone.asDayTimeDuration(this);
	}

	return _implicitTimezone;
}

void DbXmlDynamicContextImpl::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) {
	// validate the timezone -- will trow if invalid
	Timezone timezone(timezoneAsDuration, this);
	this->_implicitTimezone = timezoneAsDuration;
}

XPath2MemoryManager* DbXmlDynamicContextImpl::getMemoryManager() const
{
	return _memMgr;
}

void DbXmlDynamicContextImpl::registerURIResolver(URIResolver *resolver)
{
	if(resolver != 0) {
		_resolvers.push_back(resolver);
	}
}

Sequence DbXmlDynamicContextImpl::resolveDocument(const XMLCh* uri)
{
	bool found = false;
	Sequence result(getMemoryManager());
	std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
	for(std::vector<URIResolver *,
		    PathanAllocator<URIResolver*> >::reverse_iterator i =
		    _resolvers.rbegin(); i != end; ++i) {
		if((*i)->resolveDocument(result, uri, this)) {
			found = true;
			break;
		}
	}

	if(!found) {
		_throwResolutionError(uri, "document");
	}

	return result;
}

Sequence DbXmlDynamicContextImpl::resolveCollection(const XMLCh* uri)
{
	bool found = false;
	Sequence result(getMemoryManager());
	std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
	for(std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin(); i != end; ++i) {
		if((*i)->resolveCollection(result, uri, this)) {
			found = true;
			break;
		}
	}

	if(!found) {
		_throwResolutionError(uri, "collection");
	}

	return result;
}

Sequence DbXmlDynamicContextImpl::resolveDefaultCollection()
{
  Sequence result(getMemoryManager());
  std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
  for(std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin(); i != end; ++i) {
    if((*i)->resolveDefaultCollection(result, this)) {
      break;
    }
  }
  return result;
}

Node::Ptr DbXmlDynamicContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
	return _docCache->validate(node, valMode, this);
}

PathanFactory *DbXmlDynamicContextImpl::getPathanFactory() const
{
  return _pathanFactory;
}

void DbXmlDynamicContextImpl::setPathanFactory(PathanFactory *factory)
{
  _pathanFactory = factory;
}

void DbXmlDynamicContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
	((PathanNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void DbXmlDynamicContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
	_nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* DbXmlDynamicContextImpl::getNSResolver() const {
	return _nsResolver;
}

const XMLCh* DbXmlDynamicContextImpl::getUriBoundToPrefix(const XMLCh* prefix) const
{
	const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\'"), getMemoryManager());
		DSLthrow(NamespaceLookupException, X("DbXmlDynamicContextImpl::getUriBoundToPrefix"), msg);
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

void DbXmlDynamicContextImpl::setDefaultCollation(const XMLCh* URI)
{
	_defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* DbXmlDynamicContextImpl::getDefaultCollation() const
{
	if(_defaultCollation == 0) {
		return _staticContext->getDefaultCollation();
	}
	else {
		return getCollation(_defaultCollation);
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
