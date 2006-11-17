/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionAvg.hpp>
#include <pathan/functions/FunctionSum.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/operators/Divide.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionAvg::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAvg::minArgs = 1;
const unsigned int FunctionAvg::maxArgs = 1;

/**
 * fn:avg($arg as xdt:anyAtomicType*) as xdt:anyAtomicType?
**/

FunctionAvg::FunctionAvg(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : AggregateFunction(name, minArgs, maxArgs, "anyAtomicType*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
}

Sequence FunctionAvg::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence sequence(context->getMemoryManager());
  try {
    sequence = validateSequence(getParamNumber(1,context), context);
  } catch (IllegalArgumentException &e) {
    DSLthrow(IllegalArgumentException, X("FunctionAvg::collapseTreeInternal()"), X("Invalid argument to fn:avg() function"));
  }

  if(sequence.isEmpty())
    return Sequence(context->getMemoryManager());

  // check for types that don't support addition and division by an integer
  const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )sequence.first();
  if(!atom->isNumericValue() && 
     atom->getPrimitiveTypeIndex() != AnyAtomicType::DAY_TIME_DURATION &&
     atom->getPrimitiveTypeIndex() != AnyAtomicType::YEAR_MONTH_DURATION)
    DSLthrow(IllegalArgumentException, X("FunctionAvg::collapseTreeInternal()"), X("Invalid argument to fn:avg() function"));

  if(sequence.getLength() == 1)
    return sequence;

  AnyAtomicType::Ptr sum = (AnyAtomicType::Ptr)FunctionSum::sum(sequence, context);
  Numeric::Ptr count = context->getPathanFactory()->createDecimal((long)sequence.getLength(), context);

  if(sum->isNumericValue()) {
    return Sequence(((Numeric*)sum.get())->divide(count, context), context->getMemoryManager());
  }
  else {
    // It must be a duration type
    return Sequence(((ATDurationOrDerived*)sum.get())->divide(count, context), context->getMemoryManager());
  }
}
