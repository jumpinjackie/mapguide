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
 * $Id: FunctionReverse.cpp,v 1.11 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionReverse.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>

const XMLCh FunctionReverse::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const unsigned int FunctionReverse::minArgs = 1;
const unsigned int FunctionReverse::maxArgs = 1;

/**
 * fn:reverse($arg as item()*) as item()*
 */

FunctionReverse::FunctionReverse(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionReverse::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionReverse::staticTyping(StaticContext *context)
{
  _src.clear();

  ASTNode *result = calculateSRCForArguments(context);
  if(result == this) {
    _src.getStaticType() = _args[0]->getStaticResolutionContext().getStaticType();
  }
  return result;
}

Sequence FunctionReverse::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence origSeq = getParamNumber(1, context)->toSequence(context);
  // empty or singleton sequences are identical when reversed
  unsigned int len=origSeq.getLength();
  if(len<2)
    return origSeq;

  Sequence result(len,memMgr);
  for(Sequence::reverse_iterator i=origSeq.rbegin();i!=origSeq.rend();i++)
    result.addItem(*i);
  
  return result;
}
