
/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATEUTILS_HPP
#define _DATEUTILS_HPP

#include <time.h>

#include <pathan/DynamicContext.hpp>
#include "Date.hpp"
#include <pathan/internal/m_apm.h>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class XMLBuffer;
XERCES_CPP_NAMESPACE_END

/** Class with static methods for dealing with date operations */
class DateUtils
{
public:

  /**
   * Constants used when doing date math
   */
  static const int g_secondsPerHour, g_secondsPerMinute, g_secondsPerDay, g_minutesPerHour, g_hoursPerDay;

  /**
   * Helper function that appends the specified number with the requested precision
   * to the buffer (by adding 0s before the number so that it always has minDigits digits)
   */
  static void formatNumber(int value, int minDigits, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buffer);
  
  /** 
   * Helper function for adding durations to dateTimes
   * Returns the greatest integer less than or equal to a/b
   */
  static int fQuotient(int a, int b);

  /** 
   * Helper function for adding durations to dateTimes
   * Returns the greatest integer less than or equal to a/b
   */
  static int fQuotient(double a, double b);

  /**
   * Helper function for adding durations to dateTimes
   * Mod function
   */
  static int modulo(int a, int b);

  /**
   * Helper function for adding durations to dateTimes
   * Mod function for seconds part
   */
  static double modulo(double a, double b);

  /**
   * Helper function for adding durations to dateTimes
   * Mod function for seconds part
   */
  static MAPM modulo(MAPM a, MAPM b);

  /**
   * Helper function for adding durations for dateTimes
   * Returns fQuotient(a - low, high - low)
   */
  static int fQuotient(int a, int low, int high);

  /**
   * Helper function for adding durations for dateTimes
   * Returns modulow(a - low, high - low)
   */
  static int modulo(int a, int low, int high);

  /**
   * Returns the maximum day of the given month for the given year
   * month - Value between 1 (January) and 12 (December)
   */
  static int maximumDayInMonthFor(int yearValue, int monthValue);

  /**
   * Returns the number of days in the given year
   */
  static int daysInYear(int yearValue);

  /**
   * Returns a Date representing the given Date as well as the given number
   * of days.
   */
  static Date addDaysToDate(Date &day, int days);

  /**
   * Returns the current date as an ATDate
   */
  static const ATDateOrDerived::Ptr getCurrentDate(const DynamicContext* context);

  /**
   * Returns the current date and time as an ATDateTime
   */
  static const ATDateTimeOrDerived::Ptr getCurrentDateTime(const DynamicContext* context);

  static void initialize();
  static void terminate();
  static struct tm *threadsafe_localtime(const time_t *timep, struct tm *result);
  static struct tm *threadsafe_gmtime(const time_t *timep, struct tm *result);
};

#endif // _DATEUTILS_HPP

