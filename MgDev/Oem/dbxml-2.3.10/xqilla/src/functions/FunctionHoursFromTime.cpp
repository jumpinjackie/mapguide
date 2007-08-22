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
 * $Id: FunctionHoursFromTime.cpp,v 1.13 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionHoursFromTime.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>

const XMLCh FunctionHoursFromTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionHoursFromTime::minArgs = 1;
const unsigned int FunctionHoursFromTime::maxArgs = 1;

/*
 * fn:hours-from-time($arg as xs:time?) as xs:integer?
 */

FunctionHoursFromTime::FunctionHoursFromTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "time?", args, memMgr)
{
}

ASTNode* FunctionHoursFromTime::staticResolution(StaticContext *context)
{
  return resolveArguments(context, /*checkTimezone*/true);
}

ASTNode *FunctionHoursFromTime::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;
  return calculateSRCForArguments(context, /*checkTimezone*/true);
}

Sequence FunctionHoursFromTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr arg = getParamNumber(1, context)->next(context);
  if(arg.isNull()) return Sequence(memMgr);

  return Sequence(((const ATTimeOrDerived*)arg.get())->getHours(context), memMgr);
}
















