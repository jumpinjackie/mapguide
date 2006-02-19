/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/internal/items/impl/ATBooleanOrDerivedImpl.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/DOMutils/XStr.hpp> // defines X() and XMLCh*
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>

ATBooleanOrDerivedImpl::
ATBooleanOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  // canonical value "0"
  static const XMLCh zero[] = {XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
                               XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  
  if( XPath2Utils::equals(value, zero) || 
      XPath2Utils::equals(value, 
        XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_FALSE)) {
    _value = false;
  } else {
    _value = true;
  }  
}

ATBooleanOrDerivedImpl::
ATBooleanOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, bool value, const StaticContext* context): 
    _value(value),
    _typeName(typeName),
    _typeURI(typeURI)
{ }
    
void *ATBooleanOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type 
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATBooleanOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATBooleanOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATBooleanOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATBooleanOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATBooleanOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::BOOLEAN;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATBooleanOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  // deviation from spec -- they say to convert to 1.0 and 0.0, but we don't see the point,
  // and it doesn't make casting to integer a special case (crioux)
  const XMLCh one[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, 
                        XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  const XMLCh zero[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
                         XERCES_CPP_NAMESPACE_QUALIFIER chNull };   
  const XMLCh* chValue = _value ? one : zero;

  switch(targetIndex) {
    case FLOAT: {
      return (const AnyAtomicType::Ptr)context->getPathanFactory()->createFloatOrDerived(targetURI, targetType, chValue, context);
    }
    case DECIMAL:  {
      return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDecimalOrDerived(targetURI, targetType, chValue, context);
    }
    case DOUBLE: {
      return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDoubleOrDerived(targetURI, targetType, chValue, context);
    }
    default: {
      return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
    }
  }
  
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATBooleanOrDerivedImpl::asString(const DynamicContext* context) const {
  return asString();
}

const XMLCh* ATBooleanOrDerivedImpl::asString() const
{
  if(_value) {
    return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_TRUE;
  } else {
    return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgATTVAL_FALSE;
  }
}

/* returns true if the two objects have the same boolean value
 * false otherwise */
bool ATBooleanOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATBooleanOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  return _value == ((const ATBooleanOrDerived::Ptr)target)->isTrue();
}

/* returns true if 'this' is true and 'other' is false, otherwise
 * returns false */
bool ATBooleanOrDerivedImpl::greaterThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const {
  if(this->isTrue() && ((const ATBooleanOrDerived*)other)->isFalse()) {
    return true;
  }
  return false;
}


/* returns true if 'other' is true and 'this' is false, otherwise
 * returns false */
bool ATBooleanOrDerivedImpl::lessThan(const ATBooleanOrDerived::Ptr &other, const DynamicContext* context) const {
  if(this->isFalse() && ((const ATBooleanOrDerived*)other)->isTrue()) {
    return true;
  }
  return false;
}

/* returns true if boolean value evaluates to true
 * false otherwise */
bool ATBooleanOrDerivedImpl::isTrue() const {
  return _value;
}

/* returns false if boolean value evaluates to true
 * true otherwise */
bool ATBooleanOrDerivedImpl::isFalse() const {
  return !isTrue();
}

AnyAtomicType::AtomicObjectType ATBooleanOrDerivedImpl::getPrimitiveTypeIndex() const {
  return getTypeIndex();
}
