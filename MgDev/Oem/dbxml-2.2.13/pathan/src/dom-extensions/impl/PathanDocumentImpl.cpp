/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "PathanDocumentImpl.hpp"

#include <pathan/internal/dom-extensions/PathanExpressionImpl.hpp>
#include <pathan/PathanExpression.hpp>

#include <pathan/PathanNSResolver.hpp>
#include <pathan/XPath2Result.hpp>
#include <pathan/DynamicContext.hpp>

#include <simpleVariables/VarStoreImpl.hpp>
#include <parser/XPath2ParserControl.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/ContextException.hpp>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathEvaluator.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/dom/DOMException.hpp>

#include <pathan/XPath2Utils.hpp>

PathanDocumentImpl::PathanDocumentImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr)
  : XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl(memMgr), _memMgr(memMgr), _xmlGrammarPool(0)
{
  // Nothing
}//constructor

PathanDocumentImpl::PathanDocumentImpl(const XMLCh *fNamespaceURI, const XMLCh *qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *doctype,
                                       XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr) 
  : XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl(fNamespaceURI, qualifiedName, doctype, memMgr), _memMgr(memMgr), _xmlGrammarPool(0)
{
  // Nothing
}

PathanDocumentImpl::~PathanDocumentImpl()
{
  // Nothing
}//destructor

// weak version, create a context within
const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathExpression* PathanDocumentImpl::createExpression(const XMLCh* expression, 
                                                                                              const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver)
  throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException)
{
  return new (&_memMgr) PathanExpressionImpl(expression, this, &_memMgr, resolver, _xmlGrammarPool);
}

// weak version, create context within
void* PathanDocumentImpl::evaluate(const XMLCh* expression,
                                   XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                                   const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver,
                                   unsigned short type,
                                   void* reuseableResult)
  throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException) 
{
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathExpression* expr=createExpression(expression, resolver);
  return expr->evaluate(contextNode, type, reuseableResult);
}

/** Create an NSResolver */
const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* PathanDocumentImpl::createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* nodeResolver) {
  return _memMgr.createNSResolver(nodeResolver);
}

void PathanDocumentImpl::setGrammarPool(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGrammarPool) {
    _xmlGrammarPool = xmlGrammarPool;
}

XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *PathanDocumentImpl::getGrammarPool() {
  return _xmlGrammarPool;
}
