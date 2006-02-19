/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   Position function

*/

#ifndef _FUNCTIONPOSITION_HPP
#define _FUNCTIONPOSITION_HPP

#include <pathan/dataItem/DataItemFunction.hpp>

/** Position function*/
class PATHAN_EXPORT FunctionPosition : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionPosition(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONPOSITION_HPP



