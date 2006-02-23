/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionDefaultCollation.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionDefaultCollation::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDefaultCollation::minArgs = 0;
const unsigned int FunctionDefaultCollation::maxArgs = 0;

/**
 * fn:default-collation() as xs:string
**/

FunctionDefaultCollation::FunctionDefaultCollation(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

Sequence FunctionDefaultCollation::collapseTreeInternal(DynamicContext* context, int flags) const
{
  Collation* collation = context->getDefaultCollation();
  if(collation != NULL)
    return Sequence(context->getPathanFactory()->createString(collation->getCollationName(), context),
                    context->getMemoryManager());

  return Sequence(context->getPathanFactory()->createString(CodepointCollation::getCodepointCollationName(), context),
                  context->getMemoryManager());
}
