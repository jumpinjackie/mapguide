/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionStringLength.hpp>
#include <pathan/functions/FunctionString.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionStringLength::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStringLength::minArgs = 0;
const unsigned int FunctionStringLength::maxArgs = 1;

/**
 * fn:string-length() as xs:integer
 * fn:string-length($arg as xs:string?) as xs:integer
**/

FunctionStringLength::FunctionStringLength(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

const XMLCh* FunctionStringLength::getString(DynamicContext* context) const {
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  //setup xf:string with empty args
  VectorOfDataItems args=VectorOfDataItems(PathanAllocator<DataItem*>(context->getMemoryManager()));
  FunctionString stringGrabber(args, memMgr);
  //call xf:string and extract result
  return stringGrabber.collapseTree(context).next(context)->asString(context);
}

DataItem* FunctionStringLength::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionStringLength::collapseTreeInternal(DynamicContext* context, int flags) const
{ 
    const XMLCh* str;
    XPath2MemoryManager* memMgr = context->getMemoryManager();
    if (getNumArgs() == 0) {
        str = getString(context);
    } else {

        Sequence strParm=getParamNumber(1,context);
        if(strParm.isEmpty())
            return Sequence(context->getPathanFactory()->createInteger(0, context), memMgr);

        str = strParm.first()->asString(context);
    }
    const ATDecimalOrDerived::Ptr length = context->getPathanFactory()->createInteger((long)XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(str), context);
    return Sequence(length, memMgr);
}
