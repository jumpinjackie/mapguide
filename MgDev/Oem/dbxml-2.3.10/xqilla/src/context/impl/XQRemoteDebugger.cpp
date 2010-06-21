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
 * $Id: XQRemoteDebugger.cpp,v 1.15.2.1 2006/12/08 15:19:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQRemoteDebugger.cpp: implementation of the XQRemoteDebugger class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/context/impl/XQRemoteDebugger.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/items/impl/NodeImpl.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/HashPtr.hpp>
#include <xercesc/dom/DOM.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const XMLCh gHTTPString[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

static const XMLCh gSlash[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szHandshake[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};

const XMLCh XQRemoteDebugger::g_szBreakpoint[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_B, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_k, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szEnterFunction[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szExitFunction[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szError[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szQueryText[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szResult[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szVariables[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szExpression[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szFile[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szLine[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szColumn[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szFunction[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szSessionId[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_I, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szQuit[] = 
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szMessage[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
    XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

const XMLCh XQRemoteDebugger::g_szText[] =
{
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, 
    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

XQRemoteDebugger::XQRemoteDebugger(const XMLCh* lpchHostName, XPath2MemoryManager* memMgr) :
    m_filePool(7, memMgr),
    m_szSessionId(NULL),
    m_bEnableComm(true),
    m_szLastFileSeen(NULL),
    m_nLastLineSeen(0),
    m_urlBuffer(1023,memMgr),
    m_messageBuffer(24*1024, memMgr),
    m_messageFormatter("utf-8", &m_messageBuffer, XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes, XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRep_CharRef, memMgr),
    m_pMemMgr(memMgr)
{
    m_szHostName=XPath2Utils::concatStrings(gHTTPString,lpchHostName,gSlash,m_pMemMgr);
    // force an exception if the host name is not a valid URL
    XERCES_CPP_NAMESPACE_QUALIFIER XMLURL query(m_szHostName);

    // throw an exception also if there is no support for HTTP client
    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgNetAccessor==NULL)
        XQSimpleThrow(X("Xerces-C++ has not been compiled with HTTP support"),NULL,0,0);

    m_backMappingInfo=new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER ValueHashTableOf<unsigned int>(197, new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER HashPtr(), memMgr);
}

XQRemoteDebugger::~XQRemoteDebugger()
{
}

void XQRemoteDebugger::SendNotification(DynamicContext* context, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& command, const char* postData/*=NULL*/)
{
    if(m_szSessionId==NULL)
    {
        try
        {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLURL query(m_szHostName, g_szHandshake);
            XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream* answer=query.makeNewStream();
            XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream> janStream(answer);
            XMLByte sessId[20];
            unsigned int recvd=answer->readBytes(sessId,19);
            sessId[recvd]=0;
            // check if the debugger refuses us
            if(recvd==0 || strcmp((const char*)sessId,"\r\n")==0)
            {
                m_bEnableComm=false;
                return;
            }
            m_szSessionId=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode((const char*)sessId,m_pMemMgr);
        }
        catch(XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException&)
        {
        }
        catch(XERCES_CPP_NAMESPACE_QUALIFIER NetAccessorException&)
        {
            // if we cannot establish a connection now, just give up (maybe the server has not been started)
            m_bEnableComm=false;
            return;
        }
    }
    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(command.getRawBuffer(),XERCES_CPP_NAMESPACE_QUALIFIER chQuestion,0,m_pMemMgr)==-1)
        command.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
    else
        command.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    command.append(g_szSessionId);
    command.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    command.append(m_szSessionId);
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLURL query(m_szHostName, command.getRawBuffer());
        XERCES_CPP_NAMESPACE_QUALIFIER XMLNetHTTPInfo httpInfo;
        char extraHeaders[40]="Content-Length: ";
        if(postData!=NULL)
        {
            httpInfo.fHTTPMethod=XERCES_CPP_NAMESPACE_QUALIFIER XMLNetHTTPInfo::POST;
            int postDataLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(postData);
            char number[10];
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(postDataLen,number,9,10,m_pMemMgr);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::catString(extraHeaders,number);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::catString(extraHeaders,"\r\n");
            httpInfo.fHeaders=extraHeaders;
            httpInfo.fHeadersLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(extraHeaders);
            httpInfo.fPayload=postData;
            httpInfo.fPayloadLen=postDataLen;
        }
        XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream* answer=XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgNetAccessor->makeNew(query,&httpInfo);
        XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream> janStream(answer);
        // Possible answers
        //  a) GO
        //  b) CONT
        //  b) STOP
        //  c) GET_VARIABLES_IN_SCOPE
        //  d) EVALUATE_EXPRESSION
        XMLByte data[2048];
        unsigned int recvd=answer->readBytes(data,2047);
        data[recvd]=0;
        char* firstNL=strstr((char*)data,"\r\n");
        char* arguments=NULL;
        if(firstNL)
        {
            *firstNL=0;
            arguments=firstNL+2;
        }

        if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals((const char*)data,"GO"))
    		m_bEnableComm=false;
        else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals((const char*)data,"STOP"))
    		XQSimpleThrow(X("Debugging aborted"),m_szLastFileSeen,m_nLastLineSeen,0);
        else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals((const char*)data,"GET_VARIABLES_IN_SCOPE"))
        {
	        VariableStore* varStore=context->getVariableStore();
            m_messageBuffer.reset();
            static const char* header="<xqe:variables xmlns:xqe=\"http://www.stylusstudio.com/XQEngine\">";
            static const unsigned int headerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(header);
            static const char* c1="<xqe:variable uri=\"";
            static const unsigned int c1Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c1);
            static const char* c2="\" name=\"";
            static const unsigned int c2Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c2);
            static const char* c3="\">";
            static const unsigned int c3Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c3);
            static const char* c4="</xqe:variable>";
            static const unsigned int c4Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c4);
            m_messageBuffer.writeChars((const XMLByte*)header,headerLen,&m_messageFormatter);
            // always add the context item
            m_messageBuffer.writeChars((const XMLByte*)c1,c1Len,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c2,c2Len,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)".",1,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c3,c3Len,&m_messageFormatter);
            const Item::Ptr ctx=context->getContextItem();
            if(ctx!=NULLRCP)
                SerializeItem(context, ctx, false);
            m_messageBuffer.writeChars((const XMLByte*)c4,c4Len,&m_messageFormatter);
            // and now the local variables
            std::vector< std::pair<const XMLCh*,const XMLCh*> > vec=varStore->getVars();
	        for(std::vector< std::pair<const XMLCh*,const XMLCh*> >::iterator it=vec.begin();it!=vec.end();it++)
	        {
                m_messageBuffer.writeChars((const XMLByte*)c1,c1Len,&m_messageFormatter);
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                                   << it->first
                                   << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
                m_messageBuffer.writeChars((const XMLByte*)c2,c2Len,&m_messageFormatter);
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                                   << it->second
                                   << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
                m_messageBuffer.writeChars((const XMLByte*)c3,c3Len,&m_messageFormatter);
                std::pair<bool, Sequence> varValue=context->getVariableStore()->getVar(it->first,it->second,context);
                if(varValue.first)
                    SerializeSequence(context, varValue.second, false);
                m_messageBuffer.writeChars((const XMLByte*)c4,c4Len,&m_messageFormatter);
	        }
            static const char* footer="</xqe:variables>";
            static const unsigned int footerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(footer);
            m_messageBuffer.writeChars((const XMLByte*)footer,footerLen,&m_messageFormatter);

            XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer cmd(50,m_pMemMgr);
            cmd.set(g_szVariables);
            SendNotification(context, cmd, (const char*)m_messageBuffer.getRawBuffer());
        }
        else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals((const char*)data,"EVALUATE_EXPRESSION"))
        {
            unsigned int argLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(arguments);
            unsigned int unicodeQueryLen=argLen;
            XMLCh* unicodeQuery=(XMLCh*)m_pMemMgr->allocate((argLen+1)*sizeof(XMLCh));
            unsigned int bytesEaten=0;
            unsigned char* charSizes=(unsigned char*)m_pMemMgr->allocate(argLen);
            const_cast<XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder*>(m_messageFormatter.getTranscoder())->transcodeFrom((const XMLByte*)arguments,argLen,unicodeQuery,unicodeQueryLen,bytesEaten,charSizes);
            m_pMemMgr->deallocate(charSizes);

            m_messageBuffer.reset();
            bool oldDebuggingVal=context->isDebuggingEnabled();
            context->enableDebugging(false);
            XQDebugCallback* oldDbgCallback=context->getDebugCallback();
            context->setDebugCallback(NULL);

            // the new query will try to access the VariableTypeStore to find the types of the variables; so we need to 
            // populate it with dummy values
            context->getVariableTypeStore()->addLocalScope();

	    StaticResolutionContext dummy_src(context->getMemoryManager());
            std::vector< std::pair<const XMLCh*,const XMLCh*> > vec=context->getVariableStore()->getVars();
	        for(std::vector< std::pair<const XMLCh*,const XMLCh*> >::iterator it=vec.begin();it!=vec.end();it++)
                context->getVariableTypeStore()->declareVar(it->first, it->second, dummy_src);

            try 
            {
                AutoDelete<XQQuery> pQuery(XQilla::parse(unicodeQuery, XQilla::XQUERY, context));
                Sequence result=pQuery->execute(context)->toSequence(context);

                static const char* header="<xqe:items xmlns:xqe=\"http://www.stylusstudio.com/XQEngine\">";
                static const unsigned int headerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(header);
                m_messageBuffer.writeChars((const XMLByte*)header,headerLen,&m_messageFormatter);
                SerializeSequence(context, result, false);
                static const char* footer="</xqe:items>";
                static const unsigned int footerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(footer);
                m_messageBuffer.writeChars((const XMLByte*)footer,footerLen,&m_messageFormatter);
            }
            catch(XQException& e)
            {
                static const char* header="<xqe:error xmlns:xqe=\"http://www.stylusstudio.com/XQEngine\">";
                static const unsigned int headerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(header);
                m_messageBuffer.writeChars((const XMLByte*)header,headerLen,&m_messageFormatter);
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                                   << e.getError();
                static const char* footer="</xqe:error>";
                static const unsigned int footerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(footer);
                m_messageBuffer.writeChars((const XMLByte*)footer,footerLen,&m_messageFormatter);
            }
            context->getVariableTypeStore()->removeScope();
            context->enableDebugging(oldDebuggingVal);
            context->setDebugCallback(oldDbgCallback);

            XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer cmd(500,m_pMemMgr);
            cmd.set(g_szExpression);
            cmd.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
            cmd.append(g_szText);
            cmd.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
            Encode(unicodeQuery,cmd);

            m_pMemMgr->deallocate(unicodeQuery);

            SendNotification(context, cmd, (const char*)m_messageBuffer.getRawBuffer());
        }
    }
    catch(XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException&)
    {
    }
    catch(XERCES_CPP_NAMESPACE_QUALIFIER NetAccessorException&)
    {
        // we lost the connection, but let's keep trying...
    }
}

void XQRemoteDebugger::Encode(const XMLCh* data, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buffer)
{
    unsigned int len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(data);
    for(unsigned int index=0;index<len;index++)
    {
        if(data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chSpace)
            buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chPlus);
        else if(data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chPercent)
        {
            buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
            buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
        }
        else if(data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chPlus || 
                data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash || 
                data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chBackSlash || 
                data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chColon ||
                data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chEqual ||
                data[index]==XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand)
        {
            XMLCh encoded[3];
            buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chPercent);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(data[index],encoded,2,16,m_pMemMgr);
            buffer.append(encoded);
        }
        else
            buffer.append(data[index]);
    }
}

void XQRemoteDebugger::IsBreakPointHit(DynamicContext* context, const XMLCh* file, unsigned int nLine)
{
    if(!m_bEnableComm)
        return;
    if(nLine==m_nLastLineSeen && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(m_szLastFileSeen,file))
        return;
    m_nLastLineSeen=nLine;
    m_szLastFileSeen=file;
    m_urlBuffer.reset();
    m_urlBuffer.set(g_szBreakpoint);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
    m_urlBuffer.append(g_szFile);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(file,m_urlBuffer);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szLine);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    XMLCh tmpBuff[21];
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nLine,tmpBuff,20,10,m_pMemMgr);
    m_urlBuffer.append(tmpBuff);
    SendNotification(context,m_urlBuffer);
}

void XQRemoteDebugger::EnterFunction(DynamicContext* context, const XMLCh* file, const XMLCh* fnName, unsigned int nLine, unsigned int nColumn)
{
    if(!m_bEnableComm)
        return;
    m_urlBuffer.reset();
    m_urlBuffer.set(g_szEnterFunction);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
    m_urlBuffer.append(g_szFile);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(file,m_urlBuffer);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szFunction);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(fnName,m_urlBuffer);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szLine);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    XMLCh tmpBuff[21];
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nLine,tmpBuff,20,10,m_pMemMgr);
    m_urlBuffer.append(tmpBuff);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szColumn);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nColumn,tmpBuff,20,10,m_pMemMgr);
    m_urlBuffer.append(tmpBuff);
    SendNotification(context,m_urlBuffer);
}

void XQRemoteDebugger::ExitFunction(DynamicContext* context, const XMLCh* fnName)
{
    if(!m_bEnableComm)
        return;
    m_urlBuffer.reset();
    m_urlBuffer.set(g_szExitFunction);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
    m_urlBuffer.append(g_szFunction);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(fnName,m_urlBuffer);
    SendNotification(context,m_urlBuffer);
}

void XQRemoteDebugger::ReportClonedNode(DynamicContext* context, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* original, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* clone)
{
    if(!m_bEnableComm)
        return;
    if(m_backMappingInfo->containsKey(original))
        m_backMappingInfo->put((void*)clone, m_backMappingInfo->get(original,m_pMemMgr));

    if(original->getNodeType()==XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
    {
        const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* cloneElem=(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)clone;
        const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* origElem=(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)original;
        const XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* attrs=origElem->getAttributes();
        for(XMLSize_t i=0;i<attrs->getLength();i++)
        {
            const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* attr=attrs->item(i);
            if(m_backMappingInfo->containsKey(attr))
            {
                const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* cloneAttr=cloneElem->getAttributeNode(attr->getNodeName());
                if(cloneAttr) m_backMappingInfo->put((void*)cloneAttr, m_backMappingInfo->get(attr,m_pMemMgr));
            }
        }
        const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* cloneChild=cloneElem->getFirstChild();
        const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* origChild=origElem->getFirstChild();
        while(cloneChild!= NULL && origChild!=NULL)
        {
            ReportClonedNode(context,origChild,cloneChild);
            cloneChild=cloneChild->getNextSibling();
            origChild=origChild->getNextSibling();
        }
    }
}

void XQRemoteDebugger::ReportResult(DynamicContext* context, const XMLCh* file, unsigned int nLine, unsigned int nColumn, Sequence& result)
{
    if(!m_bEnableComm)
        return;
    unsigned int fileID=m_filePool.addOrFind(file);
    unsigned int location= (nLine & 0x00FFFFFF) | (fileID << 24);
	for(Sequence::iterator it=result.begin();it!=result.end();it++)
	{
		Item::Ptr data=(*it);
		if(data==NULLRCP)
			continue;
		// special case for nodes: associate the info to the underlying DOMNode object
		if(data->isNode())
		{
      const NodeImpl *nodeImpl = (const NodeImpl*)data->getInterface(Item::gXQilla);
      if(nodeImpl == 0) {
        m_backMappingInfo->put((void*)(const Item*)data, location);
      }
      else {
        const XERCES_CPP_NAMESPACE::DOMNode* node = nodeImpl->getDOMNode();
        if(node!=NULL)
          m_backMappingInfo->put((void*)node, location);
      }
		}
		else 
            m_backMappingInfo->put((void*)(const Item*)data, location);
	}
}

void XQRemoteDebugger::ReportFirstError(DynamicContext* context, const XMLCh* error, const XMLCh* file, unsigned int nLine)
{
    if(!m_bEnableComm)
        return;
    XMLCh tmpBuff[21];
    m_urlBuffer.reset();
    m_urlBuffer.set(g_szError);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chQuestion);
    m_urlBuffer.append(g_szFile);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(file,m_urlBuffer);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szLine);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nLine,tmpBuff,20,10,m_pMemMgr);
    m_urlBuffer.append(tmpBuff);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand);
    m_urlBuffer.append(g_szMessage);
    m_urlBuffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chEqual);
    Encode(error,m_urlBuffer);
    SendNotification(context,m_urlBuffer);
}

void XQRemoteDebugger::NotifyQueryBegin(DynamicContext* context, const XMLCh* query)
{
    if(!m_bEnableComm)
        return;
    m_urlBuffer.reset();
    m_urlBuffer.set(g_szQueryText);

    unsigned int queryLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(query);
    // let's start with an optimistic buffer, and grow it if necessary
    unsigned int bufferLen=queryLen+1;
    XMLByte* buffer=NULL;
    while(true)
    {
        buffer=(XMLByte*)m_pMemMgr->allocate(bufferLen);
        unsigned int charsEaten=0;
        const_cast<XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder*>(m_messageFormatter.getTranscoder())->transcodeTo(query,queryLen,buffer,bufferLen,charsEaten,XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder::UnRep_RepChar);
        if(charsEaten==queryLen)
            break;
        m_pMemMgr->deallocate(buffer);
        bufferLen*=2;
    }
    SendNotification(context,m_urlBuffer, (const char*)buffer);
    m_pMemMgr->deallocate(buffer);
}

void XQRemoteDebugger::NotifyQueryEnd(DynamicContext* context, Sequence& result)
{
    if(!m_bEnableComm)
        return;

    m_urlBuffer.reset();
    m_urlBuffer.set(g_szResult);

    m_messageBuffer.reset();
    static const char* header="<xqe:queryResult xmlns:xqe=\"http://www.stylusstudio.com/XQEngine\">";
    static const unsigned int headerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(header);
    m_messageBuffer.writeChars((const XMLByte*)header,headerLen,&m_messageFormatter);
    SerializeSequence(context, result, true);
    static const char* footer="</xqe:queryResult>";
    static const unsigned int footerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(footer);
    m_messageBuffer.writeChars((const XMLByte*)footer,footerLen,&m_messageFormatter);

    SendNotification(context,m_urlBuffer, (const char*)m_messageBuffer.getRawBuffer());
}

void XQRemoteDebugger::SerializeItem(DynamicContext* context, const Item::Ptr item, bool bAddBackMapInfo)
{
    static const char* header="<xqe:item";
    static const unsigned int headerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(header);
    m_messageBuffer.writeChars((const XMLByte*)header,headerLen,&m_messageFormatter);
    if(item->isNode())
    {
        static const char* c4=" type=\"node\" subtype=\"";
        static const unsigned int c4Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c4);
        static const char* c5="\">";
        static const unsigned int c5Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c5);
        m_messageBuffer.writeChars((const XMLByte*)c4,c4Len,&m_messageFormatter);

        Node::Ptr node=(Node::Ptr)item;
        if(node->dmNodeKind() == Node::text_string)
        {
            static const char* c="text";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                               << node->dmStringValue(context);
        }
        else if(node->dmNodeKind() == Node::processing_instruction_string)
        {
            static const char* c="pi\" name=\"";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                      << node->dmNodeName(context)->asString(context)
                      << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                               << node->dmStringValue(context);
        }
        else if(node->dmNodeKind() == Node::document_string)
        {
            static const char* c="document";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            SerializeNode(context, node, bAddBackMapInfo);
        }
//         case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::DOCUMENT_FRAGMENT_NODE:
//             {
//                 static const char* c="fragment";
//                 static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
//                 m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
//                 m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
//                 SerializeNode(node, bAddBackMapInfo);
//             }
//             break;
        else if(node->dmNodeKind() == Node::element_string)
        {
            static const char* c="element";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            SerializeNode(context, node, bAddBackMapInfo);
        }
        else if(node->dmNodeKind() == Node::attribute_string)
        {
            static const char* c="attribute\" name=\"";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
			   << node->dmNodeName(context)->asString(context)
			   << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                               << node->dmStringValue(context);
        }
//         case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::CDATA_SECTION_NODE:
//             {
//                 static const char* c="cdata";
//                 static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
//                 m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
//                 m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
//                 m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
//                                    << node->getNodeValue();
//             }
//             break;
        else if(node->dmNodeKind() == Node::comment_string)
        {
            static const char* c="comment";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                               << node->dmStringValue(context);
        }
        else
        {
            static const char* c="unknown";
            static const unsigned int cLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c);
            m_messageBuffer.writeChars((const XMLByte*)c,cLen,&m_messageFormatter);
            m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
        }
    }
    else
    {
        if(bAddBackMapInfo && m_backMappingInfo->containsKey((void*)(const Item*)item))
        {
            static const char* c1=" xqe:backMapFile=\"";
            static const unsigned int c1Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c1);
            static const char* c2="\" xqe:backMapLine=\"";
            static const unsigned int c2Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c2);
            static const char* c3="\"";
            static const unsigned int c3Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c3);
            unsigned int location=m_backMappingInfo->get((void*)(const Item*)item,m_pMemMgr);
            unsigned int nLine = (location & 0x00FFFFFF);
            unsigned int fileID= location >> 24;
            const XMLCh* fileName = m_filePool.getValueForId(fileID);
            m_messageBuffer.writeChars((const XMLByte*)c1,c1Len,&m_messageFormatter);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                               << fileName
                               << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
            m_messageBuffer.writeChars((const XMLByte*)c2,c2Len,&m_messageFormatter);
            XMLCh tmpBuff[21];
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nLine,tmpBuff,20,10,m_pMemMgr);
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                               << tmpBuff
                               << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
            m_messageBuffer.writeChars((const XMLByte*)c3,c3Len,&m_messageFormatter);
        }
        static const char* c4=" type=\"atomic\"";
        static const unsigned int c4Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c4);
        m_messageBuffer.writeChars((const XMLByte*)c4,c4Len,&m_messageFormatter);
        const AnyAtomicType::Ptr atom=(const AnyAtomicType::Ptr)item;
        static const char* c5=" namespaceURI=\"";
        static const unsigned int c5Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c5);
        m_messageBuffer.writeChars((const XMLByte*)c5,c5Len,&m_messageFormatter);
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                           << atom->getTypeURI()
                           << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
        static const char* c6="\" typeName=\"";
        static const unsigned int c6Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c6);
        m_messageBuffer.writeChars((const XMLByte*)c6,c6Len,&m_messageFormatter);
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                           << atom->getTypeName()
                           << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes;
        static const char* c7="\">";
        static const unsigned int c7Len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(c7);
        m_messageBuffer.writeChars((const XMLByte*)c7,c7Len,&m_messageFormatter);

        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                           << item->asString(context);
    }
    static const char* footer="</xqe:item>";
    static const unsigned int footerLen=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(footer);
    m_messageBuffer.writeChars((const XMLByte*)footer,footerLen,&m_messageFormatter);
}

void XQRemoteDebugger::SerializeSequence(DynamicContext* context, Sequence& seq, bool bAddBackMapInfo)
{
    for(Sequence::iterator it=seq.begin(); it!=seq.end(); it++)
        SerializeItem(context, *it, bAddBackMapInfo);
}

void XQRemoteDebugger::SerializeNode(DynamicContext *context, const Node::Ptr &node, bool bAddBackMapInfo)
{
    const XMLCh*    nodeName = node->dmNodeName(context)->asString(context);
    const XMLCh*    nodeValue = node->dmStringValue(context);

    if(node->dmNodeKind() == Node::text_string) {
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::CharEscapes
                           << nodeValue;
    }
    else if(node->dmNodeKind() == Node::processing_instruction_string) {
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
                    << XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle
                    << XERCES_CPP_NAMESPACE_QUALIFIER chQuestion
                    << nodeName;
        if (nodeValue && *nodeValue)
            m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER chSpace 
                        << nodeValue;
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER chQuestion
                    << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;
    }
    else if(node->dmNodeKind() == Node::document_string) {
      // don't write the BOM, or we will produce invalid XML
      Result children = node->dmChildren(context, 0);
      Node::Ptr child;
      while((child = children->next(context)).notNull())
      {
          SerializeNode(context, child, bAddBackMapInfo);
      }
    }
    else if(node->dmNodeKind() == Node::element_string) {
            m_messageFormatter  << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                         << XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle 
                         << nodeName;
            if(bAddBackMapInfo && m_backMappingInfo->containsKey(node))
            {
                static const XMLCh backMapFile[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chColon,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_k,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
                static const XMLCh backMapLine[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chColon,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_k,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_M, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
                                                   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

                unsigned int location=m_backMappingInfo->get(node,m_pMemMgr);
                unsigned int nLine = (location & 0x00FFFFFF);
                unsigned int fileID= location >> 24;
                const XMLCh* fileName = m_filePool.getValueForId(fileID);
                XMLCh tmpBuff[21];
                XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(nLine,tmpBuff,20,10,m_pMemMgr);

                m_messageFormatter  << XERCES_CPP_NAMESPACE_QUALIFIER chSpace
                             << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                             << backMapFile
                             << XERCES_CPP_NAMESPACE_QUALIFIER chEqual 
                             << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote
                             << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                             << fileName
                             << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                             << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote
                             << XERCES_CPP_NAMESPACE_QUALIFIER chSpace
                             << backMapLine
                             << XERCES_CPP_NAMESPACE_QUALIFIER chEqual 
                             << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote
                             << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                             << tmpBuff
                             << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                             << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote;
            }
            Result attributes = node->dmAttributes(context, 0);
            Node::Ptr attr;
            while((attr = attributes->next(context)).notNull()) {
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER chSpace;
                SerializeNode(context, attr, bAddBackMapInfo);
            }
            Result children = node->dmChildren(context, 0);
            Node::Ptr child = children->next(context);
            if (child.notNull())
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
                            << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;

                while (child.notNull())
                {
                    SerializeNode(context, child, bAddBackMapInfo);
                    child = children->next(context);
                }
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
                            << XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle
                            << XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash
                            << nodeName 
                            << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;
            }
            else
            {
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
                            << XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash 
                            << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;
            }
    }
    else if(node->dmNodeKind() == Node::attribute_string) {
        m_messageFormatter  << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                     << nodeName
                     << XERCES_CPP_NAMESPACE_QUALIFIER chEqual 
                     << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote
                     << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::AttrEscapes
                     << nodeValue
                     << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes
                     << XERCES_CPP_NAMESPACE_QUALIFIER chDoubleQuote;
    }
//     case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::CDATA_SECTION_NODE:
//         m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chBang
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chOpenSquare
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chLatin_C
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chLatin_D
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chOpenSquare 
//                     << nodeValue 
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chCloseSquare
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chCloseSquare
//                     << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;
//         break;
    else if(node->dmNodeKind() == Node::comment_string) {
        m_messageFormatter << XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes 
                    << XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle
                    << XERCES_CPP_NAMESPACE_QUALIFIER chBang
                    << XERCES_CPP_NAMESPACE_QUALIFIER chDash
                    << XERCES_CPP_NAMESPACE_QUALIFIER chDash
                    << nodeValue 
                    << XERCES_CPP_NAMESPACE_QUALIFIER chDash
                    << XERCES_CPP_NAMESPACE_QUALIFIER chDash
                    << XERCES_CPP_NAMESPACE_QUALIFIER chCloseAngle;
    }
//     case XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::DOCUMENT_TYPE_NODE:
//         // don't write it, or we get an invalid XML
//         break;
    else {
        // XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ENTITY_REFERENCE_NODE
        // XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ENTITY_NODE
    }
}

