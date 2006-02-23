/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ARITHMETICOPERATOR_HPP
#define _ARITHMETICOPERATOR_HPP

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

class AnyAtomicType;

class PATHAN_EXPORT ArithmeticOperator : public DataItemOperator
{
public:
  ArithmeticOperator(const XMLCh* opName, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  virtual DataItem* staticResolution(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

  AnyAtomicType::Ptr getArgument(unsigned int index, DynamicContext *context) const;

  virtual Item::Ptr execute(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, DynamicContext *context) const = 0;

protected:

  class ArithmeticResult : public SingleResult
  {
  public:
    ArithmeticResult(const ArithmeticOperator *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const ArithmeticOperator *_op;
  };

};

#endif // _ARITHMETICOPERATOR_HPP
