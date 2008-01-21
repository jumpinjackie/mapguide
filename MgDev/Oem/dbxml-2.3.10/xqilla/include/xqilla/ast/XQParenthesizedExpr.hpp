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
 * $Id: XQParenthesizedExpr.hpp,v 1.9 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQPARENTHESIZEDEXPR_HPP
#define _XQPARENTHESIZEDEXPR_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQParenthesizedExpr : public ASTNodeImpl
{

public:
  XQParenthesizedExpr(XPath2MemoryManager* memMgr);	

  ///adds a ASTNode to this XQParenthesizedExpr
  void addItem(ASTNode* di);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const VectorOfASTNodes &getChildren() const;

private:
  class ParenthesizedResult : public ResultImpl
  {
  public:
    ParenthesizedResult(const XQParenthesizedExpr *di, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const XQParenthesizedExpr *_di;
    VectorOfASTNodes::const_iterator _i;
    Result _result;
  };

	VectorOfASTNodes _astNodes;
};

#endif
