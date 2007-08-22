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
 * $Id: FunctionNot.cpp,v 1.13 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

const XMLCh FunctionNot::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNot::minArgs = 1;
const unsigned int FunctionNot::maxArgs = 1;

/**
 * fn:not($arg as item()*) as xs:boolean
**/

FunctionNot::FunctionNot(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionNot::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionNot::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionNot::collapseTreeInternal(DynamicContext* context, int flags) const
{
	bool result = !getParamNumber(1,context)->getEffectiveBooleanValue(context, this);
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	return Sequence(context->getItemFactory()->createBoolean(result, context), memMgr);
}
