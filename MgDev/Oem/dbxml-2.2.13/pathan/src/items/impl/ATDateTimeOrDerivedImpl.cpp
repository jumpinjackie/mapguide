/** Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATDateTimeOrDerivedImpl.hpp"
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATGDayOrDerived.hpp>
#include <pathan/ATGMonthDayOrDerived.hpp>
#include <pathan/ATGMonthOrDerived.hpp>
#include <pathan/ATGYearMonthOrDerived.hpp>
#include <pathan/ATGYearOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/Timezone.hpp>
#include <pathan/PathanFactory.hpp>

#include <limits.h>   // for INT_MIN and INT_MAX
#include <stdlib.h>   // for atoi
#include <assert.h>

#include "../../utils/DateUtils.hpp"
#include "../../utils/Date.hpp"

ATDateTimeOrDerivedImpl::
ATDateTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context): 
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  setDateTime(value, context);
}

// private constructor for internal use
ATDateTimeOrDerivedImpl::ATDateTimeOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, 
    const ATDecimalOrDerived::Ptr &YY, const ATDecimalOrDerived::Ptr &MM, const ATDecimalOrDerived::Ptr &DD, 
    const ATDecimalOrDerived::Ptr &hh, const ATDecimalOrDerived::Ptr &mm, const ATDecimalOrDerived::Ptr &ss, 
    const Timezone::Ptr &timezone, bool hasTimezone) : 
    _YY(YY), _MM(MM), _DD(DD),
    _hh(hh), _mm(mm), _ss(ss),
    timezone_(timezone), _hasTimezone(hasTimezone),
    _typeName(typeName),
    _typeURI(typeURI) {
}

void *ATDateTimeOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATDateTimeOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATDateTimeOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATDateTimeOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATDateTimeOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATDateTimeOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::DATE_TIME;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATDateTimeOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, context->getMemoryManager());
  
  switch (targetIndex) {
    case DATE: {
      if(_YY->asMAPM() > 9999) {
        buf.set(_YY->asString(context));
      } else {
        buf.set(_YY->asString(4, context)); //pad to 4 digits
      }
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_MM->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
      buf.append(_DD->asString(2, context));
      // Add timezone if exists
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createDateOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
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
    case TIME: {
      buf.append(_hh->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
      buf.append(_mm->asString(2, context));
      buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
      if(_ss->lessThan(context->getPathanFactory()->createDecimal(10, context), context)) {
        buf.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
      }
      buf.append(_ss->asString(context));
      if (_hasTimezone) {
        buf.append(timezone_->asString(context));
      }
      return context->getPathanFactory()->createTimeOrDerived(targetURI, targetType, buf.getRawBuffer(), context);
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
const XMLCh* ATDateTimeOrDerivedImpl::asString(const DynamicContext* context) const {
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
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
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

/* returns true if the two objects represent the same date,
 * false otherwise */
bool ATDateTimeOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATDateTimeOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  ATDateTimeOrDerived::Ptr myDateTimeCopy = this;
  ATDateTimeOrDerived::Ptr otherDateTimeCopy = (const ATDateTimeOrDerived::Ptr )target;
  if(myDateTimeCopy->hasTimezone() || otherDateTimeCopy->hasTimezone())
  {
    myDateTimeCopy = myDateTimeCopy->normalize(context);
    otherDateTimeCopy = otherDateTimeCopy->normalize(context);
  }
  return (myDateTimeCopy->getYears()->equals(otherDateTimeCopy->getYears(), context) &&
          myDateTimeCopy->getMonths()->equals(otherDateTimeCopy->getMonths(), context) &&
          myDateTimeCopy->getDays()->equals(otherDateTimeCopy->getDays(), context) &&
          myDateTimeCopy->getHours()->equals(otherDateTimeCopy->getHours(), context) &&
          myDateTimeCopy->getMinutes()->equals(otherDateTimeCopy->getMinutes(), context) &&
          myDateTimeCopy->getSeconds()->equals(otherDateTimeCopy->getSeconds(), context) );
}

/**
 * Returns true if and only if this date is greater than the given date. 
 * The order relation on date values is the order relation on their 
 * starting instants.
 */
bool ATDateTimeOrDerivedImpl::greaterThan(const ATDateTimeOrDerived::Ptr &other, const DynamicContext* context) const {
  
  ATDateTimeOrDerived::Ptr myDateTimeCopy = this;
  ATDateTimeOrDerived::Ptr otherDateTimeCopy = other;
  if(myDateTimeCopy->hasTimezone() || otherDateTimeCopy->hasTimezone())
  {
    myDateTimeCopy = myDateTimeCopy->normalize(context);
    otherDateTimeCopy = otherDateTimeCopy->normalize(context);
  }

  if(myDateTimeCopy->getYears()->greaterThan(otherDateTimeCopy->getYears(), context))
      return true;
  else if(myDateTimeCopy->getYears()->lessThan(otherDateTimeCopy->getYears(), context))
      return false;
    
  if(myDateTimeCopy->getMonths()->greaterThan(otherDateTimeCopy->getMonths(), context))
      return true;
  else if(myDateTimeCopy->getMonths()->lessThan(otherDateTimeCopy->getMonths(), context))
      return false;

  if(myDateTimeCopy->getDays()->greaterThan(otherDateTimeCopy->getDays(), context))
      return true;
  else if(myDateTimeCopy->getDays()->lessThan(otherDateTimeCopy->getDays(), context))
      return false;

  if(myDateTimeCopy->getHours()->greaterThan(otherDateTimeCopy->getHours(), context))
      return true;
  else if(myDateTimeCopy->getHours()->lessThan(otherDateTimeCopy->getHours(), context))
      return false;

  if(myDateTimeCopy->getMinutes()->greaterThan(otherDateTimeCopy->getMinutes(), context))
      return true;
  else if(myDateTimeCopy->getMinutes()->lessThan(otherDateTimeCopy->getMinutes(), context)) 
      return false;

  return myDateTimeCopy->getSeconds()->greaterThan(otherDateTimeCopy->getSeconds(), context);
}

/**
 * Returns true if and only if this date is less than the given date. 
 * The order relation on date values is the order relation on their
 * starting instants.
 */
bool ATDateTimeOrDerivedImpl::lessThan(const ATDateTimeOrDerived::Ptr &other,  const DynamicContext* context) const {

  ATDateTimeOrDerived::Ptr myDateTimeCopy = this;
  ATDateTimeOrDerived::Ptr otherDateTimeCopy = other;
  if(myDateTimeCopy->hasTimezone() || otherDateTimeCopy->hasTimezone())
  {
    myDateTimeCopy = myDateTimeCopy->normalize(context);
    otherDateTimeCopy = otherDateTimeCopy->normalize(context);
  }

  if(myDateTimeCopy->getYears()->greaterThan(otherDateTimeCopy->getYears(), context))
      return false;
  else if(myDateTimeCopy->getYears()->lessThan(otherDateTimeCopy->getYears(), context))
      return true;

  if(myDateTimeCopy->getMonths()->greaterThan(otherDateTimeCopy->getMonths(), context))
      return false;
  else if(myDateTimeCopy->getMonths()->lessThan(otherDateTimeCopy->getMonths(), context))
      return true;

  if(myDateTimeCopy->getDays()->greaterThan(otherDateTimeCopy->getDays(), context))
      return false;
  else if(myDateTimeCopy->getDays()->lessThan(otherDateTimeCopy->getDays(), context))
      return true;

  if(myDateTimeCopy->getHours()->greaterThan(otherDateTimeCopy->getHours(), context))
      return false;
  else if(myDateTimeCopy->getHours()->lessThan(otherDateTimeCopy->getHours(), context))
      return true;

  if(myDateTimeCopy->getMinutes()->greaterThan(otherDateTimeCopy->getMinutes(), context))
      return false;
  else if(myDateTimeCopy->getMinutes()->lessThan(otherDateTimeCopy->getMinutes(), context))
      return true;

  return myDateTimeCopy->getSeconds()->lessThan(otherDateTimeCopy->getSeconds(), context);
}

/** 
 * Returns an integer representing the year component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getYears() const {
  return _YY;
}

/** 
 * Returns an integer representing the month component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getMonths() const {
 return _MM;
}

/** 
 * Returns an integer representing the day component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getDays() const {
  return _DD;
}

/** 
 * Returns an integer representing the hour component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getHours() const {
  return _hh;
}

/** 
 * Returns an integer representing the minute component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getMinutes() const {
 return _mm;
}

/** 
 * Returns an decimal representing the second component of this object
 */
const ATDecimalOrDerived::Ptr &ATDateTimeOrDerivedImpl::getSeconds() const {
  return _ss;
}

/**
 * Returns the timezone associated with this object, or
 * null, if the timezone is not set
 */
const Timezone::Ptr &ATDateTimeOrDerivedImpl::getTimezone() const {
  return timezone_;
}
  
/**
 * Returns true if the timezone is defined for this object, false otherwise.
 */
bool ATDateTimeOrDerivedImpl::hasTimezone() const {
  return _hasTimezone;
}

/**
 * Setter for timezone.  Overrides the current timezone. (Not to be 
 * confused with addTimezone().  If passed null, timezone is removed (unset)
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const {
  bool hasTimezone = timezone == NULLRCP ? false : true;
  return new
    ATDateTimeOrDerivedImpl(this->_typeURI, 
                        this->_typeName, 
                        this->_YY, this->_MM, this->_DD, 
                        this->_hh, this->_mm, this->_ss, 
                        timezone, hasTimezone);
}

/**
 * Returns an ATDateTimeOrDerived with a timezone added to it
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::addTimezone(const ATDurationOrDerived::Ptr &timezone, const DynamicContext* context) const {
  
  Timezone::Ptr tz = new Timezone(timezone, context);  

  // If this dateTime does not have a timezone, add the given timezone
  if (!_hasTimezone) {
    return this->setTimezone(tz, context);
  } else { //else convert the dateTime into an equivalent one with given timezone
    // Minutes
    MAPM offset = tz->getMinutes()-timezone_->getMinutes();
    MAPM temp = ((const ATDecimalOrDerived*)_mm)->asMAPM()+offset;
    MAPM mm = DateUtils::modulo(temp, DateUtils::g_minutesPerHour);
    MAPM carry = (temp / DateUtils::g_minutesPerHour).floor();
  
    // Hours
    offset = tz->getHours()-timezone_->getHours();
    temp = ((const ATDecimalOrDerived*)_hh)->asMAPM()+offset+carry;
    MAPM hh = DateUtils::modulo(temp, DateUtils::g_hoursPerDay);
    carry = (temp / DateUtils::g_hoursPerDay).floor();
    
    Date thisDate = Date(asInt(_DD->asMAPM()),
                         asInt(_MM->asMAPM()),
                         asInt(_YY->asMAPM()));
    Date sumDate = thisDate + asInt(carry);
    
    return new
      ATDateTimeOrDerivedImpl(this->getTypeURI(), 
                              this->getTypeName(), 
                              context->getPathanFactory()->createInteger(sumDate.Year(), context),
                              context->getPathanFactory()->createNonNegativeInteger(sumDate.Month(), context),
                              context->getPathanFactory()->createNonNegativeInteger(sumDate.Day(), context),
                              context->getPathanFactory()->createNonNegativeInteger(hh, context),
                              context->getPathanFactory()->createNonNegativeInteger(mm, context),
                              _ss, 
                              tz, 
                              true);
  }  

}

/**
 * Returns a date with the given yearMonthDuration added to it
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::addYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth,  const DynamicContext* context) const {
  MAPM year=((const ATDecimalOrDerived*)((const ATDurationOrDerived*)yearMonth)->getYears())->asMAPM();
  MAPM month=((const ATDecimalOrDerived*)((const ATDurationOrDerived*)yearMonth)->getMonths())->asMAPM();
  if(((const ATDurationOrDerived*)yearMonth)->isNegative()) {
    year=year.neg();
    month=month.neg();
  }
  return addYearMonthDuration(year, month, context);
}

/**
 * Returns a date with the given yearMonthDuration added to it
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::addYearMonthDuration(MAPM years, MAPM months, const DynamicContext* context) const {
  MAPM totalMonths = getMonths()->asMAPM()+months-MM_One;
  
  MAPM MM = DateUtils::modulo(totalMonths, 12)+MM_One;
  MAPM carry = (totalMonths / 12).floor();
  MAPM finalYears = carry + years + getYears()->asMAPM(); 
  
  assert(!getYears()->isZero());  // We should never have _YY = 0000

  MAPM YY;
  // Fix year 0000 problem
  if ( finalYears <= MM_Zero && getYears()->isPositive()) {
    YY = finalYears - MM_One;
  }
  else if (finalYears >= MM_Zero && getYears()->isNegative()) {
    YY = finalYears+ MM_One;
  } else {
    YY = finalYears;
  }
  
  return new
    ATDateTimeOrDerivedImpl(getTypeURI(), 
                            getTypeName(), 
                            context->getPathanFactory()->createInteger(YY, context),
                            context->getPathanFactory()->createNonNegativeInteger(MM, context),
                            _DD, 
                            getHours(),
                            getMinutes(),
                            getSeconds(),
                            getTimezone(), 
                            hasTimezone());
}

/**
 * Returns a date with the given dayTimeDuration added to it
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::addDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(((const ATDurationOrDerived*)dayTime)->isNegative()) {
    return subtractDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getDays())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  } else {
    return addDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getDays())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  }
}
  
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::normalize(const DynamicContext* context) const {  

  Timezone::Ptr timezone;
  if (!_hasTimezone) {
    timezone = new Timezone(context->getImplicitTimezone(), context);
  } else {
    timezone = this->timezone_;
  }

  // Minutes
  MAPM tzMinutes = timezone->getMinutes();
  MAPM temp = ((const ATDecimalOrDerived*)_mm)->asMAPM() - tzMinutes;
  MAPM mm = DateUtils::modulo(temp, DateUtils::g_minutesPerHour);
  MAPM carry = (temp / DateUtils::g_minutesPerHour).floor();
  
  // Hours
  MAPM tzHours = timezone->getHours();
  temp = ((const ATDecimalOrDerived*)_hh)->asMAPM() - tzHours + carry;
  MAPM hh = DateUtils::modulo(temp, DateUtils::g_hoursPerDay);
  carry = (temp / DateUtils::g_hoursPerDay).floor();
  
  Date thisDate = Date(asInt(((const ATDecimalOrDerived*)_DD)->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)_MM)->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)_YY)->asMAPM()));
  Date sumDate = thisDate + asInt(carry);
  
  return new
    ATDateTimeOrDerivedImpl(this->getTypeURI(), 
                        this->getTypeName(), 
                        context->getPathanFactory()->createInteger(sumDate.Year(), context),
                        context->getPathanFactory()->createNonNegativeInteger(sumDate.Month(), context),
                        context->getPathanFactory()->createNonNegativeInteger(sumDate.Day(), context),
                        context->getPathanFactory()->createNonNegativeInteger(hh, context),
                        context->getPathanFactory()->createNonNegativeInteger(mm, context),
                        _ss,
                        new Timezone(0, 0), true  // timezone set to UTC 
                        );
}


/**
 * Returns a date with the given yearMonthDuration subtracted from it
 */
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::subtractYearMonthDuration(const ATDurationOrDerived::Ptr &yearMonth, const DynamicContext* context) const {
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
ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::subtractDayTimeDuration(const ATDurationOrDerived::Ptr &dayTime, const DynamicContext* context) const {
  if(((const ATDurationOrDerived*)dayTime)->isNegative()) {
    return addDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getDays())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                              ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  } else {
    return subtractDayTimeDuration(((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getDays())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getHours())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getMinutes())->asMAPM(),
                                   ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTime)->getSeconds())->asMAPM(), context);
  }
  
}

ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::addDayTimeDuration(MAPM days, MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const {
  // This algorithm is according to spec from http://www.w3.org/TR/xmlschema-2/#adding-durations-to-dateTimes
  // Seconds
  MAPM temp = getSeconds()->asMAPM() + seconds;
  MAPM ss = DateUtils::modulo(temp, DateUtils::g_secondsPerMinute);
  MAPM carry = (temp / DateUtils::g_secondsPerMinute).floor();

  // Minutes
  temp = getMinutes()->asMAPM()+minutes+carry;
  MAPM mm = DateUtils::modulo(temp, DateUtils::g_minutesPerHour);
  carry = (temp/DateUtils::g_minutesPerHour).floor();
	
  // Hours
  temp = getHours()->asMAPM()+hours+carry;
  MAPM hh = DateUtils::modulo(temp, DateUtils::g_hoursPerDay);
  carry = (temp / DateUtils::g_hoursPerDay).floor();

  Date thisDate = Date(asInt(getDays()->asMAPM()),
                       asInt(getMonths()->asMAPM()),
                       asInt(getYears()->asMAPM()));
  Date sumDate = thisDate + asInt(days) + asInt(carry);
  
  return new
    ATDateTimeOrDerivedImpl(_typeURI, 
                            _typeName, 
                            context->getPathanFactory()->createInteger(sumDate.Year(), context),
                            context->getPathanFactory()->createNonNegativeInteger(sumDate.Month(), context),
                            context->getPathanFactory()->createNonNegativeInteger(sumDate.Day(), context),
                            context->getPathanFactory()->createNonNegativeInteger(hh, context),
                            context->getPathanFactory()->createNonNegativeInteger(mm, context),
                            context->getPathanFactory()->createDecimal(ss, context),
                            getTimezone(), hasTimezone());
}

ATDateTimeOrDerived::Ptr ATDateTimeOrDerivedImpl::subtractDayTimeDuration(MAPM days, MAPM hours, MAPM minutes, MAPM seconds, const DynamicContext* context) const {
  return this->addDayTimeDuration(days.neg(), hours.neg(), minutes.neg(), seconds.neg(), context);
}

/**
 * Returns a dayTimeDuration corresponding to the difference between this
 * and the given ATDateTimeOrDerived*
 */
ATDurationOrDerived::Ptr ATDateTimeOrDerivedImpl::subtractDateTimeAsDayTimeDuration(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const {
  // normalize both dates first
  const ATDateTimeOrDerived::Ptr thisDate = this->normalize(context);
  const ATDateTimeOrDerived::Ptr otherDate = date->normalize(context);
  
  // this as julian
  Date dateThis = Date(asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getDays())->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getMonths())->asMAPM()),
                       asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getYears())->asMAPM()));
  // other date as julian
  Date dateOther = Date(asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getDays())->asMAPM()),
                        asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getMonths())->asMAPM()),
                        asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getYears())->asMAPM()));
  // difference in days
  long dateDiff = dateThis - dateOther;

  MAPM dateDiffInSeconds = dateDiff*DateUtils::g_secondsPerDay;
  
  // calculate the differences in seconds
  MAPM thisSeconds = ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getHours())->asMAPM() * DateUtils::g_secondsPerHour +
                     ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getMinutes())->asMAPM() * DateUtils::g_secondsPerMinute +
                     ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)thisDate)->getSeconds())->asMAPM();
  MAPM otherSeconds = ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getHours())->asMAPM() * DateUtils::g_secondsPerHour +
                      ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getMinutes())->asMAPM() * DateUtils::g_secondsPerMinute +
                      ((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)otherDate)->getSeconds())->asMAPM();

  MAPM secDiff = thisSeconds - otherSeconds + dateDiffInSeconds;

  bool isNegative = (secDiff < MM_Zero);
  MAPM endDiff = secDiff.abs();

  // getDays 
  const ATDecimalOrDerived::Ptr DD = context->getPathanFactory()->createInteger((endDiff / DateUtils::g_secondsPerDay).floor(), context);
  MAPM carry = DateUtils::modulo(endDiff,DateUtils::g_secondsPerDay);

  // get hour
  const ATDecimalOrDerived::Ptr hh = context->getPathanFactory()->createInteger((carry/DateUtils::g_secondsPerHour).floor(), context);

  // get minute
  const ATDecimalOrDerived::Ptr mm = context->getPathanFactory()->createInteger((DateUtils::modulo(carry,DateUtils::g_secondsPerHour)/DateUtils::g_secondsPerMinute).floor(), context);

  // get seconds
  const ATDecimalOrDerived::Ptr ss = context->getPathanFactory()->createDecimal(DateUtils::modulo( DateUtils::modulo(carry,DateUtils::g_secondsPerHour), DateUtils::g_secondsPerMinute) , context);
  
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(isNegative) {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  }
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P);
  buffer.append(DD->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
  buffer.append(hh->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_H);
  buffer.append(mm->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M);
  buffer.append(ss->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_S);
  return context->getPathanFactory()->createDayTimeDuration(buffer.getRawBuffer(), context);
}

/**
 * Returns a dayTimeDuration corresponding to the difference between this
 * and the given ATDateTimeOrDerived*
 */
ATDurationOrDerived::Ptr ATDateTimeOrDerivedImpl::subtractDateTimeAsYearMonthDuration(const ATDateTimeOrDerived::Ptr &date, const DynamicContext* context) const {

  // normalize both dates first
  const ATDateTimeOrDerived::Ptr thisDate = this->normalize(context);
  const ATDateTimeOrDerived::Ptr otherDate = ((const ATDateTimeOrDerived*)date)->normalize(context);

  // Call getDayTimeDuration
  const ATDurationOrDerived::Ptr dayTimeDiff = ((const ATDateTimeOrDerived*)thisDate)->subtractDateTimeAsDayTimeDuration(((const ATDateTimeOrDerived*)otherDate), context);

  // put it into yearMonthDuration form
  MAPM days = ((const ATDecimalOrDerived*)((const ATDurationOrDerived*)dayTimeDiff)->getDays())->asMAPM();
  if(((const ATDurationOrDerived*)dayTimeDiff)->isNegative()) 
    days = days.neg();
  
  MAPM months = MM_Zero;
  // Get number of months
  ATDateTimeOrDerived::Ptr cur = (ATDateTimeOrDerivedImpl::Ptr)date;
  while (true)
  {
    int currentDaysInMonth = DateUtils::maximumDayInMonthFor(asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)cur)->getYears())->asMAPM()),
                                                             asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)cur)->getMonths())->asMAPM()));
    int daysToNextMonth = currentDaysInMonth-asInt(((const ATDecimalOrDerived*)((const ATDateTimeOrDerived*)cur)->getDays())->asMAPM())+1;
    if (days > daysToNextMonth) {
      days = days - daysToNextMonth;
      months = months + MM_One;
      cur = (const ATDateTimeOrDerived::Ptr)((const ATDateTimeOrDerivedImpl*)(const ATDateTimeOrDerived*)cur)->addDayTimeDuration(daysToNextMonth, MM_Zero, MM_Zero, MM_Zero, context);
    }
    else
      break;
  }//while
  // Get year and month

  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(months < MM_Zero) {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    months=months.neg();
  }
  const ATDecimalOrDerived::Ptr MM=context->getPathanFactory()->createInteger(months, context);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P);
  buffer.append(MM->asString(context));
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M);
  return context->getPathanFactory()->createYearMonthDuration(buffer.getRawBuffer(), context);
}

//////////////////////////////////////
// Horrible Hack to make Dates      //
// work for now. Loss of Precision! //
//////////////////////////////////////
int ATDateTimeOrDerivedImpl::asInt(MAPM num) const
{
  if(num < INT_MIN || num > INT_MAX) {
    DSLthrow(XPath2TypeCastException, X("ATDateTimeOrDerivedImpl::asInt"), X("Invalid representation of an int"));
  } else {
    char out_string[256];
    num.toIntegerString(out_string);
    return atoi(out_string);
  }
}

AnyAtomicType::AtomicObjectType ATDateTimeOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}

void ATDateTimeOrDerivedImpl::setDateTime(const XMLCh* const dateTime, const DynamicContext* context) {
  unsigned int length = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(dateTime);

  if(dateTime == 0) {
          DSLthrow(XPath2TypeCastException,X("ATDateTimeOrDerivedImpl::setDateTime"), X("Invalid representation of dateTime"));
  }
  
  // State variables etc.
  bool gotDot = false;
  bool gotDigit = false;

  unsigned int pos = 0;
  long int tmpnum = 0;
  MAPM decplace = 1;
  MAPM tmpdec = 0;
  bool negative = false;

  unsigned numDigit = 0;

  // defaulting values
  MAPM YY = 0;
  MAPM MM = 0;
  MAPM DD = 0;
  MAPM hh = 0;
  MAPM mm = 0;
  MAPM ss = 0;
  _hasTimezone = false;

  bool zonepos = true;
  int zonehh = 0;
  int zonemm = 0;

  int state = 0 ; // 0 = year / 1 = month / 2 = day / 3 = hour 
                   // 4 = minutes / 5 = sec / 6 = timezonehour / 7 = timezonemin
  XMLCh tmpChar;
  
  bool wrongformat = false;

  if ( length > 0 && dateTime[0] == L'-'  ) {
                negative = true;
    pos = 1;
  }else{
    pos = 0;
  } 
    
  while ( ! wrongformat && pos < length) {
    tmpChar = dateTime[pos];
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
        } else  {           
          numDigit ++;
        }                        
        tmpnum *= 10;
        tmpnum +=  static_cast<int>(tmpChar - 0x0030);
        gotDigit = true;
        break;
      }
    case L'T' : {
      if ( state == 2 && gotDigit && !gotDot && numDigit == 2) {
        state ++;
        DD = tmpnum;
        tmpnum = 0;
        gotDigit = false;
        numDigit = 0;
      } else {
        wrongformat = true;
      }
      break;
    }
    case L'-' : {
      if ( gotDigit ) {
        if (state == 0 && numDigit >= 4 ) { 
          YY = tmpnum;
          if (negative) {
            YY = YY * -1;
          }
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0;
        } else if (state == 1 && numDigit == 2) {
          MM = tmpnum;    
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0;
        } else if ( state == 5 && numDigit == 2) {          
          tmpdec =  tmpnum / decplace;      
          ss += tmpdec;          
          gotDigit = false;  
          _hasTimezone = true;
          zonepos = false;
          tmpnum = 0;
          numDigit = 0;
        } else {
          wrongformat = true;
        }
        state ++;
      }
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
          gotDigit = false;
          numDigit = 0;
        } else if ( state == 4 && numDigit == 2) {
          mm = tmpnum;
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0;
        } else if ( state == 6 && numDigit == 2) {
          zonehh = tmpnum;
          tmpnum = 0;
          gotDigit = false;
          numDigit = 0;
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
    }else if ( gotDigit && state == 5 && numDigit == 2) {
      tmpdec =  tmpnum / decplace;      
      ss += tmpdec;      
    }else {
      wrongformat = true;
    }
  } 
  
  // check time format
      
  if ( MM > 12 || DD > 28 || hh > 24 || mm > 60 || ss >= 61 || zonehh > 24 || zonemm > 60 || YY == 0 ) 
  {
    // mod in MAPM is called rem
    bool leapyear = false;
    if ( YY.rem(400) == 0 || ( (YY.rem(100) != 0 ) && (YY.rem(4) == 0 ) ) ) 
      leapyear = true;
    if ( (MM == 2 && leapyear && DD > 29 ) || 
  (MM == 2 && ! leapyear && DD > 28 ) ||
  (( MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10 || MM ==12 ) && DD > 31 ) ||
        ( ( MM == 4 || MM == 6 || MM == 9 || MM == 11) && DD > 30 ) || 
  hh > 24 || mm > 60 || ss >= 61 || zonehh > 24 || zonemm > 60 || YY == 0) 
    {
    wrongformat = true;
    }
  }

  if (wrongformat) 
  {
    DSLthrow(XPath2TypeCastException,X("XSDateTimeImpl::setDateTime"), X("Invalid representation of dateTime"));
  }

  // Create Timezone object, clean this up in future
  if (zonepos == false) {
    zonehh *= -1;
    zonemm *= -1;
  }
  timezone_ = new Timezone(zonehh, zonemm);
  
  _YY = context->getPathanFactory()->createInteger(YY, context);
  _MM = context->getPathanFactory()->createNonNegativeInteger(MM, context);
  _DD = context->getPathanFactory()->createNonNegativeInteger(DD, context);
  _hh = context->getPathanFactory()->createNonNegativeInteger(hh, context);
  _mm = context->getPathanFactory()->createNonNegativeInteger(mm, context);
  _ss = context->getPathanFactory()->createDecimal(ss, context);
}

