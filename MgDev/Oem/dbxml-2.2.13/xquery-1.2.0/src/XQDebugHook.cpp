//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQDebugHook.cpp,v 1.13 2005/08/04 15:48:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQDebugHook.cpp: implementation of the XQDebugHook class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQDebugHook.hpp"
#include "xquery/XQContext.hpp"
#include "xquery/XQException.hpp"
#include "xquery/XQDebugCallback.hpp"
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/DynamicContext.hpp>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQDebugHook::XQDebugHook(const XMLCh* szFile, unsigned int nLine, unsigned int nColumn, DataItem* impl, const XMLCh* functionName, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr)
{
  m_impl=impl;
  m_nLine=nLine;
  m_nColumn=nColumn;
  m_szFile=szFile;
  m_szFunctionName=functionName;
  setType((DataItem::whichType)XQContext::DEBUG_HOOK);
}

Result XQDebugHook::collapseTree(DynamicContext *ctx, int flags) const
{
  XQContext *context = CAST_TO_XQCONTEXT(ctx);

  XQDebugCallback* pDbgCallback=context->getDebugCallback();

  if(pDbgCallback)
    {
      if(context->isDebuggingEnabled()) 
        {
          pDbgCallback->IsBreakPointHit(context,m_szFile,m_nLine);
          flags=0;    // disable optimizations returning data partially or in a different order
        }
      if(m_szFunctionName) pDbgCallback->EnterFunction(context,m_szFile,m_szFunctionName,m_nLine,m_nColumn);
    }
  try
    {
      Result result=m_impl->collapseTree(context,flags);
      if(pDbgCallback) {
        Sequence seqRes=result.toSequence(context);
        pDbgCallback->ReportResult(context,m_szFile, m_nLine, m_nColumn, seqRes);
        if(m_szFunctionName) pDbgCallback->ExitFunction(context,m_szFunctionName);
        return seqRes;
      }
      else
        return result;
    }
  catch(XQException&)
    {
      throw;
    }
  catch(DSLException& e)
    {
      if(pDbgCallback && context->isDebuggingEnabled()) pDbgCallback->ReportFirstError(context,e.getError(), m_szFile, m_nLine);
      throw XQException(e.getError(),m_szFile,m_nLine,m_nColumn);
    }
  // shouldn't get here
  assert(0);
  return Sequence(context->getMemoryManager());
}

void XQDebugHook::addPredicates(const VectorOfDataItems& steps)
{
  m_impl->addPredicates(steps);
}

DataItem* XQDebugHook::staticResolution(StaticContext *context) 
{
  try {
    m_impl=m_impl->staticResolution(context);
  }
  catch(XQException&) {
    throw;
  }
  catch(DSLException& e) {
    throw XQException(e.getError(),m_szFile,m_nLine,m_nColumn);
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

int XQDebugHook::getLine() const
{
  return m_nLine;
}

int XQDebugHook::getColumn() const
{
  return m_nColumn;
}

const XMLCh *XQDebugHook::getFile() const
{
  return m_szFile;
}
