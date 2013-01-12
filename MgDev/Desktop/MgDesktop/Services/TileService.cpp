#include "MgDesktop.h"
#include "TileService.h"

ACE_Recursive_Thread_Mutex MgdTileService::sm_MgdMutex;
bool MgdTileService::sm_initialized = false;
MgdTileService::MapCache MgdTileService::sm_mapCache;
bool MgdTileService::sm_renderOnly = false;
INT32 MgdTileService::sm_creationCutoffTime = 120;     // in seconds
INT32 MgdTileService::sm_pollingInterval = 1;          // in seconds
INT32 MgdTileService::sm_mapCacheSize = 10;

MgdTileService::MgdTileService() : MgService()
{
    if (!sm_initialized)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex));

        if (!sm_initialized)
        {
            MgConfiguration* configuration = MgConfiguration::GetInstance();

            configuration->GetBoolValue(
                MgdConfigProperties::TileServicePropertiesSection,
                MgdConfigProperties::TileServicePropertyRenderOnly,
                sm_renderOnly,
                MgdConfigProperties::DefaultTileServicePropertyRenderOnly);

            configuration->GetIntValue(
                MgdConfigProperties::TileServicePropertiesSection,
                MgdConfigProperties::TileServicePropertyCreationCutoffTime,
                sm_creationCutoffTime,
                MgdConfigProperties::DefaultTileServicePropertyCreationCutoffTime);

            configuration->GetIntValue(
                MgdConfigProperties::TileServicePropertiesSection,
                MgdConfigProperties::TileServicePropertyPollingInterval,
                sm_pollingInterval,
                MgdConfigProperties::DefaultTileServicePropertyPollingInterval);

            configuration->GetIntValue(
                MgdConfigProperties::TileServicePropertiesSection,
                MgdConfigProperties::TileServicePropertyTiledMapCacheSize,
                sm_mapCacheSize,
                MgdConfigProperties::DefaultTileServicePropertyTiledMapCacheSize);

            MgdTileCache::Initialize();
            sm_initialized = true;
        }
    }

    m_tileCache = new MgdTileCache();
}


MgdTileService::~MgdTileService()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determine if the tile cache is empty.
///
bool MgdTileService::IsTileCacheEmpty() const
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex, false));

    return sm_mapCache.empty();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Detect if the tile file has been locked by another thread or process.
///
bool MgdTileService::DetectTileLockFile(CREFSTRING lockPathname)
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
// Create tilename from mapDefinition, scaleIndex, row, and column.
// Remove lockfile, look for the tile in the cache, if not in cache create
// lockfile and look for map in mapcache.
MgByteReader* MgdTileService::GetTile(MgResourceIdentifier* mapDefinition,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow,
                                     INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_LOG_OPERATION_MESSAGE(L"GetTile");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 5);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapDefinition) ? L"MgResourceIdentifier" : mapDefinition->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileColumn);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileRow);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(scaleIndex);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    if (NULL == mapDefinition || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(L"MgdTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"5");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgdTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // get the service from our helper method
    Ptr<MgResourceService> resourceService = GetResourceServiceForMapDef(mapDefinition,
                                            L"MgdTileService.GetTile");
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
        Ptr<MgdMap> map;

        // Protect the serialized MgMap cache with a MgdMutex.  Stream reading is not
        // thread safe so we need to deserialize the map within the MgdMutex to ensure
        // that a Rewind() is not called in the middle of a Deserialize().
        // Lockfile test and creation is in same protected scope.
        {
            // Attempt to lock the tile file.
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex, NULL));

            // Bail out if the tile file has been locked for so long.
            if (DetectTileLockFile(lockPathname))
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgdTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToLockTileFile", NULL);
            }

            // try getting the tile from the cache
            ret = m_tileCache->Get(tilePathname);

            if (NULL != ret)
            {
                break;  // tile was in tileCache .. done.
            }

            // Create the lock file and close it right away.
            m_tileCache->CreateFullPath(mapDefinition, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
            lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(lockPathname), ACE_TEXT("wb"));

            if (NULL == lockFile)
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgdTileService.GetTile",
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
                map = new MgdMap();
                map->Deserialize(stream);
            }
            else
            {
                /*Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
                Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
                siteConn->Open(userInfo);
                map = new MgMap(siteConn);
                map->Create(resourceService, mapDefinition, mapString);*/
                map = new MgdMap(mapDefinition, mapString);
                cachedMap = new MgMemoryStreamHelper();
                Ptr<MgStream> stream = new MgStream(cachedMap);
                map->Serialize(stream);
                if ((INT32)sm_mapCache.size() >= sm_mapCacheSize)
                {
                    ClearMapCache(L"");
                }
                sm_mapCache[mapString] = SAFE_ADDREF((MgMemoryStreamHelper*)cachedMap);
            }
        }   // end of MgdMutex scope

        double scale = map->GetFiniteDisplayScaleAt(scaleIndex);
        map->SetViewScale(scale);

        // Render the tile and cache it.
        ret = GetTile(tilePathname, map, scaleIndex, baseMapLayerGroupName, tileColumn, tileRow);
        break;
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdTileService::GetTile")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// look for the tile in the tilecache first
MgByteReader* MgdTileService::GetTile(MgdMap* map,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow)
{
    Ptr<MgByteReader> ret;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;

    MG_LOG_OPERATION_MESSAGE(L"GetTile");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId;
    if (NULL != map)
        mapId = map->GetMapDefinition();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgResourceIdentifier" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileColumn);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileRow);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    if (NULL == map || baseMapLayerGroupName.empty())
    {
        throw new MgNullArgumentException(L"MgdTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
    {
        throw new MgInvalidMapDefinitionException(L"MgdTileService.GetTile",
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
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex, NULL));

            // Bail out if the tile file has been locked for so long.
            if (DetectTileLockFile(lockPathname))
            {
                MgStringCollection arguments;
                arguments.Add(lockPathname);

                throw new MgFileIoException(L"MgdTileService.GetTile",
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

                throw new MgFileIoException(L"MgdTileService.GetTile",
                    __LINE__, __WFILE__, &arguments, L"MgUnableToOpenLockFile", NULL);
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

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdTileService::GetTile")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// render a tile and store it in the cache
MgByteReader* MgdTileService::GetTile(CREFSTRING tilePathname, MgdMap* map, INT32 scaleIndex,
    CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow)
{
    Ptr<MgByteReader> img;

    // get a rendering service instance
    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    Ptr<MgdRenderingService> svcRendering = static_cast<MgdRenderingService*>(fact->CreateService(MgServiceType::RenderingService));
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


///////////////////////////////////////////////////////////////////////////////
// take a tile image and store it in the tilecache using lockfiles
void MgdTileService::SetTile(MgByteReader* img,
                                  MgdMap* map,
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
        throw new MgNullArgumentException(L"MgdTileService.SetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (scaleIndex < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(scaleIndex, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgdTileService.GetTile",
            __LINE__, __WFILE__, &arguments, L"MgInvalidScaleIndex", NULL);
    }

    // Generate tile and lock pathnames.
    m_tileCache->GeneratePathnames(map, scaleIndex, baseMapLayerGroupName,
        tileColumn, tileRow, tilePathname, lockPathname, true);

    {
        // Attemp to lock the tile file.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex));

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

            throw new MgFileIoException(L"MgdTileService.SetTile",
                __LINE__, __WFILE__, &arguments, L"MgUnableToOpenLockFile", NULL);
        }
        else
        {
            ACE_OS::fclose(lockFile);
        }
    }

    // cache the tile
    m_tileCache->Set(img, tilePathname);

    MG_CATCH(L"MgdTileService.SetTile")

    if (NULL != lockFile)
    {
        MgFileUtil::DeleteFile(lockPathname, false);
    }

    MG_THROW()
}


///////////////////////////////////////////////////////////////////////////////
// accessor method for resource service
MgResourceService* MgdTileService::GetResourceServiceForMapDef(MgResourceIdentifier* mapDefinition,
                                                                    CREFSTRING funcName)
{
    // Get the service from service manager
    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    Ptr<MgResourceService> resourceService = static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    assert(NULL != resourceService);
    /*
    if (!resourceService->HasPermission(mapDefinition, MgResourcePermission::ReadOnly))
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

        MgStringCollection arguments;
        arguments.Add(mapDefinition->ToString());

        throw new MgPermissionDeniedException(
            funcName, __LINE__, __WFILE__, &arguments, L"", NULL);
    }*/
    return resourceService.Detach();
}


///////////////////////////////////////////////////////////////////////////////
void MgdTileService::ClearCache(MgdMap* map)
{
    MG_LOG_OPERATION_MESSAGE(L"ClearCache");

    MG_TRY()

    Ptr<MgResourceIdentifier> resourceId;
    if (NULL != map)
        resourceId = map->GetMapDefinition();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resourceId) ? L"MgResourceIdentifier" : resourceId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    if (NULL == map)
        throw new MgNullArgumentException(L"MgdTileService.ClearCache", __LINE__, __WFILE__, NULL, L"", NULL);

    ClearMapCache(resourceId->ToString());

    m_tileCache->Clear(map);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdTileService::ClearCache")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Any tile cache associated with the specified Map Definition resources
/// will be cleared.
///
bool MgdTileService::NotifyResourcesChanged(MgSerializableCollection* resources, bool strict)
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

                    MG_CATCH(L"MgdTileService.NotifyResourcesChanged")

                    if (NULL != mgException.p)
                    {
                        success = false;

                        if (strict)
                        {
                            MG_THROW();
                        }
                        /*
                        else
                        {
                            MgdLogManager* logManager = MgdLogManager::GetInstance();
                            ACE_ASSERT(NULL != logManager);
                            logManager->LogSystemErrorEntry(mgException.p);
                        }*/
                    }
                }
            }
        }
    }

    return success;
}


///////////////////////////////////////////////////////////////////////////////
void MgdTileService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}


///////////////////////////////////////////////////////////////////////////////
void MgdTileService::ClearMapCache(CREFSTRING mapDefinition)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_MgdMutex));
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
INT32 MgdTileService::GetDefaultTileSizeX()
{
    INT32 ret = 0;
    MG_LOG_OPERATION_MESSAGE(L"GetDefaultTileSizeX");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 0);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    
    ret = MgdTileParameters::tileWidth;

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdTileService::GetDefaultTileSizeX")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
INT32 MgdTileService::GetDefaultTileSizeY()
{
    INT32 ret = 0;
    MG_LOG_OPERATION_MESSAGE(L"GetDefaultTileSizeY");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 0);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    ret = MgdTileParameters::tileHeight;

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdTileService::GetDefaultTileSizeY")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret;
}
