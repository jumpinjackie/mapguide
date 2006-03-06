/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionUpperCase.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionUpperCase::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chDash, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionUpperCase::minArgs = 1;
const unsigned int FunctionUpperCase::maxArgs = 1;

/*
 * fn:upper-case($arg as xs:string?) as xs:string
 */

FunctionUpperCase::FunctionUpperCase(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

Sequence FunctionUpperCase::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  Sequence arg=getParamNumber(1,context);
  if(arg.isEmpty()) {
    return Sequence(context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
  }
	const XMLCh *src = arg.first()->asString(context);
	const XMLCh* str = XPath2Utils::toUpper(src, memMgr);

  return Sequence(context->getPathanFactory()->createString(str, context), memMgr);
}
