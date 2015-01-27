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

#include "MapGuideCommon.h"
#include "TileCacheDefault.h"
#include "ServiceManager.h"

const STRING SCALE_INDEX_PREFIX = L"S";
const STRING ROW_PREFIX = L"R";
const STRING COLUMN_PREFIX = L"C";

STRING MgTileCacheDefault::sm_path = L"";
INT32 MgTileCacheDefault::sm_tileColumnsPerFolder = 30;
INT32 MgTileCacheDefault::sm_tileRowsPerFolder = 30;

ACE_Recursive_Thread_Mutex MgTileCacheDefault::sm_mutex;
MgTileCacheDefault::MapCache MgTileCacheDefault::sm_mapCache;
bool MgTileCacheDefault::sm_initialized = false;
bool MgTileCacheDefault::sm_renderOnly = false;
INT32 MgTileCacheDefault::sm_creationCutoffTime = 120;     // in seconds
INT32 MgTileCacheDefault::sm_pollingInterval = 1;          // in seconds
INT32 MgTileCacheDefault::sm_mapCacheSize = 10;

MgTileCacheDefault::MgTileCacheDefault()
{
    m_map = NULL;
    m_resourceId = NULL;
}

MgTileCacheDefault::MgTileCacheDefault(MgMap* map)
{
    m_map = SAFE_ADDREF(map);
    m_resourceId = NULL;
}

MgTileCacheDefault::MgTileCacheDefault(MgResourceIdentifier* resource)
{
    m_map = NULL;
    m_resourceId = SAFE_ADDREF(resource);
}

MgTileCacheDefault::~MgTileCacheDefault()
{

}

void MgTileCacheDefault::Initialize()
{
    if (!sm_initialized)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

        if (!sm_initialized)
        {
            MgConfiguration* configuration = MgConfiguration::GetInstance();

            configuration->GetBoolValue(
                MgConfigProperties::TileServicePropertiesSection,
                MgConfigProperties::TileServicePropertyRenderOnly,
                sm_renderOnly,
                MgConfigProperties::DefaultTileServicePropertyRenderOnly);

            configuration->GetIntValue(
                MgConfigProperties::TileServicePropertiesSection,
                MgConfigProperties::TileServicePropertyCreationCutoffTime,
                sm_creationCutoffTime,
                MgConfigProperties::DefaultTileServicePropertyCreationCutoffTime);

            configuration->GetIntValue(
                MgConfigProperties::TileServicePropertiesSection,
                MgConfigProperties::TileServicePropertyPollingInterval,
                sm_pollingInterval,
                MgConfigProperties::DefaultTileServicePropertyPollingInterval);

            configuration->GetIntValue(
                MgConfigProperties::TileServicePropertiesSection,
                MgConfigProperties::TileServicePropertyTiledMapCacheSize,
                sm_mapCacheSize,
                MgConfigProperties::DefaultTileServicePropertyTiledMapCacheSize);

            if (sm_path.empty())
            {
                configuration->GetStringValue(
                    MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyTileCachePath,
                    sm_path,
                    MgConfigProperties::DefaultTileServicePropertyTileCachePath);

                // generate directory location for tile cache
                MgFileUtil::AppendSlashToEndOfPath(sm_path);

                // create directory if it is not already there
                MgFileUtil::CreateDirectory(sm_path, false, true);

                MgTileParameters::tileCachePath = sm_path;

                configuration->GetIntValue(MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyTileColumnsPerFolder,
                    sm_tileColumnsPerFolder,
                    MgConfigProperties::DefaultTileServicePropertyTileColumnsPerFolder);

                configuration->GetIntValue(MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyTileRowsPerFolder,
                    sm_tileRowsPerFolder,
                    MgConfigProperties::DefaultTileServicePropertyTileRowsPerFolder);

                configuration->GetIntValue(MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyTileSizeX,
                    MgTileParameters::tileWidth,
                    MgConfigProperties::DefaultTileServicePropertyTileSizeX);

                configuration->GetIntValue(MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyTileSizeY,
                    MgTileParameters::tileHeight,
                    MgConfigProperties::DefaultTileServicePropertyTileSizeY);

                STRING format;
                configuration->GetStringValue(MgConfigProperties::TileServicePropertiesSection,
                    MgConfigProperties::TileServicePropertyImageFormat,
                    format,
                    MgConfigProperties::DefaultTileServicePropertyImageFormat);

                // Only allow GIF, PNG, PNG8 and JPG as tile formats
                if (format == MgImageFormats::Png || format == MgImageFormats::Png8 || format == MgImageFormats::Jpeg || format == MgImageFormats::Gif)
                {
                    MgTileParameters::tileFormat = format;
                }
                else
                {
                    MgTileParameters::tileFormat = MgImageFormats::Png;
                }
            }
            sm_initialized = true;
        }
    }
}

MgByteReader* MgTileCacheDefault::GetTile(CREFSTRING baseMapLayerGroupName,
                                          INT32 tileColumn,
                                          INT32 tileRow,
                                          INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (m_map != NULL)
        ret = GetTileForMap(baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);
    else if (m_resourceId != NULL)
        ret = GetTileForResource(m_resourceId, baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgTileCacheDefault.GetTile")

    return ret.Detach();
}

MgByteReader* MgTileCacheDefault::GetTileForResource(MgResourceIdentifier* resource,
                                                     CREFSTRING baseMapLayerGroupName,
                                                     INT32 tileColumn,
                                                     INT32 tileRow,
                                                     INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    CHECKARGUMENTNULL(resource, L"MgTileCacheDefault.GetTileForResource");
    CHECKARGUMENTEMPTYSTRING(baseMapLayerGroupName, L"MgTileCacheDefault.GetTileForResource");

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"5");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgTileCacheDefault.GetTileForResource",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // get the service from our helper method
    Ptr<MgResourceService> resourceService = GetResourceServiceForMapDef(resource,
                                            L"MgTileCacheDefault.GetTileForResource");
    // Generate tile and lock pathnames.
    GeneratePathNames(scaleIndex, baseMapLayerGroupName, tileColumn, tileRow, tilePathname, lockPathname, false);

    // If there is a dangling lock file, then attempt to remove it.
    if (DetectTileLockFile(lockPathname))
    {
        // TODO: Handle the exception by displaying a tile with an error message?
        MgFileUtil::DeleteFile(lockPathname, true);
    }

    // try getting the tile from the cache
    ret = Get(tilePathname);

    // if the reader is NULL then the tile wasn't in the cache and we
    // need to generate it
    while (NULL == ret)
    {
        // Attempt use a cached & serialized MgMap object
        Ptr<MgMemoryStreamHelper> cachedMap;
        STRING mapString = resource->ToString();
        Ptr<MgMap> map;

        // Protect the serialized MgMap cache with a mutex.  Stream reading is not
        // thread safe so we need to deserialize the map within the mutex to ensure
        // that a Rewind() is not called in the middle of a Deserialize().
        // Lockfile test and creation is in same protected scope.
        {
            // Attempt to lock the tile file.
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

            // Bail out if the tile file has been locked for so long.
            if (DetectTileLockFile(lockPathname))
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgTileCacheDefault.GetTileForResource",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToLockTileFile", NULL);
            }

            // try getting the tile from the cache
            ret = Get(tilePathname);

            if (NULL != ret)
            {
                break;  // tile was in tileCache .. done.
            }

            // Create the lock file and close it right away.
            CreateFullPath(scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
            lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

            if (NULL == lockFile)
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgTileCacheDefault.GetTileForResource",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToOpenLockFile", NULL);
            }
            else
            {
                ACE_OS::fclose(lockFile);
            }

            MapCache::const_iterator iter = sm_mapCache.find(mapString);
            if (sm_mapCache.end() != iter)
            {
                cachedMap = SAFE_ADDREF((*iter).second);
                cachedMap->Rewind();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map = new MgMap();
                map->Deserialize(stream);
            }
            else
            {
                Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
                Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
                siteConn->Open(userInfo);
                map = new MgMap(siteConn);
                map->Create(resourceService, resource, mapString, false /* Allow MgMap to be created from any tile provider if resource is tile set */);
                cachedMap = new MgMemoryStreamHelper();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map->Serialize(stream);
                if ((INT32)sm_mapCache.size() >= sm_mapCacheSize)
                {
                    ClearMapCache(L"");
                }
                sm_mapCache[mapString] = SAFE_ADDREF((MgMemoryStreamHelper*)cachedMap);
            }
        }   // end of mutex scope

        //Some tile providers (eg. XYZ) may not work with pre-defined scale lists, so the scale index doesn't
        //resolve to some position in a finite scale list (that won't exist for XYZ), but rather it is part of 
        //some formula used to determine the appropriate scale
        if (map->GetFiniteDisplayScaleCount() > 0)
        {
            double scale = map->GetFiniteDisplayScaleAt(scaleIndex);
            map->SetViewScale(scale);
        }

        // Render the tile and cache it.
        ret = RenderAndCacheTile(tilePathname, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
        break;
    }

    MG_CATCH(L"MgTileCacheDefault.GetTileForResource")

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    MG_THROW()

    return ret.Detach();
}

MgByteReader* MgTileCacheDefault::GetTileForMap(CREFSTRING baseMapLayerGroupName,
                                                INT32 tileColumn,
                                                INT32 tileRow,
                                                INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(baseMapLayerGroupName, L"MgTileCacheDefault.SetTile");
    CHECKNULL(m_map, L"MgTileCacheDefault.SetTile");

    // find the finite display scale closest to the requested map scale
    double scale = m_map->GetViewScale();
    INT32 scaleIndex = m_map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
    {
        throw new MgInvalidMapDefinitionException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Detect the lock file to see if another thread is creating the tile file.
    GeneratePathNames(scaleIndex, baseMapLayerGroupName,
        tileColumn, tileRow, tilePathname, lockPathname, false);

    // If there is a dangling lock file, then attempt to remove it.
    if (DetectTileLockFile(lockPathname))
    {
        // TODO: Handle the exception by displaying a tile with an error message?
        MgFileUtil::DeleteFile(lockPathname, true);
    }

    // try getting the tile from the cache
    ret = Get(tilePathname);

    // if the reader is NULL then the tile wasn't in the cache and we
    // need to generate it
    while (NULL == ret)
    {
        {
            // Attemp to lock the tile file.
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

            // Bail out if the tile file has been locked for so long.
            if (DetectTileLockFile(lockPathname))
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgServerTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToLockTileFile", NULL);
            }

            // try getting the tile from the cache
            ret = Get(tilePathname);

            if (NULL != ret)
            {
                break;
            }

            // Create the lock file and close it right away.
            CreateFullPath(scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
            lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

            if (NULL == lockFile)
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgServerTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToOpenLockFile", NULL);
            }
            else
            {
                ACE_OS::fclose(lockFile);
            }
        }

        // Render the tile and cache it.
        ret = RenderAndCacheTile(tilePathname, m_map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
        break;
    }

    MG_CATCH(L"MgServerTileService.GetTile")

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    MG_THROW()

    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// render a tile and store it in the cache
MgByteReader* MgTileCacheDefault::RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
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
        if (!sm_renderOnly)
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

void MgTileCacheDefault::SetTile(CREFSTRING baseMapLayerGroupName,
                                 INT32 tileColumn,
                                 INT32 tileRow,
                                 INT32 scaleIndex,
                                 MgByteReader* img)
{
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    CHECKARGUMENTNULL(img, L"MgTileCacheDefault.SetTile");
    CHECKARGUMENTEMPTYSTRING(baseMapLayerGroupName, L"MgTileCacheDefault.SetTile");
    CHECKNULL((MgMap*)m_map, L"MgTileCacheDefault.SetTile");

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgTileCacheDefault.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // Generate tile and lock pathnames.
    GeneratePathNames(scaleIndex, baseMapLayerGroupName,
        tileColumn, tileRow, tilePathname, lockPathname, true);

    {
        // Attemp to lock the tile file.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

        if (DetectTileLockFile(lockPathname))
        {
            // Attemp to remove a dangling lock file.
            MgFileUtil::DeleteFile(lockPathname, true);
        }

        // Create the lock file and close it right away.
        lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

        if (NULL == lockFile)
        {
            MgStringCollection arguments;
            arguments.Add(lockPathname);

            throw new MgFileIoException(L"MgTileCacheDefault.SetTile",
                __LINE__, __WFILE__, &arguments, L"MgUnableToOpenLockFile", NULL);
        }
        else
        {
            ACE_OS::fclose(lockFile);
        }
    }

    // cache the tile
    Set(img, tilePathname);

    MG_CATCH(L"MgTileCacheDefault.SetTile")

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    MG_THROW()
}

void MgTileCacheDefault::Clear()
{
    STRING basePath = GetBasePath();

    // delete main map directory
    if (!basePath.empty())
        MgFileUtil::DeleteDirectory(basePath, true, false);
}

INT32 MgTileCacheDefault::GetDefaultTileSizeX()
{
    return MgTileParameters::tileWidth;
}

INT32 MgTileCacheDefault::GetDefaultTileSizeY()
{
    return MgTileParameters::tileHeight;
}

STRING MgTileCacheDefault::GetTileFormat()
{
    return MgTileParameters::tileFormat;
}

///////////////////////////////////////////////////////////////////////////////
// accessor method for resource service
MgResourceService* MgTileCacheDefault::GetResourceServiceForMapDef(MgResourceIdentifier* mapDefinition,
                                                                   CREFSTRING funcName)
{
    // get service manager
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(NULL != resourceService);

    if (!resourceService->HasPermission(mapDefinition, MgResourcePermission::ReadOnly))
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

        MgStringCollection arguments;
        arguments.Add(mapDefinition->ToString());

        throw new MgPermissionDeniedException(
            funcName, __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    return resourceService.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Detect if the tile file has been locked by another thread or process.
///
bool MgTileCacheDefault::DetectTileLockFile(CREFSTRING lockPathname)
{
    bool found = false;
    struct _stat64 lockFileInfo;

    // Check the lock file to see if another thread/process is writing the tile file.
    while (MgFileUtil::GetFileStatus(lockPathname, lockFileInfo))
    {
        time_t currTime;
        ACE_OS::time(&currTime);
        INT32 diffTime = (INT32)(currTime - lockFileInfo.st_mtime);

        if (diffTime < sm_creationCutoffTime)
        {
            ACE_OS::sleep(sm_pollingInterval);
        }
        else
        {
            found = true;
            break;
        }
    }

    return found;
}

///////////////////////////////////////////////////////////////////////////////
void MgTileCacheDefault::ClearMapCache(CREFSTRING mapDefinition)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
    MapCache::iterator iter = sm_mapCache.end();
    if (mapDefinition.empty())
    {
        for (iter = sm_mapCache.begin(); iter != sm_mapCache.end(); ++iter)
        {
            SAFE_RELEASE((*iter).second);
            (*iter).second = NULL;
        }
        sm_mapCache.clear();

        STRING message;
        MgResources* resources = NULL;
        MG_TRY()
        resources = MgResources::GetInstance();
        if (NULL != resources)
        {
            message = resources->GetResourceMessage(MgResources::ErrorDescription, L"MgMapCacheCleared", NULL);
            MG_LOG_ERROR_ENTRY(message.c_str());
        }
        MG_CATCH_AND_RELEASE()
    }
    else
    {
        iter = sm_mapCache.find(mapDefinition);
        if (sm_mapCache.end() != iter)
        {
            SAFE_RELEASE((*iter).second);
            (*iter).second = NULL;
            sm_mapCache.erase(iter);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generate tile and lock pathnames.
///
void MgTileCacheDefault::GeneratePathNames(int scaleIndex,
    CREFSTRING group, int tileColumn, int tileRow,
    STRING& tilePathname, STRING& lockPathname, bool createFullPath)
{
    STRING fileName = L"/" + GetTileName(tileRow, tileColumn) + L".";
    STRING basePath = GetBasePath();

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
    STRING fmt = GetTileFormat();
    if (fmt == MgImageFormats::Jpeg)
    {
        tilePathname += L"jpg";
    }
    else if (fmt == MgImageFormats::Gif)
    {
        tilePathname += L"gif";
    }
    else
    {
        tilePathname += L"png";
    }

    lockPathname += L"lck";
}

// Get the filename corresponding to the specified row and column.
// No file extension is added.
STRING MgTileCacheDefault::GetTileName(int tileRow, int tileColumn)
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
STRING MgTileCacheDefault::GetTileIndexString(int tileIndex, int tilesPerFolder)
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

// Get the folder name corresponding to the specified scale index
STRING MgTileCacheDefault::GetScaleIndexFolder(int scaleIndex)
{
    STRING scaleIndexString;
    MgUtil::Int32ToString(scaleIndex, scaleIndexString);
    return SCALE_INDEX_PREFIX + scaleIndexString;
}

// Get the folder name corresponding to the specified tile row
STRING MgTileCacheDefault::GetRowFolder(int tileRow)
{
    return GetFolder(ROW_PREFIX, tileRow, sm_tileRowsPerFolder);
}

STRING MgTileCacheDefault::CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
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

STRING MgTileCacheDefault::CreateFullPath(int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    return CreateFullPath(GetBasePath(), scaleIndex, group, tileColumn, tileRow);
}

STRING MgTileCacheDefault::GetBasePath()
{
    Ptr<MgResourceIdentifier> resId;
    if (NULL != (MgMap*)m_map)
    {
        resId = m_map->GetMapDefinition();
    }
    else if (NULL != m_resourceId && m_resourceId->GetResourceType() == MgResourceType::MapDefinition)
    {
        resId = SAFE_ADDREF((MgResourceIdentifier*)m_resourceId);
    }
    return GetBasePathFromResourceId(resId, sm_path);
}

STRING MgTileCacheDefault::GetBasePathFromResourceId(MgResourceIdentifier* resId, CREFSTRING rootPath)
{
    STRING mapPath;
    if (resId->GetRepositoryType() == MgRepositoryType::Library)
    {
        // for maps in the library repository the path+name is unique
        mapPath  = resId->GetPath();
        mapPath += L"_";
        mapPath += resId->GetName();
    }
    else if (resId->GetRepositoryType() == MgRepositoryType::Session)
    {
        // for maps in the session repository we use the session + path + map name
        mapPath  = resId->GetRepositoryName();
        mapPath += L"_";

        STRING resourcePath  = resId->GetPath();

        if (!resourcePath.empty())
        {
            mapPath += resourcePath;
            mapPath += L"_";
        }

        mapPath += resId->GetName();
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
    STRING basePath = rootPath;
    basePath += mapPath;

    return basePath;
}

// gets the full path to use with the tile cache for the given base path / scale index / group
STRING MgTileCacheDefault::GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
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
STRING MgTileCacheDefault::GetFullPath(int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    return GetFullPath(GetBasePath(), scaleIndex, group, tileColumn, tileRow);
}

// Get the folder name corresponding to the specified tile column
STRING MgTileCacheDefault::GetColumnFolder(int tileColumn)
{
    return GetFolder(COLUMN_PREFIX, tileColumn, sm_tileColumnsPerFolder);
}

// Get the parent folder for a given row or column
STRING MgTileCacheDefault::GetFolder(STRING prefix, int tileIndex, int tilesPerFolder)
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

MgByteReader* MgTileCacheDefault::Get(CREFSTRING path)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (MgFileUtil::PathnameExists(path))
    {
        Ptr<MgByteSource> byteSource = new MgByteSource(path, false);

        if (MgTileParameters::tileFormat == MgImageFormats::Jpeg)
        {
            byteSource->SetMimeType(MgMimeType::Jpeg);
        }
        else if (MgTileParameters::tileFormat == MgImageFormats::Gif)
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

void MgTileCacheDefault::Set(MgByteReader* img, CREFSTRING path)
{
    if (img != NULL)
    {
        Ptr<MgByteSink> byteSink = new MgByteSink(img);

        byteSink->ToFile(path);
    }
}

bool MgTileCacheDefault::IsTileCacheEmpty()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    return sm_mapCache.empty();
}
