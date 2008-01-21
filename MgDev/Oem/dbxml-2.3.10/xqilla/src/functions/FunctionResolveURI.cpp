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
 * $Id: FunctionResolveURI.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/functions/FunctionResolveURI.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include "../exceptions/InvalidLexicalSpaceException.hpp"
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
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

FunctionResolveURI::FunctionResolveURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?, string", args, memMgr),
    baseURI_(0)
{
}

ASTNode* FunctionResolveURI::staticResolution(StaticContext *context)
{
  if (getNumArgs() == 1) {
    baseURI_ = context->getBaseURI();
  }

  return resolveArguments(context);
}

ASTNode *FunctionResolveURI::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::ANY_URI_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionResolveURI::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  Sequence relativeSeq = getParamNumber(1, context)->toSequence(context);
  if(relativeSeq.isEmpty())
    return Sequence(memMgr);

  const XMLCh* relativeURI = relativeSeq.first()->asString(context);
  try {
    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLUri::isValidURI(false, relativeURI))
      return Sequence(context->getItemFactory()->createAnyURI(relativeURI, context), memMgr); 
  } catch(InvalidLexicalSpaceException &e){
    XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Invalid argument to resolve-uri [err:FORG0002]"));
  }

  try {

    const XMLCh* baseURI;
    if (getNumArgs() == 1) 
    {
      baseURI = baseURI_;
      if (!baseURI)
        XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"),
                X("Base uri undefined in the static context [err:FONS0005]"));
    }
    else 
    {
      Sequence baseSeq = getParamNumber(2, context)->toSequence(context);
      baseURI = baseSeq.first()->asString(context);
    }

    if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLUri::isValidURI(true, relativeURI))
      XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"),
              X("Invalid relative uri argument to resolve-uri [err:FORG0002]"));
    if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLUri::isValidURI(false, baseURI))
      XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"),
              X("Invalid base-uri argument to resolve-uri [err:FORG0002]"));
  
    try {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLUri base(baseURI);

      XERCES_CPP_NAMESPACE_QUALIFIER XMLUri full(&base, relativeURI);

      const XMLCh* fullURI = getMemoryManager()->getPooledString(full.getUriText());    
      return Sequence(context->getItemFactory()->createAnyURI(fullURI, context), memMgr); 

    } catch(InvalidLexicalSpaceException &e){
      XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Invalid argument to resolve-uri [err:FORG0002]"));
    }

  } catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
    //if can't build, assume its cause there was a relative URI given
    XQThrow(FunctionException, X("FunctionResolveURI::collapseTreeInternal"), X("Relative URI base argument to resolve-uri [err:FORG0009]"));
  }
  
  //should not get here
  assert(0);

}
