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

#ifndef MG_MAP_SERVER_CONNECTION_POOL_H
#define MG_MAP_SERVER_CONNECTION_POOL_H


#ifdef _WIN32
#undef CreateService

class MG_MAPGUIDE_API MgServerConnection;
class MG_MAPGUIDE_API MgConnectionProperties;

#endif

class MgServerConnectionEventHandler;

#include <map>
#include "MapGuideCommon.h"
#include "ace/Reactor.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

class MgServerConnectionPool;
template class MG_MAPGUIDE_API Ptr<MgServerConnectionPool>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////
/// \brief
/// Represents a FILO stack of map server connections.  Tracks idle time
/// for each connection and removes stale connections.
/// INTERNAL_ONLY:
class MG_MAPGUIDE_API MgServerConnectionPool : public MgDisposable
{
    DECLARE_CLASSNAME(MgServerConnectionPool)

INTERNAL_API:

    /// \brief
    /// Constructor for connection stack
    ///
    MgServerConnectionPool();

    /// \brief
    /// Destructor for connection stack
    ///
    virtual ~MgServerConnectionPool();

    /// \brief
    /// Return global instance
    ///
    static MgServerConnectionPool* GetInstance();

    /// \brief
    /// Close expired connections
    ///
    static void CloseStaleConnections(ACE_Time_Value* timeValue);

    /// \brief
    /// Close global instance and release all connections
    ///
    static void CloseConnections();

    typedef std::map<wstring, MgServerConnectionStack*> ConnectionPool;

    ConnectionPool pool;

protected:

    void Dispose();

private:

    ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> m_timer;
    long m_id;
    MgServerConnectionEventHandler* m_eventHandler;
    static MgServerConnectionPool* sm_pool;
};
/// \endcond

#endif
