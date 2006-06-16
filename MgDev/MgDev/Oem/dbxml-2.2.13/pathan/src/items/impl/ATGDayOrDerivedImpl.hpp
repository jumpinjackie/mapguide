/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGDAYORDERIVEDIMPL_HPP
#define _ATGDAYORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATGDayOrDerived.hpp>

#include <pathan/Pathan.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Timezone.hpp>

class PATHAN_EXPORT ATGDayOrDerivedImpl : public ATGDayOrDerived 
{

public:

  /* constructor */
  ATGDayOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

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

  /* returns true if the two objects are equal
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;
  
  /** Returns true if this is greater than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool greaterThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if this is less than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool lessThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const;

  /** Sets the timezone to the given timezone.*/
  virtual ATGDayOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;
  
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

private:

  /* parse the gDay */
  void setGDay(const XMLCh* const value, const DynamicContext* context);
  
  /*The value of this gDay*/
  ATDecimalOrDerived::Ptr _gDay;  // as nonNegativeInteger

  /* whether this gDay has a timezone value*/
  bool _hasTimezone;

  /* the timezone value, if it exist */
  Timezone::Ptr timezone_;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATGDAYORDERIVEDIMPL_HPP
