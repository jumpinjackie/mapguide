//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGCONNECTION_H_
#define MGCONNECTION_H_

#include "ace/ACE.h"
#include "ace/OS.h"
#include "ace/High_Res_Timer.h"
#include "ace/SOCK_Stream.h"
#include "ace/Unbounded_Set.h"

#include "ServerManagerDllExport.h"
#include "MapGuideCommon.h"

/// This class is responsible for managing state information for a connection.
///
class MG_SERVER_MANAGER_API MgConnection
{
    DECLARE_CLASSNAME(MgConnection)

/// Enumerations

public:

    enum OperationStatus
    {
        OpFailed    = -1,
        OpOk        =  0,
        OpUnknown   =  1
    };

/// Constructors/Destructor

public:

    MgConnection();
    ~MgConnection();

/// Methods

public:

    static void SetCurrentConnection(MgConnection* connection);
    static MgConnection* GetCurrentConnection();

    void SetBusy();
    void ClearBusy();

    bool IsExpired(INT32 nIdleTimeout) const;

    void SetClientAgent(CREFSTRING agent);
    void SetClientIp(CREFSTRING ip);
    void SetUserName(CREFSTRING user);
    void SetSessionId(CREFSTRING sessionId);
    void UpdateLastUsageTime();
    void IncrementProcessedOperations();
    void IncrementReceivedOperations();
    void SetCurrentOperationStatus(OperationStatus opStatus);
    void SetCurrentOperationTime(INT32 opTime);

    CREFSTRING GetClientAgent() const;
    CREFSTRING GetClientIp() const;
    CREFSTRING GetUserName() const;
    CREFSTRING GetSessionId() const;
    const ACE_Time_Value& GetLastUsageTime() const;
    const ACE_Time_Value& GetStartTime() const;
    INT32 GetProcessedOperations() const;
    INT32 GetReceivedOperations() const;
    OperationStatus GetCurrentOperationStatus() const;
    INT32 GetCurrentOperationTime() const;
    double GetConnectionTime() const;

private:

    void Start();
    void End();

/// Data Members

private:

    mutable ACE_Recursive_Thread_Mutex m_mutex;
    bool m_busy;

    STRING m_clientAgent;
    STRING m_clientIp;

    STRING m_userName;
    STRING m_sessionId;

    ACE_Time_Value m_startTime;
    ACE_Time_Value m_lastUsageTime;

    INT32 m_nReceivedOperations;
    INT32 m_nProcessedOperations;

    OperationStatus m_currOperationStatus;
    INT32 m_currOperationTime;
};

/// Inline Methods

inline CREFSTRING MgConnection::GetClientAgent() const
{
    return m_clientAgent;
}

inline CREFSTRING MgConnection::GetClientIp() const
{
    return m_clientIp;
}

inline CREFSTRING MgConnection::GetUserName() const
{
    return m_userName;
}

inline CREFSTRING MgConnection::GetSessionId() const
{
    return m_sessionId;
}

inline const ACE_Time_Value& MgConnection::GetStartTime() const
{
    return m_startTime;
}

inline const ACE_Time_Value& MgConnection::GetLastUsageTime() const
{
    return m_lastUsageTime;
}

inline INT32 MgConnection::GetProcessedOperations() const
{
    return m_nProcessedOperations;
}

inline INT32 MgConnection::GetReceivedOperations() const
{
    return m_nReceivedOperations;
}

inline MgConnection::OperationStatus MgConnection::GetCurrentOperationStatus() const
{
    return m_currOperationStatus;
}

inline INT32 MgConnection::GetCurrentOperationTime() const
{
    return m_currOperationTime;
}

#endif
