/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATQNameTester.hpp"

#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATQNameOrDerived.hpp>

ATQNameTester::ATQNameTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATQNameOrDerived") {
}

void ATQNameTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATQNameOrDerived::Ptr qName1 = this->createQName(X("xs:foo"), context);
  const ATQNameOrDerived::Ptr qName2 = this->createQName(X("http://www.w3.org/2001/XMLSchema"), X("xs"), X("foo"), context);

  assertObjectEquals(qName1, qName1, context);
  assertObjectEquals(qName1, qName2, context);

  assertEquals(qName1->asString(context), X("http://www.w3.org/2001/XMLSchema:foo"));
  assertEquals(qName1->asString(context), qName2->asString(context));
  
  assertEquals(((const ATQNameOrDerived*)(const AnyAtomicType*)qName1)->getURI(), X("http://www.w3.org/2001/XMLSchema"));
  assertEquals(((const ATQNameOrDerived*)(const AnyAtomicType*)qName1)->getName(), X("foo"));

  // test 2 -- inequality
  const ATQNameOrDerived::Ptr qName3 = this->createQName(X("xs:bar"), context);
  assertCondition(!qName1->equals(qName3, context));

  // test 3 -- typeURI and typeName
  assertEquals(qName3->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME);
  assertEquals(qName3->getTypeName(), qName3->getPrimitiveTypeName());
  
  assertEquals(qName3->getTypeURI(), qName3->getPrimitiveTypeURI());
  assertEquals(qName3->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
  
}

const ATQNameOrDerived::Ptr ATQNameTester::createQName(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
      value, context);
}

const ATQNameOrDerived::Ptr ATQNameTester::createQName(const XMLCh* uri,
                                                       const XMLCh* prefix,
                                                   const XMLCh* name,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createQNameOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
      uri, prefix, name, context);
}
