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

#include "GwsConnectionPool.h"

MgGwsConnectionPool* MgGwsConnectionPool::Create()
{
    MgGwsConnectionPool* pool = new MgGwsConnectionPool();
    pool->AddRef();
    return pool;
}

MgGwsConnectionPool::MgGwsConnectionPool ()
{
    m_iRefCount = 0;
}

MgGwsConnectionPool::~MgGwsConnectionPool ()
{
    if (!m_connections.empty())
    {
        for (MgGwsConnectionIter iter = m_connections.begin();
             iter != m_connections.end();
             iter++)
        {
            MgServerFeatureConnection * conn = (*iter).second;
            if(conn)
            {
                conn->Release ();
            }
        }
        m_connections.clear();
    }
}

FdoInt32 MgGwsConnectionPool::AddRef ()
{
    return m_iRefCount ++;
}
FdoInt32 MgGwsConnectionPool::Release ()
{
    m_iRefCount --;
    assert (m_iRefCount >= 0);
    if (m_iRefCount == 0)
        Dispose ();
    return m_iRefCount;
}
void MgGwsConnectionPool::Dispose ()
{
    delete this;
}

IGWSObject * MgGwsConnectionPool::GetOwner ()
{
    return NULL;
}

void MgGwsConnectionPool::SetOwner (IGWSObject * pObj)
{
}

// Gws Query engine expects that this method throws exception
// in case when connection is not found in the pool
FdoIConnection * MgGwsConnectionPool::GetConnection (FdoString * name)
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

    MgServerFeatureConnection * conn = (*iter).second;
    if(conn)
    {
        fdoConn = conn->GetConnection();
    }
    return fdoConn;
}

void MgGwsConnectionPool::AddConnection (
    FdoString       * name,
    MgServerFeatureConnection* conn
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


void MgGwsConnectionPool::RemoveConnection (FdoString * name)
{
    if (name == NULL || * name == 0)
        throw IGWSException::Create (eGwsNullPointer);

    MgGwsConnectionIter iter = m_connections.find (name);
    if (iter == m_connections.end ()) {
        throw IGWSException::Create (eGwsSessionNotFound);
    }

    MgServerFeatureConnection * conn = (*iter).second;
    if(conn)
    {
        conn->Release ();
    }
    m_connections.erase (iter);
}


