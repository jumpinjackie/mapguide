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
 * $Id: FunctionDocumentURI.cpp,v 1.9 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionDocumentURI.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>

const XMLCh FunctionDocumentURI::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDocumentURI::minArgs = 1;
const unsigned int FunctionDocumentURI::maxArgs = 1;

/*
 * fn:document-uri($arg as node()?) as xs:anyURI?
 */

FunctionDocumentURI::FunctionDocumentURI(const VectorOfASTNodes  &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::ANY_URI_TYPE;
}

Sequence FunctionDocumentURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1,context)->toSequence(context);
  if(arg.isEmpty())
    return Sequence(context->getMemoryManager());
  return ((Node*)(const Item*)arg.first())->dmDocumentURI(context);
}
