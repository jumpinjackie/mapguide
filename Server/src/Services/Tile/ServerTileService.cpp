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

#include "AceCommon.h"
#include "ServerTileService.h"


MgServerTileService::MgServerTileService(MgConnectionProperties* connection) : MgTileService(connection)
{
    // store references to the various services we use
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_tileCache = new MgTileCache();
}


MgServerTileService::MgServerTileService() : MgTileService(NULL)
{
}


MgServerTileService::~MgServerTileService()
{
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
            SetTile(ret, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);

            // rewind the reader since setting the tile advances it to the end
            if (ret)
                ret->Rewind();
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

    m_tileCache->Clear(map);

    MG_CATCH_AND_THROW(L"MgServerTileService.ClearCache")
}


void MgServerTileService::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    if (NULL == resources || resources->GetCount() <= 0)
    {
        return;
    }

    // get any parent map definitions associated with these resources
    Ptr<MgSerializableCollection> parentMaps = NULL; // m_svcResource->EnumerateParentMapDefinitions(resources);

    if (parentMaps != NULL)
    {
        // iterate over the maps
        INT32 numMaps = parentMaps->GetCount();
        for (INT32 i=0; i<numMaps; ++i)
        {
            Ptr<MgResourceIdentifier> mapResId = dynamic_cast<MgResourceIdentifier*>(parentMaps->GetItem(i));
            if (mapResId != NULL)
            {
                if (mapResId->IsResourceTypeOf(MgResourceType::MapDefinition))
                {
                    // clear any tile cache associated with this map
                    m_tileCache->Clear(mapResId);
                }
                else if (mapResId->IsResourceTypeOf(MgResourceType::Map))
                {
                    // TODO?
                }
            }
        }
    }
}
