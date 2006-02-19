/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGDAYORDERIVED_HPP
#define _ATGDAYORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/Pathan.hpp>
#include "DateOrTimeType.hpp"
#include <pathan/Timezone.hpp>

class DynamicContext;

class PATHAN_EXPORT ATGDayOrDerived : public DateOrTimeType
{
public:
  typedef RefCountPointer<const ATGDayOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects are equal 
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns true if this is greater than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool greaterThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns true if this is less than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool lessThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const = 0;

  /** Sets the timezone to the given timezone.*/
  virtual ATGDayOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATGDAYORDERIVED_HPP
