/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCeiling.hpp>
#include <pathan/Numeric.hpp>
#include <pathan/DynamicContext.hpp>

const XMLCh FunctionCeiling::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCeiling::minArgs = 1;
const unsigned int FunctionCeiling::maxArgs = 1;

/*
 * fn:ceiling($arg as numeric?) as numeric?
 */

FunctionCeiling::FunctionCeiling(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : NumericFunction(name, minArgs, maxArgs, "anySimpleType?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

Sequence FunctionCeiling::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1, context);
    
  //If the argument is the empty sequence, the empty sequence is returned.

  if(arg.isEmpty()) {
    return Sequence(context->getMemoryManager());
  }

  const Numeric::Ptr numericArg = (const Numeric::Ptr )arg.first();
  if(isNaN(numericArg))
    return Sequence(numericArg, context->getMemoryManager());
  if(isINF(numericArg))
    return Sequence(numericArg, context->getMemoryManager());
  return Sequence(numericArg->ceiling(context), context->getMemoryManager());
}
