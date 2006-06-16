

/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONADJUSTDATETOTIMEZONE_HPP
#define _FUNCTIONADJUSTDATETOTIMEZONE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Adjusts a date to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-date-to-timezone(date? $srcval) => date?
 *  fn:adjust-date-to-timezone(date? $srcval, dayTimeDuration? $timezone) => date?
 */
class PATHAN_EXPORT FunctionAdjustDateToTimezone : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustDateToTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  /**
   * Adjusts a date to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTDATETOTIMEZONE_HPP



