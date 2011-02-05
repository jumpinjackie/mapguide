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
#include "EventTimerManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgEventTimerManager::MgEventTimerManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgEventTimerManager::~MgEventTimerManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns an event timer based on the specified type.
///
MgEventTimer& MgEventTimerManager::GetEventTimer(MgEventTimer::Type type)
{
    INT32 index = static_cast<int>(type);
    MG_CHECK_RANGE(index, 0, MG_MAX_EVENT_TIMERS - 1,
        L"MgEventTimerManager.GetEventTimer");

    return m_eventTimers[index];
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes all the event timers.
///
void MgEventTimerManager::Initialize()
{
    MG_TRY()

    for (int i = 0; i < MG_MAX_EVENT_TIMERS; ++i)
    {
        m_eventTimers[i].Initialize(static_cast<MgEventTimer::Type>(i));
    }

    MG_CATCH_AND_THROW(L"MgEventTimerManager.Initialize");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Activates all the event timers.
///
void MgEventTimerManager::Activate()
{
    MG_TRY()

    ACE_Time_Value startTime = ACE_OS::gettimeofday() + ACE_Time_Value(5); // 5-second delay after server startup

    for (int i = 0; i < MG_MAX_EVENT_TIMERS; ++i)
    {
        m_eventTimers[i].Activate(startTime);
    }

    MG_CATCH_AND_THROW(L"MgEventTimerManager.Initialize");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deactivates all the event timers.
///
void MgEventTimerManager::Deactivate()
{
    MG_TRY()

    for (int i = 0; i < MG_MAX_EVENT_TIMERS; ++i)
    {
        m_eventTimers[i].Deactivate();
    }

    MG_CATCH_AND_RELEASE();
}
