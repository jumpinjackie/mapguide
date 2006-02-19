
/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */
#include "../config/pathan_config.h"
#include "DateUtils.hpp"
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <stdio.h> // for sprintf
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>
#include <math.h>

#include <xercesc/util/Mutexes.hpp>

const int DateUtils::g_secondsPerMinute = 60;
const int DateUtils::g_minutesPerHour = 60;
const int DateUtils::g_hoursPerDay = 24;
const int DateUtils::g_secondsPerHour = DateUtils::g_secondsPerMinute*DateUtils::g_minutesPerHour;
const int DateUtils::g_secondsPerDay = DateUtils::g_secondsPerHour*DateUtils::g_hoursPerDay;

void DateUtils::formatNumber(int value, int minDigits, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buffer)
{
  bool bIsNegative=false;
  if(value<0)
  {
    bIsNegative=true;
    value=-value;
  }
  XMLCh tmpBuff[19];
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(value,tmpBuff,18,10);
  if(bIsNegative)
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDash);
  for(int len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(tmpBuff);len<minDigits;len++)
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
  buffer.append(tmpBuff);
}

int DateUtils::fQuotient( int a, int b) {
  if (b == 0)
    return 0;

  return (int)floor(a/(double)b);
}

int DateUtils::fQuotient( double a, double b) {
  if (b == 0)
    return 0;

  return (int)floor(a/(double)b);
}

int DateUtils::modulo( int a, int b)  {
   return a - fQuotient(a, b)*b;
}

double DateUtils::modulo( double a, double b) {
   return a - fQuotient(a, b)*b;
}

int DateUtils::fQuotient(int a, int low, int high ) {
    return fQuotient(a-low,high-low);
}

int DateUtils::modulo(int a, int low, int high ) {
    return modulo(a - low, high-low) + low;
}

// modulo(a,b) = a - (floor(a / b) * b)
MAPM DateUtils::modulo( MAPM a, MAPM b)  {
  MAPM quotient=a/b;
  MAPM result=a - (quotient.floor() * b);
  return result;
}

int DateUtils::maximumDayInMonthFor(int yearValue, int monthValue) {
  int M = modulo(monthValue,1,13);
  int Y = yearValue + fQuotient(monthValue,1,13);
  if ( M == 1 || M == 3 || M == 5 || M == 7 || M == 8 || M == 10 || M == 12 )
    return 31;
  else if ( M == 4 || M == 6 || M == 9 || M == 11 )
    return 30;
  else if ( M == 2 && modulo(Y,4) == 0 && ( modulo(Y,400) == 0 || modulo(Y,100) != 0 ) )
    return 29;
  else
    return 28;

}

int DateUtils::daysInYear(int yearValue) {
  if (maximumDayInMonthFor(yearValue, 2) == 29)
    return 366;
  else 
    return 365;
}

Date DateUtils::addDaysToDate(Date &date, int days) 
{
  return (date + days);
}


const ATDateOrDerived::Ptr DateUtils::getCurrentDate(const DynamicContext* context) 
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  // We get the current time and adjust it to our timezone.  We then set
  // this timezone in the Date object.
  time_t curDate=context->getCurrentTime();
  // Note using localtime uses the tzset() function used by
  // DateUtils::getImplicitTimezone.  This function and getImplicitTimezone
  // MUST get the same value in order for the correct time to be stored.
  struct tm time_struct;
  struct tm* curLocalDate=threadsafe_localtime(&curDate, &time_struct);
  char szDate[256];

  sprintf(szDate,"%04d-%02d-%02d",curLocalDate->tm_year+1900, curLocalDate->tm_mon+1, curLocalDate->tm_mday);
  // no need to add timezone, it's already compensated for in localtime
  // date.setTimezone(Timezone(XSDecimal(DateUtils::getImplicitTimezone(), context->getMemoryManager())));
  return context->getPathanFactory()->createDate(memMgr->getPooledString(szDate), context);
}

const ATDateTimeOrDerived::Ptr DateUtils::getCurrentDateTime(const DynamicContext* context) 
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  // We get the current time and adjust it to our timezone.  We then set
  // this timezone in the DateTime object.
  time_t curDate=context->getCurrentTime();
  // Note using localtime uses the tzset() function used by
  // DateUtils::getImplicitTimezone.  This function and getImplicitTimezone
  // MUST get the same value in order for the correct time to be stored.
  struct tm time_struct;
  struct tm* curLocalDate=threadsafe_localtime(&curDate, &time_struct);
  char szDate[256];

  sprintf(szDate,"%04d-%02d-%02dT%02d:%02d:%02dZ", 
          curLocalDate->tm_year+1900,
          curLocalDate->tm_mon+1,
          curLocalDate->tm_mday,
          curLocalDate->tm_hour,
          curLocalDate->tm_min,
          curLocalDate->tm_sec);

  // no need to add timezone, it's already compensated for in localtime
  // dateTime.setTimezone(Timezone(XSDecimal(DateUtils::getImplicitTimezone(), context->getMemoryManager())));
  return context->getPathanFactory()->createDateTime(memMgr->getPooledString(szDate), context);
}

static XERCES_CPP_NAMESPACE_QUALIFIER XMLMutex *time_mutex = 0;

void DateUtils::initialize()
{
  if(time_mutex == 0) {
    time_mutex = new XERCES_CPP_NAMESPACE_QUALIFIER XMLMutex();
  }
}

void DateUtils::terminate()
{
  delete time_mutex;
  time_mutex = 0;
}

struct tm *DateUtils::threadsafe_localtime(const time_t *timep, struct tm *result)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLMutexLock lock(time_mutex);

  struct tm *tmp = ::localtime(timep);
  memcpy(result, tmp, sizeof(struct tm));
  return result;
}

struct tm *DateUtils::threadsafe_gmtime(const time_t *timep, struct tm *result)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLMutexLock lock(time_mutex);

  struct tm *tmp = ::gmtime(timep);
  memcpy(result, tmp, sizeof(struct tm));
  return result;
}
