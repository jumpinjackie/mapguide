/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _COMPARISONOPERATOR_HPP
#define _COMPARISONOPERATOR_HPP

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

class AnyAtomicType;

class PATHAN_EXPORT ComparisonOperator : public DataItemOperator
{
public:
  ComparisonOperator(const XMLCh* opName, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Result createResult(DynamicContext* context, int flags=0) const;

  AnyAtomicType::Ptr getArgument(unsigned int index, DynamicContext *context) const;

  virtual bool execute(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, DynamicContext *context) const = 0;

protected:

  class ComparisonResult : public SingleResult
  {
  public:
    ComparisonResult(const ComparisonOperator *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const ComparisonOperator *_op;
  };
};

#endif // _COMPARISONOPERATOR_HPP
