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
 * $Id: Multiply.cpp,v 1.11 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

/*static*/ const XMLCh Multiply::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Multiply::Multiply(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
  // Nothing to do
}

void Multiply::calculateStaticType()
{
  const StaticType &arg0 = _args[0]->getStaticResolutionContext().getStaticType();
  const StaticType &arg1 = _args[1]->getStaticResolutionContext().getStaticType();

  calculateStaticTypeForNumerics(arg0, arg1);

  // Multiplying a duration by a number
  if(arg0.containsType(StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE) && arg1.containsType(StaticType::NUMERIC_TYPE))
    _src.getStaticType().flags |= arg0.flags & (StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
  if(arg0.containsType(StaticType::NUMERIC_TYPE) && arg1.containsType(StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE))
    _src.getStaticType().flags |= arg1.flags & (StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
}

Item::Ptr Multiply::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;

  // xs:double * xs:duration (only xdt:dayTimeDuration and xdt:yearMonthDuration)
  if(atom1->isNumericValue() &&
     (atom2->getPrimitiveTypeIndex() == AnyAtomicType::DURATION ||
      atom2->getPrimitiveTypeIndex() == AnyAtomicType::DAY_TIME_DURATION ||
      atom2->getPrimitiveTypeIndex() == AnyAtomicType::YEAR_MONTH_DURATION)) {
      return (const Item::Ptr)((const ATDurationOrDerived*)atom2.get())->multiply((const Numeric::Ptr)atom1, context);
  }
  // xs:duration * xs:double (only xdt:dayTimeDuration and xdt:yearMonthDuration)
  if(atom2->isNumericValue() &&
     (atom1->getPrimitiveTypeIndex() == AnyAtomicType::DURATION ||
      atom1->getPrimitiveTypeIndex() == AnyAtomicType::DAY_TIME_DURATION ||
      atom1->getPrimitiveTypeIndex() == AnyAtomicType::YEAR_MONTH_DURATION)) {
    return (const Item::Ptr)((const ATDurationOrDerived*)atom1.get())->multiply((const Numeric::Ptr)atom2, context);
  }

  // numeric * numeric
  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->multiply((const Numeric::Ptr )atom2, context);
    }
    else {
      XQThrow(XPath2ErrorException,X("Multiply::collapseTreeInternal"), X("An attempt to multiply a non numeric type to a numeric type has occurred [err:XPTY0004]"));
    }
  }

  XQThrow(XPath2ErrorException,X("Multiply::collapseTreeInternal"), X("The operator * has been called on invalid operand types [err:XPTY0004]"));

}

