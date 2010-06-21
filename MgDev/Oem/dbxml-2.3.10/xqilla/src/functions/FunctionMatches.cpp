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
 * $Id: FunctionMatches.cpp,v 1.16 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionMatches.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionMatches::name[] = {
  chLatin_m, chLatin_a, chLatin_t, 
  chLatin_c, chLatin_h, chLatin_e, 
  chLatin_s, chNull 
};
const unsigned int FunctionMatches::minArgs = 2;
const unsigned int FunctionMatches::maxArgs = 3;

/**
 * fn:matches($input as xs:string?, $pattern as xs:string) as xs:boolean
 * fn:matches($input as xs:string?, $pattern as xs:string, $flags as xs:string) as xs:boolean
 */
  
FunctionMatches::FunctionMatches(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string, string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::BOOLEAN_TYPE;
}

Sequence FunctionMatches::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  const XMLCh* input = XMLUni::fgZeroLenString;
  Item::Ptr inputItem = getParamNumber(1,context)->next(context);
  if(inputItem.notNull())
    input=inputItem->asString(context);
  const XMLCh* pattern = getParamNumber(2,context)->next(context)->asString(context);
  // If the value of $operand1 is the zero-length string and the value of $operand2 is not the zero-length string,
  // then the function returns false.
  if(XMLString::stringLen(input)==0 && XMLString::stringLen(pattern)>0)
    return Sequence(context->getItemFactory()->createBoolean(false, context), memMgr);
  // If the value of $operand2 is the zero-length string, then the function returns true
  if(XMLString::stringLen(pattern)==0)
    return Sequence(context->getItemFactory()->createBoolean(true, context), memMgr);

  const XMLCh* options = XMLUni::fgZeroLenString;
  if(getNumArgs()>2)
    options=getParamNumber(3,context)->next(context)->asString(context);

  //Check that the options are valid - throw an exception if not (can have s,m,i and x)
  //Note: Are allowed to duplicate the letters.
  const XMLCh* cursor=options;
  for(; *cursor != 0; ++cursor){
    switch(*cursor) {
    case chLatin_s:
    case chLatin_m:
    case chLatin_i:
    case chLatin_x:
      break;
    default:
      XQThrow(FunctionException, X("FunctionMatches::collapseTreeInternal"),X("Invalid regular expression flags [err:FORX0001]."));
    }
  }

  //Build the Regular Expression
  try {
    // Always turn off head character optimisation, since it is broken
    XMLBuffer optionsBuf(1023, context->getMemoryManager());
    optionsBuf.set(options);
    optionsBuf.append(chLatin_H);

    RegularExpression regEx(pattern, optionsBuf.getRawBuffer(), memMgr);
    return Sequence(context->getItemFactory()->createBoolean(regEx.matches(input), context), memMgr);
  } catch (ParseException &e){ 
    XMLBuffer buf(1023, memMgr);
    buf.set(X("Invalid regular expression: "));
    buf.append(e.getMessage());
    buf.append(X(" [err:FORX0002]"));
    XQThrow(FunctionException, X("FunctionMatches::collapseTreeInternal"), buf.getRawBuffer());
  } catch (XMLException &e){ 
    XQThrow(FunctionException, X("FunctionMatches::collapseTreeInternal"), e.getMessage());  
  }  

  //do not get here

}
