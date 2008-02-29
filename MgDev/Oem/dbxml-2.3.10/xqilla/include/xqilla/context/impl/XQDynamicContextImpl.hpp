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
 * $Id: XQDynamicContextImpl.hpp,v 1.22 2006/11/01 16:37:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQDynamicContextImpl.h: proxies calls to the embedded XQStaticContext
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQDYNAMICCONTEXTIMPL_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
#define AFXQ_XQDYNAMICCONTEXTIMPL_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/framework/ProxyMemoryManager.hpp>
#include <xqilla/exceptions/ContextException.hpp>

class XQILLA_API XQDynamicContextImpl : public DynamicContext
{
public:
  XQDynamicContextImpl(const StaticContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  ~XQDynamicContextImpl();

  virtual DynamicContext *createModuleContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr =
                                              XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;
  virtual DynamicContext *createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr
                                               = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;

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

  virtual void incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const;
  virtual void decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const;
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* retrieveDocument(const XMLCh* uri);
  virtual void storeDocument(const XMLCh* uri,XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document);

  /** Resets the dynamic context, as if it had never been used */
  virtual void clearDynamicContext();

  /** Returns the ItemFactory for this context, which is used to create XQilla items and other objects. */
  virtual ItemFactory *getItemFactory() const;
  /** Sets the ItemFactory for this context */
  virtual void setItemFactory(ItemFactory *factory);

  /** Get the context Item */
  virtual Item::Ptr getContextItem() const;
  /** Set the context item to item */
  virtual void setContextItem(const Item::Ptr &item);
  /** Sets the context item to an external document.
      This is needed so that the DOMDocument reference counting
      does not release the document that the node comes from. */
  virtual void setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

  /** Get the context position */
  virtual unsigned int getContextPosition() const;
  /** Set the context position */
  virtual void setContextPosition(unsigned int pos);

  /** Get the context size */
  virtual unsigned int getContextSize() const;
  /** Set the context size */
  virtual void setContextSize(unsigned int size);

  /** get the variable store */
  virtual VariableStore* getVariableStore();

  /** Return the current time */
  virtual time_t getCurrentTime() const;
  /** Set the current time */
  virtual void setCurrentTime(time_t newTime);

  /** Return the implicit timezone for this system.*/
  virtual ATDurationOrDerived::Ptr getImplicitTimezone() const;
  /** Set the implicit timezone */
  virtual void setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration);

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

  /** Used whenever we need to create a new document (including parsing in documents) */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *createNewDocument() const;
  virtual void releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const;

  /** send a pair of strings to the "trace" data set */
  virtual void trace(const XMLCh* message1, const XMLCh* message2);

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
  /** sets the repository for the grammars **/
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

  /** Register a callback object for resolving module URIs */
  virtual void setModuleResolver(ModuleResolver *resolver);
  /** Returns the module resolver currently set */
  virtual ModuleResolver *getModuleResolver() const;
  /* Retrieve a list of locations for the given module URI */
  virtual VectorOfStrings* resolveModuleURI(const XMLCh* uri) const;

  /** add the location for the grammar of a specific namespace **/
  virtual void addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations);

  /** get the variable type store */
  virtual VariableTypeStore* getVariableTypeStore();

  /** adds a custom function to the function table */
  virtual void addCustomFunction(FuncFactory *func);
	/** returns a function with name name in the namespace represented by prefix */
  virtual ASTNode* lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v,
                                  const LocationInfo *location) const;

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
  virtual NodeSetOrdering getNodeSetOrdering() const;
  /** Set the ordering method for node sets */
  virtual void setNodeSetOrdering(NodeSetOrdering newOrder);

  /** Return the base URI */
  virtual const XMLCh* getBaseURI() const;
  /** Set the base URI */
  virtual void setBaseURI(const XMLCh* newURI);

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

  /////////////////////////////////////////
  //  XQilla context specific accessors  //
  /////////////////////////////////////////

  /** Get the memory manager */
  virtual XPath2MemoryManager* getMemoryManager() const;

protected:
  const StaticContext *_staticContext;

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

  /** Default element/type namespace. This is a namespace URI or "none". This
   * namespace is used for any unprefixed QName appearing in a
   * position where an element or type name is expected.
   * THIS OUGHT TO BE IN THE STATIC CONTEXT - jpcs */
  const XMLCh* _defaultElementNS;

  /** The context item is the item currently being processed. */
  Item::Ptr _contextItem;

  /** The context position is the position of  the context item within
   * the sequence of items currently being processed. */
  unsigned int _contextPosition;

  /** The context size is the number of items in the sequence of
   * items currently being processed. */
  unsigned int _contextSize;

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

  /** Implicit timezone. This is the timezone to be used when a date,
   * time, or dateTime value that does not have a timezone is used in a
   * comparison or in any other operation. This value is an instance of
   * xdt:dayTimeDuration */
  ATDurationOrDerived::Ptr _implicitTimezone;

  /** For each atomic type in the in-scope type  definitions, there is 
   * a constructor function in the in-scope functions. Constructor 
   * functions are discussed in 3.10.4 Constructor Functions */
  ItemFactory* _itemFactory;

  /// A stack of URIResolver pointers
  std::vector<URIResolver *, XQillaAllocator<URIResolver*> > _resolvers;

  /** Contains the XMLGrammarPool of the StaticContext, and is used to
   * load xml documents for resolveCollection and resolveDocument */
  DocumentCache* _docCache;

  class DocRefCount {
  public:
    DocRefCount() : doc(0), ref_count(1), next(0) {}

    const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
    unsigned int ref_count;
    DocRefCount *next;
  };

  DocRefCount *_firstDocRefCount;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument > _documentMap;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XMLCh > _uriMap;

  // used for memory management
  XPath2MemoryManager* _memMgr;

  /////////////////////////////////////////
  //  XQuery dynamic context variables   //
  /////////////////////////////////////////

  bool m_bEnableDebugging;

  XQDebugCallback* m_pDebugCallback;
};


inline void XQDynamicContextImpl::setContextItemType(const StaticType &)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setXPath1CompatibilityMode(bool newMode)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setBaseURI(const XMLCh* newURI)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::addCustomFunction(FuncFactory *func)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::addCollation(Collation* collation)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline VariableTypeStore* XQDynamicContextImpl::getVariableTypeStore()
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); return 0; }
inline void XQDynamicContextImpl::setConstructionMode(StaticContext::ConstructionMode newMode)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setPreserveBoundarySpace(bool value)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setDefaultFLWOROrderingMode(StaticContext::FLWOROrderingMode newMode)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setInheritNamespaces(bool value) 
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setPreserveNamespaces(bool value) 
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void XQDynamicContextImpl::setModuleResolver(ModuleResolver *resolver)
{ XQThrow2(ContextException,X("XQDynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }

inline const StaticType &XQDynamicContextImpl::getContextItemType() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline const XMLCh* XQDynamicContextImpl::getDefaultFuncNS() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline StaticContext::NodeSetOrdering XQDynamicContextImpl::getNodeSetOrdering() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline ModuleResolver * XQDynamicContextImpl::getModuleResolver() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline ASTNode* XQDynamicContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfASTNodes& v, const LocationInfo *location) const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline StaticContext::FLWOROrderingMode XQDynamicContextImpl::getDefaultFLWOROrderingMode() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline bool XQDynamicContextImpl::getXPath1CompatibilityMode() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }
inline bool XQDynamicContextImpl::getPreserveBoundarySpace() const { XQThrow2(ContextException,X("XQDynamicContextImpl"), X("The static context is not available from a proxying dynamic context")); }


inline const XMLCh* XQDynamicContextImpl::getBaseURI() const { return _staticContext->getBaseURI(); }
inline Collation* XQDynamicContextImpl::getCollation(const XMLCh* URI, const LocationInfo *location) const { return _staticContext->getCollation(URI, location); }
inline bool XQDynamicContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const { return _staticContext->isTypeOrDerivedFromType(uri, typeName, uriToCheck, typeNameToCheck); }
inline bool XQDynamicContextImpl::getInheritNamespaces() const { return _staticContext->getInheritNamespaces(); }
inline bool XQDynamicContextImpl::getPreserveNamespaces() const { return _staticContext->getPreserveNamespaces(); }
inline StaticContext::ConstructionMode XQDynamicContextImpl::getConstructionMode() const { return _staticContext->getConstructionMode(); }

#endif // !defined(AFXQ_XQDYNAMICCONTEXTIMPL_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
