/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XQDynamicContextImpl.cpp,v 1.26 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQContext.cpp: implementation of the XQContext class.
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include <xercesc/util/XMLURL.hpp>

#include <xqilla/context/impl/XQDynamicContextImpl.hpp>
#include <xqilla/context/impl/ItemFactoryImpl.hpp>

#include <xqilla/context/VariableStore.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/ContextException.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/TypeNotFoundException.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/items/DatatypeLookup.hpp>
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>
#include <xqilla/dom-api/impl/XQillaNSResolverImpl.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/URIResolver.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/utils/ContextUtils.hpp>
#include <xqilla/items/impl/NodeImpl.hpp>
#include "../../dom-api/impl/XPathDocumentImpl.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQDynamicContextImpl::XQDynamicContextImpl(const StaticContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _staticContext(staticContext),
    _createdWith(memMgr),
    _internalMM(memMgr),
    _nsResolver(staticContext->getNSResolver()),
    _defaultCollation(0),
    _defaultElementNS(staticContext->getDefaultElementAndTypeNS()),
    _contextItem(0),
    _contextPosition(1),
    _contextSize(1),
    _implicitTimezone(0),
    _resolvers(XQillaAllocator<URIResolver*>(&_internalMM)),
    _docCache(staticContext->getDocumentCache()->createDerivedCache(&_internalMM)),
    _documentMap(3,false,&_internalMM),
    _uriMap(3,false, new (&_internalMM) XERCES_CPP_NAMESPACE_QUALIFIER HashPtr(), &_internalMM)
{
  time(&_currentTime);
  _memMgr = &_internalMM;
  _firstDocRefCount=new (&_internalMM) DocRefCount();
  _varStore = _internalMM.createVariableStore();
  _itemFactory = new (&_internalMM) ItemFactoryImpl(_docCache, &_internalMM);

  m_pDebugCallback = NULL;
  m_bEnableDebugging = false;
}

XQDynamicContextImpl::~XQDynamicContextImpl()
{
  _contextItem = 0;
  _implicitTimezone = 0;

  delete _varStore;
  delete _itemFactory;

  DocRefCount *drc;
  while(_firstDocRefCount != 0) {
    drc = _firstDocRefCount;
    _firstDocRefCount = _firstDocRefCount->next;
    _internalMM.deallocate(drc);
  }
}

DynamicContext *XQDynamicContextImpl::createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  return _staticContext->createModuleContext(memMgr);
}

DynamicContext *XQDynamicContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
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

void XQDynamicContextImpl::incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const
{
  assert(document != 0);

  XQDynamicContextImpl* me=const_cast<XQDynamicContextImpl*>(this);

  DocRefCount *found = _firstDocRefCount;
  while(found->doc != 0 && found->doc != document) {
    found = found->next;
  }

  if(found->doc == 0) {
    found->doc = document;
    found->next = new (&me->_internalMM) DocRefCount();
  }
  else {
    ++found->ref_count;
  }
}

void XQDynamicContextImpl::decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const
{
  assert(document != 0);

  XQDynamicContextImpl* me=const_cast<XQDynamicContextImpl*>(this);

  DocRefCount *prev = 0;
  DocRefCount *found = _firstDocRefCount;
  while(found->doc != 0 && found->doc != document) {
    prev = found;
    found = found->next;
  }

  if(found->doc != 0) {
    if(--found->ref_count == 0) {
      if(prev == 0) {
        me->_firstDocRefCount = found->next;
      }
      else {
        prev->next = found->next;
      }
      me->_internalMM.deallocate(found);
      XMLCh *uri = me->_uriMap.get((void*)document);
      if(uri != 0) {
        me->_uriMap.removeKey((void*)document);
        me->_documentMap.removeKey((void*)uri);
      }
      releaseDocument(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*>(document));
    }
  }
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XQDynamicContextImpl::retrieveDocument(const XMLCh* uri)
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = _documentMap.get((void*)uri);
  return doc;
}

void XQDynamicContextImpl::storeDocument(const XMLCh* uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document)
{
  uri=_internalMM.getPooledString(uri);
  _documentMap.put((void*)uri, document);
  _uriMap.put((void*)document, const_cast<XMLCh*>(uri));
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
  _documentMap.removeAll();
  _uriMap.removeAll();
  time(&_currentTime);
}

const DocumentCache *XQDynamicContextImpl::getDocumentCache() const
{
  return _docCache;
}

void XQDynamicContextImpl::setDocumentCache(DocumentCache* docCache)
{
  _docCache=docCache;
}

void XQDynamicContextImpl::setContextItem(const Item::Ptr &item)
{
  _contextItem = item;
}

void XQDynamicContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  // bump the document reference count, so that it will never reach zero...
  incrementDocumentRefCount(XPath2Utils::getOwnerDoc(node));
  setContextItem(new NodeImpl(node, this));
}

void XQDynamicContextImpl::setContextSize(size_t size)
{
  _contextSize = size;
}

void XQDynamicContextImpl::setContextPosition(size_t pos)
{
  _contextPosition = pos;
}

Item::Ptr XQDynamicContextImpl::getContextItem() const
{
  return _contextItem;
}

size_t XQDynamicContextImpl::getContextSize() const
{
  return _contextSize;
}

size_t XQDynamicContextImpl::getContextPosition() const
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

ATDurationOrDerived::Ptr XQDynamicContextImpl::getImplicitTimezone() const {
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

const XMLCh* XQDynamicContextImpl::getDefaultElementAndTypeNS() const
{
  return _defaultElementNS;
}

void XQDynamicContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS) {
  _defaultElementNS = newNS;
}

XERCES_CPP_NAMESPACE::DOMDocument *XQDynamicContextImpl::createNewDocument() const
{
  return new (getMemoryManager()) XPathDocumentImpl(getMemoryManager());
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

Sequence XQDynamicContextImpl::resolveDocument(const XMLCh* uri, const LocationInfo *location)
{
  Sequence result(getMemoryManager());

  // Check the URIResolver objects
  bool found = false;
  std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
  for(std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin();
      !found && i != end; ++i) {
    found = (*i)->resolveDocument(result, uri, this);
  }

  if(!found) {
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    // Resolve the uri against the base uri
    const XMLCh *systemId = uri;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(&_internalMM);
    if(urlTmp.setURL(getBaseURI(), uri, urlTmp)) {
      systemId = _internalMM.getPooledString(urlTmp.getURLText());
    }

    // Check in the cache
    doc = retrieveDocument(systemId);

    // Check to see if we can locate and parse the document
    if(!doc) {
      try {
        doc = _docCache->loadXMLDocument(uri, this);
        storeDocument(systemId, doc);
      }
      catch(const XMLParseException& e) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
        errMsg.set(X("Error parsing resource: "));
        errMsg.append(uri);
        errMsg.append(X(". Error message: "));
        errMsg.append(e.getError());
        errMsg.append(X(" [err:FODC0002]"));
        XQThrow3(XMLParseException,X("XQDynamicContextImpl::resolveDocument"), errMsg.getRawBuffer(), location);
      }
    }

    if(doc) {
      result.addItem(new NodeImpl(doc, this));
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      errMsg.append(X(" [err:FODC0002]"));
      XQThrow3(XMLParseException,X("XQDynamicContextImpl::resolveDocument"), errMsg.getRawBuffer(), location);
    }
  }

  return result;
}

Sequence XQDynamicContextImpl::resolveCollection(const XMLCh* uri, const LocationInfo *location)
{
  Sequence result(getMemoryManager());

  // Check the URIResolver objects
  bool found = false;
  std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator end = _resolvers.rend();
  for(std::vector<URIResolver *, XQillaAllocator<URIResolver*> >::reverse_iterator i = _resolvers.rbegin();
      !found && i != end; ++i) {
    found = (*i)->resolveCollection(result, uri, this);
  }

  if(!found) {
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    // Resolve the uri against the base uri
    const XMLCh *systemId = uri;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(&_internalMM);
    if(urlTmp.setURL(getBaseURI(), uri, urlTmp)) {
      systemId = _internalMM.getPooledString(urlTmp.getURLText());
    }

    // Check in the cache
    doc = retrieveDocument(systemId);

    // Check to see if we can locate and parse the document
    if(!doc) {
      try {
        doc = _docCache->loadXMLDocument(uri, this);
        storeDocument(systemId, doc);
      }
      catch(const XMLParseException& e) {
        doc = 0;
      }
    }

    if(doc) {
      result.addItem(new NodeImpl(doc, this));
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      errMsg.append(X(" [err:FODC0004]"));
      XQThrow3(XMLParseException,X("XQDynamicContextImpl::resolveCollection"), errMsg.getRawBuffer(), location);
    }
  }

  return result;
}

Sequence XQDynamicContextImpl::resolveDefaultCollection()
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

VectorOfStrings* XQDynamicContextImpl::resolveModuleURI(const XMLCh* uri) const
{
  return _staticContext->resolveModuleURI(uri);
}

Node::Ptr XQDynamicContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
  return _docCache->validate(node, valMode, this);
}

ItemFactory *XQDynamicContextImpl::getItemFactory() const
{
  return _itemFactory;
}

void XQDynamicContextImpl::setItemFactory(ItemFactory *factory)
{
  _itemFactory = factory;
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
	((XQillaNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void XQDynamicContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
  _nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* XQDynamicContextImpl::getNSResolver() const {
  return _nsResolver;
}

const XMLCh* XQDynamicContextImpl::getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const
{
  const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\' [err:XPST0081]"), getMemoryManager());
		XQThrow3(NamespaceLookupException, X("XQDynamicContextImpl::getUriBoundToPrefix"), msg, location);
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

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* XQDynamicContextImpl::getXMLEntityResolver() const
{
  return _docCache->getXMLEntityResolver();
}

void XQDynamicContextImpl::setDefaultCollation(const XMLCh* URI)
{
  _defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* XQDynamicContextImpl::getDefaultCollation(const LocationInfo *location) const
{
  if(_defaultCollation == 0) {
    return _staticContext->getDefaultCollation(location);
  }
  else {
    return getCollation(_defaultCollation, location);
  }
}
