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
 * $Id: FunctionOneOrMore.cpp,v 1.12 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/functions/FunctionOneOrMore.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

const XMLCh FunctionOneOrMore::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionOneOrMore::minArgs = 1;
const unsigned int FunctionOneOrMore::maxArgs = 1;

/*
 * fn:one-or-more($arg as item()*) as item()+
 */

FunctionOneOrMore::FunctionOneOrMore(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::ITEM_TYPE;
}

Result FunctionOneOrMore::createResult(DynamicContext* context, int flags) const
{
  return new OneOrMoreResult(this, flags);
}

FunctionOneOrMore::OneOrMoreResult::OneOrMoreResult(const FunctionOneOrMore *func, int flags)
  : ResultImpl(func),
    _flags(flags),
    _func(func),
    _arg(0),
    _argNo(0)
{
}

Item::Ptr FunctionOneOrMore::OneOrMoreResult::next(DynamicContext *context)
{
  if(_arg.isNull()) {
    _arg = _func->getParamNumber(1, context);
  }

  const Item::Ptr result = _arg->next(context);
  ++_argNo;

  if(_argNo == 1 && result == NULLRCP) {
    XQThrow(FunctionException, X("FunctionOneOrMore::OneOrMoreResult::next"),X("fn:one-or-more called with a sequence containing no items [err:FORG0004]"));
  }

  return result;
}

std::string FunctionOneOrMore::OneOrMoreResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<oneormore/>" << std::endl;

  return oss.str();
}
