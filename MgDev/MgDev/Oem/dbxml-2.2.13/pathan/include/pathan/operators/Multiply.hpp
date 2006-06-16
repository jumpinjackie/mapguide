/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Multiply operator function
*/

#ifndef _MULTIPLY_HPP
#define _MULTIPLY_HPP

#include <pathan/Pathan.hpp>

#include "ArithmeticOperator.hpp"

/** OpTimes operator function*/
class PATHAN_EXPORT Multiply : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Multiply(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _FUNCTIONOPTIMES_HPP

