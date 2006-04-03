/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATAnyURITester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATAnyURIOrDerived.hpp>

ATAnyURITester::ATAnyURITester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATAnyURIOrDerived") {
}

void ATAnyURITester::run(const DynamicContext* context) {
  // test 1 -- equality
  ATAnyURIOrDerived::Ptr uri1 = this->createURI(X("http://www.example.org"), context);
  ATAnyURIOrDerived::Ptr uri2 = this->createURI(X("http://www.example.org"), context);

  assertObjectEquals(uri1, uri1, context);
  assertObjectEquals(uri1, uri2, context);

  assertEquals(uri1->asString(context), X("http://www.example.org"));
  assertEquals(uri1->asString(context), uri2->asString(context));

  // test 2 -- inequality
  ATAnyURIOrDerived::Ptr uri3 = this->createURI(X("http://www.w3.org"), context);
  assertCondition(!uri1->equals(uri3, context));
  
  // test 3 -- typeURI and typeName
  assertEquals(uri3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYURI);
  assertEquals(uri3->getTypeName(), uri3->getPrimitiveTypeName());
  
  assertEquals(uri3->getTypeURI(), uri3->getPrimitiveTypeURI());
  assertEquals(uri3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

ATAnyURIOrDerived::Ptr ATAnyURITester::createURI(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createAnyURI(value, context);
}

