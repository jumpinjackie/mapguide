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
 * $Id: FunctionTimezoneFromDate.cpp,v 1.10 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionTimezoneFromDate.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>

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

FunctionTimezoneFromDate::FunctionTimezoneFromDate(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "date?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::DAY_TIME_DURATION_TYPE;
}

Sequence FunctionTimezoneFromDate::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence arg=getParamNumber(1,context)->toSequence(context);
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

