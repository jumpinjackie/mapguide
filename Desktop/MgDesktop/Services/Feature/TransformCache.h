#ifndef DESKTOP_TRANSFORM_CACHE
#define DESKTOP_TRANSFORM_CACHE

#include "MgDesktop.h"
#include "MgCSTrans.h"

class TransformCache;
typedef std::map<STRING, TransformCache*> TransformCacheMap;

// Class to cache coordinate systems and transforms during processing.  Many
// layers will use the same coordinate system so this is an effective way to
// reduce significant overhead.
class MG_DESKTOP_API TransformCache
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