/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionConcat.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionConcat::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionConcat::minArgs = 2;
const unsigned int FunctionConcat::maxArgs = UNLIMITED;

/**
 * fn:concat($arg1 as xdt:anyAtomicType?, $arg2 as xdt:anyAtomicType?, ...) as xs:string
**/


FunctionConcat::FunctionConcat(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr) // Between 2 and UNLIMITED arguments
{
  // Nothing to do
}

Sequence FunctionConcat::collapseTreeInternal(DynamicContext* context, int flags) const
{
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer result(1023, context->getMemoryManager());
	for(unsigned int i = 1; i <= getNumArgs(); ++i) 
	{
		// use this version of getParamNumber so that I can specify that the signature for the argument is always "string?"
		Sequence item = getParamNumber(i,1,context);
		if(!item.isEmpty()) {
		  result.append(item.first()->asString(context));
		}
	}
  const ATStringOrDerived::Ptr strResult = context->getPathanFactory()->createString(result.getRawBuffer(), context);
	/*	cerr << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(strResult.asString()) << endl;*/

	return Sequence(strResult, context->getMemoryManager());
}
