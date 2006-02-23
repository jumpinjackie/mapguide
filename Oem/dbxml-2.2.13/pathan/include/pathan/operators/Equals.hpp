/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   Equals operator function
*/

#ifndef _EQUALS_HPP
#define _EQUALS_HPP

#include <pathan/Pathan.hpp>

#include "ComparisonOperator.hpp"

class Collation;

/** OpEqualsEquals operator function*/
class PATHAN_EXPORT Equals : public ComparisonOperator
{
public:
  static const XMLCh name[];

  Equals(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  static bool equals(const Item::Ptr &arg1, const Item::Ptr &arg2, Collation* collation, DynamicContext* context);

  static bool equals(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _EQUALS_HPP

