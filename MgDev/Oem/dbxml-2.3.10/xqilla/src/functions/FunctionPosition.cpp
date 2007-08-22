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
 * $Id: FunctionPosition.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionPosition.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

const XMLCh FunctionPosition::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionPosition::minArgs = 0;
const unsigned int FunctionPosition::maxArgs = 0;

/**
 * fn:position() as xs:integer
**/

FunctionPosition::FunctionPosition(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionPosition::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionPosition::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;
  _src.contextPositionUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionPosition::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  if(context->getContextItem()==NULLRCP)
    XQThrow(FunctionException,X("FunctionPosition::collapseTreeInternal"), X("Undefined context item in fn:position [err:XPDY0002]"));
  return Sequence(context->getItemFactory()->createInteger((long)context->getContextPosition(), context), memMgr);
}
















