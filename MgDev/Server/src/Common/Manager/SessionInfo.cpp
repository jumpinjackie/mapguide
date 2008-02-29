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

#include "SessionInfo.h"
#include "LogManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgSessionInfo::MgSessionInfo() :
    m_numOpsFailed(0),
    m_numOpsReceived(0),
    m_totalOpTime(0.0)
{
    m_startTime.SetMicrosecond(0);
    SetAccessedTime(ACE_High_Res_Timer::gettimeofday());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgSessionInfo::~MgSessionInfo()
{
    MG_TRY()

    MG_LOG_SESSION_ENTRY(*this);

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the end time.
///
MgDateTime MgSessionInfo::GetEndTime() const
{
    MgDateTime endTime; // current time

    endTime.SetMicrosecond(0);

    return endTime;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the average operation time in milliseconds.
///
double MgSessionInfo::GetAverageOperationTime() const
{
    INT32 numOpsSucceeded = m_numOpsReceived - m_numOpsFailed;

    if (numOpsSucceeded > 0)
    {
        return (m_totalOpTime / numOpsSucceeded);
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the user.
///
void MgSessionInfo::SetUser(CREFSTRING user)
{
    if (user.empty())
    {
        throw new MgNullArgumentException(L"MgSessionInfo.SetUser",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_user = user;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the accessed time.
///
void MgSessionInfo::SetAccessedTime(const ACE_Time_Value& accessedTime)
{
    m_accessedTime = accessedTime;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the operation information.
///
void MgSessionInfo::SetOperationInfo(const MgConnection& connection)
{
    if (0 == m_numOpsReceived)
    {
        m_client = connection.GetClientAgent();
        m_clientIp = connection.GetClientIp();
    }

    if (MgConnection::OpOk == connection.GetCurrentOperationStatus())
    {
        m_totalOpTime += connection.GetCurrentOperationTime();
    }
    else
    {
        ++m_numOpsFailed;
    }

    ++m_numOpsReceived;
}
