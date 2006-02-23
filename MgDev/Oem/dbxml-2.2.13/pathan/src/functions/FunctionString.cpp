/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionString.hpp>
#include <assert.h>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>

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

FunctionString::FunctionString(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "item()?", args, memMgr)
{
}

DataItem* FunctionString::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionString::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr item = 0;
	if(getNumArgs() == 0) {
		item = context->getContextItem();
    if(item == NULLRCP) {
      DSLthrow(FunctionException, X("FunctionString::collapseTreeInternal"),
               X("Undefined context item in fn:string [err:FONC0001]"));
    }
  }
	else {
		item = getParamNumber(1, context).next(context);
	}

  return Sequence(string(item, context), memMgr);
}

Item::Ptr FunctionString::string(const Item::Ptr &item, DynamicContext *context)
{
  if(item == NULLRCP) {
    return (const Item::Ptr)context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
  }
	else if(item->isNode()) {
    return (const Item::Ptr)context->getPathanFactory()->createString(((Node*)(const Item*)item)->dmStringValue(context), context);
  }
  else if(item->isAtomicValue()) {
    return (const Item::Ptr)((AnyAtomicType*)(const Item*)item)->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                                XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
	}
  assert(false);
  return 0;
}
