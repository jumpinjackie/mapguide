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
 * $Id: FunctionNamespaceURIForPrefix.cpp,v 1.12 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNamespaceURIForPrefix.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/functions/FunctionNormalizeSpace.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
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
 * fn:namespace-uri-for-prefix($prefix as xs:string?, $element as element()) as xs:anyURI?
**/

/** Returns the namespace URI of one of the in-scope namespaces for $element, identified 
 * by its namespace prefix.
**/

FunctionNamespaceURIForPrefix::FunctionNamespaceURIForPrefix(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, element()", args, memMgr)
{
  _src.getStaticType().flags = StaticType::ANY_URI_TYPE;
}

Sequence FunctionNamespaceURIForPrefix::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Item::Ptr first=getParamNumber(1, context)->next(context);
  const XMLCh* prefix = 0;
  if(first.notNull())
    prefix=first->asString(context);

  if(XPath2Utils::equals(prefix, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString)) {
    prefix = 0; 
  }

  Node::Ptr node = (Node::Ptr)getParamNumber(2,context)->next(context);

  Result namespaces = node->dmNamespaceNodes(context, this);
  Node::Ptr ns;
  while((ns = (Node::Ptr)namespaces->next(context)).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      if(prefix == 0) {
        return Sequence(context->getItemFactory()->createAnyURI(ns->dmStringValue(context), context), context->getMemoryManager());
      }
    }
    else {
      if(XPath2Utils::equals(((const ATQNameOrDerived*)name.get())->getName(), prefix)) {
        return Sequence(context->getItemFactory()->createAnyURI(ns->dmStringValue(context), context), context->getMemoryManager());
      }
    }
  }

  return Sequence(context->getMemoryManager());
}

