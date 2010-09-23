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

#ifndef MGSESSIONINFO_H_
#define MGSESSIONINFO_H_

#include "ServerManager.h"

class MgConnection;

class MG_SERVER_MANAGER_API MgSessionInfo
{
    DECLARE_CLASSNAME(MgSessionInfo)

/// Constructors/Destructor

public:

    MgSessionInfo();
    virtual ~MgSessionInfo();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSessionInfo(const MgSessionInfo&);
    MgSessionInfo& operator=(const MgSessionInfo&);

/// Methods

public:

    STRING GetUser() const;
    STRING GetClient() const;
    STRING GetClientIp() const;
    const MgDateTime& GetStartTime() const;
    MgDateTime GetEndTime() const;
    const ACE_Time_Value& GetAccessedTime() const;
    INT32 GetNumberOfOperationsFailed() const;
    INT32 GetNumberOfOperationsReceived() const;
    double GetTotalOperationTime() const;
    double GetAverageOperationTime() const;

    void SetUser(CREFSTRING user);
    void SetAccessedTime(const ACE_Time_Value& accessedTime);
    void SetOperationInfo(const MgConnection& connection);

/// Data Members

private:

    STRING m_user;
    STRING m_client;
    STRING m_clientIp;
    MgDateTime m_startTime;
    ACE_Time_Value m_accessedTime;
    INT32 m_numOpsFailed;
    INT32 m_numOpsReceived;
    double m_totalOpTime;
};

/// Inline Methods

inline STRING MgSessionInfo::GetUser() const
{
    return m_user;
}

inline STRING MgSessionInfo::GetClient() const
{
    return m_client;
}

inline STRING MgSessionInfo::GetClientIp() const
{
    return m_clientIp;
}

inline const MgDateTime& MgSessionInfo::GetStartTime() const
{
    return m_startTime;
}

inline const ACE_Time_Value& MgSessionInfo::GetAccessedTime() const
{
    return m_accessedTime;
}

inline INT32 MgSessionInfo::GetNumberOfOperationsFailed() const
{
    return m_numOpsFailed;
}

inline INT32 MgSessionInfo::GetNumberOfOperationsReceived() const
{
    return m_numOpsReceived;
}

inline double MgSessionInfo::GetTotalOperationTime() const
{
    return m_totalOpTime;
}

#endif
