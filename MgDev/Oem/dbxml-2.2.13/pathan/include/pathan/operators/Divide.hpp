/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Divide operator function
*/

#ifndef _DIVIDE_HPP
#define _DIVIDE_HPP

#include <pathan/Pathan.hpp>
#include "ArithmeticOperator.hpp"

/** OpDivide operator function*/
class PATHAN_EXPORT Divide : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Divide(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _DIVIDE_HPP


