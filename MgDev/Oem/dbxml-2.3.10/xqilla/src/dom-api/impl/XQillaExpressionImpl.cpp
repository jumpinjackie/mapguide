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
 * $Id: XQillaExpressionImpl.cpp,v 1.10.2.2 2007/01/19 18:52:17 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include "XQillaExpressionImpl.hpp"
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/dom-api/XQillaExpression.hpp>
#include "XPath2ResultImpl.hpp"
#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/XQillaException.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/XercesDefs.hpp>

XQillaExpressionImpl::XQillaExpressionImpl(const XMLCh *expression,
                                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                                           const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsr,
                                           XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGP)
	: _createdWith(memMgr)
{
  try {
    _staticContext = new (_createdWith) XQContextImpl(_createdWith, xmlGP);
    if(nsr != 0) _staticContext->setNSResolver(nsr);
    _compiledExpression = XQilla::parse(expression, XQilla::XPATH2, _staticContext, NULL, XQilla::NO_ADOPT_CONTEXT,
                                        _createdWith);
  }
  catch(XQException &e) {
    if(XQillaException::getDebug()) {
      e.printDebug(X("Caught exception at Interface"));
    }
    throw XQillaException(e);
  }
}

XQillaExpressionImpl::~XQillaExpressionImpl() 
{
  delete _staticContext;
  delete _compiledExpression;
}

void XQillaExpressionImpl::release()
{
  this->~XQillaExpressionImpl();
  _createdWith->deallocate(this);
}

void* XQillaExpressionImpl::evaluate(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                                     unsigned short type,
                                     void*) const
{
  switch((XPath2Result::ResultType)type) {
  case XPath2Result::FIRST_RESULT: {
    return new (_createdWith) XPath2FirstResultImpl(_compiledExpression, contextNode,
                                                    _staticContext, _createdWith);
    break;
  }
  case XPath2Result::ITERATOR_RESULT: {
    return new (_createdWith) XPath2IteratorResultImpl(_compiledExpression, contextNode,
                                                       _staticContext, _createdWith);
    break;
  }
  case XPath2Result::SNAPSHOT_RESULT: {
    return new (_createdWith) XPath2SnapshotResultImpl(_compiledExpression, contextNode,
                                                       _staticContext, _createdWith);
    break;
  }
  }
  return 0;
}

void* XQillaExpressionImpl::evaluateOnce(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                                         unsigned short type,
                                         void*)
{
  switch((XPath2Result::ResultType)type) {
  case XPath2Result::FIRST_RESULT: {
    return new (_createdWith) XPath2FirstResultImpl(_compiledExpression, contextNode,
                                                    _staticContext, _createdWith, this);
    break;
  }
  case XPath2Result::ITERATOR_RESULT: {
    return new (_createdWith) XPath2IteratorResultImpl(_compiledExpression, contextNode,
                                                       _staticContext, _createdWith, this);
    break;
  }
  case XPath2Result::SNAPSHOT_RESULT: {
    return new (_createdWith) XPath2SnapshotResultImpl(_compiledExpression, contextNode,
                                                       _staticContext, _createdWith, this);
    break;
  }
  }
  return 0;
}
