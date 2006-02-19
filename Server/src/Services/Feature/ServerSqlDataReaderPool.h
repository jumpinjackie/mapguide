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

#ifndef _MG_SERVER_SQL_DATA_READER_POOL_H_
#define _MG_SERVER_SQL_DATA_READER_POOL_H_

class MgServerSqlProcessor;

class MgServerSqlDataReaderPool;
template class Ptr<MgServerSqlDataReaderPool>;

class MgServerSqlDataReaderPool : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerSqlDataReaderPool)

public:
    /// <summary>
    /// Get pointer to a process-wide MgServerSqlDataReaderPool.
    /// </summary>
    static MgServerSqlDataReaderPool* GetInstance(void);

    void Add(MgServerSqlProcessor* sqlReader);

    void Remove(MgServerSqlProcessor* sqlReader);

    bool Contains(MgServerSqlProcessor* sqlReader);

protected:

    /// <summary>
    /// Self Destructor
    /// </summary>

    virtual void Dispose();

    /// <summary>
    /// Destructor
    /// </summary>

    virtual ~MgServerSqlDataReaderPool(void);

    MgServerSqlDataReaderPool(void);

private:

    /// Pointer to a process-wide singleton.
    static Ptr<MgServerSqlDataReaderPool> m_drPool;

    Ptr<MgDisposableCollection> m_drCollection;

    ACE_Recursive_Thread_Mutex m_mutex;
};

#endif //_MG_SERVER_FEATURE_READER_IDENTIFIER_POOL_H_
