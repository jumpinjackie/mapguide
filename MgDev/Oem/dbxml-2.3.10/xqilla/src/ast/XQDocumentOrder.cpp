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
 * $Id: XQDocumentOrder.cpp,v 1.5 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>

XQDocumentOrder::XQDocumentOrder(ASTNode* expr, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    unordered_(false)
{
  setType(ASTNode::DOCUMENT_ORDER);
}

ASTNode* XQDocumentOrder::staticResolution(StaticContext *context)
{
  unordered_ = context->getNodeSetOrdering() == StaticContext::ORDERING_UNORDERED;

  expr_ = expr_->staticResolution(context);

  return this;
}

ASTNode *XQDocumentOrder::staticTyping(StaticContext *context)
{
  _src.clear();

  expr_ = expr_->staticTyping(context);
  _src.getStaticType() = expr_->getStaticResolutionContext().getStaticType();
  _src.add(expr_->getStaticResolutionContext());

  // Check if nodes will be returned
  if(!_src.getStaticType().containsType(StaticType::NODE_TYPE)) {
    return expr_;
  }

  // Check if it's already in document order
  if((expr_->getStaticResolutionContext().getProperties() &
      StaticResolutionContext::DOCORDER) != 0) {
    return expr_;
  }

  _src.setProperties(expr_->getStaticResolutionContext().getProperties()
                     | StaticResolutionContext::DOCORDER
                     | StaticResolutionContext::GROUPED);

  if(expr_->isConstant()) {
    return constantFold(context);
  }
  return this;
}

Result XQDocumentOrder::createResult(DynamicContext* context, int flags) const
{
  if(unordered_) {
    return new UniqueNodesResult(this, expr_->collapseTree(context, flags), context);
  }
  else {
    return new DocumentOrderResult(this, expr_->collapseTree(context, flags), context);
  }
}

void DocumentOrderResult::getResult(Sequence &toFill, DynamicContext *context) const
{
  toFill = parent_->toSequence(context);
  parent_ = 0;
  toFill.sortIntoDocumentOrder(context);
}

Item::Ptr UniqueNodesResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_->next(context);

  if(nTypeOfItemsInLastStep_ == 0 && result.notNull())
	  nTypeOfItemsInLastStep_ = result->isNode() ? 1 : 2;

  if(nTypeOfItemsInLastStep_ == 1) {
	  while(result.notNull()) {
		  if(noDups_.insert(result).second) break;
		  else result = parent_->next(context);
	  }
  }

  return result;
}
