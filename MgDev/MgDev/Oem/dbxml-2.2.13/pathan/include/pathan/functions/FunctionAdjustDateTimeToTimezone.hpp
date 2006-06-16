/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP
#define _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Adjusts a dateTime to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-dateTime-to-timezone(dateTime? $srcval) => dateTime?
 *  fn:adjust-dateTime-to-timezone(dateTime? $srcval, dayTimeDuration? $timezone) => dateTime?
 */
class PATHAN_EXPORT FunctionAdjustDateTimeToTimezone : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustDateTimeToTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  /**
   * Adjusts a dateTime to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP



