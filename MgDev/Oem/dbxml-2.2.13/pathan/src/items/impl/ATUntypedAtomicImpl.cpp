/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATUntypedAtomicImpl.hpp"
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/util/XMLString.hpp>

// We ignore the type URI and typeName passed in, since they are always xdt:untypedAtomic
// (you cannot derived from it)
ATUntypedAtomicImpl::
ATUntypedAtomicImpl(const XMLCh*, const XMLCh*, const XMLCh* value, const StaticContext* context): 
    ATUntypedAtomic() {
    
  _value = context->getMemoryManager()->getPooledString(value);
      
}

void *ATUntypedAtomicImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATUntypedAtomicImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATUntypedAtomicImpl::getPrimitiveName()  {
  return ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
}

/* Get the namespace uri of the primitive type (basic type) of this type */
const XMLCh* ATUntypedAtomicImpl::getPrimitiveTypeURI() const{
  return FunctionConstructor::XMLChXPath2DatatypesURI;
}

/* Get the name of this type  (ie "integer" for xs:integer)-- 
 * same as the primitive type name. UntypedAtomic cannot be 
 * extended */
const XMLCh* ATUntypedAtomicImpl::getTypeName() const {
  return getPrimitiveTypeName();
}

/* Get the namespace URI for this type -- same as the primitive uri
 * UntypedAtomic cannot be extended */
const XMLCh* ATUntypedAtomicImpl::getTypeURI() const {
  return getPrimitiveTypeURI(); 
}

AnyAtomicType::AtomicObjectType ATUntypedAtomicImpl::getTypeIndex() {
  return AnyAtomicType::UNTYPED_ATOMIC;
} 

AnyAtomicType::Ptr ATUntypedAtomicImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  switch (targetIndex) {
    case DOUBLE: {
      const XMLCh* upCase = XPath2Utils::toUpper(_value, context->getMemoryManager());
      return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDoubleOrDerived(targetURI, targetType, upCase, context);
    }
    case FLOAT: {
      const XMLCh* upCase = XPath2Utils::toUpper(_value, context->getMemoryManager());
      return (const AnyAtomicType::Ptr)context->getPathanFactory()->createFloatOrDerived(targetURI, targetType, upCase, context);
    }
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATUntypedAtomicImpl::asString(const DynamicContext* context) const {
  return _value;
}

/* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
bool ATUntypedAtomicImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATUntypedAtomicImpl::equals"), X("Equality operator for given types not supported"));
  }
  return XPath2Utils::equals(target->asString(context), _value);
}

AnyAtomicType::AtomicObjectType ATUntypedAtomicImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
