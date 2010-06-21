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
 * $Id: ConstantFoldingFunction.cpp,v 1.10 2006/11/01 16:37:17 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/context/StaticContext.hpp>

ConstantFoldingFunction::ConstantFoldingFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, argsFrom, argsTo, paramDecl, args, memMgr)
{
}


ConstantFoldingFunction::~ConstantFoldingFunction()
{
}

ASTNode* ConstantFoldingFunction::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode* ConstantFoldingFunction::staticTyping(StaticContext *context) {
  return calculateSRCForArguments(context);
}
