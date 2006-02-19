/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDATETIMEORDERIVEDIMPL_HPP
#define _ATDATETIMEORDERIVEDIMPL_HPP

#include <xercesc/util/XMLString.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/internal/m_apm.h>

#include <pathan/Pathan.hpp>

class Timezone;
class ATDecimalOrDerived;
class ATDurationOrDerived;
class XPath2MemoryManager;
class DynamicContext;
class StaticContext;

class PATHAN_EXPORT ATDateTimeOrDerivedImpl : public ATDateTimeOrDerived 
{

public:

  /* constructor */
  ATDateTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

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

  /* returns true if the two objects represent the same date,
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;

  /**
   * Returns true if and only if this date is greater than the given date. 
   * The order relation on date values is the order relation on their 
   * starting instants.
   */
  virtual bool greaterThan(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const;

  /**
   * Returns true if and only if this date is less than the given date. 
   * The order relation on date values is the order relation on their
   * starting instants.
   */
  virtual bool lessThan(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const;

  /** 
   * Returns an integer representing the year component  of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getYears() const;

  /** 
   * Returns an integer representing the month component  of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getMonths() const;

  /** 
   * Returns an integer representing the day component  of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getDays() const;

  /** 
   * Returns an integer representing the hour component of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getHours() const;

  /** 
   * Returns an integer representing the minute component of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getMinutes() const;

  /** 
   * Returns an decimal representing the second component of this object
   */
  virtual const ATDecimalOrDerived::Ptr &getSeconds() const;

  /**
   * Returns the timezone associated with this object, or
   * null, if the timezone is not set
   */
  virtual const Timezone::Ptr &getTimezone() const;
  
  /**
   * Returns true if the timezone is defined for this object, false otherwise.
   */
  virtual bool hasTimezone() const;

  /**
   * Setter for timezone.  Overrides the current timezone. (Not to be 
   * confused with addTimezone().
   */
  virtual ATDateTimeOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;

  virtual ATDateTimeOrDerived::Ptr normalize(const DynamicContext* context) const;

  /**
   * Returns an ATDateTimeOrDerived with a timezone added to it
   */
  virtual ATDateTimeOrDerived::Ptr addTimezone(const ATDurationOrDerived::Ptr &timezone, const DynamicContext* context) const;
  
  /**
   * Returns a date with the given yearMonthDuration added to it
   */
  virtual ATDateTimeOrDerived::Ptr addYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth,  const DynamicContext* context) const;

  /**
   * Returns a date with the given dayTimeDuration added to it
   */
  virtual ATDateTimeOrDerived::Ptr addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const;
  
  /**
   * Returns a date with the given yearMonthDuration subtracted from it
   */
  virtual ATDateTimeOrDerived::Ptr subtractYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth, const DynamicContext* context) const;

  /**
   * Returns a date with the given dayTimeDuration subtracted from it
   */
  virtual ATDateTimeOrDerived::Ptr subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const;

  /**
   * Returns a dayTimeDuration corresponding to the difference between this
   * and the given ATDateTimeOrDerived*
   */
  virtual ATDurationOrDerived::Ptr subtractDateTimeAsDayTimeDuration(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const;

  /**
   * Returns a dayTimeDuration corresponding to the difference between this
   * and the given ATDateTimeOrDerived*
   */
  virtual ATDurationOrDerived::Ptr subtractDateTimeAsYearMonthDuration(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const;

  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 
  
  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();
  
  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

protected:
  
  /* If possible, cast this type to the target type */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

private:
  // private constructor for use with clone()
  ATDateTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const ATDecimalOrDerived::Ptr &YY, 
                      const ATDecimalOrDerived::Ptr &MM, const ATDecimalOrDerived::Ptr &DD, const ATDecimalOrDerived::Ptr &hh, 
                      const ATDecimalOrDerived::Ptr &mm, const ATDecimalOrDerived::Ptr &ss,
                      const Timezone::Ptr &timezone, bool hasTimezone);

  void setDateTime(const XMLCh* const date, const DynamicContext* context);

  ATDateTimeOrDerived::Ptr addDayTimeDuration(MAPM days, MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const;

  ATDateTimeOrDerived::Ptr subtractDayTimeDuration(MAPM days, MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const;

  ATDateTimeOrDerived::Ptr addYearMonthDuration(MAPM years, MAPM months, const DynamicContext* context) const;

  int asInt(MAPM num) const;
  
  /*The value of this date*/
  ATDecimalOrDerived::Ptr _YY;  // year as xs:integer
  ATDecimalOrDerived::Ptr _MM;  // month as xs:nonNegativeInteger
  ATDecimalOrDerived::Ptr _DD;  // day as xs:nonNegativeInteger
  
  ATDecimalOrDerived::Ptr _hh;  // hours as xs:nonNegativeInteger
  ATDecimalOrDerived::Ptr _mm;  // minutes as xs:nonNegativeInteger
  ATDecimalOrDerived::Ptr _ss;  // seconds xs:decimal
  
  /* the timezone associated with this ATDateTimeOrDerived */
  Timezone::Ptr timezone_;
  
  /* true if the timezone is defined */
  bool _hasTimezone;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATDATETIMEORDERIVEDIMPL_HPP
