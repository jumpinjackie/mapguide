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
 * $Id: NodeComparison.hpp,v 1.11 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _NODECOMPARISON_HPP
#define _NODECOMPARISON_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

/** NodeComparison operator function*/
class XQILLA_API NodeComparison : public XQOperator
{
public:
  static const XMLCh name[];

  NodeComparison(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class NodeComparisonResult : public SingleResult
  {
  public:
    NodeComparisonResult(const NodeComparison *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const NodeComparison *_op;
  };
};

#endif // _FUNCTIONOPPLUS_HPP
