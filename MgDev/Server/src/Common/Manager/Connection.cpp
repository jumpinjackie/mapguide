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
#include "Connection.h"

static ACE_thread_key_t g_threadLocalConnection = 0;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgConnection::MgConnection() :
    m_busy(false),
    m_startTime(0),
    m_lastUsageTime(0),
    m_nReceivedOperations(0),
    m_nProcessedOperations(0),
    m_currOperationStatus(MgConnection::OpUnknown),
    m_currOperationTime(0)
{
    // Initialize MgConnection start time.
    Start();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Copy constructor.
/// </summary>
MgConnection::MgConnection(const MgConnection& connection)
{
    *this = connection;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgConnection::~MgConnection()
{
    // Perform cleanup and write MgConnection info to log.
    End();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Assignment operator.
/// </summary>
MgConnection& MgConnection::operator=(const MgConnection& connection)
{
    if (&connection != this)
    {
        m_busy = connection.m_busy;
        m_clientAgent = connection.m_clientAgent;
        m_clientIp = connection.m_clientIp;
        m_userName = connection.m_userName;
        m_sessionId = connection.m_sessionId;
        m_startTime = connection.m_startTime;
        m_lastUsageTime = connection.m_lastUsageTime;
        m_nReceivedOperations = connection.m_nReceivedOperations;
        m_nProcessedOperations = connection.m_nProcessedOperations;
        m_currOperationStatus = connection.m_currOperationStatus;
        m_currOperationTime = connection.m_currOperationTime;
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Sets the connection information for the current thread.  This
///function uses thread local storage.
///</summary>
void MgConnection::SetCurrentConnection(MgConnection* connection)
{
    if (0 == g_threadLocalConnection)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
        if (0 == g_threadLocalConnection)
        {
            if (ACE_OS::thr_keycreate(&g_threadLocalConnection, NULL) < 0)
            {
                g_threadLocalConnection = 0;
            }
            else
            {
                ACE_OS::thr_setspecific(g_threadLocalConnection, NULL);
            }
        }
    }

    if (0 != g_threadLocalConnection)
    {
        MgConnection* oldInfo = NULL;
        ACE_OS::thr_getspecific(g_threadLocalConnection, (void**) &oldInfo);

        MgConnection* tempConnection = NULL;
        if(connection != NULL)
        {
            tempConnection = new MgConnection(*connection);
        }

        ACE_OS::thr_setspecific(g_threadLocalConnection, tempConnection);

        if (NULL != oldInfo)
        {
            delete oldInfo;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Gets the connection information for the current thread which was
///set previously using SetCurrentConnection.  This
///function uses thread local storage.
///</summary>
MgConnection* MgConnection::GetCurrentConnection()
{
    MgConnection* connection = NULL;

    if (0 != g_threadLocalConnection)
    {
        ACE_OS::thr_getspecific(g_threadLocalConnection, (void**) &connection);
    }

    return connection;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This method is called by constructor when a MgConnection is created.  It
/// initializes the connection start time.
/// </summary>
void MgConnection::Start()
{
    UpdateLastUsageTime();
    m_startTime = m_lastUsageTime;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This method is called by destructor when a connection is disconnected, which
/// makes a call to log information about this MgConnection.
/// </summary>
void MgConnection::End()
{
    // Determine the connection time in milliseconds.
    double connectionTime = GetConnectionTime();

    // Log connection info (to the screen)
    ACE_DEBUG ((LM_DEBUG, "Ended connection.  Ops processed: %d/%d, Connect Time: %f msec\n",
        m_nProcessedOperations, m_nReceivedOperations, connectionTime));
}

void MgConnection::SetBusy()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_busy = true;
}

void MgConnection::ClearBusy()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_busy = false;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Check if this connection has expired.  A connection becomes expired
/// if there has been no activity in the connection for a predetermined
/// amount of time.
/// Returns true if expired,otherwise returns false.
/// </summary>
bool MgConnection::IsExpired(INT32 nIdleTimeout) const
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bExpired = false;

    // Check to see if we are busy
    if(!m_busy)
    {
        ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();
        ACE_Time_Value diffTime = now - m_lastUsageTime;
        double diff = diffTime.sec();
        if (  diff  > nIdleTimeout )
        {
            bExpired = true;
        }
    }

    return bExpired;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the duration of the connection in milliseconds.
/// This method is called when a ending a MgConnection.
/// </summary>
double MgConnection::GetConnectionTime() const
{
    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();
    ACE_Time_Value diffTime = now - m_startTime;
    double connectTime = (diffTime.sec() * 1000) + (diffTime.usec() * 0.001);
    return connectTime;
}

void MgConnection::SetClientAgent(CREFSTRING agent)
{
    m_clientAgent = agent;
}

void MgConnection::SetClientIp(CREFSTRING ip)
{
    // Perform Cross Site Scripting Attack validations.
    // Note that MgIpUtil::ValidateAddress is not used here because
    // an IP look up will affect peformance.
    MgUtil::CheckXss(ip);
    m_clientIp = ip;
}

void MgConnection::SetUserName(CREFSTRING user)
{
    MgUtil::CheckXss(user);
    m_userName = user;
}

void MgConnection::SetSessionId(CREFSTRING sessionId)
{
    MgUtil::CheckXss(sessionId);
    m_sessionId = sessionId;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Set the MgConnection's last usage time stamp with the current time.
/// </summary>
void MgConnection::UpdateLastUsageTime()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_lastUsageTime = ACE_High_Res_Timer::gettimeofday();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Increment the counter which keeps track of the number of operations
/// successfuly processed during a connection.
/// </summary>
void MgConnection::IncrementProcessedOperations()
{
    ++m_nProcessedOperations;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Increment the counter which keeps track of the number of operations
/// received during a connection.
/// </summary>
void MgConnection::IncrementReceivedOperations()
{
    ++m_nReceivedOperations;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the current operation status.
///
void MgConnection::SetCurrentOperationStatus(OperationStatus opStatus)
{
    m_currOperationStatus = opStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the current operation time (in milliseconds).
///
void MgConnection::SetCurrentOperationTime(INT32 opTime)
{
    m_currOperationTime = opTime;
}
