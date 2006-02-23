/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Data function
*/

#ifndef _FUNCTIONDATA_HPP
#define _FUNCTIONDATA_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class PATHAN_EXPORT FunctionData : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionData(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  virtual DataItem* staticResolution(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDATA_HPP



