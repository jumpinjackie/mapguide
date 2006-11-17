/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/internal/dom-extensions/PathanExpressionImpl.hpp>
#include <pathan/PathanEngine.hpp>
#include <pathan/Node.hpp>
#include <pathan/Item.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanExpression.hpp>
#include <pathan/XPath2Result.hpp>
#include <pathan/dataItem/DataItem.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/XPath2MemoryManager.hpp>

#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/ContextException.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/XercesDefs.hpp>

PathanExpressionImpl::PathanExpressionImpl(const XMLCh *expression,
                                           const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* documentRoot,
                                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                                           const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsr,
                                           XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP)
	: _createdWith(memMgr),
    _memMgr(memMgr),
    _docRoot(documentRoot),
    _staticContextOwned(true)
{
  _staticContext = PathanEngine::createContext(xmlGP, &_memMgr);
  _staticContext->setNSResolver(nsr);
  _compiledExpression = PathanEngine::createDataItem(expression, _staticContext, &_memMgr, true);
}

PathanExpressionImpl::PathanExpressionImpl(const XMLCh *expression, DynamicContext *context,
                                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
	: _createdWith(memMgr),
    _memMgr(memMgr),
    _docRoot(0),
    _staticContextOwned(false),
    _staticContext(context)
{
  _compiledExpression = PathanEngine::createDataItem(expression, _staticContext, &_memMgr, true);
}

PathanExpressionImpl::~PathanExpressionImpl() 
{
  if(_staticContextOwned) _staticContext->release();
}//destructor

void PathanExpressionImpl::release()
{
  this->~PathanExpressionImpl();
  _createdWith->deallocate(this);
}

// weak version -  must create a context from scratch
void* PathanExpressionImpl::evaluate(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                                     unsigned short type,
                                     void*) const
  throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException)
{
  return new (_createdWith) XPath2Result((XPath2Result::ResultType)type, _compiledExpression,
                                         contextNode, _staticContext, _createdWith);
}

// strong version, use given context
XPath2Result* PathanExpressionImpl::evaluate(DynamicContext* context, unsigned short type) const
	throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException)
{
  return new (_createdWith) XPath2Result((XPath2Result::ResultType)type, _compiledExpression,
                                         context, _createdWith);
}

DynamicContext *PathanExpressionImpl::createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm) const
{
  if(mm == 0) {
    return 0;
  }
  return _staticContext->createDynamicContext(mm);
}
