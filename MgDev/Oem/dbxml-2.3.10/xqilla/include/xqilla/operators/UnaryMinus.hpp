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
 * $Id: UnaryMinus.hpp,v 1.11 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   UnaryMinus operator function
*/

#ifndef _UNARYMINUS_HPP
#define _UNARYMINUS_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/operators/ArithmeticOperator.hpp>

/** Unary Minus operator function*/
class XQILLA_API UnaryMinus : public ArithmeticOperator
{
public:
  static const XMLCh name[];

  UnaryMinus(bool positive, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  void calculateStaticType();
  Item::Ptr execute(const AnyAtomicType::Ptr &atom1, const AnyAtomicType::Ptr &atom2, DynamicContext *context) const;

  bool getIsPositive() const { return positive_; }

private:
  bool positive_;
};

#endif // _UNARYMINUS_HPP

