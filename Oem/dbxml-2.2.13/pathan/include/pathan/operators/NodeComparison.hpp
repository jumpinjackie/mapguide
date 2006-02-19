/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _NODECOMPARISON_HPP
#define _NODECOMPARISON_HPP

#include <pathan/Pathan.hpp>

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

/** NodeComparison operator function*/
class PATHAN_EXPORT NodeComparison : public DataItemOperator
{
public:
  static const XMLCh name[];

  NodeComparison(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class NodeComparisonResult : public SingleResult
  {
  public:
    NodeComparisonResult(const NodeComparison *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const NodeComparison *_op;
  };
};

#endif // _FUNCTIONOPPLUS_HPP
