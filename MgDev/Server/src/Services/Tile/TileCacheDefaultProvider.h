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
#ifndef _MG_TILE_CACHE_DEFAULT_PROVIDER_H_
#define _MG_TILE_CACHE_DEFAULT_PROVIDER_H_

#include "TileCacheDefault.h"

class MG_SERVER_TILE_API MgTileCacheDefaultProvider : public MgTileCacheDefault
{
public:
    MgTileCacheDefaultProvider(MgResourceIdentifier* tileSetId, CREFSTRING path, INT32 tileWidth, INT32 tileHeight, CREFSTRING format, bool bRenderOnly);
    virtual ~MgTileCacheDefaultProvider();

    virtual MgByteReader* GetTile(CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow,
                                  INT32 scaleIndex);

    virtual MgByteReader* RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex, CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow);

    virtual INT32 GetDefaultTileSizeX();

    virtual INT32 GetDefaultTileSizeY();

    virtual STRING GetTileFormat();

protected:
    virtual void Dispose()
    {
        delete this;
    }

    virtual STRING GetBasePath();

private:
    Ptr<MgResourceIdentifier> m_tilesetId;
    STRING m_path;
    INT32 m_tileWidth;
    INT32 m_tileHeight;
    STRING m_format;
    bool m_renderOnly;
};

#endif