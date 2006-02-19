/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Minus operator function
*/

#ifndef _MINUS_HPP
#define _MINUS_HPP

#include <pathan/Pathan.hpp>

#include "ArithmeticOperator.hpp"

/** OpMinus operator function*/
class PATHAN_EXPORT Minus : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Minus(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _FUNCTIONOPMINUS_HPP

