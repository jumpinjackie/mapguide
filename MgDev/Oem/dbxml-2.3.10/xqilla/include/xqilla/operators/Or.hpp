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
 * $Id: Or.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _OR_HPP
#define _OR_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class XQILLA_API Or : public XQOperator
{
public:
  static const XMLCh name[];

  Or(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

private:
  class OrResult : public SingleResult
  {
  public:
    OrResult(const Or *op);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;    
  private:
    const Or *_op;
  };
};

#endif // _OR_HPP
