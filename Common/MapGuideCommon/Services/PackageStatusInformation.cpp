//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "PackageStatusInformation.h"

MG_IMPL_DYNCREATE(MgPackageStatusInformation);

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPackageStatusInformation::MgPackageStatusInformation() :
    m_statusCode(MgPackageStatusCode::Unknown),
    m_packageSize(0),
    m_opsFailed(0),
    m_opsReceived(0)
{
    m_startTime = new MgDateTime();
    m_packageDate = new MgDateTime();
    m_endTime = new MgDateTime();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copy constructor.
///
MgPackageStatusInformation::MgPackageStatusInformation(const MgPackageStatusInformation& packageStatusInfo)
{
    *this = packageStatusInfo;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPackageStatusInformation::~MgPackageStatusInformation()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Assignment operator.
///
MgPackageStatusInformation& MgPackageStatusInformation::operator=(const MgPackageStatusInformation& packageStatusInfo)
{
    if (&packageStatusInfo != this)
    {
        m_apiName               = packageStatusInfo.m_apiName;
        m_statusCode            = packageStatusInfo.m_statusCode;
        m_packageName           = packageStatusInfo.m_packageName;
        m_packageDescription    = packageStatusInfo.m_packageDescription;

        if (packageStatusInfo.m_packageDate == NULL)
        {
            m_packageDate       = NULL;
        }
        else
        {
            m_packageDate   = new MgDateTime(*packageStatusInfo.m_packageDate.p);
        }

        m_packageSize           = packageStatusInfo.m_packageSize;
        m_userName              = packageStatusInfo.m_userName;
        m_serverName            = packageStatusInfo.m_serverName;
        m_serverAddress         = packageStatusInfo.m_serverAddress;

        if (packageStatusInfo.m_startTime == NULL)
        {
            m_startTime         = NULL;
        }
        else
        {
            m_startTime         = new MgDateTime(*packageStatusInfo.m_startTime.p);
        }

        if (packageStatusInfo.m_endTime == NULL)
        {
            m_endTime           = NULL;
        }
        else
        {
            m_endTime           = new MgDateTime(*packageStatusInfo.m_endTime.p);
        }

        m_opsFailed             = packageStatusInfo.m_opsFailed;
        m_opsReceived           = packageStatusInfo.m_opsReceived;
        m_errorMessage          = packageStatusInfo.m_errorMessage;
        m_stackTrace            = packageStatusInfo.m_stackTrace;
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgPackageStatusInformation::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the class ID.
///
/// \returns
/// Class ID.
///
INT32 MgPackageStatusInformation::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
/// \param stream
/// Stream.
///
void MgPackageStatusInformation::Serialize(MgStream* stream)
{
    stream->WriteString(m_apiName);
    stream->WriteString(m_statusCode);
    stream->WriteString(m_packageName);
    stream->WriteString(m_packageDescription);
    stream->WriteObject(m_packageDate);
    stream->WriteInt64(m_packageSize);
    stream->WriteString(m_userName);
    stream->WriteString(m_serverName);
    stream->WriteString(m_serverAddress);
    stream->WriteObject(m_startTime);
    stream->WriteObject(m_endTime);
    stream->WriteInt32(m_opsFailed);
    stream->WriteInt32(m_opsReceived);
    stream->WriteString(m_errorMessage);
    stream->WriteString(m_stackTrace);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
/// \param stream
/// Stream.
///
void MgPackageStatusInformation::Deserialize(MgStream* stream)
{
    stream->GetString(m_apiName);
    stream->GetString(m_statusCode);
    stream->GetString(m_packageName);
    stream->GetString(m_packageDescription);
    m_packageDate = (MgDateTime*)stream->GetObject();
    stream->GetInt64(m_packageSize);
    stream->GetString(m_userName);
    stream->GetString(m_serverName);
    stream->GetString(m_serverAddress);
    m_startTime = (MgDateTime*)stream->GetObject();
    m_endTime = (MgDateTime*)stream->GetObject();
    stream->GetInt32(m_opsFailed);
    stream->GetInt32(m_opsReceived);
    stream->GetString(m_errorMessage);
    stream->GetString(m_stackTrace);
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the package status message.
///
/// \return
/// The package status message.
///
STRING MgPackageStatusInformation::GetStatusMessage()
{
    STRING statusMessage;

    MG_TRY()

    if (MgPackageStatusCode::Unknown  != m_statusCode
     && MgPackageApiName::LoadPackage != m_apiName
     && MgPackageApiName::MakePackage != m_apiName)
    {
        throw new MgInvalidOperationException(
            L"MgPackageStatusInformation.GetStatusMessage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING resourceId;
    MgStringCollection arguments;

    if (MgPackageStatusCode::Succeeded == m_statusCode)
    {
        resourceId = (MgPackageApiName::LoadPackage == m_apiName) ?
            L"MgPackageStatusMessageLoadPackageSucceeded" :
            L"MgPackageStatusMessageMakePackageSucceeded";
        arguments.Add(m_serverName.empty() ? m_serverAddress : m_serverName);
        arguments.Add(m_endTime->ToXmlString(false));
    }
    else if (MgPackageStatusCode::Failed == m_statusCode)
    {
        resourceId = (MgPackageApiName::LoadPackage == m_apiName) ?
            L"MgPackageStatusMessageLoadPackageFailed" :
            L"MgPackageStatusMessageMakePackageFailed";
        arguments.Add(m_serverName.empty() ? m_serverAddress : m_serverName);
        arguments.Add(m_endTime->ToXmlString(false));
    }
    else if (MgPackageStatusCode::InProgress == m_statusCode)
    {
        resourceId = (MgPackageApiName::LoadPackage == m_apiName) ?
            L"MgPackageStatusMessageLoadPackageInProgress" :
            L"MgPackageStatusMessageMakePackageInProgress";
        arguments.Add(m_packageName);
    }
    else if (MgPackageStatusCode::NotStarted == m_statusCode)
    {
        resourceId = L"MgPackageStatusMessageNotStarted";
    }
    else // if (MgPackageStatusCode::Unknown == m_statusCode)
    {
        resourceId = L"MgPackageStatusMessageUnknown";
    }

    MgResources* resources = MgResources::GetInstance();
    ACE_ASSERT(NULL != resources);

    if (NULL == resources)
    {
        statusMessage = MgResources::Unknown;
    }
    else
    {
        statusMessage = resources->GetMessage(MgResources::ResourceService,
            resourceId, &arguments);
    }

    MG_CATCH_AND_THROW(L"MgPackageStatusInformation.GetStatusMessage");

    return statusMessage;
}

STRING MgPackageStatusInformation::GetStatusCode()
{
    return m_statusCode;
}

void MgPackageStatusInformation::SetStatusCode(CREFSTRING statusCode)
{
    m_statusCode = statusCode;
}

STRING MgPackageStatusInformation::GetApiName()
{
    return m_apiName;
}

void MgPackageStatusInformation::SetApiName(CREFSTRING apiName)
{
    if (MgPackageApiName::LoadPackage != apiName &&
        MgPackageApiName::MakePackage != apiName)
    {
        throw new MgInvalidOperationException(
            L"MgPackageStatusInformation.SetApiName",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_apiName = apiName;
}

STRING MgPackageStatusInformation::GetPackageName()
{
    return m_packageName;
}

void MgPackageStatusInformation::SetPackageName(CREFSTRING packageName)
{
    m_packageName = packageName;
}

STRING MgPackageStatusInformation::GetPackageDescription()
{
    return m_packageDescription;
}

void MgPackageStatusInformation::SetPackageDescription(CREFSTRING packageDescription)
{
    m_packageDescription = packageDescription;
}

MgDateTime* MgPackageStatusInformation::GetPackageDate()
{
    return SAFE_ADDREF((MgDateTime*)m_packageDate);
}

void MgPackageStatusInformation::SetPackageDate(MgDateTime* packageDate)
{
    m_packageDate = SAFE_ADDREF(packageDate);
}

INT64 MgPackageStatusInformation::GetPackageSize()
{
    return m_packageSize;
}

void MgPackageStatusInformation::SetPackageSize(INT64 packageSize)
{
    m_packageSize = packageSize;
}

STRING MgPackageStatusInformation::GetUserName()
{
    return m_userName;
}

void MgPackageStatusInformation::SetUserName(CREFSTRING userName)
{
    m_userName = userName;
}

STRING MgPackageStatusInformation::GetServerName()
{
    return m_serverName;
}

void MgPackageStatusInformation::SetServerName(CREFSTRING serverName)
{
    m_serverName = serverName;
}

STRING MgPackageStatusInformation::GetServerAddress()
{
    return m_serverAddress;
}

void MgPackageStatusInformation::SetServerAddress(CREFSTRING serverAddress)
{
    m_serverAddress = serverAddress;
}

MgDateTime* MgPackageStatusInformation::GetStartTime()
{
    return SAFE_ADDREF((MgDateTime*)m_startTime);
}

void MgPackageStatusInformation::SetStartTime(MgDateTime* startTime)
{
    m_startTime = SAFE_ADDREF(startTime);
}

MgDateTime* MgPackageStatusInformation::GetEndTime()
{
    return SAFE_ADDREF((MgDateTime*)m_endTime);
}

void MgPackageStatusInformation::SetEndTime(MgDateTime* endTime)
{
    m_endTime = SAFE_ADDREF(endTime);
}

INT32 MgPackageStatusInformation::GetOperationsFailed()
{
    return m_opsFailed;
}

void MgPackageStatusInformation::SetOperationsFailed(INT32 opsFailed)
{
    m_opsFailed = opsFailed;
}

INT32 MgPackageStatusInformation::GetOperationsReceived()
{
    return m_opsReceived;
}

void MgPackageStatusInformation::SetOperationsReceived(INT32 opsReceived)
{
    m_opsReceived = opsReceived;
}

double MgPackageStatusInformation::GetAverageOperationTime()
{
    // The average operation time is in milliseconds.
    INT32 opsSucceeded = m_opsReceived - m_opsFailed;

    if (opsSucceeded > 0)
    {
        double totalOpTime = m_endTime->ToMilliSeconds() - m_startTime->ToMilliSeconds();

        return (totalOpTime / opsSucceeded);
    }
    else
    {
        return 0.0;
    }
}

STRING MgPackageStatusInformation::GetErrorMessage()
{
    return m_errorMessage;
}

void MgPackageStatusInformation::SetErrorMessage(CREFSTRING errorMessage)
{
    m_errorMessage = errorMessage;
}

STRING MgPackageStatusInformation::GetStackTrace()
{
    return m_stackTrace;
}

void MgPackageStatusInformation::SetStackTrace(CREFSTRING stackTrace)
{
    m_stackTrace = stackTrace;
}
