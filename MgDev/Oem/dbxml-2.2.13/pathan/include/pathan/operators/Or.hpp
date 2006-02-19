/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _OR_HPP
#define _OR_HPP

#include <pathan/Pathan.hpp>

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

class PATHAN_EXPORT Or : public DataItemOperator
{
public:
  static const XMLCh name[];

  Or(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;
  virtual DataItem* staticResolution(StaticContext *context);

private:
  class OrResult : public SingleResult
  {
  public:
    OrResult(const Or *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const Or *_op;
  };
};

#endif // _OR_HPP
