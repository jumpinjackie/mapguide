/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/functions/FunctionMonthFromDateTime.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>

const XMLCh FunctionMonthFromDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionMonthFromDateTime::minArgs = 1;
const unsigned int FunctionMonthFromDateTime::maxArgs = 1;

/*
 * fn:month-from-dateTime($arg as xs:dateTime?) as xs:integer?
 */

FunctionMonthFromDateTime::FunctionMonthFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "dateTime?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

DataItem* FunctionMonthFromDateTime::staticResolution(StaticContext *context)
{
  return resolveDataItemsForDateOrTime(_args, context, true);
}

Sequence FunctionMonthFromDateTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence arg=getParamNumber(1,context);
  if(arg.isEmpty())
    return Sequence(memMgr);

  const ATDateTimeOrDerived::Ptr dateTime = (const ATDateTimeOrDerived::Ptr )arg.first();
  return Sequence(dateTime->getMonths(), memMgr);

}
















