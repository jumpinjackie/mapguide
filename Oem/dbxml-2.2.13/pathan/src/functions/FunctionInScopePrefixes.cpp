/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionInScopePrefixes.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/internal/DOMutils/XMLChCompare.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XMLString.hpp>

/**
 * fn:in-scope-prefixes($element as element) as xs:string*
**/

const XMLCh FunctionInScopePrefixes::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionInScopePrefixes::minArgs = 1;
const unsigned int FunctionInScopePrefixes::maxArgs = 1;


FunctionInScopePrefixes::FunctionInScopePrefixes(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "element()", args, memMgr)
{
}

Sequence FunctionInScopePrefixes::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg1 = getParamNumber(1,context).toSequence(context);
  Node::Ptr node = (Node::Ptr)arg1.first();

  Sequence returnSeq(context->getMemoryManager());

  Result namespaces = node->dmNamespaceNodes(context);
  Node::Ptr ns;
  while((ns = (Node::Ptr)namespaces.next(context)).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      returnSeq.addItem(context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context));
    }
    else {
      returnSeq.addItem(context->getPathanFactory()->createString(((const ATQNameOrDerived*)name.get())->getName(), context));
    }
  }

  return returnSeq;
}
