/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/functions/FunctionOneOrMore.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>

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

FunctionOneOrMore::FunctionOneOrMore(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::NODE_TYPE | StaticType::OTHER_TYPE;
}

Result FunctionOneOrMore::createResult(DynamicContext* context, int flags) const
{
  return new OneOrMoreResult(this, flags, context);
}

FunctionOneOrMore::OneOrMoreResult::OneOrMoreResult(const FunctionOneOrMore *func, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _func(func),
    _arg(0),
    _argNo(0)
{
}

Item::Ptr FunctionOneOrMore::OneOrMoreResult::next(DynamicContext *context)
{
  if(_arg.isNull()) {
    _arg = _func->getParamNumber(1, context, _flags & ~(DataItem::RETURN_ONE|DataItem::RETURN_TWO));
  }

  const Item::Ptr result = _arg.next(context);
  ++_argNo;

  if(_argNo == 1 && result == NULLRCP) {
    DSLthrow(FunctionException, X("FunctionOneOrMore::OneOrMoreResult::next"),X("fn:one-or-more called with a sequence containing no items [err:FORG0004]"));
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
