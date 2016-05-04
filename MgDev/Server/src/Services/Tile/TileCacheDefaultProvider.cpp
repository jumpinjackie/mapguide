//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "TileCacheDefaultProvider.h"

MgTileCacheDefaultProvider::MgTileCacheDefaultProvider(MgResourceIdentifier* tileSetId, CREFSTRING path, INT32 tileWidth, INT32 tileHeight, CREFSTRING format, bool bRenderOnly)
{
    m_tilesetId = SAFE_ADDREF(tileSetId);
    m_path = path;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_format = format;
    m_renderOnly = bRenderOnly;
}

MgTileCacheDefaultProvider::~MgTileCacheDefaultProvider()
{

}

MgByteReader* MgTileCacheDefaultProvider::GetTile(CREFSTRING baseMapLayerGroupName,
                                                  INT32 tileColumn,
                                                  INT32 tileRow,
                                                  INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    MG_TRY()

    ret = GetTileForResource(m_tilesetId, baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgTileCacheDefaultProvider.GetTile")
    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// render a tile and store it in the cache
MgByteReader* MgTileCacheDefaultProvider::RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
    CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow)
{
    Ptr<MgByteReader> img;

    // get a rendering service instance
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);
    Ptr<MgRenderingService> svcRendering = dynamic_cast<MgRenderingService*>(
        serviceMan->RequestService(MgServiceType::RenderingService));
    assert(NULL != svcRendering);

    if (svcRendering != NULL)
    {
        // generate the tile
        img = svcRendering->RenderTile(map, baseMapLayerGroupName, tileColumn, tileRow, GetDefaultTileSizeX(), GetDefaultTileSizeY(), map->GetDisplayDpi(), GetTileFormat());

        // cache the tile
        if (!m_renderOnly)
        {
            Set(img, tilePathname);

            // rewind the reader since setting the tile advances it to the end
            if (img)
            {
                img->Rewind();
            }
        }
    }

    return img.Detach();
}

INT32 MgTileCacheDefaultProvider::GetDefaultTileSizeX()
{
    return m_tileWidth;
}

INT32 MgTileCacheDefaultProvider::GetDefaultTileSizeY()
{
    return m_tileHeight;
}

STRING MgTileCacheDefaultProvider::GetTileFormat()
{
    return m_format;
}

STRING MgTileCacheDefaultProvider::GetBasePath()
{
    return GetBasePathFromResourceId(m_tilesetId, m_path);
}