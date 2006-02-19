/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONTRUE_HPP
#define _FUNCTIONTRUE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Returns the boolean value true.
 *
 * xf:true() => boolean
 */
class PATHAN_EXPORT FunctionTrue : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;
  
  /**
   * Constructor.
   */
  FunctionTrue(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns the boolean value true.
   */
  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONTRUE_HPP

