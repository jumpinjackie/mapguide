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
 * $Id: FunctionCurrentDateTime.cpp,v 1.11 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCurrentDateTime.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include "../utils/DateUtils.hpp"
#include <xqilla/ast/StaticResolutionContext.hpp>

const XMLCh FunctionCurrentDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCurrentDateTime::minArgs = 0;
const unsigned int FunctionCurrentDateTime::maxArgs = 0;

/**
 * fn:current-dateTime() as xs:dateTime
**/

FunctionCurrentDateTime::FunctionCurrentDateTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionCurrentDateTime::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionCurrentDateTime::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DATE_TIME_TYPE;
  _src.currentTimeUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionCurrentDateTime::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(DateUtils::getCurrentDateTime(context), context->getMemoryManager());
}
















