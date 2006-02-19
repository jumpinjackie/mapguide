/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATHexBinaryTester.hpp"
#include <xercesc/util/XMLString.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATHexBinaryOrDerived.hpp>

ATHexBinaryTester::ATHexBinaryTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATHexBinaryOrDerived") {
}
  
void ATHexBinaryTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATHexBinaryOrDerived::Ptr bin1 = this->createHexBinary(X("A4C4D3FF"), context);
  const ATHexBinaryOrDerived::Ptr bin2 = this->createHexBinary(X("a4c4d3ff"), context);

  assertObjectEquals(bin1, bin1, context);
  assertObjectEquals(bin1, bin2, context);

  assertEquals(bin1->asString(context), X("A4C4D3FF"));
  assertEquals(bin1->asString(context), bin2->asString(context));

  // test 2 -- inequality
  const ATHexBinaryOrDerived::Ptr bin3 = this->createHexBinary(X("a4c4d3fe"), context);
  assertCondition(!bin1->equals(bin3, context));
  assertCondition(!XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(bin3->asString(context), X("a4c4d3fgg")));

  // test 3 -- typeURI and typeName
  assertEquals(bin3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY);
  assertEquals(bin3->getTypeName(), bin3->getPrimitiveTypeName());
  
  assertEquals(bin3->getTypeURI(), bin3->getPrimitiveTypeURI());
  assertEquals(bin3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}

const ATHexBinaryOrDerived::Ptr ATHexBinaryTester::createHexBinary(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY,
      value, context);
}


