/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionTranslate.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>

#include <map>

const XMLCh FunctionTranslate::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionTranslate::minArgs = 3;
const unsigned int FunctionTranslate::maxArgs = 3;

/**
 * fn:translate($arg as xs:string?, $mapString as xs:string, $transString as xs:string) as xs:string
**/

FunctionTranslate::FunctionTranslate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string,string", args, memMgr)
{
}

Sequence FunctionTranslate::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XPath2MemoryManager* memMgr = context->getMemoryManager();
	Sequence str1 = getParamNumber(1,context);

	if(str1.isEmpty()) {
    return Sequence(context->getPathanFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
  }
 
	Sequence str2 = getParamNumber(2,context);
	Sequence str3 = getParamNumber(3,context);

	const XMLCh* container = str1.first()->asString(context);
	const XMLCh* search = str2.first()->asString(context);
	const XMLCh* replace = str3.first()->asString(context);

	const XMLCh* newString = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;

  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(search)==0)
    return Sequence(context->getPathanFactory()->createString(container, context), memMgr);

  std::map<XMLCh,XMLCh> Change;

	for ( unsigned int i = 0; i < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(search) ; i++)
  {
    if(Change.find(search[i])!=Change.end())
      continue;
		if(i<XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(replace))
			Change[search[i]] = replace[i];
		else
			Change[search[i]] = 0;
  }
  
	for (unsigned int j = 0; j < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(container) ; j++) 
	{
		if (Change.find(container[j]) == Change.end()) {
		  newString = XPath2Utils::concatStrings(newString, container[j], memMgr);
		}
		else
			if (Change[container[j]] != 0 ) {
			  newString = XPath2Utils::concatStrings(newString, Change[container[j]], memMgr);
			}
    }

	return Sequence(context->getPathanFactory()->createString(newString, context), memMgr);
}
