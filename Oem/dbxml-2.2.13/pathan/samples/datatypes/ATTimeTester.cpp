/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATTimeTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/Timezone.hpp>

ATTimeTester::ATTimeTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATTimeOrDerived") {
}

void ATTimeTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATTimeOrDerived::Ptr time1 = this->createTime(X("13:20:10.0564+14:00"), context);
  const ATTimeOrDerived::Ptr time2 = this->createTime(X("11:20:10.0564+12:00"), context);
  const ATTimeOrDerived::Ptr time3 = this->createTime(X("16:00:00+01:00"), context);
  const ATTimeOrDerived::Ptr time4 = this->createTime(X("04:20:00"), context);
  const ATTimeOrDerived::Ptr time_wrap = this->createTime(X("04:20:00+06:30"), context);

  assertObjectEquals(time1, time1, context);
  assertObjectEquals(time1, time2, context);
  assertObjectEquals(time4, time4, context);

  assertEquals(time1->asString(context), X("23:20:10.0564Z"));
  assertEquals(time1->asString(context), time2->asString(context));
  assertEquals(time_wrap->asString(context), X("21:50:00Z"));

  // test 2 -- inequality
  assertCondition(!time1->equals(time3, context));
  assertCondition(((const ATTimeOrDerived*)time3)->lessThan(time1, context));
  assertCondition(((const ATTimeOrDerived*)time1)->greaterThan(time3, context));
  
  // test 3 -- component extraction
  assertObjectEquals(((const ATTimeOrDerived*)time1)->getHours(), context->getPathanFactory()->createInteger(13, context), context);
  assertObjectEquals(((const ATTimeOrDerived*)time1)->getMinutes(), context->getPathanFactory()->createInteger(20, context), context);
  assertObjectEquals(((const ATTimeOrDerived*)time1)->getSeconds(), context->getPathanFactory()->createDecimal(10.0564, context), context);
  assertCondition(((const ATTimeOrDerived*)time1)->hasTimezone());
  assertCondition(((const ATTimeOrDerived*)time1)->getTimezone()->equals(new Timezone(14, 0)));

  // test 4 -- adding and subtracting durations/dates
  const ATDurationOrDerived::Ptr duration1 = context->getPathanFactory()->createDayTimeDuration(X("P3DT10H"), context);
  const ATTimeOrDerived::Ptr result1 = ((const ATTimeOrDerived*)time1)->addDayTimeDuration(duration1, context);
  assertEquals(result1->asString(context), X("09:20:10.0564Z"));
  const ATTimeOrDerived::Ptr result2 = ((const ATTimeOrDerived*)result1)->subtractDayTimeDuration(duration1, context);
  assertObjectEquals(time1, result2, context);
  const ATDurationOrDerived::Ptr result5 = ((const ATTimeOrDerived*)time1)->subtractTime(time3, context);
  const ATDurationOrDerived::Ptr difference = context->getPathanFactory()->createDayTimeDuration(X("PT8H20M10.0564S"), context);
  assertObjectEquals(result5, difference, context);
  // test 5 -- timezones
  assertCondition(!((const ATTimeOrDerived*)time4)->hasTimezone());

  const ATTimeOrDerived::Ptr time5 = ((const ATTimeOrDerived*)time4)->addTimezone(context->getPathanFactory()->createDayTimeDuration(X("-PT5H"), context) ,context);
  assertCondition(((const ATTimeOrDerived*)time5)->hasTimezone());
  assertCondition(((const ATTimeOrDerived*)time5)->getTimezone()->equals(new Timezone(-5, 0)));

  const ATTimeOrDerived::Ptr time6 = ((const ATTimeOrDerived*)time5)->setTimezone(new Timezone(-8, 0), context);
  assertCondition(((const ATTimeOrDerived*)time6)->getTimezone()->equals(new Timezone(-8, 0)));

  const ATTimeOrDerived::Ptr time7 = ((const ATTimeOrDerived*)time6)->normalize(context);
  const ATTimeOrDerived::Ptr time8 = this->createTime(X("12:20:00Z"), context);
  assertObjectEquals(time7, time8, context);

  // test 6 -- typeURI and typeName
  assertEquals(time3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME);
  assertEquals(time3->getTypeName(), time3->getPrimitiveTypeName());
  
  assertEquals(time3->getTypeURI(), time3->getPrimitiveTypeURI());
  assertEquals(time3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

const ATTimeOrDerived::Ptr ATTimeTester::createTime(const XMLCh* value,
                                                const DynamicContext* context) {
  return context->getPathanFactory()->createTimeOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME,
      value, context);
}

