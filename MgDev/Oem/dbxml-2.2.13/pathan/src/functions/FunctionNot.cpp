/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNot.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionNot::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNot::minArgs = 1;
const unsigned int FunctionNot::maxArgs = 1;

/**
 * fn:not($arg as item()*) as xs:boolean
**/

FunctionNot::FunctionNot(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

Sequence FunctionNot::collapseTreeInternal(DynamicContext* context, int flags) const
{
	bool result = !getParamNumber(1,context,DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context);
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	return Sequence(context->getPathanFactory()->createBoolean(result, context), memMgr);
}
