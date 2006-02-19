/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCodepointEqual.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionCodepointEqual::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const unsigned int FunctionCodepointEqual::minArgs = 2;
const unsigned int FunctionCodepointEqual::maxArgs = 2;

/**
 * fn:codepoint-equal($comparand1 as xs:string?, $comparand2 as xs:string?) as xs:boolean?
**/

FunctionCodepointEqual::FunctionCodepointEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string?", args, memMgr)
{
}

Sequence FunctionCodepointEqual::collapseTreeInternal(DynamicContext* context, int flags) const
{
    Sequence str1 = getParamNumber(1,context);
    Sequence str2 = getParamNumber(2,context);
    if(str1.isEmpty() || str2.isEmpty())
        return Sequence(context->getMemoryManager());

    const XMLCh* string1 = str1.first()->asString(context);
    const XMLCh* string2 = str2.first()->asString(context);
    
    Collation* collation=context->getCollation(CodepointCollation::getCodepointCollationName());
    return Sequence(context->getPathanFactory()->createBoolean(collation->compare(string1,string2)==0, context), context->getMemoryManager());
}
