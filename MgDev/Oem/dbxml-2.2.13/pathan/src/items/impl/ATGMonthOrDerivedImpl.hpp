/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGMONTHORDERIVEDIMPL_HPP
#define _ATGMONTHORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATGMonthOrDerived.hpp>

#include <pathan/Pathan.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Timezone.hpp>

class PATHAN_EXPORT ATGMonthOrDerivedImpl : public ATGMonthOrDerived 
{

public:

  /* constructor */
  ATGMonthOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

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
  virtual bool greaterThan(const ATGMonthOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if this is less than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool lessThan(const ATGMonthOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const;

  /** Sets the timezone to the given timezone.*/
  virtual ATGMonthOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;
  
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

private:

  /* parse the gDay */
  void setGMonth(const XMLCh* const value, const DynamicContext* context);
  
  int asInt(MAPM num) const;

  /*The value of this gDay*/
  ATDecimalOrDerived::Ptr _gMonth; // as nonNegativeInteger

  /* whether this gDay has a timezone value*/
  bool _hasTimezone;

  /* the timezone value, if it exist */
  Timezone::Ptr timezone_;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATGMONTHORDERIVEDIMPL_HPP
