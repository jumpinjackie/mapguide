/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDECIMALORDERIVEDIMPL_HPP
#define _ATDECIMALORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/internal/m_apm.h>

#include <pathan/Pathan.hpp>

#define DECIMAL_MAX_DIGITS 50
class StaticContext;

class PATHAN_EXPORT ATDecimalOrDerivedImpl : public ATDecimalOrDerived 
{

public:

  /* constructor */
  ATDecimalOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context);

  /* constructor */
  ATDecimalOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const StaticContext* context);

  /* constructor */
  ATDecimalOrDerivedImpl(int value);

  /** destructor -- do nothing*/
  virtual ~ATDecimalOrDerivedImpl() { };
  
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
  virtual const XMLCh* asString(const StaticContext* context) const;

  /* returns an XMLCh* representation of this Numeric with precision signinficant digits */
  virtual const XMLCh* asString(int precision, const DynamicContext* context) const;

  /* Promote this to the given type, if possible */
  virtual Numeric::Ptr promoteTypeIfApplicable(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const;
  
  /* returns true if the two objects' decimal values are equal
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;

  /** Returns true if this is less than other, false otherwise */
  virtual bool lessThan(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns true if this is greater than other, false otherwise */
  virtual bool greaterThan(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns a Numeric object which is the sum of this and other */
  virtual Numeric::Ptr add(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns a Numeric object which is the difference of this and
   * other */
  virtual Numeric::Ptr subtract(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns a Numeric object which is the product of this and other */
  virtual Numeric::Ptr multiply(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns a Numeric object which is the quotient of this and other */
  virtual Numeric::Ptr divide(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns the arithmetic product of its operands as a Numeric */
  virtual Numeric::Ptr mod(const Numeric::Ptr &other, const DynamicContext* context) const;

  /** Returns the floor of this Numeric */
  virtual Numeric::Ptr floor(const DynamicContext* context) const;

  /** Returns the ceiling of this Numeric */
  virtual Numeric::Ptr ceiling(const DynamicContext* context) const;

  /** Rounds this Numeric */
  virtual Numeric::Ptr round(const DynamicContext* context) const;

  /** Rounds this Numeric to the given precision, and rounds a half to even */
  virtual Numeric::Ptr roundHalfToEven(const Numeric::Ptr &precision, const DynamicContext* context) const;
  
  /** Returns the Additive inverse of this Numeric */
  virtual Numeric::Ptr invert(const DynamicContext* context) const;

  /** Returns the absolute value of this Numeric */
  virtual Numeric::Ptr abs(const DynamicContext* context) const;

  /** Does this Numeric have value 0? */
  virtual bool isZero() const;

  /** Is this Numeric negative? */
  virtual bool isNegative() const;

  /** Is this Numeric positive? */
  virtual bool isPositive() const;

  /** Treat this decimal (must be integer) as a codepoint **/
  virtual XMLCh treatAsCodepoint(const DynamicContext* context) const;

  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();
  
  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

protected:
  //////////////////////////////////////
  // Horrible Hack to make Dates      //
  // work for now. Loss of Precision! //
  //////////////////////////////////////  
  virtual MAPM asMAPM() const;
  
  /* If possible, cast this type to the target type */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

private:

  /* set the value of this decimal */
  void setDecimal(const XMLCh* const value, const StaticContext *context);

  /* flag to note that we are an Integer -- to avoid many calls to
   * isInstanceOfType */
  bool _isInteger;

  /*The value of this decimal*/
  MAPM _decimal;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATDECIMALORDERIVEDIMPL_HPP
