/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/functions/FunctionResolveURI.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include "../exceptions/InvalidLexicalSpaceException.hpp"
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUri.hpp>

const XMLCh FunctionResolveURI::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionResolveURI::minArgs = 1;
const unsigned int FunctionResolveURI::maxArgs = 2;

/**
 * fn:resolve-uri($relative as xs:string?) as xs:anyURI?
 * fn:resolve-uri($relative as xs:string?, $base as xs:string) as xs:anyURI?
 **/

FunctionResolveURI::FunctionResolveURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string", args, memMgr) 
{
}

Sequence FunctionResolveURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
	Sequence relativeSeq = getParamNumber(1, context);
  if(relativeSeq.isEmpty())
    return Sequence(memMgr);
  const XMLCh* relativeURI = relativeSeq.first()->asString(context);
  try {
    context->getPathanFactory()->createAnyURI(relativeURI, context);
  } catch(InvalidLexicalSpaceException &e) {
     DSLthrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Invalid argument to resolve-uri"));
  }
  
  const XMLCh* baseURI;

  if (getNumArgs() == 1) {
    baseURI = context->getBaseURI();
    if (!baseURI)
      DSLthrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Base uri undefined in the static context"));
  
  } else {
    Sequence baseSeq = getParamNumber(2, context);
    baseURI = baseSeq.first()->asString(context);
  }

	try {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLUri base(baseURI);

    XERCES_CPP_NAMESPACE_QUALIFIER XMLUri full(&base, relativeURI);

    const XMLCh* fullURI = getMemoryManager()->getPooledString(full.getUriText());    
    return Sequence(context->getPathanFactory()->createAnyURI(fullURI, context), memMgr); 

  } catch(InvalidLexicalSpaceException &e){
    DSLthrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Invalid argument to resolve-uri"));
  } catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
    //if can't build, assume its cause there was a relative URI given
    DSLthrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Relative URI base argument to resolve-uri"));
  }
  
  //should not get here
  assert(0);

}
