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
 * $Id: XQContextImpl.cpp,v 1.29 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQContextImpl.cpp: implementation of the XQContextImpl class.
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLURL.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/context/impl/ItemFactoryImpl.hpp>
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/URIResolver.hpp>
#include <xqilla/context/ModuleResolver.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/ContextUtils.hpp>
#include <xqilla/exceptions/ContextException.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/TypeNotFoundException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/items/DatatypeLookup.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/impl/NodeImpl.hpp>
#include <xqilla/functions/FunctionLookup.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>
#include <xqilla/dom-api/impl/XQillaNSResolverImpl.hpp>
#include "../../dom-api/impl/XPathDocumentImpl.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

const XMLCh XMLChXS[]    = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChXSI[]   = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChFN[]    = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChXDT[]   = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChLOCAL[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static CodepointCollation g_codepointCollation;

XQContextImpl::XQContextImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                             XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr,
                             XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode)
  : _createdWith(memMgr),
    _internalMM(memMgr),
    _varTypeStore(0),
    _functionTable(0),
    _collations(XQillaAllocator<Collation*>(&_internalMM)),
    _constructionMode(CONSTRUCTION_MODE_PRESERVE),
    _bPreserveBoundarySpace(false),
    _varStore(0),
    _documentMap(3,false,&_internalMM),
    _uriMap(3,false, new (&_internalMM) XERCES_CPP_NAMESPACE_QUALIFIER HashPtr(), &_internalMM),
    _resolvers(XQillaAllocator<URIResolver*>(&_internalMM)),
    _moduleResolver(0)
{
  _memMgr = &_internalMM;
  _firstDocRefCount=new (&_internalMM) DocRefCount();

  ////////////////////
  // static context //
  ////////////////////
  _contextItemType.flags = StaticType::ITEM_TYPE;
  _xpath1Compatibility = false;    // according to Formal Semantics, ?4.1.1
  _ordering = ORDERING_ORDERED;

  _globalNSResolver = new (&_internalMM) XQillaNSResolverImpl(&_internalMM, 0); // resolve acc.to null node
  _nsResolver = _globalNSResolver;

  _defaultElementNS = 0;
  // by default, the default namespace for functions is the XPath2 namespace
  _functionNS = XQFunction::XMLChFunctionURI;

  if(xmlgr)
    _docCache=new (&_internalMM) DocumentCacheImpl(&_internalMM, xmlgr);
  else 
    _docCache=new (&_internalMM) DocumentCacheImpl(&_internalMM);

  if(_varStore==NULL)
    _varStore=_internalMM.createVariableStore();
  if(_varTypeStore==NULL)
    _varTypeStore=_internalMM.createVariableTypeStore();
  if(_functionTable==NULL)
    _functionTable=_internalMM.createFunctionTable();

  _itemFactory = new (&_internalMM) ItemFactoryImpl(_docCache, &_internalMM);

  // insert the default collation
  addCollation(_internalMM.createCollation(&g_codepointCollation));
  setDefaultCollation(g_codepointCollation.getCollationName());

  _flworOrderingMode = FLWOR_ORDER_EMPTY_LEAST; // implementation-defined

  _bInheritNamespaces = true;
  _bPreserveNamespaces = true;

  if(contextNode==0)
		_baseURI=0;
  else {
    if(contextNode->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::DOCUMENT_NODE) {
      _baseURI = _internalMM.getPooledString(contextNode->getBaseURI());
    } else {
      _baseURI = _internalMM.getPooledString(contextNode->getOwnerDocument()->getBaseURI());
    }
  }

  ////////////////////////
  // Evaluation context //
  ////////////////////////

  if(contextNode!=NULL)
    setExternalContextNode(contextNode);
  else
    _contextItem = NULL;

  _contextPosition = 1;
  _contextSize = 1;
  time(&_currentTime);

  // implicit timezone - lazily set in getImplicitTimezone
  _implicitTimezone = 0;

  // XQuery stuff
  m_pDebugCallback = NULL;
  m_bEnableDebugging = false;

  // XQuery defines these predefined namespace bindings
  setNamespaceBinding(XMLChXS, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
  setNamespaceBinding(XMLChXSI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_XSI);
  setNamespaceBinding(XMLChFN, XQFunction::XMLChFunctionURI);
  setNamespaceBinding(XMLChXDT, FunctionConstructor::XMLChXPath2DatatypesURI);
  setNamespaceBinding(XMLChLOCAL, XQUserFunction::XMLChXQueryLocalFunctionsURI);
}

XQContextImpl::~XQContextImpl()
{
  _varStore->clear();
  _varTypeStore->clear();
  _contextItem = 0;
  _implicitTimezone = 0;

  delete _varStore;
  delete _varTypeStore;
  delete _itemFactory;

  DocRefCount *drc;
  while(_firstDocRefCount != 0) {
    drc = _firstDocRefCount;
    _firstDocRefCount = _firstDocRefCount->next;
    _internalMM.deallocate(drc);
  }
}

DynamicContext *XQContextImpl::createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  DynamicContext* moduleCtx = new (memMgr) XQContextImpl(memMgr);
  // force the context to use our memory manager
  moduleCtx->setMemoryManager(getMemoryManager());
  // propagate debug settings
  moduleCtx->enableDebugging(isDebuggingEnabled());
  return moduleCtx;
}

DynamicContext *XQContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  return new (memMgr) XQDynamicContextImpl(this, memMgr);
}

void XQContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
  _memMgr = memMgr;
}

void XQContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
    if(XPath2Utils::equals(prefix,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) || 
       XPath2Utils::equals(prefix,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString))
      XQThrow2(StaticErrorException,X("XQContextImpl::setNamespaceBinding"),X("The prefixes 'xmlns' and 'xml' cannot be used in a namespace declaration [err:XQST0070]"));
    if(XPath2Utils::equals(uri,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName))
      XQThrow2(StaticErrorException,X("XQContextImpl::setNamespaceBinding"),X("The 'http://www.w3.org/XML/1998/namespace' namespace cannot be bound to any prefix [err:XQST0070]"));

	((XQillaNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void XQContextImpl::setPreserveBoundarySpace(bool value)
{
	_bPreserveBoundarySpace=value;
}

bool XQContextImpl::getPreserveBoundarySpace() const
{
	return _bPreserveBoundarySpace;
}

void XQContextImpl::setInheritNamespaces(bool value)
{
    _bInheritNamespaces=value;
}

bool XQContextImpl::getInheritNamespaces() const
{
    return _bInheritNamespaces;
}

void XQContextImpl::setPreserveNamespaces(bool value)
{
    _bPreserveNamespaces=value;
}

bool XQContextImpl::getPreserveNamespaces() const
{
    return _bPreserveNamespaces;
}

void XQContextImpl::setDebugCallback(XQDebugCallback* callback)
{
    m_pDebugCallback=callback;
}

XQDebugCallback* XQContextImpl::getDebugCallback() const
{
    return m_pDebugCallback;
}

void XQContextImpl::enableDebugging(bool enable/*=true*/)
{
    m_bEnableDebugging=enable;
}

bool XQContextImpl::isDebuggingEnabled() const
{
    return m_bEnableDebugging;
}

void XQContextImpl::incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const
{
  assert(document != 0);

  XQContextImpl* me=const_cast<XQContextImpl*>(this);

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

void XQContextImpl::decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const
{
  assert(document != 0);

  XQContextImpl* me=const_cast<XQContextImpl*>(this);

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

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XQContextImpl::retrieveDocument(const XMLCh* uri)
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = _documentMap.get((void*)uri);
  return doc;
}

void XQContextImpl::storeDocument(const XMLCh* uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document)
{
  uri=_internalMM.getPooledString(uri);
  _documentMap.put((void*)uri, document);
  _uriMap.put((void*)document, const_cast<XMLCh*>(uri));
}

void XQContextImpl::clearDynamicContext()
{
  _nsResolver = _globalNSResolver;
  _contextItem = 0;
  _contextSize = 1;
  _contextPosition = 1;
  _varStore->clear();
  _implicitTimezone = 0;
  _resolvers.clear();
  _moduleResolver = 0;
  _documentMap.removeAll();
  _uriMap.removeAll();
  time(&_currentTime);
}

const StaticType &XQContextImpl::getContextItemType() const
{
  return _contextItemType;
}

void XQContextImpl::setContextItemType(const StaticType &st)
{
  _contextItemType = st;
}

bool XQContextImpl::getXPath1CompatibilityMode() const
{
  return _xpath1Compatibility;
}

void XQContextImpl::setXPath1CompatibilityMode(bool newMode)
{
  _xpath1Compatibility=newMode;
}

const XMLCh* XQContextImpl::getDefaultFuncNS() const
{
  return _functionNS;
}

void XQContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{
  _functionNS=newNS;
}

const XMLCh* XQContextImpl::getBaseURI() const
{
  return _baseURI;
}

void XQContextImpl::setBaseURI(const XMLCh* newURI)
{
	_baseURI = _internalMM.getPooledString(newURI);
}

StaticContext::ConstructionMode XQContextImpl::getConstructionMode() const
{
    return _constructionMode;
}

void XQContextImpl::setConstructionMode(ConstructionMode newMode)
{
    _constructionMode=newMode;
}

StaticContext::NodeSetOrdering XQContextImpl::getNodeSetOrdering() const
{
    return _ordering;
}

void XQContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{
    _ordering=newOrder;
}

StaticContext::FLWOROrderingMode XQContextImpl::getDefaultFLWOROrderingMode() const
{
    return _flworOrderingMode;
}

void XQContextImpl::setDefaultFLWOROrderingMode(StaticContext::FLWOROrderingMode newMode)
{
    _flworOrderingMode=newMode;
}

time_t XQContextImpl::getCurrentTime() const
{
  return _currentTime;
}

void XQContextImpl::setCurrentTime(time_t newTime)
{
  _currentTime=newTime;
}

ATDurationOrDerived::Ptr XQContextImpl::getImplicitTimezone() const {
	if(_implicitTimezone == NULLRCP) {
		// validate tzone
		Timezone tzone(ContextUtils::getTimezone());
    // c-style the const away since we are in a const method
    ((XQContextImpl*)this)->_implicitTimezone = tzone.asDayTimeDuration(this);
	}

	return _implicitTimezone;
}

void XQContextImpl::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) {
  // validate the timezone -- will trow if invalid
  Timezone timezone(timezoneAsDuration, this);
  this->_implicitTimezone = timezoneAsDuration;
}

void XQContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
  _nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* XQContextImpl::getNSResolver() const {
  return _nsResolver;
}

const XMLCh* XQContextImpl::getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const
{
  const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\' [err:XPST0081]"), getMemoryManager());
		XQThrow3(NamespaceLookupException, X("XQContextImpl::getUriBoundToPrefix"), msg, location);
	}

	return uri;
}

const XMLCh* XQContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
  return _nsResolver->lookupPrefix(uri);
}

void XQContextImpl::setContextItem(const Item::Ptr &item)
{
  _contextItem = item;
}

void XQContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  // bump the document reference count, so that it will never reach zero...
  incrementDocumentRefCount(XPath2Utils::getOwnerDoc(node));
  setContextItem(new NodeImpl(node, this));
}

void XQContextImpl::addCustomFunction(FuncFactory *func)
{
  _functionTable->insertFunction(func);
}

void XQContextImpl::setContextSize(size_t size)
{
  _contextSize = size;
}

void XQContextImpl::setContextPosition(size_t pos)
{
  _contextPosition = pos;
}

Item::Ptr XQContextImpl::getContextItem() const
{
  return _contextItem;
}

VariableStore* XQContextImpl::getVariableStore()
{
  return _varStore;
}

VariableTypeStore* XQContextImpl::getVariableTypeStore()
{
  return _varTypeStore;
}

size_t XQContextImpl::getContextSize() const
{
  return _contextSize;
}

size_t XQContextImpl::getContextPosition() const
{
  return _contextPosition;
}

XPath2MemoryManager* XQContextImpl::getMemoryManager() const
{
  return _memMgr;
}

const XMLCh* XQContextImpl::getDefaultElementAndTypeNS() const
{
  return _defaultElementNS;
}

void XQContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS) {
  _defaultElementNS = newNS;
}

void XQContextImpl::addCollation(Collation* collation)
{
  _collations.push_back(collation);
}

void XQContextImpl::setDefaultCollation(const XMLCh* URI)
{
  _defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* XQContextImpl::getCollation(const XMLCh* URI, const LocationInfo *location) const
{
  if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLUri::isValidURI(false, URI))
  {
    const XMLCh* baseURI=getBaseURI();
    if(baseURI && *baseURI)
    {
      try
      {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLUri base(baseURI, getMemoryManager());
        XERCES_CPP_NAMESPACE_QUALIFIER XMLUri full(&base, URI, getMemoryManager());
        URI = getMemoryManager()->getPooledString(full.getUriText());
      }
      catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e)
      {
        //if can't build, assume it's because there was an invalid base URI, so use the original URI
      }
    }
  }
  for(std::vector<Collation*, XQillaAllocator<Collation*> >::const_iterator it= _collations.begin(); it!=_collations.end(); ++it)
    if(XPath2Utils::equals((*it)->getCollationName(), URI))
      return (*it);
  const XMLCh* msg = XPath2Utils::concatStrings(X("The requested collation ('"), URI, X("') is not defined [err:FOCH0002]"), getMemoryManager());

  XQThrow3(ContextException, X("XQContextImpl::getCollation"), msg, location);
  return NULL;
}

Collation* XQContextImpl::getDefaultCollation(const LocationInfo *location) const
{
  return getCollation(_defaultCollation, location);
}

ASTNode* XQContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v,
                                       const LocationInfo *location) const
{
	const XMLCh* uri;

	//look at default namespace
	if(prefix == 0) {
		uri = getDefaultFuncNS();
	}
	else {
		uri = getUriBoundToPrefix(prefix, location);

		//not bound to anything - error
		if(uri == 0) {
			const XMLCh* msg = XPath2Utils::concatStrings(X("The prefix '"), prefix , X("' is not bound to a uri in the current context"), getMemoryManager());
			XQThrow3(NamespaceLookupException, X("XQContextImpl::lookUpFunction"), msg, location);
		}
	}

	ASTNode* functionImpl=_functionTable->lookUpFunction(uri, name, v, getMemoryManager());

  if(functionImpl == NULL && v.size() == 1) {
    // maybe it's not a function, but a datatype
    try {
      bool isPrimitive;
      _itemFactory->getPrimitiveTypeIndex(uri, name, isPrimitive);
      functionImpl = new (getMemoryManager())
        FunctionConstructor(uri, name, v, getMemoryManager());
    }
    catch(TypeNotFoundException&) {
      // ignore this exception: it means the type has not been found
    }
  }
  return functionImpl;
}

XERCES_CPP_NAMESPACE::DOMDocument *XQContextImpl::createNewDocument() const
{
  return new (getMemoryManager()) XPathDocumentImpl(getMemoryManager());
}

void XQContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
  doc->release();
}

void XQContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _docCache->setXMLEntityResolver(handler);
}

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* XQContextImpl::getXMLEntityResolver() const
{
  return _docCache->getXMLEntityResolver();
}

void XQContextImpl::registerURIResolver(URIResolver *resolver)
{
  if(resolver != 0) {
    _resolvers.push_back(resolver);
  }
}

Sequence XQContextImpl::resolveDocument(const XMLCh* uri, const LocationInfo *location)
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
        XQThrow3(XMLParseException,X("XQContextImpl::resolveDocument"), errMsg.getRawBuffer(), location);
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
      XQThrow3(XMLParseException,X("XQContextImpl::resolveDocument"), errMsg.getRawBuffer(), location);
    }
  }

  return result;
}

Sequence XQContextImpl::resolveCollection(const XMLCh* uri, const LocationInfo *location)
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
      XQThrow3(XMLParseException,X("XQContextImpl::resolveCollection"), errMsg.getRawBuffer(), location);
    }
  }

  return result;
}

Sequence XQContextImpl::resolveDefaultCollection()
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

void XQContextImpl::setModuleResolver(ModuleResolver *resolver)
{
  _moduleResolver=resolver;
}

ModuleResolver * XQContextImpl::getModuleResolver() const
{
  return _moduleResolver;
}

VectorOfStrings* XQContextImpl::resolveModuleURI(const XMLCh* uri) const
{
  VectorOfStrings* vect=new (getMemoryManager()) VectorOfStrings(XQillaAllocator<const XMLCh*>(getMemoryManager()));
  _moduleResolver->resolveModuleLocation(vect, uri, this);
  return vect;
}

/*
 * returns true if the type represented by uri:typename is an instance of uriToCheck:typeNameToCheck
 *
 * ie: to check
 * xs:integer instance of xs:decimal,
 * call
 * isTypeOrDerivedFromType("xs", "integer", "xs", "decimal")
 * (except of course, call with URIs, not prefixes!)
 */
bool XQContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const
{
	return _docCache->isTypeOrDerivedFromType(uri,typeName,uriToCheck,typeNameToCheck);
}

void XQContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{
	_docCache->addSchemaLocation(uri, locations, this);
}

const DocumentCache* XQContextImpl::getDocumentCache() const
{
	return _docCache;
}

void XQContextImpl::setDocumentCache(DocumentCache* docCache)
{
    _docCache = docCache;
}

Node::Ptr XQContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
  return _docCache->validate(node, valMode, this);
}

ItemFactory *XQContextImpl::getItemFactory() const
{
  return _itemFactory;
}

void XQContextImpl::setItemFactory(ItemFactory *factory)
{
  _itemFactory = factory;
}

void XQContextImpl::trace(const XMLCh* message1, const XMLCh* message2) {
    char* msg1=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message1);
    char* msg2=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message2);
    std::cerr << msg1 << " " << msg2 << std::endl;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg1);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg2);
}
