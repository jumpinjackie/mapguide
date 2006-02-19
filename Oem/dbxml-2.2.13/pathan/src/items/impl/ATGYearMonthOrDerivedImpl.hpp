/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGYEARMONTHORDERIVEDIMPL_HPP
#define _ATGYEARMONTHORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATGYearMonthOrDerived.hpp>

#include <pathan/Pathan.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Timezone.hpp>

class DynamicContext;

class PATHAN_EXPORT ATGYearMonthOrDerivedImpl : public ATGYearMonthOrDerived 
{

public:

  /* constructor */
  ATGYearMonthOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

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
  virtual bool greaterThan(const ATGYearMonthOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if this is less than other.  Ignores timezones.
   * Returns false otherwise. */
  virtual bool lessThan(const ATGYearMonthOrDerived::Ptr &other, const DynamicContext* context) const;

  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const;

  /** Sets the timezone to the given timezone.*/
  virtual ATGYearMonthOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;
  
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

private:

  /* parse the gYearMonth */
  void setGYearMonth(const XMLCh* const value, const DynamicContext* context);
  
  int asInt(MAPM num) const;

  /*The values of this gYearMonth*/
  ATDecimalOrDerived::Ptr _MM;  // as nonNegativeInteger
  ATDecimalOrDerived::Ptr _YY;  // as integer

  /* whether this gYearMonth has a timezone value*/
  bool _hasTimezone;

  /* the timezone value, if it exist */
  Timezone::Ptr timezone_;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATGYEARMONTHORDERIVEDIMPL_HPP
