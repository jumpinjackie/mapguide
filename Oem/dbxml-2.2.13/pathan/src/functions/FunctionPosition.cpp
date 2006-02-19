/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionPosition.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionPosition::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionPosition::minArgs = 0;
const unsigned int FunctionPosition::maxArgs = 0;

/**
 * fn:position() as xs:integer
**/

FunctionPosition::FunctionPosition(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionPosition::staticResolution(StaticContext *context) {
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
  _src.contextPositionUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionPosition::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  if(context->getContextItem()==NULLRCP)
    DSLthrow(FunctionException,X("FunctionPosition::collapseTreeInternal"), X("Undefined context item in fn:position [err:FONC0001]"));
  return Sequence(context->getPathanFactory()->createInteger((long)context->getContextPosition(), context), memMgr);
}
















