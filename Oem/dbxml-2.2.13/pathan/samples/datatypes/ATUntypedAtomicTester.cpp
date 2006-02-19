/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATUntypedAtomicTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

#include <pathan/ATUntypedAtomic.hpp>

ATUntypedAtomicTester::ATUntypedAtomicTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATUntypedAtomic") {
}

void ATUntypedAtomicTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATUntypedAtomic::Ptr untyped1 = context->getPathanFactory()->createUntypedAtomic(X("DSL"), context);
  const ATUntypedAtomic::Ptr untyped2 = context->getPathanFactory()->createUntypedAtomic(X("DSL"), context);

  assertObjectEquals(untyped1, untyped1, context);  
  assertObjectEquals(untyped1, untyped2, context);

  assertEquals(untyped1->asString(context), X("DSL")); 
  assertEquals(untyped1->asString(context), untyped2->asString(context));

  // test 2 -- inequality
  const ATUntypedAtomic::Ptr untyped3 = context->getPathanFactory()->createUntypedAtomic(X("Mr. Bungle"), context);
  assertCondition(!untyped1->equals(untyped3, context));

  // test 3 -- typeURI and typeName
  assertEquals(untyped3->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
  assertEquals(untyped3->getTypeName(), untyped3->getPrimitiveTypeName());
  
  assertEquals(untyped3->getTypeURI(), untyped3->getPrimitiveTypeURI());
  assertEquals(untyped3->getTypeURI(), FunctionConstructor::XMLChXPath2DatatypesURI);
}
