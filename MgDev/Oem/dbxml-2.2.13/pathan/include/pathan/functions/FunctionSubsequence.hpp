/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONSUBSEQUENCE_HPP
#define _FUNCTIONSUBSEQUENCE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionSubsequence : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubsequence(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class SubsequenceResult : public ResultImpl
  {
  public:
    SubsequenceResult(const FunctionSubsequence *func, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionSubsequence *_func;
    Numeric::Ptr _length;
    Numeric::Ptr _one;
    Numeric::Ptr _i;
    Result _source;
  };
};

#endif // _FUNCTIONSUBSEQUENCE_HPP





