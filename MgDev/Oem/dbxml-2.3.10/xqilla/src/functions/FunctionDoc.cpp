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
 * $Id: FunctionDoc.cpp,v 1.17 2006/11/01 16:37:19 jpcs Exp $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionDoc::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDoc::minArgs = 1;
const unsigned int FunctionDoc::maxArgs = 1;

/**
 * fn:doc($uri as xs:string?) as document?
 **/
FunctionDoc::FunctionDoc(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

ASTNode* FunctionDoc::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionDoc::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
                     StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::ONENODE);
  _src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
  _src.availableDocumentsUsed(true);

  return calculateSRCForArguments(context);
}

Sequence FunctionDoc::collapseTreeInternal(DynamicContext* context, int flags) const {
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
    XQThrow(FunctionException, X("FunctionDoc::collapseTreeInternal"), X("Invalid argument to fn:doc function [err:FODC0005]"));

  try {
    return context->resolveDocument(uri, this);
  } 
  //TODO:  once DocumentCacheImpl can throw different errors, we should be able to throw the correct corresponding error messages.
  catch(XMLParseException &e) {
    XQThrow(FunctionException, X("FunctionDoc::collapseTreeInternal"), e.getError());
  }
	return Sequence(context->getMemoryManager());
}
