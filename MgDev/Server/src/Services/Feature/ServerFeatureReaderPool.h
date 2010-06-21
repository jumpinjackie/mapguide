//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MG_SERVER_FEATURE_READER_POOL_H_
#define _MG_SERVER_FEATURE_READER_POOL_H_

class MgFeatureReader;

typedef std::map<STRING, MgFeatureReader*> FeatureReaderCollection;
typedef std::pair<STRING, MgFeatureReader*> FeatureReaderCacheEntry_Pair;

class MgServerFeatureReaderPool : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerFeatureReaderPool)

public:
    /// <summary>
    /// Get pointer to a process-wide MgServerFeatureReaderPool.
    /// </summary>

    static MgServerFeatureReaderPool* GetInstance();

    STRING Add(MgFeatureReader* featureReader);

    bool Remove(STRING featureReader);

    MgFeatureReader* GetReader(STRING featureReader);
    STRING GetReaderId(MgFeatureReader* featureReader);

protected:

    /// <summary>
    /// Self Destructor
    /// </summary>

    virtual void Dispose();

    /// <summary>
    /// Destructor
    /// </summary>

    virtual ~MgServerFeatureReaderPool();

    MgServerFeatureReaderPool();

private:

    /// Pointer to a process-wide singleton.
    static Ptr<MgServerFeatureReaderPool> m_frPool;

    FeatureReaderCollection m_frCollection;

    ACE_Recursive_Thread_Mutex m_mutex;
};

#endif //_MG_SERVER_FEATURE_READER_POOL_H_
