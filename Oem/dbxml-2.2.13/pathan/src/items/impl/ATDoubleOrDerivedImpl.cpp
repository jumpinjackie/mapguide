/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/internal/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/PathanFactory.hpp>

#include <assert.h>

ATDoubleOrDerivedImpl::
ATDoubleOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATDoubleOrDerived(), 
    _isNegative(false),
    _typeName(typeName),
    _typeURI(typeURI) { 
      
  setDouble(value, context);
  
  // if state is NaN, it could be because it should be INF or -INF
  if(_state == NaN) {
    if(XPath2Utils::equals(value, Numeric::NegINF_string)) {
      _state = NEG_INF;
      _isNegative = true;
    } else if (XPath2Utils::equals(value, Numeric::INF_string)) {
      _state = INF;
    }
  }
}

ATDoubleOrDerivedImpl::
ATDoubleOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const StaticContext* context): 
    ATDoubleOrDerived(),
    _isNegative(false), 
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  _double = value;
  _state = NUM;
  if (value.sign() < 0) 
    _isNegative = true;
}

void *ATDoubleOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATDoubleOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATDoubleOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATDoubleOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATDoubleOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATDoubleOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::DOUBLE;
} 

AnyAtomicType::Ptr ATDoubleOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  switch (targetIndex) {
    case DECIMAL: {
      if (_state != NUM)
        DSLthrow(IllegalArgumentException, X("ATDoubleOrDerivedImpl::castAsInternal"), X("Special values like NaN, INF or -INF cannot be cast to decimal [err:FOCA0002]"));

      if (context->isTypeOrDerivedFromType(targetURI, targetType,
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER)) {
        char obuf[1024];
        _double.toIntegerString(obuf);
        return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDecimalOrDerived(targetURI, targetType, obuf, context);
      }    
      else
        return (const AnyAtomicType::Ptr)context->getPathanFactory()->createDecimalOrDerived(targetURI, targetType, _double, context);
    }
    case BOOLEAN: {
      if (this->isZero() || this->isNaN()) {
        return context->getPathanFactory()->createBooleanOrDerived(targetURI, targetType, false, context);
      } else {
        return context->getPathanFactory()->createBooleanOrDerived(targetURI, targetType, true, context);
      }
    }
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATDoubleOrDerivedImpl::asString(const DynamicContext* context) const {

  switch (_state) {
    case NaN:     return Numeric::NaN_string;
    case INF:     return Numeric::INF_string;
    case NEG_INF: return Numeric::NegINF_string;
    default: /*NUM*/
    {
      if(isZero())
      {
        if(isNegative())
          return Numeric::NegZero_string;
        else
          return Numeric::PosZero_string;
      }
      else
      {
        MAPM lower(0.000001), upper(1000000);
        MAPM absVal=_double.abs();
        if(absVal<upper && absVal>=lower)
        {
          // treat it as a decimal
          const ATDecimalOrDerived::Ptr decimal=context->getPathanFactory()->createDecimal(_double, context);
          return decimal->asString(context);
        }
        else
        {
          char obuf[1024];
          int precision = _double.significant_digits();
          // we need to subtract 1 for the digit before the '.'
          int outputPrecision = precision < DOUBLE_MAX_DIGITS ? precision-1 : DOUBLE_MAX_DIGITS;
          // precision needs to be at least 1
          if(outputPrecision == 0) {
            outputPrecision++;
          }
          _double.toString(obuf, outputPrecision);
          return context->getMemoryManager()->getPooledString(obuf);
        }
      }
    }
  }

  return 0;
}

/* Promote this to the given type, if possible */
Numeric::Ptr ATDoubleOrDerivedImpl::promoteTypeIfApplicable(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const {
  // can only promote if this target is instance of double, and this isInstanceOf target
  if(context->isTypeOrDerivedFromType(typeURI, typeName, this->getPrimitiveTypeURI(), this->getPrimitiveTypeName()) &&
     this->isInstanceOfType(typeURI, typeName, context)) {
    return this;  // no need to promote, already a double
    //return (const Numeric::Ptr )this->castAs(typeURI, typeName, context);
  } else {
    return 0;
  }  
}

/* returns true if the two objects' value are equal
   * false otherwise */
bool ATDoubleOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(!target->isNumericValue()) {
    DSLthrow(IllegalArgumentException,X("ATDoubleOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  } 

  if(target->getPrimitiveTypeIndex() != AnyAtomicType::DOUBLE) {
    // if target is not a double, promote it to one
    return this->equals(target->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } else {
    // same primitive type, can make comparison
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const AnyAtomicType*)target;
    if (_state != otherImpl->_state)
      return false;

    switch (_state) {
      case NaN: return false;
      case INF: return otherImpl->isInfinite() && !otherImpl->isNegative();
      case NEG_INF: return otherImpl->isInfinite() && otherImpl->isNegative();
      default: /*NUM*/{
        return _double == otherImpl->_double;
      }
    }
  } 
}

/** Returns true if this is less than other, false otherwise */
bool ATDoubleOrDerivedImpl::lessThan(const Numeric::Ptr &other, const DynamicContext* context) const { 
  if(other->getPrimitiveTypeIndex() != AnyAtomicType::DOUBLE) {
    // if other is not a double, promote it to one
    return this->lessThan((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } else {
    // same primitive type, can make comparison
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return false;  

    switch (_state) {
      case NaN: return false;
      case INF: return false;
      case NEG_INF: return true;
      default: /*NUM*/{
        switch(otherImpl->_state) {
          case NaN: return false; // case taken care of above
          case INF: return true;
          case NEG_INF: return false;
          default:  return _double < otherImpl->_double;
        }// switch
      }// default
    }// switch
  }// else
}

/** Returns true if this is greater than other, false otherwise */
bool ATDoubleOrDerivedImpl::greaterThan(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(other->getPrimitiveTypeIndex() != AnyAtomicType::DOUBLE) {
    // if other is not a double, promote it to one
    return this->greaterThan((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } else {
    // same primitive type, can make comparison
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return false;

    switch (_state) {
      case NaN: return false;
      case INF: return true;
      case NEG_INF: return false;
      default: /*NUM*/{
        switch(otherImpl->_state) {
          case NaN: return false; // case taken care of above
          case INF: return false;
          case NEG_INF: return true;
          default:  return _double > otherImpl->_double;
        }// switch
      }// default
    }// switch
  }// else   
}

/** Returns a Numeric object which is the sum of this and other */
Numeric::Ptr ATDoubleOrDerivedImpl::add(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() == other->getPrimitiveTypeIndex()) {
    // if both are of the same type exactly, we can perform addition
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return notANumber(context);

    switch (_state) {
      case NaN: return notANumber(context);
      case INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);       // case taken care of above
          case NUM: return infinity(context);         // INF + NUM = INF
          case INF: return infinity(context);         // INF + INF = INF
          case NEG_INF: return notANumber(context);   // INF + (-INF) = NaN
          default: assert(false); return 0; // should never get here
        }
      }
      case NEG_INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          //case taken care of above
          case NUM: return negInfinity(context);         // -INF + NUM = -INF
          case INF: return notANumber(context);          // -INF + INF = NaN
          case NEG_INF: return negInfinity(context);     // -INF + (-INF) = -INF
          default: assert(false); return 0; // should never get here
        }
      }                
      case NUM: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context); // case taken care of above
          case INF: return infinity(context);
          case NEG_INF: return negInfinity(context);
          case NUM: return newDouble(_double + otherImpl->_double, context);
          default: assert(false); return 0; // should never get here 
        }
      }
      default: assert(false); return 0; // should never get here 
    } 
    
  } else {
    // if other is not a double, then we need to promote it to a double
    return this->add((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } 
}

/** Returns a Numeric object which is the difference of this and
   * other */
Numeric::Ptr ATDoubleOrDerivedImpl::subtract(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() == other->getPrimitiveTypeIndex()) {
    // if both are of the same type exactly, we can perform subtraction
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return notANumber(context);

    switch (_state) {
      case NaN: return notANumber(context);
      case INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);   // case taken care of above
          case NUM: return infinity(context);     // INF - NUM = INF
          case INF: return notANumber(context);   // INF - INF = NaN
          case NEG_INF: return infinity(context); // INF - (-INF) = INF
          default: assert(false); return 0; // should never get here
        }
      }
      case NEG_INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          //case taken care of above
          case NUM: return negInfinity(context);         // -INF - NUM = -INF
          case INF: return negInfinity(context);         // -INF - INF = -INF
          case NEG_INF: return notANumber(context);      // -INF - (-INF) = NaN
          default: assert(false); return 0; // should never get here
        }
      }                
      case NUM: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          // case taken care of above
          case INF: return negInfinity(context);         // NUM - INF = -INF
          case NEG_INF: return infinity(context);        // NUM - (-INF) = INF
          case NUM: return newDouble(_double - otherImpl->_double, context);
          default: assert(false); return 0;  // should never get here
        }
      }
      default: assert(false); return 0;  // should never get here
    } 
    
  } else {
    // if other is not a double, then we need to promote it to a double
    return this->subtract((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } 
}

/** Returns a Numeric object which is the product of this and other */
Numeric::Ptr ATDoubleOrDerivedImpl::multiply(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() == other->getPrimitiveTypeIndex()) {
    // if both are of the same type, we can perform multiplication
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return notANumber(context);

    switch (_state) {
      case NaN: return notANumber(context);
      case INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);      // case taken care of above
          case NUM: return other->isPositive() ? infinity(context) : negInfinity(context);        // INF * NUM = +/-INF
          case INF: return infinity(context);        // INF * INF = INF
          case NEG_INF: return negInfinity(context); // INF * (-INF) = -INF
          default: assert(false); return 0; // should never get here
        }
      }
      case NEG_INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          //case taken care of above
          case NUM: return other->isPositive() ? negInfinity(context) : infinity(context);         // -INF * NUM = +/-INF
          case INF: return negInfinity(context);         // -INF * INF = -INF
          case NEG_INF: return infinity(context);        // -INF * (-INF) = INF
          default: assert(false); return 0; // should never get here
        }
      }                
      case NUM: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          // case taken care of above
          case INF: return this->isPositive() ? infinity(context) : negInfinity(context);            // NUM * INF = +/-INF
          case NEG_INF: return this->isPositive() ? negInfinity(context) : infinity(context);        // NUM * (-INF) = +/-INF
          case NUM: 
            if(other->isZero() || this->isZero()) {
              if((this->isNegative() && other->isPositive()) ||
                 (this->isPositive() && other->isNegative())) {
                return negZero(context);                // 0 / (-NUM) or (-0) / NUM = -0
              } else {
                return newDouble(0, context);           // 0 / NUM or (-0) / (-NUM) = 0
              }
            }
            return newDouble(_double * otherImpl->_double, context);
          default: assert(false); return 0;  // should never get here
        }
      }
      default: assert(false); return 0;  // should never get here
    } 
    
  } else {
    // if other is not a double, then we need to promote it to a double
    return this->multiply((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } 
}

/** Returns a Numeric object which is the quotient of this and other */
Numeric::Ptr ATDoubleOrDerivedImpl::divide(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() == other->getPrimitiveTypeIndex()) {
    // if both are of the same type, we can perform division
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(otherImpl->_state == NaN) return notANumber(context);
        switch (_state) {
      case NaN: return notANumber(context);
      case INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);      // case taken care of above
          case NUM: return other->isPositive() ? infinity(context) : negInfinity(context);        // INF / NUM = +/-INF
          case INF: return notANumber(context);      // INF / INF = NaN
          case NEG_INF: return notANumber(context);  // INF / (-INF) = NaN
          default: assert(false); return 0; // should never get here
        } // switch
      }// case
      case NEG_INF: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          //case taken care of above
          case NUM: return other->isPositive() ? negInfinity(context) : infinity(context);         // -INF / NUM = -INF
          case INF: return notANumber(context);          // -INF / INF = NaN
          case NEG_INF: return notANumber(context);      // -INF / (-INF) = NaN
          default: assert(false); return 0; // should never get here
        } // switch
      } // case    
      case NUM: {
        switch(otherImpl->_state) {
          case NaN: return notANumber(context);          // case taken care of above
          case INF: {                                    // NUM / INF = +/-0
            if(this->isNegative()) {
              return negZero(context);
            } else {
              return newDouble(0, context);
            }
          }// case
          case NEG_INF: {                               // NUM / -INF = +/-0
            if(this->isPositive()) {
              return negZero(context);
            } else {
              return newDouble(0, context);
            }
          }// case
          case NUM: {
            if(other->isZero()) {
              if(this->isZero()) return notANumber(context);
              if((this->isNegative() && other->isPositive()) ||
                 (this->isPositive() && other->isNegative())) {
                return negInfinity(context);            // NUM / (-0) or (-NUM) / 0 = -INF
              } else {
                return infinity(context);               // NUM / 0 or (-NUM) / (-0) = INF
              }
            }
            else if(this->isZero())
            {
              if((this->isNegative() && other->isPositive()) ||
                 (this->isPositive() && other->isNegative())) {
                return negZero(context);                // 0 / (-NUM) or (-0) / NUM = -0
              } else {
                return newDouble(0, context);           // 0 / NUM or (-0) / (-NUM) = 0
              }
            }
            return newDouble(_double / otherImpl->_double, context);
          }// case
          default: assert(false); return 0;  // should never get here
        }// switch
      }// case
      default: assert(false); return 0;  // should never get here
    }// switch

  } else {
    // if other is not a double, then we need to promote it to a double
    return this->divide((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } 
}

/** Returns the mod of its operands as a Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::mod(const Numeric::Ptr &other, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() == other->getPrimitiveTypeIndex()) {
    // if both are of the same type, we can perform mod
    ATDoubleOrDerivedImpl* otherImpl = (ATDoubleOrDerivedImpl*)(const Numeric*)other;
    if(this->isNaN() || otherImpl->isNaN() || this->isInfinite() || otherImpl->isZero()) {
      return notANumber(context);
    
    } else if(otherImpl->isInfinite() || this->isZero()) {
      return (const Numeric::Ptr )this->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                          XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
    
    } else {
      MAPM result = _double;
      MAPM r;
      r = result.integer_divide(otherImpl->_double);
      result -= r * otherImpl->_double;
      if (result == 0 && isNegative())
        return negZero(context);
      return newDouble(result, context);
    }
  } else {
    // if other is not a double, then we need to promote it to a double
    return this->mod((const Numeric::Ptr )other->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context), context);
  } 
}

/** Returns the floor of this Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::floor(const DynamicContext* context) const {
  switch (_state) {
    case NaN: return notANumber(context);
    case INF: return infinity(context);
    case NEG_INF: return negInfinity(context);
    case NUM: { 
      if (isZero() && isNegative())
        return this;
      return newDouble(_double.floor(), context); 
    }      
    default: { assert(false); return 0; // should never get here 
    }
  }
}

/** Returns the ceiling of this Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::ceiling(const DynamicContext* context) const {
  switch (_state) {
    case NaN: return notANumber(context);
    case INF: return infinity(context);
    case NEG_INF: return negInfinity(context);
    case NUM: { 
      if (isNegative() && _double >= -0.5) {
        return negZero(context);
      }
      return newDouble(_double.ceil(), context); 
    }
    default: { assert(false); return 0; // should never get here 
    }
  }
}

/** Rounds this Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::round(const DynamicContext* context) const {
  switch (_state) {
    case NaN: return notANumber(context);
    case INF: return infinity(context);
    case NEG_INF: return negInfinity(context);
    case NUM: { 
      if (isNegative() &&_double >= -0.5) {
        return negZero(context);
      }      
      MAPM value = _double + 0.5; 
      return newDouble(value.floor(), context); 
    }
  default: {assert(false); return 0; // should never get here
    }
  }
}

/** Rounds this Numeric to the given precision, and rounds a half to even */
Numeric::Ptr ATDoubleOrDerivedImpl::roundHalfToEven(const Numeric::Ptr &precision, const DynamicContext* context) const {
  switch (_state) {
    case NaN: return notANumber(context);
    case INF: return infinity(context);
    case NEG_INF: return negInfinity(context);
    case NUM: break;
    default: { assert(false); return 0;  // should never get here
    }
  }

  if (isZero() && isNegative())
    return this;
  
  ATDoubleOrDerived::Ptr double_precision = (const Numeric::Ptr)precision->castAs(this->getPrimitiveTypeURI(), this->getPrimitiveTypeName(), context);
  MAPM exp = MAPM(10).pow(((ATDoubleOrDerivedImpl*)(const ATDoubleOrDerived*)double_precision)->_double);
  MAPM value = _double * exp;
  bool halfVal = false;

  // check if we're rounding on a half value 
  if((value-0.5) == (value.floor())) {
    halfVal = true;
  }
  value = _double * exp + 0.5;
  value = value.floor();

  // if halfVal make sure what we return has the least significant digit even
  if (halfVal) {
    if(value.is_odd()) {
      value = value - 1;
    }
  }
  value = value / exp;

  // the spec doesn't actually say to do this, but djf believes this is the correct way to handle rounding of -ve values which will result in 0.0E0
  // if (value == 0 && isNegative())
    // return negZero(context);
  return newDouble(value, context);
}

/** Returns the Additive inverse of this Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::invert(const DynamicContext* context) const {
  switch (_state) {
    case NaN: return this;
    case INF: return negInfinity(context);
    case NEG_INF: return infinity(context);
    case NUM: 
        if(this->isZero())
        {
          if(this->isNegative())
            return newDouble(0, context);
          else
            return negZero(context);
        }
        return newDouble(_double.neg(), context);
    default: assert(false); return 0;  // should never get here
  }
}

/** Returns the absolute value of this Numeric */
Numeric::Ptr ATDoubleOrDerivedImpl::abs(const DynamicContext* context) const {
  switch (_state) {
    case NaN: return this;
    case INF: return infinity(context);
    case NEG_INF: return infinity(context);
    case NUM: return newDouble(_double.abs(), context);
    default: assert(false); return 0;  // should never get here
  }
}

/** Does this Numeric have value 0? */
bool ATDoubleOrDerivedImpl::isZero() const {
  switch (_state) {
    case NaN: 
    case INF: 
    case NEG_INF: return false;
    default: /*NUM*/{
      return _double == MM_Zero;
    }
  }
}

/** Is this Numeric negative? */
bool ATDoubleOrDerivedImpl::isNegative() const {
  switch (_state) {
    case NaN: return false;
    case INF: return false;
    case NEG_INF: return true;
    default: /*NUM*/{
      return _isNegative;
    }
  }
}

/** Is this Numeric positive? */
bool ATDoubleOrDerivedImpl::isPositive() const {
  switch (_state) {
    case NaN: return false;
    case INF: return true;
    case NEG_INF: return false;
    default: /*NUM*/{
      return !_isNegative;
    }
  }
}

/* Is this xs:double not a number */
bool ATDoubleOrDerivedImpl::isNaN() const {
  return _state == NaN;
}


/* Is this xs:double infinite? */
bool ATDoubleOrDerivedImpl::isInfinite() const {
  return _state == INF || _state == NEG_INF;
}


AnyAtomicType::AtomicObjectType ATDoubleOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

/* returns a new infinity xs:double*/
ATDoubleOrDerived::Ptr ATDoubleOrDerivedImpl::infinity(const DynamicContext* context) const {
  return context->getPathanFactory()->createDouble(Numeric::INF_string, context);
}

/* returns a new negative infinity xs:double*/
ATDoubleOrDerived::Ptr ATDoubleOrDerivedImpl::negInfinity(const DynamicContext* context) const {
  return context->getPathanFactory()->createDouble(Numeric::NegINF_string, context);
}
  
/* returns a NaN xs:double*/
ATDoubleOrDerived::Ptr ATDoubleOrDerivedImpl::notANumber(const DynamicContext* context) const {
  return context->getPathanFactory()->createDouble(Numeric::NaN_string, context);
}

/* returns a -0 ATDoubleOrDerived*/
ATDoubleOrDerived::Ptr ATDoubleOrDerivedImpl::negZero(const DynamicContext* context) const {
  return context->getPathanFactory()->createDouble(Numeric::NegZero_string, context);
}

/*returns a ATDoubleOrDerived of value value*/
ATDoubleOrDerived::Ptr ATDoubleOrDerivedImpl::newDouble(MAPM value, const DynamicContext* context) const {
  return context->getPathanFactory()->createDouble(value, context);
}

void ATDoubleOrDerivedImpl::setDouble(const XMLCh* const value, const StaticContext *context) {

  if(value == NULL) {
    // Not a Number
    _double = MM_Zero;
    _state = NaN;
    return;
  }//if
  
  unsigned int length=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(value) + 1;

  AutoDeallocate<char> buffer(context->getMemoryManager(), length * sizeof(char));

  bool gotBase = false;
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
      if(gotSign || gotDigit) {
        stop = true;
      } else {
        gotSign = true;
      }
      break;
    }
             
    case L'-': {
      *dest++ = '-';
      if(gotSign || gotDigit) {
        stop = true;
      } else {
        gotSign = true;
        if(!gotBase) _isNegative = true;
      }
      break;
    }
             
    case 0x0045:
    case 0x0065: {
      *dest++ = 'e';
      if(!gotDigit || gotBase) {
        stop = true;
      } else {
        gotPoint = false;
        gotSign = false;        
        gotBase = true;
        gotDigit = false;
      }
      break;
    }
             
      //This is '.'
    case 0x002e: {
      *dest++ = '.';
      if(gotPoint || gotBase) {
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
             
    case 0x0660:
    case 0x0661:
    case 0x0662:
    case 0x0663:
    case 0x0664:
    case 0x0665:
    case 0x0666:
    case 0x0667:
    case 0x0668:
    case 0x0669: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0660) + '0';
      break;
    }
             
    case 0x06f0:
    case 0x06f1:
    case 0x06f2:
    case 0x06f3:
    case 0x06f4:
    case 0x06f5:
    case 0x06f6:
    case 0x06f7:
    case 0x06f8:
    case 0x06f9: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x06f0) + '0';
      break;
    }
             
    case 0x0966:
    case 0x0967:
    case 0x0968:
    case 0x0969:
    case 0x096a:
    case 0x096b:
    case 0x096c:
    case 0x096d:
    case 0x096e:
    case 0x096f: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0966) + '0';
      break;
    }
             
    case 0x09e6:
    case 0x09e7:
    case 0x09e8:
    case 0x09e9:
    case 0x09ea:
    case 0x09eb:
    case 0x09ec:
    case 0x09ed:
    case 0x09ee:
    case 0x09ef: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x09e6) + '0';
      break;
    }
             
    case 0x0a66:
    case 0x0a67:
    case 0x0a68:
    case 0x0a69:
    case 0x0a6a:
    case 0x0a6b:
    case 0x0a6c:
    case 0x0a6d:
    case 0x0a6e:
    case 0x0a6f: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0a66) + '0';
      break;
    }
             
    case 0x0ae6:
    case 0x0ae7:
    case 0x0ae8:
    case 0x0ae9:
    case 0x0aea:
    case 0x0aeb:
    case 0x0aec:
    case 0x0aed:
    case 0x0aee:
    case 0x0aef: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0ae6) + '0';
      break;
    }
             
    case 0x0b66:
    case 0x0b67:
    case 0x0b68:
    case 0x0b69:
    case 0x0b6a:
    case 0x0b6b:
    case 0x0b6c:
    case 0x0b6d:
    case 0x0b6e:
    case 0x0b6f: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0b66) + '0';
      break;
    }
             
      //Note: For this language, there appears to be no zero
      //case 0x0be6
    case 0x0be7:
    case 0x0be8:
    case 0x0be9:
    case 0x0bea:
    case 0x0beb:
    case 0x0bec:
    case 0x0bed:
    case 0x0bee:
    case 0x0bef: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0be6) + '0';
      break;
    }
             
    case 0x0c66:
    case 0x0c67:
    case 0x0c68:
    case 0x0c69:
    case 0x0c6a:
    case 0x0c6b:
    case 0x0c6c:
    case 0x0c6d:
    case 0x0c6e:
    case 0x0c6f: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0c66) + '0';
      break;
    }
             
    case 0x0ce6:
    case 0x0ce7:
    case 0x0ce8:
    case 0x0ce9:
    case 0x0cea:
    case 0x0ceb:
    case 0x0cec:
    case 0x0ced:
    case 0x0cee:
    case 0x0cef: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0ce6) + '0';
      break;
    }
             
    case 0x0d66:
    case 0x0d67:
    case 0x0d68:
    case 0x0d69:
    case 0x0d6a:
    case 0x0d6b:
    case 0x0d6c:
    case 0x0d6d:
    case 0x0d6e:
    case 0x0d6f: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0d66) + '0';
      break;
    }
             
    case 0x0e50:
    case 0x0e51:
    case 0x0e52:
    case 0x0e53:
    case 0x0e54:
    case 0x0e55:
    case 0x0e56:
    case 0x0e57:
    case 0x0e58:
    case 0x0e59: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0e50) + '0';
      break;
    }
             
    case 0x0ed0:
    case 0x0ed1:
    case 0x0ed2:
    case 0x0ed3:
    case 0x0ed4:
    case 0x0ed5:
    case 0x0ed6:
    case 0x0ed7:
    case 0x0ed8:
    case 0x0ed9: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0ed0) + '0';
      break;
    }
             
    case 0x0f20:
    case 0x0f21:
    case 0x0f22:
    case 0x0f23:
    case 0x0f24:
    case 0x0f25:
    case 0x0f26:
    case 0x0f27:
    case 0x0f28:
    case 0x0f29: {
      gotDigit = true;
      *dest++ = (char)(tmpChar - 0x0f20) + '0';
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
    // Not a Number
    _double = MM_Zero;
    _state = NaN;
    return;
  }

  *dest++ = 0; // Null terminate  
  _double = (char*)buffer;
  _state = NUM;
}
 

