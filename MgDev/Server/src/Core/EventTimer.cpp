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
#include "EventTimer.h"
#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgEventTimer::MgEventTimer() :
    m_id(0),
    m_interval(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgEventTimer::~MgEventTimer()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the timed event handler.
///
MgTimedEventHandler& MgEventTimer::GetEventHandler() const
{
    if (NULL == m_eventHandler.get())
    {
        throw new MgNullReferenceException(
            L"MgEventTimer.GetEventHandler", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return *m_eventHandler.get();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the event timeout.
///
INT32 MgEventTimer::GetEventTimeout() const
{
    return GetEventHandler().GetEvent().GetTimeout();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the timer interval (in seconds).
///
void MgEventTimer::SetInterval(INT32 interval)
{
    m_interval = interval;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes the event timer.
///
void MgEventTimer::Initialize(MgEventTimer::Type type)
{
    MG_TRY()

    if (NULL != m_eventHandler.get() || 0 != m_id)
    {
        throw new MgInvalidOperationException(
            L"MgEventTimer.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Create the event handler that responds to the timer thread.
    m_eventHandler.reset(MgTimedEventHandler::Create(type, *this));

    MG_CATCH_AND_THROW(L"MgEventTimer.Initialize");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Terminates the event timer.
///
void MgEventTimer::Terminate()
{
    MG_TRY()

    // Deactivate the event timer if necessary.
    Deactivate();
    // Destroy the event handler.
    m_eventHandler.reset(NULL);

    MG_CATCH_AND_RELEASE();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Schedules the timer to go off at specified intervals. When the timer goes
/// off, the event will be handled based on an ID.
///
void MgEventTimer::Schedule(const ACE_Time_Value& startTime)
{
    MG_TRY()

    if (NULL == m_eventHandler.get() || 0 != m_id)
    {
        throw new MgInvalidOperationException(
            L"MgEventTimer.Schedule", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    const MgTimedEvent& event = m_eventHandler->GetEvent();
    const long& eventId = event.GetId();
    ACE_Time_Value interval = ACE_Time_Value(m_interval);

    m_id = m_timer.schedule(m_eventHandler.get(), &eventId, startTime, interval);

    MG_CATCH_AND_THROW(L"MgEventTimer.Schedule");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Cancels the timer for all events.
///
void MgEventTimer::Cancel()
{
    MG_TRY()

    if (0 != m_id)
    {
        const long* eventId = NULL;
        int result = m_timer.cancel(m_id, (const void**)&eventId);

        if (1 == result)
        {
            ACE_ASSERT(NULL != eventId);

            if (NULL != m_eventHandler.get())
            {
                ACE_ASSERT(m_eventHandler->GetEvent().GetId() == *eventId);
            }

            m_id = 0;
        }
        else
        {
            ACE_ASSERT(false);
        }
    }

    MG_CATCH_AND_RELEASE();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Activates the event timer.
///
void MgEventTimer::Activate(const ACE_Time_Value& startTime)
{
    MG_TRY()

    if (NULL != m_eventHandler.get() && 0 == m_id)
    {
        // Start up the event timer.
        m_timer.reactor(ACE_Reactor::instance());
        m_timer.activate();

        // Add the event handler to the timer queue.
        Schedule(startTime);
    }

    MG_CATCH_AND_THROW(L"MgEventTimer.Activate");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deactivates the event timer.
///
void MgEventTimer::Deactivate()
{
    MG_TRY()

    if (0 != m_id)
    {
        // Shut down the event timer.
        Cancel();
        m_timer.deactivate();
        m_timer.wait();
        m_timer.close();

        // Remove the event handler.
        if (NULL != m_eventHandler.get())
        {
            ACE_Reactor::instance()->remove_handler(m_eventHandler.get(),
                ACE_Event_Handler::TIMER_MASK | ACE_Event_Handler::DONT_CALL);
        }
    }

    MG_CATCH_AND_RELEASE();
}
