/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/context/XPath2ContextImpl.hpp>
#include <assert.h>
#include <iostream>
#include <memory>
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
#include <pathan/internal/factory/PathanFactoryImpl.hpp>
#include <pathan/functionAPI/FunctionLookup.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/internal/context/DocumentCacheImpl.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/URIResolver.hpp>
#include <pathan/exceptions/XMLParseException.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/internal/utils/ContextUtils.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/RuntimeException.hpp>

static CodepointCollation g_codepointCollation;

XPath2ContextImpl::XPath2ContextImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                                     XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr,
                                     XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode) :  
  _createdWith(memMgr),
  _internalMM(memMgr),
  _varTypeStore(0),
  _functionTable(0),
  _collations(PathanAllocator<Collation*>(&_internalMM)),
  _varStore(0),
  _resolvers(PathanAllocator<URIResolver*>(&_internalMM))
{
  _memMgr = &_internalMM;

  ////////////////////
  // static context //
  ////////////////////
  _contextItemType.flags = StaticType::NODE_TYPE | StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
  _xpath1Compatibility = false;
  _ordering = ORDERING_ORDERED;
    
  _globalNSResolver = new (&_internalMM) PathanNSResolverImpl(&_internalMM, 0); // resolve acc.to null node
  _nsResolver = _globalNSResolver;
    
  _defaultElementNS = 0;
  // by default, the default namespace for functions is the XPath2 namespace
  _functionNS = DataItemFunction::XMLChFunctionURI;
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

  _pathanFactory = new (&_internalMM) PathanFactoryImpl(_docCache, &_internalMM);

  // insert the default collation
  addCollation(_internalMM.createCollation(&g_codepointCollation));
  setDefaultCollation(g_codepointCollation.getCollationName());

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
}

XPath2ContextImpl::~XPath2ContextImpl()
{
  _contextItem = 0;
  _implicitTimezone = 0;
}

void XPath2ContextImpl::release()
{
  this->~XPath2ContextImpl();
  _createdWith->deallocate(this);
}

void XPath2ContextImpl::clearDynamicContext()
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

DynamicContext *XPath2ContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  if(memMgr == 0) {
    return 0;
  }
  return new (memMgr) DynamicContextImpl(this, memMgr);
} 

const StaticType &XPath2ContextImpl::getContextItemType() const
{
  return _contextItemType;
}

void XPath2ContextImpl::setContextItemType(const StaticType &st)
{
  _contextItemType = st;
}

bool XPath2ContextImpl::getXPath1CompatibilityMode() const
{
  return _xpath1Compatibility;
}

void XPath2ContextImpl::setXPath1CompatibilityMode(bool newMode)
{
  _xpath1Compatibility=newMode;
}

const XMLCh* XPath2ContextImpl::getDefaultFuncNS() const
{
  return _functionNS;
}

void XPath2ContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{
  _functionNS=newNS;
}

const XMLCh* XPath2ContextImpl::getBaseURI() const
{
  return _baseURI;
}

void XPath2ContextImpl::setBaseURI(const XMLCh* newURI)
{
	_baseURI = _internalMM.getPooledString(newURI);
}

StaticContext::NodeSetOrdering XPath2ContextImpl::getNodeSetOrdering() const
{
    return _ordering;
}

void XPath2ContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{
    _ordering=newOrder;
}

time_t XPath2ContextImpl::getCurrentTime() const
{
  return _currentTime;
}

void XPath2ContextImpl::setCurrentTime(time_t newTime)
{
  _currentTime=newTime;
}

const ATDurationOrDerived::Ptr &XPath2ContextImpl::getImplicitTimezone() const {
	if(_implicitTimezone == (const ATDurationOrDerived*)0) {
		// validate tzone
		Timezone tzone(ContextUtils::getTimezone());
		// c-style the const away since we are in a const method
		((XPath2ContextImpl*)this)->_implicitTimezone =
			tzone.asDayTimeDuration(this);
	}
	return _implicitTimezone;
}

void XPath2ContextImpl::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) {
  // validate the timezone -- will trow if invalid
  Timezone timezone(timezoneAsDuration, this);
  this->_implicitTimezone = timezoneAsDuration;
}

void XPath2ContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
  _nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* XPath2ContextImpl::getNSResolver() const {
  return _nsResolver;
}

const XMLCh* XPath2ContextImpl::getUriBoundToPrefix(const XMLCh* prefix) const
{
  const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\'"), getMemoryManager());
		DSLthrow(NamespaceLookupException, X("XPath2ContextImpl::getUriBoundToPrefix"), msg);
	}

	return uri;
}

const XMLCh* XPath2ContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
  return _nsResolver->lookupPrefix(uri);
}

void XPath2ContextImpl::setContextItem(const Item::Ptr &item)
{
  _contextItem = item;
}

void XPath2ContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  // bump the document reference count, so that it will never reach zero...
  getDocumentCache()->incrementDocumentRefCount(XPath2Utils::getOwnerDoc(node));
  setContextItem(new NodeImpl(node, this));
}

/** Set the memory manager to the one given */
void XPath2ContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
  _memMgr = memMgr;
}

void XPath2ContextImpl::addCustomFunction(FuncFactory *func)
{
  _functionTable->insertFunction(func);
}

void XPath2ContextImpl::setContextSize(unsigned int size)
{
  _contextSize = size;
}

void XPath2ContextImpl::setContextPosition(unsigned int pos)
{
  _contextPosition = pos;
}

const Item::Ptr &XPath2ContextImpl::getContextItem() const
{
  return _contextItem;
}

VariableStore* XPath2ContextImpl::getVariableStore()
{
  return _varStore;
}

VariableTypeStore* XPath2ContextImpl::getVariableTypeStore()
{
  return _varTypeStore;
}

unsigned int XPath2ContextImpl::getContextSize() const
{
  return _contextSize;
}

unsigned int XPath2ContextImpl::getContextPosition() const
{
  return _contextPosition;
}

XPath2MemoryManager* XPath2ContextImpl::getMemoryManager() const
{
  return _memMgr;
}

const XMLCh* XPath2ContextImpl::getDefaultElementAndTypeNS() const
{
  return _defaultElementNS;
}

void XPath2ContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS) {
  _defaultElementNS = newNS;
}

void XPath2ContextImpl::addCollation(Collation* collation)
{
  _collations.push_back(collation);
}

void XPath2ContextImpl::setDefaultCollation(const XMLCh* URI)
{
  _defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* XPath2ContextImpl::getCollation(const XMLCh* URI) const
{
  for(std::vector<Collation*, PathanAllocator<Collation*> >::const_iterator it= _collations.begin(); it!=_collations.end(); ++it)
    if(XPath2Utils::equals((*it)->getCollationName(), URI)) 
      return (*it);

  const XMLCh* msg = XPath2Utils::concatStrings(X("The requested collation ('"), URI, X("') is not defined [err:FOCH0002]"), getMemoryManager());
  
  DSLthrow(ContextException, X("XPath2ContextImpl::getCollation"), msg);
  return NULL;
}

Collation* XPath2ContextImpl::getDefaultCollation() const
{
  return getCollation(_defaultCollation);
}


DataItem* XPath2ContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfDataItems& v) const
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
			DSLthrow(NamespaceLookupException, X("XPath2ContextImpl::lookUpFunction"), msg);
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

XERCES_CPP_NAMESPACE::DOMDocument *XPath2ContextImpl::createNewDocument() const
{
  return XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation()->createDocument(getMemoryManager());
}

void XPath2ContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
  doc->release();
}

void XPath2ContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _docCache->setXMLEntityResolver(handler);
}

void XPath2ContextImpl::registerURIResolver(URIResolver *resolver)
{
  if(resolver != 0) {
    _resolvers.push_back(resolver);
  }
}

Sequence XPath2ContextImpl::resolveDocument(const XMLCh* uri)
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

    if(doc != (const Node *)0) {
      result.addItem(doc);
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      DSLthrow(XMLParseException,X("XPath2ContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XPath2ContextImpl::resolveCollection(const XMLCh* uri)
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

    if(doc != (const Node *)0) {
      result.addItem(doc);
    }
    else {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
      errMsg.set(X("Error retrieving resource: "));
      errMsg.append(uri);
      DSLthrow(XMLParseException,X("XPath2ContextImpl::resolveDocument"), errMsg.getRawBuffer());
    }
  }

  return result;
}

Sequence XPath2ContextImpl::resolveDefaultCollection()
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
bool XPath2ContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const
{
	return _docCache->isTypeOrDerivedFromType(uri,typeName,uriToCheck,typeNameToCheck);
}

void XPath2ContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{
	_docCache->addSchemaLocation(uri, locations, this);
}

const DocumentCache* XPath2ContextImpl::getDocumentCache() const
{
	return _docCache;
}

Node::Ptr XPath2ContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
  return _docCache->validate(node, valMode, this);
}

PathanFactory *XPath2ContextImpl::getPathanFactory() const
{
  return _pathanFactory;
}

void XPath2ContextImpl::setPathanFactory(PathanFactory *factory)
{
  _pathanFactory = factory;
}

void XPath2ContextImpl::trace(const XMLCh* message1, const XMLCh* message2) {
    char* msg1=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message1);
    char* msg2=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(message2);
    std::cerr << msg1 << " " << msg2 << std::endl; 
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg1);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&msg2);
}
