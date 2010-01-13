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
#include "TimedEventHandler.h"
#include "ServerManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgTimedEventHandler::MgTimedEventHandler(MgEventTimer& timer) :
    m_timer(timer),
    m_mutexShared(false)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgTimedEventHandler::~MgTimedEventHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an event handler based on the specified timer type.
///
MgTimedEventHandler* MgTimedEventHandler::Create(MgEventTimer::Type type,
    MgEventTimer& timer)
{
    MgServerManager* serverManager = MgServerManager::GetInstance();
    ACE_ASSERT(NULL != serverManager);

    if (NULL == serverManager)
    {
        throw new MgNullReferenceException(L"MgTimedEventHandler.Create",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgTimedEventHandler> eventHandler;
    bool isSiteServer = serverManager->IsSiteServer();

    switch (type)
    {
        case MgEventTimer::ServiceRegistration:
            eventHandler.reset(new MgServiceRegistrationEventHandler(timer));
            break;

        case MgEventTimer::SessionTimeout:
            eventHandler.reset(isSiteServer ? new MgSessionTimeoutEventHandler(timer) : NULL);
            break;

        case MgEventTimer::ConnectionTimeout:
            eventHandler.reset(new MgConnectionTimeoutEventHandler(timer));
            break;

        case MgEventTimer::DataConnectionTimeout:
            eventHandler.reset(new MgDataConnectionTimeoutEventHandler(timer));
            break;

        case MgEventTimer::RepositoryCheckpoint:
            eventHandler.reset(isSiteServer ? new MgRepositoryCheckpointEventHandler(timer) : NULL);
            break;

        case MgEventTimer::ResourceChange:
            eventHandler.reset(isSiteServer ? new MgResourceChangeEventHandler(timer) : NULL);
            break;

        case MgEventTimer::FeatureServiceCacheTimeLimit:
            eventHandler.reset(new MgFeatureServiceCacheTimeLimitEventHandler(timer));
            break;

        case MgEventTimer::DataTransactionTimeout:
            eventHandler.reset(new MgDataTransactionTimeoutEventHandler(timer));
            break;

        case MgEventTimer::PerformanceLogging:
            eventHandler.reset(new MgPerformanceLoggingEventHandler(timer));
            break;

        default:
            throw new MgInvalidArgumentException(
                L"MgTimedEventHandler.Create", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return eventHandler.release();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handles the event that caused the timeout (when the associated timer fires).
///
int MgTimedEventHandler::handle_timeout(const ACE_Time_Value& currentTime, const void* arg)
{
    MG_TRY()

    ACE_UNUSED_ARG(currentTime);
    const long* eventId = ACE_static_cast(const long*, arg);

    if (NULL != eventId)
    {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Handling Event ID: %d\n"), *eventId));

        if (m_mutexShared)
        {
            ACE_MT(ACE_GUARD_ACTION(ACE_Lock, ace_mon, ACE_Reactor::instance()->lock(), HandleEvent(*eventId);, ;));
        }
        else
        {
            HandleEvent(*eventId);
        }
    }

    MG_CATCH(L"MgTimedEventHandler.handle_timeout")

    if (mgException != NULL)
    {
        LogException(*mgException);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Logs the specified exception.
///
void MgTimedEventHandler::LogException(MgException& e)
{
    MG_TRY()

    MgServerManager* serverManager = MgServerManager::GetInstance();
    ACE_ASSERT(NULL != serverManager);

    if (NULL != serverManager)
    {
        CREFSTRING locale = serverManager->GetDefaultMessageLocale();

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), e.GetDetails(locale).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e.GetMessage(locale).c_str(), e.GetStackTrace(locale).c_str());
    }

    MG_CATCH_AND_RELEASE()
}
