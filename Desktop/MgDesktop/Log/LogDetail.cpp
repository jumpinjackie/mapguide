//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "LogDetail.h"

MgLogDetail::MgLogDetail(INT32 serviceNum, INT8 detail, CREFSTRING methodName, REFSTRING errorLogVar)
: m_errorLogVar(errorLogVar)
{
    MgLogManager* logMgr = MgLogManager::GetInstance();
    m_minDetail = logMgr->GetDetailLevelForService(serviceNum);
    m_bLoggingActive = logMgr->IsTraceLogEnabled();
    m_delimiter = logMgr->GetLogDelimiter();
    m_detail = detail;
    m_methodName = methodName;
}

MgLogDetail::~MgLogDetail()
{
    Terminate();
}

void MgLogDetail::AppendName(CREFSTRING paramName)
{
    if (m_params.length() > 0)
    {
        m_params.append(m_delimiter);
    }
    m_params.append(paramName);
    m_params.append(L"=");
}

 bool MgLogDetail::ParamsActive()
 {
     return m_minDetail > MgLogDetail::Error;
 }

 bool MgLogDetail::ShouldLog()
 {
     return m_detail <= m_minDetail;
 }

void MgLogDetail::AddResourceIdentifier(CREFSTRING paramName, MgResourceIdentifier* resId)
{

    if (NULL != resId && ParamsActive())
    {
        AppendName(paramName);
        m_params.append(resId->ToString());
    }
}

void MgLogDetail::AddInt64(CREFSTRING paramName, INT64 paramValue)
{
    if (ParamsActive())
    {
        AppendName(paramName);
        STRING temp;
        MgUtil::Int64ToString(paramValue, temp);
        m_params.append(temp);
    }
}

void MgLogDetail::AddInt32(CREFSTRING paramName, INT32 paramValue)
{
    if (ParamsActive())
    {
        AppendName(paramName);
        STRING temp;
        MgUtil::Int32ToString(paramValue, temp);
        m_params.append(temp);
    }
}

void MgLogDetail::AddBool(CREFSTRING paramName, bool paramValue)
{
    if (ParamsActive())
    {
        AppendName(paramName);
        m_params.append(paramValue ? L"1" : L"0");
    }
}

void MgLogDetail::AddString(CREFSTRING paramName, CREFSTRING paramValue)
{
    if (ParamsActive())
    {
        AppendName(paramName);
        m_params.append(paramValue);
    }
}

void MgLogDetail::AddObject(CREFSTRING paramName, MgSerializable* object)
{
    if (NULL != object && ParamsActive())
    {
        AppendName(paramName);
        m_params.append(object->GetLogString());
    }
}

void MgLogDetail::Create()
{
    // Always propagate parameters for exception message
    m_errorLogVar = m_params;

    if (ShouldLog())
    {
        STRING tempStr;
        tempStr.append(L"BGN");
        tempStr.append(m_delimiter);
        tempStr.append(m_methodName);
        tempStr.append(m_delimiter);
        tempStr.append(m_params);
        MG_LOG_TRACE_ENTRY(tempStr);
    }
}

void MgLogDetail::Terminate()
{
    if (ShouldLog())
    {
        STRING tempStr;
        tempStr.append(L"END");
        tempStr.append(m_delimiter);
        tempStr.append(m_methodName);
        MG_LOG_TRACE_ENTRY(tempStr);
    }
}
