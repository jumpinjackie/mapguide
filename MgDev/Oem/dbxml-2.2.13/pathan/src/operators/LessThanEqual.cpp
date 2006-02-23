/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/LessThanEqual.hpp>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>

/*static*/ const XMLCh LessThanEqual::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

LessThanEqual::LessThanEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ComparisonOperator(name, args, memMgr)
{
}

/*static*/ bool LessThanEqual::less_than_equal(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context)
{
    // A le B numeric               numeric                 op:numeric-less-than(A, B) or op:numeric-equal(A, B)
    // A le B xs:boolean            xs:boolean              fn:not(op:boolean-greater-than(A, B))
    // A le B xs:string             xs:string               op:numeric-less-than(fn:compare(A, B), 1)
    // A le B xs:date               xs:date                 fn:not(op:date-greater-than(A, B))
    // A le B xs:time               xs:time                 fn:not(op:time-greater-than(A, B))
    // A le B xs:dateTime           xs:dateTime             fn:not(op:datetime-greater-than(A, B))
    // A le B xdt:yearMonthDuration xdt:yearMonthDuration   fn:not(op:yearMonthDuration-greater-than(A, B))
    // A le B xdt:dayTimeDuration   xdt:dayTimeDuration     fn:not(op:dayTimeDuration-greater-than(A, B))
    // numeric values need a special comparison, for the others we can just rely on GreaterThan
    if(arg1->isNumericValue()) {
        if(arg2->isNumericValue()) {
            return ((Numeric*)(const AnyAtomicType*)arg1)->lessThan((const Numeric::Ptr )arg2, context) ||
                   ((Numeric*)(const AnyAtomicType*)arg1)->equals((const Numeric::Ptr )arg2, context);
        } else {
            DSLthrow(XPath2ErrorException,X("LessThanEqual::less_than_equal"), X("An attempt to compare a numeric type to a non numeric type has occurred"));
        }
    }

    return !GreaterThan::greater_than(arg1,arg2,collation,context);
}

bool LessThanEqual::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  Collation* collation=context->getDefaultCollation();
  if(collation==NULL)
     collation=context->getCollation(CodepointCollation::getCodepointCollationName());
  return less_than_equal(atom1, atom2, collation, context);
}
