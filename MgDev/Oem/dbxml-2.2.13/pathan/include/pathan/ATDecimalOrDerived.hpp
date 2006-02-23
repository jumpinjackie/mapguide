/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDECIMALORDERIVED_HPP
#define _ATDECIMALORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/Pathan.hpp>
#include "Numeric.hpp"

class DynamicContext;
class MAPM;
class PATHAN_EXPORT ATDecimalOrDerived : public Numeric
{
public:
  typedef RefCountPointer<const ATDecimalOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;
  virtual const XMLCh* asString(const StaticContext* context) const = 0;
  
  /* returns an XMLCh* representation of this Numeric with precision signinficant digits */
  virtual const XMLCh* asString(int precision, const DynamicContext* context) const = 0;

  /* Promote this to the given type, if possible */
  virtual Numeric::Ptr promoteTypeIfApplicable(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const = 0;

  /* returns true if the two objects are equal (MAPM) false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns true if this is less than other, false otherwise */
  virtual bool lessThan(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns true if this is greater than other, false otherwise */
  virtual bool greaterThan(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns a Numeric object which is the sum of this and other */
  virtual Numeric::Ptr add(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns a Numeric object which is the difference of this and
   * other */
  virtual Numeric::Ptr subtract(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns a Numeric object which is the product of this and other */
  virtual Numeric::Ptr multiply(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns a Numeric object which is the quotient of this and other */
  virtual Numeric::Ptr divide(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns the arithmetic product of its operands as a Numeric */
  virtual Numeric::Ptr mod(const Numeric::Ptr &other, const DynamicContext* context) const = 0;

  /** Returns the floor of this Numeric */
  virtual Numeric::Ptr floor(const DynamicContext* context) const = 0;

  /** Returns the ceiling of this Numeric */
  virtual Numeric::Ptr ceiling(const DynamicContext* context) const = 0;

  /** Rounds this Numeric */
  virtual Numeric::Ptr round(const DynamicContext* context) const = 0;

  /** Rounds this Numeric to the given precision, and rounds a half to even */
  virtual Numeric::Ptr roundHalfToEven(const Numeric::Ptr &precision, const DynamicContext* context) const = 0;

  /** Returns the Additive inverse of this Numeric */
  virtual Numeric::Ptr invert(const DynamicContext* context) const = 0;

  /** Returns the absolute value of this Numeric */
  virtual Numeric::Ptr abs(const DynamicContext* context) const = 0;

  /** Does this Numeric have value 0? */
  virtual bool isZero() const = 0;

  /** Is this Numeric negative? */
  virtual bool isNegative() const = 0;

  /** Is this Numeric positive? */
  virtual bool isPositive() const = 0;

  /** Treat this decimal (must be integer) as a codepoint **/
  virtual XMLCh treatAsCodepoint(const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;

protected: 
  friend class ATDateOrDerivedImpl;
  friend class ATDateTimeOrDerivedImpl;
  friend class ATTimeOrDerivedImpl;
  friend class ATDurationOrDerivedImpl;
  friend class ATGYearMonthOrDerivedImpl;
  friend class ATGMonthOrDerivedImpl;
  friend class Sequence;
  virtual MAPM asMAPM() const = 0;  
};

#endif //  _ATDECIMALORDERIVED_HPP
