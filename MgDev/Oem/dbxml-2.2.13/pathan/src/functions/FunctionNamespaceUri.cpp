/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/functions/FunctionNamespaceUri.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/dom/DOM.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>

const XMLCh FunctionNamespaceUri::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNamespaceUri::minArgs = 0;
const unsigned int FunctionNamespaceUri::maxArgs = 1;

/**
 * fn:namespace-uri() as xs:anyURI
 * fn:namespace-uri($arg as node()?) as xs:anyURI
**/

FunctionNamespaceUri::FunctionNamespaceUri(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

DataItem* FunctionNamespaceUri::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionNamespaceUri::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Node::Ptr ctxNode;
  if(getNumArgs() == 1)
  {
    Sequence arg=getParamNumber(1,context);
    if(arg.isEmpty())
      return Sequence(context->getPathanFactory()->createAnyURI(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
    ctxNode=arg.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
      DSLthrow(FunctionException, X("FunctionNamespaceUri::collapseTreeInternal"),X("Undefined context item in fn:namespace-uri [err:FONC0001]"));
    if(!item->isNode())
      DSLthrow(FunctionException, X("FunctionNamespaceUri::collapseTreeInternal"),X("The context item is not a node [err:XPTY0006]"));
    ctxNode=item;
  }

  ATQNameOrDerived::Ptr name = ctxNode->dmNodeName(context);
  if(name.notNull())
    return Sequence(context->getPathanFactory()->createAnyURI(((const ATQNameOrDerived*)name.get())->getURI(), context), memMgr);
  
  return Sequence(context->getPathanFactory()->createAnyURI(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
}
