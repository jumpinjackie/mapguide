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
 * $Id: FunctionSubstring.cpp,v 1.13 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionSubstring.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>

const XMLCh FunctionSubstring::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionSubstring::minArgs = 2;
const unsigned int FunctionSubstring::maxArgs = 3;


/**
 * fn:substring($sourceString as xs:string?, $startingLoc as xs:double) as xs:string
 * fn:substring($sourceString as xs:string?, $startingLoc as xs:double, $length as xs:double) as xs:string
**/

FunctionSubstring::FunctionSubstring(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, double, double", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionSubstring::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Sequence string=getParamNumber(1, context)->toSequence(context);
  if(string.isEmpty())
    return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), memMgr);
	
  ATStringOrDerived::Ptr str = (const ATStringOrDerived::Ptr )string.first();

  Sequence startingLoc=getParamNumber(2,context)->toSequence(context);
  ATDoubleOrDerived::Ptr index = (const ATDoubleOrDerived::Ptr )startingLoc.first();
  ATDoubleOrDerived::Ptr subStrLength;
  if(getNumArgs()>2)
  {
    Sequence length=getParamNumber(3,context)->toSequence(context);
    subStrLength=(const ATDoubleOrDerived::Ptr )length.first();
  }
  else {
    subStrLength=(const ATDoubleOrDerived::Ptr )context->getItemFactory()->createDouble((long)((const ATStringOrDerived*)str)->getLength(), context);
  }
  
  return Sequence(((const ATStringOrDerived*)str)->substring(index, subStrLength, context), memMgr);
}
