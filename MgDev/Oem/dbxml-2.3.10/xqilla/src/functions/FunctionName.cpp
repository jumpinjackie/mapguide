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
 * $Id: FunctionName.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/functions/FunctionString.hpp>

const XMLCh FunctionName::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionName::minArgs = 0;
const unsigned int FunctionName::maxArgs = 1;

/**
 * fn:name() as xs:string
 * fn:name($arg as node()?) as xs:string
**/

FunctionName::FunctionName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

ASTNode* FunctionName::staticResolution(StaticContext *context) {
  if(!_args.empty() && (*_args.begin())->getType()==ASTNode::CONTEXT_ITEM)
      _args.clear();
  return resolveArguments(context);
}

ASTNode *FunctionName::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::STRING_TYPE;
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return calculateSRCForArguments(context);
}

Sequence FunctionName::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Node::Ptr ctxNode;
  if(getNumArgs() == 1)
  {
    Sequence arg=getParamNumber(1,context)->toSequence(context);
    if(arg.isEmpty())
      return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
    ctxNode=arg.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
      XQThrow(FunctionException, X("FunctionName::collapseTreeInternal"),X("Undefined context item in fn:name [err:XPDY0002]"));
    if(!item->isNode())
      XQThrow(FunctionException, X("FunctionName::collapseTreeInternal"),X("The context item is not a node [err:XPTY0004]"));
    ctxNode=item;
  }

  return Sequence(FunctionString::string(ctxNode->dmNodeName(context), context), memMgr);
}







