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
#include "IdleTimeoutHandler.h"
#include "Server.h"
#include "ServerCommon.h"
#include "ClientHandler.h"
#include "Connection.h"
#include "FdoConnectionManager.h"
#include "SessionManager.h"
#include "ServiceManager.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgIdleTimeoutHandler::MgIdleTimeoutHandler(MgServer* server,
    INT32 nConnectionTimeout, INT32 nSessionTimeout) :
    m_server(server),
    m_nConnectionTimeout(nConnectionTimeout),
    m_nSessionTimeout(nSessionTimeout)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgIdleTimeoutHandler::~MgIdleTimeoutHandler()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method is called when the associated timer fires.  The method
/// determines which event caused the timeout and decides whether to look
/// for idle connections or idle data connections.
/// </summary>
int MgIdleTimeoutHandler::handle_timeout(const ACE_Time_Value &currentTime, const void *arg)
{
    ACE_UNUSED_ARG(currentTime);

    MG_TRY()

    const int *val = ACE_static_cast(const int*, arg);

    switch(*val)
    {
        case evtConnection:
            {
            //ACE_DEBUG( (LM_DEBUG, ACE_TEXT("Connection Expiry handled by thread %t\n")));

            // Here we need to cycle through the set of client handles to determine if any of the connections
            // should be closed due to inactivity.

            // Mutex is shared with MgClientHandler::handle_close
            ACE_MT (ACE_GUARD_ACTION(ACE_Lock, ace_mon, ACE_Reactor::instance()->lock(), FindAndCloseIdleConnections();, ;));
            }
            break;

        case evtDataConnection:
            //ACE_DEBUG( (LM_DEBUG, ACE_TEXT("DataConnection Expiry handled by thread %t\n")));

            // Here we need to cycle through the connections in the pool to determine if any of them
            // should be closed due to inactivity.
            FindAndCloseIdleDataConnections();
            break;

        case evtServiceRegistration:
            RegisterServices();
            break;

        case evtSessionCleanup:
            CleanUpSessions();
            break;

        case evtRepositoryCheckpoints:
            PerformRepositoryCheckpoints();
            break;

        default:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("Error - invalid timer ID.\n")));
            break;
    }

    MG_CATCH(L"MgIdleTimeoutHandler.handle_timeout")

    if (mgException != NULL)
    {
        MgServerManager* serverManager = MgServerManager::GetInstance();
        assert(NULL != serverManager);
        CREFSTRING defaultLocale = serverManager->GetDefaultLocale();

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(defaultLocale).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(defaultLocale).c_str(), mgException->GetStackTrace(defaultLocale).c_str());
    }

    return 0;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Helper method called from handle_timeout to seach for inactive data connections
/// and remove them from the connection pool.
/// </summary>
void MgIdleTimeoutHandler::FindAndCloseIdleDataConnections()
{
    //ACE_DEBUG( (LM_DEBUG, ACE_TEXT("FindAndCloseIdleDataConnections()\n")));

    MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
    CHECKNULL(pFdoConnectionManager, L"MgServerFeatureConnection.MgServerFeatureConnection()");

    pFdoConnectionManager->RemoveExpiredConnections();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Helper method called from handle_timeout to seach for inactive connections
/// and shutdown and remove the associated connection.
/// </summary>
void MgIdleTimeoutHandler::FindAndCloseIdleConnections()
{
    //ACE_DEBUG( (LM_DEBUG, ACE_TEXT("(%P|%t) FindAndCloseIdleConnections()\n")));

    // Iterate the handles, obtaining the MgClientHandler object and its Connection.  Examine the connection to
    // determine if it has timed-out, any flag any expired connections (handlers) for removal.
    MgServerManager* pServerManager = MgServerManager::GetInstance();
    if(pServerManager)
    {
        ACE_Unbounded_Set<ACE_HANDLE>* pClientHandles = pServerManager->GetClientHandles();

        if (pClientHandles && !pClientHandles->is_empty() )
        {
            // Create a temporary collection to hold handles that will be removed from the reactor.
            ACE_Unbounded_Set<ACE_HANDLE> expiredHandles;
            expiredHandles.reset();
            ACE_Unbounded_Set_Iterator<ACE_HANDLE> handleIter(*pClientHandles);
            for (handleIter = pClientHandles->begin(); handleIter != pClientHandles->end(); handleIter++)
            {
                ACE_HANDLE handle = (*handleIter);
                MgClientHandler* pClientHandler = NULL;
                pClientHandler = (MgClientHandler*)ACE_Reactor::instance()->find_handler(handle);
                if (pClientHandler)
                {
                    //ACE_DEBUG( (LM_DEBUG, ACE_TEXT("Found MgClientHandler!\n")));
                    MgConnection* pConnection = pClientHandler->GetConnection();
                    if ( pConnection && pConnection->IsExpired(m_nConnectionTimeout) )
                    {
                        ACE_DEBUG( (LM_DEBUG, ACE_TEXT("(%P|%t) Found Expired Connection!\n")));
                        expiredHandles.insert(handle);
                    }
                }
            }

            // Unregister any expired handles from the reactor and remove from our collection of MgClientHandlers.
            if (!expiredHandles.is_empty())
            {
                ACE_Unbounded_Set_Iterator<ACE_HANDLE> expiredHandleIter(expiredHandles);
                for (expiredHandleIter = expiredHandles.begin(); expiredHandleIter != expiredHandles.end(); expiredHandleIter++)
                {
                    ACE_DEBUG( (LM_DEBUG, ACE_TEXT("(%P|%t) Removing connection handler !\n")));

                    ACE_HANDLE handle = (*expiredHandleIter);
                    ACE_Reactor::instance()->remove_handler(handle, ACE_Event_Handler::READ_MASK);
                }
                expiredHandles.reset();
            }
        }
    }

}

///----------------------------------------------------------------------------
/// <summary>
/// Performs the service registration if this has not been done yet.
/// Here we need to check if the site server is online so that this
/// support server can register its services.
/// </summary>
///----------------------------------------------------------------------------

void MgIdleTimeoutHandler::RegisterServices()
{
    if (NULL != m_server)
    {
        m_server->RegisterServices();
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up expired sessions.
/// </summary>
///----------------------------------------------------------------------------

void MgIdleTimeoutHandler::CleanUpSessions()
{
    if (m_expiredSessions == NULL)
    {
        m_expiredSessions = new MgStringCollection;
    }

    MgSessionManager::CleanUpSessions(m_nSessionTimeout, m_expiredSessions);

    // Use the Administrator account to delete expired session repositories.

    if (m_expiredSessions->GetCount() > 0)
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        assert(NULL != serviceManager);
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(adminUserInfo);
        m_expiredSessions = serviceManager->CleanUpRepositories(m_expiredSessions);
        MgUserInformation::SetCurrentUserInfo(NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs checkpoints for all the repositories.
/// </summary>
///----------------------------------------------------------------------------

void MgIdleTimeoutHandler::PerformRepositoryCheckpoints()
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    assert(NULL != serviceManager);

    serviceManager->PerformRepositoryCheckpoints();
}
