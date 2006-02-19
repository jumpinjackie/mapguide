/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/functions/FunctionSubsequence.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

const XMLCh FunctionSubsequence::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionSubsequence::minArgs = 2;
const unsigned int FunctionSubsequence::maxArgs = 3;

/**
 * fn:subsequence($sourceSeq as item()*, $startingLoc as xs:double) as item()*
 * fn:subsequence($sourceSeq as item()*, $startingLoc as xs:double, $length as xs:double) as item()*
**/

FunctionSubsequence::FunctionSubsequence(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, double, double", args, memMgr)
{
  // TBD - could do better here - jpcs
  _src.getStaticType().flags = StaticType::NUMERIC_TYPE | StaticType::NODE_TYPE | StaticType::OTHER_TYPE;
}

Result FunctionSubsequence::createResult(DynamicContext* context, int flags) const
{
  return new SubsequenceResult(this, flags, context);
}

FunctionSubsequence::SubsequenceResult::SubsequenceResult(const FunctionSubsequence *func, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _func(func),
    _length(0),
    _one(0),
    _i(0),
    _source(0)
{
}

Item::Ptr FunctionSubsequence::SubsequenceResult::next(DynamicContext *context)
{
  if(_one == NULLRCP) {
    _one = context->getPathanFactory()->createDouble(1, context);
    _source = _func->getParamNumber(1, context);
    if(_func->getNumArgs()>2) {
      _length = ((const Numeric::Ptr )_func->getParamNumber(3, context).next(context))->round(context);
    }

    _i = _one;
    const Numeric::Ptr position = ((const Numeric::Ptr )_func->getParamNumber(2, context).next(context))->round(context);
    while(_i->lessThan(position, context) && _source.next(context) != NULLRCP) {
      _i = _i->add(_one, context);
    }
    _i = _one;
  }

  if(_length != NULLRCP && _i->greaterThan(_length, context)) {
    return 0;
  }

  _i = _i->add(_one, context);
  return _source.next(context);
}

std::string FunctionSubsequence::SubsequenceResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<subsequence/>" << std::endl;

  return oss.str();
}
