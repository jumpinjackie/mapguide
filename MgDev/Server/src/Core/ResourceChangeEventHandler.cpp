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
#include "ResourceChangeEventHandler.h"
#include "LoadBalanceManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourceChangeEventHandler::MgResourceChangeEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
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
        MG_CHECK_RANGE(
            interval, 
            MgConfigProperties::MinimumResourceServicePropertyResourceChangeTimerInterval, 
            MgConfigProperties::MaximumResourceServicePropertyResourceChangeTimerInterval, 
            L"MgResourceChangeEventHandler.MgResourceChangeEventHandler");
    }

    m_timer.SetInterval(ACE_Time_Value(interval));
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
        MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
        ACE_ASSERT(NULL != loadBalanceManager);

        if (NULL != loadBalanceManager)
        {
            loadBalanceManager->DispatchResourceChangeNotifications(NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgResourceChangeEventHandler.HandleEvent")
}
