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
 * $Id: FunctionLast.cpp,v 1.15 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

const XMLCh FunctionLast::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionLast::minArgs = 0;
const unsigned int FunctionLast::maxArgs = 0;

/**
 * fn:last() as xs:integer
**/

FunctionLast::FunctionLast(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionLast::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionLast::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;
  _src.contextSizeUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionLast::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  if(context->getContextItem()==NULLRCP)
    XQThrow(FunctionException,X("FunctionPosition::collapseTreeInternal"), X("Undefined context item in fn:last [err:XPDY0002]"));
  return Sequence(context->getItemFactory()->createInteger((long)context->getContextSize(), context), memMgr);
}
















