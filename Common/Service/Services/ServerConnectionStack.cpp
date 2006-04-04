//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "ServerConnectionStack.h"

/// <summary>
/// Constructor for connection stack
/// </summary>
MgServerConnectionStack::MgServerConnectionStack()
{
    m_stack = new ConnectionStack;
    m_inUse = new ConnectionList;
}

MgServerConnectionStack::~MgServerConnectionStack()
{

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


    while (m_stack->size() > 0)
    {
        MgServerConnection* conn = m_stack->top();
        if (NULL != conn)
        {
            m_stack->pop();
            SAFE_RELEASE(conn);
        }
    }
    delete m_stack;
    m_stack = NULL;
}

/// <summary>
/// Pushes server connection to the top of the stack
/// </summary>
/// <param name="connection">
/// Connection to add
/// <param>
void MgServerConnectionStack::Push(MgServerConnection* connection)
{
    m_inUse->remove(connection);
    m_stack->push(connection);
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
    MgServerConnection* conn = NULL;

    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();
    while (conn == NULL && m_stack->size() > 0)
    {
        conn = m_stack->top();
        if (NULL != conn)
        {
            m_stack->pop();
            ACE_Time_Value diffTime = now - *(conn->LastUsed());
            double diff = diffTime.sec();
            if (diff > 60.0 )
            {
                SAFE_RELEASE(conn);
                conn = NULL;
            }
            else
            {
                // Check to see if stream is still alive... It better be!
                Ptr<MgStream> stream = conn->GetStream();
                if (stream != NULL && stream->GetStreamHelper()->IsConnected() == false)
                {
                    stream = NULL;
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
    SAFE_ADDREF(connection);
    m_inUse->push_back(connection);
}
