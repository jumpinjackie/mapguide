//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "ServiceOperation.h"
#include "Connection.h"
#include "LogManager.h"
#include "SessionManager.h"
#include "ServiceManager.h"
#include "ServerSiteService.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServiceOperation::MgServiceOperation() :
    m_currConnection(NULL)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServiceOperation::~MgServiceOperation()
{
    // If the session log is enabled, then exclude any overhead operation
    // from the session log by not specifying the session ID.

    if (NULL != m_currConnection)
    {
        if (IsOverheadOperation())
        {
            m_currConnection->SetSessionId(L"");
        }
        else
        {
            MgLogManager* logMan = MgLogManager::GetInstance();

            if (NULL == logMan || !logMan->IsSessionLogEnabled())
            {
                m_currConnection->SetSessionId(L"");
            }
        }
   }
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the Administrator role.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgServiceOperation::GetAdministratorRole() const
{
    Ptr<MgStringCollection> roles;

    MG_TRY()

    roles = new MgStringCollection;

    roles->Add(MgRole::Administrator);

    MG_CATCH_AND_THROW(L"MgServiceOperation.GetAdministratorRole")

    return roles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the Author role.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgServiceOperation::GetAuthorRole() const
{
    Ptr<MgStringCollection> roles = GetAdministratorRole();

    roles->Add(MgRole::Author);

    return roles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the OGC role.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgServiceOperation::GetOgcRole() const
{
    Ptr<MgStringCollection> roles = GetAdministratorRole();

    roles->Add(MgRole::Ogc);

    return roles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the role(s) required to perform this operation.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgServiceOperation::GetRoles() const
{
    return NULL;
}

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the object from the given stream data and operation packet
/// parameters. Implements IOperation::Init().
/// </summary>
///
/// <param name="data">
/// A pointer to the MgStreamData object that encapsulates the incoming and
/// outgoing stream information.
/// </param>
///
/// <param name = "packet">
/// The operation packet that this operation will be initialized from.
/// </param>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServiceOperation::Init(MgStreamData* data,
    const MgOperationPacket& packet)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::Init()\n" )));
    ACE_ASSERT(0 != data);

    MG_TRY()

    if (0 == data)
    {
        throw new MgNullArgumentException(L"MgServiceOperation.Init",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_data = data;
    m_packet = packet;

    // Pull first argument. It is the user information.
    Ptr<MgStream> stream = new MgStream(m_data->GetStreamHelper());
    Ptr<MgUserInformation> currUserInfo = dynamic_cast<MgUserInformation*>(stream->GetObject());
    assert(currUserInfo != NULL);

    MgUserInformation::SetCurrentUserInfo(currUserInfo);
    m_packet.m_UserInfo = NULL;

    // Update client info in the connection that is associated with this operation.
    m_currConnection = MgConnection::GetCurrentConnection();
    assert(NULL != m_currConnection);

    m_currConnection->SetClientAgent(currUserInfo->GetClientAgent());
    m_currConnection->SetClientIp(currUserInfo->GetClientIp());
    m_currConnection->SetUserName(currUserInfo->GetUserName());
    m_currConnection->SetSessionId(currUserInfo->GetMgSessionId());
    m_currConnection->SetCurrentOperationStatus(MgConnection::OpUnknown);

    // Decrement number of arguments since we have pulled the first one.
    m_packet.m_NumArguments--;

    MG_CATCH_AND_THROW(L"MgServiceOperation.Init")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));

    MG_TRY()

    MgService* service = GetService();
    assert(0 != service);

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 1);
        stream.WriteObject(warning);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 0);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified BOOL value to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream, bool value)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));

    MG_TRY()

    MgService* service = GetService();
    assert(0 != service);

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteBoolean(value);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteBoolean(value);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified INT32 value to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream, INT32 value)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));

    MgService* service = GetService();
    assert(0 != service);

    MG_TRY()

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteInt32(value);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteInt32(value);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified INT64 value to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream, INT64 value)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));

    MgService* service = GetService();
    assert(0 != service);

    MG_TRY()

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteInt64(value);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteInt64(value);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified string to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream, STRING value)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));

    MgService* service = GetService();
    assert(0 != service);

    MG_TRY()

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteString(value);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteString(value);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified object to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream,
    MgSerializable* obj)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));
    // ACE_ASSERT(0 != obj);

    MG_TRY()

    MgService* service = GetService();
    assert(0 != service);

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteObject(obj);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteObject(obj);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified string collection to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream,
    MgStringCollection* stringCollection)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));
    // ACE_ASSERT(0 != stringCollection);

    MG_TRY()

    MgService* service = GetService();
    assert(0 != service);

    if (service->HasWarnings())
    {
        Ptr<MgWarnings> warning = service->GetWarningsObject();

        stream.WriteResponseHeader(MgPacketParser::mecSuccessWithWarning, 2);
        stream.WriteObject(warning);
        stream.WriteObject(stringCollection);
    }
    else
    {
        stream.WriteResponseHeader(MgPacketParser::mecSuccess, 1);
        stream.WriteObject(stringCollection);
    }

    stream.WriteStreamEnd();

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpOk);

    MG_CATCH_AND_THROW(L"MgServiceOperation.WriteResponseStream")
}

///----------------------------------------------------------------------------
/// <summary>
/// Write the specified exception to the response stream.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceOperation::WriteResponseStream(MgStream& stream,
    MgException* except)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgServiceOperation::WriteResponseStream()\n" )));
    ACE_ASSERT(0 != except);

    try
    {
        except->GetMessage();
        except->GetDetails();
        except->GetStackTrace();

        stream.WriteResponseHeader(MgPacketParser::mecFailure, 1);
        stream.WriteObject(except);
        stream.WriteStreamEnd();
    }
    catch (MgException* e) // connection lost?
    {
        SAFE_RELEASE(e);
        assert(false);
    }
    catch (...)
    {
        assert(false);
    }

    m_currConnection->SetCurrentOperationStatus(MgConnection::OpFailed);
}

///----------------------------------------------------------------------------
/// <summary>
/// Authenticates the current user and checks whether or not he/she has an
/// adequate role to perform the current operation.
/// </summary>
///
/// <exceptions>
/// MgAuthenticationFailedException
/// MgUnauthorizedAccessException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServiceOperation::Authenticate()
{
    // Currently, only Server Admin/Site/Resource Service operations need
    // user role authentication. This may change in the future.

    MgServerManager* serverManager = MgServerManager::GetInstance();
    assert(NULL != serverManager);

    if (serverManager->IsSiteServer())
    {
        MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();
        assert(NULL != currUserInfo);
        Ptr<MgStringCollection> requiredRoles = GetRoles();

        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        assert(NULL != serviceManager);
        Ptr<MgServerSiteService> siteService =
            dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(
                MgServiceType::SiteService));
        assert(siteService != NULL);

        siteService->Authenticate(currUserInfo, requiredRoles, false);
    }
}
