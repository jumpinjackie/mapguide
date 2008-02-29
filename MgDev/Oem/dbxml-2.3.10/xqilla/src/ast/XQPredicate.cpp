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
 * $Id: XQPredicate.cpp,v 1.8 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/runtime/SequenceResult.hpp>

XQPredicate::XQPredicate(ASTNode *predicate, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(0),
    predicate_(predicate)
{
  setType(ASTNode::PREDICATE);
}

XQPredicate::XQPredicate(ASTNode* expr, ASTNode *predicate, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    expr_(expr),
    predicate_(predicate)
{
  setType(ASTNode::PREDICATE);
}

ASTNode* XQPredicate::staticResolution(StaticContext *context)
{
  expr_ = expr_->staticResolution(context);

  {
    AutoNodeSetOrderingReset orderReset(context);
    predicate_ = predicate_->staticResolution(context);
  }

  return this;
}

ASTNode* XQPredicate::staticTyping(StaticContext *context)
{
  _src.clear();

  XPath2MemoryManager *mm = context->getMemoryManager();

  expr_ = expr_->staticTyping(context);
  _src.copy(expr_->getStaticResolutionContext());

  if(expr_->getType() == SEQUENCE &&
     ((XQSequence*)expr_)->getItemConstructors().empty()) {
    return expr_;
  }

  AutoContextItemTypeReset contextTypeReset(context, expr_->getStaticResolutionContext().getStaticType());

  predicate_ = predicate_->staticTyping(context);

  const StaticResolutionContext &newSrc = predicate_->getStaticResolutionContext();

  if(!newSrc.isUsed() && !predicate_->isSingleNumericConstant(context)) {
    // It's not a numeric constant
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(mm);
    if(predicate_->collapseTree(dContext)->getEffectiveBooleanValue(dContext, this)) {
      // We have a true predicate
      return expr_;
    }
    else {
      // We have a false predicate, which is constant folded to an empty sequence
      ASTNode *result = new (mm) XQSequence(mm);
      result->setLocationInfo(expr_);
      return result;
    }
  }

  // Remove context item usage
  _src.addExceptContextFlags(newSrc);

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

Result XQPredicate::createResult(DynamicContext* context, int flags) const
{
  const StaticResolutionContext &src = predicate_->getStaticResolutionContext();

  Result parent = expr_->collapseTree(context, flags);

  unsigned int contextSize = 0;
  if(src.isContextSizeUsed()) {
    // We need the context size, so convert to a Sequence to work it out
    Sequence seq(parent->toSequence(context));
    contextSize = seq.getLength();
    parent = new SequenceResult(this, seq);
  }
  if(src.getStaticType().isType(StaticType::NUMERIC_TYPE) &&
     !src.isContextItemUsed() && !src.isContextPositionUsed()) {
    // It only contains numeric type results, and doesn't use the context
    // item or position
    return new NumericPredicateFilterResult(parent, predicate_, contextSize);
  }
  else if(!src.getStaticType().containsType(StaticType::NUMERIC_TYPE)) {
    // It only contains non-numeric results
    return new NonNumericPredicateFilterResult(parent, predicate_, contextSize);
  }
  else {
    return new PredicateFilterResult(parent, predicate_, contextSize);
  }
}

ASTNode *XQPredicate::addPredicates(ASTNode *expr, VectorOfPredicates *preds)
{
  VectorOfPredicates::iterator i = preds->begin();
  VectorOfPredicates::iterator end = preds->end();
  for(; i != end; ++i) {
    (*i)->setExpression(expr);
    expr = *i;
  }
  return expr;
}

/////////////////////////////////////
// PredicateFilterResult
/////////////////////////////////////

XQPredicate::PredicateFilterResult::PredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize)
  : ResultImpl(pred),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextPos_(0),
    contextSize_(contextSize)
{
}

Item::Ptr XQPredicate::PredicateFilterResult::next(DynamicContext *context)
{
  AutoContextInfoReset autoReset(context);

  bool contextUsed = pred_->getStaticResolutionContext().isContextItemUsed() ||
    pred_->getStaticResolutionContext().isContextPositionUsed();

  Item::Ptr result = 0;
  while(result == NULLRCP) {
    result = parent_->next(context);
    if(result == NULLRCP) {
      parent_ = 0;
      return 0;
    }

    ++contextPos_;

    if(todo_ || contextUsed) {
      todo_ = false;

      context->setContextSize(contextSize_);
      context->setContextPosition(contextPos_);
      context->setContextItem(result);

      Result pred_result = pred_->collapseTree(context);
      first_ = pred_result->next(context);
      if(first_ != NULLRCP) {
        second_ = pred_result->next(context);
      }

      autoReset.resetContextInfo();
    }

    // 3.2.2 ...
    // The predicate truth value is derived by applying the following rules, in order:
    // 1) If the value of the predicate expression is an atomic value of a numeric type, the predicate truth
    // value is true if and only if the value of the predicate expression is equal to the context position.
    if(first_ != NULLRCP && second_ == NULLRCP && first_->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)first_)->isNumericValue()) {
      const Numeric::Ptr num = (const Numeric::Ptr)first_;
      if(!num->equals((const AnyAtomicType::Ptr)context->getItemFactory()->createInteger((long)contextPos_, context), context)) {
        result = 0;
      }
      else if(!contextUsed) {
        parent_ = 0;
      }
    }
    else {
      // 2) Otherwise, the predicate truth value is the effective boolean value of the predicate expression
      if(!ResultImpl::getEffectiveBooleanValue(first_, second_, context, this)) {
        result = 0;
      }
    }
  }

  return result;
}

void XQPredicate::PredicateFilterResult::skip()
{
  parent_->skip();
}

std::string XQPredicate::PredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  return "predicatefilterresult";
}

/////////////////////////////////////
// NonNumericPredicateFilterResult
/////////////////////////////////////

XQPredicate::NonNumericPredicateFilterResult::NonNumericPredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize)
  : ResultImpl(pred),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextPos_(0),
    contextSize_(contextSize)
{
}

Item::Ptr XQPredicate::NonNumericPredicateFilterResult::next(DynamicContext *context)
{
  AutoContextInfoReset autoReset(context);

  bool contextUsed = pred_->getStaticResolutionContext().isContextItemUsed() ||
    pred_->getStaticResolutionContext().isContextPositionUsed();

  Item::Ptr result = 0;
  while(result == NULLRCP) {
    result = parent_->next(context);
    if(result == NULLRCP) {
      parent_ = 0;
      return 0;
    }

    ++contextPos_;

    if(todo_ || contextUsed) {
      todo_ = false;

      context->setContextSize(contextSize_);
      context->setContextPosition(contextPos_);
      context->setContextItem(result);

      // 2) Otherwise, the predicate truth value is the effective boolean value of the predicate expression
      if(!pred_->collapseTree(context)->getEffectiveBooleanValue(context, this)) {
        result = 0;
        if(!contextUsed) {
          parent_ = 0;
        }
      }

      autoReset.resetContextInfo();
    }
  }

  return result;
}

void XQPredicate::NonNumericPredicateFilterResult::skip()
{
  parent_->skip();
}

std::string XQPredicate::NonNumericPredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  return "nonnumericpredicatefilterresult";
}

/////////////////////////////////////
// NumericPredicateFilterResult
/////////////////////////////////////

XQPredicate::NumericPredicateFilterResult::NumericPredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize)
  : ResultImpl(pred),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextSize_(contextSize)
{
}

Item::Ptr XQPredicate::NumericPredicateFilterResult::next(DynamicContext *context)
{
  if(todo_) {
    todo_ = false;

    ItemFactory* pFactory=context->getItemFactory();

    AutoContextInfoReset autoReset(context);
    context->setContextSize(contextSize_);

    // Set the context item to something other than null,
    // since fn:last() checks to see that there is actually
    // a context item
    context->setContextItem(pFactory->createInteger(1, context));

    Result pred_result = pred_->collapseTree(context);
    Numeric::Ptr first = (Numeric::Ptr)pred_result->next(context);
    if(first == NULLRCP) {
      // The effective boolean value is therefore false
      parent_ = 0;
      return 0;
    }

    Item::Ptr second = pred_result->next(context);
    if(second != NULLRCP) {
      // The effective boolean value causes an error -
      // so call it to get the correct error
      parent_ = 0;
      ResultImpl::getEffectiveBooleanValue(first, second, context, this);
      return 0;
    }

    autoReset.resetContextInfo();

    int pos = 1;
    while(pFactory->createInteger(pos, context)->lessThan(first, context) && parent_->next(context) != NULLRCP) {
      pos++;
    }

    if(pFactory->createInteger(pos, context)->equals(first, context)) {
      Item::Ptr result = parent_->next(context);
      parent_ = 0;
      return result;
    }

    parent_ = 0;
  }

  return 0;
}

void XQPredicate::NumericPredicateFilterResult::skip()
{
  parent_->skip();
}

std::string XQPredicate::NumericPredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  return "numericpredicatefilterresult";
}
