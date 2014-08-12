//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef _MG_TILE_CACHE_H_
#define _MG_TILE_CACHE_H_

#include "ServerTileDllExport.h"

//An abstract tile cache
class MG_SERVER_TILE_API MgTileCache : public MgDisposable
{
public:
    MgTileCache();
    virtual ~MgTileCache();

    virtual MgByteReader* GetTile(CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow,
                                  INT32 scaleIndex) = 0;

    
    virtual void SetTile(CREFSTRING baseMapLayerGroupName,
                         INT32 tileColumn,
                         INT32 tileRow,
                         INT32 scaleIndex,
                         MgByteReader* img) = 0;

    virtual void Clear() = 0;

    virtual INT32 GetDefaultTileSizeX() = 0;

    virtual INT32 GetDefaultTileSizeY() = 0;
};

#endif
