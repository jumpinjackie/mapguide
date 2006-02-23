/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   NotEquals operator function
*/

#ifndef _NOTEQUALS_HPP
#define _NOTEQUALS_HPP

#include <pathan/Pathan.hpp>

#include "ComparisonOperator.hpp"

class Collation;

/** OpNotEquals operator function*/
class PATHAN_EXPORT NotEquals : public ComparisonOperator
{
public:
  static const XMLCh name[];

  NotEquals(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  static bool not_equals(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _NOTEQUALS_HPP
