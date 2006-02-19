/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATBase64BinaryTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATBase64BinaryOrDerived.hpp>

ATBase64BinaryTester::ATBase64BinaryTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATBase64BinaryOrDerived") {
}
  
void ATBase64BinaryTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATBase64BinaryOrDerived::Ptr bin1 = this->createBinary(X("01101100"), context);
  const ATBase64BinaryOrDerived::Ptr bin2 = this->createBinary(X("01101100"), context);

  assertObjectEquals(bin1, bin1, context);
  assertObjectEquals(bin1, bin2, context);

  assertEquals(bin1->asString(context), X("01101100"));
  assertEquals(bin1->asString(context), bin2->asString(context));

  // test 2 -- inequality
  const ATBase64BinaryOrDerived::Ptr bin3 = this->createBinary(X("0110110010010011"), context);
  assertCondition(!bin1->equals(bin3, context));

  // test 3 -- typeURI and typeName
  assertEquals(bin3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY);
  assertEquals(bin3->getTypeName(), bin3->getPrimitiveTypeName());
  
  assertEquals(bin3->getTypeURI(), bin3->getPrimitiveTypeURI());
  assertEquals(bin3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
  
}

const ATBase64BinaryOrDerived::Ptr ATBase64BinaryTester::createBinary(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY,
      value, context);
}


