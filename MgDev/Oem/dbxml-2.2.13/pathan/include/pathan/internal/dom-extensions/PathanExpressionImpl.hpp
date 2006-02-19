/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANEXPRESSIONIMPL_HPP
#define __PATHANEXPRESSIONIMPL_HPP

#include <pathan/Pathan.hpp>
#include <pathan/PathanExpression.hpp>
#include <pathan/Sequence.hpp>

class XPath2Result;
class DataItem;
class DynamicContext;
class XPath2MemoryManager;

XERCES_CPP_NAMESPACE_BEGIN
class XMLGrammarPool;
class DOMNode;
class DOMDocument;
class DOMException;
XERCES_CPP_NAMESPACE_END 

class PATHAN_EXPORT PathanExpressionImpl : public PathanExpression
{
public:

  PathanExpressionImpl(const XMLCh *expression,
                       const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* documentRoot,
                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                       const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsr,
                       XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP);
  PathanExpressionImpl(const XMLCh *expression, DynamicContext *context,
                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  virtual ~PathanExpressionImpl();

  // weak version -  must create a context from scratch
  virtual void* evaluate(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                         unsigned short type,
                         void* reuseableResult) const 
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  // strong version -- use the user's context
  virtual XPath2Result* evaluate(DynamicContext* context,
                                 unsigned short type) const
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);

  virtual DynamicContext *createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager) const;

  virtual void release();

  const DataItem *getCompiledExpression() const { return _compiledExpression; }
  void setCompiledExpression(DataItem *expr) { _compiledExpression = expr; }

private:
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _createdWith;
  ProxyMemoryManager _memMgr;

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* _docRoot;

  bool _staticContextOwned;
  DynamicContext *_staticContext;
	DataItem* _compiledExpression;
}; //PathanExpressionImpl


#endif //__PATHANEXPRESSIONIMPL_HPP

