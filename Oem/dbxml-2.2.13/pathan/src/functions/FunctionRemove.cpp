/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/functions/FunctionRemove.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionRemove::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionRemove::minArgs = 2;
const unsigned int FunctionRemove::maxArgs = 2;

/**
 * fn:remove($target as item()*, $position as xs:integer) as item()*
 */

FunctionRemove::FunctionRemove(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, integer", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::NODE_TYPE | StaticType::OTHER_TYPE;
}

Result FunctionRemove::createResult(DynamicContext* context, int flags) const
{
  return new RemoveResult(this, flags, context);
}

FunctionRemove::RemoveResult::RemoveResult(const FunctionRemove *func, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _func(func),
    _position(0),
    _one(0),
    _i(0),
    _removeDone(false),
    _target(0)
{
}

Item::Ptr FunctionRemove::RemoveResult::next(DynamicContext *context)
{
  if(_position == NULLRCP) {
    _position = ((const ATDecimalOrDerived::Ptr )_func->getParamNumber(2, context).next(context));
    _one = context->getPathanFactory()->createInteger(1, context);
    _i = _one;
    _target = _func->getParamNumber(1, context);
  }

  if(!_removeDone) {
    if(_position->equals((const AnyAtomicType::Ptr)_i, context)) {
      _target.next(context);
      _removeDone = true;
    }
    else {
      _i = _i->add(_one, context);
    }
  }

  return _target.next(context);
}

std::string FunctionRemove::RemoveResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<remove/>" << std::endl;

  return oss.str();
}
