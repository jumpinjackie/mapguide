#include "TransformCache.h"

ACE_Recursive_Thread_Mutex TransformCache::sm_mutex;


TransformCache::TransformCache(MgCSTrans* transform, MgCoordinateSystem* coordinateSystem)
{
    m_xform.reset(transform);
    m_coordSys = SAFE_ADDREF(coordinateSystem);
}


TransformCache::~TransformCache()
{
}


MgCSTrans* TransformCache::GetTransform()
{
    return m_xform.get();
}


MgCoordinateSystem* TransformCache::GetCoordSys()
{
    return SAFE_ADDREF(m_coordSys.p);
}


void TransformCache::SetMgTransform(MgCoordinateSystemTransform* mgTransform)
{
    m_transform = SAFE_ADDREF(mgTransform);
}


MgCoordinateSystemTransform* TransformCache::GetMgTransform()
{
    return SAFE_ADDREF(m_transform.p);
}


void TransformCache::SetEnvelope(MgEnvelope* envelope)
{
    m_envelope = SAFE_ADDREF(envelope);
}


MgEnvelope* TransformCache::GetEnvelope()
{
    return SAFE_ADDREF(m_envelope.p);
}


TransformCache* TransformCache::GetLayerToMapTransform(TransformCacheMap& cache,
                                                       CREFSTRING featureName,
                                                       MgResourceIdentifier* resId,
                                                       MgCoordinateSystem* dstCs,
                                                       MgCoordinateSystemFactory* csFactory,
                                                       MgFeatureService* svcFeature)
{
    // prevent separate threads from simultaneously creating coordinate systems
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    TransformCache* item = NULL;

    // Now get the coordinate system of the layer data.
    // Feature Service caches these so we only take the performance hit on
    // the 1st one that is not cached.

    // Parse the feature name for the schema and class
    STRING schemaName, className;
    MgUtil::ParseQualifiedClassName(featureName, schemaName, className);

    STRING spatialContextAssociation = L"";

    // Get the class definition so that we can find the spatial context association
    Ptr<MgClassDefinition> classDef = svcFeature->GetClassDefinition(resId, schemaName, className, false);
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

    // Find the spatial context for the geometric or raster property. Use the first one if there are many defined.
    for(int index=0;index<propDefCol->GetCount();index++)
    {
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
        if (propDef->GetPropertyType () == MgFeaturePropertyType::GeometricProperty)
        {
            // We found the geometric property
            MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(propDef.p);
            spatialContextAssociation = geomProp->GetSpatialContextAssociation();
            break;
        }
        else if(propDef->GetPropertyType () == MgFeaturePropertyType::RasterProperty)
        {
            // We found the raster property
            MgRasterPropertyDefinition* rasterProp = static_cast<MgRasterPropertyDefinition*>(propDef.p);
            spatialContextAssociation = rasterProp->GetSpatialContextAssociation();
            break;
        }

    }

    // We want all of the spatial contexts
    Ptr<MgSpatialContextReader> csrdr = svcFeature->GetSpatialContexts(resId, false);

    // This is the strategy we use for picking the spatial context
    // Find the 1st spatial context that satisfies one of the following: (Processed in order)
    // 1) Matches the association spatial context name
    // 2) The 1st spatial context returned
    // 3) FAIL - none of the above could be satisfied

    Ptr<MgCoordinateSystem> srcCs;

    if (dstCs)
    {
        STRING srcwkt = L"";
        STRING csrName = L"";
        bool bHaveFirstSpatialContext = false;

        while(csrdr.p != NULL && csrdr->ReadNext())
        {
            csrName = csrdr->GetName();
            if((!spatialContextAssociation.empty()) && (csrName == spatialContextAssociation))
            {
                // Match found for the association)
                srcwkt = csrdr->GetCoordinateSystemWkt();
                break;
            }
            else if(!bHaveFirstSpatialContext)
            {
                // This is the 1st spatial context returned
                // This will be overwritten if we find the association
                srcwkt = csrdr->GetCoordinateSystemWkt();
                bHaveFirstSpatialContext = true;
            }
        }
        if(csrdr.p != NULL)
            csrdr->Close();

        // Create coordinate system transformer
        if (!srcwkt.empty())
        {
            TransformCacheMap::const_iterator iter = cache.find(srcwkt);
            if (cache.end() != iter) item = (*iter).second;
            if (NULL == item)
            {
                Ptr<MgCoordinateSystem> srcCs = csFactory->Create(srcwkt);
                if (srcCs.p)
                {
                    item = new TransformCache(new MgCSTrans(srcCs, dstCs), srcCs);
                    cache[srcwkt] = item;

                    // Set the coordinate system transform
                    Ptr<MgCoordinateSystemTransform> trans = csFactory->GetTransform(dstCs, srcCs);
                    trans->IgnoreDatumShiftWarning(true);
                    trans->IgnoreOutsideDomainWarning(true);

                    item->SetMgTransform(trans);
                }
            }
        }
    }
    else
    {
        // No coordinate system!!!
        // We fail here and do not use a default
    }

    return item;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the specified Transform Cache map.
///
void TransformCache::Clear(TransformCacheMap& cacheMap)
{
    for (TransformCacheMap::iterator i = cacheMap.begin();
        i != cacheMap.end(); ++i)
    {
        delete (*i).second;
        (*i).second = NULL;
    }

    cacheMap.clear();
}
