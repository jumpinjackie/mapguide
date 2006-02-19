/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/internal/context/DocumentCacheImpl.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLResourceIdentifier.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>
#include <xercesc/validators/common/GrammarResolver.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLSchemaDescription.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/internal/XMLScanner.hpp>
#include <xercesc/util/HashPtr.hpp>

#include <pathan/exceptions/DynamicErrorException.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>
#include <pathan/exceptions/XMLParseException.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/internal/AnyAtomicTypeDatatypeValidator.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

void DocumentCacheErrorCatcher::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
{
  // ignore them
}

void DocumentCacheErrorCatcher::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
{
  throw toCatch;
}

void DocumentCacheErrorCatcher::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
{
  throw toCatch;
}

void DocumentCacheErrorCatcher::resetErrors()
{
}

/////////////////////////////////////////////////////////////////
//
//

/* untyped */
const XMLCh DocumentCacheParser::g_szUntyped[]=                   {XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,
                                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chNull};

XERCES_CPP_NAMESPACE_QUALIFIER Grammar* DocumentCacheParser::createXQueryTypes(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm, XERCES_CPP_NAMESPACE_QUALIFIER GrammarResolver *gr) {
  XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar *sg =  new (mm) XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar(mm);

  // TODO: These lines are not needed when Xerces fixes a bug in the XSModel::addGrammarToXSModel() - jpcs
  sg->setComplexTypeRegistry(new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo>(1,mm));
  sg->setAttributeDeclRegistry(new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER XMLAttDef>(1,mm));
  sg->setAttGroupInfoRegistry(new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER XercesAttGroupInfo>(1,mm));
  sg->setGroupInfoRegistry(new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER XercesGroupInfo>(1,mm));

  sg->setTargetNamespace(FunctionConstructor::XMLChXPath2DatatypesURI);
  sg->setGrammarDescription(gr->getGrammarPool()->createSchemaDescription(FunctionConstructor::XMLChXPath2DatatypesURI));

  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory *dtvf = sg->getDatatypeRegistry();

  // xdt:dayTimeDuration
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>* facets =
    new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>(1,mm);
  facets->put((void*) XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN,
              new (mm) XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN, ATDurationOrDerived::pattern_DT_YEARMONTHDURATION,mm));
    
  dtvf->createDatatypeValidator(ATDurationOrDerived::fgDT_YEARMONTHDURATION_XERCESHASH, 
                                dtvf->getDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION),
                                facets, 0, false, 0, true, mm);
    
  // xdt:yearMonthDuration
  facets = new (mm) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>(1,mm);
  facets->put((void*) XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN,
              new (mm) XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN, ATDurationOrDerived::pattern_DT_DAYTIMEDURATION,mm));
    
  dtvf->createDatatypeValidator(ATDurationOrDerived::fgDT_DAYTIMEDURATION_XERCESHASH, 
                                dtvf->getDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION),
                                facets, 0, false, 0, true, mm);

  // xdt:anyAtomicType -- no facets.  We need to put this validator in the xerces memory manager, and put it in the registry ourselves
  XERCES_CPP_NAMESPACE_QUALIFIER DVHashTable *defRegistry = dtvf->getBuiltInRegistry();
  if(!defRegistry->containsKey((void*) AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH)) {
    XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* dv = new (XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) AnyAtomicTypeDatatypeValidator();
    dv->setTypeName(AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH);
    defRegistry->put((void*) AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH, dv);
  }
    
  // xdt:untypedAtomic -- no facets
  dtvf->createDatatypeValidator(ATUntypedAtomic::fgDT_UNTYPEDATOMIC_XERCESHASH, 
                                dtvf->getDatatypeValidator(AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH),
                                0, 0, false, 0, true, mm);

  return sg;
}

DocumentCacheParser::DocumentCacheParser(const DocumentCacheParser &parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : XercesDOMParser(0, memMgr, parent.getGrammarResolver()->getGrammarPool()),
    _documentMap(3,false,memMgr),
    _uriMap(3,false, new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER HashPtr(), memMgr),
    _context(0),
    _memMgr(memMgr)
{
  init();

  getGrammarResolver()->cacheGrammars();
  getGrammarResolver()->useCachedGrammarInParse(true);

  // also set these things to true.  Not sure if this is correct, but without
  // them, all built-in types cannot be found (untypedAtomic, dayTimeDuration and yearMonthDuration) -- crioux
  getGrammarResolver()->cacheGrammarFromParse(true);
  if(!isCachingGrammarFromParse())
    cacheGrammarFromParse(true);    // hold the loaded schemas in the cache, so that can be reused    
}

DocumentCacheParser::DocumentCacheParser(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr) :
  XercesDOMParser(0,memMgr, xmlgr),
  _documentMap(3,false,memMgr),
  _uriMap(3,false, new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER HashPtr(), memMgr),
  _context(0),
  _memMgr(memMgr)
{
  init();

  // add XPath2 types
  getGrammarResolver()->putGrammar(createXQueryTypes(_memMgr, getGrammarResolver()));

  getGrammarResolver()->cacheGrammars();
  getGrammarResolver()->useCachedGrammarInParse(true);

  // also set these things to true.  Not sure if this is correct, but without
  // them, all built-in types cannot be found (untypedAtomic, dayTimeDuration and yearMonthDuration) -- crioux
  getGrammarResolver()->cacheGrammarFromParse(true);
  if(!isCachingGrammarFromParse())
    cacheGrammarFromParse(true);    // hold the loaded schemas in the cache, so that can be reused    
}

DocumentCacheParser::~DocumentCacheParser()
{
}

void DocumentCacheParser::init()
{
  //todo this does not need to happen now if these types are already in the cache!!! 
  //also sort out what state to be in for the query - ie no cacheing to polute the grammar

  // set up the parser
  setDoSchema(true);// enable schema processing
  setDoNamespaces(true);// enable namespaces (needed for schema processing)
  setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER AbstractDOMParser::Val_Auto);// try to validate
  setCreateEntityReferenceNodes(false);// don't create entity reference nodes, but represent its value in the DOM tree
  setValidationConstraintFatal(false);// if a schema validation error is encountered, don't abort parsing
  setErrorHandler(&_errorHandler);
  setCreateSchemaInfo(true);
}

void DocumentCacheParser::startDocument()
{
  // Mostly copied from AbstractDOMParser::startDocument()

  if(_context) {
    fDocument = (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl *)
      _context->createNewDocument(); // This uses the context's memory manager
  }
  else {
    fDocument = (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl *)
      XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation::getImplementation()->createDocument(fMemoryManager);
  }
                                                                                                                                                              
  // Just set the document as the current parent and current node
  fCurrentParent = fDocument;
  fCurrentNode   = fDocument;
  // set DOM error checking off
  fDocument->setErrorChecking(false);
  fDocument->setDocumentURI(fScanner->getLocator()->getSystemId());
  fDocument->setActualEncoding(fScanner->getReaderMgr()->getCurrentEncodingStr());
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheParser::parseWithContext(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource &source, StaticContext *context)
{
  _context = context;
  try {
    parse(source);
  }
  catch(...) {
    _context = 0;
    throw;
  }
  _context = 0;
  return adoptDocument();
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheParser::parseWithContext(const XMLCh* const uri, StaticContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER InputSource* srcToUse = 0;
  if (getXMLEntityResolver()){
    XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier resourceIdentifier(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier::UnKnown,
                                                                            uri, 
                                                                            0, 
                                                                            XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, 
                                                                            context->getBaseURI());
    srcToUse = getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
  }
  XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER InputSource> janIS(srcToUse);

  // Resolve the uri against the base uri
  const XMLCh *systemId = 0;
  if(srcToUse)
    systemId=srcToUse->getSystemId();
  else
  {
    systemId=uri;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(context->getMemoryManager());
    if(urlTmp.setURL(context->getBaseURI(), uri, urlTmp)) {
      systemId = context->getMemoryManager()->getPooledString(urlTmp.getURLText());
    }
  }

  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *cachedDoc = retrieveDocument(systemId);
  if(cachedDoc) {
    return cachedDoc;
  }

  _context = context;
  try {
    if(srcToUse)
      parse(*srcToUse);
    else
      parse(systemId);
    storeDocument(systemId, getDocument());
  }
  catch(...) {
    _context = 0;
    throw;
  }
  _context = 0;
  return adoptDocument();
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentCacheParser::retrieveDocument(const XMLCh* uri)
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = _documentMap.get((void*)uri);
  return doc;
}

void DocumentCacheParser::storeDocument(const XMLCh* uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document)
{
  _documentMap.put((void*)uri, document);
  _uriMap.put((void*)document, const_cast<XMLCh*>(uri));
}

void DocumentCacheParser::removeDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document)
{
  XMLCh *uri = _uriMap.get((void*)document);
  if(uri != 0) {
    _uriMap.removeKey((void*)document);
    _documentMap.removeKey((void*)uri);
  }
}

void DocumentCacheParser::clearStoredDocuments()
{
  _documentMap.removeAll();
  _uriMap.removeAll();
}
 
void DocumentCacheParser::loadSchema(const XMLCh* const uri, const XMLCh* const location, StaticContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER InputSource* srcToUse = 0;
  if (getXMLEntityResolver()){
    XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier resourceIdentifier(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier::SchemaGrammar,
                                                                            location, 
                                                                            uri, 
                                                                            XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, 
                                                                            context->getBaseURI());
    srcToUse = getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
  }
  XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER InputSource> janIS(srcToUse);

  // always validate, so that the preloaded schema can be matched even if the XML doesn't reference it    
  setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER AbstractDOMParser::Val_Always);

  // prepopulate the cache with the requested grammar, so that static typing can use it
  // if needed, set the "use cached grammars" flag
  if(!isCachingGrammarFromParse())
    cacheGrammarFromParse(true);// hold the loaded schemas in the cache, so that can be reused

  if(srcToUse)
    getScanner()->loadGrammar(*srcToUse, XERCES_CPP_NAMESPACE_QUALIFIER Grammar::SchemaGrammarType, true);
  else if(location)
  {
    // Resolve the location against the base uri
    const XMLCh *systemId = location;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(context->getMemoryManager());
    if(urlTmp.setURL(context->getBaseURI(), location, urlTmp)) {
      systemId = urlTmp.getURLText();
    }

    getScanner()->loadGrammar(systemId, XERCES_CPP_NAMESPACE_QUALIFIER Grammar::SchemaGrammarType, true);
  }
}

unsigned int DocumentCacheParser::getSchemaUriId(const XMLCh* uri) const
{
  // This is a kind of hack... I should directly use fURIStringPool, but it's private.
  // So I ask the scanner to give me back the string pool I gave him before....
  return getScanner()->getURIStringPool()->getId(uri);
}

const XMLCh* DocumentCacheParser::getSchemaUri(unsigned int id) const
{
  // This is a kind of hack... I should directly use fURIStringPool, but it's private.
  // So I ask the scanner to give me back the string pool I gave him before....
  return getScanner()->getURIStringPool()->getValueForId(id);
}

bool DocumentCacheParser::isChildElement( XERCES_CPP_NAMESPACE_QUALIFIER ContentSpecNode *topContentSpec, unsigned int uriId, const XMLCh* localPart ) const
{
  if(topContentSpec){
    if(topContentSpec->getType()==XERCES_CPP_NAMESPACE_QUALIFIER ContentSpecNode::Leaf)
      {
        XERCES_CPP_NAMESPACE_QUALIFIER QName* qName=topContentSpec->getElement();
        if(qName->getURI()==uriId && XPath2Utils::equals(qName->getLocalPart(),localPart))
          return true;
      }
    else 
      {
        if(isChildElement(topContentSpec->getFirst(), uriId, localPart) || 
           isChildElement(topContentSpec->getSecond(), uriId, localPart))
          return true;
      }
  }
  return false;
}

XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl* DocumentCacheParser::getElementDecl(const XMLCh* elementUri, const XMLCh* elementName) const {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLSchemaDescription* gramDesc = getGrammarResolver()->getGrammarPool()->createSchemaDescription(elementUri);
    XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER XMLSchemaDescription> janName(gramDesc);
    XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar* schGrammar=(XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar*)getGrammarResolver()->getGrammar(gramDesc);
    if(schGrammar==NULL)
      return NULL;

    unsigned int uriId=getSchemaUriId(elementUri);
    // find the definition for this node, as a global definition
    return (XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl*)schGrammar->getElemDecl(uriId,
                                                                                      elementName,
                                                                                      NULL,
                                                                                      (unsigned int)XERCES_CPP_NAMESPACE_QUALIFIER Grammar::TOP_LEVEL_SCOPE);
}

XERCES_CPP_NAMESPACE_QUALIFIER SchemaAttDef* DocumentCacheParser::getAttributeDecl(const XMLCh* attributeUri, const XMLCh* attributeName) const {

    XERCES_CPP_NAMESPACE_QUALIFIER XMLSchemaDescription* gramDesc = getGrammarResolver()->getGrammarPool()->createSchemaDescription(attributeUri);
    XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER XMLSchemaDescription> janName(gramDesc);
    XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar* schGrammar=(XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar*)getGrammarResolver()->getGrammar(gramDesc);
    if(schGrammar==NULL)
      return NULL;

    // find the definition for this node, as a global definition
    return (XERCES_CPP_NAMESPACE_QUALIFIER SchemaAttDef*)schGrammar->getAttributeDeclRegistry()->get(attributeName);
}

Node::Ptr DocumentCacheParser::validate(const Node::Ptr &node,
	DocumentCache::ValidationMode valMode,
	DynamicContext *context)
{
    XERCES_CPP_NAMESPACE_QUALIFIER AbstractDOMParser::ValSchemes oldValScheme=getValidationScheme();
    try {
        // if validation is strict, there must be a schema for the root node
        if(valMode==DocumentCache::VALIDATION_STRICT) {
            ATQNameOrDerived::Ptr name = node->dmNodeName(context);
            const XMLCh *node_uri = ((const ATQNameOrDerived*)name.get())->getURI();
            const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

            XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl* elemDecl=getElementDecl(node_uri, node_name);
            if(elemDecl==NULL) {
                XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer msg(1023, context->getMemoryManager());
                msg.set(X("Element {"));
                msg.append(node_uri);
                msg.append(X("}"));
                msg.append(node_name);
                msg.append(X(" is not defined as a global element [err:XQDY0084]"));
                DSLthrow(DynamicErrorException,X("DocumentCacheParser::validate"), msg.getRawBuffer());
            }
        }
        // - build a textual representation of the element
        const XMLCh* serializedForm = node->asString(context);

        // - enable the flag bValidationConstraintFatal, if validation is strict
        if(valMode==DocumentCache::VALIDATION_STRICT)
            setValidationConstraintFatal(true);
        setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER AbstractDOMParser::Val_Always);
        setIdentityConstraintChecking(false);

        XERCES_CPP_NAMESPACE_QUALIFIER MemBufInputSource inputSrc(  (const XMLByte*)serializedForm, 
                                                                    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(serializedForm)*sizeof(XMLCh), 
                                                                    XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString,
                                                                    true,
                                                                    context->getMemoryManager());
        inputSrc.setCopyBufToStream(false);
        inputSrc.setEncoding(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgUTF16EncodingString);
        // - parse the text (with validation on)
        XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* pDoc=parseWithContext(inputSrc, context);
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        setIdentityConstraintChecking(true);
        // - return the document element
        if(pDoc==NULL)
            return NULL;
        // if we were asked to validate a document node, return the document; otherwise, return the root node
        if(node->dmNodeKind() == Node::document_string)
            return new NodeImpl(pDoc, context);
        else
            return new NodeImpl(pDoc->getDocumentElement(), context);
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER SAXException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.getMessage());
        exc_msg.append(X(" [err:XQDY0027]"));
        DSLthrow(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
    }
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.msg);
        exc_msg.append(X(" [err:XQDY0027]"));
        DSLthrow(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.getMessage());
        exc_msg.append(X(" [err:XQDY0027]"));
        DSLthrow(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
    }
    return NULL;
}

void DocumentCacheParser::error(const   unsigned int                errCode
                                , const XMLCh* const                msgDomain
                                , const XERCES_CPP_NAMESPACE_QUALIFIER XMLErrorReporter::ErrTypes  errType
                                , const XMLCh* const                errorText
                                , const XMLCh* const                systemId
                                , const XMLCh* const                publicId
                                , const XMLSSize_t                  lineNum
                                , const XMLSSize_t                  colNum
                                )
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLErrorReporter::ErrTypes newErrType=errType;
  // if we have disabled validation errors, convert them into warnings
  if (!getValidationConstraintFatal() && XPath2Utils::equals(msgDomain, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgValidityDomain)) 
    newErrType=XERCES_CPP_NAMESPACE_QUALIFIER XMLErrorReporter::ErrType_Warning;
  XercesDOMParser::error(errCode,msgDomain,newErrType,errorText,systemId,publicId,lineNum,colNum);
}

/////////////////////////////////////////////////////
//

DocumentCacheImpl::DocumentCacheImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr) :
  _parser(memMgr, xmlgr),
  _firstDocRefCount(new (memMgr) DocRefCount()),
  _memMgr(memMgr)
{
}

DocumentCacheImpl::DocumentCacheImpl(const DocumentCacheImpl *parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _parser(parent->_parser, memMgr),
    _firstDocRefCount(new (memMgr) DocRefCount()),
    _memMgr(memMgr)
{
}

DocumentCacheImpl::~DocumentCacheImpl()
{
  DocRefCount *drc;
  while(_firstDocRefCount != 0) {
    drc = _firstDocRefCount;
    _firstDocRefCount = _firstDocRefCount->next;
    _memMgr->deallocate(drc);
  }
}

void DocumentCacheImpl::release()
{
  this->~DocumentCacheImpl();
  _memMgr->deallocate(this);
}

void DocumentCacheImpl::incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const
{
  assert(document != 0);

  DocRefCount *found = _firstDocRefCount;
  while(found->doc != 0 && found->doc != document) {
    found = found->next;
  }

  if(found->doc == 0) {
    found->doc = document;
    found->next = new (_memMgr) DocRefCount();
  }
  else {
    ++found->ref_count;
  }
}

void DocumentCacheImpl::decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document, const StaticContext *context) const
{
  assert(document != 0);

  DocumentCacheImpl *me = const_cast<DocumentCacheImpl*>(this);

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
      _memMgr->deallocate(found);
      me->_parser.removeDocument(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*>(document));
      context->releaseDocument(const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*>(document));
    }
  }
}

void DocumentCacheImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _parser.setXMLEntityResolver(handler);
}

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* DocumentCacheImpl::getXMLEntityResolver() const
{
    return const_cast<XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver*>(_parser.getXMLEntityResolver());
}

void DocumentCacheImpl::clearStoredDocuments()
{
  _parser.clearStoredDocuments();
}
 
Node::Ptr DocumentCacheImpl::loadXMLDocument(const XMLCh* uri, DynamicContext *context)
{
  Node::Ptr result;
  try {
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = _parser.parseWithContext(uri, context);
    result = new NodeImpl(doc, context);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER SAXException& toCatch) {
    //TODO: Find a way to decipher whether the exception is actually because of a parsing problem or because the document can't be found
    DSLthrow(XMLParseException, X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
    DSLthrow(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.msg);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
    DSLthrow(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
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
bool DocumentCacheImpl::isTypeOrDerivedFromType(const XMLCh* const uri, const XMLCh* const typeName, const XMLCh* const uriToCheck, const XMLCh* const typeNameToCheck) const
{
  //cerr<<"is "<<XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(typeName)<<" instance of "<<XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(typeNameToCheck)<<"?"<<endl;
  // dumb check
  if(XPath2Utils::equals(typeName,typeNameToCheck) && 
     XPath2Utils::equals(uri,uriToCheck) )
    return true;

  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* dtvDerived=_parser.getGrammarResolver()->getDatatypeValidator(uri,typeName);
  if(dtvDerived==NULL)
    {
      ////cerr << "looking at complex" << endl;
      //now lets take a look at complex stuff
      XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo *cti = getComplexTypeInfo(uri, typeName);

      //in the case of non-schema grammar
      if (cti) 
        {
          // if we are here, the type is a complex type
          if(XPath2Utils::equals(uriToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
             XPath2Utils::equals(typeNameToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_ANYTYPE))
            return true;

          while(cti != 0) 
            {
              if(XPath2Utils::equals(uriToCheck, cti->getTypeUri()) && 
                 XPath2Utils::equals(typeNameToCheck, cti->getTypeLocalName())) 
                return true;
              XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo *ctiOld = cti;
              cti = cti->getBaseComplexTypeInfo();

              //might be derived from a simple type
              if(cti == 0) 
                {
                  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* dtv = ctiOld->getDatatypeValidator();

                  while(dtv != 0) 
                    {
                      //cerr << "dtv type nameUDCom is '" << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(dtv->getTypeName()) << "'" << endl;
                      if(XPath2Utils::equals(uriToCheck, dtv->getTypeUri()) && 
                         XPath2Utils::equals(typeNameToCheck, dtv->getTypeLocalName())) 
                        return true;
                      dtv = dtv->getBaseValidator();
                    }   
                }
            }
        }
      return false;
    }

  // if we are here, the type is a simple type
  if( (XPath2Utils::equals(typeNameToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYSIMPLETYPE) &&
       XPath2Utils::equals(uriToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) ) ||
      (XPath2Utils::equals(typeNameToCheck, AnyAtomicType::fgDT_ANYATOMICTYPE) &&
       XPath2Utils::equals(uriToCheck, FunctionConstructor::XMLChXPath2DatatypesURI) ) )
    return true;

  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator* dtvBase=_parser.getGrammarResolver()->getDatatypeValidator(uriToCheck,typeNameToCheck);
  if(dtvBase==NULL)
    return false;
  while(dtvDerived != 0) 
    {
      if(XPath2Utils::equals(dtvBase->getTypeUri(), dtvDerived->getTypeUri()) && 
         XPath2Utils::equals(dtvBase->getTypeLocalName(), dtvDerived->getTypeLocalName())) 
        return true;
      dtvDerived = dtvDerived->getBaseValidator();
    }

  // if we are here, the type is a type
  if(XPath2Utils::equals(uriToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
     XPath2Utils::equals(typeNameToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_ANYTYPE))
    return true;

  return false;
}

void DocumentCacheImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations, StaticContext *context)
{
  if(locations==NULL)
  {
    // if no locations are given, try to see if the entity resolver can still find it
    try {
        _parser.loadSchema(uri, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
    }
    catch(...) {
    }
    return;
  }

  bool bFoundSchema=false;
  for(VectorOfStrings::iterator it=locations->begin(); it!=locations->end(); it++)
  {
      try {
        _parser.loadSchema(uri, *it, context);
        bFoundSchema=true;
        break;
	  }
      catch(...) {
	  }
  }
  if(!bFoundSchema)
    DSLthrow(StaticErrorException,X("DocumentCacheImpl::addSchemaLocation"), X("Schema not found [err:XQST0059]"));
}

unsigned int DocumentCacheImpl::getSchemaUriId(const XMLCh* uri) const
{
  return _parser.getSchemaUriId(uri);
}

const XMLCh* DocumentCacheImpl::getSchemaUri(unsigned int id) const
{
  return _parser.getSchemaUri(id);
}

Node::Ptr DocumentCacheImpl::validate(const Node::Ptr &node,
	DocumentCache::ValidationMode valMode,
	DynamicContext *context)
{
  return _parser.validate(node, valMode, context);
}

XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator*  DocumentCacheImpl::getDatatypeValidator(const XMLCh* uri, const XMLCh* typeName) const {

  return _parser.getGrammarResolver()->getDatatypeValidator(uri,typeName);
}

XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl* DocumentCacheImpl::getElementDecl(const XMLCh* elementUri, const XMLCh* elementName) const {

  return _parser.getElementDecl(elementUri,elementName);
}

XERCES_CPP_NAMESPACE_QUALIFIER SchemaAttDef* DocumentCacheImpl::getAttributeDecl(const XMLCh* attributeUri, const XMLCh* attributeName) const {

  return _parser.getAttributeDecl(attributeUri,attributeName);
}

XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo*  DocumentCacheImpl::getComplexTypeInfo(const XMLCh* uri, const XMLCh* typeName) const {

  XERCES_CPP_NAMESPACE_QUALIFIER Grammar* grammar=_parser.getGrammarResolver()->getGrammar(uri);
  if(grammar==NULL)
    return NULL;
  if(grammar->getGrammarType()!=XERCES_CPP_NAMESPACE_QUALIFIER Grammar::SchemaGrammarType)
    return NULL;
  XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar* schGrammar=(XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar*)grammar;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo> *ctr = schGrammar->getComplexTypeRegistry();
  if(ctr==NULL)
    return NULL;

  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer typeKey(1023, _memMgr);
  typeKey.set(uri);
  typeKey.append(XERCES_CPP_NAMESPACE_QUALIFIER chComma);
  typeKey.append(typeName);

  return ctr->get(typeKey.getRawBuffer());

}

DocumentCache *DocumentCacheImpl::createDerivedCache(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{
  // lock the grammar pool, so we can share it accross threads
  _parser.getGrammarResolver()->getGrammarPool()->lockPool();

  // Construct a new DocumentCacheImpl, based on this one
  return new (memMgr) DocumentCacheImpl(this, memMgr);
}
