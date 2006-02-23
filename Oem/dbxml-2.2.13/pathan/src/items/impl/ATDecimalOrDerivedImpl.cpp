/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/internal/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/PathanFactory.hpp>

ATDecimalOrDerivedImpl::
ATDecimalOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATDecimalOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  setDecimal(value, context);
  if(this->isInstanceOfType (XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context)) {
    _isInteger = true;
  } else {
    _isInteger = false;
  }
}

ATDecimalOrDerivedImpl::
ATDecimalOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const StaticContext* context): 
    ATDecimalOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  _decimal = value;
  if(this->isInstanceOfType (XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context)) {
    _isInteger = true;
  } else {
    _isInteger = false;
  }
}

ATDecimalOrDerivedImpl::
ATDecimalOrDerivedImpl(int value):
    ATDecimalOrDerived(),
    _typeName(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER),
    _typeURI(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA) { 

  _decimal = value;
  _isInteger = true;
}

void *ATDecimalOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATDecimalOrDerivedImpl::getPrimitiveTypeName() const {
  return getPrimitiveName();
}

const XMLCh* ATDecimalOrDerivedImpl::getPrimitiveName() {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATDecimalOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATDecimalOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATDecimalOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::DECIMAL;
} 

AnyAtomicType::Ptr ATDecimalOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {

  if(context->isTypeOrDerivedFromType(targetURI, targetType, 
                                      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER)) {
    char obuf[1024];
    _decimal.toIntegerString(obuf);
    return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDecimalOrDerived(targetURI, targetType, obuf, context);
  } else if (targetIndex == BOOLEAN) {
    if (this->isZero()) {
      return context->getPathanFactory()->createBooleanOrDerived(targetURI, targetType, false, context);
    } else {
      return context->getPathanFactory()->createBooleanOrDerived(targetURI, targetType, true, context);
    }
  } else {
    return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATDecimalOrDerivedImpl::asString(const DynamicContext* context) const {
  return asString((StaticContext*)context);
}

const XMLCh* ATDecimalOrDerivedImpl::asString(const StaticContext* context) const {
  char obuf[1024];
  if(_decimal.is_integer())
    _decimal.toIntegerString(obuf);
  else
    _decimal.toFixPtString(obuf, DECIMAL_MAX_DIGITS);

  // Note in the canonical representation the decimal point is required
  // and there must be at least one digit to the right and one digit to 
  // the left of the decimal point (which may be 0)
  if(strchr(obuf,'.')!=0)
  {
    // remove trailing 0's
    char* lastChar=obuf+strlen(obuf)-1;
    while(*lastChar=='0') {
      *lastChar--=0;
    }
    // remove decimal point, if there are no digits after it
    if(*lastChar=='.')
      *lastChar=0;
  }
  return context->getMemoryManager()->getPooledString(obuf);
}

 /* returns an XMLCh* representation of this Numeric with precision signinficant digits */
const XMLCh* ATDecimalOrDerivedImpl::asString(int precision, const DynamicContext* context) const {
  MAPM decimal = _decimal.abs();
  char obuf[1024];
  
  if(decimal.is_integer()) {
    decimal.toFixPtString(obuf, 0);  // no significant digits
    int length = strlen(obuf);
    if(length<precision) {  // pad with 0s
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
      if(this->isNegative()) {
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      } 
      for(int i = length; i<precision; i++) 
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
      buf.append(context->getMemoryManager()->getPooledString(obuf));
      return context->getMemoryManager()->getPooledString(buf.getRawBuffer());
    } else if (length>precision) { // chop off the end
      char* lastChar=obuf+length-1;
      for(int i = precision; i<length; i++) {
        *lastChar--=0;
      }
      if(this->isNegative()) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
        buf.append(context->getMemoryManager()->getPooledString(obuf));
        return context->getMemoryManager()->getPooledString(buf.getRawBuffer());
      }
    } else {
      if(this->isNegative()) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
        buf.append(context->getMemoryManager()->getPooledString(obuf));
        return context->getMemoryManager()->getPooledString(buf.getRawBuffer());      
      }
    }
  } else {
    // Don't support this for now, unless we see a need for it
    return this->asString(context);
    
    /*decimal.toFixPtString(obuf, precision);
    int digitsBeforeDot = 2; //strchr(obuf, '.'); // TODO: FIX THIS!  Find an index of for char*
    if(digitsBeforeDot > precision) { // chop off the end
      obuf[precision] = 0;
    } else if (digitsBeforeDot < precision) {  // then we want precision - digitsBeforeDot significant digits
      obuf[digitsBeforeDot+1+precision] = 0;  // +1 for the .
      if(this->isNegative()) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
        buf.append(context->getMemoryManager()->getPooledString(obuf));
        return context->getMemoryManager()->getPooledString(buf.getRawBuffer());
      }
    }*/
  }
  
  return context->getMemoryManager()->getPooledString(obuf);
  
}

/* Promote this to the given type, if possible */
Numeric::Ptr ATDecimalOrDerivedImpl::promoteTypeIfApplicable(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const {
  // if target is instance of xs:decimal, or if typeName == double or if typeName == float, cast
  if( this->isInstanceOfType(typeURI, typeName, context) ) {
    return this;  // no need to promote, we are already a decimal (or possibly anyAtomicType, anySimpleType, anyType)
  } else if( (XPath2Utils::equals(typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE) &&      
      XPath2Utils::equals(typeURI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) ||
     (XPath2Utils::equals(typeName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT) &&      
      XPath2Utils::equals(typeURI, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) ) {
    return (const Numeric::Ptr )this->castAs(typeURI, typeName, context);
  } else {
    return 0;
  }  
}

/* returns true if the two objects' value are equal
   * false otherwise */
bool ATDecimalOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(!target->isNumericValue()) {
    DSLthrow(IllegalArgumentException,X("ATDecimalOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    // if targer is not a decimal, then we need to promote this to a float or double
    return this->castAs(target->getPrimitiveTypeURI(), target->getPrimitiveTypeName(), context)->equals(target, context);
  } else {
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const AnyAtomicType*)target;
    return _decimal == otherImpl->_decimal;
  }
}

/** Returns true if this is less than other, false otherwise */
bool ATDecimalOrDerivedImpl::lessThan(const Numeric::Ptr &other, const DynamicContext* context) const { 
  if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->lessThan(other, context);
  } else {
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
    return _decimal < otherImpl->_decimal;
  }
}

/** Returns true if this is greater than other, false otherwise */
bool ATDecimalOrDerivedImpl::greaterThan(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->greaterThan(other, context);
  } else {
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
    return _decimal > otherImpl->_decimal;
  }
}

/** Returns a Numeric object which is the sum of this and other */
Numeric::Ptr ATDecimalOrDerivedImpl::add(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->isOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // if both are of the same type exactly, we can perform addition
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;

    // if integer, return xs:integer, otherwise xs:decimal
    if(_isInteger) {
      return context->getPathanFactory()->createInteger(_decimal + otherImpl->_decimal, context);
    }
    return context->getPathanFactory()->createDecimal(_decimal + otherImpl->_decimal, context);
  } else if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->add(other, context);
  } else if (this->isInstanceOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // here we know we have two decimals, and this is 'lower' in the hierarchy than other
    // so cast this to other's type
    return ((const Numeric::Ptr )this->castAs(other->getTypeURI(), other->getTypeName(), context))->add(other, context);
  } else if (other->isInstanceOfType(this->getTypeURI(), this->getTypeName(), context)) {
    // here we have two decimals, and this is 'higher' in the hierarchy than other
    // so cast other to this' type
    return this->add((const Numeric::Ptr )other->castAs(this->getTypeURI(), this->getTypeName(), context), context);
  } else {
    // we have two separate branches.  if either is instance of integer, cast it to integer, otherwise, cast to decimal
    // revisit: this is not the prettiest way to do it.  You would want to go up the tree one by one instead of
    // jumping to integer and decimal
    ATDecimalOrDerived::Ptr first;
    ATDecimalOrDerived::Ptr second;
    if(this->_isInteger) {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }

    if(((ATDecimalOrDerivedImpl*)(const Numeric*)other)->_isInteger) {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    return first->add(second, context);
  }
      
      
}

/** Returns a Numeric object which is the difference of this and
   * other */
Numeric::Ptr ATDecimalOrDerivedImpl::subtract(const Numeric::Ptr &other, const DynamicContext* context) const {
    if(this->isOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // if both are of the same type exactly, we can perform subtraction
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
    // if integer, return xs:integer, otherwise xs:decimal    
    if(_isInteger) {
      return context->getPathanFactory()->createInteger(_decimal - otherImpl->_decimal, context);
    }
    return context->getPathanFactory()->createDecimal(_decimal - otherImpl->_decimal, context);

  } else if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->subtract(other, context);
  } else if (this->isInstanceOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // here we know we have two decimals, and this is 'lower' in the hierarchy than other
    // so cast this to other's type
    return ((const Numeric::Ptr )this->castAs(other->getTypeURI(), other->getTypeName(), context))->subtract(other, context);
  } else if (other->isInstanceOfType(this->getTypeURI(), this->getTypeName(), context)) {
    // here we have two decimals, and this is 'higher' in the hierarchy than other
    // so cast other to this' type
    return this->subtract((const Numeric::Ptr )other->castAs(this->getTypeURI(), this->getTypeName(), context), context);
  } else {
    // we have two separate branches.  if either is instance of integer, cast it to integer, otherwise, cast to decimal
    // revisit: this is not the prettiest way to do it.  You would want to go up the tree one by one instead of
    // jumping to integer and decimal
    ATDecimalOrDerived::Ptr first; 
    ATDecimalOrDerived::Ptr second; 
    if(this->_isInteger) {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }

    if(((ATDecimalOrDerivedImpl*)(const Numeric*)other)->_isInteger) {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    return first->subtract(second, context);
  }

}

/** Returns a Numeric object which is the product of this and other */
Numeric::Ptr ATDecimalOrDerivedImpl::multiply(const Numeric::Ptr &other, const DynamicContext* context) const {
    if(this->isOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // if both are of the same type exactly, we can perform multiplication
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
    // if integer, return xs:integer, otherwise xs:decimal    
    if(_isInteger) {
      return context->getPathanFactory()->createInteger(_decimal * otherImpl->_decimal, context);
    }
    return context->getPathanFactory()->createDecimal(_decimal * otherImpl->_decimal, context);

  } else if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->multiply(other, context);
  } else if (this->isInstanceOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // here we know we have two decimals, and this is 'lower' in the hierarchy than other
    // so cast this to other's type
    return ((const Numeric::Ptr )this->castAs(other->getTypeURI(), other->getTypeName(), context))->multiply(other, context);
  } else if (other->isInstanceOfType(this->getTypeURI(), this->getTypeName(), context)) {
    // here we have two decimals, and this is 'higher' in the hierarchy than other
    // so cast other to this' type
    return this->multiply((const Numeric::Ptr )other->castAs(this->getTypeURI(), this->getTypeName(), context), context);
  } else {
    // we have two separate branches.  if either is instance of integer, cast it to integer, otherwise, cast to decimal
    // revisit: this is not the prettiest way to do it.  You would want to go up the tree one by one instead of
    // jumping to integer and decimal
    ATDecimalOrDerived::Ptr first;
    ATDecimalOrDerived::Ptr second; 
    if(this->_isInteger) {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    
    if(((ATDecimalOrDerivedImpl*)(const Numeric*)other)->_isInteger) {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    return first->multiply(second, context);
  }

}

/** Returns a Numeric object which is the quotient of this and other */
Numeric::Ptr ATDecimalOrDerivedImpl::divide(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->isOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // if both are of the same type exactly, we can perform division
    ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
  
    if(otherImpl->_decimal == MM_Zero) {
      DSLthrow(XPath2ErrorException, X("ATDecimalOrDerivedImpl::divide"), X("Division by zero [err:FOAR0001]"));
    }
  
    // return a xs:decimal, regardless of the actual types of the operands
    return context->getPathanFactory()->createDecimal(_decimal / otherImpl->_decimal, context);
  } else if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->divide(other, context);
  } else if (this->isInstanceOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // here we know we have two decimals, and this is 'lower' in the hierarchy than other
    // so cast this to other's type
    return ((const Numeric::Ptr )this->castAs(other->getTypeURI(), other->getTypeName(), context))->divide(other, context);
  } else if (other->isInstanceOfType(this->getTypeURI(), this->getTypeName(), context)) {
    // here we have two decimals, and this is 'higher' in the hierarchy than other
    // so cast other to this' type
    return this->divide((const Numeric::Ptr )other->castAs(this->getTypeURI(), this->getTypeName(), context), context);
  } else {
    // we have two separate branches.  if either is instance of integer, cast it to integer, otherwise, cast to decimal
    // revisit: this is not the prettiest way to do it.  You would want to go up the tree one by one instead of
    // jumping to integer and decimal
    ATDecimalOrDerived::Ptr first;
    ATDecimalOrDerived::Ptr second; 
    if(this->_isInteger) {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    
    if(((ATDecimalOrDerivedImpl*)(const Numeric*)other)->_isInteger) {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    return first->divide(second, context);
  }

}

/** Returns the arithmetic product of its operands as a Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::mod(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->isOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // if both are of the same type exactly, we can perform the modulo
    const ATDecimalOrDerivedImpl* otherImpl = (ATDecimalOrDerivedImpl*)(const Numeric*)other;
  
    if(otherImpl->isZero()) {
      DSLthrow(IllegalArgumentException, X("ATDecimalOrDerivedImpl::mod"), X("Division by zero [err:FOAR0001]"));
    }
  
    MAPM result = _decimal;
    MAPM r;
    r = result.integer_divide(otherImpl->_decimal);
    result -= r * otherImpl->_decimal;
    // if integer, return xs:integer, otherwise xs:decimal    
    if(_isInteger) {
      return context->getPathanFactory()->createInteger(result, context);
    }
    return context->getPathanFactory()->createDecimal(result, context);
  } else if(this->getPrimitiveTypeIndex() != other->getPrimitiveTypeIndex()) {
    // if other is not a decimal, then we need to promote this to a float or double
    return ((const Numeric::Ptr )this->castAs(other->getPrimitiveTypeURI(), other->getPrimitiveTypeName(), context))->mod(other, context);
  } else if (this->isInstanceOfType(other->getTypeURI(), other->getTypeName(), context)) {
    // here we know we have two decimals, and this is 'lower' in the hierarchy than other
    // so cast this to other's type
    return ((const Numeric::Ptr )this->castAs(other->getTypeURI(), other->getTypeName(), context))->mod(other, context);
  } else if (other->isInstanceOfType(this->getTypeURI(), this->getTypeName(), context)) {
    // here we have two decimals, and this is 'higher' in the hierarchy than other
    // so cast other to this' type
    return this->mod((const Numeric::Ptr )other->castAs(this->getTypeURI(), this->getTypeName(), context), context);
  } else {
    // we have two separate branches.  if either is instance of integer, cast it to integer, otherwise, cast to decimal
    // revisit: this is not the prettiest way to do it.  You would want to go up the tree one by one instead of
    // jumping to integer and decimal
    ATDecimalOrDerived::Ptr first;
    ATDecimalOrDerived::Ptr second; 
    if(this->_isInteger) {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      first = (const ATDecimalOrDerived::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    
    if(((ATDecimalOrDerivedImpl*)(const Numeric*)other)->_isInteger) {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    } else {
      second = (const ATDecimalOrDerived::Ptr )other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
    }
    return first->mod(second, context);
  }

}

/** Returns the floor of this Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::floor(const DynamicContext* context) const {
  // if integer, return xs:integer, otherwise xs:decimal    
  if(_isInteger) {
    return (const Numeric::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                        XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
  }
  return context->getPathanFactory()->createDecimal(_decimal.floor(), context);

}

/** Returns the ceiling of this Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::ceiling(const DynamicContext* context) const {
  // if integer, return xs:integer, otherwise xs:decimal    
  if(_isInteger) {
    return (const Numeric::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                        XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
  }
  return context->getPathanFactory()->createDecimal(_decimal.ceil(), context);

}

/** Rounds this Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::round(const DynamicContext* context) const {
  // if integer, return xs:integer, otherwise xs:decimal    
  if(_isInteger) {
    return (const Numeric::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                        XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
  }
  MAPM value = _decimal + 0.5;
  return context->getPathanFactory()->createDecimal(value.floor(), context);
}

/** Rounds this Numeric to the given precision, and rounds a half to even */
Numeric::Ptr ATDecimalOrDerivedImpl::roundHalfToEven(const Numeric::Ptr &precision, const DynamicContext* context) const {
  ATDecimalOrDerived::Ptr decimal_precision = (const Numeric::Ptr)precision->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context);
  MAPM exp = MAPM(MM_Ten).pow(((ATDecimalOrDerivedImpl*)(const ATDecimalOrDerived*)decimal_precision)->_decimal);
  MAPM value = _decimal * exp;
  bool halfVal = false;

  // check if we're rounding on a half value 
  if((value-0.5) == (value.floor())) {
    halfVal = true;
  }
  value = _decimal * exp + 0.5;
  value = value.floor();

  // if halfVal make sure what we return has the least significant digit even
  if (halfVal) {
    if(value.is_odd()) {
      value = value - 1;
    }
  }
  value = value / exp;  
  // if integer, return xs:integer, otherwise xs:decimal    
  if(_isInteger) {
    return context->getPathanFactory()->createInteger(value, context);
  }
  return context->getPathanFactory()->createDecimal(value, context);
}

/** Returns the Additive inverse of this Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::invert(const DynamicContext* context) const {
  if(_isInteger) {
    return context->getPathanFactory()->createInteger(_decimal.neg(), context);
  }
  return context->getPathanFactory()->createDecimal(_decimal.neg(), context);
}

/** Returns the absolute value of this Numeric */
Numeric::Ptr ATDecimalOrDerivedImpl::abs(const DynamicContext* context) const {
  return context->getPathanFactory()->createDecimal(_decimal.abs(), context);
}

/** Does this Numeric have value 0? */
bool ATDecimalOrDerivedImpl::isZero() const {
  return _decimal == MM_Zero;
}

/** Is this Numeric negative? */
bool ATDecimalOrDerivedImpl::isNegative() const {
  return _decimal < MM_Zero;
}

/** Is this Numeric positive? */
bool ATDecimalOrDerivedImpl::isPositive() const {
  return _decimal > MM_Zero;
}

/** Treat this decimal (must be integer) as a codepoint **/
XMLCh ATDecimalOrDerivedImpl::treatAsCodepoint(const DynamicContext* context) const {
  if(_isInteger) {
    char out_string[256];
    _decimal.toIntegerString(out_string);
    int integer = atoi(out_string);
    XMLCh ch = (XMLCh)integer;
    if(integer<=0 || (int)ch != integer) { // negative or lost some info
      DSLthrow(XPath2ErrorException, X("ATDecimalOrDerivedImpl::treatAsCodepoint"), X("codepoint not legal."));
    }
    return ch;
  } else {
    DSLthrow(XPath2ErrorException, X("ATDecimalOrDerivedImpl::treatAsCodepoint"), X("Only integers can be treated as codepoints."));
  }
}

AnyAtomicType::AtomicObjectType ATDecimalOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

//////////////////////////////////////
// Horrible Hack to make Dates      //
// work for now. Loss of Precision! //
//////////////////////////////////////
MAPM ATDecimalOrDerivedImpl::asMAPM() const {
  return _decimal;
}

void ATDecimalOrDerivedImpl::setDecimal(const XMLCh* const value, const StaticContext *context) {

  if(value == NULL) {
    DSLthrow(XPath2TypeCastException,X("ATDecimalOrDerivedImpl::setDecimal"), X("Invalid representation of decimal"));
  }
  
  unsigned int length=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(value) + 1;

  AutoDeallocate<char> buffer(context->getMemoryManager(), length * sizeof(char));

  bool gotPoint = false;
  bool gotSign = false;
  bool gotDigit = false;
  bool stop = false;
  bool munchWS = true;

  const XMLCh *src = value;
  char *dest = buffer;
  XMLCh tmpChar;
  while(!stop && *src != 0) {
    tmpChar = *src++;
    
    switch(tmpChar) {/*{{{*/

    case L'+': {
      *dest++ = '+';
      if(gotSign) {
        stop = true;
      } else {
        gotSign = true;
      }
      break;
    }
             
    case L'-': {
      *dest++ = '-';
      if(gotSign) {
        stop = true;
      } else {
        gotSign = true;
      }
      break;
    }
             
      //This is '.'
    case 0x002e: {
      *dest++ = '.';
      if(gotPoint) {
        stop = true;
      } else {
        gotPoint = true;
      }
      break;
    }
             
      /* All the numerals defined by XML standard */
    case 0x0030:
    case 0x0031:
    case 0x0032:
    case 0x0033:
    case 0x0034:
    case 0x0035:
    case 0x0036:
    case 0x0037:
    case 0x0038:
    case 0x0039: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0030) + '0';
      break;
    }
             
      // whitespace at start or end of string...
    case 0x0020:
    case 0x0009:
    case 0x000d:
    case 0x000a: {
      bool endOfWS = false;
      while(!endOfWS && *src != 0) {
        tmpChar = *src++;
        switch(tmpChar) {
        case 0x0020:
        case 0x0009:
        case 0x000d:
        case 0x000a: {
          break;
        }
        default: {
          endOfWS = true;
          --src;
          if(munchWS) {
            //end of leading whitespace
            munchWS = false;
          } else {
            //trailing whitespace is followed by other characters - so return NaN.
            stop = true;
          }
        }
        }//switch
      }//while
      break;
    }
             
    default:
      stop = true;
      break;
             
    }//switch
    /*}}}*/
    
  }//while

  if(!gotDigit || stop) {
		DSLthrow(XPath2TypeCastException,X("ATDecimalOrDerivedImpl::setDecimal"), X("Invalid representation of decimal"));
  }

  *dest++ = 0; // Null terminate  
  _decimal = (char*)buffer;
}

