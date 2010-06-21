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
 * $Id: FunctionMin.cpp,v 1.19 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionMin.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/ContextHelpers.hpp>

const XMLCh FunctionMin::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionMin::minArgs = 1;
const unsigned int FunctionMin::maxArgs = 2;

/**
 * fn:min($arg as xdt:anyAtomicType*) as xdt:anyAtomicType?
 * fn:min($arg as xdt:anyAtomicType*, $collation as string) as xdt:anyAtomicType?
**/

FunctionMin::FunctionMin(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : AggregateFunction(name, minArgs, maxArgs, "anyAtomicType*, string", args, memMgr)
{
}

ASTNode* FunctionMin::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionMin::staticTyping(StaticContext *context)
{
  _src.clear();

  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::TYPED_ATOMIC_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionMin::collapseTreeInternal(DynamicContext* context, int flags) const
{
    XPath2MemoryManager* memMgr = context->getMemoryManager();

    Sequence sequence(memMgr);  
    try {
        sequence = validateSequence(getParamNumber(1,context)->toSequence(context), context);
    } catch (IllegalArgumentException &e) {
        XQThrow(IllegalArgumentException, X("FunctionMin::collapseTreeInternal"), X("Invalid argument to fn:min() function [err:FORG0006]."));
    }

    // Return the empty sequence if the sequence is empty
    if(sequence.isEmpty()) {
        return Sequence(memMgr);
    }
    if(sequence.getLength()==1 && isNumericNaN(sequence.first()))
        return sequence;

    Collation* collation=NULL;
    if (getNumArgs()>1) {
        Sequence collArg = getParamNumber(2,context)->toSequence(context);
        const XMLCh* collName = collArg.first()->asString(context);
        try {
            context->getItemFactory()->createAnyURI(collName, context);
        } catch(XPath2ErrorException &e) {
            XQThrow(FunctionException, X("FunctionMin::collapseTreeInternal"), X("Invalid collationURI"));  
        }
        collation=context->getCollation(collName, this);
    }
    else
        collation=context->getDefaultCollation(this);

    Sequence::iterator i = sequence.begin();
    AnyAtomicType::Ptr minItem = (const AnyAtomicType *)i->get();
    ++i;
    // if we have just one item, force entering the 'for' loop, or we will not test if the type had a total order
    if(i == sequence.end()) --i;
    for (; i != sequence.end(); ++i) {
        const AnyAtomicType *atomic = (const AnyAtomicType *)i->get();
        try {
          if(LessThan::less_than(atomic, minItem, collation, context, this))
            minItem = atomic;
        }
        catch (IllegalArgumentException &e) {
            XQThrow(IllegalArgumentException, X("FunctionMin::collapseTreeInternal"),
                    X("Invalid argument to fn:min() function [err:FORG0006]."));
        }
        catch (XPath2ErrorException &e) {
            XQThrow(IllegalArgumentException, X("FunctionMin::collapseTreeInternal"),
                    X("Invalid argument to fn:min() function [err:FORG0006]."));
        }
    }
    return Sequence(minItem, memMgr);
}
