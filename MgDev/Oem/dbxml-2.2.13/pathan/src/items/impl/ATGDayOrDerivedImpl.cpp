/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATGDayOrDerivedImpl.hpp"
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
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <xercesc/util/XMLString.hpp>

ATGDayOrDerivedImpl::
ATGDayOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    ATGDayOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
     setGDay(value, context); 
}

void *ATGDayOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATGDayOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATGDayOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATGDayOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATGDayOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATGDayOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::G_DAY;
} 

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATGDayOrDerivedImpl::asString(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(_gDay->asString(2, context));
  if ( _hasTimezone == true ) {
    buffer.append(timezone_->asString(context));
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

/* Returns true if and only if the xs:dateTimes representing the starting instants of equivalent occurrences of $arg1 and $arg2 
 * compare equal. The starting instants of equivalent occurrences of $arg1 and $arg2 are calculated by adding the missing 
 * components of $arg1 and $arg2 from an xs:dateTime template such as 1972-12-xxT00:00:00. Returns false otherwise.
 */
bool ATGDayOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
        DSLthrow(IllegalArgumentException,X("ATGDayOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  ATGDayOrDerivedImpl* targetGDay = (ATGDayOrDerivedImpl*)(const AnyAtomicType*)target;

    const XMLCh s1972[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_7, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    const XMLCh s12[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    const XMLCh doubleZero[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
    buffer.set(s1972);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(s12);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(_gDay->asString(2, context));
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    if ( _hasTimezone == true ) {
        buffer.append(timezone_->asString(context));
  }
    ATDateTimeOrDerived::Ptr myValue=context->getPathanFactory()->createDateTime(buffer.getRawBuffer(), context);

    buffer.set(s1972);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(s12);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(targetGDay->_gDay->asString(2, context));
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    if ( targetGDay->_hasTimezone == true ) {
        buffer.append(targetGDay->timezone_->asString(context));
  }
    ATDateTimeOrDerived::Ptr targetValue=context->getPathanFactory()->createDateTime(buffer.getRawBuffer(), context);

    return myValue->equals(targetValue, context);
}

/** Returns true if this is greater than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGDayOrDerivedImpl::greaterThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGDayOrDerived::Ptr thisNorm = this;
  ATGDayOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGDayOrDerivedImpl* thisImpl=(ATGDayOrDerivedImpl*)(const ATGDayOrDerived*)thisNorm;
  ATGDayOrDerivedImpl* otherImpl=(ATGDayOrDerivedImpl*)(const ATGDayOrDerived*)otherNorm;
  return (thisImpl->_gDay->greaterThan(otherImpl->_gDay, context) ||
         (thisImpl->_gDay->equals(otherImpl->_gDay, context) && thisImpl->timezone_->greaterThan(otherImpl->timezone_)));
}

/** Returns true if this is less than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGDayOrDerivedImpl::lessThan(const ATGDayOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGDayOrDerived::Ptr thisNorm = this;
  ATGDayOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGDayOrDerivedImpl* thisImpl=(ATGDayOrDerivedImpl*)(const ATGDayOrDerived*)thisNorm;
  ATGDayOrDerivedImpl* otherImpl=(ATGDayOrDerivedImpl*)(const ATGDayOrDerived*)otherNorm;
  return (thisImpl->_gDay->lessThan(otherImpl->_gDay, context) ||
         (thisImpl->_gDay->equals(otherImpl->_gDay, context) && thisImpl->timezone_->lessThan(otherImpl->timezone_)));
}

/** Returns true if a timezone is defined for this.  False otherwise.*/
bool ATGDayOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/** Sets the timezone to the given timezone.*/
ATGDayOrDerived::Ptr ATGDayOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(_gDay->asString(2, context));
  if (hasTimezone) 
    buffer.append(timezone->asString(context));
  const XMLCh* gDay = context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
  return context->getPathanFactory()->createGDayOrDerived(this->getTypeURI(), this->getTypeName(), gDay, context);        
}


AnyAtomicType::AtomicObjectType ATGDayOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

/* parse the gDay */
void ATGDayOrDerivedImpl::setGDay(const XMLCh* const value, const DynamicContext* context) {
 
	if(value == NULL) {
			DSLthrow(XPath2TypeCastException,X("ATGDayOrDerivedImpl::setGDay"), 
          X("Invalid representation of gDay"));
	}
  unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(value);
	
	// State variables etc.
	bool gotDigit = false;
	unsigned int pos = 0;
	long int tmpnum = 0;

	// defaulting values
	MAPM DD = 0;
	_hasTimezone = false;
	bool zonepos = false;
	int zonehh = 0;
	int zonemm = 0;

	int state = 0 ; // 0 = year / 1 = month / 2 = day / 3 = hour 
	                 // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
	XMLCh tmpChar;
	
	bool wrongformat = false;

	if ( length < 5 || value[0] != L'-' || value[1] != L'-' || value[2] != L'-') {
		wrongformat = true;
	}else{
		pos = 3;
		state = 1;
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
			tmpnum *= 10;
			tmpnum +=  static_cast<int>(tmpChar - 0x0030);
			gotDigit = true;
			
			break;
		}
		case L':' : {
			if (gotDigit &&  state == 6 ) {
				zonehh = tmpnum;
				tmpnum = 0;
				gotDigit = false;				
				state ++;
			}else {
				wrongformat = true;
			}
			break;
		}		
		case L'-' : {
			if ( gotDigit && state == 1 ) {
				DD = tmpnum;		
				state = 6;
				gotDigit = false;			
				_hasTimezone = true;
				zonepos = false;
				tmpnum = 0;
			} else {
				wrongformat = true;
			}			
			break;			
		}
    case L'+' : {
			if ( gotDigit && state == 1 ) {
				DD = tmpnum;
				state = 6; 
				gotDigit = false;			
				_hasTimezone = true;
				zonepos = true;
				tmpnum = 0;
			} else {
				wrongformat = true;
			}
			break;
		}
		case L'Z' : {
			if (gotDigit && state == 1 ) {
				DD = tmpnum;
				state = 8; // final state
				_hasTimezone = true;
				gotDigit = false;
				tmpnum = 0;
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
		if ( gotDigit && state == 7 ) {
			zonemm = tmpnum;
		}else if ( gotDigit && state == 1 ) {
			DD = tmpnum;
		}else {
			wrongformat = true;
		}
	} 
	
	// check time format

	if ( DD > 31  || zonehh > 24 || zonemm > 60 ) {
			wrongformat = true;
	}

	if ( wrongformat) {
		DSLthrow(XPath2TypeCastException,X("ATGDayOrDerivedImpl::setGDay"), 
        X("Invalid representation of gDay"));
	}

  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  timezone_ = new Timezone(zonehh, zonemm);
  _gDay = context->getPathanFactory()->createNonNegativeInteger(DD, context);
  
}
