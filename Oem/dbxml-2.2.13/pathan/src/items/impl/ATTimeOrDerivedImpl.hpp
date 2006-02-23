/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATTIMEORDERIVEDIMPL_HPP
#define _ATTIMEORDERIVEDIMPL_HPP

#include <xercesc/util/XMLString.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/internal/m_apm.h>

#include <pathan/Pathan.hpp>

class Timezone;
class ATDecimalOrDerived;
class ATDurationOrDerived;
class XPath2MemoryManager;
class DynamicContext;

class PATHAN_EXPORT ATTimeOrDerivedImpl : public ATTimeOrDerived 
{

public:

  /* constructor */
  ATTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

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

  /* returns true if the two objects represent the same time,
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;

  /**
   * Returns true if and only if this time is greater than the given time. 
   * The order relation on time values is the order relation on their 
   * starting instants.
   */
  virtual bool greaterThan(const ATTimeOrDerived::Ptr &time, const DynamicContext* context) const;

  /**
   * Returns true if and only if this time is less than the given time. 
   * The order relation on time values is the order relation on their
   * starting instants.
   */
  virtual bool lessThan(const ATTimeOrDerived::Ptr &time, const DynamicContext* context) const;

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
  virtual ATTimeOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;

  virtual ATTimeOrDerived::Ptr normalize(const DynamicContext* context) const;

  /**
   * Returns an ATTimeOrDerived with a timezone added to it
   */
  virtual ATTimeOrDerived::Ptr addTimezone(const ATDurationOrDerived::Ptr &timezone, const DynamicContext* context) const;
  
  /**
   * Returns a time with the given dayTimeDuration added to it
   */
  virtual ATTimeOrDerived::Ptr addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const;
  
  /**
   * Returns a time with the given dayTimeDuration subtracted from it
   */
  virtual ATTimeOrDerived::Ptr subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const;

  /**
   * Returns a dayTimeDuration corresponding to the difference between this
   * and the given ATTimeOrDerived*
   */
  virtual ATDurationOrDerived::Ptr subtractTime(const ATTimeOrDerived::Ptr &time, const DynamicContext* context) const;

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
  ATTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, 
                      const ATDecimalOrDerived::Ptr &hh, 
                      const ATDecimalOrDerived::Ptr &mm, const ATDecimalOrDerived::Ptr &ss,
                      const Timezone::Ptr &timezone, bool hasTimezone);

  void setTime(const XMLCh* const time, const DynamicContext* context);

  ATTimeOrDerived::Ptr addDayTimeDuration(MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const;

  ATTimeOrDerived::Ptr subtractDayTimeDuration(MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const;

  int asInt(MAPM num) const;
  
  /*The value of this time*/
  ATDecimalOrDerived::Ptr _hh;  // hours
  ATDecimalOrDerived::Ptr _mm;  // minutes
  ATDecimalOrDerived::Ptr _ss;  // seconds
  
  /* the timezone associated with this ATTimeOrDerived */
  Timezone::Ptr timezone_;
  
  /* true if the timezone is defined */
  bool _hasTimezone;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATTIMEORDERIVEDIMPL_HPP
