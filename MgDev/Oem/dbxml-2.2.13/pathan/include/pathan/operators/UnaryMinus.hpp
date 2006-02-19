/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   UnaryMinus operator function
*/

#ifndef _UNARYMINUS_HPP
#define _UNARYMINUS_HPP

#include <pathan/Pathan.hpp>
#include "ArithmeticOperator.hpp"

/** Unary Minus operator function*/
class PATHAN_EXPORT UnaryMinus : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  UnaryMinus(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _UNARYMINUS_HPP

