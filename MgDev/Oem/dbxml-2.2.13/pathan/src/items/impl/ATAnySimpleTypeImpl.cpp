/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATAnySimpleTypeImpl.hpp"
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/internal/DOMutils/XStr.hpp> // defines X() and XMLCh*
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/util/XMLString.hpp>

// We ignore the type URI and typeName passed in, since they are always xdt:anySimpleType
// (you cannot derived from it)
ATAnySimpleTypeImpl::
ATAnySimpleTypeImpl(const XMLCh*, const XMLCh*, const XMLCh* value, const StaticContext* context)
{    
  _value = context->getMemoryManager()->getPooledString(value);
}

void *ATAnySimpleTypeImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATAnySimpleTypeImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATAnySimpleTypeImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYSIMPLETYPE;
}

/* Get the name of this type  (ie "integer" for xs:integer)-- 
 * same as the primitive type name. AnySimpleType cannot be 
 * extended */
const XMLCh* ATAnySimpleTypeImpl::getTypeName() const {
  return getPrimitiveTypeName();
}

/* Get the namespace URI for this type -- same as the primitive uri
 * AnySimpleType cannot be extended */
const XMLCh* ATAnySimpleTypeImpl::getTypeURI() const {
  return getPrimitiveTypeURI(); 
}

AnyAtomicType::AtomicObjectType ATAnySimpleTypeImpl::getTypeIndex() {
  return AnyAtomicType::ANY_SIMPLE_TYPE;
} 

AnyAtomicType::Ptr ATAnySimpleTypeImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
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
const XMLCh* ATAnySimpleTypeImpl::asString(const DynamicContext* context) const {
  return _value;
}

/* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
bool ATAnySimpleTypeImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATAnySimpleTypeImpl::equals"), X("Equality operator for given types not supported"));
  }
  return XPath2Utils::equals(target->asString(context), _value);  
}

AnyAtomicType::AtomicObjectType ATAnySimpleTypeImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
