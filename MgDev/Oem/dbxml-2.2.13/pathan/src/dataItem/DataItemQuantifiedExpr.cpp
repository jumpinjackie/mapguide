/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/dataItem/DataItemQuantifiedExpr.hpp>

#include <pathan/VariableStore.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/AnyAtomicTypeConstructor.hpp>
#include <pathan/PathanFactory.hpp>

DataItemQuantifiedExpr::DataItemQuantifiedExpr(DataItemQuantifiedExpr::QuantifierType qType, VectorOfBindings* bindings, DataItem* returnExpr, XPath2MemoryManager* memMgr)
  : DataItemFor(bindings, returnExpr, memMgr)
{
  _qType=qType;
  setType(DataItem::QUANTIFIED);
}

DataItem* DataItemQuantifiedExpr::staticResolution(StaticContext *context) {
  DataItemFor::staticResolutionImpl(context);

  _src.getStaticType().flags = StaticType::OTHER_TYPE;

  if(_src.isUsed()) {
    if(_returnExpr->isConstant()) {
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      bool value = _returnExpr->collapseTree(dContext).getEffectiveBooleanValue(dContext);

      DataItem* newBlock = new (getMemoryManager())
        DataItemSequence(dContext->getPathanFactory()->createBoolean(value, dContext),
                         dContext, getMemoryManager());
      newBlock->addPredicates(getPredicates());
      return newBlock->staticResolution(context);
    }
    else {
      return resolvePredicates(context);
    }
  }
  else {
    return constantFold(context);
  }
}

Result DataItemQuantifiedExpr::createResultImpl(Bindings::const_iterator it, Bindings::const_iterator end, DynamicContext* context, int flags) const
{
  return new QuantifiedResult(_qType, it, end, _returnExpr, context);
}

DataItemQuantifiedExpr::QuantifierType DataItemQuantifiedExpr::getQuantifierType() const {
  return _qType;
}

DataItemQuantifiedExpr::QuantifiedResult::QuantifiedResult(QuantifierType type, Bindings::const_iterator it, Bindings::const_iterator end,
                                                           const DataItem *returnExpr, DynamicContext *context)
  : SingleResult(context),
    _type(type),
    _returnExpr(returnExpr)
{
  for(; it != end; ++it) {
      _ebs.push_back(VBExecution(*it, context));
  }
}

Item::Ptr DataItemQuantifiedExpr::QuantifiedResult::getSingleResult(DynamicContext *context) const
{
  VariableStore* varStore = context->getVariableStore();
  bool defaultResult = (_type == some) ? false : true;

  ExecutionBindings &ebs = const_cast<ExecutionBindings&>(_ebs); // _ebs is mutable

  // Initialise and run the execution bindings
  varStore->addLogicalBlockScope();
  if(DataItemFor::nextState(ebs, context, true)) {
    do {
      bool result = _returnExpr->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context);
      if(defaultResult != result) {
        defaultResult = result;
        break;
      }
    } while(DataItemFor::nextState(ebs, context, false));
  }
  varStore->removeScope();

  return (const Item::Ptr)context->getPathanFactory()->createBoolean(defaultResult, context);
}

std::string DataItemQuantifiedExpr::QuantifiedResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<quantified/>" << std::endl;

  return oss.str();
}
