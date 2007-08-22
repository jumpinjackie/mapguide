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
 * $Id: XQTreatAs.hpp,v 1.11 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQTREATAS_HPP
#define _XQTREATAS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/LazySequenceResult.hpp>

class SequenceType;

class XQILLA_API XQTreatAs : public ASTNodeImpl
{
public:
  XQTreatAs(ASTNode* expr, const SequenceType *exprType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  void setIsTreatAsStatement(bool bValue) { _isTreatAs=bValue; }
  const ASTNode *getExpression() const;
  const SequenceType *getSequenceType() const;
  bool getDoTypeCheck() const { return _doTypeCheck; }

  void setExpression(ASTNode *item);

protected:
  ASTNode* _expr;
  const SequenceType* _exprType;
  bool _doTypeCheck, _isTreatAs;
};

#endif
