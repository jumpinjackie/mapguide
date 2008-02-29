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

#ifndef MG_MAP_SERVER_CONNECTION_STACK_H
#define MG_MAP_SERVER_CONNECTION_STACK_H

#ifdef _WIN32
#undef CreateService

class MG_MAPGUIDE_API MgServerConnection;
class MG_MAPGUIDE_API MgConnectionProperties;

#endif

#include <stack>
#include <list>

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////
/// \brief
/// Represents a FILO stack of map server connections.  Tracks idle time
/// for each connection and removes stale connections.
/// INTERNAL_ONLY:
class MG_MAPGUIDE_API MgServerConnectionStack
{
    DECLARE_CLASSNAME(MgServerConnectionStack)

INTERNAL_API:

    /// \brief
    /// Constructor for connection stack
    ///
    MgServerConnectionStack();

    /// \brief
    /// Destructor for connection stack
    ///
    virtual ~MgServerConnectionStack();

    /// \brief
    /// Pushes server connection to the top of the stack
    ///
    /// \param connection
    /// Connection to add
    ///
    void Push(MgServerConnection* connection);

    /// \brief
    /// Pops a server connection from the top of the stack.  If
    /// a connection is not available, one is created.  Stale connections
    /// are dropped and not reused.
    ///
    /// \return
    /// previously pushed connection
    ///
    MgServerConnection* Pop();

    /// \brief
    /// Add to list of in use connections
    ///
    /// \param connection
    /// Connection to add
    ///
    void InUse(MgServerConnection* connection);

private:

    typedef std::deque<MgServerConnection*> ConnectionQueue;
    typedef std::list<MgServerConnection*> ConnectionList;

    ACE_Recursive_Thread_Mutex m_mutex;
    ConnectionQueue* m_queue;
    ConnectionList* m_inUse;
};
/// \endcond

#endif
