//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MG_GWS_CONNECTION_POOL_H_
#define _MG_GWS_CONNECTION_POOL_H_

#include "GwsQueryEngine.h"
#include "GwsCommonImp.h"

class MgServerFeatureConnection;

typedef GwsIgnoreCase MgGwsIgnoreCase;

typedef std::map<std::wstring,MgServerFeatureConnection*,MgGwsIgnoreCase> MgGwsConnectionMap;
typedef MgGwsConnectionMap::iterator  MgGwsConnectionIter;
typedef MgGwsConnectionMap::value_type MgGwsConnectionMapValue;



class MgGwsConnectionPool : public IGWSConnectionPool
{
public:

    // Creation
    static MgGwsConnectionPool* Create();

    // IGWSDisposable methods
    virtual FdoInt32 AddRef();
    virtual FdoInt32 Release();
    virtual void Dispose();

    // IGWSObject methods
    virtual IGWSObject* GetOwner();
    virtual void SetOwner(IGWSObject* pObj);

    // IGWSConnectionPool implementation
    virtual FdoIConnection* GetConnection(FdoString* name);

    // Native methods

    // add connection to the pool
    void AddConnection(FdoString* name, MgServerFeatureConnection* conn);

    // remove connection from the pool
    void RemoveConnection(FdoString* name);

    MgGwsConnectionMap* GetConnections()    { return &m_connections; }

protected:
    // Construction/destruction
    MgGwsConnectionPool();
    virtual ~MgGwsConnectionPool();

protected:
    int m_iRefCount;
    MgGwsConnectionMap m_connections;

};

#endif
