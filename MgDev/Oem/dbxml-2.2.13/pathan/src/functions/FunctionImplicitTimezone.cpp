/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionImplicitTimezone.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

/*implicit-timezone*/
const XMLCh FunctionImplicitTimezone::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionImplicitTimezone::minArgs = 0;
const unsigned int FunctionImplicitTimezone::maxArgs = 0;


/**
 * fn:implicit-timezone() as xdt:dayTimeDuration
**/

FunctionImplicitTimezone::FunctionImplicitTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionImplicitTimezone::staticResolution(StaticContext *context) {
  _src.implicitTimezoneUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionImplicitTimezone::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(context->getImplicitTimezone(), context->getMemoryManager());
}
