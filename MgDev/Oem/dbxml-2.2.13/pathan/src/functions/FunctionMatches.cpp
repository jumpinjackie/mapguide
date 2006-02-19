/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionMatches.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

const XMLCh FunctionMatches::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionMatches::minArgs = 2;
const unsigned int FunctionMatches::maxArgs = 3;

/**
 * fn:matches($input as xs:string?, $pattern as xs:string) as xs:boolean
 * fn:matches($input as xs:string?, $pattern as xs:string, $flags as xs:string) as xs:boolean
 */
  
FunctionMatches::FunctionMatches(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string, string", args, memMgr)
{
}

Sequence FunctionMatches::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence inputString=getParamNumber(1,context);
  Sequence patternString=getParamNumber(2,context);

  const XMLCh* input = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
  if(!inputString.isEmpty())
    input=inputString.first()->asString(context);
	const XMLCh* pattern = patternString.first()->asString(context);
	// If the value of $operand1 is the zero-length string and the value of $operand2 is not the zero-length string,
	// then the function returns false.
	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(input)==0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(pattern)>0)
		return Sequence(context->getPathanFactory()->createBoolean(false, context), memMgr);
	// If the value of $operand2 is the zero-length string, then the function returns true
	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(pattern)==0)
		return Sequence(context->getPathanFactory()->createBoolean(true, context), memMgr);

	const XMLCh* options = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
	if(getNumArgs()>2)
		options=getParamNumber(3,context).castAsSingleString(context);

  //Check that the options are valid - throw an exception if not (can have s,m,i and x)
  //Note: Are allowed to duplicate the letters.
  for (unsigned int i=0; i< XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(options); i ++){
   if (options[i]!= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s && 
       options[i]!= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m &&
       options[i]!= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i &&
       options[i]!= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x) 
     
    DSLthrow(FunctionException, X("FunctionMatches::collapseTreeInternal"),X("Invalid regular expression flags"));  
  }

  //Build the Regular Expression
  try {
    XERCES_CPP_NAMESPACE_QUALIFIER RegularExpression regEx(pattern, options, memMgr);
    return Sequence(context->getPathanFactory()->createBoolean(regEx.matches(input), context), memMgr);
  } catch (XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e){ 
    DSLthrow(FunctionException, X("FunctionMatches::collapseTreeInternal"), e.getMessage());  
  } catch (...){
    DSLthrow(FunctionException, X("FunctionMatches::collapseTreeInternal"),X("Invalid regular expression"));  
  }  

  //do not get here

}
