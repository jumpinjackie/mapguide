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
 * $Id: ATTimeOrDerived.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATTIMEORDERIVED_HPP
#define _ATTIMEORDERIVED_HPP

#include <xercesc/util/XMLString.hpp>
#include <xqilla/items/DateOrTimeType.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/Timezone.hpp>

class XQILLA_API ATTimeOrDerived : public DateOrTimeType
{
public:
  typedef RefCountPointer<const ATTimeOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (lexical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects represent the same time,
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATTimeOrDerived::Ptr &other, const DynamicContext *context) const = 0;
 
  /** 
   * Returns an integer representing the hour component of this object
   */
  virtual ATDecimalOrDerived::Ptr getHours(const DynamicContext *context) const = 0;

  /** 
   * Returns an integer representing the minute component of this object
   */
  virtual ATDecimalOrDerived::Ptr getMinutes(const DynamicContext *context) const = 0;

  /** 
   * Returns an decimal representing the second component of this object
   */
  virtual ATDecimalOrDerived::Ptr getSeconds(const DynamicContext *context) const = 0;

  /** 
   * Returns a timezone object representing the timezone component of this object
   */
  virtual const Timezone::Ptr &getTimezone() const = 0;

  /**
   * Returns true if the timezone is defined for this object, false otherwise.
   */
  virtual bool hasTimezone() const = 0;

  /**
   * Setter for timezone.  Overrides the current timezone. (Not to be 
   * confused with addTimezone().
   */
  virtual ATTimeOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const = 0;

  /**
   * Returns an ATTimeOrDerived with a timezone added to it
   */
  virtual ATTimeOrDerived::Ptr addTimezone(const ATDurationOrDerived::Ptr &timezone,
                                           const DynamicContext* context) const = 0;

  /**
   * Returns a time with the given dayTimeDuration added to it
   */
  virtual ATTimeOrDerived::Ptr addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime,
                                                  const DynamicContext* context) const = 0;
  
  /**
   * Returns a time with the given dayTimeDuration subtracted from it
   */
  virtual ATTimeOrDerived::Ptr subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime,
                                                       const DynamicContext* context) const = 0;

  /**
   * Returns a dayTimeDuration corresponding to the difference between this
   * and the given ATTimeOrDerived*
   */
  virtual ATDurationOrDerived::Ptr subtractTime(const ATTimeOrDerived::Ptr &time, const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATTIMEORDERIVED_HPP
