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
 * $Id: XQValidate.hpp,v 1.9 2006/11/01 16:37:11 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQValidate.hpp: interface for the XQValidate class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
#define AFXQ_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/schema/DocumentCache.hpp>

/** Validate function. */
class XQILLA_API XQValidate : public ASTNodeImpl
{
public:
  XQValidate(ASTNode* valExpr, DocumentCache::ValidationMode valMode, XPath2MemoryManager* expr);

  /** evaluate the expression, and check if it's valid according to the schemas **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  /**
   *  Called during static analysis to determine if statically correct.  If
   *  not an appropriate error is thrown.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const;
  DocumentCache::ValidationMode getValidationMode() const;

  void setExpression(ASTNode *expr);

protected:
  ASTNode* _expr;
  DocumentCache::ValidationMode _validationMode;
};

#endif // !defined(AFXQ_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
