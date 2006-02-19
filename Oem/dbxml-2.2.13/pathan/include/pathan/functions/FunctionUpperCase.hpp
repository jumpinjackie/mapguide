/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONUPPERCASE_HPP
#define _FUNCTIONUPPERCASE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Returns the given value after translating every lower-case letter to its
 * upper-case correspondent.
 *
 * xf:upper-case(string? $srcval) =>  string?
 * 
 */
class PATHAN_EXPORT FunctionUpperCase : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionUpperCase(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns the upper-case string.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONUPPERCASE_HPP



