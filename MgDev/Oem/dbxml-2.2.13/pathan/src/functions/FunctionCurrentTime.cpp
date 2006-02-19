/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCurrentTime.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/PathanFactory.hpp>
#include "../utils/DateUtils.hpp"
#include <stdio.h> // for sprintf

const XMLCh FunctionCurrentTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCurrentTime::minArgs = 0;
const unsigned int FunctionCurrentTime::maxArgs = 0;

/**
 * fn:current-time() as xs:time
**/

FunctionCurrentTime::FunctionCurrentTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionCurrentTime::staticResolution(StaticContext *context) {
  _src.currentTimeUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionCurrentTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
  time_t curDate=context->getCurrentTime();
  struct tm time_struct;
  struct tm* curLocalDate=DateUtils::threadsafe_localtime(&curDate, &time_struct);
  char szDate[128];
  sprintf(szDate,"%02d:%02d:%02dZ",curLocalDate->tm_hour, curLocalDate->tm_min, curLocalDate->tm_sec);

  const ATTimeOrDerived::Ptr time = context->getPathanFactory()->createTime(context->getMemoryManager()->getPooledString(szDate), context);
	// no need to add timezone, it's already compensated for in localtime
	// time.setTimezone(Timezone(XSDecimal(DateUtils::getImplicitTimezone(), memMgr)));
  return Sequence(time, context->getMemoryManager());
}
















