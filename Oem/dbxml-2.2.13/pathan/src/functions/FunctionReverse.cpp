/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionReverse.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>

const XMLCh FunctionReverse::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const unsigned int FunctionReverse::minArgs = 1;
const unsigned int FunctionReverse::maxArgs = 1;

/**
 * fn:reverse($arg as item()*) as item()*
 */

FunctionReverse::FunctionReverse(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::NODE_TYPE | StaticType::OTHER_TYPE;
}

Sequence FunctionReverse::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence origSeq = getParamNumber(1, context).toSequence(context);
  // empty or singleton sequences are identical when reversed
  unsigned int len=origSeq.getLength();
  if(len<2)
    return origSeq;

  Sequence result(len,memMgr);
  for(Sequence::reverse_iterator i=origSeq.rbegin();i!=origSeq.rend();i++)
    result.addItem(*i);
  
  return result;
}
