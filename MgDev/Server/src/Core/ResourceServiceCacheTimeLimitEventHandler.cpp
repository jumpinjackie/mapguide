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
#include "ResourceServiceCacheTimeLimitEventHandler.h"
#include "CacheManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourceServiceCacheTimeLimitEventHandler::MgResourceServiceCacheTimeLimitEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultResourceServicePropertyCacheTimeLimit;
    INT32 interval = MgConfigProperties::DefaultResourceServicePropertyCacheTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::ResourceServicePropertiesSection,
            MgConfigProperties::ResourceServicePropertyCacheTimeLimit,
            timeout,
            MgConfigProperties::DefaultResourceServicePropertyCacheTimeLimit);

        configuration->GetIntValue(
            MgConfigProperties::ResourceServicePropertiesSection,
            MgConfigProperties::ResourceServicePropertyCacheTimerInterval,
            interval,
            MgConfigProperties::DefaultResourceServicePropertyCacheTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::ResourceServiceCacheTimeLimit);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourceServiceCacheTimeLimitEventHandler::~MgResourceServiceCacheTimeLimitEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgResourceServiceCacheTimeLimitEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("Expired Resource Service cache entries handled by thread %t\n")));

    // Clean up resource service cache.
    if (MgTimedEvent::ResourceServiceCacheTimeLimit == eventId)
    {
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(userInfo);

        MgResourceServiceCache* resourceServiceCache = MgCacheManager::GetInstance()->GetResourceServiceCache();
        resourceServiceCache->RemoveExpiredEntries();

        MgUserInformation::SetCurrentUserInfo(NULL);
    }

    MG_CATCH_AND_THROW(L"MgResourceServiceCacheTimeLimitEventHandler.HandleEvent")
}
