/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _TOTALORDERCOMPARISON_HPP
#define _TOTALORDERCOMPARISON_HPP

#include <pathan/Pathan.hpp>

#include <pathan/operators/ComparisonOperator.hpp>

/** OpPlus operator function*/
class PATHAN_EXPORT TotalOrderComparison : public ComparisonOperator
{
public:
	//greaterThan should be set to true if you want to param1 > param2 
  TotalOrderComparison(const VectorOfDataItems &args, bool greatherThan, XPath2MemoryManager* memMgr);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;

private:
	bool _greaterThan;
};

#endif // _FUNCTIONTOTALORDER_HPP
