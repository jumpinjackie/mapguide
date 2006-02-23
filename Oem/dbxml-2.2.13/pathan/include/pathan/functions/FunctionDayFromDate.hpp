/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETDAYFROMDATE_HPP
#define _FUNCTIONGETDAYFROMDATE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Get function for the day component of date.
 *
 * xf:day-from-date(date $srcval) => integer
 */
class PATHAN_EXPORT FunctionDayFromDate : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionDayFromDate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  /**
   * Returns the integer representing the day in the value of $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETDAYFROMDATE_HPP



