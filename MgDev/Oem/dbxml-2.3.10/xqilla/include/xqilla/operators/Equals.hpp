/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: Equals.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   Equals operator function
*/

#ifndef _EQUALS_HPP
#define _EQUALS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/operators/ComparisonOperator.hpp>

class Collation;

/** OpEqualsEquals operator function*/
class XQILLA_API Equals : public ComparisonOperator
{
public:
  static const XMLCh name[];

  Equals(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  static bool equals(const Item::Ptr &arg1, const Item::Ptr &arg2, Collation* collation, DynamicContext* context, const LocationInfo *info);

  static bool equals(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, Collation* collation, DynamicContext* context, const LocationInfo *info);

  bool execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _EQUALS_HPP

