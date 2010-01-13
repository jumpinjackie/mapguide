//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "PerformanceLoggingEventHandler.h"
#include "ServiceManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPerformanceLoggingEventHandler::MgPerformanceLoggingEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 interval = MgConfigProperties::DefaultPerformanceLogPropertyInterval;
    m_bPerformanceLogEnabled = MgConfigProperties::DefaultPerformanceLogPropertyEnabled;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::PerformanceLogPropertiesSection,
            MgConfigProperties::PerformanceLogPropertyInterval,
            interval,
            MgConfigProperties::DefaultPerformanceLogPropertyInterval);

        configuration->GetBoolValue(
            MgConfigProperties::PerformanceLogPropertiesSection,
            MgConfigProperties::PerformanceLogPropertyEnabled,
            m_bPerformanceLogEnabled,
            MgConfigProperties::DefaultPerformanceLogPropertyEnabled);
    }

    m_timer.SetInterval(interval);
    m_event.SetId(MgTimedEvent::PerformanceLogging);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPerformanceLoggingEventHandler::~MgPerformanceLoggingEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgPerformanceLoggingEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    // Perform performance statistics logging.
    if (MgTimedEvent::PerformanceLogging == eventId)
    {
        MgServerManager* serverManager = MgServerManager::GetInstance();
        ACE_ASSERT(NULL != serverManager);

        if (NULL != serverManager)
        {
            // We check to see if the log is enabled here as an optimization because the GetInformationProperties() API call can be expensive.
            if(m_bPerformanceLogEnabled)
            {
                Ptr<MgPropertyCollection> properties = serverManager->GetInformationProperties();

                // Log the statistics
                MG_LOG_PERFORMANCE_PROPERTIES_ENTRY(properties);
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgPerformanceLoggingEventHandler.HandleEvent")
}
