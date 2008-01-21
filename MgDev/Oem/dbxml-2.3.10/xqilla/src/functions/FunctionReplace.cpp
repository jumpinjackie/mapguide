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
 * $Id: FunctionReplace.cpp,v 1.17 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionReplace.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionReplace::name[] = {
  chLatin_r, chLatin_e, chLatin_p, 
  chLatin_l, chLatin_a, chLatin_c, 
  chLatin_e, chNull 
};
const unsigned int FunctionReplace::minArgs = 3;
const unsigned int FunctionReplace::maxArgs = 4;

/** 
 * fn:replace($input as xs:string?, $pattern as xs:string, $replacement as xs:string) as xs:string
 * fn:replace($input as xs:string?, $pattern as xs:string, $replacement as xs:string, $flags as xs:string) as xs:string
 */
  
FunctionReplace::FunctionReplace(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string, string, string", args, memMgr)
{
  _src.getStaticType().flags = StaticType::STRING_TYPE;
}

Sequence FunctionReplace::collapseTreeInternal(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  const XMLCh* input = XMLUni::fgZeroLenString;
  Item::Ptr inputString = getParamNumber(1,context)->next(context);
  if(inputString.notNull())
    input = inputString->asString(context);
  const XMLCh *pattern = getParamNumber(2,context)->next(context)->asString(context);
  const XMLCh *replacement = getParamNumber(3,context)->next(context)->asString(context);

  bool notEscaped = true;
  const XMLCh* ptr;
  for (ptr = replacement; *ptr != chNull; ptr++)
  {
    if ((*ptr == chDollarSign) && notEscaped) {
      ptr++;
      
      //check that after the $ is a digit 
      if (!XMLString::isDigit(*ptr))
        XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), X("Invalid replacement pattern [err:FORX0004]"));
    } else {
      //if you have a slash and then a character that's not a $ or \, 
      //then it's an invalid replace string  
      if (!notEscaped && (*ptr != chDollarSign && *ptr != chBackSlash))
        XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), X("Invalid replacement pattern [err:FORX0004]"));
      
      if (*ptr == chBackSlash)
        notEscaped = false;        
      else   
        notEscaped = true;  
    }
  }
  if(!notEscaped)
    XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), X("Invalid replacement pattern [err:FORX0004]"));

  const XMLCh *options = XMLUni::fgZeroLenString;
  if(getNumArgs()>3)
    options=getParamNumber(4,context)->next(context)->asString(context);
  
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
      XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"),X("Invalid regular expression flags [err:FORX0001]."));
    }
  }

  const XMLCh* result=NULL;
  //Now attempt to replace
  try {
    // Always turn off head character optimisation, since it is broken
    XMLBuffer optionsBuf(1023, context->getMemoryManager());
    optionsBuf.set(options);
    optionsBuf.append(chLatin_H);

    RegularExpression regEx(pattern, optionsBuf.getRawBuffer(), memMgr);
    result = regEx.replace(input, replacement);
  } catch (ParseException &e){ 
    XMLBuffer buf(1023, memMgr);
    buf.set(X("Invalid regular expression: "));
    buf.append(e.getMessage());
    buf.append(X(" [err:FORX0002]"));
    XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), buf.getRawBuffer());
  } catch (RuntimeException &e){ 
    if(e.getCode()==XMLExcepts::Regex_RepPatMatchesZeroString)
      XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), X("The pattern matches the zero-length string [err:FORX0003]"));
    else if(e.getCode()==XMLExcepts::Regex_InvalidRepPattern)
      XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), X("Invalid replacement pattern [err:FORX0004]"));
    else 
      XQThrow(FunctionException, X("FunctionReplace::collapseTreeInternal"), e.getMessage());
  }  

  return Sequence(context->getItemFactory()->createString(result, context), memMgr);
}
