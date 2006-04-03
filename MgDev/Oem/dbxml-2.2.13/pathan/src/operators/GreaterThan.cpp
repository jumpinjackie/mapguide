/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/operators/GreaterThan.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/Numeric.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/Collation.hpp>
#include <pathan/internal/collations/CodepointCollation.hpp>

/*static*/ const XMLCh GreaterThan::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

GreaterThan::GreaterThan(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ComparisonOperator(name, args, memMgr)
{
}

/*static*/ bool GreaterThan::greater_than(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, Collation* collation, DynamicContext* context)
{
  // take care of Numeric types first
  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return ((Numeric*)(const AnyAtomicType*)atom1)->greaterThan((const Numeric::Ptr )atom2, context);
    } else {
      DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a numeric type to a non numeric type has occurred"));
    }
  }

  switch(atom1->getPrimitiveTypeIndex()) {
    case AnyAtomicType::BOOLEAN:
    {
      // op:boolean-greater-than(A, B)
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::BOOLEAN) 
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a boolean type to a non boolean type has occurred"));
      return ((const ATBooleanOrDerived*)(const AnyAtomicType*)atom1)->greaterThan((const ATBooleanOrDerived*)(const AnyAtomicType*)atom2, context);
    }
    case AnyAtomicType::STRING:
    {
      // use function compare
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::STRING)
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a string type to a non string type has occurred"));
      // if the function returns 1, then atom1 is greater
      return collation->compare(atom1->asString(context),atom2->asString(context))>0;
    }
    case AnyAtomicType::DATE:
    {
      // op:date-greater-than(A, B)
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::DATE)
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a date type to a non date type has occurred"));
      return ((ATDateOrDerived*)(const AnyAtomicType*)atom1)->greaterThan((const ATDateOrDerived::Ptr )atom2, context);
    }
    case AnyAtomicType::TIME:
    {
      // op:time-greater-than(A, B)
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::TIME) 
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a time type to a non time type has occurred"));
      return ((ATTimeOrDerived*)(const AnyAtomicType*)atom1)->greaterThan((const ATTimeOrDerived::Ptr )atom2, context);
    }
    case AnyAtomicType::DATE_TIME:
    {
      // op:datetime-greater-than(A, B)
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::DATE_TIME)
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a dateTime type to a non dateTime type has occurred"));
      return ((ATDateTimeOrDerived*)(const AnyAtomicType*)atom1)->greaterThan((const ATDateTimeOrDerived::Ptr )atom2, context);
    }
    case AnyAtomicType::DURATION:
    {
      if(atom2->getPrimitiveTypeIndex() != AnyAtomicType::DURATION) 
        DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("An attempt to compare a duration type to a non duration type has occurred"));
      return ((const ATDurationOrDerived*)(const AnyAtomicType*)atom1)->greaterThan((const ATDurationOrDerived*)(const AnyAtomicType*)atom2, context);
    }
    default:
      DSLthrow(XPath2ErrorException,X("GreaterThan::greater_than"), X("Unexpected data type in operator 'gt'"));
  }// switch
  DSLthrow(FunctionException,X("GreaterThan::greater_than"), X("An equality operator is not defined for the provided arguments"));
}

bool GreaterThan::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  Collation* collation=context->getDefaultCollation();
  if(collation==NULL)
     collation=context->getCollation(CodepointCollation::getCodepointCollationName());
  return greater_than(atom1, atom2, collation, context);
}

