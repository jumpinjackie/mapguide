/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETYEARFROMDATE_HPP
#define _FUNCTIONGETYEARFROMDATE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Getter function for year component of date.
 *
 * xf:year-from-date(date $srcval) => integer
 */
class PATHAN_EXPORT FunctionYearFromDate : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionYearFromDate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  /**
   * Returns the integer representing the year in the value of $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETYEARFROMDATE_HPP



