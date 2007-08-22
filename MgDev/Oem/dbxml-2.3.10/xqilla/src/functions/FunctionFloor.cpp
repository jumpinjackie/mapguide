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
 * $Id: FunctionFloor.cpp,v 1.12 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionFloor.hpp>

#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>

const XMLCh FunctionFloor::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionFloor::minArgs = 1;
const unsigned int FunctionFloor::maxArgs = 1;

/*
 * fn:floor($arg as numeric?) as numeric?
 */

FunctionFloor::FunctionFloor(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : NumericFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

Sequence FunctionFloor::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Numeric::Ptr numericArg = getNumericParam(1, context);

  //If the argument is the empty sequence, the empty sequence is returned.
  if(numericArg.isNull()) {
    return Sequence(context->getMemoryManager());
  }

  if(numericArg->isNaN() || numericArg->isInfinite())
    return Sequence(numericArg, context->getMemoryManager());
  return Sequence(numericArg->floor(context), context->getMemoryManager());
}
