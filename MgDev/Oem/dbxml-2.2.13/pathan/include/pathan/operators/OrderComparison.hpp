/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ORDERCOMPARISON_HPP
#define _ORDERCOMPARISON_HPP

#include <pathan/Pathan.hpp>

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

class PATHAN_EXPORT OrderComparison : public DataItemOperator
{
public:
  static const XMLCh name[];

  ///testBefore should be set to true if you want to test that the first parameter is before the second.
  OrderComparison(const VectorOfDataItems &args, bool testBefore, XPath2MemoryManager* memMgr);

  Result createResult(DynamicContext* context, int flags=0) const;

  bool getTestBefore() const;

private:
  class OrderComparisonResult : public SingleResult
  {
  public:
    OrderComparisonResult(const OrderComparison *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const OrderComparison *_op;
  };

	bool _testBefore;
};

#endif // _FUNCTIONOPPLUS_HPP
