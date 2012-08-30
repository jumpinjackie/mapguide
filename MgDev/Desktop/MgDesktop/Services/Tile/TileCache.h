#ifndef DESKTOP_TILE_CACHE_H
#define DESKTOP_TILE_CACHE_H

class MgResourceIdentifier;
class MgMapBase;
class MgByteReader;

class MgdTileCache : public MgGuardDisposable
{
public:
    MgdTileCache();

    static void Initialize();

    void GeneratePathnames(MgResourceIdentifier* mapDef, int scaleIndex,
        CREFSTRING group, int tileColumn, int tileRow,
        STRING& tilePathname, STRING& lockPathname, bool createFullPath);
    void GeneratePathnames(MgMapBase* map, int scaleIndex,
        CREFSTRING group, int tileColumn, int tileRow,
        STRING& tilePathname, STRING& lockPathname, bool createFullPath);

    STRING CreateFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING CreateFullPath(MgMapBase* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    MgByteReader* Get(CREFSTRING tilePathname);
    void Set(MgByteReader* img, CREFSTRING tilePathname);

    void Clear(MgMapBase* map);
    void Clear(MgResourceIdentifier* mapDef);

protected:
    virtual void Dispose()
    {
        delete this;
    }

private:
    // Unimplemented Constructors/Methods
    MgdTileCache(const MgdTileCache&);
    MgdTileCache& operator=(const MgdTileCache&);

    STRING GetBasePath(MgResourceIdentifier* mapDef);
    STRING GetBasePath(MgMapBase* map);

    STRING GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING GetFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING GetFullPath(MgMapBase* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    STRING CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    STRING GetScaleIndexFolder(int scaleIndex);
    STRING GetRowFolder(int tileRow);
    STRING GetColumnFolder(int tileColumn);
    STRING GetFolder(STRING prefix, int tileIndex, int tilesPerFolder);

    STRING GetTileName(int tileRow, int tileColumn);
    STRING GetTileIndexString(int tileIndex, int tilesPerFolder);

    static STRING sm_path;
    static INT32 sm_tileColumnsPerFolder;
    static INT32 sm_tileRowsPerFolder;
};

#endif