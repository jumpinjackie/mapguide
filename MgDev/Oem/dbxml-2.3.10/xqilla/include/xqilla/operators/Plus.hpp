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
 * $Id: Plus.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   OpPlus operator function
*/

#ifndef _PLUS_HPP
#define _PLUS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/operators/ArithmeticOperator.hpp>

/** OpPlus operator function*/
class XQILLA_API Plus : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Plus(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  void calculateStaticType();
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;

  static Item::Ptr plus(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context, const LocationInfo *info);
};

#endif // _FUNCTIONOPPLUS_HPP

