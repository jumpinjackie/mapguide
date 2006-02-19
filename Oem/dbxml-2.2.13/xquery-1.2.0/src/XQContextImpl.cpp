//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQContextImpl.cpp,v 1.31 2005/11/30 17:08:55 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQContextImpl.cpp: implementation of the XQContextImpl class.
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include "xquery/XQEngine.hpp"
#include "xquery/context/XQContextImpl.hpp"
#include "xquery/dataItem/XQDebugHook.hpp"
#include "xquery/context/XQDynamicContextImpl.hpp"
#include "xquery/functions/XQFunction.hpp"
#include "xquery/context/XQFactoryImpl.hpp"
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <pathan/Node.hpp>
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/internal/utils/ContextUtils.hpp>

#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
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
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

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

XQContextImpl::XQContextImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _createdWith(memMgr),
    _internalMM(memMgr),
    _varTypeStore(0),
    _functionTable(0),
    _collations(PathanAllocator<Collation*>(&_internalMM)),
    _constructionMode(CONSTRUCTION_MODE_PRESERVE),
    _bPreserveBoundarySpace(false),
    _varStore(0),
    _resolvers(PathanAllocator<URIResolver*>(&_internalMM))
{
  _memMgr = &_internalMM;

  ////////////////////
  // static context //
  ////////////////////
  _contextItemType.flags = StaticType::NODE_TYPE | StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
  _xpath1Compatibility = false;    // according to Formal Semantics, § 4.1.1
  _ordering = ORDERING_ORDERED;

  _globalNSResolver = new (&_internalMM) PathanNSResolverImpl(&_internalMM, 0); // resolve acc.to null node
  _nsResolver = _globalNSResolver;

  _defaultElementNS = 0;
  // by default, the default namespace for functions is the XPath2 namespace
  _functionNS = DataItemFunction::XMLChFunctionURI;
  _docCache=new (&_internalMM) DocumentCacheImpl(&_internalMM);
  if(_varStore==NULL)
    _varStore=_internalMM.createVariableStore();
  if(_varTypeStore==NULL)
    _varTypeStore=_internalMM.createVariableTypeStore();
  if(_functionTable==NULL)
    _functionTable=_internalMM.createFunctionTable();

  _pathanFactory = new (&_internalMM) XQFactoryImpl(_docCache, &_internalMM);

  // insert the default collation
  addCollation(_internalMM.createCollation(&g_codepointCollation));
  setDefaultCollation(g_codepointCollation.getCollationName());

  _baseURI=0;

  _flworOrderingMode = FLWOR_ORDER_EMPTY_LEAST; // implementation-defined

  _bInheritNamespaces = true;
  _bPreserveNamespaces = true;

  ////////////////////////
  // Evaluation context //
  ////////////////////////

  _contextItem = NULL;
  _contextPosition = 1;
  _contextSize = 1;
  time(&_currentTime);

  // implicit timezone - lazily set in getImplicitTimezone
  _implicitTimezone = 0;

  // XQuery stuff
  m_pDebugCallback = NULL;
  m_bEnableDebugging = false;

  // XQuery defines 6 predefined namespace bindings
  setNamespaceBinding(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName);
  setNamespaceBinding(XMLChXS, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
  setNamespaceBinding(XMLChXSI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_XSI);
  setNamespaceBinding(XMLChFN, DataItemFunction::XMLChFunctionURI);
  setNamespaceBinding(XMLChXDT, FunctionConstructor::XMLChXPath2DatatypesURI);
  setNamespaceBinding(XMLChLOCAL, XQFunction::XMLChXQueryLocalFunctionsURI);
}

XQContextImpl::~XQContextImpl()
{
  _varStore->clear();
  _varTypeStore->clear();
  _contextItem = 0;
  _implicitTimezone = 0;

  ((XQFactoryImpl*)_pathanFactory)->release();
}

void XQContextImpl::release()
{
  this->~XQContextImpl();
  _createdWith->deallocate(this);
}

DynamicContext *XQContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  if(memMgr == 0) {
    return 0;
  }
  return new (memMgr) XQDynamicContextImpl(this, memMgr);
}

void XQContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
  _memMgr = memMgr;
}

void XQContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
	((PathanNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
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

void XQContextImpl::clearDynamicContext()
{
  _nsResolver = _globalNSResolver;
  _contextItem = 0;
  _contextSize = 1;
  _contextPosition = 1;
  _varStore->clear();
  _implicitTimezone = 0;
  _resolvers.clear();
  _docCache->clearStoredDocuments();
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

XQStaticContext::ConstructionMode XQContextImpl::getConstructionMode() const
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

XQStaticContext::FLWOROrderingMode XQContextImpl::getDefaultFLWOROrderingMode() const
{
    return _flworOrderingMode;
}

void XQContextImpl::setDefaultFLWOROrderingMode(XQStaticContext::FLWOROrderingMode newMode)
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

const ATDurationOrDerived::Ptr &XQContextImpl::getImplicitTimezone() const {
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

const XMLCh* XQContextImpl::getUriBoundToPrefix(const XMLCh* prefix) const
{
  const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\'"), getMemoryManager());
		DSLthrow(NamespaceLookupException, X("XQContextImpl::getUriBoundToPrefix"), msg);
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
  getDocumentCache()->incrementDocumentRefCount(XPath2Utils::getOwnerDoc(node));
  setContextItem(new NodeImpl(node, this));
}

void XQContextImpl::addCustomFunction(FuncFactory *func)
{
  _functionTable->insertFunction(func);
}

void XQContextImpl::setContextSize(unsigned int size)
{
  _contextSize = size;
}

void XQContextImpl::setContextPosition(unsigned int pos)
{
  _contextPosition = pos;
}

const Item::Ptr &XQContextImpl::getContextItem() const
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

unsigned int XQContextImpl::getContextSize() const
{
  return _contextSize;
}

unsigned int XQContextImpl::getContextPosition() const
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

Collation* XQContextImpl::getCollation(const XMLCh* URI) const
{
  for(std::vector<Collation*, PathanAllocator<Collation*> >::const_iterator it= _collations.begin(); it!=_collations.end(); ++it)
    if(XPath2Utils::equals((*it)->getCollationName(), URI))
      return (*it);

  const XMLCh* msg = XPath2Utils::concatStrings(X("The requested collation ('"), URI, X("') is not defined [err:FOCH0002]"), getMemoryManager());

  DSLthrow(ContextException, X("XQContextImpl::getCollation"), msg);
  return NULL;
}

Collation* XQContextImpl::getDefaultCollation() const
{
  return getCollation(_defaultCollation);
}

DataItem* XQContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfDataItems& v) const
{
	const XMLCh* uri;

	//look at default namespace
	if(prefix == 0) {
		uri = getDefaultFuncNS();
	}
	else {
		uri = getUriBoundToPrefix(prefix);

		//not bound to anything - error
		if(uri == 0) {

			const XMLCh* msg = XPath2Utils::concatStrings(X("The prefix '"), prefix , X("' is not bound to a uri in the current context"), getMemoryManager());
			DSLthrow(NamespaceLookupException, X("XQContextImpl::lookUpFunction"), msg);
		}
	}

	DataItem* functionImpl=_functionTable->lookUpFunction(uri, name, v, getMemoryManager());

    if(functionImpl == NULL && v.size() == 1)
    {
        // maybe it's not a function, but a datatype
        try
        {
          functionImpl = new (getMemoryManager())
		  FunctionConstructor(uri, name, _pathanFactory->getPrimitiveTypeIndex(uri, name), v, getMemoryManager());
        }
        catch(TypeNotFoundException&)
        {
        // ignore this exception: it means the type has not been found
        }
    }
    return functionImpl;
}

XERCES_CPP_NAMESPACE::DOMDocument *XQContextImpl::createNewDocument() const
{
  return XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation()->createDocument(getMemoryManager());
}

void XQContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
  doc->release();
}

void XQContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _docCache->setXMLEntityResolver(handler);
}

void XQContextImpl::registerURIResolver(URIResolver *resolver)
{
  if(resolver != 0) {
    _resolvers.push_back(resolver);
  }
}

Sequence XQContextImpl::resolveDocument(const XMLCh* uri)
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
    catch(const XMLParseException& e) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error parsing resource: "));
      errMsg.append(uri);
      errMsg.append(X(". Error message: "));
      errMsg.append(e.getError());
      DSLthrow(XMLParseException,X("XQContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }

    if(doc != NULLRCP) {
      result.addItem(doc);
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      DSLthrow(XMLParseException,X("XQContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XQContextImpl::resolveCollection(const XMLCh* uri)
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
      DSLthrow(XMLParseException,X("XQContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XQContextImpl::resolveDefaultCollection()
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

Node::Ptr XQContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
  return _docCache->validate(node, valMode, this);
}

PathanFactory *XQContextImpl::getPathanFactory() const
{
  return _pathanFactory;
}

void XQContextImpl::setPathanFactory(PathanFactory *factory)
{
  _pathanFactory = factory;
}

void XQContextImpl::trace(const XMLCh* message1, const XMLCh* message2) {
    char* msg1=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message1);
    char* msg2=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message2);
    std::cerr << msg1 << " " << msg2 << std::endl;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg1);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg2);
}
