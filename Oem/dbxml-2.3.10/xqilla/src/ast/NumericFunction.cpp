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
 * $Id: NumericFunction.cpp,v 1.14 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/NumericFunction.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NumericFunction::NumericFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name,argsFrom, argsTo, paramDecl, args, memMgr) 
{ 
}

ASTNode* NumericFunction::staticResolution(StaticContext *context) {
  return resolveArguments(context, /*checkTimezone*/false, /*numericfunction*/true);
}

ASTNode *NumericFunction::staticTyping(StaticContext *context)
{
  return calculateSRCForArguments(context, /*checkTimezone*/false, /*numericfunction*/true);
}

Numeric::Ptr NumericFunction::getNumericParam(unsigned int number, DynamicContext *context, int flags) const
{
  Result arg = XQFunction::getParamNumber(number, context, flags);
  Item::Ptr item = arg->next(context);

  if(item.isNull()) {
    return 0;
  }
  else if(item->isAtomicValue() && ((const AnyAtomicType *)item.get())->isNumericValue()) {
    return (const Numeric *)item.get();
  } else {
    XQThrow(FunctionException,X("NumericFunction::getParamNumber"), X("Non-numeric argument in numeric function [err:XPTY0004]"));
  }
}

