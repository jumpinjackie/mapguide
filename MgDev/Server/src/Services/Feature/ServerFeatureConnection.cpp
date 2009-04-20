//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureConnection.h"
#include "ServiceManager.h"
#include "ServerFeatureUtil.h"
#include "FdoConnectionManager.h"

// Initialize the minimum required memeber variables
MgServerFeatureConnection::MgServerFeatureConnection(MgResourceIdentifier* featureSourceIdentifier)
{
    Initialize();

    // Get the FDO connection from the cache
    MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
    CHECKNULL(pFdoConnectionManager, L"MgServerFeatureConnection.MgServerFeatureConnection()");

    m_fdoConn = pFdoConnectionManager->Open(featureSourceIdentifier);
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.MgServerFeatureConnection()");

    m_resourceId = SAFE_ADDREF(featureSourceIdentifier);
    CHECKNULL(m_resourceId, L"MgServerFeatureConnection.MgServerFeatureConnection()");
}


MgServerFeatureConnection::MgServerFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    Initialize();

    // Get the FDO connection from the cache
    MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
    CHECKNULL(pFdoConnectionManager, L"MgServerFeatureConnection.MgServerFeatureConnection()");

    m_fdoConn = pFdoConnectionManager->Open(providerName, connectionString);
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.MgServerFeatureConnection()");
}


MgServerFeatureConnection::~MgServerFeatureConnection()
{
    MG_TRY()

    if(m_bCloseConnection)
    {
        Close();
    }

    MG_CATCH_AND_RELEASE()
}

void MgServerFeatureConnection::Dispose()
{
    delete this;
}

void MgServerFeatureConnection::Initialize()
{
    m_fdoConn = NULL;
    m_resourceId = NULL;
    m_bCloseConnection = true;
}

void MgServerFeatureConnection::Close()
{
    if (NULL != m_fdoConn)
    {
        // Let the FDO connection manager know we are done with this connection.
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        CHECKNULL(pFdoConnectionManager, L"MgServerFeatureConnection.Close");

        pFdoConnectionManager->Close(m_fdoConn);
        m_fdoConn = NULL;
    }
}

FdoIConnection* MgServerFeatureConnection::GetConnection()
{
    return FDO_SAFE_ADDREF(m_fdoConn);
}


STRING MgServerFeatureConnection::GetProviderName()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.GetProviderName");

    // Get FdoIConnectionInfo
    FdoPtr<FdoIConnectionInfo> fdoConnInfo = m_fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgServerFeatureConnection.GetProviderName");

    return fdoConnInfo->GetProviderName();
}


bool MgServerFeatureConnection::IsConnectionOpen()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.IsConnectionOpen()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Open != state)
        return false;

    return true;
}


bool MgServerFeatureConnection::IsConnectionPending()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.IsConnectionPending()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Pending != state)
        return false;

    return true;
}


bool MgServerFeatureConnection::IsConnectionBusy()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.IsConnectionBusy()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Busy != state)
        return false;

    return true;
}


bool MgServerFeatureConnection::IsConnectionClosed()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.IsConnectionClosed()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Closed != state)
        return false;

    return true;
}


bool MgServerFeatureConnection::SupportsCommand(INT32 commandType)
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.SupportsCommand");

    FdoPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgServerFeatureConnection.SupportsCommand");

    bool supports = false;

    // Find all supported command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        for (FdoInt32 i=0; i < cnt; i++)
        {
            if (fcmd[i] == (FdoInt32)commandType)
            {
                supports = true;
                break;
            }
        }
    }

    return supports;
}

void MgServerFeatureConnection::HasReader()
{
    m_bCloseConnection = false;
}
