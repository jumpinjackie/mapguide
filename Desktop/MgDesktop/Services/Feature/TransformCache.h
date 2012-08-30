#ifndef DESKTOP_TRANSFORM_CACHE
#define DESKTOP_TRANSFORM_CACHE

#include "MgDesktop.h"
#include "MgCSTrans.h"

class MgdTransformCache;
typedef std::map<STRING, MgdTransformCache*> MgdTransformCacheMap;

// Class to cache coordinate systems and transforms during processing.  Many
// layers will use the same coordinate system so this is an effective way to
// reduce significant overhead.
class MG_DESKTOP_API MgdTransformCache
{
public:
    MgdTransformCache(MgdCSTrans* transform, MgCoordinateSystem* coordinateSystem);
    ~MgdTransformCache();
    MgdCSTrans* GetTransform();
    MgCoordinateSystem* GetCoordSys();
    void SetMgTransform(MgCoordinateSystemTransform* mgTransform);
    MgCoordinateSystemTransform* GetMgTransform();
    void SetEnvelope(MgEnvelope* extent);
    MgEnvelope* GetEnvelope();

    static MgdTransformCache* GetLayerToMapTransform(MgdTransformCacheMap& cache,
                                                  CREFSTRING featureName,
                                                  MgResourceIdentifier* resId,
                                                  MgCoordinateSystem* dstCs,
                                                  MgCoordinateSystemFactory* csFactory,
                                                  MgFeatureService* svcFeature);

    static void Clear(MgdTransformCacheMap& cacheMap);

private:
    static ACE_Recursive_Thread_Mutex sm_MgdMutex;

    auto_ptr<MgdCSTrans> m_xform;
    Ptr<MgCoordinateSystem> m_coordSys;
    Ptr<MgCoordinateSystemTransform> m_transform;
    Ptr<MgEnvelope> m_envelope;
};

#endif