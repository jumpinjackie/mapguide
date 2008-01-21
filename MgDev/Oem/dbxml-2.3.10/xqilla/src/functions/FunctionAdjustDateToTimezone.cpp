/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FunctionAdjustDateToTimezone.cpp,v 1.13 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionAdjustDateToTimezone.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include "../utils/DateUtils.hpp"
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
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

FunctionAdjustDateToTimezone::FunctionAdjustDateToTimezone(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "date?, dayTimeDuration?", args, memMgr)
{
}

ASTNode* FunctionAdjustDateToTimezone::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionAdjustDateToTimezone::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DATE_TYPE;
  if(getNumArgs() == 1) {
    _src.implicitTimezoneUsed(true);
  }
  return calculateSRCForArguments(context);
}

Sequence FunctionAdjustDateToTimezone::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  //If $srcval is the empty sequence, then the result is the empty sequence.
  Sequence op1 = getParamNumber(1, context)->toSequence(context);
  if (op1.isEmpty()) {
    return Sequence(memMgr);
  }

  const ATDateOrDerived* date = (const ATDateOrDerived*)(const Item*)op1.first();

  //If $timezone is not specified, then $timezone is the value of the implicit timezone in the evaluation context.
  ATDurationOrDerived::Ptr timezoneAsDuration = 0;

  if (getNumArgs() > 1) {
    Sequence op2 = getParamNumber(2, context)->toSequence(context);
    if (op2.isEmpty()) {
      // unset the timezone
      return Sequence(date->setTimezone(0, context), memMgr);
    } else {
      timezoneAsDuration = (const ATDurationOrDerived::Ptr )op2.first();
      Timezone::Ptr timezone = new Timezone(timezoneAsDuration, context);
      if(!timezoneAsDuration->equals(timezone->asDayTimeDuration(context), context)) {
        XQThrow(FunctionException, X("FunctionAdjustDateToTimeZone::collapseTreeInternal"),X("Invalid timezone value [err:FODT0003]"));
      }
    }
  }
  else {
    timezoneAsDuration = context->getImplicitTimezone();
  }

  return Sequence(date->addTimezone(timezoneAsDuration, context), memMgr);
}
