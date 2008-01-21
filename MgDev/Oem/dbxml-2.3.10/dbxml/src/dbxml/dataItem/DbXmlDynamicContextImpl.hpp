//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlDynamicContextImpl.hpp,v 1.27.2.1 2007/01/03 19:21:13 jsnelson Exp $
//

#ifndef __DBXMLDYNAMICCONTEXTIMPL_HPP
#define	__DBXMLDYNAMICCONTEXTIMPL_HPP

#include "DbXmlContext.hpp"
#include <xqilla/context/impl/XQDynamicContextImpl.hpp>

namespace DbXml
{

class DbXmlDynamicContextImpl : public DbXmlContext
{
public:
  DbXmlDynamicContextImpl(QueryContext &qc, Transaction *txn, const DbXmlContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  virtual ~DbXmlDynamicContextImpl();

  virtual DynamicContext *createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
                                              XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;
  virtual DynamicContext *createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr
                                               = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;

  /** Overridden to use an XmlDocument, and add it to the ReferenceMinder */
  virtual XERCES_CPP_NAMESPACE::DOMDocument *createNewDocument() const;
  virtual void releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const;

  //////////////////////////////////////
  // XQuery Static Context Accessors  //
  //////////////////////////////////////

  /**
   * Overrides the inherited method, and disowns any output
   * document that was created with the previous memory manager.
   */
  virtual void setMemoryManager(XPath2MemoryManager *memMgr);

  /** Binds a prefix to a namespace URI */
  virtual void setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri);

  /** Set the policy for boundary space */
  virtual void setPreserveBoundarySpace(bool value);
  /** Get the policy for boundary space */
  virtual bool getPreserveBoundarySpace() const;

  /** Return the default ordering mode for FLWOR blocks */
  virtual FLWOROrderingMode getDefaultFLWOROrderingMode() const;
  /** Set the default ordering mode for FLWOR blocks */
  virtual void setDefaultFLWOROrderingMode(FLWOROrderingMode newMode);

  /** Set the policy for namespace inheritance */
  virtual void setInheritNamespaces(bool value);
  /** Get the policy for namespace inheritance */
  virtual bool getInheritNamespaces() const;

  /** Set the policy for namespace copy */
  virtual void setPreserveNamespaces(bool value);
  /** Get the policy for namespace copy */
  virtual bool getPreserveNamespaces() const;

  //////////////////////////////////////
  // XQuery Dynamic Context Accessors //
  //////////////////////////////////////

  /** Set the object to be used for debugging callbacks */
  virtual void setDebugCallback(XQDebugCallback* callback);
  /** Get the object to be used for debugging callbacks */
  virtual XQDebugCallback* getDebugCallback() const;

  virtual void enableDebugging(bool enable=true);
  virtual bool isDebuggingEnabled() const;

  //////////////////////////////////
  // Dynamic Context Accessors    //
  //////////////////////////////////

  /** Resets the dynamic context, as if it had never been used */
  virtual void clearDynamicContext();

  /** Returns the XQillaFactory for this context, which is used to create XQilla items and other objects. */
  virtual ItemFactory *getItemFactory() const;
  /** Sets the ItemFactory for this context */
  virtual void setItemFactory(ItemFactory *factory);

  /** get the variable store */
  virtual VariableStore* getVariableStore();

  /** Return the current time */
  virtual time_t getCurrentTime() const;
  /** Set the current time */
  virtual void setCurrentTime(time_t newTime);

  /**
     Register a callback object for resolving URIs. Multiple URIResolver
     objects can be registered with the DynamicContext at the same time.
     The most recently added URIResolver will be called first, and if it
     returns NULL, subsequent ones will be called - ending in a call to
     the fallback routines.

     The DynamicContext will not
     adopt this object, making the user responsible for making sure that the
     URIResolver object's lifespan matches or exceeds the life of the
     DynamicContext.
  */
  virtual void registerURIResolver(URIResolver *resolver);
  /* Resolve the given uri (and baseUri) to a DOMDocument. If the uri
     is relative, the base uri is obtained from the context. */
  virtual Sequence resolveDocument(const XMLCh* uri, const LocationInfo *location);
  /* Resolve the given uri (and baseUri) to a list of DOMNode objects. If
     the uri is relative, the base uri is obtained from the context. */
  virtual Sequence resolveCollection(const XMLCh* uri, const LocationInfo *location);
  virtual Sequence resolveDefaultCollection();

  //////////////////////////////////
  // Static Context Accessors     //
  //////////////////////////////////

  /** Get the static type of the context item */
  virtual const StaticType &getContextItemType() const;
  /** Set the static type of the context item */
  virtual void setContextItemType(const StaticType &st);

  /** Get the current XPath 1.0 compatibility mode */
  virtual bool getXPath1CompatibilityMode() const;
  /** Set the current XPath 1.0 compatibility mode */
  virtual void setXPath1CompatibilityMode(bool newMode);

  /** Get the NS resolver */
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* getNSResolver() const;
  /** returns the URI that is bound in prefix in the current scope or zero
      length string otherwise */
  virtual const XMLCh* getUriBoundToPrefix(const XMLCh* prefix, const LocationInfo *location) const;
  /** returns the prefix that is bound in uri in the current scope or zero
      length string otherwise */
  virtual const XMLCh* getPrefixBoundToUri(const XMLCh* uri) const;
  /** Set the NS resolver */
  virtual void setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver);

  /** get the value of the default namespace for elements and types */
  virtual const XMLCh* getDefaultElementAndTypeNS() const;
  /** set the value of the default namespace for elements and types */
  virtual void setDefaultElementAndTypeNS(const XMLCh* newNS);

  /** Return the default namespace for functions */
  virtual const XMLCh* getDefaultFuncNS() const;
  /** Set the default namespace for functions */
  virtual void setDefaultFuncNS(const XMLCh* newNS);

  /** retrieve the repository for the grammars **/
  virtual const DocumentCache* getDocumentCache() const;
  virtual void setDocumentCache(DocumentCache* docCache);
  /** returns the validated node */
  virtual Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode);
  /** returns true if the type represented by uri:typename is an instance of uriToCheck:typeNameToCheck
   *
   * ie: to check
   * xs:integer instance of xs:decimal,
   * call
   * isTypeOrDerivedFromType("xs", "integer", "xs", "decimal")
   * (except of course, call with URIs, not prefixes!)
   */
  virtual bool isTypeOrDerivedFromType(const XMLCh* const uri, const XMLCh* const typeName, const XMLCh* const uriToCheck, const XMLCh* const typeNameToCheck) const;
  /**
   * Sets the XMLEntityResolver that is used by Xerces when it is used
   * to parse documents. This affects the behaviour of XQilla whenever
   * it retrieves a DTD or XML Schema grammar.
   */
  virtual void setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler);
  /** Returns the entity resolver currently set */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* getXMLEntityResolver() const;

  /** add the location for the grammar of a specific namespace **/
  virtual void addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations);

  /** get the variable type store */
  virtual VariableTypeStore* getVariableTypeStore();

  /** adds a custom function to the function table */
  virtual void addCustomFunction(FuncFactory *func);
	/** returns a function with name name in the namespace represented by prefix */
  virtual ASTNode* lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v, const LocationInfo *location) const;

  /** Get the implementation for the specified collation */
  virtual Collation* getCollation(const XMLCh* const URI, const LocationInfo *location) const;
  /** Add a collation	*/
  virtual void addCollation(Collation* collation);

  /** Get the default collation */
  virtual Collation* getDefaultCollation(const LocationInfo *location) const;
  /** Specify which collation is the default one */
  virtual void setDefaultCollation(const XMLCh* const URI);

  /** Return the construction mode */
  virtual ConstructionMode getConstructionMode() const;
  /** Set the construction mode */
  virtual void setConstructionMode(ConstructionMode newMode);

  /** Return the ordering method for node sets */
  virtual StaticContext::NodeSetOrdering getNodeSetOrdering() const;
  /** Set the ordering method for node sets */
  virtual void setNodeSetOrdering(StaticContext::NodeSetOrdering newOrder);

  /** Return the base URI */
  virtual const XMLCh* getBaseURI() const;
  /** Set the base URI */
  virtual void setBaseURI(const XMLCh* newURI);

  /////////////////////////////////////////
  //  XQilla context specific accessors  //
  /////////////////////////////////////////

  /** Get the memory manager */
  virtual XPath2MemoryManager* getMemoryManager() const;

protected:
  const DbXmlContext *_staticContext;

  // The memory manager used to create this context
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _createdWith;

  // used for managing the memory of objects inside this context
  ProxyMemoryManager _internalMM;

  /** In-scope namespaces. This is a set of  (prefix, URI) pairs.
   * The in-scope namespaces are used for resolving prefixes used
   * in QNames within the expression
   * THIS OUGHT TO BE IN THE STATIC CONTEXT - jpcs */
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* _nsResolver;

  /** Default collation. This is a collation. This collation is used by
   * string comparison functions when no explicit collation is specified.
   * THIS OUGHT TO BE IN THE STATIC CONTEXT - jpcs */
  const XMLCh* _defaultCollation;

  /** Ordering mode, which has the value ordered or unordered, affects
   *  the ordering of the result sequence returned by path expressions,
   *  union, intersect, and except expressions, and FLWOR expressions
   *  that have no order by clause.
   * THIS OUGHT TO BE IN THE STATIC CONTEXT - jpcs */
  NodeSetOrdering _ordering;

  /** Default element/type namespace. This is a namespace URI or "none". This
   * namespace is used for any unprefixed QName appearing in a
   * position where an element or type name is expected.
   * THIS OUGHT TO BE IN THE STATIC CONTEXT - jpcs */
  const XMLCh* _defaultElementNS;

  /** In-scope variable values. This is a set of  (QName, Sequence) pairs.
   * It defines the set of variables that have been declared and are
   * available for reference within the expression. The QName
   * represents the name of the variable, and the Sequence represents its
   * value */
  VariableStore* _varStore;

  /** Current date and time. This information  represents an
   * implementation-dependent point in time during processing of a query
   * or transformation */
  time_t _currentTime;

  /** For each atomic type in the in-scope type  definitions, there is 
   * a constructor function in the in-scope functions. Constructor 
   * functions are discussed in 3.10.4 Constructor Functions */
  ItemFactory* _itemFactory;

  /// A stack of URIResolver pointers
  std::vector<URIResolver *, XQillaAllocator<URIResolver*> > _resolvers;

  /** Contains the XMLGrammarPool of the StaticContext, and is used to
   * load xml documents for resolveCollection and resolveDocument */
  DocumentCache* _docCache; // mutable

  // used for memory management
  XPath2MemoryManager* _memMgr;

  /////////////////////////////////////////
  //  XQuery dynamic context variables   //
  /////////////////////////////////////////

  bool m_bEnableDebugging;

  XQDebugCallback* m_pDebugCallback;
};

inline void DbXmlDynamicContextImpl::setContextItemType(const StaticType &)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setXPath1CompatibilityMode(bool newMode)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setBaseURI(const XMLCh* newURI)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::addCustomFunction(FuncFactory *func)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::addCollation(Collation* collation)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline VariableTypeStore* DbXmlDynamicContextImpl::getVariableTypeStore()
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setConstructionMode(StaticContext::ConstructionMode newMode)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setPreserveBoundarySpace(bool value)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setDefaultFLWOROrderingMode(StaticContext::FLWOROrderingMode newMode)
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setInheritNamespaces(bool value) 
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DbXmlDynamicContextImpl::setPreserveNamespaces(bool value) 
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline DynamicContext *DbXmlDynamicContextImpl::createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr) const
{ XQThrow2(ContextException,X("DbXmlDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }

inline const StaticType &DbXmlDynamicContextImpl::getContextItemType() const { return _staticContext->getContextItemType(); }
inline bool DbXmlDynamicContextImpl::getXPath1CompatibilityMode() const { return _staticContext->getXPath1CompatibilityMode(); }
inline const XMLCh* DbXmlDynamicContextImpl::getDefaultFuncNS() const { return _staticContext->getDefaultFuncNS(); }
inline const XMLCh* DbXmlDynamicContextImpl::getBaseURI() const { return _staticContext->getBaseURI(); }
inline StaticContext::FLWOROrderingMode DbXmlDynamicContextImpl::getDefaultFLWOROrderingMode() const { return _staticContext->getDefaultFLWOROrderingMode(); }
inline bool DbXmlDynamicContextImpl::getInheritNamespaces() const { return _staticContext->getInheritNamespaces(); }
inline bool DbXmlDynamicContextImpl::getPreserveNamespaces() const { return _staticContext->getPreserveNamespaces(); }

inline Collation* DbXmlDynamicContextImpl::getCollation(const XMLCh* URI, const LocationInfo *location) const { return _staticContext->getCollation(URI, location); }
inline ASTNode* DbXmlDynamicContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v, const LocationInfo *location) const { return _staticContext->lookUpFunction(prefix, name, v, location); }
inline bool DbXmlDynamicContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const { return _staticContext->isTypeOrDerivedFromType(uri, typeName, uriToCheck, typeNameToCheck); }

inline StaticContext::ConstructionMode DbXmlDynamicContextImpl::getConstructionMode() const { return _staticContext->getConstructionMode(); }
inline bool DbXmlDynamicContextImpl::getPreserveBoundarySpace() const { return _staticContext->getPreserveBoundarySpace(); }

}

#endif
