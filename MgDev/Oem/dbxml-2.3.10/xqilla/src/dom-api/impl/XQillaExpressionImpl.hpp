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
 * $Id: XQillaExpressionImpl.hpp,v 1.4 2006/11/01 16:37:18 jpcs Exp $
 */

#ifndef __XQILLAEXPRESSIONIMPL_HPP
#define __XQILLAEXPRESSIONIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/dom-api/XQillaExpression.hpp>
#include <xqilla/runtime/Sequence.hpp>

class XPath2Result;
class DynamicContext;
class XPath2MemoryManager;
class XQQuery;

XERCES_CPP_NAMESPACE_BEGIN
class XMLGrammarPool;
class DOMNode;
class DOMDocument;
class DOMException;
class DOMXPathNSResolver;
XERCES_CPP_NAMESPACE_END 

class XQILLA_API XQillaExpressionImpl : public XQillaExpression
{
public:

  XQillaExpressionImpl(const XMLCh *expression,
                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                       const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsr,
                       XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP);
  virtual ~XQillaExpressionImpl();

  virtual void* evaluate(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                         unsigned short type,
                         void* reuseableResult) const;
  virtual void* evaluateOnce(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                             unsigned short type,
                             void* reuseableResult);

  virtual void release();

  const XQQuery *getCompiledExpression() const { return _compiledExpression; }
  void setCompiledExpression(XQQuery *expr) { _compiledExpression = expr; }

private:
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _createdWith;

  DynamicContext *_staticContext;
  XQQuery* _compiledExpression;
}; //XQillaExpressionImpl


#endif //__XQILLAEXPRESSIONIMPL_HPP

