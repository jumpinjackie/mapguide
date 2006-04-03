/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Distinct-values function
  
*/

#ifndef _FUNCTIONDISTINCTVALUES_HPP
#define _FUNCTIONDISTINCTVALUES_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

// Distinct values is not really an aggregate function, but since its behaviour is so
// similar (in terms of sequence validation) we will extend from it.
class PATHAN_EXPORT FunctionDistinctValues : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDistinctValues(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:  
  class DistinctValueResult : public ResultImpl
  {
  public:
    DistinctValueResult(const FunctionDistinctValues *fdv, DynamicContext *context);
    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const FunctionDistinctValues *fdv_;
    Result parent_;
    Collation *collation_;
    bool toDo_;

    bool seenNan_;
    Sequence alreadySeen_;
  };

};

#endif // _FUNCTIONDISTINCTVALUES_HPP
