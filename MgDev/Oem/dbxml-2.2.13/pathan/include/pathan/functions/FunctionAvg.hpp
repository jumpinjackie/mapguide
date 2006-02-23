/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * Avg function -- takes a sequence, returns a vlue of the same type as the items which is
 * the average of the values
 */

#ifndef _FUNCTIONAVG_HPP
#define _FUNCTIONAVG_HPP

#include <pathan/functionAPI/AggregateFunction.hpp>

/** Avg operator function*/
class PATHAN_EXPORT FunctionAvg : public AggregateFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionAvg(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Takes a list of nodes and averages their values */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONAVG_HPP
