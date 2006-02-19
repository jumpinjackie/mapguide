/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * Sum function -- takes a sequence, returns a value of the same type as the items which is 
 *                 the sum of the values.
*/

#ifndef _FUNCTIONSUM_HPP
#define _FUNCTIONSUM_HPP

#include <pathan/functionAPI/AggregateFunction.hpp>

/** Sum operator function*/
class PATHAN_EXPORT FunctionSum : public AggregateFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSum(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Takes a list of nodes and sums their values */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static Item::Ptr sum(const Sequence &sequence, DynamicContext *context);

};
#endif // _FUNCTIONSUM_HPP
