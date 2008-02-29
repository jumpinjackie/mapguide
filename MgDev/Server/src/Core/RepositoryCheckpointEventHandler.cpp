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
#include "RepositoryCheckpointEventHandler.h"
#include "ServiceManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgRepositoryCheckpointEventHandler::MgRepositoryCheckpointEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 interval = MgConfigProperties::DefaultResourceServicePropertyRepositoryCheckpointsTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::ResourceServicePropertiesSection,
            MgConfigProperties::ResourceServicePropertyRepositoryCheckpointsTimerInterval,
            interval,
            MgConfigProperties::DefaultResourceServicePropertyRepositoryCheckpointsTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetId(MgTimedEvent::RepositoryCheckpoint);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgRepositoryCheckpointEventHandler::~MgRepositoryCheckpointEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgRepositoryCheckpointEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    // Perform repository checkpoints.
    if (MgTimedEvent::RepositoryCheckpoint == eventId)
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        ACE_ASSERT(NULL != serviceManager);

        if (NULL != serviceManager)
        {
            Ptr<MgUserInformation> userInfo = new MgUserInformation(
                MgUser::Administrator, L"");

            MgUserInformation::SetCurrentUserInfo(userInfo);
            serviceManager->PerformRepositoryCheckpoints();
            MgUserInformation::SetCurrentUserInfo(NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgRepositoryCheckpointEventHandler.HandleEvent")
}
