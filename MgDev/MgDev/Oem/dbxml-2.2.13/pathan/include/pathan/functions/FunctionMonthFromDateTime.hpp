/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETMONTHFROMDATETIME_HPP
#define _FUNCTIONGETMONTHFROMDATETIME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Getter function for the month component of dateTime.
 *
 * month-from-dateTime(dateTime $srcval) => integer
 */
class PATHAN_EXPORT FunctionMonthFromDateTime : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionMonthFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  /**
   * Returns an integer representing the month component in the value of 
   * $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMONTHFROMDATETIME_HPP



