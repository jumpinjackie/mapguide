//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQDebugHook.hpp,v 1.8 2005/08/04 15:48:05 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQDebugHook.hpp: interface for the XQDebugHook class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_)
#define AFX_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/dataItem/DataItemImpl.hpp>

class XQENGINE_API XQDebugHook : public DataItemImpl
{
public:
	XQDebugHook(const XMLCh* szFile, unsigned int nLine, unsigned int nColumn, DataItem* impl, const XMLCh* functionName, XPath2MemoryManager* memMgr);
	
  virtual Result collapseTree(DynamicContext* context, int flags=0) const;

	virtual void addPredicates(const VectorOfDataItems &steps);
	virtual DataItem* staticResolution(StaticContext *context);

  virtual const StaticResolutionContext &getStaticResolutionContext() const;

  const XMLCh *getFunctionName() const;
  int getLine() const;
  int getColumn() const;
  const XMLCh *getFile() const;

	DataItem* m_impl;
protected:
	const XMLCh* m_szFunctionName;
	unsigned int m_nLine,m_nColumn;
	const XMLCh* m_szFile; 
};

#endif // !defined(AFX_XQDEBUGHOOK_H__446AD191_E9D0_4658_BD8C_032D29DA123C__INCLUDED_)
