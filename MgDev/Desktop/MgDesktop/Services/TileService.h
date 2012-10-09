#ifndef DESKTOP_TILE_SERVICE_H
#define DESKTOP_TILE_SERVICE_H

#include "Tile/TileCache.h"
/// \ingroup Desktop_Service_Module
/// \{
class MG_DESKTOP_API MgdTileService : public MgService
{
    DECLARE_CLASSNAME(MgdTileService)

INTERNAL_API:
    MgdTileService();

EXTERNAL_API:
    virtual ~MgdTileService();

PUBLISHED_API:
    virtual MgByteReader* GetTile(MgdMap* map,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow);

    virtual MgByteReader* GetTile(MgResourceIdentifier* mapDefinition,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow,
                                  INT32 scaleIndex);

    virtual void ClearCache(MgdMap* map);

    virtual INT32 GetDefaultTileSizeX();

    virtual INT32 GetDefaultTileSizeY();

INTERNAL_API:

    virtual void SetTile(MgByteReader* img,
                         MgdMap* map,
                         INT32 scaleIndex,
                         CREFSTRING baseMapLayerGroupName,
                         INT32 tileColumn,
                         INT32 tileRow);

    virtual bool IsTileCacheEmpty() const;
    virtual bool NotifyResourcesChanged(MgSerializableCollection* resources,
        bool strict = true);

    void SetConnectionProperties(MgConnectionProperties* connProp);

protected:

    virtual INT32 GetClassId() { return m_cls_id; }

    virtual void Dispose() { delete this; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_TileService_TileService;

private:

    bool DetectTileLockFile(CREFSTRING lockPathname);

    MgByteReader* GetTile(CREFSTRING tilePathname, MgdMap* map, INT32 scaleIndex,
        CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow);

    void ClearMapCache(CREFSTRING mapName);

    MgResourceService* GetResourceServiceForMapDef(MgResourceIdentifier* mapDefinition, CREFSTRING funcName);

    // member data
    Ptr<MgdTileCache> m_tileCache;

    typedef std::map<STRING, MgMemoryStreamHelper*> MapCache;

    static ACE_Recursive_Thread_Mutex sm_MgdMutex;
    static bool sm_initialized;
    static MapCache sm_mapCache;
    static bool sm_renderOnly;
    static INT32 sm_creationCutoffTime;
    static INT32 sm_pollingInterval;
    static INT32 sm_mapCacheSize;
};
/// \}
#endif