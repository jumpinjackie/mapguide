/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCount.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionCount::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCount::minArgs = 1;
const unsigned int FunctionCount::maxArgs = 1;

/**
 * fn:count($arg as item()*) as xs:integer
**/

FunctionCount::FunctionCount(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

Sequence FunctionCount::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1,context,DataItem::UNORDERED);
	return Sequence(context->getPathanFactory()->createInteger((long)arg.getLength(), context), context->getMemoryManager());
}

