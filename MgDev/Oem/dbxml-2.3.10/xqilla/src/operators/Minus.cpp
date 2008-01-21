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
 * $Id: Minus.cpp,v 1.10 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/context/DynamicContext.hpp>

/*static*/ const XMLCh Minus::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Minus::Minus(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
}

void Minus::calculateStaticType()
{
  const StaticType &arg0 = _args[0]->getStaticResolutionContext().getStaticType();
  const StaticType &arg1 = _args[1]->getStaticResolutionContext().getStaticType();

  calculateStaticTypeForNumerics(arg0, arg1);

  // Subtracting a duration from a date, dateTime, time, or duration
  if(arg1.containsType(StaticType::DAY_TIME_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg0.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::TIME_TYPE|
                                                StaticType::DAY_TIME_DURATION_TYPE);
  }
  if(arg1.containsType(StaticType::YEAR_MONTH_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg0.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
  }

  // Subtracting date, dateTime and time
  if(arg0.containsType(StaticType::DATE_TYPE) && arg1.containsType(StaticType::DATE_TYPE)) {
    _src.getStaticType().flags |= StaticType::DAY_TIME_DURATION_TYPE;
  }
  if(arg0.containsType(StaticType::DATE_TIME_TYPE) && arg1.containsType(StaticType::DATE_TIME_TYPE)) {
    _src.getStaticType().flags |= StaticType::DAY_TIME_DURATION_TYPE;
  }
  if(arg0.containsType(StaticType::TIME_TYPE) && arg1.containsType(StaticType::TIME_TYPE)) {
    _src.getStaticType().flags |= StaticType::DAY_TIME_DURATION_TYPE;
  }
}

Item::Ptr Minus::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;

  if(atom1->isNumericValue()) {
    if(atom2->isNumericValue()) {
      return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->subtract((const Numeric::Ptr )atom2, context);
    }
    else {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An attempt to subtract a non numeric type from a numeric type has occurred [err:XPTY0004]"));
    }
  }
  
  switch(atom1->getPrimitiveTypeIndex()) {
  case AnyAtomicType::DATE : {
    switch(atom2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DAY_TIME_DURATION: {
      return (const Item::Ptr)((ATDateOrDerived*)atom1.get())->subtractDayTimeDuration((const ATDurationOrDerived *)atom2.get(), context);
    }
    case AnyAtomicType::YEAR_MONTH_DURATION: {
      return (const Item::Ptr)((ATDateOrDerived*)atom1.get())->subtractYearMonthDuration((const ATDurationOrDerived *)atom2.get(), context);
    }
    case AnyAtomicType::DATE: {
      return (const Item::Ptr)((ATDateOrDerived*)atom1.get())->subtractDate((const ATDateOrDerived *)atom2.get(), context);
    }
    default: {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An invalid attempt to subtract from xs:date type has occurred [err:XPTY0004]"));
    }
    }
  }
  case AnyAtomicType::TIME : {
    switch(atom2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DAY_TIME_DURATION : {
      return (const Item::Ptr)((ATTimeOrDerived*)atom1.get())->subtractDayTimeDuration((const ATDurationOrDerived *)atom2.get(), context );
    }
    case AnyAtomicType::TIME : {
      return (const Item::Ptr)((ATTimeOrDerived*)atom1.get())->subtractTime((const ATTimeOrDerived *)atom2.get(), context);
    }
    default: {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An invalid attempt to subtract from xs:time type has occurred [err:XPTY0004]"));
    }
    }
  }
  case AnyAtomicType::DATE_TIME : {
    switch(atom2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DAY_TIME_DURATION: {
      return (const Item::Ptr)((ATDateTimeOrDerived*)atom1.get())->subtractDayTimeDuration((const ATDurationOrDerived*)atom2.get(), context);
    }
    case AnyAtomicType::YEAR_MONTH_DURATION: {
      return (const Item::Ptr)((ATDateTimeOrDerived*)atom1.get())->subtractYearMonthDuration((const ATDurationOrDerived*)atom2.get(), context);
    }
    case AnyAtomicType::DATE_TIME : {
      return (const Item::Ptr)((ATDateTimeOrDerived*)(const AnyAtomicType*)atom1)->subtractDateTimeAsDayTimeDuration((const ATDateTimeOrDerived::Ptr )atom2, context);
    }
    default: {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An invalid attempt to subtract from xs:dateTime type has occurred [err:XPTY0004]"));
    }
    }
  }
  case AnyAtomicType::DAY_TIME_DURATION: {
    switch(atom2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DAY_TIME_DURATION: {
      return (const Item::Ptr)((ATDurationOrDerived*)atom1.get())->subtract((const ATDurationOrDerived *)atom2.get(), context);
    }
    default: {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An invalid attempt to subtract from xdt:dayTimeDuration type has occurred [err:XPTY0004]"));
    }
    }
  }
  case AnyAtomicType::YEAR_MONTH_DURATION: {
    switch(atom2->getPrimitiveTypeIndex()) {
    case AnyAtomicType::YEAR_MONTH_DURATION: {
      return (const Item::Ptr)((ATDurationOrDerived*)atom1.get())->subtract((const ATDurationOrDerived *)atom2.get(), context);
    }
    default: {
      XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("An invalid attempt to subtract from xdt:yearMonthDuration type has occurred [err:XPTY0004]"));
    }
    }
  }
  default: {
    XQThrow(XPath2ErrorException,X("Minus::collapseTreeInternal"), X("The operator subtract ('-') has been called on invalid operand types [err:XPTY0004]"));
  }
  }
}

