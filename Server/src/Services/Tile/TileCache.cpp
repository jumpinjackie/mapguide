//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "TileCache.h"

STRING MgTileCache::sm_lockFolderName = L"Lock";    // name of file folder for locks
STRING MgTileCache::sm_path = L"";

// default constructor
MgTileCache::MgTileCache()
{
    //TODO: It is possible to get a double write on sm_path here.  We need
    //to investigate general mutex use in this class.
    if (sm_path.empty())
    {
        // initialize the tile cache path
        MgConfiguration* pConf = MgConfiguration::GetInstance();

        pConf->GetStringValue(
            MgConfigProperties::TileServicePropertiesSection,
            MgConfigProperties::TileServicePropertyLockFolderName,
            sm_lockFolderName,
            MgConfigProperties::DefaultTileServicePropertyLockFolderName);

        pConf->GetStringValue(
            MgConfigProperties::TileServicePropertiesSection,
            MgConfigProperties::TileServicePropertyTileCachePath,
            sm_path,
            MgConfigProperties::DefaultTileServicePropertyTileCachePath);

        // generate directory location for tile cache
        MgFileUtil::AppendSlashToEndOfPath(sm_path);

        // create directory if it is not already there
        if (!MgFileUtil::PathnameExists(sm_path))
            MgFileUtil::CreateDirectory(sm_path, false);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generate tile and lock pathnames.
///
void MgTileCache::GeneratePathnames(MgResourceIdentifier* mapDef, int scaleIndex,
    CREFSTRING group, int tileColumn, int tileRow,
    STRING& tilePathname, STRING& lockPathname, bool createFullPath)
{
    wchar_t fileName[MAXPATHLEN] = { 0 };
    ::swprintf(fileName, MAXPATHLEN, L"/%d_%d.", tileRow, tileColumn);

    lockPathname = GetBasePath(mapDef);
    tilePathname;

    if (createFullPath)
    {
        tilePathname = CreateFullPath(lockPathname, scaleIndex, group);
    }
    else
    {
        tilePathname = GetFullPath(lockPathname, scaleIndex, group);
    }

    // Generate the tile pathname using format "%ls/%d_%d.png":
    //     <tilePathname> = <fullPath>/<row>_<column>.png
    tilePathname += fileName;
    tilePathname += L"png";

    // Generate the lock pathname using format "%ls/%d_%d.lck":
    //     <lockPathname> = <basePath>/<row>_<column>.lck
    lockPathname += L"/";
    lockPathname += sm_lockFolderName;
    lockPathname += fileName;
    lockPathname += L"lck";
}

void MgTileCache::GeneratePathnames(MgMap* map, int scaleIndex,
    CREFSTRING group, int tileColumn, int tileRow,
    STRING& tilePathname, STRING& lockPathname, bool createFullPath)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();

    GeneratePathnames(mapDef, scaleIndex, group, tileColumn, tileRow,
        tilePathname, lockPathname, createFullPath);
}

// returns any cached tile for the given pathname
MgByteReader* MgTileCache::Get(CREFSTRING tilePathname)
{
    // acquire a read lock - this blocks if a writer holds the lock
    ACE_Read_Guard<ACE_RW_Thread_Mutex> ace_mon(m_rwMutex);

    Ptr<MgByteReader> ret;

    MG_TRY()

    if (MgFileUtil::PathnameExists(tilePathname))
    {
        Ptr<MgByteSource> byteSource = new MgByteSource(tilePathname, false);

        byteSource->SetMimeType(MgMimeType::Png);
        ret = byteSource->GetReader();
    }

    MG_CATCH_AND_RELEASE()

    return ret.Detach();
}

// caches a tile for the given pathname
void MgTileCache::Set(MgByteReader* img, CREFSTRING tilePathname)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_rwMutex);

    if (img != NULL)
    {
        Ptr<MgByteSink> byteSink = new MgByteSink(img);

        byteSink->ToFile(tilePathname);
    }
}

// clears the tile cache for the given map
void MgTileCache::Clear(MgMap* map)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_rwMutex);

    if (map != NULL)
    {
        STRING basePath = GetBasePath(map);

        // delete main map directory
        if (!basePath.empty())
            MgFileUtil::DeleteDirectory(basePath, true, false);
    }
}

// clears the tile cache for the given map
void MgTileCache::Clear(MgResourceIdentifier* mapDef)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_rwMutex);

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
STRING MgTileCache::GetBasePath(MgResourceIdentifier* mapDef)
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
STRING MgTileCache::GetBasePath(MgMap* map)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();
    return GetBasePath(mapDef);
}

// gets the full path to use with the tile cache for the given base path / scale index / group
STRING MgTileCache::GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group)
{
    // Build full path using format "%ls/%d/%ls":
    //     <fullPath> = <basePath>/<scaleIndex>/<group>
    assert(!basePath.empty());
    STRING fullPath = basePath;
    STRING buffer;
    MgUtil::Int32ToString(scaleIndex, buffer);

    fullPath += L"/";
    fullPath += buffer;
    fullPath += L"/";
    fullPath += group;

    return fullPath;
}

// gets the full path to use with the tile cache for the given map definition / scale index / group
STRING MgTileCache::GetFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group)
{
    return GetFullPath(GetBasePath(mapDef), scaleIndex, group);
}

// gets the full path to use with the tile cache for the given map / scale index / group
STRING MgTileCache::GetFullPath(MgMap* map, int scaleIndex, CREFSTRING group)
{
    assert(NULL != map);
    Ptr<MgResourceIdentifier> mapDef = map->GetMapDefinition();
    return GetFullPath(mapDef, scaleIndex, group);
}

STRING MgTileCache::CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group)
{
    assert(!basePath.empty());
    STRING fullPath = basePath;

    // Create base directory if it does not exist.
    MgFileUtil::CreateDirectory(fullPath, false);
    fullPath += L"/";

    // Create lock directory if it does not exist.
    STRING lockPath = fullPath;

    lockPath += sm_lockFolderName;
    MgFileUtil::CreateDirectory(lockPath, false);

    // Create scale directory if it does not exist.
    STRING buffer;
    MgUtil::Int32ToString(scaleIndex, buffer);

    fullPath += buffer;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create group directory if it does not exist.
    fullPath += L"/";
    fullPath += group;
    MgFileUtil::CreateDirectory(fullPath, false);

    return fullPath;
}

STRING MgTileCache::CreateFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group)
{
    return CreateFullPath(GetBasePath(mapDef), scaleIndex, group); 
}

STRING MgTileCache::CreateFullPath(MgMap* map, int scaleIndex, CREFSTRING group)
{
    return CreateFullPath(GetBasePath(map), scaleIndex, group); 
}
