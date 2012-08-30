#include "MgDesktop.h"
#include "TileDefs.h"
#include "TileCache.h"

const STRING SCALE_INDEX_PREFIX = L"S";
const STRING ROW_PREFIX = L"R";
const STRING COLUMN_PREFIX = L"C";

STRING MgdTileCache::sm_path = L"";
INT32 MgdTileCache::sm_tileColumnsPerFolder = 30;
INT32 MgdTileCache::sm_tileRowsPerFolder = 30;

// default constructor
MgdTileCache::MgdTileCache()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the tile cache configuration.
///
void MgdTileCache::Initialize()
{
    if (sm_path.empty())
    {
        // initialize the tile cache path
        MgConfiguration* configuration = MgConfiguration::GetInstance();

        configuration->GetStringValue(
            MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyTileCachePath,
            sm_path,
            MgdConfigProperties::DefaultTileServicePropertyTileCachePath);

        // generate directory location for tile cache
        MgFileUtil::AppendSlashToEndOfPath(sm_path);

        // create directory if it is not already there
        MgFileUtil::CreateDirectory(sm_path, false, true);

        configuration->GetIntValue(MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyTileColumnsPerFolder,
            sm_tileColumnsPerFolder,
            MgdConfigProperties::DefaultTileServicePropertyTileColumnsPerFolder);

        configuration->GetIntValue(MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyTileRowsPerFolder,
            sm_tileRowsPerFolder,
            MgdConfigProperties::DefaultTileServicePropertyTileRowsPerFolder);

        configuration->GetIntValue(MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyTileSizeX,
            MgdTileParameters::tileWidth,
            MgdConfigProperties::DefaultTileServicePropertyTileSizeX);

        configuration->GetIntValue(MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyTileSizeY,
            MgdTileParameters::tileHeight,
            MgdConfigProperties::DefaultTileServicePropertyTileSizeY);

        STRING format;
        configuration->GetStringValue(MgdConfigProperties::TileServicePropertiesSection,
            MgdConfigProperties::TileServicePropertyImageFormat,
            format,
            MgdConfigProperties::DefaultTileServicePropertyImageFormat);

        // Only allow GIF, PNG, PNG8 and JPG as tile formats
        if (format == MgdImageFormats::Png || format == MgdImageFormats::Png8 || format == MgdImageFormats::Jpeg || format == MgdImageFormats::Gif)
        {
            MgdTileParameters::tileFormat = format;
        }
        else
        {
            MgdTileParameters::tileFormat = MgdImageFormats::Png;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generate tile and lock pathnames.
///
void MgdTileCache::GeneratePathnames(MgResourceIdentifier* mapDef, int scaleIndex,
    CREFSTRING group, int tileColumn, int tileRow,
    STRING& tilePathname, STRING& lockPathname, bool createFullPath)
{
    STRING fileName = L"/" + GetTileName(tileRow, tileColumn) + L".";
    STRING basePath = GetBasePath(mapDef);

    if (createFullPath)
    {
        tilePathname = CreateFullPath(basePath, scaleIndex, group, tileColumn, tileRow);
    }
    else
    {
        tilePathname = GetFullPath(basePath, scaleIndex, group, tileColumn, tileRow);
    }

    // Generate the tile and lock pathnames
    //     <tilePathname> = <fullPath>/<row>_<column>.png/jpg
    //     <lockPathname> = <fullPath>/<row>_<column>.lck
    tilePathname += fileName;
    lockPathname = tilePathname;
    if (MgdTileParameters::tileFormat == MgdImageFormats::Jpeg)
    {
        tilePathname += L"jpg";
    }
    else if (MgdTileParameters::tileFormat == MgdImageFormats::Gif)
    {
        tilePathname += L"gif";
    }
    else
    {
        tilePathname += L"png";
    }

    lockPathname += L"lck";
}

void MgdTileCache::GeneratePathnames(MgMapBase* map, int scaleIndex,
    CREFSTRING group, int tileColumn, int tileRow,
    STRING& tilePathname, STRING& lockPathname, bool createFullPath)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();

    GeneratePathnames(mapDef, scaleIndex, group, tileColumn, tileRow,
        tilePathname, lockPathname, createFullPath);
}

// returns any cached tile for the given pathname
MgByteReader* MgdTileCache::Get(CREFSTRING tilePathname)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (MgFileUtil::PathnameExists(tilePathname))
    {
        Ptr<MgByteSource> byteSource = new MgByteSource(tilePathname, false);

        if (MgdTileParameters::tileFormat == MgdImageFormats::Jpeg)
        {
            byteSource->SetMimeType(MgMimeType::Jpeg);
        }
        else if (MgdTileParameters::tileFormat == MgdImageFormats::Gif)
        {
            byteSource->SetMimeType(MgMimeType::Gif);
        }
        else
        {
            byteSource->SetMimeType(MgMimeType::Png);
        }

        ret = byteSource->GetReader();
    }

    MG_CATCH_AND_RELEASE()

    return ret.Detach();
}

// caches a tile for the given pathname
void MgdTileCache::Set(MgByteReader* img, CREFSTRING tilePathname)
{
    if (img != NULL)
    {
        Ptr<MgByteSink> byteSink = new MgByteSink(img);

        byteSink->ToFile(tilePathname);
    }
}

// clears the tile cache for the given map
void MgdTileCache::Clear(MgMapBase* map)
{
    if (map != NULL)
    {
        STRING basePath = GetBasePath(map);

        // delete main map directory
        if (!basePath.empty())
            MgFileUtil::DeleteDirectory(basePath, true, false);
    }
}

// clears the tile cache for the given map
void MgdTileCache::Clear(MgResourceIdentifier* mapDef)
{
    // the resource must be a map definition
    if (mapDef != NULL && mapDef->GetResourceType() == MgResourceType::MapDefinition)
    {
        STRING basePath = GetBasePath(mapDef);

        // delete main map directory
        if (!basePath.empty())
            MgFileUtil::DeleteDirectory(basePath, true, false);
    }
}

// gets the base path to use with the tile cache for the given map definition resource
STRING MgdTileCache::GetBasePath(MgResourceIdentifier* mapDef)
{
    assert(mapDef != NULL);
    assert(mapDef->GetResourceType() == MgResourceType::MapDefinition);
    STRING mapPath;

    if (mapDef->GetRepositoryType() == MgRepositoryType::Library)
    {
        // for maps in the library repository the path+name is unique
        mapPath  = mapDef->GetPath();
        mapPath += L"_";
        mapPath += mapDef->GetName();
    }
    else if (mapDef->GetRepositoryType() == MgRepositoryType::Session)
    {
        // for maps in the session repository we use the session + path + map name
        mapPath  = mapDef->GetRepositoryName();
        mapPath += L"_";

        STRING resourcePath  = mapDef->GetPath();

        if (!resourcePath.empty())
        {
            mapPath += resourcePath;
            mapPath += L"_";
        }

        mapPath += mapDef->GetName();
    }
    else
    {
        // shouldn't get here
        assert(false);
        return mapPath;
    }

    // for safety
    mapPath = MgUtil::ReplaceString(mapPath, L"/", L"_");
    mapPath = MgUtil::ReplaceString(mapPath, L":", L"_");

    // Build the base path using format "%ls%ls":
    //     <basePath> = <tileCachePath><mapPath>
    STRING basePath = sm_path;
    basePath += mapPath;

    return basePath;
}

// gets the base path to use with the tile cache for the given map
STRING MgdTileCache::GetBasePath(MgMapBase* map)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();
    return GetBasePath(mapDef);
}

// gets the full path to use with the tile cache for the given base path / scale index / group
STRING MgdTileCache::GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    // Build full path
    //     <fullPath> = <basePath>/<scaleIndex>/<group>/<rowFolder>/<columnFolder>
    assert(!basePath.empty());
    STRING fullPath = basePath;

    fullPath += L"/";
    fullPath += GetScaleIndexFolder(scaleIndex);

    fullPath += L"/";
    fullPath += group;

    fullPath += L"/";
    fullPath += GetRowFolder(tileRow);

    fullPath += L"/";
    fullPath += GetColumnFolder(tileColumn);

    return fullPath;
}

// gets the full path to use with the tile cache for the given map definition / scale index / group
STRING MgdTileCache::GetFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    return GetFullPath(GetBasePath(mapDef), scaleIndex, group, tileColumn, tileRow);
}

// gets the full path to use with the tile cache for the given map / scale index / group
STRING MgdTileCache::GetFullPath(MgMapBase* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();
    return GetFullPath(mapDef, scaleIndex, group, tileColumn, tileRow);
}

STRING MgdTileCache::CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    assert(!basePath.empty());
    STRING fullPath = basePath;

    // Create base directory if it does not exist.
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create scale directory if it does not exist.
    fullPath += L"/";
    fullPath += GetScaleIndexFolder(scaleIndex);
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create group directory if it does not exist.
    fullPath += L"/";
    fullPath += group;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create row directory if it does not exist.
    fullPath += L"/";
    fullPath += GetRowFolder(tileRow);
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create column directory if it does not exist.
    fullPath += L"/";
    fullPath += GetColumnFolder(tileColumn);
    MgFileUtil::CreateDirectory(fullPath, false);

    return fullPath;
}

STRING MgdTileCache::CreateFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    return CreateFullPath(GetBasePath(mapDef), scaleIndex, group, tileColumn, tileRow);
}

STRING MgdTileCache::CreateFullPath(MgMapBase* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    return CreateFullPath(GetBasePath(map), scaleIndex, group, tileColumn, tileRow);
}

// Get the folder name corresponding to the specified scale index
STRING MgdTileCache::GetScaleIndexFolder(int scaleIndex)
{
    STRING scaleIndexString;
    MgUtil::Int32ToString(scaleIndex, scaleIndexString);
    return SCALE_INDEX_PREFIX + scaleIndexString;
}

// Get the folder name corresponding to the specified tile row
STRING MgdTileCache::GetRowFolder(int tileRow)
{
    return GetFolder(ROW_PREFIX, tileRow, sm_tileRowsPerFolder);
}

// Get the folder name corresponding to the specified tile column
STRING MgdTileCache::GetColumnFolder(int tileColumn)
{
    return GetFolder(COLUMN_PREFIX, tileColumn, sm_tileColumnsPerFolder);
}

// Get the parent folder for a given row or column
STRING MgdTileCache::GetFolder(STRING prefix, int tileIndex, int tilesPerFolder)
{
    STRING folder;

    // Determine which folder contains this tile
    int folderIndex = tileIndex / tilesPerFolder;
    int firstTileIndex = folderIndex * tilesPerFolder;
    if(tileIndex < 0 && firstTileIndex == 0)
    {
        folder = L"-0";
    }
    else
    {
        MgUtil::Int32ToString(firstTileIndex, folder);
    }
    return prefix + folder;
}

// Get the filename corresponding to the specified row and column.
// No file extension is added.
STRING MgdTileCache::GetTileName(int tileRow, int tileColumn)
{
    return GetTileIndexString(tileRow, sm_tileRowsPerFolder) + L"_" +
        GetTileIndexString(tileColumn, sm_tileColumnsPerFolder);
}

// When a tile is stored in a folder, the index value of the parent folder
// is subtracted from the overall tile index.
// e.g. If we store 30 rows of tiles per folder, a tile with overall row
// index 73 will be stored in the row folder "R60" with a row index of 13.
// Note: Negative values are maintained even near the origin, so a tile with
// an overall row index of -13 would be stored in row folder "R-0" with a
// row index of -13.
STRING MgdTileCache::GetTileIndexString(int tileIndex, int tilesPerFolder)
{
    STRING name;

    // Determine the offset of this tile within the folder
    int tileNameIndex = tileIndex % tilesPerFolder;
    if(tileIndex < 0 && tileNameIndex == 0)
    {
        name = L"-0";
    }
    else
    {
        MgUtil::Int32ToString(tileNameIndex, name);
    }
    return name;
}


