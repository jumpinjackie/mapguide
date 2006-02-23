/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */
#include "../config/pathan_config.h"
#include <pathan/functions/FunctionDocAvailable.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/URIResolver.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/XMLParseException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionDocAvailable::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDocAvailable::minArgs = 1;
const unsigned int FunctionDocAvailable::maxArgs = 1;

/**
 * fn:doc-available($uri as xs:string?) as xs:boolean
 **/
FunctionDocAvailable::FunctionDocAvailable(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

DataItem* FunctionDocAvailable::staticResolution(StaticContext *context) {
  _src.availableDocumentsUsed(true);
  return resolveDataItems(_args, context, false);
}

Sequence FunctionDocAvailable::collapseTreeInternal(DynamicContext* context, int flags) const {
  Sequence uriArg = getParamNumber(1,context);
  
  if (uriArg.isEmpty()) {
    return Sequence(context->getMemoryManager());
  }
  
  const XMLCh* uri = uriArg.first()->asString(context);
  // on Windows, we can have URIs using \ instead of /; let's normalize them
  XMLCh backSlash[]={ XERCES_CPP_NAMESPACE_QUALIFIER chBackSlash, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::findAny(uri,backSlash))
  {
	  XMLCh* newUri=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(uri,context->getMemoryManager());
	  for(unsigned int i=0;i<XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(newUri);i++)
		  if(newUri[i]==XERCES_CPP_NAMESPACE_QUALIFIER chBackSlash)
			  newUri[i]=XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash;
	  uri=newUri;
  }
  try {
    context->getPathanFactory()->createAnyURI(uri, context);
  } catch(XPath2ErrorException &e) {
    DSLthrow(FunctionException, X("FunctionDocAvailable::collapseTreeInternal"), X("Invalid argument to fn:doc-available function [err:FODC0005]"));
  }

  bool bSuccess=false;
  try {
    bSuccess = !context->resolveDocument(uri).isEmpty();
  } 
  catch(...) {
  }
  return Sequence(context->getPathanFactory()->createBoolean(bSuccess, context), context->getMemoryManager());
}
