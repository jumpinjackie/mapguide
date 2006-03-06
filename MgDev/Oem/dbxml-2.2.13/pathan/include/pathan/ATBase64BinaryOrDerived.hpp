/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBASE64BINARYORDERIVED_HPP
#define _ATBASE64BINARYORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/AnyAtomicType.hpp>

#include <pathan/Pathan.hpp>

class DynamicContext;

class PATHAN_EXPORT ATBase64BinaryOrDerived : public AnyAtomicType
{
public:
  /* Get the name of the primitive type (basic type) of this type (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects' base 64 binary representation
   *  are equal (string comparison) false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

protected: 
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATBASE64BINARYORDERIVED_HPP
