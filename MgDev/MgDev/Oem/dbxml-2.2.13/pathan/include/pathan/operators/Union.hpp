/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Union function -- takes two sequences, and merge them
*/

#ifndef _UNION_HPP
#define _UNION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemNav.hpp>
#include <pathan/dataItem/DataItemOperator.hpp>
#include <pathan/dataItem/ResultImpl.hpp>

/** Union operator function*/
class PATHAN_EXPORT Union : public DataItemOperator
{
public:
  static const XMLCh name[];

  Union(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class UnionResult : public ResultImpl
  {
  public:
    UnionResult(const Union *op, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;  
  private:
    const Union *_op;
    unsigned int _flags;
    unsigned int _index;
    Result _result;
  };
};

#endif // _UNION_HPP

