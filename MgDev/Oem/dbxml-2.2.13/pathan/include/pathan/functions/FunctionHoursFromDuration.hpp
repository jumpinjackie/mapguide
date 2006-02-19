/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETHOURSFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETHOURSFROMDAYTIMEDURATION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter for the hours component of dayTimeDuration.
 *
 * hours-from-duration(dayTimeDuration? $srcval) => integer?
 */
class PATHAN_EXPORT FunctionHoursFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionHoursFromDuration(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the hours component in the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETHOURSFROMDAYTIMEDURATION_HPP



