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
 * $Id: FunctionCodepointsToString.cpp,v 1.13 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCodepointsToString.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLChar.hpp>

const XMLCh FunctionCodepointsToString::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCodepointsToString::minArgs = 1;
const unsigned int FunctionCodepointsToString::maxArgs = 1;

/**
 * fn:codepoints-to-string($srcval as integer*) => string
 * Creates a string from a sequence of codepoints.
**/

FunctionCodepointsToString::FunctionCodepointsToString(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "integer*", args, memMgr) // 1 argument
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionCodepointsToString::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer result(1023, context->getMemoryManager());
  Sequence arg = getParamNumber(1,context)->toSequence(context);
  Sequence::iterator end = arg.end();
  for(Sequence::iterator i = arg.begin(); i != end; ++i) {
    XMLInt32 ch=((const ATDecimalOrDerived::Ptr)*i)->treatAsCodepoint(context);
	if ( ch >= 0x10000) 
    {
        XMLCh one, two;
        XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::decomposeToSurrogates(ch, one, two);
        result.append(one);
        result.append(two);
	}
    else
        result.append((XMLCh)ch);
  }
  unsigned int len=result.getLen();
  const XMLCh* str=result.getRawBuffer();
  for(unsigned int j=0;j<len;j++)
  {
      if(XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isHighSurrogate(str[j]))
        if((j+1)==len || !XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isLowSurrogate(str[j+1]) || !XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isXMLChar(str[j], str[j+1]))
          XQThrow(XPath2ErrorException, X("FunctionCodepointsToString::collapseTreeInternal"), X("String contains an invalid XML character [err:FOCH0001]."));
        else
          j++;
      else if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isXMLChar(str[j]))
        XQThrow(XPath2ErrorException, X("FunctionCodepointsToString::collapseTreeInternal"), X("String contains an invalid XML character [err:FOCH0001]."));
  }
  return Sequence(context->getItemFactory()->createString(str, context),
                  context->getMemoryManager());
}

