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
 * $Id: StaticContext.hpp,v 1.16 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _STATICCONTEXT_HPP
#define _STATICCONTEXT_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <time.h>
#include <xqilla/schema/DocumentCache.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMemory.hpp>

// Copied from ASTNode.hpp
typedef std::vector<ASTNode*,XQillaAllocator<ASTNode*> > VectorOfASTNodes;

class DynamicContext;
class XPath2MemoryManager;
class FuncFactory;
class DatatypeFactory;
class Collation;
class VariableTypeStore;
class ItemFactory;
class StaticType;
class ModuleResolver;

XERCES_CPP_NAMESPACE_BEGIN
class DOMDocument;
class DOMXPathNSResolver;
class XMLEntityResolver;
XERCES_CPP_NAMESPACE_END

/// The parse time static context interface
class XQILLA_API StaticContext : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  typedef enum {
      ORDERING_ORDERED,
      ORDERING_UNORDERED
  } NodeSetOrdering;

  typedef enum {
    CONSTRUCTION_MODE_PRESERVE,
    CONSTRUCTION_MODE_STRIP
  } ConstructionMode;

  typedef enum {
    FLWOR_ORDER_EMPTY_GREATEST,
    FLWOR_ORDER_EMPTY_LEAST
  } FLWOROrderingMode;

  virtual ~StaticContext() {};

  virtual DynamicContext *createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
                                              XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const = 0;
  virtual DynamicContext *createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
                                               XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const = 0;
  
  /** Returns the ItemFactory for this context, which is used to create XQilla items and other objects. */
  virtual ItemFactory *getItemFactory() const = 0;
  /** Sets the ItemFactory for this context */
  virtual void setItemFactory(ItemFactory *factory) = 0;

  /** Get the static type of the context item */
  virtual const StaticType &getContextItemType() const = 0;
  /** Set the static type of the context item */
  virtual void setContextItemType(const StaticType &st) = 0;

  /** Get the current XPath 1.0 compatibility mode */
  virtual bool getXPath1CompatibilityMode() const = 0;
  /** Set the current XPath 1.0 compatibility mode */
  virtual void setXPath1CompatibilityMode(bool newMode) = 0;

  /** Get the NS resolver */
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* getNSResolver() const = 0;
  /** returns the URI that is bound in prefix in the current scope or zero
      length string otherwise */
  virtual const XMLCh* getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const = 0;
  /** returns the prefix that is bound in uri in the current scope or zero
      length string otherwise */
  virtual const XMLCh* getPrefixBoundToUri(const XMLCh* uri) const = 0;
  /** Set the NS resolver */
  virtual void setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver) = 0;
  /** Binds a prefix to a namespace URI */
  virtual void setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri) = 0;

  /** get the value of the default namespace for elements and types */
  virtual const XMLCh* getDefaultElementAndTypeNS() const = 0;
  /** set the value of the default namespace for elements and types */
  virtual void setDefaultElementAndTypeNS(const XMLCh* newNS) = 0;

  /** Return the default namespace for functions */
  virtual const XMLCh* getDefaultFuncNS() const = 0;
  /** Set the default namespace for functions */
  virtual void setDefaultFuncNS(const XMLCh* newNS) = 0;

  /** retrieve the repository for the grammars **/
  virtual const DocumentCache* getDocumentCache() const = 0;
  /** sets the repository for the grammars **/
  virtual void setDocumentCache(DocumentCache* docCache) = 0;

  /** returns true if the type represented by uri:typename is an instance of uriToCheck:typeNameToCheck 
   *
   * ie: to check 
   * xs:integer instance of xs:decimal, 
   * call
   * isTypeOrDerivedFromType("xs", "integer", "xs", "decimal") 
   * (except of course, call with URIs, not prefixes!)
   */
  virtual bool isTypeOrDerivedFromType(const XMLCh* const uri, const XMLCh* const typeName, const XMLCh* const uriToCheck, const XMLCh* const typeNameToCheck) const = 0;
  /**
   * Sets the XMLEntityResolver that is used by Xerces when it is used
   * to parse documents. This affects the behaviour of XQilla whenever
   * it retrieves a DTD or XML Schema grammar.
   */
  virtual void setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler) = 0;
  /** Returns the entity resolver currently set */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* getXMLEntityResolver() const = 0;

  /** Register a callback object for resolving module URIs */
  virtual void setModuleResolver(ModuleResolver *resolver) = 0;
  /** Returns the module resolver currently set */
  virtual ModuleResolver *getModuleResolver() const = 0;
  /* Retrieve a list of locations for the given module URI */
  virtual VectorOfStrings* resolveModuleURI(const XMLCh* uri) const = 0;

  /** add the location for the grammar of a specific namespace **/
  virtual void addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations) = 0;
  
  /** get the variable type store */
  virtual VariableTypeStore* getVariableTypeStore() = 0;

  /** adds a custom function to the function table */
  virtual void addCustomFunction(FuncFactory *func) = 0;
	/** returns a function with name name in the namespace represented by prefix */
  virtual ASTNode* lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v,
                                  const LocationInfo *location) const = 0;

  /** Get the implementation for the specified collation */
  virtual Collation* getCollation(const XMLCh* const URI, const LocationInfo *location) const = 0;
  /** Add a collation	*/
  virtual void addCollation(Collation* collation) = 0;
  
  /** Get the default collation */
  virtual Collation* getDefaultCollation(const LocationInfo *location) const = 0;
  /** Specify which collation is the default one */
  virtual void setDefaultCollation(const XMLCh* const URI) = 0;

  /** Return the base URI */
  virtual const XMLCh* getBaseURI() const = 0;
  /** Set the base URI */
  virtual void setBaseURI(const XMLCh* newURI) = 0;

  /** Return the ordering method for node sets */
  virtual NodeSetOrdering getNodeSetOrdering() const = 0;
  /** Set the ordering method for node sets */
  virtual void setNodeSetOrdering(NodeSetOrdering newOrder) = 0;

  /** Return the construction mode */
  virtual ConstructionMode getConstructionMode() const = 0;
  /** Set the construction mode */
  virtual void setConstructionMode(ConstructionMode newMode) = 0;

  /** Set the policy for boundary space */
  virtual void setPreserveBoundarySpace(bool value) = 0;
  /** Get the policy for boundary  space */
  virtual bool getPreserveBoundarySpace() const = 0;

  /** Return the default ordering mode for FLWOR blocks */
  virtual FLWOROrderingMode getDefaultFLWOROrderingMode() const = 0;
  /** Set the default ordering mode for FLWOR blocks */
  virtual void setDefaultFLWOROrderingMode(FLWOROrderingMode newMode) = 0;

  /** Set the policy for namespace inheritance */
  virtual void setInheritNamespaces(bool value) = 0;
  /** Get the policy for namespace inheritance */
  virtual bool getInheritNamespaces() const = 0;

  /** Set the policy for namespace copy */
  virtual void setPreserveNamespaces(bool value) = 0;
  /** Get the policy for namespace copy */
  virtual bool getPreserveNamespaces() const = 0;

  /// The enableDebuging flag is considered to be in both the static and dynamic contexts
  virtual void enableDebugging(bool enable=true) = 0;
  /// The enableDebuging flag is considered to be in both the static and dynamic contexts
  virtual bool isDebuggingEnabled() const = 0;

  /////////////////////////////////////////
  //  XQilla context specific accessors  //
  /////////////////////////////////////////

  /** Used whenever we need to create a new document (including parsing in documents) */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *createNewDocument() const = 0;
  /** Used to release a document allocated through createNewDocument(). */
  virtual void releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const = 0;

  /** Get the memory manager */
  virtual XPath2MemoryManager* getMemoryManager() const = 0;
  /** Set the memory manager to the one given */
  virtual void setMemoryManager(XPath2MemoryManager* memMgr) = 0;
};

#endif
