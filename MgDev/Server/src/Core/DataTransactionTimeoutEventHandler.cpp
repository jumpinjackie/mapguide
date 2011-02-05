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
#include "DataTransactionTimeoutEventHandler.h"
#include "ServerFeatureTransactionPool.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgDataTransactionTimeoutEventHandler::MgDataTransactionTimeoutEventHandler(MgEventTimer& timer) :
    MgTimedEventHandler(timer)
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);
    INT32 timeout = MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimeout;
    INT32 interval = MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimerInterval;

    if (NULL != configuration)
    {
        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyDataTransactionTimeout,
            timeout,
            MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimeout);
        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyDataTransactionTimerInterval,
            interval,
            MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimerInterval);
    }

    m_timer.SetInterval(interval);
    m_event.SetTimeout(timeout);
    m_event.SetId(MgTimedEvent::DataTransactionTimeout);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgDataTransactionTimeoutEventHandler::~MgDataTransactionTimeoutEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the specified event.
///
void MgDataTransactionTimeoutEventHandler::HandleEvent(long eventId)
{
    MG_TRY()

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("FDO transaction Expiry handled by thread %t\n")));

    // Clean up timeout transactions.
    if (MgTimedEvent::DataTransactionTimeout == eventId)
    {
        // Cycle through the transactions in the pool to determine if any
        // of them should be committed or rollbacked due to timeout.
        MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();

        if (NULL != transactionPool)
        {
            Ptr<MgUserInformation> userInfo = new MgUserInformation(
                MgUser::Administrator, L"");

            MgUserInformation::SetCurrentUserInfo(userInfo);
            transactionPool->RemoveExpiredTransaction();
            MgUserInformation::SetCurrentUserInfo(NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgDataTransactionTimeoutEventHandler.HandleEvent")
}
