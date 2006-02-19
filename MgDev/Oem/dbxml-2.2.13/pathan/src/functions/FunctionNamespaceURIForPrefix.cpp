/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNamespaceURIForPrefix.hpp>

#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/functions/FunctionNormalizeSpace.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMNode.hpp>


const XMLCh FunctionNamespaceURIForPrefix::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNamespaceURIForPrefix::minArgs = 2;
const unsigned int FunctionNamespaceURIForPrefix::maxArgs = 2;

/**
 * fn:namespace-uri-for-prefix($prefix as xs:string, $element as element()) as xs:anyURI?
**/

/** Returns the namespace URI of one of the in-scope namespaces for $element, identified 
 * by its namespace prefix.
**/

FunctionNamespaceURIForPrefix::FunctionNamespaceURIForPrefix(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string, element()", args, memMgr)
{
}

Sequence FunctionNamespaceURIForPrefix::collapseTreeInternal(DynamicContext* context, int flags) const
{
  const XMLCh* prefix = getParamNumber(1, context).next(context)->asString(context);

  if(XPath2Utils::equals(prefix, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString)) {
    prefix = 0; 
  }

  Node::Ptr node = (Node::Ptr)getParamNumber(2,context).next(context);

  Result namespaces = node->dmNamespaceNodes(context);
  Node::Ptr ns;
  while((ns = (Node::Ptr)namespaces.next(context)).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      if(prefix == 0) {
        return Sequence(context->getPathanFactory()->createAnyURI(ns->dmStringValue(context), context), context->getMemoryManager());
      }
    }
    else {
      if(XPath2Utils::equals(((const ATQNameOrDerived*)name.get())->getName(), prefix)) {
        return Sequence(context->getPathanFactory()->createAnyURI(ns->dmStringValue(context), context), context->getMemoryManager());
      }
    }
  }

  return Sequence(context->getMemoryManager());
}

