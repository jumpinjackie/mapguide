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
 * $Id: FunctionString.cpp,v 1.16 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionString.hpp>
#include <assert.h>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

const XMLCh FunctionString::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionString::minArgs = 0;
const unsigned int FunctionString::maxArgs = 1;

/**
 * fn:string() as xs:string
 * fn:string($arg as item()?) as xs:string
**/

FunctionString::FunctionString(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "item()?", args, memMgr)
{
}

ASTNode* FunctionString::staticResolution(StaticContext *context) {
  if(!_args.empty() && (*_args.begin())->getType()==ASTNode::CONTEXT_ITEM)
      _args.clear();
  return resolveArguments(context);
}

ASTNode *FunctionString::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::STRING_TYPE;
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return calculateSRCForArguments(context);
}

Sequence FunctionString::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr item = 0;
  if(getNumArgs() == 0) {
    item = context->getContextItem();
    if(item == NULLRCP) {
      XQThrow(FunctionException, X("FunctionString::collapseTreeInternal"),
              X("Undefined context item in fn:string [err:XPDY0002]"));
    }
  }
  else {
    item = getParamNumber(1, context)->next(context);
  }

  return Sequence(string(item, context), memMgr);
}

Item::Ptr FunctionString::string(const Item::Ptr &item, DynamicContext *context)
{
  if(item == NULLRCP) {
    return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
  }
  else if(item->isNode()) {
    return context->getItemFactory()->createString(((Node*)item.get())->dmStringValue(context), context);
  }
  else if(item->isAtomicValue()) {
    return ((AnyAtomicType*)item.get())->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  }
  assert(false);
  return 0;
}
