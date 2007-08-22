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
 * $Id: FunctionSum.cpp,v 1.16 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/ContextHelpers.hpp>

const XMLCh FunctionSum::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionSum::minArgs = 1;
const unsigned int FunctionSum::maxArgs = 2;

/**
 * fn:sum($arg as xdt:anyAtomicType*) as xdt:anyAtomicType
 * fn:sum($arg as xdt:anyAtomicType*, $zero as xdt:anyAtomicType?) as xdt:anyAtomicType?
**/

FunctionSum::FunctionSum(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : AggregateFunction(name, minArgs, maxArgs, "anyAtomicType*,anyAtomicType?", args, memMgr)
{
}

ASTNode* FunctionSum::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionSum::staticTyping(StaticContext *context)
{
  _src.clear();

  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::TYPED_ATOMIC_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionSum::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence sequence(memMgr);
  try {
    sequence = validateSequence(getParamNumber(1,context)->toSequence(context), context);
  } catch (IllegalArgumentException &e) {
    XQThrow(IllegalArgumentException, X("FunctionSum::collapseTreeInternal"), X("Invalid argument to fn:sum() function [err:FORG0006]"));
  }

  if(sequence.isEmpty())
    if(getNumArgs() == 1)
      return Sequence(context->getItemFactory()->createInteger(0, context), memMgr);
    else
      return getParamNumber(2,context)->toSequence(context);

  // check for types that don't support addition
  const AnyAtomicType::Ptr atom = (const AnyAtomicType::Ptr )sequence.first();
  if(!atom->isNumericValue() && 
     atom->getPrimitiveTypeIndex() != AnyAtomicType::DAY_TIME_DURATION &&
     atom->getPrimitiveTypeIndex() != AnyAtomicType::YEAR_MONTH_DURATION)
    XQThrow(IllegalArgumentException, X("FunctionSum::collapseTreeInternal"), X("Invalid argument to fn:sum() function [err:FORG0006]."));

  return Sequence(sum(sequence, context, this), memMgr);
}

Item::Ptr FunctionSum::sum(const Sequence &sequence, DynamicContext *context, const LocationInfo *info)
{
  if(sequence.getLength() == 1)
    return sequence.first();

  Sequence::const_iterator i = sequence.begin();
  Item::Ptr sum = *i;
  ++i;

  for(; i != sequence.end(); ++i) {
    try {
      sum = Plus::plus(*i, sum, context, info);
    } catch (IllegalArgumentException &e) {
      XQThrow3(IllegalArgumentException, X("FunctionSum::collapseTreeInternal"), X("Invalid argument to fn:sum() function"), info);
    }  
  } 

  return sum;
}
