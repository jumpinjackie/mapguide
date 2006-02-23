/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 fn:nilled($arg as node) as boolean?
*/

#ifndef _FUNCTIONNILLED_HPP
#define _FUNCTIONNILLED_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class PATHAN_EXPORT FunctionNilled : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNilled(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONNILLED_HPP

