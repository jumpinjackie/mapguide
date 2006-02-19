/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATGMonthDayTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATGMonthDayOrDerived.hpp>
#include <pathan/Timezone.hpp>

ATGMonthDayTester::ATGMonthDayTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATGMonthDayOrDerived") {
}

void ATGMonthDayTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATGMonthDayOrDerived::Ptr gMonthDay1 = this->createGMonthDay(X("--12-06Z"), context);
  const ATGMonthDayOrDerived::Ptr gMonthDay2 = this->createGMonthDay(X("--12-06Z"), context);

  assertObjectEquals(gMonthDay1, gMonthDay1, context);
  assertObjectEquals(gMonthDay1, gMonthDay2, context);

  assertEquals(gMonthDay1->asString(context), X("--12-06Z"));
  assertEquals(gMonthDay1->asString(context), gMonthDay2->asString(context));
  
  // test 2 -- inequality
  const ATGMonthDayOrDerived::Ptr gMonthDay3 = this->createGMonthDay(X("--07-11"), context);
  assertCondition(!gMonthDay1->equals(gMonthDay3, context));
  assertCondition(((const ATGMonthDayOrDerived*)gMonthDay1)->greaterThan(gMonthDay3, context));
  assertCondition(((const ATGMonthDayOrDerived*)gMonthDay3)->lessThan(gMonthDay1, context));

  // test 3 -- timezones
  assertCondition(((const ATGMonthDayOrDerived*)gMonthDay1)->hasTimezone());
  assertCondition(!((const ATGMonthDayOrDerived*)gMonthDay3)->hasTimezone());

  const ATGMonthDayOrDerived::Ptr gMonthDay4 = ((const ATGMonthDayOrDerived*)gMonthDay3)->setTimezone(new Timezone(-8, 0), context);
  assertCondition(((const ATGMonthDayOrDerived*)gMonthDay4)->hasTimezone());
  assertEquals(gMonthDay4->asString(context), X("--07-11-08:00"));
  
  // test 4 -- typeURI and typeName
  assertEquals(gMonthDay4->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY);
  assertEquals(gMonthDay4->getTypeName(), gMonthDay4->getPrimitiveTypeName());
  
  assertEquals(gMonthDay4->getTypeURI(), gMonthDay4->getPrimitiveTypeURI());
  assertEquals(gMonthDay4->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
  
}

const ATGMonthDayOrDerived::Ptr ATGMonthDayTester::createGMonthDay(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGMonthDayOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY,
      value, context);
}
