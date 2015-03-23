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
#ifndef _MG_TILE_CACHE_DEFAULT_H_
#define _MG_TILE_CACHE_DEFAULT_H_

#include "TileCache.h"

//The default tile cache implementation with default settings based on server configuration
class MG_SERVER_TILE_API MgTileCacheDefault : public MgTileCache
{
public:
    MgTileCacheDefault();
    MgTileCacheDefault(MgMap* map);
    MgTileCacheDefault(MgResourceIdentifier* resource);
    virtual ~MgTileCacheDefault();

    static void Initialize();

    virtual MgByteReader* GetTile(CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow,
                                  INT32 scaleIndex);

    virtual void SetTile(CREFSTRING baseMapLayerGroupName,
                         INT32 tileColumn,
                         INT32 tileRow,
                         INT32 scaleIndex,
                         MgByteReader* img);

    virtual void Clear();

    virtual INT32 GetDefaultTileSizeX();

    virtual INT32 GetDefaultTileSizeY();

    virtual STRING GetTileFormat();

    void ClearMapCache(CREFSTRING mapName);

    static bool IsTileCacheEmpty();

protected:
    virtual void Dispose()
    {
        delete this;
    }

    virtual STRING GetBasePath();
    virtual STRING GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    virtual STRING CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    virtual MgByteReader* RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex, CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow);

    virtual STRING GetTileName(int tileRow, int tileColumn);

    MgByteReader* GetTileForResource(MgResourceIdentifier* resource,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow,
                                     INT32 scaleIndex);

    STRING GetBasePathFromResourceId(MgResourceIdentifier* resId, CREFSTRING rootPath);

    void Set(MgByteReader* img, CREFSTRING path);
    MgByteReader* Get(CREFSTRING path);

private:
    MgByteReader* GetTileForMap(CREFSTRING baseMapLayerGroupName,
                                INT32 tileColumn,
                                INT32 tileRow,
                                INT32 scaleIndex);

    void GeneratePathNames(int scaleIndex,
        CREFSTRING group, int tileColumn, int tileRow,
        STRING& tilePathname, STRING& lockPathname, bool createFullPath);

    STRING GetFullPath(int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    
    STRING CreateFullPath(int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    STRING GetScaleIndexFolder(int scaleIndex);
    STRING GetRowFolder(int tileRow);
    STRING GetColumnFolder(int tileColumn);
    STRING GetFolder(STRING prefix, int tileIndex, int tilesPerFolder);

    STRING GetTileIndexString(int tileIndex, int tilesPerFolder);

    MgResourceService* GetResourceServiceForMapDef(MgResourceIdentifier* mapDefinition, CREFSTRING funcName);
    bool DetectTileLockFile(CREFSTRING lockPathname);

    Ptr<MgResourceIdentifier> m_resourceId;
    Ptr<MgMap> m_map;

    static STRING sm_path;
    static INT32 sm_tileColumnsPerFolder;
    static INT32 sm_tileRowsPerFolder;

    typedef std::map<STRING, MgMemoryStreamHelper*> MapCache;

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static MapCache sm_mapCache;
    static bool sm_initialized;
    static bool sm_renderOnly;
    static INT32 sm_creationCutoffTime;
    static INT32 sm_pollingInterval;
    static INT32 sm_mapCacheSize;
};

#endif