/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionError.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/Item.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionError::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionError::minArgs = 0;
const unsigned int FunctionError::maxArgs = 3;

/*
 * fn:error() as none
 * fn:error($error as xs:QName) as none
 * fn:error($error as xs:QName?, $description as xs:string) as none
 * fn:error($error as xs:QName?, $description as xs:string, $error-object as item()*) as none
 */

FunctionError::FunctionError(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "QName?, string, item()*", args, memMgr)
{
}

DataItem* FunctionError::staticResolution(StaticContext *context)
{
  _src.forceNoFolding(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionError::collapseTreeInternal(DynamicContext* context, int flags) const
{
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_name(1023, context->getMemoryManager());
    exc_name.set(X("User-requested error"));
    switch(getNumArgs())
    {
    case 0: exc_name.append(X(" [err:FOER0000]"));
            break;
    case 3: // TODO: extra storage in the exception object for the user object
    case 2: {
                Sequence arg=getParamNumber(2,context);
                exc_name.append(X(": "));
                exc_name.append(arg.first()->asString(context));
            }
    case 1: {
                Sequence arg=getParamNumber(1,context);
                if(arg.isEmpty())
                    exc_name.set(X("The first argument of fn:error must be a valid QName"));
                else
                {
                    exc_name.append(X(" ["));
                    exc_name.append(arg.first()->asString(context));
                    exc_name.append(X("]"));
                }
            }
    }
    DSLthrow(XPath2ErrorException, X("FunctionError::collapseTreeInternal"), exc_name.getRawBuffer());
}


