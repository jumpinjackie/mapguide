/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionBaseURI.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/FunctionException.hpp>

const XMLCh FunctionBaseURI::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionBaseURI::minArgs = 0;
const unsigned int FunctionBaseURI::maxArgs = 1;

/**
 * fn:base-uri() as xs:anyURI?
 * fn:base-uri($arg as node()?) as xs:anyURI?
**/

FunctionBaseURI::FunctionBaseURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

DataItem* FunctionBaseURI::staticResolution(StaticContext *context) {
  if(_args.empty())
    _src.contextItemUsed(true);
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionBaseURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Node::Ptr node = NULL;
  if(getNumArgs() == 1)
  {
    Sequence arg1=getParamNumber(1,context);
    if(arg1.isEmpty())
      return Sequence(context->getMemoryManager());
    node = (const Node::Ptr )arg1.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
        DSLthrow(FunctionException, X("FunctionBaseURI::collapseTreeInternal"),X("Undefined context item in fn:base-uri [err:FONC0001]"));
    if(!item->isNode())
        DSLthrow(FunctionException, X("FunctionBaseURI::collapseTreeInternal"),X("The context item is not a node [err:XPTY0006]"));
    node = (const Node::Ptr )item;
  }
  return node->dmBaseURI(context);
}
