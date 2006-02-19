/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   Op Except 
The except operator takes two node sequences as operands and returns a sequence containing all the nodes that occur in the first operand
but not in the second operand
*/

#ifndef _EXCEPT_HPP
#define _EXCEPT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemOperator.hpp>
#include <pathan/dataItem/DataItemNav.hpp>
#include <pathan/dataItem/ResultBuffer.hpp>

class PATHAN_EXPORT Except : public DataItemOperator
{
public:
  static const XMLCh name[];

  Except(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class ExceptResult : public ResultImpl
  {
  public:
    ExceptResult(const Except *op, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;  
  private:
    const Except *_op;
    unsigned int _flags;
    bool _toDo;
    Result _result;
    ResultBuffer _excpt;
  };
};

#endif 
