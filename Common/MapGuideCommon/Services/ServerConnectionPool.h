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

#ifndef MG_MAP_SERVER_CONNECTION_POOL_H
#define MG_MAP_SERVER_CONNECTION_POOL_H


#ifdef _WIN32
#undef CreateService

class MG_MAPGUIDE_API MgServerConnection;
class MG_MAPGUIDE_API MgConnectionProperties;

#endif

#include <map>

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

    typedef std::map<wstring, MgServerConnectionStack*> ConnectionPool;

    ConnectionPool pool;

protected:

    void Dispose();
};
/// \endcond

#endif
