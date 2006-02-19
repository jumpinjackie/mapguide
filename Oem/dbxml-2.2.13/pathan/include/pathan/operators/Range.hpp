/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _RANGE_HPP
#define _RANGE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemOperator.hpp>

class Numeric;

/** Range operator function*/
class PATHAN_EXPORT Range : public DataItemOperator
{
public:
  static const XMLCh name[];

  ///Constructor
  Range(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class RangeResult : public ResultImpl
  {
  public:
    RangeResult(const Range *op, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const Range *_op;
    Numeric::Ptr _last;
    Numeric::Ptr _step;
    Numeric::Ptr _end;
  };
};

#endif // _RANGE_HPP


