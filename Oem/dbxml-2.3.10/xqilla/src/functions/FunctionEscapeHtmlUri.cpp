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
 * $Id: FunctionEscapeHtmlUri.cpp,v 1.6 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionEscapeHtmlUri.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>

const XMLCh FunctionEscapeHtmlUri::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionEscapeHtmlUri::minArgs = 1;
const unsigned int FunctionEscapeHtmlUri::maxArgs = 1;

static const XMLCh HEX_DIGITS[16] = 
{ 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_4, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_6, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_7, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_8, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_B, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_C, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_E, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F
};

/*
  fn:escape-html-uri($uri as xs:string?) as xs:string
 */

FunctionEscapeHtmlUri::FunctionEscapeHtmlUri(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionEscapeHtmlUri::collapseTreeInternal(DynamicContext* context, int flags) const
{
    Sequence uriPart=getParamNumber(1,context)->toSequence(context);
    if(uriPart.isEmpty())
        return Sequence(context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context), context->getMemoryManager());

    const XMLCh* source = uriPart.first()->asString(context);
    unsigned len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(source);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer outString(len+1, context->getMemoryManager());
    XERCES_CPP_NAMESPACE_QUALIFIER XMLUTF8Transcoder utf8Trans(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgUTF8EncodingString, 10, context->getMemoryManager());
    for(unsigned i=0;i<len;i++)
    {
        if(source[i]>=32 && source[i]<=126)
            outString.append(source[i]);
        else
        {
            XMLByte utf8Str[8];
            unsigned int charsEaten;
            unsigned int nLen=utf8Trans.transcodeTo(&source[i], 1, utf8Str, 7, charsEaten, XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder::UnRep_RepChar);
            for(unsigned int j=0;j<nLen;j++)
            {
                outString.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
                outString.append(HEX_DIGITS[utf8Str[j] >> 4]);
                outString.append(HEX_DIGITS[utf8Str[j] & 0xF]);
            }
        }
    }

    return Sequence(context->getItemFactory()->createString(outString.getRawBuffer(), context), context->getMemoryManager());
}
