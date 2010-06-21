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

#ifndef MG_TRANSFORM_CACHE_H_
#define MG_TRANSFORM_CACHE_H_

#include "MgCSTrans.h"

class TransformCache;
typedef std::map<STRING, TransformCache*> TransformCacheMap;


// Class to cache coordinate systems and transforms during processing.  Many
// layers will use the same coordinate system so this is an effective way to
// reduce significant overhead.
class MG_SERVER_FEATURE_API TransformCache
{
public:
    TransformCache(MgCSTrans* transform, MgCoordinateSystem* coordinateSystem);
    ~TransformCache();
    MgCSTrans* GetTransform();
    MgCoordinateSystem* GetCoordSys();
    void SetMgTransform(MgCoordinateSystemTransform* mgTransform);
    MgCoordinateSystemTransform* GetMgTransform();
    void SetEnvelope(MgEnvelope* extent);
    MgEnvelope* GetEnvelope();

    static TransformCache* GetLayerToMapTransform(TransformCacheMap& cache,
                                                  CREFSTRING featureName,
                                                  MgResourceIdentifier* resId,
                                                  MgCoordinateSystem* dstCs,
                                                  MgCoordinateSystemFactory* csFactory,
                                                  MgFeatureService* svcFeature);

    static void Clear(TransformCacheMap& cacheMap);

private:
    static ACE_Recursive_Thread_Mutex sm_mutex;

    auto_ptr<MgCSTrans> m_xform;
    Ptr<MgCoordinateSystem> m_coordSys;
    Ptr<MgCoordinateSystemTransform> m_transform;
    Ptr<MgEnvelope> m_envelope;
};

#endif
