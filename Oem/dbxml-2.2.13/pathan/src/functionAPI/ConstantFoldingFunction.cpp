/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functionAPI/ConstantFoldingFunction.hpp>
#include <pathan/StaticContext.hpp>

ConstantFoldingFunction::ConstantFoldingFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, argsFrom, argsTo, paramDecl, args, memMgr)
{
}


ConstantFoldingFunction::~ConstantFoldingFunction()
{
}

DataItem* ConstantFoldingFunction::staticResolution(StaticContext *context) {
  return resolveDataItems(_args, context, true);
}
