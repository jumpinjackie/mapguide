/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionSubstringBefore.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLUni.hpp>

const XMLCh FunctionSubstringBefore::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionSubstringBefore::minArgs = 2;
const unsigned int FunctionSubstringBefore::maxArgs = 3;

/**
 * fn:substring-before($arg1 as xs:string?, $arg2 as xs:string?) as xs:string
 * fn:substring-before($arg1 as xs:string?, $arg2 as xs:string?, $collation as xs:string) as xs:string
**/

FunctionSubstringBefore::FunctionSubstringBefore(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string?,string", args, memMgr)
{
}

Sequence FunctionSubstringBefore::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

	Sequence str1=getParamNumber(1,context);
	Sequence str2=getParamNumber(2,context);

	Collation* collation=NULL;
	if(getNumArgs()>2) {
    Sequence collArg = getParamNumber(3,context);
    const XMLCh* collName=collArg.first()->asString(context);
    try {
      context->getPathanFactory()->createAnyURI(collName, context);
    } catch(XPath2ErrorException &e) {
      DSLthrow(FunctionException, X("FunctionSubstringAfter::collapseTreeInternal"), X("Invalid collationURI"));  
    }
	  collation=context->getCollation(collName);
    if(collation==NULL)
      DSLthrow(FunctionException,X("FunctionSubstringAfter::collapseTreeInternal"),X("Collation object is not available"));
  }
	else
		collation=context->getCollation(CodepointCollation::getCodepointCollationName());

	ATStringOrDerived::Ptr container = NULL;
  if(str1.isEmpty())
    container=context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
  else
    container=(const ATStringOrDerived::Ptr )str1.first();
	ATStringOrDerived::Ptr pattern = NULL;
  if(str2.isEmpty())
    pattern=context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
  else
    pattern=(const ATStringOrDerived::Ptr )str2.first();

  return Sequence(((const ATStringOrDerived*)container)->substringBefore(pattern, collation, context), memMgr);
}
