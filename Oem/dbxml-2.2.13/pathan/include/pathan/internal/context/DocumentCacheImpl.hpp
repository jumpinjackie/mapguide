/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DOCUMENTCACHEIMPL_HPP
#define _DOCUMENTCACHEIMPL_HPP

#include <pathan/Pathan.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/util/RefHashTableOf.hpp>
#include <xercesc/util/ValueHashTableOf.hpp>

#include <pathan/DocumentCache.hpp>

class DynamicContext;
class QualifiedName;

XERCES_CPP_NAMESPACE_BEGIN
class DOMDocument;
class SchemaGrammar;
class ContentSpecNode;
class XMLGrammarPool;
XERCES_CPP_NAMESPACE_END

// convert  errors into exceptions
class PATHAN_EXPORT DocumentCacheErrorCatcher : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
{
public:
  // ErrorHandler interface

  void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
  void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
  void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
  void resetErrors();
protected:
};

// custom parser to manipulate the schema repository
class PATHAN_EXPORT DocumentCacheParser : public XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser
{
public:
  DocumentCacheParser(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr = 0);
  DocumentCacheParser(const DocumentCacheParser &parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  ~DocumentCacheParser();

  void init();

  /** Override the creation of the DOM Document, so that we can use our own memory manager */
  virtual void startDocument();

  /**
   * Overload the parse method, to create the document from a different memory manager.
   * NB the Grammar info in the tree will still be in the same memory manager as the parser.
   */
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *parseWithContext(const XMLCh* const uri, StaticContext *context);
  /**
   * Overload the parse method, to create the document from a different memory manager.
   * NB the Grammar info in the tree will still be in the same memory manager as the parser.
   */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *parseWithContext(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource &source, StaticContext *context);

  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* retrieveDocument(const XMLCh* uri);
  void storeDocument(const XMLCh* uri,XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document);
  void removeDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document);
  void clearStoredDocuments();

  XERCES_CPP_NAMESPACE_QUALIFIER GrammarResolver* getGrammarResolver() const
  {
    return XercesDOMParser::getGrammarResolver();
  }

  /** Pre parse the schema at the given uri */
  void loadSchema(const XMLCh* const uri, const XMLCh* location, StaticContext *context);
  unsigned int getSchemaUriId(const XMLCh* uri) const;
  const XMLCh* getSchemaUri(unsigned int id) const;

  XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl* getElementDecl(const XMLCh* elementUri, const XMLCh* elementName) const;

  XERCES_CPP_NAMESPACE_QUALIFIER SchemaAttDef* getAttributeDecl(const XMLCh* attributeUri, const XMLCh* attributeName) const;
  
  Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode, DynamicContext *context);

  virtual void error
  (
   const   unsigned int                errCode
   , const XMLCh* const                msgDomain
   , const XERCES_CPP_NAMESPACE_QUALIFIER XMLErrorReporter::ErrTypes  errType
   , const XMLCh* const                errorText
   , const XMLCh* const                systemId
   , const XMLCh* const                publicId
   , const XMLSSize_t                  lineNum
   , const XMLSSize_t                  colNum
   );

  /**
   * Create and return the xquery datatypes
   */
  static XERCES_CPP_NAMESPACE_QUALIFIER Grammar* createXQueryTypes(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm, XERCES_CPP_NAMESPACE_QUALIFIER GrammarResolver *gr);

  static const XMLCh g_szUntyped[];

protected:
  bool isChildElement( XERCES_CPP_NAMESPACE_QUALIFIER ContentSpecNode *topContentSpec, unsigned int uriId, const XMLCh* localPart ) const;

  DocumentCacheErrorCatcher _errorHandler;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument > _documentMap;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XMLCh > _uriMap;
  StaticContext *_context;
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _memMgr;
};

/// The class that performs the (cached) parsing of input documents
class PATHAN_EXPORT DocumentCacheImpl : public DocumentCache
{
public:
  DocumentCacheImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool* xmlgr = 0);
  DocumentCacheImpl(const DocumentCacheImpl *parent, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  ~DocumentCacheImpl();

  virtual void release();

  virtual void incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const;
  virtual void decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document, const StaticContext *context) const;

  /**
   * Sets the XMLEntityResolver that is used by Xerces when it is used
   * to parse documents. This affects the behaviour of Pathan whenever
   * it retrieves an XML, DTD or XML Schema grammar.
   */
  virtual void setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler);
  virtual XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* getXMLEntityResolver() const;

  /** load the DOM document from the requested URI (or get it from the cache) */
  Node::Ptr loadXMLDocument(const XMLCh* Uri, DynamicContext *context);

  /** Clear all the documents from the document cache */
  void clearStoredDocuments();

  /*
   * returns true if the type represented by uri:typename is an instance of uriToCheck:typeNameToCheck 
   *
   * ie: to check 
   * xs:integer instance of xs:decimal, 
   * call
   * isTypeOrDerivedFromType("xs", "integer", "xs", "decimal") 
   * (except of course, call with URIs, not prefixes!)
   */ 
  virtual bool isTypeOrDerivedFromType(const XMLCh* const uri, const XMLCh* const typeName, 
                                       const XMLCh* const uriToCheck, const XMLCh* const typeNameToCheck) const; 

  virtual void addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations, StaticContext *context);

  /** helper functions used to map namespace ids found in the SchemaGrammar **/
  virtual unsigned int getSchemaUriId(const XMLCh* uri) const;
  virtual const XMLCh* getSchemaUri(unsigned int id) const;

  virtual Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode, DynamicContext *context);

  virtual XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator*  getDatatypeValidator(const XMLCh* uri, const XMLCh* typeName) const;

  virtual XERCES_CPP_NAMESPACE_QUALIFIER ComplexTypeInfo*  getComplexTypeInfo(const XMLCh* uri, const XMLCh* typeName) const;

  virtual XERCES_CPP_NAMESPACE_QUALIFIER SchemaElementDecl* getElementDecl(const XMLCh* elementUri, const XMLCh* elementName) const;

  virtual XERCES_CPP_NAMESPACE_QUALIFIER SchemaAttDef* getAttributeDecl(const XMLCh* attributeUri, const XMLCh* attributeName) const;

  virtual DocumentCache *createDerivedCache(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const;

protected:
  class DocRefCount {
  public:
    DocRefCount() : doc(0), ref_count(1), next(0) {}

    const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
    unsigned int ref_count;
    DocRefCount *next;
  };

  DocumentCacheParser _parser;
  DocRefCount *_firstDocRefCount;
//   XERCES_CPP_NAMESPACE_QUALIFIER ValueHashTableOf<unsigned int> _docRefCountMap; // mutable
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _memMgr;
};

#endif


