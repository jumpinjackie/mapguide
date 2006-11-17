/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDURATIONORDERIVEDIMPL_HPP
#define _ATDURATIONORDERIVEDIMPL_HPP

#include <pathan/ATDurationOrDerived.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include <pathan/Pathan.hpp>

class AnyAtomicType;
class ATDecimalOrDerived;
class XPath2MemoryManager;
class MAPM;

class PATHAN_EXPORT ATDurationOrDerivedImpl : public ATDurationOrDerived 
{

public:

  /* constructor */
  ATDurationOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual void *getInterface(const XMLCh *name) const;

  /* Get the name of the primitive type (basic type) of this type 
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const;

  /* returns true if this duration is an instance of a xdt:dayTimeDuration */
  virtual bool isDayTimeDuration() const;

  /* returns true if this duration is an instance of a xdt:yearMonthDuration */
  virtual bool isYearMonthDuration() const;
  
  /* returns true if the two objects have the same duration
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;

  /** greaterThan -- only defined for fn:dayTimeDuration and fn:yearMonthDuration */
  virtual bool greaterThan(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const;

  /** lessThan -- only defined for fn:dayTimeDuration and fn:yearMonthDuration */
  virtual bool lessThan(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Divide this duration by a number -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr divide(const Numeric::Ptr &divisor, const DynamicContext* context) const;

  /** Divide this duration by a duration -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDecimalOrDerived::Ptr divide(const ATDurationOrDerived::Ptr &divisor, const DynamicContext* context) const;

  /** Multiply this duration by a number -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr multiply(const Numeric::Ptr &multiplier, const DynamicContext* context) const;

  /** Add a duration to this duration -- only available for xdt:dayTimeDuration 
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr add(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Subtract a duration from this duration -- only available for xdt:dayTimeDuration
   *  and xdt:yearMonthDuration */
  virtual ATDurationOrDerived::Ptr subtract(const ATDurationOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns the year portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getYears() const;

  /** Returns the month portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getMonths() const;

  /** Returns the days portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getDays() const;

  /** Returns the hours portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getHours() const;

  /** Returns the minutes portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getMinutes() const;

  /** Returns the seconds portion of this duration */
  virtual const ATDecimalOrDerived::Ptr &getSeconds() const;

  /** normalize this duration (only available for xdt:dayTimeDuration and
   * xdt:yearMonthDuration 
   **/
  virtual ATDurationOrDerived::Ptr normalize(const DynamicContext* context) const;
  
  /** Returns true if this Duration is negative, false otherwise */
  virtual bool isNegative() const;

  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();
  
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

protected:
  
  /* If possible, cast this type to the target type */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

private:
  // private constructor
  ATDurationOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const ATDecimalOrDerived::Ptr &year,const ATDecimalOrDerived::Ptr &month,
                          const ATDecimalOrDerived::Ptr &day, const ATDecimalOrDerived::Ptr &hour, const ATDecimalOrDerived::Ptr &minute,
                          const ATDecimalOrDerived::Ptr &sec, bool isPositive, const DynamicContext* context);

  /** Compare two dayTimeDurations, return true if this < other */
  bool dayTimeLessThan(const ATDurationOrDerived::Ptr &dayTimeDuration, const DynamicContext* context) const;
  /** Compare two dayTimeDurations, return true if this > other */
  bool dayTimeGreaterThan(const ATDurationOrDerived::Ptr &dayTimeDuration, const DynamicContext* context) const;

  /** Compare two yearMonthDurations, return true if this < other */
  bool yearMonthLessThan(const ATDurationOrDerived::Ptr &yearMonthDuration, const DynamicContext* context) const;
  /** Compare two yearMonthDurations, return true if this > other */
  bool yearMonthGreaterThan(const ATDurationOrDerived::Ptr &yearMonthDuration, const DynamicContext* context) const;

  /* Divide a xdt:dayTimeDuration by a xs:decimal */
  ATDurationOrDerived::Ptr dayTimeDivide(const Numeric::Ptr &divisor, const DynamicContext* context) const;

  /* Divide a xdt:dayTimeDuration by a xdt:dayTimeDuration */
  ATDecimalOrDerived::Ptr dayTimeDivide(const ATDurationOrDerived::Ptr &divisor, const DynamicContext* context) const;

  /* Divide a xdt:yearMonthDuration by an xs:decimal */  
  ATDurationOrDerived::Ptr yearMonthDivide(const Numeric::Ptr &divisor, const DynamicContext* context) const;

  /* Divide a xdt:yearMonthDuration by an xdt:yearMonthDuration */  
  ATDecimalOrDerived::Ptr yearMonthDivide(const ATDurationOrDerived::Ptr &divisor, const DynamicContext* context) const;

  /* normalize xdt:dayTimeDurations */
  ATDurationOrDerived::Ptr normalizeDayTimeDuration(const DynamicContext* context) const;

  /* normalize xdt:yearMonthDurations */
  ATDurationOrDerived::Ptr normalizeYearMonthDuration(const DynamicContext* context) const;  

  /* return this duration in forms of seconds */
  ATDecimalOrDerived::Ptr asSeconds(const DynamicContext* context) const;

  /*returns a ATDurationOrDerived of value value*/
  ATDurationOrDerived::Ptr newDayTimeDuration(ATDecimalOrDerived::Ptr valueSeconds, const DynamicContext* context) const;

  /*returns a ATDurationOrDerived of value value*/
  ATDurationOrDerived::Ptr newYearMonthDuration(ATDecimalOrDerived::Ptr valueMonth, const DynamicContext* context) const;

  /*The values that make up this duration */
  bool _isPositive;
  ATDecimalOrDerived::Ptr _year;  // xs:nonNegativeIngeter
  ATDecimalOrDerived::Ptr _month; // xs:nonNegativeIngeter
  ATDecimalOrDerived::Ptr _day;   // xs:nonNegativeIngeter
  ATDecimalOrDerived::Ptr _hour;  // xs:nonNegativeIngeter
  ATDecimalOrDerived::Ptr _minute;// xs:nonNegativeIngeter
  ATDecimalOrDerived::Ptr _sec;   // xs:decimal

  enum DurationType { DAY_TIME_DURATION, YEAR_MONTH_DURATION, DURATION };
  DurationType durationType;
  
  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;

  void setDuration(const XMLCh* const value, const DynamicContext* context);
 
};

#endif // _ATDURATIONORDERIVEDIMPL_HPP
