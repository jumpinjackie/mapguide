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
#include "ServiceRegistrationEventHandler.h"
#include "LoadBalanceManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgServiceRegistrationEventHandler::MgServiceRegistrationEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 interval = MgConfigProperties::DefaultGeneralPropertyServiceRegistrationTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::GeneralPropertiesSection,
            MgConfigProperties::GeneralPropertyServiceRegistrationTimerInterval,
            interval,
            MgConfigProperties::DefaultGeneralPropertyServiceRegistrationTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetId(MgTimedEvent::ServiceRegistration);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgServiceRegistrationEventHandler::~MgServiceRegistrationEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgServiceRegistrationEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    // Register server services.
    if (MgTimedEvent::ServiceRegistration == eventId)
    {
        MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
        ACE_ASSERT(NULL != loadBalanceManager);

        // Cancel the timer if the event is successfully handled.
        if (NULL != loadBalanceManager && loadBalanceManager->RegisterServices())
        {
            m_timer.Cancel();
        }
    }

    MG_CATCH_AND_THROW(L"MgServiceRegistrationEventHandler.HandleEvent")
}
