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
 * $Id: FunctionCollection.cpp,v 1.18 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCollection.hpp>

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

const XMLCh FunctionCollection::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCollection::minArgs = 0;
const unsigned int FunctionCollection::maxArgs = 1;

/**
 * fn:collection() as node()*
 * fn:collection($arg as xs:string?) as node()*
**/

FunctionCollection::FunctionCollection(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

ASTNode* FunctionCollection::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionCollection::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
                     StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
  _src.availableCollectionsUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionCollection::collapseTreeInternal(DynamicContext* context, int flags) const
{
  //args 0 - URI to resolve
  if(getNumArgs()==0)
      return context->resolveDefaultCollection();

  Sequence arg = getParamNumber(1, context)->toSequence(context);
  if(arg.isEmpty())
    return context->resolveDefaultCollection();

  const XMLCh* currentUri = arg.first()->asString(context);
  if(!XPath2Utils::isValidURI(currentUri, context->getMemoryManager()))
    XQThrow(FunctionException, X("FunctionCollection::collapseTreeInternal"), X("Invalid URI format [err:FODC0002]"));

  return context->resolveCollection(currentUri, this);
}
