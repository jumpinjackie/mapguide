/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionCodepointsToString.hpp>
#include <pathan/Item.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

const XMLCh FunctionCodepointsToString::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCodepointsToString::minArgs = 1;
const unsigned int FunctionCodepointsToString::maxArgs = 1;

/**
 * fn:codepoints-to-string($srcval as integer*) => string
 * Creates a string from a sequence of codepoints.
**/

FunctionCodepointsToString::FunctionCodepointsToString(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "integer*", args, memMgr) // 1 argument
{
  // Nothing to do
}

Sequence FunctionCodepointsToString::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer result(1023, context->getMemoryManager());
  Sequence arg = getParamNumber(1,context);
  Sequence::iterator end = arg.end();
  for(Sequence::iterator i = arg.begin(); i != end; ++i) {
    result.append(((const ATDecimalOrDerived*)(const Item*)*i)->treatAsCodepoint(context));
  }

  return Sequence(context->getPathanFactory()->createString(result.getRawBuffer(), context),
                  context->getMemoryManager());
}

