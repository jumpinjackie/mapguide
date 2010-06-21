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
 * $Id: DynamicContext.hpp,v 1.15 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _DYNAMICCONTEXT_HPP
#define _DYNAMICCONTEXT_HPP

#include <xqilla/context/StaticContext.hpp>
#include <xqilla/context/XQDebugCallback.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>

class Sequence;
class Item;
class URIResolver;
class VariableStore;
class ItemFactory;

/// The execution time dynamic context interface
class XQILLA_API DynamicContext : public StaticContext
{
public:
  virtual ~DynamicContext() {};

  /** Register a new reference to the document */
  virtual void incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const = 0;
  /** Register a closed reference to the document */
  virtual void decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const = 0;

  /** Resets the dynamic context, as if it had never been used */
  virtual void clearDynamicContext() = 0;

  /** Get the context Item */
  virtual Item::Ptr getContextItem() const = 0;
  /** Set the context item to item */
  virtual void setContextItem(const Item::Ptr &item) = 0;
  /** Sets the context item to an external document.
      This is needed so that the DOMDocument reference counting
      does not release the document that the node comes from. */
  virtual void setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) = 0;

  /** Get the context position */
  virtual size_t getContextPosition() const = 0;
  /** Set the context position */ 
  virtual void setContextPosition(size_t pos) = 0;

  /** Get the context size */
  virtual size_t getContextSize() const = 0;
  /** Set the context size */
  virtual void setContextSize(size_t size) = 0;
  
  /** get the variable store */
  virtual VariableStore* getVariableStore() = 0;

  /** Return the current time */
  virtual time_t getCurrentTime() const = 0;
  /** Set the current time */
  virtual void setCurrentTime(time_t newTime) = 0;

  /** Return the implicit timezone for this system.*/
  virtual ATDurationOrDerived::Ptr getImplicitTimezone() const = 0;
  /** Set the implicit timezone */
  virtual void setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration) = 0;

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
  virtual void registerURIResolver(URIResolver *resolver) = 0;
  /* Resolve the given uri (and baseUri) to a DOMDocument. If the uri
     is relative, the base uri is obtained from the context. */
  virtual Sequence resolveDocument(const XMLCh* uri, const LocationInfo *location) = 0;
  /* Resolve the given uri (and baseUri) to a ist of DOMNode objects. If the uri
     is relative, the base uri is obtained from the context. */
  virtual Sequence resolveCollection(const XMLCh* uri, const LocationInfo *location) = 0;
  virtual Sequence resolveDefaultCollection() = 0;
  /** returns the validated node */
  virtual Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode) = 0;

  /** send a pair of strings to the "trace" data set */
  virtual void trace(const XMLCh* message1, const XMLCh* message2) = 0;

  /** Set the object to be used for debugging callbacks */
  virtual void setDebugCallback(XQDebugCallback* callback) = 0;
  /** Get the object to be used for debugging callbacks */
  virtual XQDebugCallback* getDebugCallback() const = 0;

  /**
     Test if the query should be interrupted, and throw if so.
     This method has a default implementation of no-op.
   */
  virtual void testInterrupt() const {} 
};

#endif
