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

#include "MapGuideCommon.h"
#include "ServerConnectionStack.h"

/// <summary>
/// Constructor for connection stack
/// </summary>
MgServerConnectionStack::MgServerConnectionStack()
{
    m_queue = new ConnectionQueue;
    m_inUse = new ConnectionList;
}

MgServerConnectionStack::~MgServerConnectionStack()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    ConnectionList::iterator iter = m_inUse->begin();
    while (iter != m_inUse->end())
    {
        (*iter)->SetStack(NULL);
        SAFE_RELEASE((*iter));
        iter++;
    }
    m_inUse->clear();
    delete m_inUse;
    m_inUse = NULL;


    while (m_queue->size() > 0)
    {
        MgServerConnection* conn = m_queue->front();
        if (NULL != conn)
        {
            m_queue->pop_front();
            SAFE_RELEASE(conn);
        }
    }
    delete m_queue;
    m_queue = NULL;
}

/// <summary>
/// Pushes server connection to the top of the stack
/// </summary>
/// <param name="connection">
/// Connection to add
/// <param>
void MgServerConnectionStack::Push(MgServerConnection* connection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_inUse->remove(connection);
    m_queue->push_front(connection);
}

/// <summary>
/// Pops a server connection from the top of the stack.  If
/// a connection is not available, one is created.  Stale connections
/// are dropped and not reused.
/// </summary>
/// <param name="connProp">
/// Connection to add
/// <param>
MgServerConnection* MgServerConnectionStack::Pop()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerConnection* conn = NULL;

    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();

    // Remove a stale connection from the back of the queue
    if (m_queue->size() > 0)
    {
        conn = m_queue->back();

        if (NULL != conn && conn->IsStale(&now))
        {
            m_queue->pop_back();
            SAFE_RELEASE(conn);
            conn = NULL;
        }
    }


    // Pull a connection from the front of the queue and make sure it's valid.
    conn = NULL;
    while (NULL == conn && m_queue->size() > 0)
    {
        // Grab a connection from the queue
        {
            conn = m_queue->front();
            if (NULL != conn) m_queue->pop_front();
        }

        // And see if it is still valid
        if (NULL != conn)
        {
            if (conn->IsStale(&now))
            {
                SAFE_RELEASE(conn);
                conn = NULL;
            }
            else
            {
                // Check to see if stream is still alive... It better be!
                Ptr<MgStream> stream = conn->GetStream();
                if (stream != NULL)
                {
                    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
                    if (helper->IsConnected() == false)
                    {
                        stream = NULL;
                        SAFE_RELEASE(conn);
                        conn = NULL;
                    }
                }
                else
                {
                    SAFE_RELEASE(conn);
                    conn = NULL;
                }
            }
        }
    }

    if (NULL != conn)
    {
        InUse(conn);
    }

    // Extra reference count from InUse will be eating by external code
    return conn;
}

void MgServerConnectionStack::InUse(MgServerConnection* connection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    SAFE_ADDREF(connection);
    m_inUse->push_back(connection);
}
