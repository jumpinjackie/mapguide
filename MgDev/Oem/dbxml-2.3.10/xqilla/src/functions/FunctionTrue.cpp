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
 * $Id: FunctionTrue.cpp,v 1.10 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionTrue.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

const XMLCh FunctionTrue::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionTrue::minArgs = 0;
const unsigned int FunctionTrue::maxArgs = 0;

/*
 * fn:true() as xs:boolean
 */

FunctionTrue::FunctionTrue(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
}

Sequence FunctionTrue::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	return Sequence(context->getItemFactory()->createBoolean(true, context), memMgr);
}
