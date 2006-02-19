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

#include "AceCommon.h"
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
    // If another thread is currently in a call to Set or Clear, then the "set"
    // lock is enabled and we must wait for that call to finish.  We do this by
    // acquiring the "set" lock.
    ACE_Guard<ACE_Recursive_Thread_Mutex> ace_monSet(m_mutexSet, 1);

    // At this point no other calls to Set or Clear are active.  But since we
    // acquired the "set" lock above we're also blocking other calls to Get,
    // and we don't want that.  We therefore need to release the "set" lock.
    // But as soon as we release it then calls to Set and Clear can once again
    // happen.  We temporarily block them by enabling the "get" lock.  But only
    // do a tryacquire on this lock so that other calls to Get aren't blocked.
    ACE_Guard<ACE_Recursive_Thread_Mutex> ace_monGet(m_mutexGet, 0);

    // Now that the "get" lock is enabled we can safely release the "set' lock.
    ace_monSet.release();

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
            ret = bs->GetReader();
        }
    }

    return SAFE_ADDREF(ret.p);
}


// caches a tile for the given map / scale index / group / i / j
void MgTileCache::Set(MgByteReader* img, MgMap* map, int scaleIndex, CREFSTRING group, int i, int j)
{
    // acquire the set lock - this blocks subsequent Get, Set, and Clear calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monSet, m_mutexSet));

    // acquire the get lock - this allows completion of current Get calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monGet, m_mutexGet));

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
    // acquire the set lock - this blocks subsequent Get, Set, and Clear calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monSet, m_mutexSet));

    // acquire the get lock - this allows completion of current Get calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monGet, m_mutexGet));

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
    // acquire the set lock - this blocks subsequent Get, Set, and Clear calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monSet, m_mutexSet));

    // acquire the get lock - this allows completion of current Get calls
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_monGet, m_mutexGet));

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
    STRING mapPath;

    Ptr<MgResourceIdentifier> resId = map->GetMapDefinition();
    if (resId->GetRepositoryType() == MgRepositoryType::Library)
    {
        // for maps in the library repository the path+name is unique
        mapPath  = resId->GetPath();
        mapPath += L"_";
        mapPath += resId->GetName();
    }
    else
    {
        // for maps in the session repository the name alone is not
        // unique - we need to append the session ID
        mapPath  = resId->GetName();
        mapPath += L"_";
        mapPath += map->GetObjectId();
    }

    // for safety
    mapPath = MgUtil::ReplaceString(mapPath, L"/", L"_");
    mapPath = MgUtil::ReplaceString(mapPath, L":", L"_");

    // build the base path
    wchar_t tmp[PATH_LEN] = { 0 };
    swprintf(tmp, PATH_LEN, L"%ls%ls", m_path.c_str(), mapPath.c_str());

    return STRING(tmp);
}


// gets the base path to use with the tile cache for the given map definition resource
STRING MgTileCache::GetBasePath(MgResourceIdentifier* resId)
{
    STRING mapPath;

    if (resId->GetRepositoryType() == MgRepositoryType::Library)
    {
        // for maps in the library repository the path+name is unique
        mapPath  = resId->GetPath();
        mapPath += L"_";
        mapPath += resId->GetName();
    }
    else
    {
        // TODO - return empty string for now
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
