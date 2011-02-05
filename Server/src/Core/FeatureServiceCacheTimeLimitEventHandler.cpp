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
#include "FeatureServiceCacheTimeLimitEventHandler.h"
#include "CacheManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgFeatureServiceCacheTimeLimitEventHandler::MgFeatureServiceCacheTimeLimitEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit;
    INT32 interval = MgConfigProperties::DefaultFeatureServicePropertyCacheTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyCacheTimeLimit,
            timeout,
            MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit);

        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyCacheTimerInterval,
            interval,
            MgConfigProperties::DefaultFeatureServicePropertyCacheTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::FeatureServiceCacheTimeLimit);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgFeatureServiceCacheTimeLimitEventHandler::~MgFeatureServiceCacheTimeLimitEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgFeatureServiceCacheTimeLimitEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("Expired Feature Service cache entries handled by thread %t\n")));

    // Clean up feature service cache.
    if (MgTimedEvent::FeatureServiceCacheTimeLimit == eventId)
    {
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(userInfo);

        MgFeatureServiceCache* featureServiceCache = MgCacheManager::GetInstance()->GetFeatureServiceCache();
        featureServiceCache->RemoveExpiredEntries();

        MgUserInformation::SetCurrentUserInfo(NULL);
    }

    MG_CATCH_AND_THROW(L"MgFeatureServiceCacheTimeLimitEventHandler.HandleEvent")
}
