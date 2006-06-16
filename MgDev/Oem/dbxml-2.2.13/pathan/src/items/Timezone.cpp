/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */


#include "../config/pathan_config.h"
#include <assert.h>

#include <pathan/Timezone.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include "../utils/DateUtils.hpp"
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/PathanFactory.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

static const int g_minHour = -12;
static const int g_maxHour = 14;

Timezone::Timezone(int seconds) {
  init(seconds);
  validate();
}

Timezone::Timezone(const ATDecimalOrDerived::Ptr &hour, const ATDecimalOrDerived::Ptr &minute, const DynamicContext* context) {
  _hh=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(hour->asString(context));
  _mm=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(minute->asString(context));
  
  validate();
}

Timezone::Timezone(const ATDurationOrDerived::Ptr &duration, const DynamicContext* context) {
  int seconds=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(duration->getDays()->asString(context)) * DateUtils::g_secondsPerDay +
              XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(duration->getHours()->asString(context)) * DateUtils::g_secondsPerHour +
              XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(duration->getMinutes()->asString(context)) * DateUtils::g_secondsPerMinute +
              XERCES_CPP_NAMESPACE_QUALIFIER XMLString::parseInt(duration->getSeconds()->asString(context));

  if (((const ATDurationOrDerived*)duration)->isNegative())
    seconds = -seconds;

  init(seconds);
  validate();
}

Timezone::Timezone(int hour, int minute) {
  _hh = hour;
  _mm = minute;

  validate();
}

void Timezone::init(int seconds)
{
  // get hour : ( 60 * 60 sec = 3600)
  _hh = seconds / DateUtils::g_secondsPerHour;

  // get minute : ( 60 sec = 60)
  _mm = (seconds % DateUtils::g_secondsPerHour) / DateUtils::g_secondsPerMinute;
}

void Timezone::validate() const {
  // Check that we have a valid timezone
  if ( _hh > g_maxHour || (_hh == g_maxHour && _mm > 0) || _hh < g_minHour || (_hh == g_minHour && _mm < 0)) {
    DSLthrow(XPath2TypeCastException ,X("Timezone::Timezone"), X("Timezone outside of valid range created."));
  }
  if ( (_hh > 0 && _mm < 0) || (_hh < 0 && _mm > 0)) {
    DSLthrow(XPath2TypeCastException ,X("Timezone::Timezone"), X("Invalid timezone."));
  }
}

Timezone::Timezone(const Timezone & other) {
  _hh = other._hh;
  _mm = other._mm;
}

bool Timezone::equals(const Timezone::Ptr &other) const {
  return (_hh == other->_hh && _mm == other->_mm);
}

bool Timezone::greaterThan(const Timezone::Ptr &other) const {
  return (_hh > other->_hh ||
          (_hh == other->_hh && _mm > other->_mm));
}

bool Timezone::lessThan(const Timezone::Ptr &other) const {
  return (_hh < other->_hh ||
          (_hh == other->_hh && _mm < other->_mm));
}

const int Timezone::getMinutes() const {
  return _mm;
}

const int Timezone::getHours() const {
  return _hh;
}

ATDurationOrDerived::Ptr Timezone::asDayTimeDuration(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(_hh < 0) {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
    DateUtils::formatNumber(-_hh,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_H);
    DateUtils::formatNumber(-_mm,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M);
  } else {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T);
    DateUtils::formatNumber(_hh,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_H);
    DateUtils::formatNumber(_mm,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M);        
  }

  return context->getPathanFactory()->createDayTimeDuration(buffer.getRawBuffer(), context);
  

}

const XMLCh* Timezone::asString(const DynamicContext* context) const {
  if (_hh == 0 && _mm == 0)
	  return XPath2Utils::asStr(XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z,context->getMemoryManager());
  
  
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  if(_hh>0) {
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chPlus);
    DateUtils::formatNumber(_hh,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    DateUtils::formatNumber(_mm,2,buffer);
  } else {
    DateUtils::formatNumber(_hh,2,buffer);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    DateUtils::formatNumber(-_mm,2,buffer);
  }
  
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

const XMLCh* Timezone::printTimezone(const DynamicContext* context) const {
  
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  DateUtils::formatNumber(_hh,2,buffer);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
  
  if(_hh > 0) {
    DateUtils::formatNumber(_mm,2,buffer);
  } else {
    DateUtils::formatNumber(-_mm,2,buffer);
  }
  
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}


