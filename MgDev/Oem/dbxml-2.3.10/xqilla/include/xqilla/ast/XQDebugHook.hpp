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
 * $Id: XQDebugHook.hpp,v 1.11 2006/11/01 16:37:11 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQDebugHook.hpp: interface for the XQDebugHook class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_)
#define AFXQ_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQDebugHook : public ASTNodeImpl
{
public:
  XQDebugHook(const XMLCh* szFile, unsigned int nLine, unsigned int nColumn, ASTNode* impl, const XMLCh* functionName, XPath2MemoryManager* memMgr);
	
  virtual Result collapseTree(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  virtual const StaticResolutionContext &getStaticResolutionContext() const;

  const XMLCh *getFunctionName() const;

  ASTNode* m_impl;
protected:
  const XMLCh* m_szFunctionName;
};

#endif // !defined(AFXQ_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_)
