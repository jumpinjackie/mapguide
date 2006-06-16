/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionStartsWith.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

const XMLCh FunctionStartsWith::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStartsWith::minArgs = 2;
const unsigned int FunctionStartsWith::maxArgs = 3;

/**
 * fn:starts-with($arg1 as xs:string?, $arg2 as xs:string?) as xs:boolean
 * fn:starts-with($arg1 as xs:string?, $arg2 as xs:string?, $collation as xs:string) as xs:boolean
**/

FunctionStartsWith::FunctionStartsWith(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string?, string", args, memMgr)
{
}

Sequence FunctionStartsWith::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();

	Sequence sourceString=getParamNumber(1,context);
	Sequence findString=getParamNumber(2,context);

	const XMLCh* source = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
  if(!sourceString.isEmpty())
    source=sourceString.first()->asString(context);
	const XMLCh* find = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
  if(!findString.isEmpty())
    find=findString.first()->asString(context);

	// If the value of $operand1 is the zero-length string and the value of $operand2 is not the zero-length string, 
	// then the function returns false. 
	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(source)==0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(find)>0)
		return Sequence(context->getPathanFactory()->createBoolean(false, context), memMgr);
	// If the value of $operand2 is the zero-length string, then the function returns true
	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(find)==0)
		return Sequence(context->getPathanFactory()->createBoolean(true, context), memMgr);

	Collation* collation=NULL;
	if(getNumArgs()>2) {
    Sequence collArg = getParamNumber(3,context);
    const XMLCh* collName = collArg.first()->asString(context);
    try {
      context->getPathanFactory()->createAnyURI(collName, context);
    } catch(XPath2ErrorException &e) {
      DSLthrow(FunctionException, X("FunctionEndsWith::collapseTreeInternal"), X("Invalid collationURI"));  
    }
	  collation=context->getCollation(collName);
    if(collation==NULL)
      DSLthrow(FunctionException,X("FunctionStartsWith::collapseTreeInternal"),X("Cannot find requested collation"));
  }
	else
		collation=context->getCollation(CodepointCollation::getCodepointCollationName());

	// Returns a boolean indicating whether or not the value of $operand1 ends with a string that is equal to the value 
	// of $operand2 according to the specified collation

	if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(find)>XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(source)) {
		return Sequence(context->getPathanFactory()->createBoolean(false, context), memMgr);
	}
	const XMLCh* string = XPath2Utils::subString(source, 0,XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(find), memMgr);
	bool result = (collation->compare(string,find)==0);

	return Sequence(context->getPathanFactory()->createBoolean(result, context), memMgr);
}
