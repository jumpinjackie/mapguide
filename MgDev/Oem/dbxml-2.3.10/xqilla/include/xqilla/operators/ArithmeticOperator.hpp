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
 * $Id: ArithmeticOperator.hpp,v 1.13 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ARITHMETICOPERATOR_HPP
#define _ARITHMETICOPERATOR_HPP

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class AnyAtomicType;

class XQILLA_API ArithmeticOperator : public XQOperator
{
public:
  ArithmeticOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

  AnyAtomicType::Ptr getArgument(unsigned int index, DynamicContext *context) const;

  virtual void calculateStaticType() = 0;
  virtual Item::Ptr execute(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, DynamicContext *context) const = 0;

protected:
  void calculateStaticTypeForNumerics(const StaticType &arg0, const StaticType &arg1);

  class ArithmeticResult : public SingleResult
  {
  public:
    ArithmeticResult(const ArithmeticOperator *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const ArithmeticOperator *_op;
  };
};

#endif // _ARITHMETICOPERATOR_HPP
