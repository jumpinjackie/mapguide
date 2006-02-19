/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter for the days component of dayTimeDuration.
 *
 *  days-from-Duration(dayTimeDuration? $srcval) => integer?
 */
class PATHAN_EXPORT FunctionDaysFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionDaysFromDuration(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the days component of the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP



