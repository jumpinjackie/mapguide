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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureConnection.h"
#include "ServiceManager.h"
#include "ResourceService.h"
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
    if (NULL != m_fdoConn)
    {
        // Let the FDO connection manager know we are done with this connection

        // Get the FDO connection from the cache
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        CHECKNULL(pFdoConnectionManager, L"MgServerFeatureConnection.~MgServerFeatureConnection()");

        pFdoConnectionManager->Close(m_fdoConn);
    }
}

void MgServerFeatureConnection::Dispose(void)
{
    delete this;
}

void MgServerFeatureConnection::Initialize()
{
    m_fdoConn = NULL;
    m_resourceId = NULL;
}


FdoIConnection* MgServerFeatureConnection::GetConnection()
{
    return GIS_SAFE_ADDREF(m_fdoConn);
}


STRING MgServerFeatureConnection::GetProviderName()
{
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.SetConnectionProperties()");

    // Get FdoIConnectionInfo
    GisPtr<FdoIConnectionInfo> fdoConnInfo = m_fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgServerFeatureConnection.SetConnectionProperties()");

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
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.IsConnectionOpen()");
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
    CHECKNULL(m_fdoConn, L"MgServerFeatureConnection.SupportsConfiguration()");

    GisPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

    bool supports = false;

    // Find all supported command types
    GisInt32 cnt = 0;
    GisInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        for (GisInt32 i=0; i < cnt; i++)
        {
            if (fcmd[i] == (GisInt32)commandType)
            {
                supports = true;
            }
        }
    }

    return supports;
}


MgSpatialContextInfoMap* MgServerFeatureConnection::GetSpatialContextInfoMap()
{
    MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
    return pFdoConnectionManager->GetSpatialContextInfo(m_resourceId);
}
