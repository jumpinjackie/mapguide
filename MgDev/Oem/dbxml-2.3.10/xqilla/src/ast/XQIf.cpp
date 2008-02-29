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
 * $Id: XQIf.cpp,v 1.10 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/ast/XQIf.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>

XQIf::XQIf(ASTNode* test, ASTNode* whenTrue, ASTNode* whenFalse, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _test(test),
  _whenTrue(whenTrue),
  _whenFalse(whenFalse)
{
	setType(ASTNode::IF);
}

Result XQIf::createResult(DynamicContext* context, int flags) const
{
  return new IfResult(this, flags);
}

ASTNode* XQIf::staticResolution(StaticContext *context) {
  {
    AutoNodeSetOrderingReset orderReset(context);
    _test = _test->staticResolution(context);
  }

  _whenTrue = _whenTrue->staticResolution(context);
  _whenFalse = _whenFalse->staticResolution(context);

  return this;
}

ASTNode *XQIf::staticTyping(StaticContext *context)
{
  _src.clear();

  _test = _test->staticTyping(context);

  if(_test->isConstant()) {
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    if(_test->collapseTree(dContext)->getEffectiveBooleanValue(dContext, this)) {
      return _whenTrue->staticTyping(context);
    }
    else {
      return _whenFalse->staticTyping(context);
    }
  }
  else {
    _src.add(_test->getStaticResolutionContext());

    _whenTrue = _whenTrue->staticTyping(context);
    _src.getStaticType() = _whenTrue->getStaticResolutionContext().getStaticType();
    _src.setProperties(_whenTrue->getStaticResolutionContext().getProperties());
    _src.add(_whenTrue->getStaticResolutionContext());

    _whenFalse = _whenFalse->staticTyping(context);
    _src.getStaticType().typeUnion(_whenFalse->getStaticResolutionContext().getStaticType());
    _src.setProperties(_src.getProperties() & _whenFalse->getStaticResolutionContext().getProperties());
    _src.add(_whenFalse->getStaticResolutionContext());
  }

  return this;
}

const ASTNode *XQIf::getTest() const {
  return _test;
}

const ASTNode *XQIf::getWhenTrue() const {
  return _whenTrue;
}

const ASTNode *XQIf::getWhenFalse() const {
  return _whenFalse;
}

void XQIf::setTest(ASTNode *item)
{
  _test = item;
}

void XQIf::setWhenTrue(ASTNode *item)
{
  _whenTrue = item;
}

void XQIf::setWhenFalse(ASTNode *item)
{
  _whenFalse = item;
}

XQIf::IfResult::IfResult(const XQIf *di, int flags)
  : ResultImpl(di),
    _flags(flags),
    _di(di),
    _results(0)
{
}

Item::Ptr XQIf::IfResult::next(DynamicContext *context)
{
  if(_results.isNull()) {
    if(_di->getTest()->collapseTree(context)->getEffectiveBooleanValue(context, this)) {
      _results = _di->getWhenTrue()->collapseTree(context, _flags);
    }
    else {
      _results = _di->getWhenFalse()->collapseTree(context, _flags);
    }
  }

  return _results->next(context);
}

std::string XQIf::IfResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<if/>" << std::endl;

  return oss.str();
}
