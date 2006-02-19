/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Integer divide operator function
*/

#ifndef _INTEGER_DIVIDE_HPP
#define _INTEGER_DIVIDE_HPP

#include <pathan/Pathan.hpp>
#include "ArithmeticOperator.hpp"

/** OpIntegerDivide operator function*/
class PATHAN_EXPORT IntegerDivide : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  IntegerDivide(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _INTEGER_DIVIDE_HPP

