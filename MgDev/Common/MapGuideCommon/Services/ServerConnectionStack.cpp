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

#include "MapGuideCommon.h"
#include "ServerConnectionStack.h"

/// <summary>
/// Constructor for connection stack
/// </summary>
MgServerConnectionStack::MgServerConnectionStack(INT32 port)
{
    m_queue = new ConnectionQueue;
    m_inUse = new ConnectionList;

    // Query configuration document to determine which port type we are
    INT32 adminPort = 0;
    INT32 clientPort = 0;
    INT32 sitePort = 0;
    INT32 maxConnections = 1;
    MgConfiguration* config = MgConfiguration::GetInstance();
    config->GetIntValue(MgConfigProperties::AdministrativeConnectionPropertiesSection,
        MgConfigProperties::AdministrativeConnectionPropertyPort, adminPort,
        MgConfigProperties::DefaultAdministrativeConnectionPropertyPort);
   config->GetIntValue(MgConfigProperties::ClientConnectionPropertiesSection,
        MgConfigProperties::ClientConnectionPropertyPort, clientPort,
        MgConfigProperties::DefaultClientConnectionPropertyPort);
    config->GetIntValue(MgConfigProperties::SiteConnectionPropertiesSection,
        MgConfigProperties::SiteConnectionPropertyPort, sitePort,
        MgConfigProperties::DefaultSiteConnectionPropertyPort);

    // Note: The comment below only applies if using the ACE WFMO reactor on Windows.
    //       The ACE config.h file has been updated to use the ACE SELECT reactor on
    //       Windows which is the default reactor used on Linux.
    //       ie: #define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL
    //
    // Assign an appropriate number of connections for each connection type.  On Windows
    // we are limited to 62 connections and will typically have one mapagent and one
    // API process running.  Limiting the number of connections to 20 per process should give
    // adequate headroom to handle CLOSE_WAIT states.  Twelve actively processing client connections
    // should easily saturate a quad core machine depending on load.
    if (port == adminPort)
    {
        // Pull max connections from admin section.  Default to 2 admin connections if not present.
        config->GetIntValue(MgConfigProperties::AdministrativeConnectionPropertiesSection,
            MgConfigProperties::AdministrativeConnectionPropertyMaxConnections, maxConnections, 2);
    }
    else if (port == clientPort)
    {
        // Pull max connections from client section.  Default to 12 client connections if not present.
        config->GetIntValue(MgConfigProperties::ClientConnectionPropertiesSection,
            MgConfigProperties::ClientConnectionPropertyMaxConnections, maxConnections, 12);
    }
    else if (port == sitePort)
    {
        // Pull max connections from site section.  Default to 6 site connections if not present.
        config->GetIntValue(MgConfigProperties::SiteConnectionPropertiesSection,
            MgConfigProperties::SiteConnectionPropertyMaxConnections, maxConnections, 6);
    }

    if (maxConnections > 1)
    {
        m_activeConnections.release(maxConnections-1);
    }
}

MgServerConnectionStack::~MgServerConnectionStack()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    ConnectionList::iterator iter = m_inUse->begin();
    while (iter != m_inUse->end())
    {
        (*iter)->SetStack(NULL);
        SAFE_RELEASE((*iter));
        ++iter;
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
    m_activeConnections.release();
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
    // Wait up to 30 seconds for a connection to free up
    ACE_Time_Value delay(30);
    ACE_Time_Value future = ACE_OS::gettimeofday() + delay;
    int acquired = m_activeConnections.acquire(future);
    if (acquired == -1)
    {
        throw new MgConnectionFailedException(L"MgServerConnectionStack.Pop",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerConnection* conn = NULL;

    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();

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

void MgServerConnectionStack::CloseStaleConnections(ACE_Time_Value* timeValue)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    // Remove a stale connection from the back of the queue
    // Work from the back of the queue until the connections are not stale
    while (m_queue->size() > 0)
    {
        MgServerConnection* conn = m_queue->back();

        if (NULL != conn && conn->IsStale(timeValue))
        {
            m_queue->pop_back();
            SAFE_RELEASE(conn);
            conn = NULL;
        }
        else
        {
            break;
        }
    }
}
