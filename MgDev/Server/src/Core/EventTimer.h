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

#ifndef MGEVENTTIMER_H_
#define MGEVENTTIMER_H_

class MgTimedEventHandler;

const int MG_MAX_EVENT_TIMERS = 9;

class MgEventTimer
{
/// Enumerations

public:

    enum Type
    {
        ServiceRegistration                 = 0,
        SessionTimeout                      = 1,
        ConnectionTimeout                   = 2,
        DataConnectionTimeout               = 3,
        RepositoryCheckpoint                = 4,
        ResourceChange                      = 5,
        FeatureServiceCacheTimeLimit        = 6,
        DataTransactionTimeout              = 7,
        PerformanceLogging                  = 8,
    };

/// Constructors/Destructor

public:

    MgEventTimer();
    virtual ~MgEventTimer();

private:

    // Unimplemented Constructors/Methods

    MgEventTimer(const MgEventTimer&);
    MgEventTimer& operator=(const MgEventTimer&);

/// Methods

public:

    MgTimedEventHandler& GetEventHandler() const;
    INT32 GetEventTimeout() const;

    INT32 GetInterval() const;
    void SetInterval(INT32 interval);

    void Initialize(MgEventTimer::Type type);
    void Terminate();

    void Schedule(const ACE_Time_Value& startTime);
    void Cancel();

    void Activate(const ACE_Time_Value& startTime);
    void Deactivate();

/// Data Members

private:

    long m_id;
    ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> m_timer;
    INT32 m_interval;
    auto_ptr<MgTimedEventHandler> m_eventHandler;
};

/// Inline Methods

inline INT32 MgEventTimer::GetInterval() const
{
    return m_interval;
}

#endif
