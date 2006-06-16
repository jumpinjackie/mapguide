/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNumber.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/Numeric.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
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

FunctionNumber::FunctionNumber(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

DataItem* FunctionNumber::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionNumber::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr item = 0;
	if(getNumArgs() == 0) {
		item = context->getContextItem();
    if(item == NULLRCP) {
      DSLthrow(FunctionException, X("FunctionNumber::collapseTreeInternal"),
               X("Undefined context item in fn:number [err:FONC0001]"));
    }
    // Treat like the function parameter was "."
    Sequence ci_seq(item, memMgr);
    Result ci_result(ci_seq);

    StaticType stype;
    stype.flags = StaticType::NODE_TYPE;
    item = ci_result.convertFunctionArg((*_paramDecl)[0], stype, context).next(context);
  }
	else {
		item = getParamNumber(1, context).next(context);
	}
  
  return Sequence(number((const AnyAtomicType::Ptr )item, context), memMgr);
}

Item::Ptr FunctionNumber::number(const AnyAtomicType::Ptr &item, DynamicContext *context)
{
  if(item == NULLRCP) {
    return (const Item::Ptr)context->getPathanFactory()->createDouble(Numeric::NaN_string, context);
  }
  else {
    try {
      return (const Item::Ptr)item->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                     XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    } catch (XPath2TypeCastException &e) {
      return (const Item::Ptr)context->getPathanFactory()->createDouble(Numeric::NaN_string, context);
    }   
  }
}
