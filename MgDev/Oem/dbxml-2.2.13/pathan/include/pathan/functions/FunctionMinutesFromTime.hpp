/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETMINUTESFROMTIME_HPP
#define _FUNCTIONGETMINUTESFROMTIME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Getter function for minutes component of time.
 *
 * xf:minutes-from-time(time? $srcval) => integer?
 */
class PATHAN_EXPORT FunctionMinutesFromTime : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionMinutesFromTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  /**
   * Returns the integer representing the minutes in the value of $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMINUTESFROMTIME_HPP



