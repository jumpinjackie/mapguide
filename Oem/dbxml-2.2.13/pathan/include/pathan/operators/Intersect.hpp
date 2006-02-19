/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   Op Intersect 
The intersect operator takes two node sequences as operands and returns a sequence containing all the nodes that occur in both operands.
*/

#ifndef _INTERSECT_HPP
#define _INTERSECT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemOperator.hpp>

class PATHAN_EXPORT Intersect : public DataItemOperator
{
public:
  static const XMLCh name[];

  Intersect(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** exectutes the operator*/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif 
