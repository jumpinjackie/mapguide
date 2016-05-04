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
#include "TileCacheXYZProvider.h"

MgTileCacheXYZProvider::MgTileCacheXYZProvider(MgResourceIdentifier* tileSetId, CREFSTRING path, CREFSTRING format, bool bRenderOnly)
{
    m_tilesetId = SAFE_ADDREF(tileSetId);
    m_path = path;
    m_format = format;
    m_renderOnly = bRenderOnly;
}

MgTileCacheXYZProvider::~MgTileCacheXYZProvider()
{

}

MgByteReader* MgTileCacheXYZProvider::GetTile(CREFSTRING baseMapLayerGroupName,
                                                  INT32 tileColumn,
                                                  INT32 tileRow,
                                                  INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    MG_TRY()

    ret = GetTileForResource(m_tilesetId, baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgTileCacheXYZProvider.GetTile")
    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// render a tile and store it in the cache
MgByteReader* MgTileCacheXYZProvider::RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
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
        img = svcRendering->RenderTileXYZ(map, baseMapLayerGroupName, tileRow, tileColumn, scaleIndex, map->GetDisplayDpi(), m_format);

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

INT32 MgTileCacheXYZProvider::GetDefaultTileSizeX()
{
    return 256; //Always
}

INT32 MgTileCacheXYZProvider::GetDefaultTileSizeY()
{
    return 256; //Always
}

STRING MgTileCacheXYZProvider::GetTileFormat()
{
    return m_format;
}

STRING MgTileCacheXYZProvider::GetBasePath()
{
    return GetBasePathFromResourceId(m_tilesetId, m_path);
}

STRING MgTileCacheXYZProvider::CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    assert(!basePath.empty());
    STRING fullPath = basePath;

    // Create base directory if it does not exist.
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create group directory if it does not exist.
    fullPath += L"/";
    fullPath += group;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create z directory if it does not exist.
    fullPath += L"/";
    STRING sZ;
    MgUtil::Int32ToString(scaleIndex, sZ);
    fullPath += sZ;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create x directory if it does not exist.
    fullPath += L"/";
    STRING sX;
    MgUtil::Int32ToString(tileRow, sX);
    fullPath += sX;
    MgFileUtil::CreateDirectory(fullPath, false);

    return fullPath;
}

// gets the full path to use with the tile cache for the given base path / scale index / group
STRING MgTileCacheXYZProvider::GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    // Build full path
    //     <fullPath> = <basePath>/<group>/<z>/<x>/<y>.<format>
    assert(!basePath.empty());
    STRING fullPath = basePath;

    fullPath += L"/";
    fullPath += group;

    fullPath += L"/";
    STRING sZ;
    MgUtil::Int32ToString(scaleIndex, sZ);
    fullPath += sZ;

    fullPath += L"/";
    STRING sX;
    MgUtil::Int32ToString(tileRow, sX);
    fullPath += sX;

    return fullPath;
}

// Get the filename corresponding to the specified row and column.
// No file extension is added.
STRING MgTileCacheXYZProvider::GetTileName(int tileRow, int tileColumn)
{
    //row (x) is already part of the parent directory, so column (y) is the tile name
    STRING ret;
    MgUtil::Int32ToString(tileColumn, ret);
    return ret;
}