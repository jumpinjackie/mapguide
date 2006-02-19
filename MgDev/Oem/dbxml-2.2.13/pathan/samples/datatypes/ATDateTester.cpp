/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATDateTester.hpp"
#include <xercesc/util/XMLString.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/Timezone.hpp>

ATDateTester::ATDateTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATDateOrDerived") {
}

void ATDateTester::run(const DynamicContext* context) {
  // test constructor & asString
  const ATDecimalOrDerived::Ptr y1999 = context->getPathanFactory()->createDecimal(1999, context);
  const ATDecimalOrDerived::Ptr m05 = context->getPathanFactory()->createDecimal(5, context);
  const ATDecimalOrDerived::Ptr d31 = context->getPathanFactory()->createDecimal(31, context);
  
  const ATDateOrDerived::Ptr date1 = this->createDate(X("1999-05-31Z"), context);
  const ATDateOrDerived::Ptr date2 = this->createDate(X("1999-06-01+12:00"), context);
  assertCondition(((const ATDateOrDerived*)date1)->getYears()->equals(y1999, context) && 
                  ((const ATDateOrDerived*)date1)->getMonths()->equals(m05, context)  &&
                  ((const ATDateOrDerived*)date1)->getDays()->equals(d31, context)    &&
                  ((const ATDateOrDerived*)date1)->hasTimezone());

  assertEquals(date1->asString(context), X("1999-05-31Z"));
  assertEquals(date1->asString(context), date2->asString(context));

  const ATDecimalOrDerived::Ptr y1099 = context->getPathanFactory()->createDecimal(-1099, context);
  const ATDecimalOrDerived::Ptr m12 = context->getPathanFactory()->createDecimal(12, context);
  const ATDecimalOrDerived::Ptr d25 = context->getPathanFactory()->createDecimal(25, context);
  
  // test ordering
  const ATDateOrDerived::Ptr date3 = this->createDate(X("-1099-12-25"), context);
  assertObjectEquals(date3->getYears(), y1099, context);
  assertObjectEquals(date3->getMonths(), m12, context);
  assertObjectEquals(date3->getDays(), d25, context);
  assertCondition(!date3->hasTimezone());
  assertEquals(date3->asString(context), X("-1099-12-25"));

  assertObjectEquals(date1, date1, context);

  assertCondition(date1->equals(date1, context));
  assertCondition(!date1->equals(date3, context));
  
  assertCondition(date3->lessThan(date1, context));
  assertCondition(!date1->lessThan(date3, context));
  
  assertCondition(date1->greaterThan(date3, context));
  assertCondition(!date3->greaterThan(date1, context));
  

  // test subtractDate  
  const ATDateOrDerived::Ptr date4 = this->createDate(X("2000-01-06"), context);
  const ATDateOrDerived::Ptr date5 = this->createDate(X("2000-01-30"), context);
  const ATDurationOrDerived::Ptr expectedDiff1 = context->getPathanFactory()->createDayTimeDuration(X("P24D"), context);
  const ATDurationOrDerived::Ptr dayTimeDiff1 = date5->subtractDate(date4, context);
  assertCondition(dayTimeDiff1->equals(expectedDiff1, context));

  const ATDateOrDerived::Ptr date6 = this->createDate(X("1999-01-05"), context);
  const ATDateOrDerived::Ptr date7 = this->createDate(X("2000-01-05"), context);

  const ATDurationOrDerived::Ptr expectedDiff2 = context->getPathanFactory()->createDayTimeDuration(X("-P365D"), context);
  const ATDurationOrDerived::Ptr expectedDiff3 = context->getPathanFactory()->createDayTimeDuration(X("P365D"), context);

  const ATDurationOrDerived::Ptr dayTimeDiff2 = date6->subtractDate(date7, context);
  assertCondition(dayTimeDiff2->equals(expectedDiff2, context));
  
  const ATDurationOrDerived::Ptr dayTimeDiff3 = date7->subtractDate(date6, context);
  assertCondition(dayTimeDiff3->equals(expectedDiff3, context));

  // test addDayTimeDuration
  const ATDateOrDerived::Ptr expectedDate1 = date5;
  assertCondition(date4->addDayTimeDuration(expectedDiff1, context)->equals(expectedDate1, context));

  // test timezones
  const ATDateOrDerived::Ptr date8 = this->createDate(X("1867-07-01"), context);
  assertCondition(!date8->hasTimezone());

  const ATDateOrDerived::Ptr date9 = date8->addTimezone(context->getPathanFactory()->createDayTimeDuration(X("-PT5H"), context) ,context);
  assertCondition(date9->hasTimezone());
  assertCondition(date9->getTimezone()->equals(new Timezone(-5, 0)));

  //cannot call (non-const function) setTimezone on const ATDateOrDerived 
  const ATDateOrDerived::Ptr date10 = date9->setTimezone(new Timezone(3, 0), context);
  assertCondition(date10->getTimezone()->equals(new Timezone(3, 0)));
  //TODO: When the above is uncommented, the below test will result in the date 1867-06-30Z 

  const ATDateOrDerived::Ptr date11 = date10->normalize(context);
  const ATDateOrDerived::Ptr date12 = this->createDate(X("1867-06-30Z"), context);
  assertObjectEquals(date11, date12, context);
  
  // test 6 -- typeURI and typeName
  assertEquals(date3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE);
  assertEquals(date3->getTypeName(), date3->getPrimitiveTypeName());
  
  assertEquals(date3->getTypeURI(), date3->getPrimitiveTypeURI());
  assertEquals(date3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

const ATDateOrDerived::Ptr ATDateTester::createDate(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDateOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE,
      value, context);
}

