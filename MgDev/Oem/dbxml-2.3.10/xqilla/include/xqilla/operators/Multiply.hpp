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
 * $Id: Multiply.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   Multiply operator function
*/

#ifndef _MULTIPLY_HPP
#define _MULTIPLY_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/operators/ArithmeticOperator.hpp>

/** OpTimes operator function*/
class XQILLA_API Multiply : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  Multiply(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  void calculateStaticType();
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;
};

#endif // _FUNCTIONOPTIMES_HPP

