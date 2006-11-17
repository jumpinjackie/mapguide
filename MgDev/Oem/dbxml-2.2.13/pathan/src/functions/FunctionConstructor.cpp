/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/Node.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <assert.h>

/**
 * pref:TYPE($arg as xdt:anyAtomicType) as pref:TYPE
**/

 /* http://www.w3.org/2001/XMLSchema */
const XMLCh FunctionConstructor::XMLChConstructorURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_1, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_S,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

 /* http://www.w3.org/2005/04/xpath-datatypes */
const XMLCh FunctionConstructor::XMLChXPath2DatatypesURI[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_w,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_3, XERCES_CPP_NAMESPACE_QUALIFIER chPeriod, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_2, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_5, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0,
    XERCES_CPP_NAMESPACE_QUALIFIER chDigit_4, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chDash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh szColon[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

FunctionConstructor::FunctionConstructor(const XMLCh* nsURI, const XMLCh* typeName,
                                         AnyAtomicType::AtomicObjectType primitiveType,
                                         const VectorOfDataItems &args, XPath2MemoryManager* memMgr) 
: ConstantFoldingFunction( typeName, 1, 1, "item()", args, memMgr)
{
  _fURI=nsURI;

  switch(primitiveType) {
  case AnyAtomicType::DECIMAL:
  case AnyAtomicType::DOUBLE:
  case AnyAtomicType::FLOAT: {
    _src.getStaticType().flags = StaticType::NUMERIC_TYPE;
    break;
  }
  default: {
    _src.getStaticType().flags = StaticType::OTHER_TYPE;
    break;
  }
  }
}

Sequence FunctionConstructor::collapseTreeInternal(DynamicContext* context, int flags) const
{
  if (context->isTypeOrDerivedFromType(_fURI, _fName, XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA,  XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgNotationString)) {
      DSLthrow(FunctionException, X("FunctionConstructor::collapseTreeInternal"), X("Construction of xs:NOTATION is forbidden"));    
  }

  Sequence arg = getParamNumber(1,context);
	
  const Item::Ptr argItem=arg.first();

  if(argItem->isNode()) {
    // get the typed value from the node
    const Item::Ptr typedValue = ((Node*)(const Item*)argItem)->dmTypedValue(context).first();
    assert(typedValue->isAtomicValue());
    return Sequence(((AnyAtomicType*)(const Item*)typedValue)->castAs(_fURI, _fName, context),  context->getMemoryManager());
    
  } else if (argItem->isAtomicValue()) {
    // cast the item to AnyAtomicType
    try {
      return Sequence(((AnyAtomicType*)(const Item*)argItem)->castAs(_fURI, _fName, context),  context->getMemoryManager());
    } catch (XPath2TypeCastException &e) {
      // Deviation from spec here: says to print "Illegal value for constructor"
      // but that is particularly unhelpful -- crioux
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer msg(1023, context->getMemoryManager());
      msg.set(X("Invalid representation of "));
      msg.append(_fName);
      
      DSLthrow(FunctionException, X("FunctionConstructor::collapseTreeInternal"), msg.getRawBuffer());
    }
    
  } 
  // this should never happen
  assert(false);
  return Sequence(context->getMemoryManager());
}
