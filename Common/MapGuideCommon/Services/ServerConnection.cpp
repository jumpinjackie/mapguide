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

#include "MapGuideCommon.h"
#include "ServerConnectionImp.h"
#include "ServerConnectionStack.h"
#include "ServerConnectionPool.h"

#ifdef WIN32
#undef CreateService
#endif

#ifndef WIN32
#include <dlfcn.h>
#endif

static Ptr<MgServerConnectionPool> g_connectionPool = new MgServerConnectionPool();
ACE_Recursive_Thread_Mutex MgServerConnection::sm_mutex;

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new connection.
/// INTERNAL ONLY
/// </summary>
//Added default constructor because of a compiler error on Linux
MgServerConnection::MgServerConnection()
{
    m_serverConnImp = new MgServerConnectionImp();
    m_stream = NULL;
    m_stack = NULL;
    m_lastUsed = new ACE_Time_Value;
    m_isOpen = false;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the connection.
/// INTERNAL ONLY
/// </summary>
MgServerConnection::~MgServerConnection()
{
    assert( 0 == this->GetRefCount() );

    if (NULL != m_lastUsed)
        delete m_lastUsed;

    if (NULL != m_serverConnImp)
        delete m_serverConnImp;

    m_lastUsed = NULL;
    m_serverConnImp = NULL;
    m_stack = NULL;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Establishes a connection to a Mg server
/// INTERNAL ONLY
/// </summary>
///
/// <param name="userInformation">
/// User information to authenticate against
/// </param>
/// <param name="target">
/// Target server.  NULL or "" indicates local server
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// AuthorizationFailed
/// TargetNotFound
/// ConnectionCanNotBeEstablished
void MgServerConnection::Open(MgUserInformation* userInformation, MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
    bool connected = m_serverConnImp->Connect((MgUtil::WideCharToMultiByte(m_connProp->GetTarget())).c_str(), m_connProp->GetPort());

    // Could not connect
    if (!connected)
        throw new MgConnectionFailedException(L"MgServerConnection.Open", __LINE__, __WFILE__, NULL, L"", NULL);

    m_isOpen = true;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Return whether or not the connection is open
/// </summary>
/// <returns>
/// true if the connection is open
/// EXCEPTIONS:
/// None
 bool MgServerConnection::IsOpen()
{
    return m_isOpen;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Disconnects from the Mg server.  Close can be called multiple
/// times without generating an exception.
/// </summary>
///
/// <returns>
/// None
/// </returns>
/// EXCEPTIONS:
/// None
void MgServerConnection::Close()
{
    m_serverConnImp->Disconnect();
    m_isOpen = false;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Overloaded Release for stack management
/// </summary>
INT32 MgServerConnection::Release()
{
    INT32 refCnt = MgDisposable::Release();
    if (refCnt == 1)
    {
        *m_lastUsed = ACE_High_Res_Timer::gettimeofday();
        if (m_stack != NULL)
        {
            m_stack->Push(this);
        }
    }
    return refCnt;
}

/// <summary>
/// Self destructor
/// </summary>
/// <returns> Nothing
/// </returns>
void MgServerConnection::Dispose()
{
        delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgServerConnection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Internal method used for getting stream helper
/// which will then be used in serialize/deserialize
/// </summary>
/// <returns>
/// MgStreamHelper which has method to write or read data
/// over the wire
/// </returns>
MgStreamHelper* MgServerConnection::GetMgStreamHelper()
{
    Ptr<MgStreamHelper> stream;
    ACE_HANDLE handle = m_serverConnImp->GetHandle();
    if (handle != 0)
    {
        stream = new MgAceStreamHelper(handle);
    }
    return SAFE_ADDREF((MgStreamHelper*)stream);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Internal method used for getting stream helper
/// which will then be used in serialize/deserialize
/// </summary>
/// <returns>
/// MgStreamHelper which has method to write or read data
/// over the wire
/// </returns>
MgStream* MgServerConnection::GetStream()
{
    if (m_stream == NULL)
    {
        Ptr<MgStreamHelper> helper = GetMgStreamHelper();
        m_stream = new MgStream(helper);
        m_stream->SetConnection(this);
    }

    return SAFE_ADDREF((MgStream*)m_stream);
}


MgConnectionProperties* MgServerConnection::GetConnectionProperties()
{
    return SAFE_ADDREF((MgConnectionProperties*)m_connProp);
}

void MgServerConnection::SetStack(MgServerConnectionStack* stack)
{
    if (NULL != stack)
    {
        stack->InUse(this);
    }
    m_stack = stack;
}

ACE_Time_Value* MgServerConnection::LastUsed()
{
    return m_lastUsed;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Establishes a connection to a Mg server
/// </summary>
///
/// <param name="userInformation">
/// User information to authenticate against
/// </param>
/// <param name="target">
/// Target server.  NULL or "" indicates local server
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// AuthorizationFailed
/// TargetNotFound
MgServerConnection* MgServerConnection::Acquire(MgUserInformation* userInformation,
                                                MgConnectionProperties* connProp)
{
    CHECKNULL((MgUserInformation*)userInformation, L"MgServerConnection.Acquire");
    CHECKNULL((MgConnectionProperties*)connProp, L"MgServerConnection.Acquire");

    Ptr<MgServerConnection> msc;
    MgServerConnectionStack* stack = NULL;

    // Perform connection pooling.
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

        wstring hash = connProp->Hash();
        MgServerConnectionPool::ConnectionPool::iterator iter = g_connectionPool->pool.find(hash);

        if (iter != g_connectionPool->pool.end())
        {
            stack = iter->second;
        }

        if (stack == NULL)
        {
            stack = new MgServerConnectionStack();
            (g_connectionPool->pool)[hash] = stack;
        }
    }

    if (NULL != stack)
    {
        // Try to get a previously used connection
        msc = stack->Pop();
    }

    if (msc == NULL)
    {
        // Create new connection
        msc = new MgServerConnection();

        // Open throws TargetNotFound
        if ( msc != NULL )
        {
            msc->Open(userInformation, connProp);

            if (!msc->IsOpen())
            {
                throw new MgConnectionNotOpenException(L"MgServerConnection.Acquire", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            msc->SetStack(stack);
        }
    }

    return msc.Detach();
}

