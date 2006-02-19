/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionAbs.hpp>

#include <pathan/Numeric.hpp>
#include <pathan/DynamicContext.hpp>

const XMLCh FunctionAbs::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAbs::minArgs = 1;
const unsigned int FunctionAbs::maxArgs = 1;

/*
 * fn:abs($arg as numeric?) as numeric?
 */

FunctionAbs::FunctionAbs(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : NumericFunction(name, minArgs, maxArgs, "anySimpleType?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

Sequence FunctionAbs::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1, context);

  //If the argument is the empty sequence, the empty sequence is returned.
  
  if(arg.isEmpty()) {
    return Sequence(context->getMemoryManager());
  }

  const Numeric::Ptr numericArg = (const Numeric::Ptr )arg.first();
  if(isNaN(numericArg))
    return Sequence(numericArg, context->getMemoryManager());
    
  if(numericArg->isNegative())
    return Sequence(numericArg->invert(context), context->getMemoryManager());
  return Sequence(numericArg, context->getMemoryManager());
}
