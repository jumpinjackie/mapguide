/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATDoubleTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDoubleOrDerived.hpp>

ATDoubleTester::ATDoubleTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATDoubleOrDerived") {
}

void ATDoubleTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATDoubleOrDerived::Ptr double1 = context->getPathanFactory()->createDouble(10, context);
  const ATDoubleOrDerived::Ptr double2 = context->getPathanFactory()->createDouble(10.0, context);
  const ATDoubleOrDerived::Ptr double3 = context->getPathanFactory()->createDouble(X("10"), context);
  const ATDoubleOrDerived::Ptr double4 = context->getPathanFactory()->createDouble(X("10.0"), context);
  const ATDoubleOrDerived::Ptr double5 = context->getPathanFactory()->createDouble(X("1.0E+1"), context);
  const ATDoubleOrDerived::Ptr double6 = context->getPathanFactory()->createDouble(1.0e1, context);
  const ATDoubleOrDerived::Ptr double7 = context->getPathanFactory()->createDouble(100e-1, context);

  assertObjectEquals(double1, double2, context);
  assertObjectEquals(double1, double3, context);
  assertObjectEquals(double1, double4, context);
  assertObjectEquals(double1, double5, context);
  assertObjectEquals(double1, double6, context);
  assertObjectEquals(double1, double7, context);
  
  // test 2 -- asString
  assertEquals(double1->asString(context), X("10"));
  assertEquals(double2->asString(context), X("10"));
  assertEquals(double3->asString(context), X("10"));
  assertEquals(double4->asString(context), X("10"));
  assertEquals(double5->asString(context), X("10"));
  assertEquals(double6->asString(context), X("10"));
  assertEquals(double7->asString(context), X("10"));

  // test 3 -- inequality
  const ATDoubleOrDerived::Ptr double8 = context->getPathanFactory()->createDouble(100, context);
  const ATDoubleOrDerived::Ptr double9 = context->getPathanFactory()->createDouble(-100, context);
  const ATDoubleOrDerived::Ptr double10 = context->getPathanFactory()->createDouble(X("0.2e+1"), context);
  const ATDoubleOrDerived::Ptr double11 = context->getPathanFactory()->createDouble(X("0.5e+1"), context);
  const ATDoubleOrDerived::Ptr double12 = context->getPathanFactory()->createDouble(2.1479, context);
  const ATDoubleOrDerived::Ptr double13 = context->getPathanFactory()->createDouble(4.5, context);
  const ATDoubleOrDerived::Ptr double14 = context->getPathanFactory()->createDouble(1, context);
  
  assertCondition(!double1->equals(double8, context));
  assertCondition(!double1->equals(double9, context));
  assertCondition(!double1->equals(double10, context));

  assertCondition(double1->lessThan(double8, context));
  assertCondition(double1->greaterThan(double9, context));
  assertCondition(double1->greaterThan(double10, context));

  // test 4 -- special values (NaN, INF, etc.)
  const ATDoubleOrDerived::Ptr doubleNaN = context->getPathanFactory()->createDouble(X("NaN"), context);
  const ATDoubleOrDerived::Ptr doubleINF = context->getPathanFactory()->createDouble(X("INF"), context);
  const ATDoubleOrDerived::Ptr doubleNegINF = context->getPathanFactory()->createDouble(X("-INF"), context);
  const ATDoubleOrDerived::Ptr doubleZero = context->getPathanFactory()->createDouble(X("0"), context);
  const ATDoubleOrDerived::Ptr doubleNegZero = context->getPathanFactory()->createDouble(X("-0"), context);

  
  assertCondition(((const ATDoubleOrDerived*)doubleNaN)->isNaN());
  assertEquals(doubleNaN->asString(context), X("NaN"));
  
  assertCondition(((const ATDoubleOrDerived*)doubleINF)->isInfinite());
  assertCondition(doubleINF->isPositive());
  assertCondition(!doubleINF->isNegative());
  assertEquals(doubleINF->asString(context), X("INF"));

  
  assertCondition(((const ATDoubleOrDerived*)doubleNegINF)->isInfinite());
  assertCondition(!doubleNegINF->isPositive());
  assertCondition(doubleNegINF->isNegative());
  assertEquals(doubleNegINF->asString(context), X("-INF"));
  
  assertCondition(doubleZero->isZero());
  assertCondition(doubleZero->isPositive());
  assertCondition(!doubleZero->isNegative());
  assertEquals(doubleZero->asString(context), X("0"));
  
  
  assertCondition(doubleNegZero->isZero());
  assertCondition(!doubleNegZero->isPositive());
  assertCondition(doubleNegZero->isNegative());
  assertEquals(doubleNegZero->asString(context), X("-0"));

  // test 5 -- operators (add, subtract, divide, etc.)
  const Numeric::Ptr result1 = double1->add(double1, context);
  const Numeric::Ptr result2 = double1->subtract(double1, context);
  const Numeric::Ptr result3 = double1->multiply(double10, context);
  const Numeric::Ptr result4 = double8->divide(double1, context);
  const Numeric::Ptr result5 = double11->mod(double10, context);
  const Numeric::Ptr result6 = double12->floor(context);
  const Numeric::Ptr result7 = double13->round(context);

  assertObjectEquals(result1, result3, context);
  assertObjectEquals(result2, doubleZero, context);
  assertObjectEquals(result4, double1, context);
  assertObjectEquals(result5, double14, context);
  assertObjectEquals(result6, double10, context);
  assertObjectEquals(result7, double11, context);
  
  //add -- special values
  const ATDoubleOrDerived::Ptr result8 = static_cast<const ATDoubleOrDerived::Ptr >(double1->add(doubleNaN, context));
  const ATDoubleOrDerived::Ptr result9 = static_cast<const ATDoubleOrDerived::Ptr >(double1->add(doubleINF, context));
  const ATDoubleOrDerived::Ptr result10 = static_cast<const ATDoubleOrDerived::Ptr >(double1->add(doubleNegINF, context));
  const Numeric::Ptr result11 = double1->add(doubleNegZero, context);
  assertEquals(result8->asString(context), doubleNaN->asString(context));
  assertObjectEquals(result9, doubleINF, context);
  assertObjectEquals(result10, doubleNegINF, context);
  assertObjectEquals(result11, double1, context);

  //subtract -- special values
  const ATDoubleOrDerived::Ptr result12 = static_cast<const ATDoubleOrDerived::Ptr >(double1->subtract(doubleNaN, context));
  const ATDoubleOrDerived::Ptr result13 = static_cast<const ATDoubleOrDerived::Ptr >(double1->subtract(doubleINF, context));
  const ATDoubleOrDerived::Ptr result14 = static_cast<const ATDoubleOrDerived::Ptr >(double1->subtract(doubleNegINF, context));
  const Numeric::Ptr result15 = double1->subtract(doubleNegZero, context);
  assertEquals(result12->asString(context), doubleNaN->asString(context));
  assertObjectEquals(result13, doubleNegINF, context);
  assertObjectEquals(result14, doubleINF, context);
  assertObjectEquals(result15, double1, context);
  
  //multiply -- special values
  const ATDoubleOrDerived::Ptr result16 = static_cast<const ATDoubleOrDerived::Ptr >(double1->multiply(doubleNaN, context));
  const ATDoubleOrDerived::Ptr result17 = static_cast<const ATDoubleOrDerived::Ptr >(double1->multiply(doubleINF, context));
  const ATDoubleOrDerived::Ptr result18 = static_cast<const ATDoubleOrDerived::Ptr >(double1->multiply(doubleNegINF, context));
  const Numeric::Ptr result19 = double1->multiply(doubleNegZero, context);
  assertEquals(result16->asString(context), doubleNaN->asString(context));
  assertObjectEquals(result17, doubleINF, context);
  assertObjectEquals(result18, doubleNegINF, context);
  assertObjectEquals(result19, doubleNegZero, context);

  //divide -- special values
  const ATDoubleOrDerived::Ptr result20 = static_cast<const ATDoubleOrDerived::Ptr >(double1->divide(doubleNaN, context));
  const ATDoubleOrDerived::Ptr result21 = static_cast<const ATDoubleOrDerived::Ptr >(double1->divide(doubleINF, context));
  const ATDoubleOrDerived::Ptr result22 = static_cast<const ATDoubleOrDerived::Ptr >(double1->divide(doubleNegINF, context));
  const ATDoubleOrDerived::Ptr result23 = static_cast<const ATDoubleOrDerived::Ptr >(double1->divide(doubleNegZero, context));
  assertEquals(result20->asString(context), doubleNaN->asString(context));
  assertObjectEquals(result21, doubleZero, context);
  assertObjectEquals(result22, doubleNegZero, context);
  assertObjectEquals(result23, doubleNegINF, context);
  
  // test type name, type uri
  assertEquals(double1->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  assertEquals(double1->getTypeName(), double1->getPrimitiveTypeName());
  
  assertEquals(double1->getTypeURI(), double1->getPrimitiveTypeURI());
  assertEquals(double1->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}
