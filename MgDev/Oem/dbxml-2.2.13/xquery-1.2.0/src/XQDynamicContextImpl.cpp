//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQDynamicContextImpl.cpp,v 1.19 2005/10/12 12:17:34 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQContext.cpp: implementation of the XQContext class.
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include "xquery/context/XQDynamicContextImpl.hpp"
#include "xquery/context/XQFactoryImpl.hpp"

#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>
#include <pathan/exceptions/TypeNotFoundException.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/Collation.hpp>
#include <pathan/Item.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/functionAPI/DatatypeLookup.hpp>
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/internal/context/DocumentCacheImpl.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/URIResolver.hpp>
#include <pathan/exceptions/XMLParseException.hpp>
#include <pathan/internal/utils/ContextUtils.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

#include <xercesc/dom/DOMImplementation.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQDynamicContextImpl::XQDynamicContextImpl(const XQContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _staticContext(staticContext),
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
    _docCache(staticContext->getDocumentCache()->createDerivedCache(&_internalMM))
{
  time(&_currentTime);
  _memMgr = &_internalMM;
  _varStore = _internalMM.createVariableStore();
  _pathanFactory = new (&_internalMM) XQFactoryImpl(_docCache, &_internalMM);

  m_pDebugCallback = NULL;
  m_bEnableDebugging = false;
}

XQDynamicContextImpl::~XQDynamicContextImpl()
{
  _contextItem = 0;
  _implicitTimezone = 0;
}

void XQDynamicContextImpl::release()
{
  this->~XQDynamicContextImpl();
  _createdWith->deallocate(this);
}

DynamicContext *XQDynamicContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  if(memMgr == 0) {
    return 0;
  }
  return _staticContext->createDynamicContext(memMgr);
}

void XQDynamicContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
  _memMgr = memMgr;
}

void XQDynamicContextImpl::setDebugCallback(XQDebugCallback* callback)
{
    m_pDebugCallback=callback;
}

XQDebugCallback* XQDynamicContextImpl::getDebugCallback() const
{
    return m_pDebugCallback;
}

void XQDynamicContextImpl::enableDebugging(bool enable/*=true*/)
{
    m_bEnableDebugging=enable;
}

bool XQDynamicContextImpl::isDebuggingEnabled() const
{
    return m_bEnableDebugging;
}

void XQDynamicContextImpl::clearDynamicContext()
{
  _nsResolver = _staticContext->getNSResolver();
  _contextItem = 0;
  _contextSize = 1;
  _contextPosition = 1;
  _varStore->clear();
  _implicitTimezone = 0;
  _resolvers.clear();
  _docCache->clearStoredDocuments();
  time(&_currentTime);
}

const DocumentCache *XQDynamicContextImpl::getDocumentCache() const
{
  return _docCache;
}

void XQDynamicContextImpl::setContextItem(const Item::Ptr &item)
{
  _contextItem = item;
}

void XQDynamicContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  // bump the document reference count, so that it will never reach zero...
  _docCache->incrementDocumentRefCount(XPath2Utils::getOwnerDoc(node));
  setContextItem(new NodeImpl(node, this));
}

void XQDynamicContextImpl::setContextSize(unsigned int size)
{
  _contextSize = size;
}

void XQDynamicContextImpl::setContextPosition(unsigned int pos)
{
  _contextPosition = pos;
}

const Item::Ptr &XQDynamicContextImpl::getContextItem() const
{
  return _contextItem;
}

unsigned int XQDynamicContextImpl::getContextSize() const
{
  return _contextSize;
}

unsigned int XQDynamicContextImpl::getContextPosition() const
{
  return _contextPosition;
}

VariableStore* XQDynamicContextImpl::getVariableStore()
{
  return _varStore;
}

time_t XQDynamicContextImpl::getCurrentTime() const
{
  return _currentTime;
}

void XQDynamicContextImpl::setCurrentTime(time_t newTime)
{
  _currentTime=newTime;
}

const ATDurationOrDerived::Ptr &XQDynamicContextImpl::getImplicitTimezone() const {
  if(_implicitTimezone == NULLRCP) {
    // validate tzone
    Timezone tzone(ContextUtils::getTimezone());
    // c-style the const away since we are in a const method
    ((XQDynamicContextImpl*)this)->_implicitTimezone = tzone.asDayTimeDuration(this);
}

  return _implicitTimezone;
}

void XQDynamicContextImpl::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) {
  // validate the timezone -- will trow if invalid
  Timezone timezone(timezoneAsDuration, this);
  this->_implicitTimezone = timezoneAsDuration;
}

XPath2MemoryManager* XQDynamicContextImpl::getMemoryManager() const
{
  return _memMgr;
}

XERCES_CPP_NAMESPACE::DOMDocument *XQDynamicContextImpl::createNewDocument() const
{
  return XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation()->createDocument(getMemoryManager());
}

void XQDynamicContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
  doc->release();
}

void XQDynamicContextImpl::registerURIResolver(URIResolver *resolver)
{
  if(resolver != 0) {
    _resolvers.push_back(resolver);
  }
}

Sequence XQDynamicContextImpl::resolveDocument(const XMLCh* uri)
{
  bool found = false;
  Sequence result(getMemoryManager());
  std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
  for(std::vector<URIResolver *, PathanAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin(); i != end; ++i) {
    if((*i)->resolveDocument(result, uri, this)) {
      found = true;
      break;
    }
  }

  if(!found) {
    Node::Ptr doc;
    try {
      doc = _docCache->loadXMLDocument(uri, this);
    }
    catch(const XMLParseException&) {
      doc = 0;
    }

    if(doc != NULLRCP) {
      result.addItem(doc);
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      DSLthrow(XMLParseException,X("XQDynamicContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XQDynamicContextImpl::resolveCollection(const XMLCh* uri)
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
    Node::Ptr doc;
    try {
      doc = _docCache->loadXMLDocument(uri, this);
    }
    catch(const XMLParseException&) {
      doc = 0;
    }

    if(doc != NULLRCP) {
      result.addItem(doc);
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      DSLthrow(XMLParseException,X("XQDynamicContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XQDynamicContextImpl::resolveDefaultCollection()
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

Node::Ptr XQDynamicContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
  return _docCache->validate(node, valMode, this);
}

PathanFactory *XQDynamicContextImpl::getPathanFactory() const
{
  return _pathanFactory;
}

void XQDynamicContextImpl::setPathanFactory(PathanFactory *factory)
{
  _pathanFactory = factory;
}

void XQDynamicContextImpl::trace(const XMLCh* message1, const XMLCh* message2) {
    char* msg1=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message1);
    char* msg2=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message2);
    std::cerr << msg1 << " " << msg2 << std::endl; 
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg1);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg2);
}

void XQDynamicContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
	((PathanNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void XQDynamicContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
  _nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* XQDynamicContextImpl::getNSResolver() const {
  return _nsResolver;
}

const XMLCh* XQDynamicContextImpl::getUriBoundToPrefix(const XMLCh* prefix) const
{
  const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\'"), getMemoryManager());
		DSLthrow(NamespaceLookupException, X("XQDynamicContextImpl::getUriBoundToPrefix"), msg);
	}

	return uri;
}

const XMLCh* XQDynamicContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
  return _nsResolver->lookupPrefix(uri);
}

void XQDynamicContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _docCache->setXMLEntityResolver(handler);
}

void XQDynamicContextImpl::setDefaultCollation(const XMLCh* URI)
{
  _defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* XQDynamicContextImpl::getDefaultCollation() const
{
  if(_defaultCollation == 0) {
    return _staticContext->getDefaultCollation();
  }
  else {
    return getCollation(_defaultCollation);
  }
}

StaticContext::NodeSetOrdering XQDynamicContextImpl::getNodeSetOrdering() const
{
    return _ordering;
}

void XQDynamicContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{
    _ordering=newOrder;
}
