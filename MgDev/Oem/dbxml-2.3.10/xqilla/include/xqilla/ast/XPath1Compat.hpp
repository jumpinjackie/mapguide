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
 * $Id: XPath1Compat.hpp,v 1.4 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XPATH1COMPAT_HPP
#define _XPATH1COMPAT_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>

class SequenceType;

class XQILLA_API XPath1CompatConvertFunctionArg : public ASTNodeImpl
{
public:
  XPath1CompatConvertFunctionArg(ASTNode* expr, const SequenceType *seqType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }
  const SequenceType *getSequenceType() const { return seqType_; }

protected:
  class XPath1CompatConvertFunctionArgResult : public ResultImpl
  {
  public:
    XPath1CompatConvertFunctionArgResult(const XPath1CompatConvertFunctionArg *di, const Result &parent, const SequenceType *seqType);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const SequenceType *seqType_;
    Result parent_;
    bool oneDone_;
  };

  ASTNode* expr_;
  const SequenceType *seqType_;
};

#endif
