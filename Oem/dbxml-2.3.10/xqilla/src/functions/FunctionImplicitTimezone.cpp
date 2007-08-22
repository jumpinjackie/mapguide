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
 * $Id: FunctionImplicitTimezone.cpp,v 1.11 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionImplicitTimezone.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

/*implicit-timezone*/
const XMLCh FunctionImplicitTimezone::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionImplicitTimezone::minArgs = 0;
const unsigned int FunctionImplicitTimezone::maxArgs = 0;


/**
 * fn:implicit-timezone() as xdt:dayTimeDuration
**/

FunctionImplicitTimezone::FunctionImplicitTimezone(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionImplicitTimezone::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionImplicitTimezone::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DAY_TIME_DURATION_TYPE;
  _src.implicitTimezoneUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionImplicitTimezone::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(context->getImplicitTimezone(), context->getMemoryManager());
}
