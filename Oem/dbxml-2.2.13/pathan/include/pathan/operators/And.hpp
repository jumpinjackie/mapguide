/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _AND_HPP
#define _AND_HPP

#include <pathan/Pathan.hpp>

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

class PATHAN_EXPORT And : public DataItemOperator
{
public:
  static const XMLCh name[];

  And(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;
  virtual DataItem* staticResolution(StaticContext *context);

private:
  class AndResult : public SingleResult
  {
  public:
    AndResult(const And *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const And *_op;
  };
};

#endif // _AND_HPP
