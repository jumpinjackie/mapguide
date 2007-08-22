/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: AnyAtomicType.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include "../exceptions/InvalidLexicalSpaceException.hpp"
#include <xqilla/exceptions/TypeNotFoundException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

/* anyAtomicType */
const XMLCh AnyAtomicType::fgDT_ANYATOMICTYPE[]=
{ 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};

/* http://www.w3.org/2001/XMLSchema,anyAtomicType */
const XMLCh AnyAtomicType::fgDT_ANYATOMICTYPE_XERCESHASH[]=
{ 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,       XERCES_CPP_NAMESPACE_QUALIFIER chColon,         XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3,       XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,       XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, 
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1,       XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_S,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chComma, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,       XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,       XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};

const AnyAtomicType::CastTable AnyAtomicType::staticCastTable;

/* isAtomicValue from Item */
bool AnyAtomicType::isAtomicValue() const {
  return true;
}

/* isNode from Item */
bool AnyAtomicType::isNode() const {
  return !isAtomicValue();
}

/* is this type numeric?  Return false by default */
bool AnyAtomicType::isNumericValue() const {
  return false;
}

/* is this type date or time based?  Return false by default */
bool AnyAtomicType::isDateOrTimeTypeValue() const {
  return false;
}

/* Get the namespace uri of the primitive type (basic type) of this type */
const XMLCh* AnyAtomicType::getPrimitiveTypeURI() const{
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA;
}

AnyAtomicType::Ptr AnyAtomicType::castAs(const XMLCh* targetTypeURI, const XMLCh* targetTypeName, const DynamicContext* context) const {
  bool isPrimitive;
  AtomicObjectType targetIndex = context->getItemFactory()->getPrimitiveTypeIndex(targetTypeURI, targetTypeName, isPrimitive);

  if(!castIsSupported(targetIndex, context)) {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
    buffer.set(X("Casting from {"));
    buffer.append(this->getTypeURI());
    buffer.append(X("}"));
    buffer.append(this->getTypeName());
    buffer.append(X(" to {"));
    buffer.append(targetTypeURI);
    buffer.append(X("}"));
    buffer.append(targetTypeName);
    buffer.append(X(" is not supported [err:XPTY0004]"));

    XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), buffer.getRawBuffer());
  }
  
  try {
    return this->castAsInternal(targetIndex, targetTypeURI, targetTypeName, context);
  } catch (TypeNotFoundException &e) {
    XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), e.getError());
  } catch (InvalidLexicalSpaceException &e) {
    if(this->getPrimitiveTypeIndex() == UNTYPED_ATOMIC ||
       this->getPrimitiveTypeIndex() == ANY_SIMPLE_TYPE || 
       this->getPrimitiveTypeIndex() == STRING) {
      XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), X("Invalid lexical value [err:FORG0001]"));
    } else if (context->isTypeOrDerivedFromType(targetTypeURI, targetTypeName, this->getPrimitiveTypeURI(), this->getPrimitiveTypeName())) {
      XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), X("Value does not conform to facets [err:FORG0001]"));
    } else {
      XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), e.getError());  // should never be here, in theory
    }
  } catch (NamespaceLookupException &e) {
    XQThrow2(XPath2TypeCastException, X("AnyAtomicType::castAs"), e.getError());
  }

}

AnyAtomicType::Ptr AnyAtomicType::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetTypeURI, const XMLCh* targetTypeName, const DynamicContext* context) const {
    return context->getItemFactory()->createDerivedFromAtomicType(targetIndex, targetTypeURI, targetTypeName, this->asString(context), context);
}

/* Test if this type can be cast to the target type */
bool AnyAtomicType::castable(const XMLCh* targetTypeURI, const XMLCh* targetTypeName, const DynamicContext* context) const {
  bool isPrimitive;
  AtomicObjectType targetIndex = context->getItemFactory()->getPrimitiveTypeIndex(targetTypeURI, targetTypeName, isPrimitive);

  if(!castIsSupported(targetIndex, context)) {
          return false;
  }
  // validate the data by calling castAs (can't use checkInstance)
  try {
    this->castAsInternal(targetIndex, targetTypeURI, targetTypeName, context);
  } catch (IllegalArgumentException &e) {
    return false;
  } catch (XPath2TypeCastException &e) {
    return false;
  } catch (TypeNotFoundException &e) {
    return false;
  } catch (InvalidLexicalSpaceException &e) {
    return false;
  } catch (NamespaceLookupException &e) {
    return false;
  }
  return true;
}

/* Returns true if this typeName and uri match the given typeName and uri */
bool AnyAtomicType::isOfType(const XMLCh* targetTypeURI, const XMLCh* targetTypeName, const DynamicContext* context) const {
  return (XPath2Utils::equals(targetTypeName, this->getTypeName())) &&
         (XPath2Utils::equals(targetTypeURI,  this->getTypeURI()));
}

/* Returns true if this typeName and uri match the given type name and uri,
 * or if any of this type's parents match the given type name and uri */
bool AnyAtomicType::isInstanceOfType(const XMLCh* targetTypeURI, const XMLCh* targetTypeName, const StaticContext* context) const {
  return context->isTypeOrDerivedFromType(this->getTypeURI(), this->getTypeName(), targetTypeURI, targetTypeName);
}


/**
  * Returns true if 
  * (a) both the input type and the target type are built-in schema types and 
  *     cast is supported for the combination.
  * (b) cast is supported if the input type is a derived atomic type and the 
  *     target type is a supertype of the input type
  * (c) cast is supported if the target type is a derived atomic type and the 
  *     input type is xs:string, xs:anySimpleType, or a supertype of the 
  *     target type.
  * (d) If a primitive type P1 can be cast into a primitive type P2, then any 
  *     subtype of P1 can be cast into any subtype of P2
 **/
bool AnyAtomicType::castIsSupported(AtomicObjectType targetIndex, const DynamicContext* context) const {
  return staticCastTable.getCell(getPrimitiveTypeIndex(), targetIndex);
}


///////////////////
// methods from  //
//   CastTable   //
///////////////////

/* This is the table we will mimic, but we will make it enum safe :-)
 * 
       uA str flt dbl dec int dur yMD dTD dT tim dat gYM gYr gMD gDay gMon bol b64 hxB aURI QN NOT
uA  { { 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,   1,   1,  1,  1,  1,  1,  0,  0 },
str   { 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,   1,   1,  1,  1,  1,  1,  1,  1 },
flt   { 1, 1,  1,  1,  1,  1,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  1,  0,  0,  0,  0,  0 },
dbl   { 1, 1,  1,  1,  1,  1,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  1,  0,  0,  0,  0,  0 },
dec   { 1, 1,  1,  1,  1,  1,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  1,  0,  0,  0,  0,  0 },
int   { 1, 1,  1,  1,  1,  1,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  1,  0,  0,  0,  0,  0 },
dur   { 1, 1,  0,  0,  0,  0,  1,  1,  1,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0 },
yMD   { 1, 1,  0,  0,  0,  0,  1,  1,  1,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0 },
dTD   { 1, 1,  0,  0,  0,  0,  1,  1,  1,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0 },
dT    { 1, 1,  0,  0,  0,  0,  0,  0,  0,  1, 1,  1,  1,  1,  1,   1,   1,  0,  0,  0,  0,  0,  0 }, 
tim   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 1,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0 }, 
dat   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  1, 0,  1,  1,  1,  1,   1,   1,  0,  0,  0,  0,  0,  0 }, 
gYM   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  1,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0 }, 
gYr   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  1,  0,   0,   0,  0,  0,  0,  0,  0,  0 }, 
gMD   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  1,   0,   0,  0,  0,  0,  0,  0,  0 }, 
gDay  { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   1,   0,  0,  0,  0,  0,  0,  0 }, 
gMon  { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   1,  0,  0,  0,  0,  0,  0 }, 
bool  { 1, 1,  1,  1,  1,  1,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  1,  0,  0,  0,  0,  0 }, 
b64   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  0,  1,  1,  0,  0,  0 }, 
hxB   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  0,  1,  1,  0,  0,  0 }, 
aURI  { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1,  0,  0 }, 
QN,   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  1,  0 }, 
NOT   { 1, 1,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  1 } };
*/  


AnyAtomicType::CastTable::CastTable() {
  // initialize table to allow casting only between same types
  for(int j = 0; j<(int)AnyAtomicType::NumAtomicObjectTypes; j++)
  {
    for(int k = 0; k<(int)AnyAtomicType::NumAtomicObjectTypes; k++)
    {
      staticCastTable[j][k] = false;
      if(k==AnyAtomicType::ANY_SIMPLE_TYPE)
        continue;
      staticCastTable[AnyAtomicType::STRING][k] = true;  // strings can be cast to anything
      if(k!=AnyAtomicType::NOTATION && k!=AnyAtomicType::QNAME)
      	staticCastTable[AnyAtomicType::UNTYPED_ATOMIC][k] = true;  // untypedAtomic can be cast to almost anything
    }
    staticCastTable[j][j] = true; // you can always cast something to itself
    // if the source is not QNAME, allow casting to STRING and UNTYPED_ATOMIC
    if(j==AnyAtomicType::QNAME || j==AnyAtomicType::ANY_SIMPLE_TYPE)
      continue;
    staticCastTable[j][AnyAtomicType::STRING] = true;  // almost anything can be cast to string
    staticCastTable[j][AnyAtomicType::UNTYPED_ATOMIC] = true;  // almost anything can be cast to untypedAtomic
  }

  // finally, add special casting rules
  staticCastTable[AnyAtomicType::FLOAT][AnyAtomicType::DOUBLE] = true;
  staticCastTable[AnyAtomicType::DOUBLE][AnyAtomicType::FLOAT] = true;
  
  staticCastTable[AnyAtomicType::FLOAT][AnyAtomicType::DECIMAL] = true;
  staticCastTable[AnyAtomicType::DECIMAL][AnyAtomicType::FLOAT] = true;
  
  staticCastTable[AnyAtomicType::FLOAT][AnyAtomicType::BOOLEAN] = true;
  staticCastTable[AnyAtomicType::BOOLEAN][AnyAtomicType::FLOAT] = true;
  
  staticCastTable[AnyAtomicType::DOUBLE][AnyAtomicType::DECIMAL] = true;
  staticCastTable[AnyAtomicType::DECIMAL][AnyAtomicType::DOUBLE] = true;
  
  staticCastTable[AnyAtomicType::DOUBLE][AnyAtomicType::BOOLEAN] = true;
  staticCastTable[AnyAtomicType::BOOLEAN][AnyAtomicType::DOUBLE] = true;
  
  staticCastTable[AnyAtomicType::DECIMAL][AnyAtomicType::BOOLEAN] = true;
  staticCastTable[AnyAtomicType::BOOLEAN][AnyAtomicType::DECIMAL] = true;
  
  staticCastTable[AnyAtomicType::DURATION][AnyAtomicType::YEAR_MONTH_DURATION] = true;
  staticCastTable[AnyAtomicType::YEAR_MONTH_DURATION][AnyAtomicType::DURATION] = true;
  
  staticCastTable[AnyAtomicType::DURATION][AnyAtomicType::DAY_TIME_DURATION] = true;
  staticCastTable[AnyAtomicType::DAY_TIME_DURATION][AnyAtomicType::DURATION] = true;
  
  staticCastTable[AnyAtomicType::DAY_TIME_DURATION][AnyAtomicType::YEAR_MONTH_DURATION] = true;
  staticCastTable[AnyAtomicType::YEAR_MONTH_DURATION][AnyAtomicType::DAY_TIME_DURATION] = true;
  
  staticCastTable[AnyAtomicType::DURATION][AnyAtomicType::DAY_TIME_DURATION] = true;
  staticCastTable[AnyAtomicType::DAY_TIME_DURATION][AnyAtomicType::DURATION] = true;

  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::DATE] = true;
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::DATE_TIME] = true;
  
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::TIME] = true;
  
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::G_YEAR_MONTH] = true;
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::G_YEAR]       = true;
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::G_MONTH_DAY]  = true;
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::G_DAY]        = true;
  staticCastTable[AnyAtomicType::DATE_TIME][AnyAtomicType::G_MONTH]      = true;
  
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::G_YEAR_MONTH] = true;
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::G_YEAR]       = true;
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::G_MONTH_DAY]  = true;
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::G_DAY]        = true;
  staticCastTable[AnyAtomicType::DATE][AnyAtomicType::G_MONTH]      = true;

  staticCastTable[AnyAtomicType::BASE_64_BINARY][AnyAtomicType::HEX_BINARY]      = true;
  staticCastTable[AnyAtomicType::HEX_BINARY][AnyAtomicType::BASE_64_BINARY]      = true;

  staticCastTable[AnyAtomicType::QNAME][AnyAtomicType::UNTYPED_ATOMIC] = true;
  staticCastTable[AnyAtomicType::QNAME][AnyAtomicType::STRING] = true;
/*  debug output
 *  for(int i = 0; i<(int)AnyAtomicType::NumAtomicObjectTypes; i++) {
    for(int j = 0; j<(int)AnyAtomicType::NumAtomicObjectTypes; j++) {
       cerr << " " << staticCastTable[i][j] << ","<<flush; 
    }
    cerr << endl;
  }
*/      
  
}

bool AnyAtomicType::CastTable::getCell(AnyAtomicType::AtomicObjectType source, 
                AnyAtomicType::AtomicObjectType target) const {
  return staticCastTable[source][target];
  
}
