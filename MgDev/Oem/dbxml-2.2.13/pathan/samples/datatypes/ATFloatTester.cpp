/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATFloatTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/m_apm.h>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATFloatOrDerived.hpp>

ATFloatTester::ATFloatTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATFloatOrDerived") {
}

void ATFloatTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATFloatOrDerived::Ptr float1 = context->getPathanFactory()->createFloat(10, context);
  const ATFloatOrDerived::Ptr float2 = context->getPathanFactory()->createFloat(10.0, context);
  const ATFloatOrDerived::Ptr float3 = context->getPathanFactory()->createFloat(X("10"), context);
  const ATFloatOrDerived::Ptr float4 = context->getPathanFactory()->createFloat(X("10.0"), context);
  const ATFloatOrDerived::Ptr float5 = context->getPathanFactory()->createFloat(X("1.0E+1"), context);
  const ATFloatOrDerived::Ptr float6 = context->getPathanFactory()->createFloat(1.0e1, context);
  const ATFloatOrDerived::Ptr float7 = context->getPathanFactory()->createFloat(100e-1, context);

  assertObjectEquals(float1, float2, context);
  assertObjectEquals(float1, float3, context);
  assertObjectEquals(float1, float4, context);
  assertObjectEquals(float1, float5, context);
  assertObjectEquals(float1, float6, context);
  assertObjectEquals(float1, float7, context);
  
  // test 2 -- asString
  assertEquals(float1->asString(context), X("10"));
  assertEquals(float2->asString(context), X("10"));
  assertEquals(float3->asString(context), X("10"));
  assertEquals(float4->asString(context), X("10"));
  assertEquals(float5->asString(context), X("10"));
  assertEquals(float6->asString(context), X("10"));
  assertEquals(float7->asString(context), X("10"));

  // test 3 -- inequality
  const ATFloatOrDerived::Ptr float8 = context->getPathanFactory()->createFloat(100, context);
  const ATFloatOrDerived::Ptr float9 = context->getPathanFactory()->createFloat(-100, context);
  const ATFloatOrDerived::Ptr float10 = context->getPathanFactory()->createFloat(X("0.2e+1"), context);
  const ATFloatOrDerived::Ptr float11 = context->getPathanFactory()->createFloat(X("0.5e+1"), context);
  const ATFloatOrDerived::Ptr float12 = context->getPathanFactory()->createFloat(2.1479, context);
  const ATFloatOrDerived::Ptr float13 = context->getPathanFactory()->createFloat(4.5, context);
  const ATFloatOrDerived::Ptr float14 = context->getPathanFactory()->createFloat(1, context);
  
  assertCondition(!float1->equals(float8, context));
  assertCondition(!float1->equals(float9, context));
  assertCondition(!float1->equals(float10, context));

  assertCondition(float1->lessThan(float8, context));
  assertCondition(float1->greaterThan(float9, context));
  assertCondition(float1->greaterThan(float10, context));

  // test 4 -- special values (NaN, INF, etc.)
  const ATFloatOrDerived::Ptr floatNaN = context->getPathanFactory()->createFloat(X("NaN"), context);
  const ATFloatOrDerived::Ptr floatINF = context->getPathanFactory()->createFloat(X("INF"), context);
  const ATFloatOrDerived::Ptr floatNegINF = context->getPathanFactory()->createFloat(X("-INF"), context);
  const ATFloatOrDerived::Ptr floatZero = context->getPathanFactory()->createFloat(X("0"), context);
  const ATFloatOrDerived::Ptr floatNegZero = context->getPathanFactory()->createFloat(X("-0"), context);

  
  assertCondition(((const ATFloatOrDerived*)floatNaN)->isNaN());
  assertEquals(floatNaN->asString(context), X("NaN"));
  
  assertCondition(((const ATFloatOrDerived*)floatINF)->isInfinite());
  assertCondition(floatINF->isPositive());
  assertCondition(!floatINF->isNegative());
  assertEquals(floatINF->asString(context), X("INF"));

  
  assertCondition(((const ATFloatOrDerived*)floatNegINF)->isInfinite());
  assertCondition(!floatNegINF->isPositive());
  assertCondition(floatNegINF->isNegative());
  assertEquals(floatNegINF->asString(context), X("-INF"));
  
  assertCondition(floatZero->isZero());
  assertCondition(floatZero->isPositive());
  assertCondition(!floatZero->isNegative());
  assertEquals(floatZero->asString(context), X("0"));
  
  
  assertCondition(floatNegZero->isZero());
  assertCondition(!floatNegZero->isPositive());
  assertCondition(floatNegZero->isNegative());
  assertEquals(floatNegZero->asString(context), X("-0"));

  // test 5 -- operators (add, subtract, divide, etc.)
  const Numeric::Ptr result1 = float1->add(float1, context);
  const Numeric::Ptr result2 = float1->subtract(float1, context);
  const Numeric::Ptr result3 = float1->multiply(float10, context);
  const Numeric::Ptr result4 = float8->divide(float1, context);
  const Numeric::Ptr result5 = float11->mod(float10, context);
  const Numeric::Ptr result6 = float12->floor(context);
  const Numeric::Ptr result7 = float13->round(context);

  assertObjectEquals(result1, result3, context);
  assertObjectEquals(result2, floatZero, context);
  assertObjectEquals(result4, float1, context);
  assertObjectEquals(result5, float14, context);
  assertObjectEquals(result6, float10, context);
  assertObjectEquals(result7, float11, context);
  
  //add -- special values
  const ATFloatOrDerived::Ptr result8 = static_cast<const ATFloatOrDerived::Ptr >(float1->add(floatNaN, context));
  const ATFloatOrDerived::Ptr result9 = static_cast<const ATFloatOrDerived::Ptr >(float1->add(floatINF, context));
  const ATFloatOrDerived::Ptr result10 = static_cast<const ATFloatOrDerived::Ptr >(float1->add(floatNegINF, context));
  const Numeric::Ptr result11 = float1->add(floatNegZero, context);
  assertEquals(result8->asString(context), floatNaN->asString(context));
  assertObjectEquals(result9, floatINF, context);
  assertObjectEquals(result10, floatNegINF, context);
  assertObjectEquals(result11, float1, context);

  //subtract -- special values
  const ATFloatOrDerived::Ptr result12 = static_cast<const ATFloatOrDerived::Ptr >(float1->subtract(floatNaN, context));
  const ATFloatOrDerived::Ptr result13 = static_cast<const ATFloatOrDerived::Ptr >(float1->subtract(floatINF, context));
  const ATFloatOrDerived::Ptr result14 = static_cast<const ATFloatOrDerived::Ptr >(float1->subtract(floatNegINF, context));
  const Numeric::Ptr result15 = float1->subtract(floatNegZero, context);
  assertEquals(result12->asString(context), floatNaN->asString(context));
  assertObjectEquals(result13, floatNegINF, context);
  assertObjectEquals(result14, floatINF, context);
  assertObjectEquals(result15, float1, context);
  
  //multiply -- special values
  const ATFloatOrDerived::Ptr result16 = static_cast<const ATFloatOrDerived::Ptr >(float1->multiply(floatNaN, context));
  const ATFloatOrDerived::Ptr result17 = static_cast<const ATFloatOrDerived::Ptr >(float1->multiply(floatINF, context));
  const ATFloatOrDerived::Ptr result18 = static_cast<const ATFloatOrDerived::Ptr >(float1->multiply(floatNegINF, context));
  const Numeric::Ptr result19 = float1->multiply(floatNegZero, context);
  assertEquals(result16->asString(context), floatNaN->asString(context));
  assertObjectEquals(result17, floatINF, context);
  assertObjectEquals(result18, floatNegINF, context);
  assertObjectEquals(result19, floatNegZero, context);

  //divide -- special values
  const ATFloatOrDerived::Ptr result20 = static_cast<const ATFloatOrDerived::Ptr >(float1->divide(floatNaN, context));
  const ATFloatOrDerived::Ptr result21 = static_cast<const ATFloatOrDerived::Ptr >(float1->divide(floatINF, context));
  const ATFloatOrDerived::Ptr result22 = static_cast<const ATFloatOrDerived::Ptr >(float1->divide(floatNegINF, context));
  const ATFloatOrDerived::Ptr result23 = static_cast<const ATFloatOrDerived::Ptr >(float1->divide(floatNegZero, context));
  assertEquals(result20->asString(context), floatNaN->asString(context));
  assertObjectEquals(result21, floatZero, context);
  assertObjectEquals(result22, floatNegZero, context);
  assertObjectEquals(result23, floatNegINF, context);
  
  // test type name, type uri
  assertEquals(float1->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  assertEquals(float1->getTypeName(), float1->getPrimitiveTypeName());
  
  assertEquals(float1->getTypeURI(), float1->getPrimitiveTypeURI());
  assertEquals(float1->getTypeURI(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
}
