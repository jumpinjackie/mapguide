//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlContextImpl.cpp,v 1.35 2005/11/30 17:12:15 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <sstream>
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

#include <xquery/functions/XQFunction.hpp>

#include <dbxml/XmlException.hpp>

#include "DbXmlContextImpl.hpp"
#include "DbXmlDynamicContextImpl.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../QueryContext.hpp"
#include "../ReferenceMinder.hpp"
#include "../UTF8.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh XMLChXS[]    = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChXSI[]   = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChFN[]    = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChXDT[]   = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const XMLCh XMLChLOCAL[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static CodepointCollation g_codepointCollation;

static void _throwResolutionError(const XMLCh *uri, const char *resource)
{
	ostringstream oss;
	XMLChToUTF8 uuri(uri);
	oss << "Cannot resolve " << resource << ": " <<
		uuri.str();
	throw XmlException(XmlException::XPATH_EVALUATION_ERROR,
			   oss.str());
}

DbXmlContextImpl::DbXmlContextImpl(QueryContext &qc, Transaction *txn, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
	: DbXmlContext(qc, txn),
	  _createdWith(memMgr),
	  _internalMM(memMgr),
	  _varTypeStore(0),
	  _functionTable(0),
	  _collations(PathanAllocator<Collation*>(&_internalMM)),
	  _constructionMode(CONSTRUCTION_MODE_PRESERVE),
	  _ordering(StaticContext::ORDERING_ORDERED),
	  _bPreserveBoundarySpace(false),
	  _varStore(0),
	  _resolvers(PathanAllocator<URIResolver*>(&_internalMM))
{
	_memMgr = &_internalMM;

	////////////////////
	// static context //
	////////////////////
	_contextItemType.flags = StaticType::NODE_TYPE | StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
	_xpath1Compatibility = false;    // according to Formal Semantics, 4.1.1

	_globalNSResolver = new (&_internalMM) PathanNSResolverImpl(&_internalMM, 0); // resolve acc.to null node
	_nsResolver = _globalNSResolver;

	_defaultElementNS = 0;
	// by default, the default namespace for functions is the XPath2 namespace
	_functionNS = DataItemFunction::XMLChFunctionURI;

	// This is created with the _createdWith memory manager,
	// since a bug in xerces means we can't use a non-thread-safe
	// memory manager - jpcs
	_docCache=new (_createdWith) DocumentCacheImpl(_createdWith);

	if(_varStore==NULL)
		_varStore=_internalMM.createVariableStore();
	if(_varTypeStore==NULL)
		_varTypeStore=_internalMM.createVariableTypeStore();
	if(_functionTable==NULL)
		_functionTable=_internalMM.createFunctionTable();

	_pathanFactory = new (&_internalMM) DbXmlFactoryImpl(_docCache, &_internalMM);

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

	// DbXml specific
	registerURIResolver(&resolver_);
	setXMLEntityResolver(&resolver_);
}

DbXmlContextImpl::~DbXmlContextImpl()
{
	_varTypeStore->clear();
	_varStore->clear();
	_contextItem = 0;
	_implicitTimezone = 0;

	((XQFactory*)_pathanFactory)->release();
	_docCache->release();
}

void DbXmlContextImpl::release()
{
	this->~DbXmlContextImpl();
	_createdWith->deallocate(this);
}

DynamicContext *DbXmlContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
	if(memMgr == 0) {
		return 0;
	}
	return new (memMgr) DbXmlDynamicContextImpl(getQueryContext(), resolver_.getTransaction(), this, memMgr);
}

XERCES_CPP_NAMESPACE_QUALIFIER
DOMDocument *DbXmlContextImpl::createNewDocument() const
{
	DBXML_ASSERT(false);
	return 0;
}

void DbXmlContextImpl::releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const
{
	DBXML_ASSERT(false);
}

void DbXmlContextImpl::setMemoryManager(XPath2MemoryManager* memMgr)
{
	_memMgr = memMgr;
}

void DbXmlContextImpl::setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri)
{
	((PathanNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
}

void DbXmlContextImpl::setPreserveBoundarySpace(bool value)
{
	_bPreserveBoundarySpace=value;
}

bool DbXmlContextImpl::getPreserveBoundarySpace() const
{
	return _bPreserveBoundarySpace;
}

void DbXmlContextImpl::setInheritNamespaces(bool value)
{
    _bInheritNamespaces=value;
}

bool DbXmlContextImpl::getInheritNamespaces() const
{
    return _bInheritNamespaces;
}

void DbXmlContextImpl::setPreserveNamespaces(bool value)
{
    _bPreserveNamespaces=value;
}

bool DbXmlContextImpl::getPreserveNamespaces() const
{
    return _bPreserveNamespaces;
}

XQStaticContext::FLWOROrderingMode DbXmlContextImpl::getDefaultFLWOROrderingMode() const
{
    return _flworOrderingMode;
}

void DbXmlContextImpl::setDefaultFLWOROrderingMode(XQStaticContext::FLWOROrderingMode newMode)
{
    _flworOrderingMode=newMode;
}

void DbXmlContextImpl::setDebugCallback(XQDebugCallback* callback)
{
	m_pDebugCallback=callback;
}

XQDebugCallback* DbXmlContextImpl::getDebugCallback() const
{
	return m_pDebugCallback;
}

void DbXmlContextImpl::enableDebugging(bool enable/*=true*/)
{
	m_bEnableDebugging=enable;
}

bool DbXmlContextImpl::isDebuggingEnabled() const
{
	return m_bEnableDebugging;
}

void DbXmlContextImpl::clearDynamicContext()
{
	_nsResolver = _globalNSResolver;
	_contextItem = 0;
	_contextSize = 1;
	_contextPosition = 1;
	_varStore->clear();
	_implicitTimezone = 0;
	_resolvers.clear();
	time(&_currentTime);
}

const StaticType &DbXmlContextImpl::getContextItemType() const
{
  return _contextItemType;
}

void DbXmlContextImpl::setContextItemType(const StaticType &st)
{
  _contextItemType = st;
}

bool DbXmlContextImpl::getXPath1CompatibilityMode() const
{
	return _xpath1Compatibility;
}

void DbXmlContextImpl::setXPath1CompatibilityMode(bool newMode)
{
	_xpath1Compatibility=newMode;
}

const XMLCh* DbXmlContextImpl::getDefaultFuncNS() const
{
	return _functionNS;
}

void DbXmlContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{
	_functionNS=newNS;
}

const XMLCh* DbXmlContextImpl::getBaseURI() const
{
	return _baseURI;
}

void DbXmlContextImpl::setBaseURI(const XMLCh* newURI)
{
	_baseURI = _internalMM.getPooledString(newURI);
}

XQStaticContext::ConstructionMode DbXmlContextImpl::getConstructionMode() const
{
	return XQStaticContext::CONSTRUCTION_MODE_STRIP;
// 	return _constructionMode;
}

void DbXmlContextImpl::setConstructionMode(ConstructionMode newMode)
{
    _constructionMode=newMode;
}

StaticContext::NodeSetOrdering DbXmlContextImpl::getNodeSetOrdering() const
{
	return _ordering;
}

void DbXmlContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{
	_ordering = newOrder;
}

time_t DbXmlContextImpl::getCurrentTime() const
{
	return _currentTime;
}

void DbXmlContextImpl::setCurrentTime(time_t newTime)
{
	_currentTime=newTime;
}

const ATDurationOrDerived::Ptr &DbXmlContextImpl::getImplicitTimezone() const {
	if(_implicitTimezone == NULLRCP) {
		// validate tzone
		Timezone tzone(ContextUtils::getTimezone());
		// c-style the const away since we are in a const method
		((DbXmlContextImpl*)this)->_implicitTimezone = tzone.asDayTimeDuration(this);
	}

	return _implicitTimezone;
}

void DbXmlContextImpl::setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) {
	// validate the timezone -- will trow if invalid
	Timezone timezone(timezoneAsDuration, this);
	this->_implicitTimezone = timezoneAsDuration;
}

void DbXmlContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
	_nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* DbXmlContextImpl::getNSResolver() const {
	return _nsResolver;
}

const XMLCh* DbXmlContextImpl::getUriBoundToPrefix(const XMLCh* prefix) const
{
	const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\'"), getMemoryManager());
		DSLthrow(NamespaceLookupException, X("DbXmlContextImpl::getUriBoundToPrefix"), msg);
	}

	return uri;
}

const XMLCh* DbXmlContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
	return _nsResolver->lookupPrefix(uri);
}

void DbXmlContextImpl::setContextItem(const Item::Ptr &item)
{
	_contextItem = item;
}

void DbXmlContextImpl::setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	DBXML_ASSERT(false);
}

void DbXmlContextImpl::addCustomFunction(FuncFactory *func)
{
	_functionTable->insertFunction(func);
}

void DbXmlContextImpl::setContextSize(unsigned int size)
{
	_contextSize = size;
}

void DbXmlContextImpl::setContextPosition(unsigned int pos)
{
	_contextPosition = pos;
}

const Item::Ptr &DbXmlContextImpl::getContextItem() const
{
	return _contextItem;
}

VariableStore* DbXmlContextImpl::getVariableStore()
{
	return _varStore;
}

VariableTypeStore* DbXmlContextImpl::getVariableTypeStore()
{
	return _varTypeStore;
}

unsigned int DbXmlContextImpl::getContextSize() const
{
	return _contextSize;
}

unsigned int DbXmlContextImpl::getContextPosition() const
{
	return _contextPosition;
}

XPath2MemoryManager* DbXmlContextImpl::getMemoryManager() const
{
	return _memMgr;
}

const XMLCh* DbXmlContextImpl::getDefaultElementAndTypeNS() const
{
	return _defaultElementNS;
}

void DbXmlContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS) {
	_defaultElementNS = newNS;
}

void DbXmlContextImpl::addCollation(Collation* collation)
{
	_collations.push_back(collation);
}

void DbXmlContextImpl::setDefaultCollation(const XMLCh* URI)
{
	_defaultCollation=getMemoryManager()->getPooledString(URI);
}

Collation* DbXmlContextImpl::getCollation(const XMLCh* URI) const
{
	for(std::vector<Collation*, PathanAllocator<Collation*> >::const_iterator it= _collations.begin(); it!=_collations.end(); ++it)
		if(XPath2Utils::equals((*it)->getCollationName(), URI))
			return (*it);

	const XMLCh* msg = XPath2Utils::concatStrings(X("The requested collation ('"), URI, X("') is not defined"), getMemoryManager());

	DSLthrow(ContextException, X("DbXmlContextImpl::getCollation"), msg);
	return NULL;
}

Collation* DbXmlContextImpl::getDefaultCollation() const
{
	return getCollation(_defaultCollation);
}

DataItem* DbXmlContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfDataItems& v) const
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
			DSLthrow(NamespaceLookupException, X("DbXmlContextImpl::lookUpFunction"), msg);
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

void DbXmlContextImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
	_docCache->setXMLEntityResolver(handler);
}

void DbXmlContextImpl::registerURIResolver(URIResolver *resolver)
{
	if(resolver != 0) {
		_resolvers.push_back(resolver);
	}
}

Sequence DbXmlContextImpl::resolveDocument(const XMLCh* uri)
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
		_throwResolutionError(uri, "document");
	}

	return result;
}

Sequence DbXmlContextImpl::resolveCollection(const XMLCh* uri)
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

Sequence DbXmlContextImpl::resolveDefaultCollection()
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
bool DbXmlContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const
{
	return _docCache->isTypeOrDerivedFromType(uri,typeName,uriToCheck,typeNameToCheck);
}

void DbXmlContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{
	_docCache->addSchemaLocation(uri, locations, this);
}

const DocumentCache* DbXmlContextImpl::getDocumentCache() const
{
	return _docCache;
}

Node::Ptr DbXmlContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
	return _docCache->validate(node, valMode, this);
}

PathanFactory *DbXmlContextImpl::getPathanFactory() const
{
  return _pathanFactory;
}

void DbXmlContextImpl::setPathanFactory(PathanFactory *factory)
{
  _pathanFactory = factory;
}

