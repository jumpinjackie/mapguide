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
 * $Id: XQOrderingChange.hpp,v 1.11 2006/11/01 16:37:11 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQOrderingChange.hpp: interface for the XQOrderingChange class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
#define AFXQ_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/context/StaticContext.hpp>

class XQILLA_API XQOrderingChange : public ASTNodeImpl
{
public:
  XQOrderingChange(StaticContext::NodeSetOrdering ordering, ASTNode* astNode, XPath2MemoryManager* expr);

  virtual Result createResult(DynamicContext *context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  StaticContext::NodeSetOrdering getOrderingValue() const;
  ASTNode* getExpr() const;
  void setExpr(ASTNode *expr);

protected:
  StaticContext::NodeSetOrdering m_nOrdering;
  ASTNode* m_pExpr;
};

#endif // !defined(AFXQ_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
