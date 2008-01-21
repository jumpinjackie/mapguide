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
 * $Id: ATDurationOrDerived.hpp,v 1.8 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATDURATIONORDERIVED_HPP
#define _ATDURATIONORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>

#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>

class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API ATDurationOrDerived : public AnyAtomicType
{
public:
  typedef RefCountPointer<const ATDurationOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type 
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if this duration is an instance of a xdt:dayTimeDuration */
  virtual bool isDayTimeDuration() const = 0;

  /* returns true if this duration is an instance of a xdt:yearMonthDuration */
  virtual bool isYearMonthDuration() const = 0;

  /* returns true if the two objects have the same duration
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATDurationOrDerived::Ptr &other, const DynamicContext *context) const = 0;
  
  /** Divide this duration by a number -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr divide(const Numeric::Ptr &divisor, const DynamicContext* context) const = 0;
  
  /** Divide this duration by a duration -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDecimalOrDerived::Ptr divide(const ATDurationOrDerived::Ptr &divisor, const DynamicContext* context) const = 0;

  /** Multiply this duration by a number -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr multiply(const Numeric::Ptr &divisor, const DynamicContext* context) const = 0;
  
  /** Add a duration to this duration -- only available for xdt:dayTimeDuration 
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr add(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const = 0;
  
  /** Subtract a duration from this duration -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr subtract(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns the year portion of this duration */
  virtual ATDecimalOrDerived::Ptr getYears(const DynamicContext* context) const = 0;

  /** Returns the month portion of this duration */
  virtual ATDecimalOrDerived::Ptr getMonths(const DynamicContext* context) const = 0;

  /** Returns the days portion of this duration */
  virtual ATDecimalOrDerived::Ptr getDays(const DynamicContext* context) const = 0;

  /** Returns the hours portion of this duration */
  virtual ATDecimalOrDerived::Ptr getHours(const DynamicContext* context) const = 0;

  /** Returns the minutes portion of this duration */
  virtual ATDecimalOrDerived::Ptr getMinutes(const DynamicContext* context) const = 0;

  /** Returns the seconds portion of this duration */
  virtual ATDecimalOrDerived::Ptr getSeconds(const DynamicContext* context) const = 0;

  /* return this duration in forms of seconds */
  virtual ATDecimalOrDerived::Ptr asSeconds(const DynamicContext* context) const = 0;

  /* return this duration in forms of months */
  virtual ATDecimalOrDerived::Ptr asMonths(const DynamicContext* context) const = 0;

  /** Returns true if this Duration is negative, false otherwise */
  virtual bool isNegative() const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
  
  static const XMLCh fgDT_DAYTIMEDURATION[];
  static const XMLCh fgDT_DAYTIMEDURATION_XERCESHASH[];
  static const XMLCh pattern_DT_DAYTIMEDURATION[];
 
  static const XMLCh fgDT_YEARMONTHDURATION[];
  static const XMLCh fgDT_YEARMONTHDURATION_XERCESHASH[];
  static const XMLCh pattern_DT_YEARMONTHDURATION[];
};

#endif //  _ATDURATIONORDERIVED_HPP


