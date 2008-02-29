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
 * $Id: Plus.cpp,v 1.13 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>

/*static*/ const XMLCh Plus::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Plus::Plus(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ArithmeticOperator(name, args, memMgr)
{
  // Nothing to do
}

void Plus::calculateStaticType()
{
  const StaticType &arg0 = _args[0]->getStaticResolutionContext().getStaticType();
  const StaticType &arg1 = _args[1]->getStaticResolutionContext().getStaticType();

  calculateStaticTypeForNumerics(arg0, arg1);

  // Adding a duration to a date, dateTime, time, or duration
  if(arg1.containsType(StaticType::DAY_TIME_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg0.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::TIME_TYPE|
                                                StaticType::DAY_TIME_DURATION_TYPE);
  }
  if(arg1.containsType(StaticType::YEAR_MONTH_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg0.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
  }
  if(arg0.containsType(StaticType::DAY_TIME_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg1.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::TIME_TYPE|
                                                StaticType::DAY_TIME_DURATION_TYPE);
  }
  if(arg0.containsType(StaticType::YEAR_MONTH_DURATION_TYPE)) {
    _src.getStaticType().flags |= arg1.flags & (StaticType::DATE_TYPE|StaticType::DATE_TIME_TYPE|StaticType::YEAR_MONTH_DURATION_TYPE);
  }
}

Item::Ptr Plus::execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const
{
  return plus(atom1, atom2, context, this);
}

Item::Ptr Plus::plus(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context, const LocationInfo *info)
{
  try {
    if(atom1 == NULLRCP || atom2 == NULLRCP) return 0;

    if(atom1->isNumericValue()) {
      if(atom2->isNumericValue()) {
        return (const Item::Ptr)((Numeric*)(const AnyAtomicType*)atom1)->add((const Numeric::Ptr )atom2, context);
      }
      else {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An attempt to add a non numeric type to a numeric type has occurred [err:XPTY0004]"));
      }
    }

    switch(atom1->getPrimitiveTypeIndex()) {
    case AnyAtomicType::DATE : {
      switch(atom2->getPrimitiveTypeIndex()) {
      case AnyAtomicType::DAY_TIME_DURATION: {
        return (const Item::Ptr)((ATDateOrDerived*)atom1.get())->addDayTimeDuration((const ATDurationOrDerived*)atom2.get(), context);
      }
      case AnyAtomicType::YEAR_MONTH_DURATION: {
        return (const Item::Ptr)((ATDateOrDerived*)atom1.get())->addYearMonthDuration((const ATDurationOrDerived*)atom2.get(), context);
      }
      default: {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:date type has occurred [err:XPTY0004]"));
      }
      }
    }
    case AnyAtomicType::TIME : {
      switch(atom2->getPrimitiveTypeIndex()) {
      case AnyAtomicType::DAY_TIME_DURATION: {
        return (const Item::Ptr)((ATTimeOrDerived*)(const AnyAtomicType*)atom1)->addDayTimeDuration( (const ATDurationOrDerived::Ptr )atom2, context );
      }
      default: {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:time type has occurred [err:XPTY0004]"));
      }
      }
    }
    case AnyAtomicType::DATE_TIME : {
      switch(atom2->getPrimitiveTypeIndex()) {
      case AnyAtomicType::DAY_TIME_DURATION: {
        return (const Item::Ptr)((ATDateTimeOrDerived*)atom1.get())->addDayTimeDuration((const ATDurationOrDerived*)atom2.get(), context);
      }
      case AnyAtomicType::YEAR_MONTH_DURATION: {
        return (const Item::Ptr)((ATDateTimeOrDerived*)atom1.get())->addYearMonthDuration((const ATDurationOrDerived*)atom2.get(), context);
      }
      default: {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xs:dateTime type has occurred [err:XPTY0004]"));
      }
      }
    }
    case AnyAtomicType::DAY_TIME_DURATION: {
      switch(atom2->getPrimitiveTypeIndex()) {
      case AnyAtomicType::DAY_TIME_DURATION: {
        return (const Item::Ptr)((ATDurationOrDerived*)atom2.get())->add((const ATDurationOrDerived *)atom1.get(), context);
      }
      case AnyAtomicType::DATE: {
        return (const Item::Ptr)((ATDateOrDerived*)atom2.get())->addDayTimeDuration((const ATDurationOrDerived *)atom1.get(), context);
      }
      case AnyAtomicType::DATE_TIME: {
        return (const Item::Ptr)((ATDateTimeOrDerived*)atom2.get())->addDayTimeDuration((const ATDurationOrDerived *)atom1.get(), context);
      }
      case AnyAtomicType::TIME: {
        return (const Item::Ptr)((ATTimeOrDerived*)atom2.get())->addDayTimeDuration((const ATDurationOrDerived *)atom1.get(), context);
      }
      default: {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xdt:dayTimeDuration type has occurred [err:XPTY0004]"));
      }
      }
    }
    case AnyAtomicType::YEAR_MONTH_DURATION: {
      switch(atom2->getPrimitiveTypeIndex()) {
      case AnyAtomicType::YEAR_MONTH_DURATION: {
        return (const Item::Ptr)((ATDurationOrDerived*)atom2.get())->add((const ATDurationOrDerived *)atom1.get(), context);
      }
      case AnyAtomicType::DATE: {
        return (const Item::Ptr)((ATDateOrDerived*)atom2.get())->addYearMonthDuration((const ATDurationOrDerived *)atom1.get(), context);
      }
      case AnyAtomicType::DATE_TIME: {
        return (const Item::Ptr)((ATDateTimeOrDerived*)atom2.get())->addYearMonthDuration((const ATDurationOrDerived *)atom1.get(), context);
      }
      default: {
        XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("An invalid attempt to add to xdt:yearMonthDuration type has occurred [err:XPTY0004]"));
      }
      }
    }
    default: {
      XQThrow2(XPath2ErrorException,X("Plus::collapseTreeInternal"), X("The operator add ('+') has been called on invalid operand types [err:XPTY0004]"));
    }
    }
  }
  catch(XQException &e) {
      if(e.getXQueryLine() == 0)
        e.setXQueryPosition(info);
      throw;
  }
}

