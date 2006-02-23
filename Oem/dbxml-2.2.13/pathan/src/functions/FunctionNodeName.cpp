/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNodeName.hpp>
#include <pathan/Node.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/DynamicContext.hpp>

const XMLCh FunctionNodeName::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNodeName::minArgs = 1;
const unsigned int FunctionNodeName::maxArgs = 1;

/*
 * fn:node-name($arg as node()?) as xs:QName?
 */

FunctionNodeName::FunctionNodeName(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

Sequence FunctionNodeName::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence items=getParamNumber(1,context);
  if(items.isEmpty())
    return Sequence(context->getMemoryManager());
  const Node::Ptr node = (const Node::Ptr )items.first();
  return Sequence(node->dmNodeName(context), context->getMemoryManager());
}
















