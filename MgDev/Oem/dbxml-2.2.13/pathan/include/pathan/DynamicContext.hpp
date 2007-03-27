/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DYNAMICCONTEXT_HPP
#define _DYNAMICCONTEXT_HPP

#include "StaticContext.hpp"
#include "ATDurationOrDerived.hpp"

class Sequence;
class Item;
class URIResolver;
class VariableStore;
class PathanFactory;

/// The execution time dynamic context interface
class PATHAN_EXPORT DynamicContext : public StaticContext
{
public:
  virtual ~DynamicContext() {};

  /** Resets the dynamic context, as if it had never been used */
  virtual void clearDynamicContext() = 0;

  /** Get the context Item */
  virtual const Item::Ptr &getContextItem() const = 0;
  /** Set the context item to item */
  virtual void setContextItem(const Item::Ptr &item) = 0;
  /** Sets the context item to an external document.
      This is needed so that the DOMDocument reference counting
      does not release the document that the node comes from. */
  virtual void setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) = 0;

  /** Get the context position */
  virtual unsigned int getContextPosition() const = 0;
  /** Set the context position */ 
  virtual void setContextPosition(unsigned int pos) = 0;

  /** Get the context size */
  virtual unsigned int getContextSize() const = 0;
  /** Set the context size */
  virtual void setContextSize(unsigned int size) = 0;
  
  /** get the variable store */
  virtual VariableStore* getVariableStore() = 0;

  /** Return the current time */
  virtual time_t getCurrentTime() const = 0;
  /** Set the current time */
  virtual void setCurrentTime(time_t newTime) = 0;

  /** Return the implicit timezone for this system.*/
  virtual const ATDurationOrDerived::Ptr &getImplicitTimezone() const = 0;
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
  virtual Sequence resolveDocument(const XMLCh* uri) = 0;
  /* Resolve the given uri (and baseUri) to a ist of DOMNode objects. If the uri
     is relative, the base uri is obtained from the context. */
  virtual Sequence resolveCollection(const XMLCh* uri) = 0;
  virtual Sequence resolveDefaultCollection() = 0;
  /** returns the validated node */
  virtual Node::Ptr validate(const Node::Ptr &node, DocumentCache::ValidationMode valMode) = 0;

  /** send a pair of strings to the "trace" data set */
  virtual void trace(const XMLCh* message1, const XMLCh* message2) = 0;
};

#endif
