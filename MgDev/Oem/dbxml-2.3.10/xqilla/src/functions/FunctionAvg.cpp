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
 * $Id: FunctionAvg.cpp,v 1.15 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionAvg.hpp>
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/ItemFactory.hpp>

const XMLCh FunctionAvg::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAvg::minArgs = 1;
const unsigned int FunctionAvg::maxArgs = 1;

/**
 * fn:avg($arg as xdt:anyAtomicType*) as xdt:anyAtomicType?
**/

FunctionAvg::FunctionAvg(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : AggregateFunction(name, minArgs, maxArgs, "anyAtomicType*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::TYPED_ATOMIC_TYPE;
}

Sequence FunctionAvg::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence sequence(context->getMemoryManager());
  try {
    sequence = validateSequence(getParamNumber(1,context)->toSequence(context), context);
  } catch (IllegalArgumentException &e) {
    XQThrow(IllegalArgumentException, X("FunctionAvg::collapseTreeInternal()"), X("Invalid argument to fn:avg() function [err:FORG0006]."));
  }

  if(sequence.isEmpty())
    return Sequence(context->getMemoryManager());

  // check for types that don't support addition and division by an integer
  const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )sequence.first();
  if(!atom->isNumericValue() && 
     atom->getPrimitiveTypeIndex() != AnyAtomicType::DAY_TIME_DURATION &&
     atom->getPrimitiveTypeIndex() != AnyAtomicType::YEAR_MONTH_DURATION)
    XQThrow(IllegalArgumentException, X("FunctionAvg::collapseTreeInternal()"), X("Invalid argument to fn:avg() function [err:FORG0006]."));

  if(sequence.getLength() == 1)
    return sequence;

  AnyAtomicType::Ptr sum;
  try {
    sum = (AnyAtomicType::Ptr)FunctionSum::sum(sequence, context, this);
  }
  catch(IllegalArgumentException &) {
    XQThrow(IllegalArgumentException, X("FunctionAvg::collapseTreeInternal()"), X("Invalid argument to fn:avg() function [err:FORG0006]."));
  }

  Numeric::Ptr count = context->getItemFactory()->createDecimal((long)sequence.getLength(), context);

  if(sum->isNumericValue()) {
    return Sequence(((Numeric*)sum.get())->divide(count, context), context->getMemoryManager());
  }
  else {
    // It must be a duration type
    return Sequence(((ATDurationOrDerived*)sum.get())->divide(count, context), context->getMemoryManager());
  }
}
