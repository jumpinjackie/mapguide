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
 * $Id: FunctionLowerCase.cpp,v 1.13 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionLowerCase.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/UCANormalizer.hpp>

#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionLowerCase::name[] = {
  chLatin_l, chLatin_o, chLatin_w, 
  chLatin_e, chLatin_r, chDash, 
  chLatin_c, chLatin_a, chLatin_s, 
  chLatin_e, chNull 
};
const unsigned int FunctionLowerCase::minArgs = 1;
const unsigned int FunctionLowerCase::maxArgs = 1;

/*
 * fn:lower-case($arg as xs:string?) as xs:string
 */

FunctionLowerCase::FunctionLowerCase(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionLowerCase::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager *memMgr = context->getMemoryManager();

  Item::Ptr arg = getParamNumber(1, context)->next(context);
  if(arg.isNull()) {
    return Sequence(context->getItemFactory()->createString(XMLUni::fgZeroLenString, context), memMgr);
  }

  XMLBuffer buf(1023, context->getMemoryManager());
  Normalizer::lowerCase(arg->asString(context), buf);
  return Sequence(context->getItemFactory()->createString(buf.getRawBuffer(), context), memMgr);
}
















