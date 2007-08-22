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
 * $Id: FunctionError.cpp,v 1.14 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionError.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

const XMLCh FunctionError::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionError::minArgs = 0;
const unsigned int FunctionError::maxArgs = 3;

/*
 * fn:error() as none
 * fn:error($error as xs:QName) as none
 * fn:error($error as xs:QName?, $description as xs:string) as none
 * fn:error($error as xs:QName?, $description as xs:string, $error-object as item()*) as none
 */

FunctionError::FunctionError(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "QName?, string, item()*", args, memMgr)
{
}

ASTNode* FunctionError::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionError::staticTyping(StaticContext *context)
{
  _src.clear();

  // we need to specify ITEM_TYPE, or a sequence type applied on us will always fail
  _src.getStaticType().flags = StaticType::ITEM_TYPE;
  _src.forceNoFolding(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionError::collapseTreeInternal(DynamicContext* context, int flags) const
{
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_name(1023, context->getMemoryManager());
    exc_name.set(X("User-requested error"));
    switch(getNumArgs())
    {
    case 0: break;
    case 3: // TODO: extra storage in the exception object for the user object
    case 2: {
                Sequence arg=getParamNumber(2,context)->toSequence(context);
                exc_name.append(X(": "));
                exc_name.append(arg.first()->asString(context));
            }
    case 1: {
                Sequence arg=getParamNumber(1,context)->toSequence(context);
                if(arg.isEmpty())
                {
                    if(getNumArgs()==1)
                        XQThrow(XPath2ErrorException, X("FunctionError::collapseTreeInternal"), X("ItemType matching failed [err:XPTY0004]"));
                }
                else
                {
                    exc_name.append(X(" ["));
                    exc_name.append(arg.first()->asString(context));
                    exc_name.append(X("]"));
                }
            }
    }
    exc_name.append(X(" [err:FOER0000]"));
    XQThrow(XPath2ErrorException, X("FunctionError::collapseTreeInternal"), exc_name.getRawBuffer());
}


