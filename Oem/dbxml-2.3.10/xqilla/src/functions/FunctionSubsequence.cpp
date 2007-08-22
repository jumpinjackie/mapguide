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
 * $Id: FunctionSubsequence.cpp,v 1.14 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

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

FunctionSubsequence::FunctionSubsequence(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, double, double", args, memMgr)
{
}

ASTNode* FunctionSubsequence::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionSubsequence::staticTyping(StaticContext *context)
{
  _src.clear();

  ASTNode *result = calculateSRCForArguments(context);
  if(result == this) {
    _src.getStaticType() = _args.front()->getStaticResolutionContext().getStaticType();
  }
  return result;
}

Result FunctionSubsequence::createResult(DynamicContext* context, int flags) const
{
  return new SubsequenceResult(this, flags);
}

FunctionSubsequence::SubsequenceResult::SubsequenceResult(const FunctionSubsequence *func, int flags)
  : ResultImpl(func),
    _flags(flags),
    _func(func),
    _end(0),
    _one(0),
    _i(0),
    _source(0)
{
}

Item::Ptr FunctionSubsequence::SubsequenceResult::next(DynamicContext *context)
{
  if(_one == NULLRCP) {
    _one = context->getItemFactory()->createDouble(1, context);
    _source = _func->getParamNumber(1, context);

    _i = _one;
    const Numeric::Ptr position = ((const Numeric::Ptr )_func->getParamNumber(2, context)->next(context))->round(context);
    if(_func->getNumArgs()>2)
      _end = ((const Numeric::Ptr )_func->getParamNumber(3, context)->next(context))->round(context)->add(position, context);

    while(_i->lessThan(position, context) && _source->next(context) != NULLRCP) {
      _i = _i->add(_one, context);
    }
  }

  if(_end != NULLRCP && !_i->lessThan(_end, context)) {
    return 0;
  }

  _i = _i->add(_one, context);
  return _source->next(context);
}

std::string FunctionSubsequence::SubsequenceResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<subsequence/>" << std::endl;

  return oss.str();
}
