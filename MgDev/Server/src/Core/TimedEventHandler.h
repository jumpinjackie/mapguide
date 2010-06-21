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

#ifndef MGTIMEDEVENTHANDLER_H_
#define MGTIMEDEVENTHANDLER_H_

#include "ace/Reactor.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

#include "Server.h"
#include "TimedEvent.h"
#include "EventTimer.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Base class to handle a timed event.
///
class MgTimedEventHandler : public ACE_Event_Handler
{
/// Constructors/Destructor

public:

    explicit MgTimedEventHandler(MgEventTimer& timer);
    virtual ~MgTimedEventHandler();

    // ACE_Event_Handler method
    virtual int handle_timeout(const ACE_Time_Value& currentTime, const void* arg);

private:

    // Unimplemented Constructors/Methods

    MgTimedEventHandler();
    MgTimedEventHandler(const MgTimedEventHandler&);
    MgTimedEventHandler& operator=(const MgTimedEventHandler&);

/// Methods

public:

    static MgTimedEventHandler* Create(MgEventTimer::Type type,
        MgEventTimer& timer);

    const MgTimedEvent& GetEvent() const;

protected:

    virtual void HandleEvent(long eventId) = 0;

private:

    void LogException(MgException& e);

/// Data Members

protected:

    MgEventTimer& m_timer;
    bool m_mutexShared;
    MgTimedEvent m_event; // 1 event per timer is currently supported
};

/// Inline Methods

inline const MgTimedEvent& MgTimedEventHandler::GetEvent() const
{
    return m_event;
}

#endif
