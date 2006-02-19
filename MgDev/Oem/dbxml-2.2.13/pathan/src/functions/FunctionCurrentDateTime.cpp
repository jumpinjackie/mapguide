/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCurrentDateTime.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include "../utils/DateUtils.hpp"
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionCurrentDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCurrentDateTime::minArgs = 0;
const unsigned int FunctionCurrentDateTime::maxArgs = 0;

/**
 * fn:current-dateTime() as xs:dateTime
**/

FunctionCurrentDateTime::FunctionCurrentDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionCurrentDateTime::staticResolution(StaticContext *context) {
  _src.currentTimeUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionCurrentDateTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(DateUtils::getCurrentDateTime(context), context->getMemoryManager());
}
















