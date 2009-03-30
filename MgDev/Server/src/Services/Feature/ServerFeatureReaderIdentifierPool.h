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

#ifndef _MG_SERVER_FEATURE_READER_IDENTIFIER_POOL_H_
#define _MG_SERVER_FEATURE_READER_IDENTIFIER_POOL_H_

class MgServerFeatureProcessor;

typedef std::map<STRING, MgServerFeatureProcessor*> FeatureProcessorCollection;
typedef std::pair<STRING, MgServerFeatureProcessor*> FeatureProcessorCacheEntry_Pair;

class MgServerFeatureReaderIdentifierPool : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerFeatureReaderIdentifierPool)

public:
    /// <summary>
    /// Get pointer to a process-wide MgServerFeatureReaderIdentifierPool.
    /// </summary>

    static MgServerFeatureReaderIdentifierPool* GetInstance();

    STRING Add(MgServerFeatureProcessor* processor);

    bool Remove(STRING featureReader);

    MgServerFeatureProcessor* GetProcessor(STRING featureReader);
    STRING GetReaderId(MgServerFeatureProcessor* processor);

protected:

    /// <summary>
    /// Self Destructor
    /// </summary>

    virtual void Dispose();

    /// <summary>
    /// Destructor
    /// </summary>

    virtual ~MgServerFeatureReaderIdentifierPool();

    MgServerFeatureReaderIdentifierPool();

private:

    /// Pointer to a process-wide singleton.
    static Ptr<MgServerFeatureReaderIdentifierPool> m_fridPool;

    FeatureProcessorCollection m_fridCollection;

    ACE_Recursive_Thread_Mutex m_mutex;
};

#endif //_MG_SERVER_FEATURE_READER_IDENTIFIER_POOL_H_
