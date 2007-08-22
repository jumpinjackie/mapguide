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
 * $Id: FunctionNormalizeUnicode.cpp,v 1.17 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNormalizeUnicode.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/utils/UCANormalizer.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionNormalizeUnicode::name[] = {
  chLatin_n, chLatin_o, chLatin_r, 
  chLatin_m, chLatin_a, chLatin_l, 
  chLatin_i, chLatin_z, chLatin_e, 
  chDash,    chLatin_u, chLatin_n, 
  chLatin_i, chLatin_c, chLatin_o, 
  chLatin_d, chLatin_e, chNull 
};
const unsigned int FunctionNormalizeUnicode::minArgs = 1;
const unsigned int FunctionNormalizeUnicode::maxArgs = 2;

/**
 * fn:normalize-unicode($arg as xs:string?) as xs:string
 * fn:normalize-unicode($arg as xs:string?, $normalizationForm as xs:string) as xs:string
 **/

FunctionNormalizeUnicode::FunctionNormalizeUnicode(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionNormalizeUnicode::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  Sequence strParm=getParamNumber(1,context)->toSequence(context);
  if(strParm.isEmpty()) {
    return Sequence(context->getItemFactory()->createString(XMLUni::fgZeroLenString, context), memMgr);
  }

  const XMLCh *str = strParm.first()->asString(context);

  static const XMLCh fg_NFC[] = { chLatin_N, chLatin_F, chLatin_C, chNull };
  static const XMLCh fg_NFD[] = { chLatin_N, chLatin_F, chLatin_D, chNull };
  static const XMLCh fg_NFKC[] = { chLatin_N, chLatin_F, chLatin_K, chLatin_C, chNull };
  static const XMLCh fg_NFKD[] = { chLatin_N, chLatin_F, chLatin_K, chLatin_D, chNull };
  static const XMLCh fg_fully[] = { chLatin_F, chLatin_U, chLatin_L,
                                    chLatin_L, chLatin_Y, chDash,
                                    chLatin_N, chLatin_O, chLatin_R,
                                    chLatin_M, chLatin_A, chLatin_L,
                                    chLatin_I, chLatin_Z, chLatin_E,
                                    chLatin_D, chNull };

  const XMLCh* normalization = fg_NFC;
  if(getNumArgs()==2)
  {
    Sequence normParam=getParamNumber(2,context)->toSequence(context);
    const XMLCh *src = normParam.first()->asString(context);
    normalization = XPath2Utils::toUpper(src, memMgr);

    if (XMLString::stringLen(normalization) > 0)
    {
      unsigned int i;
      // remove leading spaces
      for(i = 0; i < XMLString::stringLen(normalization); i++) 
      {
        XMLCh ch = normalization[i];
        if((ch != 0x9) && (ch != 0xA) && (ch != 0xD) && (ch != 0x20))
          break;
      }
      const XMLCh *frontChop = XPath2Utils::subString(normalization, i,XMLString::stringLen(normalization)-i, memMgr);

      // remove trailing spaces
      for(i = XMLString::stringLen(frontChop)-1; i !=0 ; i--) 
      {
        XMLCh ch = frontChop[i];
        if((ch != 0x9) && (ch != 0xA) && (ch != 0xD) && (ch != 0x20))
          break;
      }
      normalization = XPath2Utils::subString(frontChop, 0, i+1, memMgr);
    }
  }

  if(XMLString::stringLen(normalization) == 0) {
    return Sequence(context->getItemFactory()->createString(str, context), memMgr);
  }
  else {
    XMLBuffer buf(1023, context->getMemoryManager());

    if(XPath2Utils::equals(normalization, fg_NFC)) {
      Normalizer::normalizeC(str, buf);
    }
    else if(XPath2Utils::equals(normalization, fg_NFD)) {
      Normalizer::normalizeD(str, buf);
    }
    else if(XPath2Utils::equals(normalization, fg_NFKC)) {
      Normalizer::normalizeKC(str, buf);
    }
    else if(XPath2Utils::equals(normalization, fg_NFKD)) {
      Normalizer::normalizeKD(str, buf);
    }
    else if(XPath2Utils::equals(normalization, fg_fully)) {
      XQThrow(FunctionException, X("FunctionNormalizeUnicode::collapseTreeInternal"), X("Unsupported normalization form [err:FOCH0003]."));
    }
    else { 
      XQThrow(FunctionException, X("FunctionNormalizeUnicode::collapseTreeInternal"), X("Invalid normalization form [err:FOCH0003]."));
   }

    return Sequence(context->getItemFactory()->createString(buf.getRawBuffer(), context), memMgr);
  }

  // Never reached
	return Sequence(memMgr);
}
