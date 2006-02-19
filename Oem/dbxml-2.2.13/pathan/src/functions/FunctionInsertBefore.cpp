/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/functions/FunctionInsertBefore.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

const XMLCh FunctionInsertBefore::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionInsertBefore::minArgs = 3;
const unsigned int FunctionInsertBefore::maxArgs = 3;

/**
 * fn:insert-before($target as item()*, $position as xs:integer, $inserts as item()*) as item()*
 */

FunctionInsertBefore::FunctionInsertBefore(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, integer, item()*", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::NODE_TYPE | StaticType::OTHER_TYPE;
}

Result FunctionInsertBefore::createResult(DynamicContext* context, int flags) const
{
  return new InsertBeforeResult(this, flags, context);
}

FunctionInsertBefore::InsertBeforeResult::InsertBeforeResult(const FunctionInsertBefore *func, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _func(func),
    _position(0),
    _one(0),
    _i(0),
    _insertsDone(false),
    _target(0),
    _inserts(0)
{
}

Item::Ptr FunctionInsertBefore::InsertBeforeResult::next(DynamicContext *context)
{
  if(_position == NULLRCP) {
    _position = ((const ATDecimalOrDerived::Ptr )_func->getParamNumber(2, context).next(context));
    _one = context->getPathanFactory()->createInteger(1, context);
    _i = _one;
    _target = _func->getParamNumber(1, context, _flags);

    if(!_position->greaterThan(_one, context)) {
      _inserts = _func->getParamNumber(3, context, _flags);
    }
  }

  Item::Ptr result = 0;
  while(result == NULLRCP) {
    if(!_inserts.isNull()) {
      result = _inserts.next(context);
      if(result == NULLRCP) {
        _inserts = 0;
        _insertsDone = true;
      }
    }
    else {
      result = _target.next(context);
      if(!_insertsDone) {
        _i = _i->add(_one, context);
        if(result == NULLRCP || _position->equals((const AnyAtomicType::Ptr)_i, context)) {
          _inserts = _func->getParamNumber(3, context, _flags);
        }
      }
      else if(result == NULLRCP) {
        break;
      }
    }
  }

  return result;
}

std::string FunctionInsertBefore::InsertBeforeResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<insertbefore/>" << std::endl;

  return oss.str();
}
