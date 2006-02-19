/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionDocumentURI.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Node.hpp>

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

FunctionDocumentURI::FunctionDocumentURI(const VectorOfDataItems  &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

Sequence FunctionDocumentURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Sequence arg = getParamNumber(1,context);
  if(arg.isEmpty())
    return Sequence(context->getMemoryManager());
  return ((Node*)(const Item*)arg.first())->dmDocumentURI(context);
}
