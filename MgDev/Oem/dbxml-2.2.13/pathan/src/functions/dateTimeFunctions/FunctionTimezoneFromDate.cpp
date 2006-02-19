/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/functions/FunctionTimezoneFromDate.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/ATDateOrDerived.hpp>

const XMLCh FunctionTimezoneFromDate::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionTimezoneFromDate::minArgs = 1;
const unsigned int FunctionTimezoneFromDate::maxArgs = 1;

/*
 * fn:timezone-from-date($arg as xs:date?) as xdt:dayTimeDuration?
 */

FunctionTimezoneFromDate::FunctionTimezoneFromDate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "date?", args, memMgr)
{
}

Sequence FunctionTimezoneFromDate::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence arg=getParamNumber(1,context);
  if(arg.isEmpty())
    return Sequence(memMgr);

  const ATDateOrDerived *date = (const ATDateOrDerived*)(const Item*)arg.first();
  // If $srcval does not contain a timezone, the result is the empty sequence
  if (date->hasTimezone() == false) {
    return Sequence(memMgr);
  }

  const Timezone::Ptr timezone = date->getTimezone();
  return Sequence( (const Item::Ptr )timezone->asDayTimeDuration(context), memMgr );
}

