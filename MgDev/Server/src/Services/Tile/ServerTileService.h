//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

class MG_SERVER_TILE_API MgServerTileService : public MgTileService
{
    DECLARE_CLASSNAME(MgServerTileService)

public:
    MgServerTileService();
    ~MgServerTileService();
    DECLARE_CREATE_SERVICE()

    virtual MgByteReader* GetTile(MgMap* map,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow);

    virtual MgByteReader* GetTile(MgResourceIdentifier* mapDefinition,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow,
                                  INT32 scaleIndex);

    virtual void SetTile(MgByteReader* img,
                         MgMap* map,
                         INT32 scaleIndex,
                         CREFSTRING baseMapLayerGroupName,
                         INT32 tileColumn,
                         INT32 tileRow);

    virtual void ClearCache(MgMap* map);

    virtual INT32 GetDefaultTileSizeX();

    virtual INT32 GetDefaultTileSizeY();

    virtual bool IsTileCacheEmpty() const;
    virtual bool NotifyResourcesChanged(MgSerializableCollection* resources,
        bool strict = true);

    void SetConnectionProperties(MgConnectionProperties* connProp);

private:

    bool DetectTileLockFile(CREFSTRING lockPathname);

    MgByteReader* GetTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
        CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow);

    void ClearMapCache(CREFSTRING mapName);
    MgResourceService* GetResourceServiceForMapDef (MgResourceIdentifier* mapDefinition,
                                                        CREFSTRING funcName);

    // member data
    Ptr<MgTileCache> m_tileCache;

    typedef std::map<STRING, MgMemoryStreamHelper*> MapCache;

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static bool sm_initialized;
    static MapCache sm_mapCache;
    static bool sm_renderOnly;
    static INT32 sm_creationCutoffTime;
    static INT32 sm_pollingInterval;
    static INT32 sm_mapCacheSize;
};

#endif
