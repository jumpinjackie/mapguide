/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   GreaterThan operator function
*/

#ifndef _GREATERTHAN_HPP
#define _GREATERTHAN_HPP

#include <pathan/Pathan.hpp>

#include "ComparisonOperator.hpp"

class Collation;

/** OpGreaterThan operator function*/
class PATHAN_EXPORT GreaterThan : public ComparisonOperator
{
public:
  static const XMLCh name[];

  GreaterThan(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  static bool greater_than(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};
#endif
