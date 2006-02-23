/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "TypePromotionTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>

TypePromotionTester::TypePromotionTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "Type Promotion") {
}

void TypePromotionTester::run(const DynamicContext* context) {
  // for every operation, test that
  // decimal op decimal => decimal
  // double op double => double
  // float op float => float
  //
  // decimal op double => double
  // decimal op float => float
  // float op double => double
  // float op decimal => float
  // double op decimal => double
  // double op float => double

  const ATDecimalOrDerived::Ptr decimal50 = context->getPathanFactory()->createDecimal(50, context);
  const ATDoubleOrDerived::Ptr double50   = context->getPathanFactory()->createDouble(50, context);
  const ATFloatOrDerived::Ptr float50     = context->getPathanFactory()->createFloat(50, context);

  // add ////////////////////////////////////////////////////////////////////
  // decimal + decimal
  assertCondition(decimal50->add(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // double + double
  assertCondition(double50->add(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float + float
  assertCondition(float50->add(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));

  // decimal + double
  assertCondition(decimal50->add(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // decimal + float
  assertCondition(decimal50->add(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // float + double
  assertCondition(float50->add(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float + decimal
  assertCondition(float50->add(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // double + decimal
  assertCondition(double50->add(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // double + float
  assertCondition(double50->add(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  
  // subtract //////////////////////////////////////////////////////////////////
  // decimal - decimal
  assertCondition(decimal50->subtract(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // double - double
  assertCondition(double50->subtract(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float - float
  assertCondition(float50->subtract(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));

  // decimal - double
  assertCondition(decimal50->subtract(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // decimal - float
  assertCondition(decimal50->subtract(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // float - double
  assertCondition(float50->subtract(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float - decimal
  assertCondition(float50->subtract(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // double - decimal
  assertCondition(double50->subtract(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // double - float
  assertCondition(double50->subtract(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  
  // multiply //////////////////////////////////////////////////////////////
  // decimal * decimal
  assertCondition(decimal50->multiply(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // double * double
  assertCondition(double50->multiply(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float * float
  assertCondition(float50->multiply(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));

  // decimal * double
  assertCondition(decimal50->multiply(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // decimal * float
  assertCondition(decimal50->multiply(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // float * double
  assertCondition(float50->multiply(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float * decimal
  assertCondition(float50->multiply(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // double * decimal
  assertCondition(double50->multiply(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // double * float
  assertCondition(double50->multiply(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
 
  // divide //////////////////////////////////////////////////////////////////
  // decimal / decimal
  assertCondition(decimal50->divide(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // double / double
  assertCondition(double50->divide(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float / float
  assertCondition(float50->divide(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));

  // decimal / double
  assertCondition(decimal50->divide(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // decimal / float
  assertCondition(decimal50->divide(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // float / double
  assertCondition(float50->divide(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float / decimal
  assertCondition(float50->divide(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // double / decimal
  assertCondition(double50->divide(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // double / float
  assertCondition(double50->divide(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  
  // mod //////////////////////////////////////////////////////////////////////
  // decimal mod decimal
  assertCondition(decimal50->mod(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context));
  // double mod double
  assertCondition(double50->mod(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float mod float
  assertCondition(float50->mod(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));

  // decimal mod double
  assertCondition(decimal50->mod(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // decimal mod float
  assertCondition(decimal50->mod(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // float mod double
  assertCondition(float50->mod(double50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // float mod decimal
  assertCondition(float50->mod(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context));
  // double mod decimal
  assertCondition(double50->mod(decimal50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  // double mod float
  assertCondition(double50->mod(float50, context)->isInstanceOfType(  
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context));
  
   

}

