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

#define PATH_LEN 512

// default constructor
MgTileCache::MgTileCache()
{
    // initialize the tile cache path
    MgConfiguration* pConf = MgConfiguration::GetInstance();

    pConf->GetStringValue(MgConfigProperties::TileServicePropertiesSection,
                          MgConfigProperties::TileServicePropertyTileCachePath,
                          m_path,
                          MgConfigProperties::DefaultTileServicePropertyTileCachePath);

    // generate directory location for tile cache
    MgFileUtil::AppendSlashToEndOfPath(m_path);

    // create directory if it is not already there
    if (!MgFileUtil::PathnameExists(m_path))
        MgFileUtil::CreateDirectory(m_path, false);
}


// returns any cached tile for the given map / scale index / group / i / j
MgByteReader* MgTileCache::Get(MgMap* map, int scaleIndex, CREFSTRING group, int i, int j)
{
    // acquire a read lock - this blocks if a writer holds the lock
    ACE_Read_Guard<ACE_RW_Thread_Mutex> ace_mon(m_mutexRW);

    Ptr<MgByteReader> ret;

    if (map != NULL)
    {
        STRING tilePath = MgTileCache::GetFullPath(map, scaleIndex, group);

        // generate full path to tile file using <row,column> format
        // TODO: handle case where path is > PATH_LEN
        wchar_t tmp[PATH_LEN] = { 0 };
        swprintf(tmp, PATH_LEN, L"%ls/%d_%d.png", tilePath.c_str(), j, i);

        // check if the tile exists
        if (MgFileUtil::PathnameExists(tmp))
        {
            Ptr<MgByteSource> bs = new MgByteSource(tmp, false);
            bs->SetMimeType(MgMimeType::Png);
            ret = bs->GetReader();
        }
    }

    return SAFE_ADDREF(ret.p);
}


// caches a tile for the given map / scale index / group / i / j
void MgTileCache::Set(MgByteReader* img, MgMap* map, int scaleIndex, CREFSTRING group, int i, int j)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_mutexRW);

    if (map != NULL && img != NULL)
    {
        // ensure the necessary directories are created
        STRING tilePath = MgTileCache::CreateFullPath(map, scaleIndex, group);

        // generate full path to tile file using <row,column> format
        // TODO: handle case where path is > PATH_LEN
        wchar_t tmp[PATH_LEN] = { 0 };
        swprintf(tmp, PATH_LEN, L"%ls/%d_%d.png", tilePath.c_str(), j, i);

        // save the file
        (MgByteSink(img)).ToFile(tmp);
    }
}


// clears the tile cache for the given map
void MgTileCache::Clear(MgMap* map)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_mutexRW);

    if (map != NULL)
    {
        STRING basePath = MgTileCache::GetBasePath(map);

        // delete main map directory
        if (!basePath.empty())
            MgFileUtil::DeleteDirectory(basePath, true, false);
    }
}


// clears the tile cache for the given map
void MgTileCache::Clear(MgResourceIdentifier* resId)
{
    // acquire a write lock - this blocks if any readers or a writer hold the lock
    ACE_Write_Guard<ACE_RW_Thread_Mutex> ace_mon(m_mutexRW);

    // the resource must be a map definition
    if (resId != NULL && resId->GetResourceType() == MgResourceType::MapDefinition)
    {
        STRING basePath = MgTileCache::GetBasePath(resId);

        // delete main map directory
        if (!basePath.empty())
            MgFileUtil::DeleteDirectory(basePath, true, false);
    }
}


// gets the base path to use with the tile cache for the given map
STRING MgTileCache::GetBasePath(MgMap* map)
{
    Ptr<MgResourceIdentifier> resId = map->GetMapDefinition();
    return GetBasePath(resId);
}


// gets the base path to use with the tile cache for the given map definition resource
STRING MgTileCache::GetBasePath(MgResourceIdentifier* resId)
{
    STRING mapPath;

    // safety check
    assert(resId != NULL);
    if (resId == NULL)
        return mapPath;

    // we should only be dealing with map definitions
    assert(resId->GetResourceType() == MgResourceType::MapDefinition);

    if (resId->GetRepositoryType() == MgRepositoryType::Library)
    {
        // for maps in the library repository the path+name is unique
        mapPath  = resId->GetPath();
        mapPath += L"_";
        mapPath += resId->GetName();
    }
    else if (resId->GetRepositoryType() == MgRepositoryType::Session)
    {
        // for maps in the session repository we use the session name + map name
        mapPath  = resId->GetRepositoryName();
        mapPath += L"_";
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

    // build the base path
    wchar_t tmp[PATH_LEN] = { 0 };
    swprintf(tmp, PATH_LEN, L"%ls%ls", m_path.c_str(), mapPath.c_str());

    return STRING(tmp);
}


// gets the full path to use with the tile cache for the given map / scale index / group
STRING MgTileCache::GetFullPath(MgMap* map, int scaleIndex, CREFSTRING group)
{
    STRING basePath = MgTileCache::GetBasePath(map);

    wchar_t tmp[PATH_LEN] = { 0 };
    swprintf(tmp, PATH_LEN, L"%ls/%d/%ls", basePath.c_str(), scaleIndex, group.c_str());

    return STRING(tmp);
}


STRING MgTileCache::CreateFullPath(MgMap* map, int scaleIndex, CREFSTRING group)
{
    wchar_t tmp[PATH_LEN] = { 0 };

    if (map != NULL)
    {
        STRING basePath = MgTileCache::GetBasePath(map);

        // check main map directory
        MgFileUtil::CreateDirectory(basePath, false);

        // check scale directory
        swprintf(tmp, PATH_LEN, L"%ls/%d", basePath.c_str(), scaleIndex);
        MgFileUtil::CreateDirectory(tmp, false);

        // check group directory
        swprintf(tmp, PATH_LEN, L"%ls/%d/%ls", basePath.c_str(), scaleIndex, group.c_str());
        MgFileUtil::CreateDirectory(tmp, false);
    }

    return STRING(tmp);
}
