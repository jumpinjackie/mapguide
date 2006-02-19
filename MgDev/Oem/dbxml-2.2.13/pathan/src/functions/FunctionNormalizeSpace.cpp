/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionNormalizeSpace.hpp>
#include <pathan/functions/FunctionString.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionNormalizeSpace::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNormalizeSpace::minArgs = 0;
const unsigned int FunctionNormalizeSpace::maxArgs = 1;

/**
 * fn:normalize-space() as xs:string
 * fn:normalize-space($arg as xs:string?) as xs:string
**/

FunctionNormalizeSpace::FunctionNormalizeSpace(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

const XMLCh* FunctionNormalizeSpace::getString(DynamicContext* context) const {
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  //setup xf:string with empty args
  VectorOfDataItems args=VectorOfDataItems(PathanAllocator<DataItem*>(memMgr));
  FunctionString stringGrabber(args, memMgr);
  //call xf:string and extract result
  return stringGrabber.collapseTree(context).next(context)->asString(context);
}

DataItem* FunctionNormalizeSpace::staticResolution(StaticContext *context) {
  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  return resolveDataItems(_args, context, !_args.empty());
}

Sequence FunctionNormalizeSpace::collapseTreeInternal(DynamicContext* context, int flags) const
{
    XPath2MemoryManager* memMgr = context->getMemoryManager();
    const XMLCh* str;
    if (getNumArgs() == 0) {
        str = getString(context);
    } else {
        Sequence strParm=getParamNumber(1,context);
        if(strParm.isEmpty())
            return Sequence(context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);

        str = strParm.first()->asString(context);
    }
    const XMLCh* result = NULL;

    int whitespace = 0; 
    for(unsigned int i = 0; i < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(str); i++) 
    {
        XMLCh ch = str[i];

        if((ch == 0x9) || (ch == 0xA) || (ch == 0xD) || (ch == 0x20))
        {
            whitespace++;
            ch = 0x20;
        }
        else
        {
            whitespace = 0;
        }
        if(whitespace < 2)
        {
            result = XPath2Utils::concatStrings(result, ch, memMgr);
        }
    }

    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(result)>0)
    {
        XMLCh first = result[0];
        if((first == 0x9) || (first == 0xA) || (first == 0xD) || (first == 0x20)) {
            result = XPath2Utils::deleteData(result, 0, 1, memMgr); 
        }
    }
    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(result)>0)
    {
        XMLCh last = result[XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(result) - 1];
        if((last == 0x9) || (last == 0xA) || (last == 0xD) || (last == 0x20)) {
            result = XPath2Utils::deleteData(result, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(result) -1, 1, memMgr);
        }
    }
    return Sequence(context->getPathanFactory()->createString(result, context), memMgr);
}

