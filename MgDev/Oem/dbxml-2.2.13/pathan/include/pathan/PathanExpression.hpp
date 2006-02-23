/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANEXPRESSION_HPP
#define __PATHANEXPRESSION_HPP

#include <pathan/Pathan.hpp>

#include "XPath2Result.hpp"
#include <pathan/Sequence.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMXPathExpression.hpp>
#include <xercesc/dom/DOMXPathException.hpp>


class DynamicContext;

/**
 * The <code>PathanExpression</code> interface extends the <code>DOMXPathExpression</code> interface, 
 * providing the ability to evaluate against a given context. 
 * @since DOM Level 3
 */
class PATHAN_EXPORT PathanExpression: public XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathExpression
{
public:
  // -----------------------------------------------------------------------
  //  All constructors are hidden, just the destructor is available
  // -----------------------------------------------------------------------
  /** @name Destructor */
  //@{
  /**
   * Destructor
   *
   */
  virtual ~PathanExpression() {}
  //@}
  
  // -----------------------------------------------------------------------
  // Virtual PathanExpression interface
  // -----------------------------------------------------------------------
  /** @name Pathan extension */
  //@{  
  /**
   * Evaluates this XPath expression and returns a result.
   * @param contextNode of type <code>DOMNode</code> The context
   * node for the evaluation of this XPath expression.
   * If the XPathEvaluator was obtained by casting the Document then this must 
   * be owned by the same document and must be a <code>DOMDocument</code>, <code>DOMElement</code>, 
   * <code>DOMAttribute</code>, <code>DOMText</code>, <code>DOMCDATASection</code>, 
   * <code>DOMComment</code>, <code>DOMProcessingInstruction</code>, or 
   * <code>XPathNamespace</code>. If the context node is a <code>DOMText</code> or a 
   * <code>DOMCDATASection</code>, then the context is interpreted as the whole logical
   * text node as seen by XPath, unless the node is empty in which case it may not
   * serve as the XPath context.
   * @param context of type <code>DynamicContext*</code> The context to evaluate the given expression against.
   * @param type of type unsigned short If a specific type is specified, then the result
   * will be coerced to return the specified type relying on XPath conversions and fail 
   * if the desired coercion is not possible. This must be one of the type codes of <code>XPath2Result</code>.
   * @return XPath2Result* The result of the evaluation of the XPath expression. The memory
   * returned must be managed by the user.
   * @exception DOMXPathException
   * TYPE_ERR: Raised if the result cannot be converted to return the specified type.
   * @exception DOMException
   * WRONG_DOCUMENT_ERR: The <code>DOMNode</code> is from a document that is not supported by 
   * the <code>XPathEvaluator</code> that created this <code>DOMXPathExpression</code>.
   * NOT_SUPPORTED_ERR: The Node is not a type permitted as an XPath context node or the 
   * request type is not permitted by this <code>DOMXPathExpression</code>.
   */
  virtual XPath2Result* evaluate(DynamicContext* context,
                                 unsigned short type) const
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) = 0;

  /**
   * Evaluates this XPath expression and returns a result.
   * @param contextNode of type <code>DOMNode</code> The context is context
   * node for the evaluation of this XPath expression.
   * If the XPathEvaluator was obtained by casting the Document then this must 
   * be owned by the same document and must be a <code>DOMDocument</code>, <code>DOMElement</code>, 
   * <code>DOMAttribute</code>, <code>DOMText</code>, <code>DOMCDATASection</code>, 
   * <code>DOMComment</code>, <code>DOMProcessingInstruction</code>, or 
   * <code>XPathNamespace</code>. If the context node is a <code>DOMText</code> or a 
   * <code>DOMCDATASection</code>, then the context is interpreted as the whole logical
   * text node as seen by XPath, unless the node is empty in which case it may not
   * serve as the XPath context.
   * @param type of type unsigned short If a specific type is specified, then the result
   * will be coerced to return the specified type relying on XPath conversions and fail 
   * if the desired coercion is not possible. This must be one of the type codes of <code>XPathResult</code>.
   * @param result of type void* The result specifies a specific result object which
   * may be reused and returned by this method. If this is specified as nullor the 
   * implementation does not reuse the specified result, a new result object will be constructed
   * and returned.
   * For XPath 1.0 results, this object will be of type <code>XPathResult</code>.
   * @return void* The result of the evaluation of the XPath expression.
   * For XPath 1.0 results, this object will be of type <code>XPathResult</code>.
   * @exception XPathException
   * TYPE_ERR: Raised if the result cannot be converted to return the specified type.
   * @exception DOMException
   * WRONG_DOCUMENT_ERR: The <code>DOMNode</code> is from a document that is not supported by 
   * the <code>XPathEvaluator</code> that created this <code>DOMXPathExpression</code>.
   * NOT_SUPPORTED_ERR: The Node is not a type permitted as an XPath context node or the 
   * request type is not permitted by this <code>DOMXPathExpression</code>.
   */
  virtual void* evaluate(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                         unsigned short type,
                         void* reuseableResult) const
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) = 0;

  /**
   * Creates a DynamicContext object suitable for passing to the extended evaluate methods.
   * @param mm of type <code>XPath2MemoryManager</code> - The memory manager used to create the
   * DynamicContext if present. If not, the DynamicContext is owned by the PathanExpression
   * object, and is released when the PathanExpression is deleted.
   */
  virtual DynamicContext *createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const = 0;

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
  PathanExpression() {};
  //@}
};//PathanExpression

#endif //__PATHANEXPRESSION_HPP

