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

#ifndef MGSERVERTILESERVICE_H_
#define MGSERVERTILESERVICE_H_

#include "ServerTileDllExport.h"
#include "TileCache.h"

namespace MdfModel
{
    class TileSetDefinition;
}

class MG_SERVER_TILE_API MgServerTileService : public MgTileService
{
    DECLARE_CLASSNAME(MgServerTileService)
public:
    MgServerTileService();
    ~MgServerTileService();
    DECLARE_CREATE_SERVICE()

    virtual MgByteReader* GetTile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow);

    virtual MgByteReader* GetTile(
        MgResourceIdentifier* resource,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow,
        INT32 scaleIndex);

    virtual void ClearCache(MgMap* map);

    virtual void ClearCache(MgResourceIdentifier* tileSet);

    virtual INT32 GetDefaultTileSizeX();

    virtual INT32 GetDefaultTileSizeY();

    virtual INT32 GetDefaultTileSizeX(MgResourceIdentifier* tileSet);

    virtual INT32 GetDefaultTileSizeY(MgResourceIdentifier* tileSet);

    virtual MgByteReader* GetTileProviders();

    virtual void SetTile(
        MgByteReader* img,
        MgMap* map,
        INT32 scaleIndex,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow);

    virtual bool IsTileCacheEmpty() const;

    virtual bool NotifyResourcesChanged(MgSerializableCollection* resources, bool strict = true);

    void SetConnectionProperties(MgConnectionProperties* connProp);

private:
    MgTileCache* GetTileCache(MgResourceIdentifier* resource);
    MgTileCache* GetTileCache(MgResourceIdentifier* tileSetId, MdfModel::TileSetDefinition* tileset);
};

#endif
