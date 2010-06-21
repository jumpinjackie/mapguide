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
 * $Id: FunctionCodepointEqual.cpp,v 1.14 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCodepointEqual.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

const XMLCh FunctionCodepointEqual::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const unsigned int FunctionCodepointEqual::minArgs = 2;
const unsigned int FunctionCodepointEqual::maxArgs = 2;

/**
 * fn:codepoint-equal($comparand1 as xs:string?, $comparand2 as xs:string?) as xs:boolean?
**/

FunctionCodepointEqual::FunctionCodepointEqual(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?,string?", args, memMgr)
{
}

ASTNode* FunctionCodepointEqual::staticResolution(StaticContext *context)
{
  collation_ = context->getCollation(CodepointCollation::getCodepointCollationName(), this);

  return resolveArguments(context);
}

ASTNode *FunctionCodepointEqual::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;

  return calculateSRCForArguments(context);
}

Sequence FunctionCodepointEqual::collapseTreeInternal(DynamicContext* context, int flags) const
{
    Item::Ptr str1 = getParamNumber(1,context)->next(context);
    Item::Ptr str2 = getParamNumber(2,context)->next(context);
    if(str1.isNull() || str2.isNull())
        return Sequence(context->getMemoryManager());

    const XMLCh* string1 = str1->asString(context);
    const XMLCh* string2 = str2->asString(context);
    
    return Sequence(context->getItemFactory()->createBoolean(collation_->compare(string1,string2)==0, context),
                    context->getMemoryManager());
}
