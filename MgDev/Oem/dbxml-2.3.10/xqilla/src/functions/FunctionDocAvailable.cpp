/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FunctionDocAvailable.cpp,v 1.15 2006/11/01 16:37:19 jpcs Exp $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
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
FunctionDocAvailable::FunctionDocAvailable(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

ASTNode* FunctionDocAvailable::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionDocAvailable::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
  _src.availableDocumentsUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionDocAvailable::collapseTreeInternal(DynamicContext* context, int flags) const {
  Sequence uriArg = getParamNumber(1,context)->toSequence(context);
  
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
  if(!XPath2Utils::isValidURI(uri, context->getMemoryManager()))
    XQThrow(FunctionException, X("FunctionDocAvailable::collapseTreeInternal"), X("Invalid argument to fn:doc-available function [err:FODC0005]"));

  bool bSuccess=false;
  try {
    bSuccess = !context->resolveDocument(uri, this).isEmpty();
  } 
  catch(...) {
  }
  return Sequence(context->getItemFactory()->createBoolean(bSuccess, context), context->getMemoryManager());
}
