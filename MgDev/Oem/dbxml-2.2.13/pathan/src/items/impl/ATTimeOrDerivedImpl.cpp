/** Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATTimeOrDerivedImpl.hpp"
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/PathanFactory.hpp>

#include <limits.h>   // for INT_MIN and INT_MAX
#include <stdlib.h>   // for atoi
#include <assert.h>

#include "../../utils/DateUtils.hpp"
#include "../../utils/Date.hpp"

ATTimeOrDerivedImpl::
ATTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    ATTimeOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  setTime(value, context);    
}


// private constructor for internal use
ATTimeOrDerivedImpl::ATTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, 
    const ATDecimalOrDerived::Ptr &hh, const ATDecimalOrDerived::Ptr &mm, const ATDecimalOrDerived::Ptr &ss, 
    const Timezone::Ptr &timezone, bool hasTimezone) : 
    ATTimeOrDerived(),
    _hh(hh), _mm(mm), _ss(ss),
    timezone_(timezone), _hasTimezone(hasTimezone),
    _typeName(typeName),
    _typeURI(typeURI) {
}

void *ATTimeOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATTimeOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATTimeOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATTimeOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATTimeOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATTimeOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::TIME;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATTimeOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());

  switch (targetIndex) {
    case DATE_TIME: {
      const ATDateOrDerived::Ptr currentDate = DateUtils::getCurrentDate(context); 
      if(currentDate->getYears()->greaterThan(context->getPathanFactory()->createInteger(9999, context), context)) {
        buf.set(currentDate->getYears()->asString(context));
      } else {
        buf.set(currentDate->getYears()->asString(4, context)); //pad to 4 digits
      }
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(currentDate->getMonths()->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(currentDate->getDays()->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
      buf.append(this->asString(context));
      return context->getPathanFactory()->createDateTimeOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    }
    case ANY_SIMPLE_TYPE:
    case UNTYPED_ATOMIC:
      //anySimpleType and untypedAtomic follow the same casting rules as string.
    case STRING: {
      return context->getPathanFactory()->createDerivedFromAtomicType(targetURI, targetType, this->asString(context), context);
		}
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATTimeOrDerivedImpl::asString(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.append(_hh->asString(2, context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
  buffer.append(_mm->asString(2, context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
  if(_ss->asMAPM() < MM_Ten) { // TODO: deal with precision in a better way!
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
  }
  if (_ss->equals(_ss->floor(context), context)) {
    const ATDecimalOrDerived::Ptr int_ss = (const ATDecimalOrDerived::Ptr ) _ss->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context);
    buffer.append(int_ss->asString(context));
  } else {
    buffer.append(_ss->asString(context));  
  }

  // Add timezone if exists
  if (_hasTimezone) {
    buffer.append(timezone_->asString(context));
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

/* returns true if the two objects represent the same time,
 * false otherwise */
bool ATTimeOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATTimeOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  ATTimeOrDerived::Ptr mtc = this->normalize(context);
  const ATTimeOrDerivedImpl* myTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)mtc;
  ATTimeOrDerived::Ptr otc = ((const ATTimeOrDerived* )(const AnyAtomicType*)target)->normalize(context);
  const ATTimeOrDerivedImpl* otherTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)otc;

  return ( myTimeCopy->_hh->equals((const AnyAtomicType::Ptr)otherTimeCopy->_hh, context) &&
             myTimeCopy->_mm->equals((const AnyAtomicType::Ptr)otherTimeCopy->_mm, context) &&
             myTimeCopy->_ss->equals((const AnyAtomicType::Ptr)otherTimeCopy->_ss, context) ); 
}

/**
 * Returns true i and only if this time is greater than the given time. 
 */
bool ATTimeOrDerivedImpl::greaterThan(const ATTimeOrDerived::Ptr &other, const DynamicContext* context) const {
  
  ATTimeOrDerived::Ptr mtc = this->normalize(context);
  const ATTimeOrDerivedImpl* myTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)mtc;
  ATTimeOrDerived::Ptr otc = other->normalize(context);
  const ATTimeOrDerivedImpl* otherTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)otc;

  if ((myTimeCopy->_hh->greaterThan(otherTimeCopy->_hh, context)) || 

      (myTimeCopy->_hh->equals((const AnyAtomicType::Ptr)otherTimeCopy->_hh, context) && 
       myTimeCopy->_mm->greaterThan(otherTimeCopy->_mm, context)) || 

      (myTimeCopy->_hh->equals((const AnyAtomicType::Ptr)otherTimeCopy->_hh, context) && 
       myTimeCopy->_mm->equals((const AnyAtomicType::Ptr)otherTimeCopy->_mm, context) && 
       myTimeCopy->_ss->greaterThan(otherTimeCopy->_ss, context)) ) {
        return true;
  } else {
   return false;
  } 

}

/**
 * Returns true if and only if this time is less than the given time. 
 */
bool ATTimeOrDerivedImpl::lessThan(const ATTimeOrDerived::Ptr &other,  const DynamicContext* context) const {
  ATTimeOrDerived::Ptr mtc = this->normalize(context);
  const ATTimeOrDerivedImpl* myTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)mtc;
  ATTimeOrDerived::Ptr otc = other->normalize(context);
  const ATTimeOrDerivedImpl* otherTimeCopy = (ATTimeOrDerivedImpl*)(const ATTimeOrDerived*)otc;

  if ((myTimeCopy->_hh->lessThan(otherTimeCopy->_hh, context)) || 

      (myTimeCopy->_hh->equals((const AnyAtomicType::Ptr)otherTimeCopy->_hh, context) && 
       myTimeCopy->_mm->lessThan(otherTimeCopy->_mm, context)) || 

      (myTimeCopy->_hh->equals((const AnyAtomicType::Ptr)otherTimeCopy->_hh, context) && 
       myTimeCopy->_mm->equals((const AnyAtomicType::Ptr)otherTimeCopy->_mm, context) && 
       myTimeCopy->_ss->lessThan(otherTimeCopy->_ss, context)) ) {
        return true;
  } else {
   return false;
  } 

}

/** 
 * Returns an integer representing the hour component of this object
 */
const ATDecimalOrDerived::Ptr &ATTimeOrDerivedImpl::getHours() const {
  return _hh;
}

/** 
 * Returns an integer representing the minute component of this object
 */
const ATDecimalOrDerived::Ptr &ATTimeOrDerivedImpl::getMinutes() const {
 return _mm;
}

/** 
 * Returns an decimal representing the second component of this object
 */
const ATDecimalOrDerived::Ptr &ATTimeOrDerivedImpl::getSeconds() const {
  return _ss;
}

/**
 * Returns true if the timezone is defined for this object, false otherwise.
 */
bool ATTimeOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/**
 * Returns the timezone associated with this object, or
 * null, if the timezone is not set
 */
const Timezone::Ptr &ATTimeOrDerivedImpl::getTimezone() const {
  return timezone_;
}

/**
 * Setter for timezone.  Overrides the current timezone. (Not to be 
 * confused with addTimezone(). If passed NULL, timezone is removed (unset)
 */
ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  return new
    ATTimeOrDerivedImpl(this->getTypeURI(), 
                        this->getTypeName(), 
                        this->_hh, this->_mm, this->_ss,
                        timezone, hasTimezone);
}

/**
 * Returns an ATTimeOrDerived with a timezone added to it
 */
ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::addTimezone(const ATDurationOrDerived::Ptr &timezone, const DynamicContext* context) const {
  Timezone::Ptr tz = new Timezone(timezone, context);  

  // If this time does not have a timezone, add the given timezone
  if (!_hasTimezone) {
    ATTimeOrDerived::Ptr timeCopy = this->setTimezone(tz, context);
    return timeCopy;
  } else { //else convert the time into an equivalent one with given timezone
    // Minutes
    MAPM offset = tz->getMinutes()-timezone_->getMinutes();
    MAPM temp = ((const ATDecimalOrDerived*)this->_mm)->asMAPM() + offset;
    const ATDecimalOrDerived::Ptr mm = context->getPathanFactory()->createNonNegativeInteger(
                                                                                     DateUtils::modulo(temp, DateUtils::g_minutesPerHour),
                                                                                     context);
    MAPM carry = (temp / DateUtils::g_minutesPerHour).floor();
  
    // Hours
    offset = tz->getHours()-timezone_->getHours();
    temp = ((const ATDecimalOrDerived*)this->_hh)->asMAPM() + offset + carry;
    const ATDecimalOrDerived::Ptr hh = context->getPathanFactory()->createNonNegativeInteger(
                                                                                     DateUtils::modulo(temp, DateUtils::g_hoursPerDay),
                                                                                     context);
    
    return new
      ATTimeOrDerivedImpl(this->getTypeURI(), 
                              this->getTypeName(), 
                              hh, mm, this->_ss,
                              tz, 
                              true);
  }  

  return 0;
}

/**
 * Returns a time with the given dayTimeDuration added to it
 */
ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(((const ATDurationOrDerived*)dayTime)->isNegative()) {
    return subtractDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  } else {
    return addDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  }
}
  
ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::normalize(const DynamicContext* context) const {  

  Timezone::Ptr timezone;
  if (!_hasTimezone) {
    timezone = new Timezone(context->getImplicitTimezone(), context);
  } else {
    timezone = this->timezone_;
  }

  // Minutes
  MAPM tzMinutes = timezone->getMinutes();
  MAPM temp = ((const ATDecimalOrDerived*)this->_mm)->asMAPM() - tzMinutes;
  const ATDecimalOrDerived::Ptr mm = context->getPathanFactory()->createNonNegativeInteger(
                                                                                   DateUtils::modulo(temp, DateUtils::g_minutesPerHour),
                                                                                   context);

  MAPM carry = (temp / DateUtils::g_minutesPerHour).floor();
  
  // Hours
  MAPM tzHours = timezone->getHours();
  temp = ((const ATDecimalOrDerived*)this->_hh)->asMAPM() - tzHours + carry;
  const ATDecimalOrDerived::Ptr hh = context->getPathanFactory()->createNonNegativeInteger(
                                                                                   DateUtils::modulo(temp, DateUtils::g_hoursPerDay),
                                                                                   context);
  
  return new
    ATTimeOrDerivedImpl(this->getTypeURI(), 
                        this->getTypeName(), 
                        hh, mm, 
                        this->_ss,
                        new Timezone(0, 0), true // timezone set to UTC
                        );
}


/**
 * Returns a time with the given dayTimeDuration subtracted from it
 */
ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(((const ATDurationOrDerived*)dayTime)->isNegative()) {
    return addDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  } else {
    return subtractDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  }
  
}

ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::addDayTimeDuration(MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const {
  
  // Seconds
  MAPM temp = getSeconds()->asMAPM() + seconds;
  MAPM ss = DateUtils::modulo(temp, DateUtils::g_secondsPerMinute);
  MAPM carry = (temp/DateUtils::g_secondsPerMinute).floor();

  // Minutes
  temp = getMinutes()->asMAPM() + minutes + carry;
  MAPM mm = DateUtils::modulo(temp, DateUtils::g_minutesPerHour);
  carry = (temp/DateUtils::g_minutesPerHour).floor();

  // Hours
  temp = getHours()->asMAPM() + hours + carry;
  MAPM hh = DateUtils::modulo(temp, DateUtils::g_hoursPerDay);
  
  return new
    ATTimeOrDerivedImpl(_typeURI, 
                        _typeName, 
                        context->getPathanFactory()->createNonNegativeInteger(hh, context),
                        context->getPathanFactory()->createNonNegativeInteger(mm, context),
                        context->getPathanFactory()->createDecimal(ss, context),
                        getTimezone(), 
                        hasTimezone());
}

ATTimeOrDerived::Ptr ATTimeOrDerivedImpl::subtractDayTimeDuration(MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const {
  return this->addDayTimeDuration(hours.neg(), minutes.neg(), seconds.neg(), context);
}

/**
 * Returns the xdt:dayTimeDuration that corresponds to the difference between the value of $arg1 converted to an 
 * xs:dateTime using the date components from the reference xs:dateTime and the value of $arg2 converted to an xs:dateTime 
 * using the date components from the same reference xs:dateTime
 */
ATDurationOrDerived::Ptr ATTimeOrDerivedImpl::subtractTime(const ATTimeOrDerived::Ptr &time, const DynamicContext* context) const {
  
  const XMLCh s1972[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_7, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  const XMLCh s12[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  const XMLCh s31[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
  buf.set(s1972);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buf.append(s12);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buf.append(s31);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
  buf.append(this->asString(context));
  ATDateTimeOrDerived::Ptr thisTime=context->getPathanFactory()->createDateTime(buf.getRawBuffer(), context);

  buf.set(s1972);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buf.append(s12);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  buf.append(s31);
  buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
  buf.append(time->asString(context));
  ATDateTimeOrDerived::Ptr otherTime=context->getPathanFactory()->createDateTime(buf.getRawBuffer(), context);

  return thisTime->subtractDateTimeAsDayTimeDuration(otherTime, context);
}

AnyAtomicType::AtomicObjectType ATTimeOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

void ATTimeOrDerivedImpl::setTime(const XMLCh* const time, const DynamicContext* context) {
	unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(time);
 
	if(time == NULL) {
			DSLthrow(XPath2TypeCastException,X("XSTimeImpl::setTime"), X("Invalid representation of time"));
	}
	
	// State variables etc.
	bool gotDot = false;
	bool gotDigit = false;

	unsigned int pos = 0;
	long int tmpnum = 0;
	double decplace = 1;
	double tmpdec = 0;

	unsigned int numDigit = 0;

	// defaulting values
	MAPM hh = 0;
	MAPM mm = 0;
	MAPM ss = 0;
	_hasTimezone = false;
	bool zonepos = false;
	int zonehh = 0;
	int zonemm = 0;

	int state = 3 ;  // 3 = hour
	                 // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
	XMLCh tmpChar;
	
	bool wrongformat = false;
	pos = 0;

	while ( ! wrongformat && pos < length) {
		tmpChar = time[pos];
		pos++;
		switch(tmpChar) {
		  case L'.': {
				if (! gotDot && gotDigit && state == 5 && numDigit == 2) {
					gotDot = true;
					ss = tmpnum;
					gotDigit = false;
					tmpnum = 0;
					break;
				}
				wrongformat = true;										
				break;
		  }
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
				if ( gotDot && state == 5) {
					decplace *= 10;					
				} else {
					numDigit ++;
				}
				tmpnum *= 10;
				tmpnum +=  static_cast<int>(tmpChar - 0x0030);
				gotDigit = true;
				
				break;
			}
		case L'-' : {
			if ( gotDigit && state == 5 && numDigit == 2) {					
					tmpdec =  tmpnum / decplace;			
					ss += tmpdec;					
					gotDigit = false;	
					_hasTimezone = true;
					zonepos = false;
					_hasTimezone = true;
					tmpnum = 0;
					numDigit = 0;
			}else {
				
				wrongformat = true;
			}
			state ++;		
			break;			
		}
    case L'+' : {
			if ( gotDigit && state == 5 && numDigit == 2) {
				tmpdec =  tmpnum / decplace;			
				ss += tmpdec;	
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
			if (gotDigit ) {
				if (state == 3 && numDigit == 2) {
					hh = tmpnum;	
					tmpnum = 0;
					numDigit = 0;
					gotDigit = false;
				} else if ( state == 4 && numDigit == 2) {
					mm = tmpnum;
					tmpnum = 0;
					numDigit = 0;
					gotDigit = false;
				} else if ( state == 6 && numDigit == 2) {
					zonehh = tmpnum;
					_hasTimezone = true;
					tmpnum = 0;
					numDigit = 0;
					gotDigit = false;
				} else {
					wrongformat = true;
				}
				state ++;
			}else {
				wrongformat = true;
			}
			break;
		}
		case L'Z' : {
			if (gotDigit && state == 5 && numDigit == 2) {
				tmpdec =  tmpnum / decplace;			
				ss += tmpdec;	
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
			_hasTimezone = true;
		}else if ( gotDigit && state == 5 && numDigit == 2) {
			tmpdec =  tmpnum / decplace;			
			ss += tmpdec;			
		}else {
			wrongformat = true;
		}
	} 

  if ( hh > 24 || mm > 60 || ss >= 61 || zonehh > 24 || zonemm > 60 ) {
    wrongformat = true;
  }


	if ( wrongformat) {
		DSLthrow(XPath2TypeCastException,X("XSTimeImpl::setTime"), X("Invalid representation of time"));
	}
	
  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  
  timezone_ = new Timezone(zonehh, zonemm);
  
  _hh = context->getPathanFactory()->createNonNegativeInteger(hh, context);
  _mm = context->getPathanFactory()->createNonNegativeInteger(mm, context);
  _ss = context->getPathanFactory()->createDecimal(ss, context);
}
