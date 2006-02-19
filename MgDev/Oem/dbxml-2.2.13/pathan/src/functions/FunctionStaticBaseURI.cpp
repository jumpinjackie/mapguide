/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionStaticBaseURI.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>

const XMLCh FunctionStaticBaseURI::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStaticBaseURI::minArgs = 0;
const unsigned int FunctionStaticBaseURI::maxArgs = 0;

/**
 * fn:static-base-uri() as xs:anyURI?
**/

FunctionStaticBaseURI::FunctionStaticBaseURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

DataItem* FunctionStaticBaseURI::staticResolution(StaticContext *context) {
  _src.staticBaseURIUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionStaticBaseURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
    const XMLCh* uri = context->getBaseURI();
    if(uri==NULL)
        return Sequence(context->getMemoryManager());

    return Sequence(context->getPathanFactory()->createAnyURI(uri, context), context->getMemoryManager());
}
