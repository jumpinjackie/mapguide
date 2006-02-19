/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionBoolean.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/Item.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionBoolean::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionBoolean::minArgs = 1;
const unsigned int FunctionBoolean::maxArgs = 1;

/**
 * fn:boolean($arg as item()*) as xs:boolean
**/

FunctionBoolean::FunctionBoolean(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

Sequence FunctionBoolean::collapseTreeInternal(DynamicContext* context, int flags) const
{
  bool result = getParamNumber(1,context,DataItem::UNORDERED|DataItem::RETURN_TWO)
    .getEffectiveBooleanValue(context);
  return Sequence(context->getPathanFactory()->createBoolean(result, context),
                  context->getMemoryManager());
}
