/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATBooleanTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATBooleanOrDerived.hpp>

ATBooleanTester::ATBooleanTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATBooleanOrDerived") {
}

void ATBooleanTester::run(const DynamicContext* context) {

  const ATBooleanOrDerived::Ptr bool1 = this->createBoolean(X("true"), context);
  const ATBooleanOrDerived::Ptr bool2 = this->createBoolean(X("1"), context);
  const ATBooleanOrDerived::Ptr bool3 = this->createBoolean(false, context);

  // test 1 -- equality
  assertObjectEquals(bool1, bool1, context);
  assertObjectEquals(bool1, bool2, context);
  
  assertCondition(((const ATBooleanOrDerived*)bool1)->isTrue());
  assertCondition(((const ATBooleanOrDerived*)bool2)->isTrue());
  assertCondition(((const ATBooleanOrDerived*)bool3)->isFalse());

  // test 2 -- inequality
  assertCondition(!bool1->equals(bool3, context));
  
  assertCondition(!((const ATBooleanOrDerived*)bool1)->isFalse());
  assertCondition(!((const ATBooleanOrDerived*)bool2)->isFalse());
  assertCondition(!((const ATBooleanOrDerived*)bool3)->isTrue());

  // test 3 -- typeURI and typeName
  assertEquals(bool3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN);
  assertEquals(bool3->getTypeName(), bool3->getPrimitiveTypeName());
  
  assertEquals(bool3->getTypeURI(), bool3->getPrimitiveTypeURI());
  assertEquals(bool3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

const ATBooleanOrDerived::Ptr ATBooleanTester::createBoolean(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createBooleanOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN,
      value, context);
}

const ATBooleanOrDerived::Ptr ATBooleanTester::createBoolean(bool value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createBoolean(value, context);
}
