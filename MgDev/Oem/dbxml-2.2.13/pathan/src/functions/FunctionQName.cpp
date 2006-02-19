/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionQName.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/ATQNameOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionQName::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_N, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionQName::minArgs = 2;
const unsigned int FunctionQName::maxArgs = 2;

/**
 * fn:QName($paramURI as xs:string?, $paramLocal as xs:string) as xs:QName
**/

/** Returns a QName with the URI given in $paramURI and the localname
 * given in $paramLocal **/

FunctionQName::FunctionQName(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string", args, memMgr)
{
}

Sequence FunctionQName::collapseTreeInternal(DynamicContext* context, int flags) const {
  Sequence paramURIseq = getParamNumber(1, context);
  Sequence paramLocalseq = getParamNumber(2, context);

  const XMLCh* uri = NULL;
  if(!paramURIseq.isEmpty())
    uri=paramURIseq.first()->asString(context);
  const XMLCh* local = paramLocalseq.first()->asString(context);
  const XMLCh* prefix = XPath2NSUtils::getPrefix(local, context->getMemoryManager());
  local = XPath2NSUtils::getLocalName(local);
  //Construct QName here
  Sequence result(context->getPathanFactory()->createQName(uri, prefix, local, context),
                  context->getMemoryManager());

	return result;
}


