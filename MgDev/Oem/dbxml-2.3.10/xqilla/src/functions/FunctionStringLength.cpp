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
 * $Id: FunctionStringLength.cpp,v 1.14 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionStringLength.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/regx/RegxUtil.hpp>

const XMLCh FunctionStringLength::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStringLength::minArgs = 0;
const unsigned int FunctionStringLength::maxArgs = 1;

/**
 * fn:string-length() as xs:integer
 * fn:string-length($arg as xs:string?) as xs:integer
**/

FunctionStringLength::FunctionStringLength(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?", args, memMgr)
{
}

const XMLCh* FunctionStringLength::getString(DynamicContext* context) const {
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  //setup xf:string with empty args
  VectorOfASTNodes args=VectorOfASTNodes(XQillaAllocator<ASTNode*>(context->getMemoryManager()));
  FunctionString stringGrabber(args, memMgr);
  //call xf:string and extract result
  return stringGrabber.collapseTree(context)->next(context)->asString(context);
}

ASTNode* FunctionStringLength::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionStringLength::staticTyping(StaticContext *context)
{
  _src.clear();

  if(_args.empty()) {
    _src.contextItemUsed(true);
  }
  _src.getStaticType().flags = StaticType::DECIMAL_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionStringLength::collapseTreeInternal(DynamicContext* context, int flags) const
{ 
    const XMLCh* str;
    XPath2MemoryManager* memMgr = context->getMemoryManager();
    if (getNumArgs() == 0) {
        str = getString(context);
    } else {

        Sequence strParm=getParamNumber(1,context)->toSequence(context);
        if(strParm.isEmpty())
            return Sequence(context->getItemFactory()->createInteger(0, context), memMgr);

        str = strParm.first()->asString(context);
    }
    const XMLCh* cursor=str;
    long length=0;
    while(*cursor)
    {
        length++;
        if(XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isHighSurrogate(*cursor) && 
           XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isLowSurrogate(*(cursor+1)))
            cursor++;
        cursor++;
    }
    return Sequence(context->getItemFactory()->createInteger(length, context), memMgr);
}
