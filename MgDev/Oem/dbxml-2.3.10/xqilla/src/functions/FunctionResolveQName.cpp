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
 * $Id: FunctionResolveQName.cpp,v 1.14 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionResolveQName.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

#include <xercesc/util/XMLChar.hpp>

const XMLCh FunctionResolveQName::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Q, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionResolveQName::minArgs = 2;
const unsigned int FunctionResolveQName::maxArgs = 2;

/**
 * fn:resolve-QName($qname as xs:string?, $element as element()) as xs:QName?
**/

/** Returns an QName with the lexical form given in the first argument. The 
 *  prefix is resolved using the in-scope namespaces for a given element. **/
FunctionResolveQName::FunctionResolveQName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, element()", args, memMgr)
{
  _src.getStaticType().flags = StaticType::QNAME_TYPE;
}

Sequence FunctionResolveQName::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence arg1=getParamNumber(1, context)->toSequence(context);
  if(arg1.isEmpty())
    return Sequence(memMgr);
    
  const XMLCh* paramQName = arg1.first()->asString(context);
  if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isValidQName(paramQName, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(paramQName)))
    XQThrow(FunctionException,X("FunctionResolveQName::collapseTreeInternal"),X("The first argument to fn:resolve-QName is not a valid xs:QName [err:FOCA0002]"));

  const XMLCh* prefix = XPath2NSUtils::getPrefix(paramQName, memMgr);
  const XMLCh* localName = XPath2NSUtils::getLocalName(paramQName);

  Sequence arg2=getParamNumber(2, context)->toSequence(context);
  Node::Ptr node = (Node::Ptr)arg2.first();

  bool noPrefix = XPath2Utils::equals(prefix, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString);
  const XMLCh* namespaceURI = 0;

  Result namespaces = node->dmNamespaceNodes(context, this);
  Node::Ptr ns;
  while((ns = (Node::Ptr)namespaces->next(context)).notNull()) {
    ATQNameOrDerived::Ptr name = ns->dmNodeName(context);
    if(name.isNull()) {
      if(noPrefix) {
        namespaceURI = ns->dmStringValue(context);
        break;
      }
    }
    else if(XPath2Utils::equals(((const ATQNameOrDerived*)name.get())->getName(), prefix)) {
      namespaceURI = ns->dmStringValue(context);
      break;
    }
  }

  if(!noPrefix && namespaceURI == 0) {
    XQThrow(FunctionException, X("FunctionResolveQName::collapseTreeInternal"),X("No namespace found for prefix [err:FONS0004]."));
  }

  Sequence result(context->getItemFactory()->createQName(namespaceURI, prefix, localName, context), memMgr);

  return result; 
}
