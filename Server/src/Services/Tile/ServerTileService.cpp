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

#include "MapGuideCommon.h"
#include "ServerTileService.h"

IMPLEMENT_CREATE_SERVICE(MgServerTileService)

ACE_Recursive_Thread_Mutex MgServerTileService::sm_mutex;
bool MgServerTileService::sm_initialized = false;
MgServerTileService::MapCache MgServerTileService::sm_mapCache;
bool MgServerTileService::sm_renderOnly = false;
INT32 MgServerTileService::sm_creationCutoffTime = 120;     // in seconds
INT32 MgServerTileService::sm_pollingInterval = 1;          // in seconds
INT32 MgServerTileService::sm_mapCacheSize = 10;

MgServerTileService::MgServerTileService() : MgTileService()
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

            MgTileCache::Initialize();
            sm_initialized = true;
        }
    }

    m_tileCache = new MgTileCache();
}


MgServerTileService::~MgServerTileService()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determine if the tile cache is empty.
///
bool MgServerTileService::IsTileCacheEmpty() const
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    return sm_mapCache.empty();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Detect if the tile file has been locked by another thread or process.
///
bool MgServerTileService::DetectTileLockFile(CREFSTRING lockPathname)
{
    bool found = false;
    struct _stat lockFileInfo;

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

MgByteReader* MgServerTileService::GetTile(
    MgResourceIdentifier* mapDefinition,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow,
    INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    if (NULL == mapDefinition || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"5");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

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
            L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Generate tile and lock pathnames.
    m_tileCache->GeneratePathnames(mapDefinition, scaleIndex, baseMapLayerGroupName,
        tileColumn, tileRow, tilePathname, lockPathname, false);

    // If there is a dangling lock file, then attempt to remove it.
    if (DetectTileLockFile(lockPathname))
    {
        // TODO: Handle the exception by displaying a tile with an error message?
        MgFileUtil::DeleteFile(lockPathname, true);
    }

    // try getting the tile from the cache
    ret = m_tileCache->Get(tilePathname);

    // if the reader is NULL then the tile wasn't in the cache and we
    // need to generate it
    while (NULL == ret)
    {
        // Attempt use a cached & serialized MgMap object
        Ptr<MgMemoryStreamHelper> cachedMap;
        STRING mapString = mapDefinition->ToString();
        Ptr<MgMap> map;

        // Protect the serialized MgMap cache with a mutex.  Stream reading is not
        // thread safe so we need to deserialize the map within the mutex to ensure
        // that a Rewind() is not called in the middle of a Deserialize()
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
            ret = m_tileCache->Get(tilePathname);

            if (NULL != ret)
            {
                break;
            }

            // Create the lock file and close it right away.
            m_tileCache->CreateFullPath(mapDefinition, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
            lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

            if (NULL == lockFile)
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgServerTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToOpenTileFile", NULL);
            }
            else
            {
                ACE_OS::fclose(lockFile);
            }

            MapCache::const_iterator iter = sm_mapCache.find(mapString);
            if (sm_mapCache.end() != iter)
            {
                cachedMap = SAFE_ADDREF((*iter).second);
            }
            else
            {
                map = new MgMap();
                map->Create(resourceService, mapDefinition, mapString);
                cachedMap = new MgMemoryStreamHelper();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map->Serialize(stream);
                if ((INT32)sm_mapCache.size() >= sm_mapCacheSize)
                {
                    ClearMapCache(L"");
                }
                sm_mapCache[mapString] = SAFE_ADDREF((MgMemoryStreamHelper*)cachedMap);
            }


            if (!map)
            {
                cachedMap->Rewind();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map = new MgMap();
                map->Deserialize(stream);
            }
        }

        double scale = map->GetFiniteDisplayScaleAt(scaleIndex);
        map->SetViewScale(scale);

        // Render the tile and cache it.
        ret = GetTile(tilePathname, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
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


MgByteReader* MgServerTileService::GetTile(MgMap* map,
                                           CREFSTRING baseMapLayerGroupName,
                                           INT32 tileColumn,
                                           INT32 tileRow)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    if (NULL == map || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
    {
        throw new MgInvalidMapDefinitionException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Detect the lock file to see if another thread is creating the tile file.
    m_tileCache->GeneratePathnames(map, scaleIndex, baseMapLayerGroupName,
        tileColumn, tileRow, tilePathname, lockPathname, false);

    // If there is a dangling lock file, then attempt to remove it.
    if (DetectTileLockFile(lockPathname))
    {
        // TODO: Handle the exception by displaying a tile with an error message?
        MgFileUtil::DeleteFile(lockPathname, true);
    }

    // try getting the tile from the cache
    ret = m_tileCache->Get(tilePathname);

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
            ret = m_tileCache->Get(tilePathname);

            if (NULL != ret)
            {
                break;
            }

            // Create the lock file and close it right away.
            m_tileCache->CreateFullPath(map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
            lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

            if (NULL == lockFile)
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgServerTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToOpenTileFile", NULL);
            }
            else
            {
                ACE_OS::fclose(lockFile);
            }
        }

        // Render the tile and cache it.
        ret = GetTile(tilePathname, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
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

MgByteReader* MgServerTileService::GetTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
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
        img = svcRendering->RenderTile(map, baseMapLayerGroupName, tileColumn, tileRow);

        // cache the tile
        if (!sm_renderOnly)
        {
            m_tileCache->Set(img, tilePathname);

            // rewind the reader since setting the tile advances it to the end
            if (img)
            {
                img->Rewind();
            }
        }
    }

    return img.Detach();
}

void MgServerTileService::SetTile(MgByteReader* img,
                                  MgMap* map,
                                  INT32 scaleIndex,
                                  CREFSTRING baseMapLayerGroupName,
                                  INT32 tileColumn,
                                  INT32 tileRow)
{
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_TRY()

    if (NULL == img || NULL == map || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(L"MgServerTileService.SetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // Generate tile and lock pathnames.
    m_tileCache->GeneratePathnames(map, scaleIndex, baseMapLayerGroupName,
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

            throw new MgFileIoException(L"MgServerTileService.SetTile",
                __LINE__, __WFILE__, &arguments, L"MgUnableToOpenTileFile", NULL);
        }
        else
        {
            ACE_OS::fclose(lockFile);
        }
    }

    // cache the tile
    m_tileCache->Set(img, tilePathname);

    MG_CATCH(L"MgServerTileService.SetTile")

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    MG_THROW()
}


void MgServerTileService::ClearCache(MgMap* map)
{
    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgServerTileService.ClearCache", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgResourceIdentifier> resourceId = map->GetMapDefinition();
    ClearMapCache(resourceId->ToString());

    m_tileCache->Clear(map);

    MG_CATCH_AND_THROW(L"MgServerTileService.ClearCache")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Any tile cache associated with the specified Map Definition resources
/// will be cleared.
///
bool MgServerTileService::NotifyResourcesChanged(MgSerializableCollection* resources, bool strict)
{
    bool success = true;

    if (NULL != resources)
    {
        INT32 numResources = resources->GetCount();

        if (numResources > 0)
        {
            for (INT32 i = 0; i < numResources; ++i)
            {
                Ptr<MgSerializable> serializableObj = resources->GetItem(i);
                MgResourceIdentifier* resource =
                    dynamic_cast<MgResourceIdentifier*>(serializableObj.p);
                ACE_ASSERT(NULL != resource);

                if (NULL != resource && resource->IsResourceTypeOf(MgResourceType::MapDefinition))
                {
                    MG_TRY()

                    // clear any cached mgmap objects
                    ClearMapCache(resource->ToString());

                    // clear any tile cache associated with this map
                    m_tileCache->Clear(resource);

                    MG_CATCH(L"MgServerTileService.NotifyResourcesChanged")

                    if (NULL != mgException.p)
                    {
                        success = false;

                        if (strict)
                        {
                            MG_THROW();
                        }
                        else
                        {
                            MgLogManager* logManager = MgLogManager::GetInstance();
                            ACE_ASSERT(NULL != logManager);
                            logManager->LogSystemErrorEntry(mgException.p);
                        }
                    }
                }
            }
        }
    }

    return success;
}

void MgServerTileService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}

void MgServerTileService::ClearMapCache(CREFSTRING mapDefinition)
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
            message = resources->GetMessage(MgResources::ErrorDescription, L"MgMapCacheCleared", NULL);
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

INT32 MgServerTileService::GetDefaultTileSizeX()
{
    return MgTileParameters::tileWidth;
}

INT32 MgServerTileService::GetDefaultTileSizeY()
{
    return MgTileParameters::tileHeight;
}
