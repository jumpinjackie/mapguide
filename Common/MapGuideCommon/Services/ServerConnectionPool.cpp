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
#include "ServerConnectionPool.h"
#include "ServerConnectionStack.h"
#include "ServerConnectionEventHandler.h"
 
MgServerConnectionPool* MgServerConnectionPool::sm_pool = NULL;

/// <summary>
/// Constructor for connection stack
/// </summary>
MgServerConnectionPool::MgServerConnectionPool()
{
    // Start up the event timer.
    m_timer.reactor(ACE_Reactor::instance());
    m_timer.activate();

    // Create the event handler and register to fire every 20 seconds
    m_eventHandler = new MgServerConnectionEventHandler();
    ACE_Time_Value interval = ACE_Time_Value(20);
    ACE_Time_Value startTime = ACE_OS::gettimeofday() + interval;
    m_id = m_timer.schedule(m_eventHandler, 0, startTime, interval);
}

MgServerConnectionPool::~MgServerConnectionPool()
{
    m_timer.cancel(m_id);
    m_timer.deactivate();
    m_timer.close();

    // Remove the event handler.
    if (NULL != m_eventHandler)
    {
        ACE_Reactor::instance()->remove_handler(m_eventHandler,
            ACE_Event_Handler::TIMER_MASK | ACE_Event_Handler::DONT_CALL);
    }

    delete m_eventHandler;
    m_eventHandler = NULL;

    ConnectionPool::iterator iter = pool.begin();
    while (iter != pool.end())
    {
        if (NULL != iter->second)
        {
            delete iter->second;
            iter->second = NULL;
        }
        iter++;
    }

    pool.clear();
}

void MgServerConnectionPool::Dispose()
{
    delete this;
}

MgServerConnectionPool* MgServerConnectionPool::GetInstance()
{
    MG_TRY()

    if (MgServerConnectionPool::sm_pool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerConnectionPool::sm_pool == NULL)
        {
            MgServerConnectionPool::sm_pool = new MgServerConnectionPool();
        }
    }

    MG_CATCH_AND_THROW(L"MgServerConnectionPool.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerConnectionPool::sm_pool; 
}

void MgServerConnectionPool::CloseStaleConnections(ACE_Time_Value* timeValue) 
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
    MG_TRY()
    MgServerConnectionPool::ConnectionPool::iterator iter;
    for (iter = MgServerConnectionPool::sm_pool->pool.begin();
         iter != MgServerConnectionPool::sm_pool->pool.end();
         iter++)
    {
        MgServerConnectionStack* stack = iter->second;
        if (NULL != stack)
        {
            stack->CloseStaleConnections(timeValue);
        }
    }
    MG_CATCH_AND_RELEASE()
}

void MgServerConnectionPool::CloseConnections()
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
    MG_TRY()
    if (MgServerConnectionPool::sm_pool != NULL)
    {
            delete MgServerConnectionPool::sm_pool;
            MgServerConnectionPool::sm_pool = NULL;
    }
    MG_CATCH_AND_RELEASE()   
}
