/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   GreaterThanEqual operator function
*/

#ifndef _GREATERTHANEQUAL_HPP
#define _GREATERTHANEQUAL_HPP

#include <pathan/Pathan.hpp>

#include "ComparisonOperator.hpp"

class Collation;

/** OpGreaterThanEqual operator function*/
class PATHAN_EXPORT GreaterThanEqual : public ComparisonOperator
{
public:
  static const XMLCh name[];

  GreaterThanEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  static bool greater_than_equal(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _GREATERTHANEQUAL_HPP

