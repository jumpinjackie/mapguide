/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   LessThanEqual operator function
*/

#ifndef _LESSTHANEQUAL_HPP
#define _LESSTHANEQUAL_HPP

#include <pathan/Pathan.hpp>

#include "ComparisonOperator.hpp"

class Collation;

/** OpLessThanEqual operator function*/
class PATHAN_EXPORT LessThanEqual : public ComparisonOperator
{
public:
  static const XMLCh name[];

  LessThanEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  static bool less_than_equal(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _LESSTHANEQUAL_HPP

