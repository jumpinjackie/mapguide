/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/functions/FunctionAdjustDateToTimezone.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include "../../utils/DateUtils.hpp"
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionAdjustDateToTimezone::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_j, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAdjustDateToTimezone::minArgs = 1;
const unsigned int FunctionAdjustDateToTimezone::maxArgs = 2;

/*
 * fn:adjust-date-to-timezone($arg as xs:date?) as xs:date?
 * fn:adjust-date-to-timezone($arg as xs:date?, $timezone as xdt:dayTimeDuration?) as xs:date?
 */

FunctionAdjustDateToTimezone::FunctionAdjustDateToTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "date?, dayTimeDuration?", args, memMgr)
{
}

DataItem* FunctionAdjustDateToTimezone::staticResolution(StaticContext *context) {
  if(getNumArgs() == 1) {
    _src.implicitTimezoneUsed(true);
  }
  return resolveDataItems(_args, context, getNumArgs() > 1);
}

Sequence FunctionAdjustDateToTimezone::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  //If $srcval is the empty sequence, then the result is the empty sequence.
  Sequence op1 = getParamNumber(1, context);
  if (op1.isEmpty()) {
    return Sequence(memMgr);
  }

  const ATDateOrDerived* date = (const ATDateOrDerived*)(const Item*)op1.first();

  //If $timezone is not specified, then $timezone is the value of the implicit timezone in the evaluation context.
  ATDurationOrDerived::Ptr timezoneAsDuration = 0;

  if (getNumArgs() > 1) {
    Sequence op2 = getParamNumber(2, context);
    if (op2.isEmpty()) {
      // unset the timezone
      return Sequence(date->setTimezone(0, context), memMgr);
    } else {
      timezoneAsDuration = (const ATDurationOrDerived::Ptr )op2.first();
      Timezone::Ptr timezone = new Timezone(timezoneAsDuration, context);
      if(!timezoneAsDuration->equals(timezone->asDayTimeDuration(context), context)) {
        DSLthrow(FunctionException, X("FunctionAdjustDateToTimeZone::collapseTreeInternal"),X("invalid timezone value"));    
      }
    }
  }
  else {
    timezoneAsDuration = context->getImplicitTimezone();
  }

  return Sequence(date->addTimezone(timezoneAsDuration, context), memMgr);
}
