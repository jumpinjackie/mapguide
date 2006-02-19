/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "ATDecimalTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDecimalOrDerived.hpp>

ATDecimalTester::ATDecimalTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "ATDecimalOrDerived") {
}

void ATDecimalTester::run(const DynamicContext* context) {
  // test 1 -- equality
  const ATDecimalOrDerived::Ptr dec1 = context->getPathanFactory()->createDecimal(10, context);
  const ATDecimalOrDerived::Ptr dec2 = context->getPathanFactory()->createDecimalOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, X("10"), context);
  const ATDecimalOrDerived::Ptr int1 = context->getPathanFactory()->createInteger(10, context);

  assertObjectEquals(dec1, dec1, context);  // 1
  assertObjectEquals(dec1, dec2, context);  // 2
  assertObjectEquals(dec1, int1, context);  // 3
  

  assertEquals(dec1->asString(context), X("10")); // 4
  assertEquals(int1->asString(context), X("10"));   // 5
  assertEquals(dec1->asString(context), dec2->asString(context)); // 6

  // test 2 -- inequality
  const ATDecimalOrDerived::Ptr dec3 = context->getPathanFactory()->createDecimal(20, context);
  assertCondition(!dec1->equals(dec3, context));  // 8


  // test type name, type uri, hierarchy
  const ATDecimalOrDerived::Ptr decimal = context->getPathanFactory()->createDecimal(25, context);
  const ATDecimalOrDerived::Ptr nonNegInteger = context->getPathanFactory()->createNonNegativeInteger(25, context);
  const ATDecimalOrDerived::Ptr integer = context->getPathanFactory()->createInteger(25, context);

  assertCondition(decimal->isInstanceOfType(  // 10
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  assertCondition(!decimal->isInstanceOfType(  // 11
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  assertCondition(!decimal->isInstanceOfType(  // 12
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NONNEGATIVEINTEGER, context));
  assertCondition(nonNegInteger->isInstanceOfType(  // 13
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  assertCondition(nonNegInteger->isInstanceOfType(  // 14
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  assertCondition(nonNegInteger->isInstanceOfType(  // 15
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NONNEGATIVEINTEGER, context));
  assertCondition(integer->isInstanceOfType(  // 16
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  assertCondition(integer->isInstanceOfType(  // 17
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  assertCondition(!integer->isInstanceOfType(  // 18
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NONNEGATIVEINTEGER, context));
  ///////
  assertCondition(nonNegInteger->isInstanceOfType(nonNegInteger->getPrimitiveTypeURI(),  //19
                                                  nonNegInteger->getPrimitiveTypeName(),
                                                  context));
  assertCondition(nonNegInteger->isInstanceOfType(nonNegInteger->getTypeURI(),  // 20
                                                  nonNegInteger->getTypeName(),
                                                  context));

  // test canonical representation
  assertEquals(decimal->asString(context), X("25"));  // 21
  assertEquals(integer->asString(context), X("25"));    // 22
  assertEquals(nonNegInteger->asString(context), X("25")); // 23
  
  // test lessThan, greaterThan
  const ATDecimalOrDerived::Ptr integer10 = context->getPathanFactory()->createInteger(10, context);  // 24
  const ATDecimalOrDerived::Ptr integer11 = context->getPathanFactory()->createInteger(11, context);  // 25

  assertCondition(integer10->lessThan(integer11, context));  // 26
  assertCondition(!integer10->lessThan(integer10, context)); // 27
  assertCondition(!integer11->lessThan(integer10, context)); // 28
  
  assertCondition(!integer10->greaterThan(integer11, context));  // 29
  assertCondition(!integer10->greaterThan(integer10, context));  // 30
  assertCondition(integer11->greaterThan(integer10, context));   // 31

  ////////////////////////
  // test add, subtract //
  // /////////////////////
  const ATDecimalOrDerived::Ptr integer1 = context->getPathanFactory()->createInteger(1, context);
  
  const Numeric::Ptr sum = integer10->add(integer1, context);
  assertObjectEquals(sum, integer11, context);  // 32
  
  const Numeric::Ptr diff = integer11->subtract(integer1, context); 
  assertObjectEquals(diff, integer10, context);  // 33

  const ATDecimalOrDerived::Ptr integer0 = context->getPathanFactory()->createInteger(0, context);
  const ATDecimalOrDerived::Ptr decimal0 = context->getPathanFactory()->createDecimal(MAPM(0), context);

  assertObjectEquals(integer10->add(integer0, context), integer10, context); // 34
  const Numeric::Ptr diff2 = integer10->subtract(decimal0, context);
  assertObjectEquals(diff2, integer10, context);  // 35

  // test subtype substitution 
  // integer + integer => interger
  assertCondition(sum->isInstanceOfType(  // 36
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  // integer - integer => integer
  assertCondition(diff->isInstanceOfType(  // 37
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  // integer - decimal => decimal
  assertCondition(diff2->isInstanceOfType(  // 38
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));

  ///////////////////////////
  // test multiply, divide //
  // ////////////////////////
  const ATDecimalOrDerived::Ptr integer100 = context->getPathanFactory()->createInteger(100, context);
  const ATDecimalOrDerived::Ptr decimal10  = context->getPathanFactory()->createDecimal(10, context);
  const ATDecimalOrDerived::Ptr decimal11  = context->getPathanFactory()->createDecimal(11, context);
  const ATDecimalOrDerived::Ptr decimal110 = context->getPathanFactory()->createDecimal(110, context);

  const Numeric::Ptr integerProduct = integer10->multiply(integer10, context);
  assertObjectEquals(integerProduct, integer100, context);  // 39

  const Numeric::Ptr decimalProduct = decimal10->multiply(decimal11, context);
  assertObjectEquals(decimalProduct, decimal110, context);  // 40

  const Numeric::Ptr crossProduct1 = integer10->multiply(decimal11, context);
  assertObjectEquals(crossProduct1, decimal110, context);  // 41

  const Numeric::Ptr crossProduct2 = decimal11->multiply(integer10, context);
  assertObjectEquals(crossProduct2, decimal110, context);  // 42

  assertObjectEquals(integer100->multiply(integer0, context), integer0, context);  // 43
  assertObjectEquals(decimal110->multiply(decimal0, context), decimal0, context);  // 44
  
  const Numeric::Ptr integerQuotient = integer100->divide(integer10, context);  
  assertObjectEquals(integerQuotient, integer10, context);  // 46

  const Numeric::Ptr decimalQuotient = decimal110->divide(decimal11, context);
  assertObjectEquals(decimalQuotient, decimal10, context);  // 47

  const Numeric::Ptr crossQuotient1 = decimal110->divide(integer10, context);
  assertObjectEquals(crossQuotient1, decimal11, context);  // 48

  const Numeric::Ptr crossQuotient2 = integer100->divide(decimal10, context);
  assertObjectEquals(crossQuotient2, decimal10, context);
  
  assertObjectEquals(integer0->divide(integer10, context), integer0, context);
  assertObjectEquals(decimal0->divide(decimal11, context), decimal0, context);

  // integer divide
  const ATDecimalOrDerived::Ptr integer3 = context->getPathanFactory()->createInteger(3, context);
  const ATDecimalOrDerived::Ptr integer33 = context->getPathanFactory()->createInteger(33, context);

  const ATDecimalOrDerived::Ptr integerResult = ((const ATDecimalOrDerived*)integer100)->integerDivide(integer3, context);
  assertObjectEquals(integerResult, integer33, context);
  

  // test subtype substitution
  // integer * integer => integer
  assertCondition(integerProduct->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  // decimal * decimal => decimal
  assertCondition(decimalProduct->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // integer * decimal => decimal
  assertCondition(crossProduct1->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // decimal * integer => decimal
  assertCondition(crossProduct2->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  
  // integer / integer => decimal
  assertCondition(integerQuotient->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // decimal / integer => decimal
  assertCondition(decimalQuotient->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // integer / decimal => decimal
  assertCondition(crossQuotient1->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // decimal / integer => decimal
  assertCondition(crossQuotient2->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));

  // integer idiv integer => integer
  assertCondition(integerResult->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  
  
  //////////////
  // test mod //
  //////////////
  const ATDecimalOrDerived::Ptr integerOdd23 = context->getPathanFactory()->createInteger(23, context);
  const ATDecimalOrDerived::Ptr integerEven22 = context->getPathanFactory()->createInteger(22, context);
  const ATDecimalOrDerived::Ptr decimalOdd2005 = context->getPathanFactory()->createDecimal(2005, context);
  const ATDecimalOrDerived::Ptr decimalEven204 = context->getPathanFactory()->createDecimal(204, context);
  const ATDecimalOrDerived::Ptr integer2 = context->getPathanFactory()->createInteger(2, context);
  const ATDecimalOrDerived::Ptr decimal2 = context->getPathanFactory()->createDecimal(2, context);
  const ATDecimalOrDerived::Ptr decimal1 = context->getPathanFactory()->createDecimal(1, context);


  const Numeric::Ptr mod1 = integerOdd23->mod(integer2, context);
  assertObjectEquals(mod1, integer1, context);

  const Numeric::Ptr mod2 = integerEven22->mod(integer2, context);
  assertObjectEquals(mod2, integer0, context);

  const Numeric::Ptr mod3 = decimalOdd2005->mod(decimal2, context);
  assertObjectEquals(mod3, decimal1, context);

  const Numeric::Ptr mod4 = decimalEven204->mod(decimal2, context);
  assertObjectEquals(mod4, decimal0, context);

  const Numeric::Ptr mod5 = decimalOdd2005->mod(integer2, context);
  assertObjectEquals(mod5, decimal1, context);

  const Numeric::Ptr mod6 = integerEven22->mod(decimal2, context);
  assertObjectEquals(mod6, decimal0, context);

  // test subtype substitution
  // integer mod integer => integer
  assertCondition(mod1->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_INTEGER, context));
  // decimal mod decimal => decimal
  assertCondition(mod3->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // decimal mod integer => decimal
  assertCondition(mod5->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // integer mod decimal => decimal
  assertCondition(mod6->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));

  //////////////////////////////////////////////////
  //  test floor, ceiling, round, roundHalfToEven //
  //////////////////////////////////////////////////
  const ATDecimalOrDerived::Ptr integer28 = context->getPathanFactory()->createInteger(28, context);
  const ATDecimalOrDerived::Ptr decimal27_5 = context->getPathanFactory()->createDecimal(27.5, context);
  const ATDecimalOrDerived::Ptr decimal28 = context->getPathanFactory()->createDecimal(28, context);
  const ATDecimalOrDerived::Ptr decimal27 = context->getPathanFactory()->createDecimal(27, context);

  assertObjectEquals(integer28->floor(context), integer28, context);
  assertObjectEquals(decimal28->floor(context), decimal28, context);
  assertObjectEquals(decimal27_5->floor(context), decimal27, context);
  
  assertObjectEquals(integer28->ceiling(context), integer28, context);
  assertObjectEquals(decimal28->ceiling(context), decimal28, context);
  assertObjectEquals(decimal27_5->ceiling(context), decimal28, context);

  assertObjectEquals(integer28->round(context), integer28, context);
  assertObjectEquals(decimal28->round(context), decimal28, context);
  assertObjectEquals(decimal27_5->round(context), decimal28, context);

  assertObjectEquals(integer28->roundHalfToEven(integer0, context), integer28, context);
  assertObjectEquals(decimal28->roundHalfToEven(integer0, context), decimal28, context);
  assertObjectEquals(decimal27_5->roundHalfToEven(integer0, context), decimal28, context);

  /////////////////////////////
  // test isZero, isNegative //
  /////////////////////////////
  const ATDecimalOrDerived::Ptr integerNeg28 = context->getPathanFactory()->createInteger(-28, context);
  const ATDecimalOrDerived::Ptr decimalNeg28 = context->getPathanFactory()->createDecimal(-28, context);

  assertCondition(integer0->isZero());
  assertCondition(integer1->isPositive());
  assertCondition(!integer1->isNegative());
  assertCondition(!integerNeg28->isPositive());
  assertCondition(integerNeg28->isNegative());
  
  assertCondition(decimal0->isZero());
  assertCondition(decimal2->isPositive());
  assertCondition(!decimal2->isNegative());
  assertCondition(!decimalNeg28->isPositive());
  assertCondition(decimalNeg28->isNegative());
}


