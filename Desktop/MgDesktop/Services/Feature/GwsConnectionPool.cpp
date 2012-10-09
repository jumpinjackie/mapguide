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

#include "GwsConnectionPool.h"

MgdGwsConnectionPool* MgdGwsConnectionPool::Create()
{
    MgdGwsConnectionPool* pool = new MgdGwsConnectionPool();
    pool->AddRef();
    return pool;
}

MgdGwsConnectionPool::MgdGwsConnectionPool ()
{
    m_iRefCount = 0;
}

MgdGwsConnectionPool::~MgdGwsConnectionPool ()
{
    if (!m_connections.empty())
    {
        for (MgGwsConnectionIter iter = m_connections.begin();
             iter != m_connections.end();
             iter++)
        {
            MgdFeatureConnection * conn = (*iter).second;
            if(conn)
            {
                conn->Release ();
            }
        }
        m_connections.clear();
    }
}

FdoInt32 MgdGwsConnectionPool::AddRef ()
{
    return m_iRefCount ++;
}
FdoInt32 MgdGwsConnectionPool::Release ()
{
    m_iRefCount --;
    assert (m_iRefCount >= 0);
    if (0 != m_iRefCount)
        return m_iRefCount;

    Dispose ();
    return 0;
}
void MgdGwsConnectionPool::Dispose ()
{
    delete this;
}

IGWSObject * MgdGwsConnectionPool::GetOwner ()
{
    return NULL;
}

void MgdGwsConnectionPool::SetOwner (IGWSObject * pObj)
{
}

// Gws Query engine expects that this method throws exception
// in case when connection is not found in the pool
FdoIConnection * MgdGwsConnectionPool::GetConnection (FdoString * name)
{
    FdoIConnection* fdoConn = NULL;

    if (name == NULL || * name == 0)
    {
        throw IGWSException::Create (eGwsDataSourceNotFound);
    }

    MgGwsConnectionIter iter = m_connections.find (name);
    if (iter == m_connections.end ())
    {
        throw IGWSException::Create (eGwsDataSourceNotFound,
                                     NULL,
                                     L"FeatureSourceName",
                                     name,
                                     NULL);

    }

    MgdFeatureConnection * conn = (*iter).second;
    if(conn)
    {
        fdoConn = conn->GetConnection();
    }
    return fdoConn;
}

void MgdGwsConnectionPool::AddConnection (
    FdoString       * name,
    MgdFeatureConnection* conn
)
{
    if (name == NULL || * name == 0 || conn == NULL)
        throw IGWSException::Create (eGwsNullPointer);

    MgGwsConnectionIter iter = m_connections.find (name);
    if (iter == m_connections.end ())
    {
        conn->AddRef();
        m_connections.insert (MgGwsConnectionMapValue (name, conn));
    }
    else
    {
        throw IGWSException::Create (eGwsDataSourceAlreadyExists);
    }
}


void MgdGwsConnectionPool::RemoveConnection (FdoString * name)
{
    if (name == NULL || * name == 0)
        throw IGWSException::Create (eGwsNullPointer);

    MgGwsConnectionIter iter = m_connections.find (name);
    if (iter == m_connections.end ()) {
        throw IGWSException::Create (eGwsSessionNotFound);
    }

    MgdFeatureConnection * conn = (*iter).second;
    if(conn)
    {
        conn->Release ();
    }
    m_connections.erase (iter);
}
