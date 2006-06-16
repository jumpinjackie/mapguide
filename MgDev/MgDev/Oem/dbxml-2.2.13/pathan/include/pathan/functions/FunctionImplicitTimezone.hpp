/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   implicit-timezone function

*/


#ifndef _FUNCTIONIMPLICITTIMEZONE_HPP
#define _FUNCTIONIMPLICITTIMEZONE_HPP

#include <pathan/dataItem/DataItemFunction.hpp>

/** implicit-timezone function*/
class PATHAN_EXPORT FunctionImplicitTimezone : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionImplicitTimezone(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONIMPLICITTIMEZONE_HPP
