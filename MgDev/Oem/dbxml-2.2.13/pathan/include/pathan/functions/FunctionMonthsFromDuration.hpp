/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETMONTHSFROMYEARMONTHDURATION_HPP
#define _FUNCTIONGETMONTHSFROMYEARMONTHDURATION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter function for the months component of yearMonthDuration.
 *
 * months-from-duration(yearMonthDuration? $srcval) => integer?
 */
class PATHAN_EXPORT FunctionMonthsFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionMonthsFromDuration(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the months component in the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMONTHSFROMYEARMONTHDURATION_HPP



