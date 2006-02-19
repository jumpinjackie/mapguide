/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Mod operator function
*/

#ifndef _MOD_HPP
#define _MOD_HPP

#include <pathan/Pathan.hpp>
#include "ArithmeticOperator.hpp"

/** OpMod operator function*/
class PATHAN_EXPORT Mod : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Mod(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _FUNCTIONOPMOD_HPP

