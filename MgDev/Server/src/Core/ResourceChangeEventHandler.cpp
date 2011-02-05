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

#include "MapGuideCommon.h"
#include "ResourceChangeEventHandler.h"
#include "ServiceManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourceChangeEventHandler::MgResourceChangeEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    m_serviceManager = MgServiceManager::GetInstance();
    ACE_ASSERT(NULL != m_serviceManager);

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 interval = MgConfigProperties::DefaultResourceServicePropertyResourceChangeTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::ResourceServicePropertiesSection,
            MgConfigProperties::ResourceServicePropertyResourceChangeTimerInterval,
            interval,
            MgConfigProperties::DefaultResourceServicePropertyResourceChangeTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetId(MgTimedEvent::ResourceChange);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourceChangeEventHandler::~MgResourceChangeEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgResourceChangeEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    // Dispatch resource change notifications.
    if (MgTimedEvent::ResourceChange == eventId)
    {
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(userInfo);
        m_serviceManager->DispatchResourceChangeNotifications();
        MgUserInformation::SetCurrentUserInfo(NULL);
    }

    MG_CATCH_AND_THROW(L"MgResourceChangeEventHandler.HandleEvent")
}
