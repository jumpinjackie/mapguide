/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionRoot::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionRoot::minArgs = 0;
const unsigned int FunctionRoot::maxArgs = 1;

/**
 * fn:root() as node()
 * fn:root($arg as node()?) as node()?
**/

FunctionRoot::FunctionRoot(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
}

/*static*/ Node::Ptr FunctionRoot::root(const Node::Ptr &node, const DynamicContext *context) 
{
  Node::Ptr result = node;
  Node::Ptr parent = node->dmParent(context);
  while(parent.notNull()) {
    result = parent;
    parent = result->dmParent(context);
  }
  return result;
}

DataItem* FunctionRoot::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionRoot::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Node::Ptr node = NULL;
  if(getNumArgs() == 1)
  {
    Sequence arg1=getParamNumber(1,context);
    if(arg1.isEmpty())
      return Sequence(memMgr);
    node = (const Node::Ptr )arg1.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
        DSLthrow(FunctionException, X("FunctionRoot::collapseTreeInternal"),X("Undefined context item in fn:root [err:FONC0001]"));
    if(!item->isNode())
        DSLthrow(FunctionException, X("FunctionRoot::collapseTreeInternal"),X("The context item is not a node [err:XPTY0006]"));
    node = (const Node::Ptr )item;
  }
  return Sequence(root(node, context), memMgr);
}
