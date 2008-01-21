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
 * $Id: OrderComparison.hpp,v 1.11 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ORDERCOMPARISON_HPP
#define _ORDERCOMPARISON_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class XQILLA_API OrderComparison : public XQOperator
{
public:
  static const XMLCh name[];

  ///testBefore should be set to true if you want to test that the first parameter is before the second.
  OrderComparison(const VectorOfASTNodes &args, bool testBefore, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

  bool getTestBefore() const;

private:
  class OrderComparisonResult : public SingleResult
  {
  public:
    OrderComparisonResult(const OrderComparison *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const OrderComparison *_op;
  };

	bool _testBefore;
};

#endif // _FUNCTIONOPPLUS_HPP
