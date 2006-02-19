/** Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATDateOrDerivedImpl.hpp"
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATGDayOrDerived.hpp>
#include <pathan/ATGMonthDayOrDerived.hpp>
#include <pathan/ATGMonthOrDerived.hpp>
#include <pathan/ATGYearMonthOrDerived.hpp>
#include <pathan/ATGYearOrDerived.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/PathanFactory.hpp>

#include <limits.h>   // for INT_MIN and INT_MAX
#include <stdlib.h>   // for atoi
#include <assert.h>

#include "../../utils/DateUtils.hpp"
#include "../../utils/Date.hpp"


ATDateOrDerivedImpl::
ATDateOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  setDate(value, context);    
}

// private constructor for internal use()
ATDateOrDerivedImpl::ATDateOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const ATDecimalOrDerived::Ptr &YY, const ATDecimalOrDerived::Ptr &MM, const ATDecimalOrDerived::Ptr &DD, const Timezone::Ptr &timezone, bool hasTimezone) : 
    _YY(YY), _MM(MM), _DD(DD),
    timezone_(timezone), _hasTimezone(hasTimezone),
    _typeName(typeName),
    _typeURI(typeURI) {
}

void *ATDateOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATDateOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATDateOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATDateOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATDateOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATDateOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::DATE;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATDateOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
  
  const XMLCh doubleZero[] = { XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

  switch (targetIndex) {
    case DATE_TIME: {
      if(_YY->asMAPM() > 9999) {
        buf.set(_YY->asString(context));
      } else {
        buf.set(_YY->asString(4, context)); //pad to 4 digits
      }
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_MM->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_DD->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
      buf.append(doubleZero);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
      buf.append(doubleZero);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
      buf.append(doubleZero);
      
      // Add timezone if exists
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createDateTimeOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    }
    case G_DAY: {
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_DD->asString(2, context));
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createGDayOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    }
    case G_MONTH_DAY: {
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_MM->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_DD->asString(2, context));
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createGMonthDayOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    } 
    case G_MONTH: {
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_MM->asString(2, context));
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createGMonthOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    } 
    case G_YEAR_MONTH: {
      if(_YY->asMAPM() > 9999) {
        buf.set(_YY->asString(context));
      } else {
        buf.set(_YY->asString(4, context)); //pad to 4 digits
      }
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_MM->asString(2, context));
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createGYearMonthOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    } 
    case G_YEAR: {
      if(_YY->asMAPM() > 9999) {
        buf.set(_YY->asString(context));
      } else {
        buf.set(_YY->asString(4, context)); //pad to 4 digits
      }
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createGYearOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
    } 
    case ANY_SIMPLE_TYPE:
    case UNTYPED_ATOMIC:
      //anySimpleType and untypedAtomic follow the same casting rules as string.
    case STRING: {
      return context->getPathanFactory()->createDerivedFromAtomicType(targetURI, targetType, asString(context), context);
		} 
    default: {
      return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
    }
  }  
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATDateOrDerivedImpl::asString(const DynamicContext* context) const {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
    if(_YY->asMAPM() > 9999 || _YY->asMAPM() < -9999) {
      buffer.set(_YY->asString(context));
    } else {
      buffer.set(_YY->asString(4, context)); //pad to 4 digits
    }
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(_MM->asString(2, context));
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(_DD->asString(2, context));

    // Add timezone if exists
    if (_hasTimezone) {
      buffer.append(timezone_->asString(context));
    }
    return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

/* returns true if the two objects represent the same date,
 * false otherwise */
bool ATDateOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATDateOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  const ATDateOrDerivedImpl* targetDate = (ATDateOrDerivedImpl*)(const AnyAtomicType*)target;
  if ( _hasTimezone == targetDate->_hasTimezone ) {  // must be in the same state
    return ( (!_hasTimezone || timezone_->equals(targetDate->timezone_))
          && _YY->equals(targetDate->_YY, context) &&
             _MM->equals(targetDate->_MM, context) &&
             _DD->equals(targetDate->_DD, context) ); 
  }
  else {
    return false;
  }
}

/**
 * Returns true if and only if the starting instant of $arg1 is greater than the starting instant of $arg2. 
 * Returns false otherwise.
 * The starting instant of an xs:date is the xs:dateTime at time 00:00:00 on that date.
 */
bool ATDateOrDerivedImpl::greaterThan(const ATDateOrDerived::Ptr &other, const DynamicContext* context) const {
  ATDateTimeOrDerived::Ptr myDateTime=castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  ATDateTimeOrDerived::Ptr otherDateTime=other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  return myDateTime->greaterThan(otherDateTime, context);
}

/**
 * Returns true if and only if the starting instant of $arg1 is less than the starting instant of $arg2. 
 * Returns false otherwise.
 * The starting instant of an xs:date is the xs:dateTime at time 00:00:00 on that date.
 */
bool ATDateOrDerivedImpl::lessThan(const ATDateOrDerived::Ptr &other,  const DynamicContext* context) const {
  ATDateTimeOrDerived::Ptr myDateTime=castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  ATDateTimeOrDerived::Ptr otherDateTime=other->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  return myDateTime->lessThan(otherDateTime, context);
}

/** 
 * Returns an integer representing the year component  of this object
 */
const ATDecimalOrDerived::Ptr &ATDateOrDerivedImpl::getYears() const {
  return _YY;
}

/** 
 * Returns an integer representing the month component  of this object
 */
const ATDecimalOrDerived::Ptr &ATDateOrDerivedImpl::getMonths() const {
 return _MM;
}

/** 
 * Returns an integer representing the day component  of this object
 */
const ATDecimalOrDerived::Ptr &ATDateOrDerivedImpl::getDays() const {
  return _DD;
}

/**
 * Returns the timezone associated with this object, or
 * null, if the timezone is not set
 */
const Timezone::Ptr &ATDateOrDerivedImpl::getTimezone() const {
  return timezone_;
}
  

/**
 * Returns true if the timezone is defined for this object, false otherwise.
 */
bool ATDateOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/**mrg@decisionsoft.com
 * Setter for timezone.  Overrides the current timezone. (Not to be 
 * confused with addTimezone(). If passed NULL, timezone is removed (unset)
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  return new
    ATDateOrDerivedImpl(this->_typeURI, 
                        this->_typeName, 
                        this->_YY, this->_MM, this->_DD, 
                        timezone, hasTimezone);
}

/**
 * Returns an ATDateOrDerived with a timezone added to it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::addTimezone(const ATDurationOrDerived::Ptr &timezone, const DynamicContext* context) const {
  // For purposes of timezone adjustment, an xs:date is treated as an xs:dateTime with time 00:00:00.
  ATDateTimeOrDerived::Ptr dateTime=castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                           context);
  ATDateTimeOrDerived::Ptr dateTimeTz=dateTime->addTimezone(timezone, context);
  return dateTimeTz->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE, 
                            context);
}

/**
 * Returns a date with the given yearMonthDuration added to it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::addYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth,  const DynamicContext* context) const {
  if(!yearMonth->isYearMonthDuration()) {
    DSLthrow(IllegalArgumentException,X("ATDurationOrDerivedImpl::addYearMonthDuration"), X("addYearMonthDuration for given type not supported"));
  }
  MAPM year=yearMonth->getYears()->asMAPM();
  MAPM month=yearMonth->getMonths()->asMAPM();
  if(yearMonth->isNegative()) {
    year=year.neg();
    month=month.neg();
  }
  return this->addYearMonthDuration(year, month, context);
}

/**
 * Returns a date with the given yearMonthDuration added to it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::addYearMonthDuration(MAPM years, MAPM months, const DynamicContext* context) const {
  MAPM totalMonths = getMonths()->asMAPM()+months-MM_One;
  
  MAPM MM = DateUtils::modulo(totalMonths, 12) + MM_One;
  MAPM carry = (totalMonths/12).floor();
  MAPM finalYears = carry + years + getYears()->asMAPM(); 
  
  assert(!getYears()->isZero());  // We should never have _YY = 0000

  MAPM YY;
  // Fix year 0000 problem
  if ( finalYears <= MM_Zero && getYears()->isPositive()) {
    YY = finalYears - MM_One;
  }
  else if (finalYears >= MM_Zero && getYears()->isNegative()) {
    YY = finalYears + MM_One;
  } else {
    YY = finalYears;
  }

  return new ATDateOrDerivedImpl(getTypeURI(), getTypeName(),
                                 context->getPathanFactory()->createInteger(YY, context),
                                 context->getPathanFactory()->createNonNegativeInteger(MM, context),
                                 _DD,
                                 getTimezone(), 
                                 hasTimezone());
}

/**
 * Returns a date with the given dayTimeDuration added to it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(!dayTime->isDayTimeDuration()) {
    DSLthrow(IllegalArgumentException,X("ATDateOrDerivedImpl::addDayTimeDuration"),  X("addDayTimeDuration for given type not supported"));
  }
  ATDateTimeOrDerived::Ptr myDateTime=castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  return myDateTime->addDayTimeDuration(dayTime, context)
                   ->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE, 
                            context);
}
  
/**
 * Returns a date with the given yearMonthDuration subtracted from it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::subtractYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth, const DynamicContext* context) const {
  if(!((const ATDurationOrDerived*)yearMonth)->isYearMonthDuration()) {
    DSLthrow(IllegalArgumentException,X("ATDurationOrDerivedImpl::subtractYearMonthDuration"), X("subtractYearMonthDuration for given type not supported")); 
  }
  
  MAPM year=((const ATDecimalOrDerived*)((const ATDurationOrDerived*)yearMonth)->getYears())->asMAPM();
  MAPM month=((const ATDecimalOrDerived*)((const ATDurationOrDerived*)yearMonth)->getMonths())->asMAPM();
  if(!((const ATDurationOrDerived*)yearMonth)->isNegative()) {
    year=year.neg();
    month=month.neg();
  }
  return this->addYearMonthDuration(year, month, context);
}

/**
 * Returns a date with the given dayTimeDuration subtracted from it
 */
ATDateOrDerived::Ptr ATDateOrDerivedImpl::subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(!((const ATDurationOrDerived*)dayTime)->isDayTimeDuration()) {
    DSLthrow(IllegalArgumentException,X("ATDateOrDerivedImpl::subtractDayTimeDuration"),  X("subtractDayTimeDuration for given type not supported"));
  }
  
  ATDateTimeOrDerived::Ptr myDateTime=castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, 
                                             context);
  return myDateTime->subtractDayTimeDuration(dayTime, context)
                   ->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                            XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE, 
                            context);
}

/**
 * Returns a dayTimeDuration corresponding to the difference between this
 * and the given ATDateOrDerived*
 */
ATDurationOrDerived::Ptr ATDateOrDerivedImpl::subtractDate(const ATDateOrDerived::Ptr &date, const DynamicContext* context) const {
  
  // normalize both dates first
  const ATDateOrDerived::Ptr thisDate = this->normalize(context);
  const ATDateOrDerived::Ptr otherDate = date->normalize(context);

  // this as julian
  Date dateThis = Date(asInt(thisDate->getDays()->asMAPM()),
                       asInt(thisDate->getMonths()->asMAPM()),
                       asInt(thisDate->getYears()->asMAPM()));
  // date as julian
  Date dateOther = Date(asInt(otherDate->getDays()->asMAPM()),
                        asInt(otherDate->getMonths()->asMAPM()),
                        asInt(otherDate->getYears()->asMAPM()));

  // return the difference as a dayTimeDuration
  // Returns the number of days
  long diff = dateThis - dateOther;
  ATDecimalOrDerived::Ptr dateDiff = (const ATDecimalOrDerived::Ptr )context->getPathanFactory()->createInteger(diff, context);
  bool isNegative = dateDiff->isNegative();
  ATDecimalOrDerived::Ptr endDiff = dateDiff;
  
  if(isNegative) {
    endDiff = (const ATDecimalOrDerived::Ptr )dateDiff->invert(context);
  }


  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(isNegative) {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  }
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P);
  buffer.append(endDiff->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D);

  return context->getPathanFactory()->createDayTimeDuration(buffer.getRawBuffer(), context);
}


AnyAtomicType::AtomicObjectType ATDateOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

ATDateOrDerived::Ptr ATDateOrDerivedImpl::normalize(const DynamicContext* context) const {
  Timezone::Ptr timezone;
  if (!_hasTimezone) {
    timezone = new Timezone(context->getImplicitTimezone(), context);
  } else {
    timezone = this->timezone_;
  }

  // Minutes
  MAPM temp = -timezone->getMinutes();
  MAPM carry = (temp / DateUtils::g_minutesPerHour).floor();

  // Hours
  temp =  -timezone->getHours() + carry;
  carry = (temp / DateUtils::g_hoursPerDay).floor();

  Date thisDate = Date(asInt(((const ATDecimalOrDerived*)_DD)->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)_MM)->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)_YY)->asMAPM()));
  Date sumDate = thisDate + asInt(carry);

  return new
    ATDateOrDerivedImpl(this->_typeURI, 
                        this->_typeName, 
                        context->getPathanFactory()->createInteger(sumDate.Year(), context),
                        context->getPathanFactory()->createNonNegativeInteger(sumDate.Month(), context),
                        context->getPathanFactory()->createNonNegativeInteger(sumDate.Day(), context),
                        new Timezone(0, 0), true  // timezone set to UTC 
                        );
}

//////////////////////////////////////
// Horrible Hack to make Dates      //
// work for now. Loss of Precision! //
//////////////////////////////////////
int ATDateOrDerivedImpl::asInt(MAPM num) const
{
  if(num < INT_MIN || num > INT_MAX) {
    DSLthrow(XPath2TypeCastException, X("ATDateOrDerivedImpl::asInt"), X("Invalid representation of an int"));
  } else {
    char out_string[256];
    num.toIntegerString(out_string);
    return atoi(out_string);
  }
}


void ATDateOrDerivedImpl::setDate(const XMLCh* const date, const DynamicContext* context) {
  unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(date);
 
  if(date == 0) {
      DSLthrow(XPath2TypeCastException,X("ATDateOrDerived::setDate"), X("Invalid representation of date"));
  }
  
  // State variables etc.
  bool gotDigit = false;

  unsigned int pos = 0;
  long int tmpnum = 0;
  unsigned int numDigit = 0;
  bool negative = false;

  // defaulting values
  MAPM YY = 1;
  MAPM MM = 0;
  MAPM DD = 0;
  _hasTimezone = false;
  bool zonepos = false;
  int zonehh = 0;
  int zonemm = 0;

  int state = 0 ; // 0 = year / 1 = month / 2 = day 
                  // 6 = timezonehour / 7 = timezonemin
  XMLCh tmpChar;
  
  bool wrongformat = false;

  if ( length > 0 && date[0] == L'-'  ) {
    negative = true;
    pos = 1;
  }else{
    pos = 0;
  } 
    
  while ( ! wrongformat && pos < length) {
    tmpChar = date[pos];
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
      if ( gotDigit ) {
        if (state == 0 && numDigit >= 4) { 
          YY = tmpnum;
          if (negative) {
            YY = YY * -1;
          }
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0 ;
        } else if (state == 1 && numDigit == 2) {
          MM = tmpnum;    
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0 ;
        } else if ( state == 2 && numDigit == 2) {          
          DD += tmpnum;          
          gotDigit = false;  
          zonepos = false;
          _hasTimezone = true;
          tmpnum = 0;
          state = 5;
          numDigit = 0 ;
        } else {
          wrongformat = true;
        }
        state ++;
      }
      break;      
    }
    case L':' : {
      if (gotDigit && state == 6 && numDigit == 2) {
        zonehh = tmpnum;
        tmpnum = 0;
        gotDigit = false;
        state ++;
        numDigit = 0 ;
      }else {
        wrongformat = true;
      }
      break;
    }
    case L'+' : {
      if ( gotDigit && state == 2 && numDigit == 2) {
        DD += tmpnum;  
        state = 6; 
        gotDigit = false;      
        zonepos = true;
        _hasTimezone = true;
        tmpnum = 0;
        numDigit = 0 ;
      } else {
        wrongformat = true;
      }
      break;
    }
    case L'Z' : {
      if (gotDigit && state == 2 && numDigit == 2) {
        DD += tmpnum;  
        state = 8; // final state
        _hasTimezone = true;
        gotDigit = false;
        tmpnum = 0;
        numDigit = 0 ;
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
    }else if ( gotDigit && state == 2 && numDigit == 2) {
      DD += tmpnum;      
    }else {
      wrongformat = true;
    }
  } 

  // Verify date
  if ( MM > 12 || YY == 0 || zonehh > 24 || zonemm > 59 ) 
  {
    wrongformat = true;
  }

  else if ( DD > 28)
  {
    bool leapyear = false;

    // the mod operator on mapm is called rem
    if ( YY.rem(400) == 0 || ( (YY.rem(100) != 0 ) && (YY.rem(4) == 0 ) ) ) 
      leapyear = true;

    if ( (MM == 2 && leapyear && DD > 29 ) || (MM == 2 && !leapyear && DD > 28 ) ||
      ( ( MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10 || MM ==12 ) && DD > 31 ) ||
      ( (MM == 4 || MM == 6 || MM == 9 || MM == 11) && DD > 30 ) ||
    zonehh > 24 || zonemm > 60 || YY == 0) 
      {
        wrongformat = true;
      }
  }

  if ( wrongformat) 
  {
    DSLthrow(XPath2TypeCastException,X("ATDateOrDerivedImpl::setDate"), X("Invalid representation of date"));
  }

  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  timezone_ = new Timezone(zonehh, zonemm);

  _DD = context->getPathanFactory()->createNonNegativeInteger(DD, context);
  _MM = context->getPathanFactory()->createNonNegativeInteger(MM, context);
  _YY = context->getPathanFactory()->createInteger(YY, context);
}

