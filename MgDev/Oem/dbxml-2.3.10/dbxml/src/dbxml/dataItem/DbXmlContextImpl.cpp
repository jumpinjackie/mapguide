//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlContextImpl.cpp,v 1.49.2.2 2007/01/03 19:21:13 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <sstream>
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
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

#include <xercesc/util/XMLUri.hpp>

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

const XMLCh VARNAME_TEMPORARY[] = { ':', ':', 't', 'e', 'm', 'p', 'o', 'r', 'a', 'r', 'y', 0 };
const XMLCh VARNAME_RESULT[] = { ':', ':', 'r', 'e', 's', 'u', 'l', 't', 0 };

static CodepointCollation g_codepointCollation;

DbXmlContextImpl::DbXmlContextImpl(QueryContext &qc, Transaction *txn, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
	: DbXmlContext(qc, txn),
	  _createdWith(memMgr),
	  _internalMM(memMgr),
	  _varTypeStore(0),
	  _functionTable(0),
	  _collations(XQillaAllocator<Collation*>(&_internalMM)),
	  _constructionMode(CONSTRUCTION_MODE_PRESERVE),
	  _ordering(StaticContext::ORDERING_ORDERED),
	  _bPreserveBoundarySpace(false),
	  _varStore(0),
	  _resolvers(XQillaAllocator<URIResolver*>(&_internalMM))
{
	_memMgr = &_internalMM;

	////////////////////
	// static context //
	////////////////////
	_contextItemType.flags = StaticType::ITEM_TYPE;
	_xpath1Compatibility = false;    // according to Formal Semantics, 4.1.1

	_globalNSResolver = new (&_internalMM) XQillaNSResolverImpl(&_internalMM, 0); // resolve acc.to null node
	_nsResolver = _globalNSResolver;

	_defaultElementNS = 0;
	// by default, the default namespace for functions is the XPath2 namespace
	_functionNS = XQFunction::XMLChFunctionURI;

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

	_itemFactory = new (&_internalMM) DbXmlFactoryImpl(_docCache, &_internalMM);

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

	time(&_currentTime);

	// XQuery stuff
	m_pDebugCallback = NULL;
	m_bEnableDebugging = false;

	// XQuery defines 6 predefined namespace bindings
	setNamespaceBinding(XMLChXS, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
	setNamespaceBinding(XMLChXSI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_XSI);
	setNamespaceBinding(XMLChFN, XQFunction::XMLChFunctionURI);
	setNamespaceBinding(XMLChXDT, FunctionConstructor::XMLChXPath2DatatypesURI);
	setNamespaceBinding(XMLChLOCAL, XQUserFunction::XMLChXQueryLocalFunctionsURI);

	// DbXml specific
	registerURIResolver(&resolver_);
	setModuleResolver(&resolver_);
	setXMLEntityResolver(&resolver_);
	qc_->populateStaticContext(this);
}

DbXmlContextImpl::~DbXmlContextImpl()
{
	_varTypeStore->clear();
	_varStore->clear();

	delete _itemFactory;
	delete _docCache;
}

DynamicContext *DbXmlContextImpl::createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
	DynamicContext* moduleCtx = new (memMgr) DbXmlContextImpl(getQueryContext(), resolver_.getTransaction(), memMgr);
        // force the context to use our memory manager
        moduleCtx->setMemoryManager(getMemoryManager());
        // propagate debug settings
        moduleCtx->enableDebugging(isDebuggingEnabled());
	return moduleCtx;
}

DynamicContext *DbXmlContextImpl::createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
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
	if(XPath2Utils::equals(prefix,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) || 
		XPath2Utils::equals(prefix,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString))
		XQThrow2(StaticErrorException,X("XQContextImpl::setNamespaceBinding"),
			X("The prefixes 'xmlns' and 'xml' cannot be used in a namespace declaration [err:XQST0070]"));
	if(XPath2Utils::equals(uri,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName))
		XQThrow2(StaticErrorException,X("XQContextImpl::setNamespaceBinding"),
			X("The 'http://www.w3.org/XML/1998/namespace' namespace cannot be bound to any prefix [err:XQST0070]"));
	((XQillaNSResolverImpl*)_nsResolver)->addNamespaceBinding(prefix,uri);
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

StaticContext::FLWOROrderingMode DbXmlContextImpl::getDefaultFLWOROrderingMode() const
{
    return _flworOrderingMode;
}

void DbXmlContextImpl::setDefaultFLWOROrderingMode(StaticContext::FLWOROrderingMode newMode)
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

StaticContext::ConstructionMode DbXmlContextImpl::getConstructionMode() const
{
	return StaticContext::CONSTRUCTION_MODE_STRIP;
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

void DbXmlContextImpl::setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) {
	_nsResolver = resolver;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* DbXmlContextImpl::getNSResolver() const {
	return _nsResolver;
}

const XMLCh* DbXmlContextImpl::getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const
{
	const XMLCh* uri = _nsResolver->lookupNamespaceURI(prefix);

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(prefix) > 0){
		const XMLCh* msg = XPath2Utils::concatStrings(X("No namespace for prefix \'"), prefix, X("\' [err:XPST0081]"), getMemoryManager());
		XQThrow3(NamespaceLookupException, X("DbXmlContextImpl::getUriBoundToPrefix"), msg, location);
	}

	return uri;
}

const XMLCh* DbXmlContextImpl::getPrefixBoundToUri(const XMLCh* uri) const
{
	return _nsResolver->lookupPrefix(uri);
}

void DbXmlContextImpl::addCustomFunction(FuncFactory *func)
{
	_functionTable->insertFunction(func);
}

VariableStore* DbXmlContextImpl::getVariableStore()
{
	return _varStore;
}

VariableTypeStore* DbXmlContextImpl::getVariableTypeStore()
{
	return _varTypeStore;
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

Collation* DbXmlContextImpl::getCollation(const XMLCh* URI, const LocationInfo *location) const
{
	if(!XMLUri::isValidURI(false, URI)) {
		const XMLCh* baseURI=getBaseURI();
		if(baseURI && *baseURI) {
			try {
				XMLUri base(baseURI, getMemoryManager());
				XMLUri full(&base, URI, getMemoryManager());
				URI = getMemoryManager()->getPooledString(full.getUriText());
			}
			catch(XMLException &e) {
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

Collation* DbXmlContextImpl::getDefaultCollation(const LocationInfo *location) const
{
	return getCollation(_defaultCollation, location);
}

ASTNode* DbXmlContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v, const LocationInfo *location) const
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
			XQThrow3(NamespaceLookupException, X("DbXmlContextImpl::lookUpFunction"), msg, location);
		}
	}

	ASTNode* functionImpl=_functionTable->lookUpFunction(uri, name, v, getMemoryManager());

	if(functionImpl == NULL && v.size() == 1)
	{
		// maybe it's not a function, but a datatype
		try
		{
			bool isPrimitive;
			_itemFactory->getPrimitiveTypeIndex(uri, name, isPrimitive);
			functionImpl = new (getMemoryManager())
				FunctionConstructor(uri, name, v, getMemoryManager());
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

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* DbXmlContextImpl::getXMLEntityResolver() const
{
	return _docCache->getXMLEntityResolver();
}

void DbXmlContextImpl::registerURIResolver(URIResolver *resolver)
{
	if(resolver != 0) {
		_resolvers.push_back(resolver);
	}
}

Sequence DbXmlContextImpl::resolveDocument(const XMLCh* uri, const LocationInfo *location)
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
		XQThrow3(XMLParseException,X("DbXmlContextImpl::resolveDocument"),
			errMsg.getRawBuffer(), location);
	}

	return result;
}

Sequence DbXmlContextImpl::resolveCollection(const XMLCh* uri, const LocationInfo *location)
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
		XQThrow3(XMLParseException,X("DbXmlContextImpl::resolveCollection"),
			errMsg.getRawBuffer(), location);
	}

	return result;
}

Sequence DbXmlContextImpl::resolveDefaultCollection()
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

void DbXmlContextImpl::setDocumentCache(DocumentCache* docCache)
{
	_docCache = docCache;
}

Node::Ptr DbXmlContextImpl::validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode)
{
	DBXML_ASSERT(false);
	return 0;
}

ItemFactory *DbXmlContextImpl::getItemFactory() const
{
  return _itemFactory;
}

void DbXmlContextImpl::setItemFactory(ItemFactory *factory)
{
  _itemFactory = factory;
}

