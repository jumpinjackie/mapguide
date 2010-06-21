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
 * $Id: XQDebugCallback.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQDebugCallback.h: interface for debugging
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQDEBUGCALLBACK_H__446AD191_E9D0_4658_BD8C_032D29DA125F__INCLUDED_)
#define AFXQ_XQDEBUGCALLBACK_H__446AD191_E9D0_4658_BD8C_032D29DA125F__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>

class Sequence;
class DynamicContext;

XERCES_CPP_NAMESPACE_BEGIN
    class DOMNode;
XERCES_CPP_NAMESPACE_END

class XQILLA_API XQDebugCallback
{
public:
  virtual ~XQDebugCallback() {}

  virtual void IsBreakPointHit(DynamicContext* context, const XMLCh* file, unsigned int nLine) = 0;
  virtual void EnterFunction(DynamicContext* context, const XMLCh* file, const XMLCh* fnName, unsigned int nLine, unsigned int nColumn) = 0;
  virtual void ExitFunction(DynamicContext* context, const XMLCh* fnName) = 0;
  virtual void ReportClonedNode(DynamicContext* context, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* original, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* clone) = 0;
  virtual void ReportResult(DynamicContext* context, const XMLCh* file, unsigned int nLine, unsigned int nColumn, Sequence& result) = 0;
  virtual void ReportFirstError(DynamicContext* context, const XMLCh* error, const XMLCh* file, unsigned int nLine) = 0;

  virtual void NotifyQueryBegin(DynamicContext* context, const XMLCh* query) = 0;
  virtual void NotifyQueryEnd(DynamicContext* context, Sequence& result) = 0;
};

#endif // !defined(AFXQ_XQDEBUGCALLBACK_H__446AD191_E9D0_4658_BD8C_032D29DA125F__INCLUDED_)
