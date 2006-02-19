/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBASE64BINARYORDERIVEDIMPL_HPP
#define _ATBASE64BINARYORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/ATBase64BinaryOrDerived.hpp>

#include <pathan/Pathan.hpp>

class XPath2MemoryManager;
class AnyAtomicType;

class PATHAN_EXPORT ATBase64BinaryOrDerivedImpl : public ATBase64BinaryOrDerived 
{

public:

  /* constructor */
  ATBase64BinaryOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context);

  virtual void *getInterface(const XMLCh *name) const;

  /* Get the name of the primitive type (basic type) of this type (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const;

  /* returns true if the two objects' base 64 binary representation
   *  are equal (string comparison) false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;
  
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;
  
protected:
  
  /* If possible, cast this type to the target type */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

private:

  /*The value of this base 64 value*/
  const XMLCh* _base64Data;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATBASE64BINARYORDERIVEDIMPL_HPP
