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
 * $Id: XQParenthesizedExpr.cpp,v 1.11 2006/11/02 15:07:56 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/ast/XQParenthesizedExpr.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>

XQParenthesizedExpr::XQParenthesizedExpr(XPath2MemoryManager* memMgr)
	: ASTNodeImpl(memMgr), _astNodes(XQillaAllocator<ASTNode*>(memMgr))
{
  setType(ASTNode::PARENTHESIZED);
}

Result XQParenthesizedExpr::createResult(DynamicContext* context, int flags) const
{
  return new ParenthesizedResult(this, flags);
}

void XQParenthesizedExpr::addItem(ASTNode* di) {

	_astNodes.push_back(di);
}

ASTNode* XQParenthesizedExpr::staticResolution(StaticContext *context) {
  // Return a blank XQSequence if we have no children
  if(_astNodes.empty()) {
    ASTNode *result = new (getMemoryManager()) XQSequence(getMemoryManager());
    result->setLocationInfo(this);
    return result;
  }

  // Dissolve ourselves if we have only one child
  if(_astNodes.size() == 1) {
    return _astNodes.front()->staticResolution(context);
  }

  for(VectorOfASTNodes::iterator i = _astNodes.begin(); i != _astNodes.end(); ++i) {
    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode* XQParenthesizedExpr::staticTyping(StaticContext *context) {
  _src.getStaticType().flags = 0;

  for(VectorOfASTNodes::iterator i = _astNodes.begin(); i != _astNodes.end(); ++i) {
    *i = (*i)->staticTyping(context);
    _src.getStaticType().typeUnion((*i)->getStaticResolutionContext().getStaticType());
    _src.add((*i)->getStaticResolutionContext());
  }

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

const VectorOfASTNodes &XQParenthesizedExpr::getChildren() const {
  return _astNodes;
}

XQParenthesizedExpr::ParenthesizedResult::ParenthesizedResult(const XQParenthesizedExpr *di, int flags)
  : ResultImpl(di),
    _flags(flags),
    _di(di),
    _i(di->getChildren().begin()),
    _result(0)
{
}

Item::Ptr XQParenthesizedExpr::ParenthesizedResult::next(DynamicContext *context)
{
  Item::Ptr item = _result->next(context);

  while(item == NULLRCP) {
    if(_i != _di->getChildren().end()) {
      _result = (*_i++)->collapseTree(context, _flags);
      item = _result->next(context);
    }
    else {
      _result  = 0;
      break;
    }
  }

  return item;
}

std::string XQParenthesizedExpr::ParenthesizedResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<parenthesized/>" << std::endl;

  return oss.str();
}
