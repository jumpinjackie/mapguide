/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBOOLEANORDERIVED_HPP
#define _ATBOOLEANORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/AnyAtomicType.hpp>

#include <pathan/Pathan.hpp>

class XPath2MemoryManager;

class PATHAN_EXPORT ATBooleanOrDerived : public AnyAtomicType
{
public:
  typedef RefCountPointer<const ATBooleanOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type 
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects have the same boolean value
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /* returns true if 'this' is true and 'other' is false, otherwise
   * returns false */
  virtual bool greaterThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const = 0;


  /* returns true if 'other' is true and 'this' is false, otherwise
   * returns false */
  virtual bool lessThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const = 0;
  
  /* returns true if boolean value evaluates to true
   * false otherwise */
  virtual bool isTrue() const = 0;

  /* returns false if boolean value evaluates to true
   * true otherwise */
  virtual bool isFalse() const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATBOOLEANORDERIVED_HPP
