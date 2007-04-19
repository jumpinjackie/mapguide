//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "TransformCache.h"


TransformCache::TransformCache(MgCSTrans* transform, MgCoordinateSystem* coordinateSystem)
: m_xform(transform)
{
    m_coordSys = SAFE_ADDREF(coordinateSystem);
}


TransformCache::~TransformCache()
{
    delete m_xform;
    m_xform = NULL;
}


MgCSTrans* TransformCache::GetTransform()
{
    return m_xform;
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
    TransformCache* item = NULL;

    // Now get the coordinate system of the layer data.
    // Feature Service caches these so we only take the performance hit on
    // the 1st one that is not cached.

    // Parse the feature name for the schema and class
    STRING::size_type nDelimiter = featureName.find(L":");
    STRING schemaName;
    STRING className;

    if(STRING::npos == nDelimiter)
    {
        schemaName = L"";
        className = featureName;
    }
    else
    {
        schemaName = featureName.substr(0, nDelimiter);
        className = featureName.substr(nDelimiter + 1);
    }

    STRING spatialContextAssociation = L"";

    // Get the class definition so that we can find the spatial context association
    Ptr<MgClassDefinition> classDef = svcFeature->GetClassDefinition(resId, schemaName, className);
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

    // Find the spatial context for the geometric property. Use the first one if there are many defined.
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
    }

    // We want the active the spatial contexts
    Ptr<MgSpatialContextReader> csrdr = svcFeature->GetSpatialContexts(resId, true);

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

        while(csrdr->ReadNext())
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
