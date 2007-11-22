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
 * $Id: XQRemoteDebugger.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQRemoteDebugger.h: interface for the XQRemoteDebugger class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQREMOTEDEBUGGER_H__972A3595_1779_4CAA_8539_35DD002362DB__INCLUDED_)
#define AFXQ_XQREMOTEDEBUGGER_H__972A3595_1779_4CAA_8539_35DD002362DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xqilla/context/XQDebugCallback.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/ValueHashTableOf.hpp>
#include <xqilla/framework/StringPool.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/Node.hpp>

class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API XQRemoteDebugger : public XQDebugCallback
{
public:
	XQRemoteDebugger(const XMLCh* lpchHostName, XPath2MemoryManager* memMgr);
	virtual ~XQRemoteDebugger();

    virtual void IsBreakPointHit(DynamicContext* context, const XMLCh* file, unsigned int nLine);
    virtual void EnterFunction(DynamicContext* context, const XMLCh* file, const XMLCh* fnName, unsigned int nLine, unsigned int nColumn);
    virtual void ExitFunction(DynamicContext* context, const XMLCh* fnName);
    virtual void ReportClonedNode(DynamicContext* context, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* original, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* clone);
    virtual void ReportResult(DynamicContext* context, const XMLCh* file, unsigned int nLine, unsigned int nColumn, Sequence& result);
    virtual void ReportFirstError(DynamicContext* context, const XMLCh* error, const XMLCh* file, unsigned int nLine);
    virtual void NotifyQueryBegin(DynamicContext* context, const XMLCh* query);
    virtual void NotifyQueryEnd(DynamicContext* context, Sequence& result);

    static const XMLCh g_szHandshake[];
    static const XMLCh g_szBreakpoint[];
    static const XMLCh g_szEnterFunction[];
    static const XMLCh g_szExitFunction[];
    static const XMLCh g_szError[];
    static const XMLCh g_szQueryText[];
    static const XMLCh g_szResult[];
    static const XMLCh g_szVariables[];
    static const XMLCh g_szExpression[];

    static const XMLCh g_szFile[];
    static const XMLCh g_szLine[];
    static const XMLCh g_szColumn[];
    static const XMLCh g_szFunction[];
    static const XMLCh g_szSessionId[];
    static const XMLCh g_szQuit[];
    static const XMLCh g_szMessage[];
    static const XMLCh g_szText[];

protected:
    void SendNotification(DynamicContext* context, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& command, const char* postData=NULL);
    void Encode(const XMLCh* data, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buffer);
    void SerializeItem(DynamicContext* context, const Item::Ptr item, bool bAddBackMapInfo);
    void SerializeSequence(DynamicContext* context, Sequence& seq, bool bAddBackMapInfo);
    void SerializeNode(DynamicContext *context, const Node::Ptr &node, bool bAddBackMapInfo);

    XERCES_CPP_NAMESPACE_QUALIFIER XMLStringPool m_filePool;
    XERCES_CPP_NAMESPACE_QUALIFIER ValueHashTableOf<unsigned int>* m_backMappingInfo;

    const XMLCh* m_szHostName, *m_szSessionId;
    bool m_bEnableComm;
    const XMLCh* m_szLastFileSeen;
    unsigned int m_nLastLineSeen;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer m_urlBuffer;
    XERCES_CPP_NAMESPACE_QUALIFIER MemBufFormatTarget m_messageBuffer;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter m_messageFormatter;

    XPath2MemoryManager* m_pMemMgr;
};

#endif // !defined(AFXQ_XQREMOTEDEBUGGER_H__972A3595_1779_4CAA_8539_35DD002362DB__INCLUDED_)
