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
#include "DataConnectionTimeoutEventHandler.h"
#include "FdoConnectionManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgDataConnectionTimeoutEventHandler::MgDataConnectionTimeoutEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimeout;
    INT32 interval = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyDataConnectionTimeout,
            timeout,
            MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimeout);
        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyDataConnectionTimerInterval,
            interval,
            MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::DataConnectionTimeout);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgDataConnectionTimeoutEventHandler::~MgDataConnectionTimeoutEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgDataConnectionTimeoutEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("FDO Connection Expiry handled by thread %t\n")));

    // Clean up inactive FDO connections.
    if (MgTimedEvent::DataConnectionTimeout == eventId)
    {
        // Cycle through the FDO connections in the pool to determine if any
        // of them should be closed due to inactivity.
        MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();

        if (NULL != fdoConnectionManager)
        {
            Ptr<MgUserInformation> userInfo = new MgUserInformation(
                MgUser::Administrator, L"");

            MgUserInformation::SetCurrentUserInfo(userInfo);
            fdoConnectionManager->RemoveExpiredFdoConnections();
            MgUserInformation::SetCurrentUserInfo(NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgDataConnectionTimeoutEventHandler.HandleEvent")
}
