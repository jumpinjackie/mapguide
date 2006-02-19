/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * xf:exactly-one(item* $srcval) => item
 */

#ifndef _FUNCTIONEXACTLYONE_HPP
#define _FUNCTIONEXACTLYONE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionExactlyOne : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionExactlyOne(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class ExactlyOneResult : public ResultImpl
  {
  public:
    ExactlyOneResult(const FunctionExactlyOne *func, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionExactlyOne *_func;
    Result _arg;
    unsigned int _argNo;
  };
};

#endif // _FUNCTIONEXACTLYONE_HPP
