/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   OpPlus operator function
*/

#ifndef _PLUS_HPP
#define _PLUS_HPP

#include <pathan/Pathan.hpp>

#include "ArithmeticOperator.hpp"

/** OpPlus operator function*/
class PATHAN_EXPORT Plus : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Plus(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;

  static Item::Ptr plus(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context);
};

#endif // _FUNCTIONOPPLUS_HPP

