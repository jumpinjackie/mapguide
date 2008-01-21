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
 * $Id: FunctionLocalname.cpp,v 1.15 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/functions/FunctionLocalname.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

const XMLCh FunctionLocalname::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionLocalname::minArgs = 0;
const unsigned int FunctionLocalname::maxArgs = 1;

/**
 * fn:local-name() as xs:string
 * fn:local-name($arg as node()?) as xs:string
**/

FunctionLocalname::FunctionLocalname(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

ASTNode* FunctionLocalname::staticResolution(StaticContext *context) {
  if(!_args.empty() && (*_args.begin())->getType()==ASTNode::CONTEXT_ITEM)
      _args.clear();
  return resolveArguments(context);
}

ASTNode *FunctionLocalname::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::STRING_TYPE;
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return calculateSRCForArguments(context);
}

Sequence FunctionLocalname::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Node::Ptr ctxNode;
  if(getNumArgs() == 1) {
    Sequence arg=getParamNumber(1,context)->toSequence(context);
    if(arg.isEmpty())
      return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
    ctxNode=arg.first();
  }
  else {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
      XQThrow(FunctionException, X("FunctionLocalName::collapseTreeInternal"),X("Undefined context item in fn:local-name [err:XPDY0002]"));
    if(!item->isNode())
      XQThrow(FunctionException, X("FunctionLocalName::collapseTreeInternal"),X("The context item is not a node [err:XPTY0004]"));
    ctxNode=item;
	}

  ATQNameOrDerived::Ptr name = ctxNode->dmNodeName(context);
  if(name.notNull())
    return Sequence(context->getItemFactory()->createString(((const ATQNameOrDerived*)name.get())->getName(), context), memMgr);

  return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
}





