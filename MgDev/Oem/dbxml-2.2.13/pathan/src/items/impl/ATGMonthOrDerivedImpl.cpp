/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATGMonthOrDerivedImpl.hpp"
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

ATGMonthOrDerivedImpl::
ATGMonthOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    ATGMonthOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
     setGMonth(value, context); 
}

void *ATGMonthOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATGMonthOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATGMonthOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATGMonthOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATGMonthOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATGMonthOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::G_MONTH;
} 

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATGMonthOrDerivedImpl::asString(const DynamicContext* context) const {
  /* --MM format*/
  
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(_gMonth->asString(2, context));
  if ( _hasTimezone == true ) {
    buffer.append(timezone_->asString(context));
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());

}

/* Returns true if and only if the xs:dateTimes representing the starting instants of equivalent occurrences of $arg1 and $arg2 
 * compare equal. The starting instants of equivalent occurrences of $arg1 and $arg2 are calculated by adding the missing components 
 * of $arg1 and $arg2 from an xs:dateTime template such as 1972-xx-ddT00:00:00 where dd represents the last day of the month 
 * component in $arg1 or $arg2. Returns false otherwise.
 */
bool ATGMonthOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
        DSLthrow(IllegalArgumentException,X("ATGMonthOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  ATGMonthOrDerivedImpl* targetGMonth = (ATGMonthOrDerivedImpl*)(const AnyAtomicType*)target;
  
    const XMLCh s1972[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_7, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    const XMLCh doubleZero[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
    buffer.set(s1972);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(_gMonth->asString(2, context));
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    DateUtils::formatNumber(DateUtils::maximumDayInMonthFor(1972, asInt(_gMonth->asMAPM())),2,buffer);
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
    buffer.append(targetGMonth->_gMonth->asString(2, context));
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    DateUtils::formatNumber(DateUtils::maximumDayInMonthFor(1972, asInt(targetGMonth->_gMonth->asMAPM())),2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(doubleZero);
    if ( targetGMonth->_hasTimezone == true ) {
        buffer.append(targetGMonth->timezone_->asString(context));
  }
    ATDateTimeOrDerived::Ptr targetValue=context->getPathanFactory()->createDateTime(buffer.getRawBuffer(), context);

    return myValue->equals(targetValue, context);
}

/** Returns true if this is greater than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGMonthOrDerivedImpl::greaterThan(const ATGMonthOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGMonthOrDerived::Ptr thisNorm = this;
  ATGMonthOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGMonthOrDerivedImpl* thisImpl=(ATGMonthOrDerivedImpl*)(const ATGMonthOrDerived*)thisNorm;
  ATGMonthOrDerivedImpl* otherImpl=(ATGMonthOrDerivedImpl*)(const ATGMonthOrDerived*)otherNorm;
  return (thisImpl->_gMonth->greaterThan(otherImpl->_gMonth, context) ||
         (thisImpl->_gMonth->equals(otherImpl->_gMonth, context) && thisImpl->timezone_->greaterThan(otherImpl->timezone_)));
}

/** Returns true if this is less than other.  Ignores timezones.
 * Returns false otherwise. */
bool ATGMonthOrDerivedImpl::lessThan(const ATGMonthOrDerived::Ptr &other, const DynamicContext* context) const {
  ATGMonthOrDerived::Ptr thisNorm = this;
  ATGMonthOrDerived::Ptr otherNorm = other;
  if (!thisNorm->hasTimezone())
    thisNorm = thisNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  if (!otherNorm->hasTimezone())
    otherNorm = otherNorm->setTimezone(new Timezone(context->getImplicitTimezone(), context), context);
  ATGMonthOrDerivedImpl* thisImpl=(ATGMonthOrDerivedImpl*)(const ATGMonthOrDerived*)thisNorm;
  ATGMonthOrDerivedImpl* otherImpl=(ATGMonthOrDerivedImpl*)(const ATGMonthOrDerived*)otherNorm;
  return (thisImpl->_gMonth->lessThan(otherImpl->_gMonth, context) ||
         (thisImpl->_gMonth->equals(otherImpl->_gMonth, context) && thisImpl->timezone_->lessThan(otherImpl->timezone_)));
}

/** Returns true if a timezone is defined for this.  False otherwise.*/
bool ATGMonthOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/** Sets the timezone to the given timezone.*/
ATGMonthOrDerived::Ptr ATGMonthOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buffer.append(_gMonth->asString(2, context));
  if (hasTimezone) 
     buffer.append(timezone->asString(context));
  const XMLCh* gMonth = context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
  return context->getPathanFactory()->createGMonthOrDerived(this->getTypeURI(), this->getTypeName(), gMonth, context);        
}


AnyAtomicType::AtomicObjectType ATGMonthOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

/* parse the gMonth */
void ATGMonthOrDerivedImpl::setGMonth(const XMLCh* const value, const DynamicContext* context) {
 
   unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(value);
 
  if(value == NULL) {
    DSLthrow(XPath2TypeCastException,X("XSGMonthImpl::setGMonth"), X("Invalid representation of gMonth"));
  }
	
	// State variables etc.

	bool gotDigit = false;
	unsigned int pos = 0;
	long int tmpnum = 0;
	unsigned int numDigit = 0;

	// defaulting values
	MAPM MM = 0;
	_hasTimezone = false;
	bool zonepos = false;
	int zonehh = 0;
	int zonemm = 0;

	int state = 0 ; // 0 = year / 1 = month / 2 = day / 3 = hour 
	                 // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
	XMLCh tmpChar;
	
	bool wrongformat = false;

	if ( length  < 4 || value[0] != L'-' || value[1] != L'-' ) {
		wrongformat = true;
	}else{
		pos = 2;
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
        numDigit ++;
        break;
	}

      case L'-' : {
        if ( gotDigit && state == 1 && numDigit == 2) {
          MM = tmpnum;		
          tmpnum = 0;
          gotDigit = false;
          state = 6;
          numDigit = 0;
          _hasTimezone = true;
          zonepos = false;				
        } else {
          wrongformat = true;
        }
        break;			
      }
      case L'+' : {
        if ( gotDigit && state == 1 && numDigit == 2) {
          MM = tmpnum;                      
          tmpnum = 0;
          gotDigit = false;
          state = 6;
          numDigit = 0;
          _hasTimezone = true;
          zonepos = true;        
        } else {
          wrongformat = true;
        }
    	break;
      }
		case L':' : {
			if (gotDigit &&  state == 6 && numDigit == 2) {
				zonehh = tmpnum;
				tmpnum = 0;
				gotDigit = false;				
				state ++;
				numDigit = 0;
	            state = 7;
			}else {
				wrongformat = true;
			}
			break;
		}
		case L'Z' : {
            if ( gotDigit && state == 1 && numDigit == 2) {
				state = 8; // final state
                MM = tmpnum;                      
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
        if ( state == 1 && numDigit == 2)
            MM = tmpnum;                      
		else if( state == 7 && numDigit == 2)
			zonemm = tmpnum;
		else
			wrongformat = true;
	} 
	
	// check time format

	if ( MM > 12 || zonehh > 24 || zonemm > 60 ) {
			wrongformat = true;
	}

	if ( wrongformat) {
		DSLthrow(XPath2TypeCastException,X("ATGMonthOrDerivedImpl::setGMonth"), X("Invalid representation of gMonth"));
	}

  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  _gMonth = context->getPathanFactory()->createNonNegativeInteger(MM, context);
  timezone_ = new Timezone(zonehh, zonemm);
  
}

//////////////////////////////////////
// Horrible Hack to make Dates      //
// work for now. Loss of Precision! //
//////////////////////////////////////
int ATGMonthOrDerivedImpl::asInt(MAPM num) const
{
  if(num < INT_MIN || num > INT_MAX) {
    DSLthrow(XPath2TypeCastException, X("ATGMonthOrDerivedImpl::asInt"), X("Invalid representation of an int"));
  } else {
    char out_string[256];
    num.toIntegerString(out_string);
    return atoi(out_string);
  }
}
