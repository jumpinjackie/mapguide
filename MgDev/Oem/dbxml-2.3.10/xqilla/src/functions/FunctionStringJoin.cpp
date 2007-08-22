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
 * $Id: FunctionStringJoin.cpp,v 1.11 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionStringJoin.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

const XMLCh FunctionStringJoin::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_j, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStringJoin::minArgs = 2;
const unsigned int FunctionStringJoin::maxArgs = 2;

/**
 * fn:string-join($arg1 as xs:string*, $arg2 as xs:string) as xs:string
**/

FunctionStringJoin::FunctionStringJoin(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string*, string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionStringJoin::collapseTreeInternal(DynamicContext* context, int flags) const {
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer result(1023, context->getMemoryManager());
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  Sequence strSeq = getParamNumber(1, context)->toSequence(context);
  const XMLCh* strSep = getParamNumber(2,context)->next(context)->asString(context);

  //if the sequence is non-empty, append the 1st string to the result
  if(!strSeq.isEmpty()) {
    Sequence::iterator end = strSeq.end();
    Sequence::iterator i = strSeq.begin();

    result.append((*i)->asString(context));

    //join the last length - 1 strings each prefixed by the separator
    for(++i; i != end; ++i) {
      result.append(strSep);
      result.append((*i)->asString(context));
    }
  }
  
  const ATStringOrDerived::Ptr strResult = context->getItemFactory()->createString(result.getRawBuffer(), context);
  return Sequence(strResult, memMgr);
}
