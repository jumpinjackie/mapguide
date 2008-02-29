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
 * $Id: XQCastableAs.hpp,v 1.8 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQCASTABLEAS_HPP
#define _XQCASTABLEAS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class SequenceType;

class XQILLA_API XQCastableAs : public ASTNodeImpl
{
public:
	XQCastableAs(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const;
  const SequenceType *getSequenceType() const;

  void setExpression(ASTNode *item);

protected:
  class CastableAsResult : public SingleResult
  {
  public:
    CastableAsResult(const XQCastableAs *di);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQCastableAs *_di;
  };

	ASTNode* _expr;
	SequenceType* _exprType;
};

#endif // _XQCASTABLEAS_HPP
