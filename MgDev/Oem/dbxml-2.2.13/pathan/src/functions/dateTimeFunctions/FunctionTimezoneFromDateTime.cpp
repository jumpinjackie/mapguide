/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/functions/FunctionTimezoneFromDateTime.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>

const XMLCh FunctionTimezoneFromDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionTimezoneFromDateTime::minArgs = 1;
const unsigned int FunctionTimezoneFromDateTime::maxArgs = 1;

/*
 * fn:timezone-from-dateTime($arg as xs:dateTime?) as xdt:dayTimeDuration?
 */

FunctionTimezoneFromDateTime::FunctionTimezoneFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "dateTime?", args, memMgr)
{
}

Sequence FunctionTimezoneFromDateTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence arg=getParamNumber(1,context);
  if(arg.isEmpty()) {
    return Sequence(memMgr);
  }

  const ATDateTimeOrDerived * dateTime = (const ATDateTimeOrDerived *)(const Item*)arg.first();
  // If $srcval does not contain a timezone, the result is the empty sequence
  if (dateTime->hasTimezone() == false) {
    return Sequence(memMgr);
  }

  const Timezone::Ptr timezone = dateTime->getTimezone();
  return Sequence( (const Item::Ptr )timezone->asDayTimeDuration(context), memMgr );

}
















