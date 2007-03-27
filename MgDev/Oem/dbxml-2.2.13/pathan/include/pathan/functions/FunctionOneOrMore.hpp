/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * xf:one-or-more(item* $srcval) => item+
 */

#ifndef _FUNCTIONONEORMORE_HPP
#define _FUNCTIONONEORMORE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionOneOrMore : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionOneOrMore(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class OneOrMoreResult : public ResultImpl
  {
  public:
    OneOrMoreResult(const FunctionOneOrMore *func, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionOneOrMore *_func;
    Result _arg;
    unsigned int _argNo;
  };
};

#endif // _FUNCTIONONEORMORE_HPP
