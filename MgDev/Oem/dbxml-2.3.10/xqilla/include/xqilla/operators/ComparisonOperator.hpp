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
 * $Id: ComparisonOperator.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _COMPARISONOPERATOR_HPP
#define _COMPARISONOPERATOR_HPP

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class AnyAtomicType;

class XQILLA_API ComparisonOperator : public XQOperator
{
public:
  ComparisonOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

  AnyAtomicType::Ptr getArgument(unsigned int index, DynamicContext *context) const;

  virtual bool execute(const AnyAtomicType::Ptr &arg1, const AnyAtomicType::Ptr &arg2, DynamicContext *context) const = 0;

protected:

  class ComparisonResult : public SingleResult
  {
  public:
    ComparisonResult(const ComparisonOperator *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const ComparisonOperator *_op;
  };
};

#endif // _COMPARISONOPERATOR_HPP
