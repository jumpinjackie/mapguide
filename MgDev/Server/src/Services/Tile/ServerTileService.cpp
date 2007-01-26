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

#include "MapGuideCommon.h"
#include "ServerTileService.h"

IMPLEMENT_CREATE_SERVICE(MgServerTileService)

ACE_Recursive_Thread_Mutex MgServerTileService::sm_mutex;
MgServerTileService::MapCache MgServerTileService::sm_mapCache;
INT32 MgServerTileService::sm_mapCacheSize = -1;
bool MgServerTileService::sm_renderOnly = false;

MgServerTileService::MgServerTileService() : MgTileService()
{
    if (sm_mapCacheSize < 0)
    {
        // initialize the tile cache size
        MgConfiguration* pConf = MgConfiguration::GetInstance();

        pConf->GetIntValue(MgConfigProperties::TileServicePropertiesSection,
            MgConfigProperties::TileServicePropertyTiledMapCacheSize,
            sm_mapCacheSize,
            MgConfigProperties::DefaultTileServicePropertyTiledMapCacheSize);

        pConf->GetBoolValue(MgConfigProperties::TileServicePropertiesSection,
            MgConfigProperties::TileServicePropertyRenderOnly,
            sm_renderOnly,
            MgConfigProperties::DefaultTileServicePropertyRenderOnly);
    }

    m_tileCache = new MgTileCache();
}


MgServerTileService::~MgServerTileService()
{
}

MgByteReader* MgServerTileService::GetTile(
    MgResourceIdentifier* mapDefinition,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow,
    INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == mapDefinition || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerTileService.GetTile", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    bool bAllowed = resourceService->HasPermission(mapDefinition, MgResourcePermission::ReadOnly);

    if (!bAllowed)
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

        MgStringCollection arguments;
        arguments.Add(mapDefinition->ToString());

        throw new MgPermissionDeniedException(
            L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    
    ret = m_tileCache->Get(mapDefinition, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);

    if (!ret)
    {

        // Attempt use a cached & serialized MgMap object
        Ptr<MgMemoryStreamHelper> cachedMap;

        STRING mapString = mapDefinition->ToString();

        Ptr<MgMap> map;

        // Protect the serialized MgMap cache with a mutex.  Stream reading is not
        // thread safe so we need to deserialize the map within the mutex to ensure
        // that a Rewind() is not called in the middle of a Deserialize()
        {
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));
            MapCache::const_iterator iter = sm_mapCache.find(mapString);
            if (sm_mapCache.end() != iter)
            {
                cachedMap = SAFE_ADDREF((*iter).second);
            }
            else
            {
                map = new MgMap();
                map->Create(resourceService, mapDefinition, mapString);
                cachedMap = new MgMemoryStreamHelper();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map->Serialize(stream);
                if ((INT32)sm_mapCache.size() >= sm_mapCacheSize)
                {
                    ClearMapCache(L"");
                }
                sm_mapCache[mapString] = SAFE_ADDREF((MgMemoryStreamHelper*)cachedMap);
            }


            if (!map)
            {
                cachedMap->Rewind();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map = new MgMap();
                map->Deserialize(stream);
            }
        }

        double scale = map->GetFiniteDisplayScaleAt(scaleIndex);
        map->SetViewScale(scale);
        ret = GetTile(map, baseMapLayerGroupName, tileColumn, tileRow);
    }

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTile")

    return ret.Detach();
}


MgByteReader* MgServerTileService::GetTile(MgMap* map,
                                           CREFSTRING baseMapLayerGroupName,
                                           INT32 tileColumn,
                                           INT32 tileRow)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || baseMapLayerGroupName.empty())
        throw new MgNullArgumentException(L"MgServerTileService.GetTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
        throw new MgInvalidMapDefinitionException(L"MgServerTileService.GetTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // try getting the tile from the cache
    ret = m_tileCache->Get(map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);

    // if the reader is NULL then the tile wasn't in the cache and we
    // need to generate it
    if (ret == NULL)
    {
        // get a rendering service instance
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);
        Ptr<MgRenderingService> svcRendering = dynamic_cast<MgRenderingService*>(
            serviceMan->RequestService(MgServiceType::RenderingService));

        if (svcRendering != NULL)
        {
            // generate the tile
            ret = svcRendering->RenderTile(map, baseMapLayerGroupName, tileColumn, tileRow);

            // cache the tile
            if (!sm_renderOnly)
            {
                SetTile(ret, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);

                // rewind the reader since setting the tile advances it to the end
                if (ret)
                    ret->Rewind();
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTile")

    return SAFE_ADDREF(ret.p);
}


void MgServerTileService::SetTile(MgByteReader* img,
                                  MgMap* map,
                                  INT32 scaleIndex,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow)
{
    MG_TRY()

    if (NULL == img || NULL == map || baseMapLayerGroupName.empty())
        throw new MgNullArgumentException(L"MgServerTileService.SetTile", __LINE__, __WFILE__, NULL, L"", NULL);

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // cache the tile
    m_tileCache->Set(img, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);

    MG_CATCH_AND_THROW(L"MgServerTileService.SetTile")
}


void MgServerTileService::ClearCache(MgMap* map)
{
    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgServerTileService.ClearCache", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgResourceIdentifier> resourceId = map->GetMapDefinition();
    ClearMapCache(resourceId->ToString());

    m_tileCache->Clear(map);

    MG_CATCH_AND_THROW(L"MgServerTileService.ClearCache")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Any tile cache associated with the specified Map Definition resources 
/// will be cleared.
///
void MgServerTileService::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    MG_TRY()

    if (NULL != resources)
    {
        INT32 numMaps = resources->GetCount();

        for (INT32 i = 0; i < numMaps; ++i)
        {
            Ptr<MgSerializable> serializableObj = resources->GetItem(i);
            MgResourceIdentifier* mapResId = 
                dynamic_cast<MgResourceIdentifier*>(serializableObj.p);

            if (NULL == mapResId)
            {
                throw new MgInvalidCastException(
                    L"MgServerTileService.NotifyResourcesChanged",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            if (mapResId->IsResourceTypeOf(MgResourceType::MapDefinition))
            {
                // clear any cached mgmap objects
                ClearMapCache(mapResId->ToString());

                // clear any tile cache associated with this map
                m_tileCache->Clear(mapResId);
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgServerTileService.NotifyResourcesChanged")
}

void MgServerTileService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}

void MgServerTileService::ClearMapCache(CREFSTRING mapDefinition)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
    MapCache::iterator iter = sm_mapCache.end();
    if (mapDefinition.empty())
    {
        for (iter = sm_mapCache.begin(); iter != sm_mapCache.end(); ++iter)
        {
            SAFE_RELEASE((*iter).second);
            (*iter).second = NULL;
        }
        sm_mapCache.clear();

        STRING message;
        MgResources* resources = NULL;
        MG_TRY()
        resources = MgResources::GetInstance();
        if (NULL != resources)
        {
            message = resources->GetMessage(MgResources::ErrorDescription, L"MgMapCacheCleared", NULL);
            MG_LOG_ERROR_ENTRY(message.c_str());
        }
        MG_CATCH_AND_RELEASE()
    }
    else
    {
        iter = sm_mapCache.find(mapDefinition);
        if (sm_mapCache.end() != iter)
        {
            SAFE_RELEASE((*iter).second);
            (*iter).second = NULL;
            sm_mapCache.erase(iter);
        }
    }
}
