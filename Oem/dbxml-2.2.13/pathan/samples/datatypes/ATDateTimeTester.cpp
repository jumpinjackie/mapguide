/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATDateTimeTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/Timezone.hpp>

ATDateTimeTester::ATDateTimeTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATDateTimeOrDerived") {
}

void ATDateTimeTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATDateTimeOrDerived::Ptr dateTime1 = this->createDateTime(X("2000-02-29T13:20:10.0564+02:00"), context);
  const ATDateTimeOrDerived::Ptr dateTime2 = this->createDateTime(X("2000-02-29T15:20:10.0564+04:00"), context);

  assertObjectEquals(dateTime1, dateTime1, context);
  assertObjectEquals(dateTime1, dateTime2, context);

  assertEquals(dateTime1->asString(context), X("2000-02-29T11:20:10.0564Z"));
  assertEquals(dateTime1->asString(context), dateTime2->asString(context));
  
  // test 2 -- inequality
  const ATDateTimeOrDerived::Ptr dateTime3 = this->createDateTime(X("2002-04-02T06:00:00+01:00"), context);
  assertCondition(!dateTime1->equals(dateTime3, context));
  assertCondition(((const ATDateTimeOrDerived*)dateTime1)->lessThan(dateTime3, context));
  assertCondition(((const ATDateTimeOrDerived*)dateTime3)->greaterThan(dateTime1, context));
  
  // test 3 -- component extraction
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getYears(), context->getPathanFactory()->createInteger(2000, context), context);
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getMonths(), context->getPathanFactory()->createInteger(2, context), context);
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getDays(), context->getPathanFactory()->createInteger(29, context), context);
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getHours(), context->getPathanFactory()->createInteger(13, context), context);
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getMinutes(), context->getPathanFactory()->createInteger(20, context), context);
  assertObjectEquals(((const ATDateTimeOrDerived*)dateTime1)->getSeconds(), context->getPathanFactory()->createDecimal(10.0564, context), context);
  assertCondition(((const ATDateTimeOrDerived*)dateTime1)->hasTimezone());
  assertCondition(((const ATDateTimeOrDerived*)dateTime1)->getTimezone()->equals(new Timezone(2, 0)));

  // test 4 -- adding and subtracting durations/dates
  const ATDurationOrDerived::Ptr duration1 = context->getPathanFactory()->createDayTimeDuration(X("P3DT10H"), context);
  const ATDateTimeOrDerived::Ptr result1 = ((const ATDateTimeOrDerived*)dateTime1)->subtractDayTimeDuration(duration1, context);
  assertEquals(result1->asString(context), X("2000-02-26T01:20:10.0564Z"));
  const ATDateTimeOrDerived::Ptr result2 = ((const ATDateTimeOrDerived*)result1)->addDayTimeDuration(duration1, context);
  assertObjectEquals(dateTime1, result2, context);

  const ATDurationOrDerived::Ptr duration2 = context->getPathanFactory()->createYearMonthDuration(X("P1Y2M"), context);
  const ATDateTimeOrDerived::Ptr result3 = ((const ATDateTimeOrDerived*)dateTime1)->addYearMonthDuration(duration2, context);
  assertEquals(result3->asString(context), X("2001-04-29T11:20:10.0564Z"));
  const ATDateTimeOrDerived::Ptr result4 = ((const ATDateTimeOrDerived*)result3)->subtractYearMonthDuration(duration2, context);
  assertObjectEquals(dateTime1, result4, context);
 
  const ATDurationOrDerived::Ptr result5 = ((const ATDateTimeOrDerived*)dateTime3)->subtractDateTimeAsDayTimeDuration(dateTime1, context);
  const ATDurationOrDerived::Ptr difference = context->getPathanFactory()->createDayTimeDuration(X("P762DT17H39M49.9436S"), context);
  assertObjectEquals(result5, difference, context);

  // test 5 -- timezones
  const ATDateTimeOrDerived::Ptr dateTime4 = this->createDateTime(X("1867-07-01T15:20:00"), context);
  assertCondition(!((const ATDateTimeOrDerived*)dateTime4)->hasTimezone());

  const ATDateTimeOrDerived::Ptr dateTime5 = ((const ATDateTimeOrDerived*)dateTime4)->addTimezone(context->getPathanFactory()->createDayTimeDuration(X("-PT5H"), context) ,context);
  assertCondition(((const ATDateTimeOrDerived*)dateTime5)->hasTimezone());
  assertCondition(((const ATDateTimeOrDerived*)dateTime5)->getTimezone()->equals(new Timezone(-5, 0)));

  const ATDateTimeOrDerived::Ptr dateTime6 = ((const ATDateTimeOrDerived*)dateTime5)->setTimezone(new Timezone(-8, 0), context);
  assertCondition(((const ATDateTimeOrDerived*)dateTime6)->getTimezone()->equals(new Timezone(-8, 0)));

  const ATDateTimeOrDerived::Ptr dateTime7 = ((const ATDateTimeOrDerived*)dateTime6)->normalize(context);
  const ATDateTimeOrDerived::Ptr dateTime8 = this->createDateTime(X("1867-07-01T23:20:00Z"), context);
  assertObjectEquals(dateTime7, dateTime8, context);

  // test 6 -- typeURI and typeName
  assertEquals(dateTime3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME);
  assertEquals(dateTime3->getTypeName(), dateTime3->getPrimitiveTypeName());
  
  assertEquals(dateTime3->getTypeURI(), dateTime3->getPrimitiveTypeURI());
  assertEquals(dateTime3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

const ATDateTimeOrDerived::Ptr ATDateTimeTester::createDateTime(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDateTimeOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME,
      value, context);
}

