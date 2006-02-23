/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNilled.hpp>

#include <pathan/DynamicContext.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/XPath2Utils.hpp>
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

FunctionNilled::FunctionNilled(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

DataItem* FunctionNilled::staticResolution(StaticContext *context) {
  return resolveDataItems(_args, context, false);
}

Sequence FunctionNilled::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1, context);
  // If the argument is the empty sequence, returns the empty sequence
  if(arg.isEmpty())
    return Sequence(context->getMemoryManager());

  Node::Ptr node=(Node::Ptr)arg.first();
  return Sequence(node->dmNilled(context), context->getMemoryManager());
}
