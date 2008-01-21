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
 * $Id: FunctionNodeName.cpp,v 1.10 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNodeName.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/context/DynamicContext.hpp>

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

FunctionNodeName::FunctionNodeName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::QNAME_TYPE;
}

Sequence FunctionNodeName::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence items=getParamNumber(1,context)->toSequence(context);
  if(items.isEmpty())
    return Sequence(context->getMemoryManager());
  const Node::Ptr node = (const Node::Ptr )items.first();
  return Sequence(node->dmNodeName(context), context->getMemoryManager());
}
















