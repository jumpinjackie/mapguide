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
#include "SessionTimeoutEventHandler.h"
#include "SessionManager.h"
#include "ServiceManager.h"
#include "LongTransactionManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgSessionTimeoutEventHandler::MgSessionTimeoutEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultSiteServicePropertySessionTimeout;
    INT32 interval = MgConfigProperties::DefaultSiteServicePropertySessionTimerInterval;

    // TODO: Validate other configuration properties (e.g. using MG_CHECK_RANGE as below, etc.).
    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::SiteServicePropertiesSection,
            MgConfigProperties::SiteServicePropertySessionTimeout,
            timeout,
            MgConfigProperties::DefaultSiteServicePropertySessionTimeout);
        configuration->GetIntValue(
            MgConfigProperties::SiteServicePropertiesSection,
            MgConfigProperties::SiteServicePropertySessionTimerInterval,
            interval,
            MgConfigProperties::DefaultSiteServicePropertySessionTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::SessionTimeout);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgSessionTimeoutEventHandler::~MgSessionTimeoutEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgSessionTimeoutEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    // Clean up expired sessions.
    if (MgTimedEvent::SessionTimeout == eventId)
    {
        if (m_expiredSessions == NULL)
        {
            m_expiredSessions = new MgStringCollection;
        }

        MgSessionManager::CleanUpSessions(m_event.GetTimeout(), m_expiredSessions);

        if (m_expiredSessions->GetCount() > 0)
        {
            MgLongTransactionManager::RemoveLongTransactionNames(m_expiredSessions);

            MgServiceManager* serviceManager = MgServiceManager::GetInstance();
            ACE_ASSERT(NULL != serviceManager);

            if (NULL != serviceManager)
            {
                Ptr<MgUserInformation> userInfo = new MgUserInformation(
                    MgUser::Administrator, L"");

                MgUserInformation::SetCurrentUserInfo(userInfo);
                m_expiredSessions = serviceManager->CleanUpRepositories(m_expiredSessions);
                MgUserInformation::SetCurrentUserInfo(NULL);
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgSessionTimeoutEventHandler.HandleEvent")
}
