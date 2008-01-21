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
 * $Id: DocumentCacheImpl.cpp,v 1.26.2.1 2006/12/08 15:19:12 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/schema/DocumentCacheImpl.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLResourceIdentifier.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/validators/datatype/DatatypeValidatorFactory.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLSchemaDescription.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/internal/XMLScanner.hpp>
#include <xercesc/internal/XMLScannerResolver.hpp>
#include <xercesc/util/HashPtr.hpp>

#include <xqilla/exceptions/DynamicErrorException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/schema/AnyAtomicTypeDatatypeValidator.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/items/impl/NodeImpl.hpp>

class HackGrammarResolver : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
    bool                            fCacheGrammar;
    bool                            fUseCachedGrammar;
    bool                            fGrammarPoolFromExternalApplication;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLStringPool*                  fStringPool;
    XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER Grammar>*        fGrammarBucket;
    XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER Grammar>*        fGrammarFromPool;
    XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory*       fDataTypeReg;
    XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager*                  fMemoryManager;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool*                 fGrammarPool;
    XERCES_CPP_NAMESPACE_QUALIFIER XSModel*                        fXSModel;
    XERCES_CPP_NAMESPACE_QUALIFIER XSModel*                        fGrammarPoolXSModel;
    XERCES_CPP_NAMESPACE_QUALIFIER ValueVectorOf<XERCES_CPP_NAMESPACE_QUALIFIER SchemaGrammar*>*  fGrammarsToAddToXSModel;
};

XQillaGrammarResolver::XQillaGrammarResolver(
                      XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* const gramPool, 
                      XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager*  const manager /*= XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager*/)
: GrammarResolver(gramPool, manager)
{
  // trigger the creation of the fDataTypeReg data member
  getDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidatorFactory* dvdt=((HackGrammarResolver*)this)->fDataTypeReg;
  // xdt:yearMonthDuration
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>* facets =
    new (manager) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>(1,manager);
  facets->put((void*) XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN,
              new (manager) XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN, ATDurationOrDerived::pattern_DT_YEARMONTHDURATION,manager));
    
  dvdt->createDatatypeValidator(ATDurationOrDerived::fgDT_YEARMONTHDURATION, 
                                dvdt->getDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION),
                                facets, 0, false, 0, true, manager);
    
  // xdt:dayTimeDuration
  facets = new (manager) XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair>(1,manager);
  facets->put((void*) XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN,
              new (manager) XERCES_CPP_NAMESPACE_QUALIFIER KVStringPair(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgELT_PATTERN, ATDurationOrDerived::pattern_DT_DAYTIMEDURATION,manager));
    
  dvdt->createDatatypeValidator(ATDurationOrDerived::fgDT_DAYTIMEDURATION, 
                                dvdt->getDatatypeValidator(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION),
                                facets, 0, false, 0, true, manager);

  // xdt:untypedAtomic -- no facets
  dvdt->createDatatypeValidator(ATUntypedAtomic::fgDT_UNTYPEDATOMIC, 
                                dvdt->getDatatypeValidator(AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH),
                                0, 0, false, 0, true, manager);
}

/////////////////////////////////////////////////////////////////
//
//

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
const XMLCh DocumentCacheParser::g_szUntyped[]= {
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

DocumentCacheParser::DocumentCacheParser(const DocumentCacheParser &parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : XercesDOMParser(0, memMgr, parent.getGrammarResolver()->getGrammarPool()),
    _context(0)
{
  init();
}

DocumentCacheParser::DocumentCacheParser(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr) :
  XercesDOMParser(0,memMgr, xmlgr),
  _context(0)
{
  init();
}

DocumentCacheParser::~DocumentCacheParser()
{
}

void DocumentCacheParser::init()
{
  delete fGrammarResolver; delete fScanner;
  // re-init using this grammar resolver
  fGrammarResolver = new (fMemoryManager) XQillaGrammarResolver(fGrammarPool, fMemoryManager);
  fURIStringPool = fGrammarResolver->getStringPool();

  //  Create a scanner and tell it what validator to use. Then set us
  //  as the document event handler so we can fill the DOM document.
  fScanner = XERCES_CPP_NAMESPACE_QUALIFIER XMLScannerResolver::getDefaultScanner(fValidator, fGrammarResolver, fMemoryManager);
  fScanner->setDocHandler(this);
  fScanner->setDocTypeHandler(this);
  fScanner->setURIStringPool(fURIStringPool);

  // hold the loaded schemas in the cache, so that can be reused    
  fGrammarResolver->cacheGrammarFromParse(true);
  fGrammarResolver->useCachedGrammarInParse(true);
  cacheGrammarFromParse(true);

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

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheParser::parseWithContext(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource &source, DynamicContext *context)
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

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheParser::parseWithContext(const XMLCh* const uri, DynamicContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER InputSource* srcToUse = 0;
  if(getXMLEntityResolver()){
    XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier
      resourceIdentifier(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier::UnKnown, uri, 0,
                         XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context->getBaseURI());
    srcToUse = getXMLEntityResolver()->resolveEntity(&resourceIdentifier);
  }
  XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER InputSource> janIS(srcToUse);

  _context = context;
  try {
    if(srcToUse)
      parse(*srcToUse);
    else {
      // Resolve the uri against the base uri
      const XMLCh *systemId = uri;
      XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(context->getMemoryManager());
      if(urlTmp.setURL(context->getBaseURI(), uri, urlTmp)) {
        systemId = context->getMemoryManager()->getPooledString(urlTmp.getURLText());
      }

      parse(systemId);
    }
  }
  catch(...) {
    _context = 0;
    throw;
  }
  _context = 0;
  return adoptDocument();
}

void DocumentCacheParser::loadSchema(const XMLCh* const uri, const XMLCh* const location, StaticContext *context)
{
  // if we are requested to load the XMLSchema schema, just return
  if(XPath2Utils::equals(uri, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
    return;
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

  XERCES_CPP_NAMESPACE_QUALIFIER Grammar* grammar=NULL;
  if(srcToUse) {
    grammar=getScanner()->loadGrammar(*srcToUse, XERCES_CPP_NAMESPACE_QUALIFIER Grammar::SchemaGrammarType, true);
  }
  else if(location) {
    // Resolve the location against the base uri
    const XMLCh *systemId = location;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(context->getMemoryManager());
    if(urlTmp.setURL(context->getBaseURI(), location, urlTmp)) {
      systemId = urlTmp.getURLText();
    }

    grammar=getScanner()->loadGrammar(systemId, XERCES_CPP_NAMESPACE_QUALIFIER Grammar::SchemaGrammarType, true);
  }
  if(grammar==NULL)
    XQThrow2(StaticErrorException,X("DocumentCacheParser::loadSchema"), X("Schema not found [err:XQST0059]"));
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
        if(node->dmNodeKind() == Node::document_string)
        {
            Result children = node->dmChildren(context,0);
            Node::Ptr child;
            bool bSeenElement=false;
            while((child=children->next(context)).notNull())
            {
              if(child->dmNodeKind()==Node::element_string)
                if(bSeenElement)
                  XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), X("A document being validate must have exactly one child element [err:XQDY0061]"));
                else
                  bSeenElement=true;
              else if(child->dmNodeKind()!=Node::processing_instruction_string && 
                      child->dmNodeKind()!=Node::comment_string)
                XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), X("A document being validate can only have element, comments and processing instructions as children [err:XQDY0061]"));
            }
            if(!bSeenElement)
              XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), X("A document being validate must have exactly one child element [err:XQDY0061]"));
        }

        // if validation is strict, there must be a schema for the root node
        if(valMode==DocumentCache::VALIDATION_STRICT) {
            ATQNameOrDerived::Ptr name;
            if(node->dmNodeKind() == Node::document_string)
                name = ((const Node::Ptr)(node->dmChildren(context,0)->next(context)))->dmNodeName(context);
            else
                name = node->dmNodeName(context);
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
                XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), msg.getRawBuffer());
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
                                                                    false,
                                                                    context->getMemoryManager());
        inputSrc.setCopyBufToStream(false);
        inputSrc.setEncoding(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgUTF16EncodingString);
        // - parse the text (with validation on)
        XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* pDoc=parseWithContext(inputSrc, context);
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        setIdentityConstraintChecking(true);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release((void**)&serializedForm, context->getMemoryManager() );
        // - return the document element
        if(pDoc==NULL)
            return NULL;
        // if we were asked to validate a document node, return the document; otherwise, return the root node
        if(node->dmNodeKind() == Node::document_string)
            return new NodeImpl(pDoc, context);
        else
        {
          // the newly returned node has no parent
          return new NodeImpl(pDoc->removeChild(pDoc->getDocumentElement()), context);
        }
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER SAXException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.getMessage());
        exc_msg.append(X(" [err:XQDY0027]"));
        XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
    }
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.msg);
        exc_msg.append(X(" [err:XQDY0027]"));
        XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
        setValidationConstraintFatal(false);
        setValidationScheme(oldValScheme);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_msg(1023, context->getMemoryManager());
        exc_msg.set(X("Validation failed: "));
        exc_msg.append(toCatch.getMessage());
        exc_msg.append(X(" [err:XQDY0027]"));
        XQThrow2(DynamicErrorException,X("DocumentCacheParser::validate"), exc_msg.getRawBuffer());
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
  _loadedSchemas(0),
  _memMgr(memMgr)
{
    _loadedSchemas=new (_memMgr) XERCES_CPP_NAMESPACE_QUALIFIER XMLStringPool(3, _memMgr);
}

DocumentCacheImpl::DocumentCacheImpl(const DocumentCacheImpl *parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : _parser(parent->_parser, memMgr),
    _loadedSchemas(0),
    _memMgr(memMgr)
{
}

DocumentCacheImpl::~DocumentCacheImpl()
{
  if(_loadedSchemas)
	  delete _loadedSchemas;
}

void DocumentCacheImpl::setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler)
{
  _parser.setXMLEntityResolver(handler);
}

XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* DocumentCacheImpl::getXMLEntityResolver() const
{
    return const_cast<XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver*>(_parser.getXMLEntityResolver());
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheImpl::loadXMLDocument(XERCES_CPP_NAMESPACE_QUALIFIER
                                                                               InputSource& inputSource,
                                                                               DynamicContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;
  try {
    doc = _parser.parseWithContext(inputSource, context);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER SAXException& toCatch) {
    //TODO: Find a way to decipher whether the exception is actually because of a parsing problem or because the document can't be found
    XQThrow2(XMLParseException, X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
    XQThrow2(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.msg);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
    XQThrow2(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
  }
  return doc;
}
 
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *DocumentCacheImpl::loadXMLDocument(const XMLCh* uri, DynamicContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;
  try {
    doc = _parser.parseWithContext(uri, context);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER SAXException& toCatch) {
    //TODO: Find a way to decipher whether the exception is actually because of a parsing problem or because the document can't be found
    XQThrow2(XMLParseException, X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
    XQThrow2(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.msg);
  }
  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
    XQThrow2(XMLParseException,X("DocumentCacheImpl::loadXMLDocument"), toCatch.getMessage());
  }
  return doc;
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

  // xs:anyType matches anything
  if(XPath2Utils::equals(uriToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
     XPath2Utils::equals(typeNameToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_ANYTYPE))
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
  if(XPath2Utils::equals(typeNameToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYSIMPLETYPE) &&
     XPath2Utils::equals(uriToCheck, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
    return true;

  if(XPath2Utils::equals(typeNameToCheck, AnyAtomicType::fgDT_ANYATOMICTYPE) &&
     XPath2Utils::equals(uriToCheck, FunctionConstructor::XMLChXPath2DatatypesURI) )
    return dtvDerived->isAtomic();

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

  return false;
}

bool DocumentCacheImpl::isTypeDefined(const XMLCh* const uri, const XMLCh* const typeName) const
{
  if(getComplexTypeInfo(uri, typeName) != NULL)
      return true;

  if(_parser.getGrammarResolver()->getDatatypeValidator(uri,typeName) != NULL)
      return true;

  // these types are not present in the XMLSchema grammar, but they are defined
  if(XPath2Utils::equals(uri, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
  {
      return (XPath2Utils::equals(typeName, AnyAtomicType::fgDT_ANYATOMICTYPE) ||
              XPath2Utils::equals(typeName, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) ||
              XPath2Utils::equals(typeName, DocumentCacheParser::g_szUntyped) ||
              XPath2Utils::equals(typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_ANYTYPE));
  }
  return false;
}

void DocumentCacheImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations, StaticContext *context)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023,context->getMemoryManager());
  if(_loadedSchemas->exists(uri))
  {
    buf.set(X("More than one 'import schema' specifies the same target namespace \""));
    buf.append(uri);
    buf.append(X("\" [err:XQST0058]"));
    XQThrow2(StaticErrorException,X("DocumentCacheImpl::addSchemaLocation"), buf.getRawBuffer());
  }
  _loadedSchemas->addOrFind(uri);

  bool bFoundSchema=false;
  if(locations==NULL) {
    // if no locations are given, try to see if the entity resolver can still find it
    try {
      _parser.loadSchema(uri, 0, context);
      bFoundSchema = true;
    } catch(XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e) {
      buf.set(X("An error occurred while trying to load the schema for namespace \""));
      buf.append(uri);
      buf.append(X("\": "));
      buf.append(e.getMessage());
      buf.append(X(" [err:XQST0059]"));
    }
  }
  else {
    for(VectorOfStrings::iterator it=locations->begin(); it!=locations->end(); it++) {
      try {
        _parser.loadSchema(uri, *it, context);
        bFoundSchema = true;
        break;
      } catch(XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e) {
        buf.set(X("An error occurred while trying to load the schema for namespace \""));
        buf.append(uri);
        buf.append(X("\" from \""));
        buf.append(*it);
        buf.append(X("\": "));
        buf.append(e.getMessage());
        buf.append(X(" [err:XQST0059]"));
      }
    }
  }
  if(!bFoundSchema)
  {
    if(buf.isEmpty())
      XQThrow2(StaticErrorException,X("DocumentCacheImpl::addSchemaLocation"), X("Schema not found [err:XQST0059]"));
    else
      XQThrow2(StaticErrorException,X("DocumentCacheImpl::addSchemaLocation"), buf.getRawBuffer());
  }
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
