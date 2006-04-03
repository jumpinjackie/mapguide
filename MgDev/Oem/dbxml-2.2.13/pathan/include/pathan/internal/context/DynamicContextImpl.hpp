/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DYNAMICCONTEXTIMPL_HPP
#define _DYNAMICCONTEXTIMPL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/StaticContext.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include "../ProxyMemoryManager.hpp"

/// Stores the DynamicContext information, and proxies the calls for StaticContext information to the given StaticContext
class PATHAN_EXPORT DynamicContextImpl : public DynamicContext
{
public:
  DynamicContextImpl(const StaticContext *staticContext, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  virtual ~DynamicContextImpl();

  virtual void release();

  const StaticContext *getStaticContext() const;
  virtual DynamicContext *createDynamicContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr
                                               = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;
  
  //////////////////////////////////
  // Dynamic Context Accessors    //
  //////////////////////////////////

  /** Redefined in this class */
  virtual void clearDynamicContext();

  /** Returns the PathanFactory for this context, which is used to create Pathan items and other objects. */
  virtual PathanFactory *getPathanFactory() const;
  /** Sets the PathanFactory for this context */
  virtual void setPathanFactory(PathanFactory *factory);

  /** Redefined in this class */
  virtual const Item::Ptr &getContextItem() const;
  /** Redefined in this class */
  virtual void setContextItem(const Item::Ptr &item);
  /** Sets the context item to an external document.
      This is needed so that the DOMDocument reference counting
      does not release the document that the node comes from. */
  virtual void setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

  /** Redefined in this class */
  virtual unsigned int getContextPosition() const;
  /** Redefined in this class */
  virtual void setContextPosition(unsigned int pos);

  /** Redefined in this class */
  virtual unsigned int getContextSize() const;
  /** Redefined in this class */
  virtual void setContextSize(unsigned int size);
  
  /** Redefined in this class */
  virtual VariableStore* getVariableStore();

  /** Redefined in this class */
  virtual time_t getCurrentTime() const;
  /** Redefined in this class */
  virtual void setCurrentTime(time_t newTime);

  /** Redefined in this class */
  virtual const ATDurationOrDerived::Ptr &getImplicitTimezone() const;
  /** Redefined in this class */
  virtual void setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration);

  /** Redefined in this class */
  virtual void registerURIResolver(URIResolver *resolver);
  /** Redefined in this class, uses the dynamic document cache */
  virtual Sequence resolveDocument(const XMLCh* uri);
  /** Redefined in this class, uses the dynamic document cache */
  virtual Sequence resolveCollection(const XMLCh* uri);
  virtual Sequence resolveDefaultCollection();

  /** Redefined in this class */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *createNewDocument() const;
  virtual void releaseDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) const;

  /** Redefined in this class */
  virtual void trace(const XMLCh* message1, const XMLCh* message2);

  //////////////////////////////////
  // Static Context Accessors     //
  //////////////////////////////////

  /** Get the static type of the context item */
  virtual const StaticType &getContextItemType() const;
  /** Set the static type of the context item */
  virtual void setContextItemType(const StaticType &st);

  /** Proxied to the underlying static context */
  virtual bool getXPath1CompatibilityMode() const;
  /** Throws exception, as the static context is read only */
  virtual void setXPath1CompatibilityMode(bool newMode);

  /** Redefined in this class */
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* getNSResolver() const;
  /** Redefined in this class */
  virtual const XMLCh* getUriBoundToPrefix(const XMLCh* prefix) const;
  /** Redefined in this class */
  virtual const XMLCh* getPrefixBoundToUri(const XMLCh* uri) const;
  /** Redefined in this class */
  virtual void setNSResolver(const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver);

  /** Proxied to the underlying static context */
  virtual const XMLCh* getDefaultElementAndTypeNS() const;
  /** Throws exception, as the static context is read only */
  virtual void setDefaultElementAndTypeNS(const XMLCh* newNS);

  /** Proxied to the underlying static context */
  virtual const XMLCh* getDefaultFuncNS() const;
  /** Throws exception, as the static context is read only */
  virtual void setDefaultFuncNS(const XMLCh* newNS);

  /** Proxied to the underlying static context */
  virtual const DocumentCache* getDocumentCache() const;
  /** returns the validated node */
  virtual Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode);
  /** Proxied to the underlying static context */
  virtual bool isTypeOrDerivedFromType(const XMLCh* const uri, const XMLCh* const typeName, const XMLCh* const uriToCheck, const XMLCh* const typeNameToCheck) const;
  /** Redefined in this class, sets the dynamic document cache's XMLEntityResolver */
  void setXMLEntityResolver(XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver* const handler);
  /** Throws exception, as the static context is read only */
  virtual void addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations);
  
  /** Throws exception, as the static context is read only */
  virtual VariableTypeStore* getVariableTypeStore();

  /** Throws exception, as the static context is read only */
  virtual void addCustomFunction(FuncFactory *func);
  /** Proxied to the underlying static context */
  virtual DataItem* lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfDataItems& v) const;

  /** Proxied to the underlying static context */
  virtual Collation* getCollation(const XMLCh* const URI) const;
  /** Throws exception, as the static context is read only */
  virtual void addCollation(Collation* collation);
  
  /** Proxied to the underlying static context */
  virtual Collation* getDefaultCollation() const;
  /** Throws exception, as the static context is read only */
  virtual void setDefaultCollation(const XMLCh* const URI);

  /** Proxied to the underlying static context */
  virtual const XMLCh* getBaseURI() const;
  /** Throws exception, as the static context is read only */
  virtual void setBaseURI(const XMLCh* newURI);

  /** Return the ordering method for node sets */
  virtual NodeSetOrdering getNodeSetOrdering() const;
  /** Set the ordering method for node sets */
  virtual void setNodeSetOrdering(NodeSetOrdering newOrder);

  /////////////////////////////////////////
  //  Pathan context specific accessors  //
  /////////////////////////////////////////

  /** Redefined in this class */
  virtual XPath2MemoryManager* getMemoryManager() const;
  /** Redefined in this class */
  virtual void setMemoryManager(XPath2MemoryManager* memMgr);

protected:
  const StaticContext *_staticContext;

  // The memory manager used to create this DynamicContextImpl
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *_createdWith;

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
  PathanFactory* _pathanFactory;

  /// A stack of URIResolver pointers
  std::vector<URIResolver *, PathanAllocator<URIResolver*> > _resolvers;

  /** Contains the XMLGrammarPool of the StaticContext, and is used to
   * load xml documents for resolveCollection and resolveDocument */
  DocumentCache* _docCache; // mutable

  // used for memory management
  XPath2MemoryManager* _memMgr;
};

inline const StaticContext *DynamicContextImpl::getStaticContext() const { return _staticContext; }

inline void DynamicContextImpl::setContextItemType(const StaticType &st)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::setXPath1CompatibilityMode(bool newMode)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::setDefaultFuncNS(const XMLCh* newNS)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::setBaseURI(const XMLCh* newURI)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::setNodeSetOrdering(NodeSetOrdering newOrder)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::addCustomFunction(FuncFactory *func)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::setDefaultElementAndTypeNS(const XMLCh* newNS)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::addCollation(Collation* collation)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline void DynamicContextImpl::addSchemaLocation(const XMLCh* uri, VectorOfStrings* locations)
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }
inline VariableTypeStore* DynamicContextImpl::getVariableTypeStore()
{ DSLthrow(ContextException,X("DynamicContextImpl"), X("You cannot change the static context when using a proxying dynamic context")); }

inline const StaticType &DynamicContextImpl::getContextItemType() const { return _staticContext->getContextItemType(); }
inline bool DynamicContextImpl::getXPath1CompatibilityMode() const { return _staticContext->getXPath1CompatibilityMode(); }
inline const XMLCh* DynamicContextImpl::getDefaultFuncNS() const { return _staticContext->getDefaultFuncNS(); }
inline const XMLCh* DynamicContextImpl::getBaseURI() const { return _staticContext->getBaseURI(); }
inline StaticContext::NodeSetOrdering DynamicContextImpl::getNodeSetOrdering() const { return _staticContext->getNodeSetOrdering(); }
inline const XMLCh* DynamicContextImpl::getDefaultElementAndTypeNS() const { return _staticContext->getDefaultElementAndTypeNS(); }
inline Collation* DynamicContextImpl::getCollation(const XMLCh* URI) const { return _staticContext->getCollation(URI); }
inline DataItem* DynamicContextImpl::lookUpFunction(const XMLCh* prefix, const XMLCh* name, VectorOfDataItems& v) const { return _staticContext->lookUpFunction(prefix, name, v); }
inline bool DynamicContextImpl::isTypeOrDerivedFromType(const XMLCh* uri, const XMLCh* typeName, const XMLCh* uriToCheck, const XMLCh* typeNameToCheck) const { return _staticContext->isTypeOrDerivedFromType(uri, typeName, uriToCheck, typeNameToCheck); }

#endif
