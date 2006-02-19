/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANNSRESOLVER_HPP
#define __PATHANNSRESOLVER_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/dom/DOMXPathNSResolver.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class XercesDefs;
class DOMNode;
XERCES_CPP_NAMESPACE_END

class XPath2MemoryManager;

/**
 * The <code>PathanNSResolver</code> interface extends the <code>DOMXPathNSResolver</code>
 * providing the ability to add additional namespace bindings.
 * @since DOM Level 3
 */
class PATHAN_EXPORT PathanNSResolver : public XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver
{
public:
  
  // -----------------------------------------------------------------------
  // Non-standard extension
  // -----------------------------------------------------------------------
  /** @name Non-standard extension */
  //@{
  
  /**
   * Non-standard extension
   *
   * Pathan requires additional bindings of namespaces.  This method will bind
   * the given prefix to the associated namespace.
   * @param prefix of type XMLCh - The prefix to bind.
   * @param URI of type XMLCh - The associated namespace.
   */
  virtual void addNamespaceBinding(const XMLCh* prefix, const XMLCh* uri) = 0;

  /** Look up the namespace URI associated to the given namespace prefix. 
   * The XPath evaluator must never call this with a null or empty argument, 
   * because the result of doing this is undefined.
   * @param prefix of type XMLCh - The prefix to look for.
   * @return the associated namespace URI or null if none is found.
   */
  virtual const XMLCh* lookupNamespaceURI(const XMLCh* prefix) const = 0;

  /**
   * Non-standard extension
   *
   * XPath2 implementations require a reverse lookup in the static context.
   * Look up the prefix associated with the namespace URI
   * The XPath evaluator must never call this with a null or empty argument, 
   * because the result of doing this is undefined.
   * @param URI of type XMLCh - The namespace to look for.
   * @return the associated prefix or null if none is found.
   */
  virtual const XMLCh* lookupPrefix(const XMLCh* uri) const = 0;
      
  //@}
  /** @name Non-standard Extension */
  //@{
  /**
   * Called to indicate that this object (and its associated children) is no longer in use
   * and that the implementation may relinquish any resources associated with it and
   * its associated children.
   *
   * Access to a released object will lead to unexpected result.
   */
  virtual void release() = 0;
  //@}

protected:
  // -----------------------------------------------------------------------
  //  Hidden constructors
  // -----------------------------------------------------------------------
  /** @name Hidden constructors */
  //@{    
	PathanNSResolver() {};
  //@}
};//PathanNSResolver


#endif //__PATHANNSRESOLVER_HPP
