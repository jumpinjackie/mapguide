/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONREMOVE_HPP
#define _FUNCTIONREMOVE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionRemove : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRemove(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class RemoveResult : public ResultImpl
  {
  public:
    RemoveResult(const FunctionRemove *func, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    int _flags;
    const FunctionRemove *_func;
    Numeric::Ptr _position;
    Numeric::Ptr _one;
    Numeric::Ptr _i;
    bool _removeDone;
    Result _target;
  };
};

#endif // _FUNCTIONREMOVE_HPP
