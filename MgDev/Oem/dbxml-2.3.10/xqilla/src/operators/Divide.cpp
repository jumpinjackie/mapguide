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
 * $Id: Divide.cpp,v 1.10 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/context/DynamicContext.hpp>

/*static*/ const XMLCh Divide::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Divide::Divide(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
  // Nothing to do
}

void Divide::calculateStaticType()
{
  const StaticType &arg0 = _args[0]->getStaticResolutionContext().getStaticType();
  const StaticType &arg1 = _args[1]->getStaticResolutionContext().getStaticType();

  calculateStaticTypeForNumerics(arg0, arg1);

  // Dividing a duration by a number
  if(arg0.containsType(StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE)
     && arg1.containsType(StaticType::NUMERIC_TYPE)) {
    _src.getStaticType().flags |= arg0.flags & (StaticType::DAY_TIME_DURATION_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
  }

  // Dividing a duration by a duration
  if(arg0.containsType(StaticType::DAY_TIME_DURATION_TYPE) && arg1.containsType(StaticType::DAY_TIME_DURATION_TYPE)) {
    _src.getStaticType().flags |= StaticType::DECIMAL_TYPE;
  }
  if(arg0.containsType(StaticType::YEAR_MONTH_DURATION_TYPE) && arg1.containsType(StaticType::YEAR_MONTH_DURATION_TYPE)) {
    _src.getStaticType().flags |= StaticType::DECIMAL_TYPE;
  }
}

Item::Ptr Divide::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;
  
  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->divide((const Numeric::Ptr )atom2, context);
    }
    else {
      XQThrow(XPath2ErrorException,X("Divide::collapseTreeInternal"), X("An attempt to divide a numeric type by a non-numeric type has occurred [err:XPTY0004]"));
    }
  }

  if(atom1->getPrimitiveTypeIndex() == AnyAtomicType::DAY_TIME_DURATION ||
     atom1->getPrimitiveTypeIndex() == AnyAtomicType::YEAR_MONTH_DURATION) {
    const ATDurationOrDerived* duration = (const ATDurationOrDerived*)atom1.get();
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)duration->divide((const Numeric *)atom2.get(), context);
    }
    else if(atom2->getPrimitiveTypeIndex() == AnyAtomicType::DAY_TIME_DURATION ||
              atom2->getPrimitiveTypeIndex() == AnyAtomicType::YEAR_MONTH_DURATION) {
      return (const Item::Ptr)duration->divide((const ATDurationOrDerived*)atom2.get(), context);
    }
    else {
      XQThrow(XPath2ErrorException,X("Divide::collapseTreeInternal"), X("An attempt to divide an xs:duration by an invalid type has occured [err:XPTY0004]"));
    }
  }
  else {
    XQThrow(XPath2ErrorException,X("Divide::collapseTreeInternal"), X("The operator div has been called on invalid operand types [err:XPTY0004]"));
  }

  return 0;
}

