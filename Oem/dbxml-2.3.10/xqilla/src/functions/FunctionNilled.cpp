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
 * $Id: FunctionNilled.cpp,v 1.11 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNilled.hpp>

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

const XMLCh FunctionNilled::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNilled::minArgs = 1;
const unsigned int FunctionNilled::maxArgs = 1;

/*
 * fn:nilled($arg as node()?) as boolean?
 */

FunctionNilled::FunctionNilled(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

ASTNode* FunctionNilled::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionNilled::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionNilled::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1, context)->toSequence(context);
  // If the argument is the empty sequence, returns the empty sequence
  if(arg.isEmpty())
    return Sequence(context->getMemoryManager());

  Node::Ptr node=(Node::Ptr)arg.first();
  return Sequence(node->dmNilled(context), context->getMemoryManager());
}
