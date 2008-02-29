//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "ConnectionTimeoutEventHandler.h"
#include "ServerManager.h"
#include "ClientHandler.h"
#include "Connection.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgConnectionTimeoutEventHandler::MgConnectionTimeoutEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    m_mutexShared = true;

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultGeneralPropertyConnectionTimeout;
    INT32 interval = MgConfigProperties::DefaultGeneralPropertyConnectionTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::GeneralPropertiesSection,
            MgConfigProperties::GeneralPropertyConnectionTimeout,
            timeout,
            MgConfigProperties::DefaultGeneralPropertyConnectionTimeout);
        configuration->GetIntValue(
            MgConfigProperties::GeneralPropertiesSection,
            MgConfigProperties::GeneralPropertyConnectionTimerInterval,
            interval,
            MgConfigProperties::DefaultGeneralPropertyConnectionTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::ConnectionTimeout);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgConnectionTimeoutEventHandler::~MgConnectionTimeoutEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgConnectionTimeoutEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("Client Connection Expiry handled by thread %t\n")));

    // Clean up inactive client connections.
    MgServerManager* serverManager = MgServerManager::GetInstance();
    ACE_ASSERT(NULL != serverManager);

    if (MgTimedEvent::ConnectionTimeout == eventId)
    {
        if (NULL != serverManager)
        {
            ACE_Unbounded_Set<ACE_HANDLE>* clientHandles = serverManager->GetClientHandles();

            if (NULL != clientHandles && !clientHandles->is_empty())
            {
                // Create a temporary collection to hold handles that will be removed from the reactor.
                ACE_Unbounded_Set<ACE_HANDLE> expiredHandles;
                expiredHandles.reset();
                ACE_Unbounded_Set_Iterator<ACE_HANDLE> handleIter(*clientHandles);
                INT32 timeout = m_event.GetTimeout();

                // Cycle through the client handlers to determine if any of the
                // connections in the pool should be closed due to inactivity.
                for (handleIter = clientHandles->begin(); handleIter != clientHandles->end(); handleIter++)
                {
                    // Mutex is shared with MgClientHandler::handle_close.
                    ACE_HANDLE handle = (*handleIter);
                    MgClientHandler* clientHandler = (MgClientHandler*)ACE_Reactor::instance()->find_handler(handle);

                    if (NULL != clientHandler)
                    {
                        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("Found MgClientHandler!\n")));
                        MgConnection* connection = clientHandler->GetConnection();

                        if (NULL != connection && connection->IsExpired(timeout))
                        {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Found Expired Connection!\n")));
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
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Removing connection handler!\n")));
                        ACE_HANDLE handle = (*expiredHandleIter);
                        ACE_Reactor::instance()->remove_handler(handle, ACE_Event_Handler::READ_MASK);
                    }

                    expiredHandles.reset();
                }
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgConnectionTimeoutEventHandler.HandleEvent")
}
