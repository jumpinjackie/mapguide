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
 * $Id: FunctionNumber.cpp,v 1.16 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <assert.h>

const XMLCh FunctionNumber::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNumber::minArgs = 0;
const unsigned int FunctionNumber::maxArgs = 1;

/**
 * fn:number() as xs:double
 * fn:number($arg as anyAtomicType?) as xs:double
**/

FunctionNumber::FunctionNumber(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr)
{
}

ASTNode* FunctionNumber::staticResolution(StaticContext *context)
{
  if(!_args.empty() && (*_args.begin())->getType()==ASTNode::CONTEXT_ITEM)
      _args.clear();
  return resolveArguments(context);
}

ASTNode *FunctionNumber::staticTyping(StaticContext *context)
{
  _src.clear();

  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  _src.getStaticType().flags = StaticType::DOUBLE_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionNumber::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  
  Item::Ptr item = 0;
  if(getNumArgs() == 0) {
    item = context->getContextItem();
    if(item == NULLRCP) {
      XQThrow(FunctionException, X("FunctionNumber::collapseTreeInternal"), X("Undefined context item in fn:number [err:XPDY0002]"));
    }

    if(item->isNode())
    {
        Sequence typedValue = ((Node *)item.get())->dmTypedValue(context);
        if(typedValue.getLength() < 1) {
          XQThrow(XPath2TypeMatchException, X("FunctionNumber::collapseTreeInternal"),
                  X("SequenceType matching failed: the sequence does not contain items [err:XPTY0004]"));
        }
        if(typedValue.getLength() > 1) {
          XQThrow(XPath2TypeMatchException, X("FunctionNumber::collapseTreeInternal"),
                  X("SequenceType matching failed: the sequence contains more than one item [err:XPTY0004]"));
        }
        item = typedValue.first();
    }
  }
  else {
    item = getParamNumber(1, context)->next(context);
  }

  return Sequence(number((const AnyAtomicType::Ptr )item, context), memMgr);
}

Item::Ptr FunctionNumber::number(const AnyAtomicType::Ptr &item, DynamicContext *context)
{
  if(item == NULLRCP) {
    return (const Item::Ptr)context->getItemFactory()->createDouble(Numeric::NaN_string, context);
  }
  else {
    try {
      return (const Item::Ptr)item->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                     XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    } catch (XPath2TypeCastException &e) {
      return (const Item::Ptr)context->getItemFactory()->createDouble(Numeric::NaN_string, context);
    }   
  }
}
