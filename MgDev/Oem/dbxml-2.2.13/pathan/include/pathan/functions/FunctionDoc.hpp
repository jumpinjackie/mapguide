/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Doc function

*/

#ifndef _FUNCTIONDOC_HPP
#define _FUNCTIONDOC_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class PATHAN_EXPORT FunctionDoc : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDoc(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDOC_HPP
