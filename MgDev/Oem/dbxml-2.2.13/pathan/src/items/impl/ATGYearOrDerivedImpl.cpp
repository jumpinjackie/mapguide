/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATGYearOrDerivedImpl.hpp"
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <pathan/internal/DOMutils/XStr.hpp> // defines X() and XMLCh*
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/Timezone.hpp>
#include "../../utils/DateUtils.hpp"
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

#include <xercesc/util/XMLString.hpp>

ATGYearOrDerivedImpl::
ATGYearOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    ATGYearOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
     setGYear(value, context); 
}

void *ATGYearOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATGYearOrDerivedImpl::getPrimitiveTypeName() const {
  return getPrimitiveName();
}

const XMLCh* ATGYearOrDerivedImpl::getPrimitiveName() {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATGYearOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATGYearOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATGYearOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::G_YEAR;
} 

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATGYearOrDerivedImpl::asString(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(_YY->greaterThan(context->getPathanFactory()->createInteger(9999, context), context) ||
     _YY->lessThan(context->getPathanFactory()->createInteger(-9999, context), context)) {
    buffer.set(_YY->asString(context));
  } else {
    buffer.set(((const ATDecimalOrDerived*)_YY)->asString(4, context)); //pad to 4 digits
  }
  
  if ( _hasTimezone == true ) {
    buffer.append(timezone_->asString(context));
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());

}

/* returns true if the two objects
   * false otherwise */
bool ATGYearOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATGYearOrDerivedImpl::equals"), 
        X("Equality operator for given types not supported"));
  }
  ATGYearOrDerivedImpl* targetGYear = (ATGYearOrDerivedImpl*)(const AnyAtomicType*)target;
  if ( _hasTimezone == targetGYear->_hasTimezone) {
    return ( (!_hasTimezone || timezone_->equals(targetGYear->timezone_)) &&
             this->_YY->equals((const AnyAtomicType::Ptr)targetGYear->_YY, context) );
  }
  else {
    return false;
  }

}

/** Returns true if this is greater than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGYearOrDerivedImpl::greaterThan(const ATGYearOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGYearOrDerived::Ptr thisNorm = this;
  ATGYearOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGYearOrDerivedImpl* thisImpl=(ATGYearOrDerivedImpl*)(const ATGYearOrDerived*)thisNorm;
  ATGYearOrDerivedImpl* otherImpl=(ATGYearOrDerivedImpl*)(const ATGYearOrDerived*)otherNorm;
  return (thisImpl->_YY->greaterThan(otherImpl->_YY, context) ||
         (thisImpl->_YY->equals(otherImpl->_YY, context) && thisImpl->timezone_->greaterThan(otherImpl->timezone_)));
}

/** Returns true if this is less than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGYearOrDerivedImpl::lessThan(const ATGYearOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGYearOrDerived::Ptr thisNorm = this;
  ATGYearOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGYearOrDerivedImpl* thisImpl=(ATGYearOrDerivedImpl*)(const ATGYearOrDerived*)thisNorm;
  ATGYearOrDerivedImpl* otherImpl=(ATGYearOrDerivedImpl*)(const ATGYearOrDerived*)otherNorm;
  return (thisImpl->_YY->lessThan(otherImpl->_YY, context) ||
         (thisImpl->_YY->equals(otherImpl->_YY, context) && thisImpl->timezone_->lessThan(otherImpl->timezone_)));
}

/** Returns true if a timezone is defined for this.  False otherwise.*/
bool ATGYearOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/** Sets the timezone to the given timezone.*/
ATGYearOrDerived::Ptr ATGYearOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(_YY->greaterThan(context->getPathanFactory()->createInteger(9999, context), context)) {
    buffer.set(_YY->asString(context));
  } else {
    buffer.set(((const ATDecimalOrDerived*)_YY)->asString(4, context)); //pad to 4 digits
  }
  if (hasTimezone) 
    buffer.append(timezone->asString(context));
  const XMLCh* gYearMonth = context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
  return context->getPathanFactory()->createGYearOrDerived(this->getTypeURI(), this->getTypeName(), gYearMonth, context);        
}


AnyAtomicType::AtomicObjectType ATGYearOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

/* parse the gYear */
void ATGYearOrDerivedImpl::setGYear(const XMLCh* const value, const DynamicContext* context) {
 
  unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(value);
  if(value == NULL) {
			DSLthrow(XPath2TypeCastException,X("ATGYearOrDerivedImpl::setGYear"), X("Invalid representation of gYear"));
	}
	
	// State variables etc.
	bool gotDigit = false;

	unsigned int pos = 0;
	long int tmpnum = 0;
	unsigned int numDigit = 0;
	bool negative = false;

	// defaulting values
	MAPM YY = 0;
  _hasTimezone = false;
	bool zonepos = false;
	int zonehh = 0;
	int zonemm = 0;

	int state = 0 ; // 0 = year / 1 = month / 2 = day / 3 = hour 
	                 // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
	XMLCh tmpChar;
	
	bool wrongformat = false;

	if ( length > 0 && value[0] == L'-'  ) {
	  negative = true;
		pos = 1;
	}else{
		pos = 0;
	} 
		
	while ( ! wrongformat && pos < length) {
		tmpChar = value[pos];
		pos++;
		switch(tmpChar) {
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
				numDigit ++;
				tmpnum *= 10;
				tmpnum +=  static_cast<int>(tmpChar - 0x0030);
				gotDigit = true;
				
				break;
			}
		case L'-' : {
			if ( gotDigit && state == 0 && numDigit >= 4) { 
					YY = tmpnum;
					tmpnum = 0;
					gotDigit = false;			
					_hasTimezone = true;
					zonepos = false;
					state = 6;
					numDigit = 0;
			} else {
				wrongformat = true;
			}
			break;			
		}
    case L'+' : {
			if ( gotDigit && state == 0 && numDigit >= 4) {
				YY = tmpnum;
				state = 6; 
				gotDigit = false;			
				_hasTimezone = true;
				zonepos = true;
				tmpnum = 0;
				numDigit = 0;
			} else {
				wrongformat = true;
			}
			break;
		}
		case L':' : {
			if (gotDigit && state == 6 && numDigit == 2 ) {
					zonehh = tmpnum;
					tmpnum = 0;
					gotDigit = false;
					numDigit = 0;
					state = 7;
			}else {
				wrongformat = true;
			}
			break;
		}
		case L'Z' : {
			if (gotDigit && state == 0 && numDigit >= 4) {
				YY = tmpnum;
				state = 8; // final state
				_hasTimezone = true;
				gotDigit = false;
				tmpnum = 0;
				numDigit = 0;
			} else {
				
				wrongformat = true;
			}
			break;
		}
		default:
			wrongformat = true;
		}	
	}

	if (gotDigit) {
		if ( gotDigit && state == 7 && numDigit == 2) {
			zonemm = tmpnum;
		}else if ( gotDigit && state == 0 && numDigit >= 4 ) {
				YY += tmpnum;			
		}else {
			wrongformat = true;
		}
	} 

	if (negative) {
	  YY = YY * -1;
	}

	if (YY == 0) {
	  wrongformat = true;
	}
	
	// check time format

	if (wrongformat) {
		DSLthrow(XPath2TypeCastException,X("ATGYearOrDerivedImpl::setGYear"), X("Invalid representation of gYear"));
	}

  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  timezone_ = new Timezone(zonehh, zonemm);

  _YY = context->getPathanFactory()->createInteger(YY, context);
  
}
