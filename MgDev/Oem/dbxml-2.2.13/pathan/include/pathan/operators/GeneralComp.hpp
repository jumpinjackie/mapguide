/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   General Comparison operator function
*/

#ifndef _GENERALCOMP_HPP
#define _GENERALCOMP_HPP

#include <pathan/Pathan.hpp>

#include "../dataItem/DataItemOperator.hpp"
#include "../dataItem/SingleResult.hpp"

/** GeneralComp operator function*/
class PATHAN_EXPORT GeneralComp : public DataItemOperator
{
public:
  typedef enum {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL
  } ComparisonOperation;

  static const XMLCh name[];

  GeneralComp(ComparisonOperation operation, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Result createResult(DynamicContext* context, int flags=0) const;

  ComparisonOperation getOperation() const;

  static bool compare(GeneralComp::ComparisonOperation operation,
                      AnyAtomicType::Ptr first, AnyAtomicType::Ptr second,
                      Collation* collation, DynamicContext *context);

protected:
  class GeneralCompResult : public SingleResult
  {
  public:
    GeneralCompResult(const GeneralComp *op, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const GeneralComp *_op;
  };

  ComparisonOperation _operation;
};

#endif // _GENERALCOMP_HPP



