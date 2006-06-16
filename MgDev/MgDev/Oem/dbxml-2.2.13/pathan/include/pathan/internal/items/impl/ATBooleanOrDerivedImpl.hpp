/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBOOLEANORDERIVEDIMPL_HPP
#define _ATBOOLEANORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATBooleanOrDerived.hpp>

#include <pathan/Pathan.hpp>

class DynamicContext;
class StaticContext;

class PATHAN_EXPORT ATBooleanOrDerivedImpl : public ATBooleanOrDerived 
{

public:

  /* constructor */
  ATBooleanOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context);
  
  /* constructor */
  ATBooleanOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, bool value, const StaticContext* context);

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
  virtual const XMLCh* asString() const;

  /* returns true if the two objects have the same boolean value
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;

  /* returns true if 'this' is true and 'other' is false, otherwise
   * returns false */
  virtual bool greaterThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const;


  /* returns true if 'other' is true and 'this' is false, otherwise
   * returns false */
  virtual bool lessThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const;


  /* returns true if boolean value evaluates to true
   * false otherwise */
  virtual bool isTrue() const;

  /* returns false if boolean value evaluates to true
   * true otherwise */
  virtual bool isFalse() const;

  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

protected:
  
  /* If possible, cast this type to the target type */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

private:

  /*The value of this boolean */
  bool _value;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATBOOLEANORDERIVEDIMPL_HPP
