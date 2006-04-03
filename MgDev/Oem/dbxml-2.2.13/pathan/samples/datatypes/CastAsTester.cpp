/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "CastAsTester.hpp"
#include <pathan/internal/DOMutils/XStr.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

#include <pathan/ATAnySimpleType.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATBase64BinaryOrDerived.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/ATGDayOrDerived.hpp>
#include <pathan/ATGMonthOrDerived.hpp>
#include <pathan/ATGMonthDayOrDerived.hpp>
#include <pathan/ATGYearMonthOrDerived.hpp>
#include <pathan/ATGYearOrDerived.hpp>
#include <pathan/ATHexBinaryOrDerived.hpp>
#include <pathan/ATQNameOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>

CastAsTester::CastAsTester(XPath2MemoryManager* memMgr) : DatatypeTester(memMgr, "CastAs") {
}

void CastAsTester::run(const DynamicContext* context) {
  // for every built-in primitive datatype (and type defined by XQuery/XPath), test that it properly 
  // casts to all of the primitive datatypes as defined by the spec

  // create data to cast 
  const ATUntypedAtomic::Ptr uA = context->getPathanFactory()->createUntypedAtomic(X("DSL"), context);
  const ATAnyURIOrDerived::Ptr aURI = this->createURI(X("http://www.example.org"), context);
  const ATBase64BinaryOrDerived::Ptr b64 = this->createBinary(X("01101100"), context);
  const ATBooleanOrDerived::Ptr boolean = this->createBoolean(X("false"), context);
  const ATDateOrDerived::Ptr dat = context->getPathanFactory()->createDate(X("1999-05-31-08:00"), context);
  const ATGDayOrDerived::Ptr Day = this->createGDay(X("---29Z"), context);
  const ATDoubleOrDerived::Ptr dbl = context->getPathanFactory()->createDouble(X("0.0E+0"), context);
  const ATDecimalOrDerived::Ptr dec = context->getPathanFactory()->createDecimal("1", context);
  const ATDateTimeOrDerived::Ptr dT = context->getPathanFactory()->createDateTime(X("2000-02-29T13:20:10.0564+02:00"), context);
  const ATDurationOrDerived::Ptr dTD = context->getPathanFactory()->createDayTimeDuration(X("-P15DT7H21M10.69S"), context);
  const ATDurationOrDerived::Ptr dur = createDuration(X("P21Y11M35DT13H20M10.69S"), context);
  const ATFloatOrDerived::Ptr flt = context->getPathanFactory()->createFloat(X("1.0000"), context);
  const ATHexBinaryOrDerived::Ptr hxB = this->createHexBinary(X("A4C4D3FF"), context);
  const ATGMonthDayOrDerived::Ptr MD = this->createGMonthDay(X("--12-06Z"), context);
  const ATGMonthOrDerived::Ptr Mon = this->createGMonth(X("--12--Z"), context);
  const ATQNameOrDerived::Ptr QN = this->createQName(X("xs:foo"), context);
  const ATStringOrDerived::Ptr str = this->createString(X("inf"), context);
  const ATTimeOrDerived::Ptr tim = context->getPathanFactory()->createTime(X("13:20:10.0564+14:00"), context);
  const ATGYearMonthOrDerived::Ptr YM = this->createGYearMonth(X("2003-07Z"), context);
  const ATDurationOrDerived::Ptr yMD = context->getPathanFactory()->createYearMonthDuration(X("-P7Y5M"), context);
  const ATGYearOrDerived::Ptr Yr = this->createGYear(X("2003Z"), context);

  // create pointers for casted data
  AnyAtomicType::Ptr casted_uA;
  AnyAtomicType::Ptr casted_aURI;
  AnyAtomicType::Ptr casted_b64;
  AnyAtomicType::Ptr casted_boolean;
  AnyAtomicType::Ptr casted_dat;
  AnyAtomicType::Ptr casted_Day;
  AnyAtomicType::Ptr casted_dbl;
  AnyAtomicType::Ptr casted_dec;
  AnyAtomicType::Ptr casted_dT;
  AnyAtomicType::Ptr casted_dTD;
  AnyAtomicType::Ptr casted_dur;
  AnyAtomicType::Ptr casted_flt;
  AnyAtomicType::Ptr casted_hxB;
  AnyAtomicType::Ptr casted_MD;
  AnyAtomicType::Ptr casted_Mon;
  AnyAtomicType::Ptr casted_QN;
  AnyAtomicType::Ptr casted_str;
  AnyAtomicType::Ptr casted_tim;
  AnyAtomicType::Ptr casted_YM;
  AnyAtomicType::Ptr casted_yMD;
  AnyAtomicType::Ptr casted_Yr;


  //test ATUntypedAtomic::castAs
  casted_uA = uA->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = uA->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);


  //test ATAnyURIOrDerived::castAs
  casted_uA = aURI->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
  assertEquals(casted_uA->asString(context), X("http://www.example.org"));

  casted_str = aURI->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);
  assertEquals(casted_str->asString(context), X("http://www.example.org")); 

  casted_aURI = aURI->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYURI, context);
  assertEquals(casted_aURI->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYURI);
  assertEquals(casted_aURI->asString(context), X("http://www.example.org"));

  
  //test ATQNameOrDerived::castAs
  casted_QN = QN->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME, context);
  assertEquals(casted_QN->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME);
  

  //test ATBase64OrDerived::castAs
  casted_uA = b64->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = b64->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_b64 = b64->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY, context);
  assertEquals(casted_b64->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY);

  casted_hxB = b64->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY, context);
  assertEquals(casted_hxB->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY);

  //test ATHexBinaryOrDerived::castAs
  casted_uA = hxB->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = hxB->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_b64 = hxB->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY, context);
  assertEquals(casted_b64->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY);

  casted_hxB = hxB->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY, context);
  assertEquals(casted_hxB->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY);


  //test ATBooleanOrDerived::castAs
  casted_uA = boolean->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = boolean->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_boolean = boolean->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN, context);
  assertEquals(casted_boolean->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN);

  casted_dec = boolean->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
  assertEquals(casted_dec->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL);
  assertEquals(casted_dec->asString(context), X("0")); 

  casted_dbl = boolean->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  assertEquals(casted_dbl->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  assertEquals(casted_dbl->asString(context), X("0")); 

  casted_flt = boolean->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context);
  assertEquals(casted_flt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  assertEquals(casted_flt->asString(context), X("0")); 

  
  //test ATDecimalOrDerived::castAs
  casted_uA = dec->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_dec = dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
  assertEquals(casted_dec->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL);

  casted_dbl = dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  assertEquals(casted_dbl->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  assertEquals(casted_dbl->asString(context), X("1")); 

  casted_flt = dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context);
  assertEquals(casted_flt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  assertEquals(casted_flt->asString(context), X("1")); 
  
  casted_boolean  = dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN, context);
  assertEquals(casted_boolean->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN);
  assertEquals(casted_boolean->asString(context), X("true")); 

  
  //test ATDoubleOrDerived::castAs
  casted_uA = dbl->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_dbl = dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  assertEquals(casted_dbl->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  
  casted_dec = dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
  assertEquals(casted_dec->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL);
  assertEquals(casted_dec->asString(context), X("0")); 
  
  casted_flt = dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context);
  assertEquals(casted_flt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  assertEquals(casted_flt->asString(context), X("0")); 
  
  casted_boolean  = dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN, context);
  assertEquals(casted_boolean->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN);
  assertEquals(casted_boolean->asString(context), X("false")); 
  

  //test ATFloatOrDerived::castAs
  casted_uA = flt->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_flt = flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context);
  assertEquals(casted_flt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  
  casted_dec = flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
  assertEquals(casted_dec->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL);
  assertEquals(casted_dec->asString(context), X("1")); 

  casted_dbl = flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  assertEquals(casted_dbl->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  assertEquals(casted_dbl->asString(context), X("1")); 

  casted_boolean  = flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN, context);
  assertEquals(casted_boolean->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN);
  assertEquals(casted_boolean->asString(context), X("true")); 


  //test ATDateOrDerived::castAs
  casted_uA = dat->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
  assertEquals(casted_uA->asString(context), X("1999-05-31-08:00")); 

  casted_str = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);
  assertEquals(casted_str->asString(context), X("1999-05-31-08:00")); 

  casted_dat = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE, context);
  assertEquals(casted_dat->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE);

  casted_dT = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, context);
  assertEquals(casted_dT->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME);
  assertEquals(casted_dT->asString(context), X("1999-05-31T08:00:00Z")); 
 
  casted_Day = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY, context);
  assertEquals(casted_Day->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY);
  assertEquals(casted_Day->asString(context), X("---31-08:00")); 

  casted_MD = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY, context);
  assertEquals(casted_MD->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY);
  assertEquals(casted_MD->asString(context), X("--05-31-08:00")); 

  casted_Mon = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH, context);
  assertEquals(casted_Mon->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH);
  assertEquals(casted_Mon->asString(context), X("--05---08:00")); 

  casted_YM = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH, context);
  assertEquals(casted_YM->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH);
  assertEquals(casted_YM->asString(context), X("1999-05-08:00")); 
  
  casted_Yr = dat->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR, context);  
  assertEquals(casted_Yr->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR);
  assertEquals(casted_Yr->asString(context), X("1999-08:00")); 


  //test ATDateTimeOrDerived::castAs
  casted_uA = dT->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
  assertEquals(casted_uA->asString(context), X("2000-02-29T13:20:10.0564+02:00")); 

  casted_str = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);
  assertEquals(casted_str->asString(context), X("2000-02-29T13:20:10.0564+02:00")); 

  casted_dT = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, context);
  assertEquals(casted_dT->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME);

  casted_dat = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE, context);
  assertEquals(casted_dat->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATE);
  assertEquals(casted_dat->asString(context), X("2000-02-28Z")); 
 
  casted_Day = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY, context);
  assertEquals(casted_Day->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY);
  assertEquals(casted_Day->asString(context), X("---29+02:00")); 

  casted_MD = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY, context);
  assertEquals(casted_MD->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY);
  assertEquals(casted_MD->asString(context), X("--02-29+02:00")); 

  casted_Mon = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH, context);
  assertEquals(casted_Mon->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH);
  assertEquals(casted_Mon->asString(context), X("--02--+02:00")); 

  casted_tim = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME, context);
  assertEquals(casted_tim->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME);
  assertEquals(casted_tim->asString(context), X("11:20:10.0564Z")); 
  
  casted_YM = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH, context);
  assertEquals(casted_YM->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH);
  assertEquals(casted_YM->asString(context), X("2000-02+02:00")); 
  
  casted_Yr = dT->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR, context);  
  assertEquals(casted_Yr->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR);
  assertEquals(casted_Yr->asString(context), X("2000+02:00")); 


  //test ATTimeOrDerived::castAs
  casted_uA = tim->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
  assertEquals(casted_uA->asString(context), X("13:20:10.0564+14:00")); 

  casted_str = tim->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);
  assertEquals(casted_str->asString(context), X("13:20:10.0564+14:00")); 

  casted_tim = tim->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME, context);
  assertEquals(casted_tim->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_TIME);

  // No longer possible? - jpcs
//   casted_dT = tim->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME, context);
//   assertEquals(casted_dT->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DATETIME);
  /// Depends on current date
  // assertEquals(casted_dT->asString(context), X("2003-08-21T23:20:10.0564Z")); 


  //test ATDurationOrDerived::castAs (includes dayTimeDuration and yearMonthDuration)
  //duration
  casted_uA = dur->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = dur->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_dur = dur->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION, context);
  assertEquals(casted_dur->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION);

  casted_dTD = dur->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATDurationOrDerived::fgDT_DAYTIMEDURATION, context);
  assertEquals(casted_dTD->getTypeName(), ATDurationOrDerived::fgDT_DAYTIMEDURATION);
  assertEquals(casted_dTD->asString(context), X("P35DT13H20M10.69S")); 

  casted_yMD = dur->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATDurationOrDerived::fgDT_YEARMONTHDURATION, context);
  assertEquals(casted_yMD->getTypeName(), ATDurationOrDerived::fgDT_YEARMONTHDURATION);
  assertEquals(casted_yMD->asString(context), X("P21Y11M")); 

  //dayTimeDuration
  casted_uA = dTD->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = dTD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_dTD = dTD->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATDurationOrDerived::fgDT_DAYTIMEDURATION, context);
  assertEquals(casted_dTD->getTypeName(), ATDurationOrDerived::fgDT_DAYTIMEDURATION);

  casted_dur = dTD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION, context);
  assertEquals(casted_dur->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION);
  assertEquals(casted_dur->asString(context), X("-P15DT7H21M10.69S")); 

  //yearMonthDuration
  casted_uA = yMD->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = yMD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_yMD = yMD->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATDurationOrDerived::fgDT_YEARMONTHDURATION, context);
  assertEquals(casted_yMD->getTypeName(), ATDurationOrDerived::fgDT_YEARMONTHDURATION);

  casted_dur = yMD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION, context);
  assertEquals(casted_dur->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION);
  assertEquals(casted_dur->asString(context), X("-P7Y5M")); 


  //test ATGDayOrDerived::castAs
  casted_uA = Day->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = Day->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_Day = Day->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY, context);
  assertEquals(casted_Day->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY);


  //test ATGMonthDayOrDerived::castAs
  casted_uA = MD->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = MD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_MD = MD->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY, context);
  assertEquals(casted_MD->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY);


  //test ATGMonthOrDerived::castAs
  casted_uA = Mon->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = Mon->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_Mon = Mon->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH, context);
  assertEquals(casted_Mon->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH);


  //test ATGYearMonthOrDerived::castAs
  casted_uA = YM->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = YM->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_YM = YM->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH, context);
  assertEquals(casted_YM->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH);


  //test ATGYearOrDerived::castAs
  casted_uA = Yr->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = Yr->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_Yr = Yr->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR, context);
  assertEquals(casted_Yr->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR);


  //test ATStringOrDerived::castAs
  casted_uA = str->castAs(FunctionConstructor::XMLChXPath2DatatypesURI, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
  assertEquals(casted_uA->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC);

  casted_str = str->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

  casted_dbl = str->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);
  assertEquals(casted_dbl->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE);
  assertEquals(casted_dbl->asString(context), X("INF")); 
  
  casted_flt = str->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context);
  assertEquals(casted_flt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT);
  assertEquals(casted_flt->asString(context), X("INF")); 


  // try casting with built-in derived datatypes.  cast them up to primitive datatypes, 
  // cast down to them from primitive datatypes, and cast them across to each other
  
  // create data to cast
  ATDecimalOrDerived::Ptr lng = context->getPathanFactory()->createDecimalOrDerived(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                                             XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_LONG,
                                                                             X("5"), context);
                               
  // create pointers for casted data
  AnyAtomicType::Ptr casted_lng;
  AnyAtomicType::Ptr casted_nCN;
  AnyAtomicType::Ptr casted_nonNegInt;

  
  // cast untypedAtomic down to NCName and back up to string
  casted_nCN = uA->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NCNAME, context);
  assertEquals(casted_nCN->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NCNAME);

  casted_str = casted_nCN->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  assertEquals(casted_str->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING);

                          
  // cast long up to decimal, down to nonNegativeInteger, and across to long
  casted_dec = lng->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);
  assertEquals(casted_dec->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL);

  casted_nonNegInt = casted_dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NONNEGATIVEINTEGER, context);
  assertEquals(casted_nonNegInt->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_NONNEGATIVEINTEGER);

  casted_lng = casted_nonNegInt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_LONG, context);
  assertEquals(casted_lng->getTypeName(), XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_LONG);


  //failed casts
  /*
  const ATQNameOrDerived::Ptr invalid_QN = getMemoryManager()->createQNameOrDerived(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                                                XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME, 
                                                                                X("http://www.fakethe.com"), X("funk"), context);
  casted_str = invalid_QN->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
  delete invalid_QN;
  
  invalid_QN = getMemoryManager()->createQNameOrDerived(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                                                      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME, X(""), X("funk"), context);
  casted_str = invalid_QN->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING, context);
 
  const ATStringOrDerived::Ptr invalid_dec = this->createString(X("NaN"), context)
  casted_dec = invalid_dec->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DECIMAL, context);

  const ATAnySimpleType::Ptr invalid_dbl = getMemoryManager()->createAnySimpleType(X("NoN"), context);
  casted_dbl = invalid_dbl->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DOUBLE, context);

  const ATUntypedAtomic::Ptr invalid_flt = getMemoryManager()->createUntypedAtomic(X("UNF"), context);
  casted_flt = invalid_flt->castAs(XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_FLOAT, context); 
  */  
}

const ATAnyURIOrDerived::Ptr CastAsTester::createURI(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYURI,
      value, context);
}

const ATBase64BinaryOrDerived::Ptr CastAsTester::createBinary(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BASE64BINARY,
      value, context);
}

const ATBooleanOrDerived::Ptr CastAsTester::createBoolean(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createBooleanOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_BOOLEAN,
      value, context);
}

const ATGDayOrDerived::Ptr CastAsTester::createGDay(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGDayOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DAY,
      value, context);
}

const ATDurationOrDerived::Ptr CastAsTester::createDuration(const XMLCh* value,
                                             const DynamicContext* context) {
  return context->getPathanFactory()->createDurationOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_DURATION,
      value, context);
}

const ATHexBinaryOrDerived::Ptr CastAsTester::createHexBinary(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_HEXBINARY,
      value, context);
}

const ATGMonthDayOrDerived::Ptr CastAsTester::createGMonthDay(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGMonthDayOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTHDAY,
      value, context);
}


const ATGMonthOrDerived::Ptr CastAsTester::createGMonth(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGMonthOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_MONTH,
      value, context);
}

const ATQNameOrDerived::Ptr CastAsTester::createQName(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createDerivedFromAtomicType(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME,
      value, context);
}

const ATStringOrDerived::Ptr CastAsTester::createString(const XMLCh* value,
                                   const DynamicContext* context) {
  return context->getPathanFactory()->createStringOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING,
      value, context);
}

const ATGYearMonthOrDerived::Ptr CastAsTester::createGYearMonth(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGYearMonthOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEARMONTH,
      value, context);
}

const ATGYearOrDerived::Ptr CastAsTester::createGYear(const XMLCh* value,
                                                   const DynamicContext* context) {
  return context->getPathanFactory()->createGYearOrDerived(
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
      XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_YEAR,
      value, context);
}
