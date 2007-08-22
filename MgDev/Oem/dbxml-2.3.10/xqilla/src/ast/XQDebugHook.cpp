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
 * $Id: XQDebugHook.cpp,v 1.15 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQDebugHook.cpp: implementation of the XQDebugHook class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/context/XQDebugCallback.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQDebugHook::XQDebugHook(const XMLCh* szFile, unsigned int nLine, unsigned int nColumn, ASTNode* impl, const XMLCh* functionName, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr)
{
  m_impl=impl;
  m_szFunctionName=functionName;
  setType(ASTNode::DEBUG_HOOK);
  setLocationInfo(szFile, nLine, nColumn);
}

Result XQDebugHook::collapseTree(DynamicContext *context, int flags) const
{
  XQDebugCallback* pDbgCallback=context->getDebugCallback();

  if(pDbgCallback)
    {
      if(context->isDebuggingEnabled()) 
        {
          pDbgCallback->IsBreakPointHit(context,getFile(),getLine());
          flags=0;    // disable optimizations returning data partially or in a different order
        }
      if(m_szFunctionName) pDbgCallback->EnterFunction(context,getFile(),m_szFunctionName,getLine(),getColumn());
    }
  try
    {
      Result result=m_impl->collapseTree(context,flags);
      if(pDbgCallback) {
        Sequence seqRes=result->toSequence(context);
        pDbgCallback->ReportResult(context,getFile(), getLine(), getColumn(), seqRes);
        if(m_szFunctionName) pDbgCallback->ExitFunction(context,m_szFunctionName);
        return seqRes;
      }
      else
        return result;
    }
  catch(XQException& e) {
    if(!e.isErrorReported()) {
      if(pDbgCallback && context->isDebuggingEnabled())
        pDbgCallback->ReportFirstError(context, e.getError(), getFile(), getLine());
      e.setErrorReported();
      if(e.getXQueryLine() == 0)
        e.setXQueryPosition(this);
    }
    throw e;
  }
  // shouldn't get here
  assert(0);
  return Sequence(context->getMemoryManager());
}

ASTNode* XQDebugHook::staticResolution(StaticContext *context) 
{
  try {
    m_impl=m_impl->staticResolution(context);
  }
  catch(XQException& e) {
    if(e.getXQueryLine() == 0) {
      e.setXQueryPosition(this);
    }
    throw e;
  }
  return this;
}

ASTNode* XQDebugHook::staticTyping(StaticContext *context) 
{
  try {
    m_impl=m_impl->staticTyping(context);
  }
  catch(XQException& e) {
    if(e.getXQueryLine() == 0) {
      e.setXQueryPosition(this);
    }
    throw e;
  }
  return this;
}

const StaticResolutionContext &XQDebugHook::getStaticResolutionContext() const
{
  return m_impl->getStaticResolutionContext();
}

const XMLCh *XQDebugHook::getFunctionName() const
{
  return m_szFunctionName;
}
