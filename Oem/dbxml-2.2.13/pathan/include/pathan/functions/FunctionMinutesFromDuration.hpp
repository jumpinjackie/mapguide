/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter function for the minutes component of dayTimeDuration.
 *
 * minutes-from-duration(dayTimeDuration? $srcval) => integer?
 */
class PATHAN_EXPORT FunctionMinutesFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;
  
  /**
   * Constructor.
   */
  FunctionMinutesFromDuration(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the minutes component in the value
   * of $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP



