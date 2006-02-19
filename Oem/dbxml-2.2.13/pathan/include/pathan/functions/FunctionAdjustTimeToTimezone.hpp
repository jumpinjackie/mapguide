/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONADJUSTTIMETOTIMEZONE_HPP
#define _FUNCTIONADJUSTTIMETOTIMEZONE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Adjusts a time to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-time-to-timezone(time? $srcval) => time?
 *  fn:adjust-time-to-timezone(time? $srcval, dayTimeDuration? $timezone) => time?
 */
class PATHAN_EXPORT FunctionAdjustTimeToTimezone : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustTimeToTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  /**
   * Adjusts a time to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTTIMETOTIMEZONE_HPP
