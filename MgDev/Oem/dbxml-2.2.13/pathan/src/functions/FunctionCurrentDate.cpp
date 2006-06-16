/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCurrentDate.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include "../utils/DateUtils.hpp"
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionCurrentDate::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCurrentDate::minArgs = 0;
const unsigned int FunctionCurrentDate::maxArgs = 0;

/**
 * fn:current-date() as xs:date
**/

FunctionCurrentDate::FunctionCurrentDate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionCurrentDate::staticResolution(StaticContext *context) {
  _src.currentTimeUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionCurrentDate::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(DateUtils::getCurrentDate(context), context->getMemoryManager());
}
















