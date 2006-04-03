/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/functions/FunctionAdjustTimeToTimezone.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include "../../utils/DateUtils.hpp"
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionAdjustTimeToTimezone::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_j, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAdjustTimeToTimezone::minArgs = 1;
const unsigned int FunctionAdjustTimeToTimezone::maxArgs = 2;

/*
 * fn:adjust-time-to-timezone($arg as xs:time?) as xs:time?
 * fn:adjust-time-to-timezone($arg as xs:time?, $timezone as xdt:dayTimeDuration?) as xs:time?
 */

FunctionAdjustTimeToTimezone::FunctionAdjustTimeToTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "time?, dayTimeDuration?", args, memMgr)
{
}

DataItem* FunctionAdjustTimeToTimezone::staticResolution(StaticContext *context) {
  if(getNumArgs() == 1) {
    _src.implicitTimezoneUsed(true);
  }
  return resolveDataItems(_args, context, getNumArgs() > 1);
}

Sequence FunctionAdjustTimeToTimezone::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  //If $srcval is the empty sequence, then the result is the empty sequence.
  Sequence op1 = getParamNumber(1, context);
  if (op1.isEmpty()) {
    return Sequence(memMgr);
  }
  const ATTimeOrDerived* time = (const ATTimeOrDerived*)(const Item*)op1.first();

  //If $timezone is not specified, then $timezone is the value of the implicit timezone in the evaluation context.
  ATDurationOrDerived::Ptr timezoneAsDuration = 0;

  if (getNumArgs() > 1) {
    Sequence op2 = getParamNumber(2, context);
    if (op2.isEmpty()) {
      // unset the timezone
      return Sequence(time->setTimezone(0, context), memMgr);
    } else {
      timezoneAsDuration = (const ATDurationOrDerived::Ptr )op2.first();
      Timezone::Ptr timezone = new Timezone(timezoneAsDuration, context);
      if(!timezoneAsDuration->equals(timezone->asDayTimeDuration(context), context)) {
        DSLthrow(FunctionException, X("FunctionAdjustTimeToTimeZone::collapseTreeInternal"),X("invalid timezone value"));    
      }
    }
  }
  else {
    timezoneAsDuration = context->getImplicitTimezone();
  }

  return Sequence(time->addTimezone(timezoneAsDuration, context), memMgr);

}
