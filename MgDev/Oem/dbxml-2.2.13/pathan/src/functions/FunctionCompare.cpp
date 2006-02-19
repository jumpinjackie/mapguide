/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCompare.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionCompare::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCompare::minArgs = 2;
const unsigned int FunctionCompare::maxArgs = 3;

/**
 * fn:compare($comparand1 as xs:string?, $comparand2 as xs:string?) as xs:integer?
 * fn:compare($comparand1 as xs:string?, $comparand2 as xs:string?, $collation as xs:string) as xs:integer?
**/

FunctionCompare::FunctionCompare(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string?,string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
}

Sequence FunctionCompare::collapseTreeInternal(DynamicContext* context, int flags) const
{
    Sequence str1 = getParamNumber(1,context);
    Sequence str2 = getParamNumber(2,context);
    if(str1.isEmpty() || str2.isEmpty())
        return Sequence(context->getMemoryManager());

    Collation* collation = NULL;
    if(getNumArgs()>2) {
        Sequence collArg = getParamNumber(3,context);
        const XMLCh* collName = collArg.first()->asString(context);
        try {
            context->getPathanFactory()->createAnyURI(collName, context);
        } catch(XPath2ErrorException &e) {
            DSLthrow(FunctionException, X("FunctionCompare::collapseTreeInternal"), X("Invalid argument to compare function"));  
        }
        collation = context->getCollation(collName);
        if(collation == NULL)
            DSLthrow(FunctionException,X("FunctionCompare::collapseTreeInternal"),X("Collation object is not available"));
    }
    else
        collation = context->getDefaultCollation();
    if(collation == NULL)
        collation = context->getCollation(CodepointCollation::getCodepointCollationName());

    const XMLCh* string1 = str1.first()->asString(context);
    const XMLCh* string2 = str2.first()->asString(context);
    Sequence result(context->getPathanFactory()->createInteger(collation->compare(string1,string2), context), context->getMemoryManager());

    return result;
}
