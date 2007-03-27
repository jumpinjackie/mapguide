/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/operators/And.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/PathanFactory.hpp>

/*static*/ const XMLCh And::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

And::And(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
}

Result And::createResult(DynamicContext* context, int flags) const
{
  return new AndResult(this, context);
}

DataItem* And::staticResolution(StaticContext *context)
{
  VectorOfDataItems newArgs(PathanAllocator<DataItem*>(context->getMemoryManager()));

  for(VectorOfDataItems::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = (*i)->staticResolution(context);
    const StaticResolutionContext &valueSrc = (*i)->getStaticResolutionContext();

    if(valueSrc.isUsed()) {
      _src.add(valueSrc);
      newArgs.push_back(*i);
    }
    else {
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      if(!(*i)->collapseTree(dContext, DataItem::UNORDERED | DataItem::RETURN_TWO).getEffectiveBooleanValue(dContext)) {
        // It's constantly false, so this expression is false
        DataItem* newBlock = new (getMemoryManager())
          DataItemSequence(dContext->getPathanFactory()->createBoolean(false, dContext),
                           dContext, getMemoryManager());
        newBlock->addPredicates(_predList);
        return newBlock->staticResolution(context);
      }
    }
  }

  _args = newArgs;
  return resolvePredicates(context);
}

And::AndResult::AndResult(const And *op, DynamicContext *context)
  : SingleResult(context),
    _op(op)
{
}

Item::Ptr And::AndResult::getSingleResult(DynamicContext *context) const
{
  unsigned int numArgs=_op->getNumArgs();
  for(unsigned int i=0;i<numArgs;i++) {
    if(!_op->getArgument(i)->collapseTree(context, DataItem::UNORDERED | DataItem::RETURN_TWO).getEffectiveBooleanValue(context)) {
      return (const Item::Ptr)context->getPathanFactory()->createBoolean(false, context);
    }
  }

	return (const Item::Ptr)context->getPathanFactory()->createBoolean(true, context);
}

std::string And::AndResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<and/>" << std::endl;

  return oss.str();
}
