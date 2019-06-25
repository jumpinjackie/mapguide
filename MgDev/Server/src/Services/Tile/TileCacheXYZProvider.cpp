//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "TileCacheXYZProvider.h"

ACE_Recursive_Thread_Mutex MgTileCacheXYZProvider::sm_MetaTileMutex;

MgTileCacheXYZProvider::MgTileCacheXYZProvider(MgResourceIdentifier* tileSetId,
                                               CREFSTRING path,
                                               CREFSTRING format,
                                               bool bRenderOnly,
                                               double tileExtentOffset,
                                               INT32 metaTileFactor,
                                               INT32 metaTileLockMethod,
                                               INT32 retinaScale)
{
    m_tilesetId = SAFE_ADDREF(tileSetId);
    m_path = path;
    m_format = format;
    m_renderOnly = bRenderOnly;
    m_tileExtentOffset = tileExtentOffset;
    m_metaTileFactor = metaTileFactor;
    m_metaTileLockMethod = metaTileLockMethod;
    m_retinaScale = retinaScale;
}

MgTileCacheXYZProvider::~MgTileCacheXYZProvider()
{

}

bool MgTileCacheXYZProvider::IsTileImageFormat()
{
    return m_format == L"UTFGRID";
}

MgByteReader* MgTileCacheXYZProvider::GetTile(CREFSTRING baseMapLayerGroupName,
                                              INT32 tileColumn,
                                              INT32 tileRow,
                                              INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;
    MG_TRY()

        if (IsTileImageFormat() && m_metaTileFactor > 1)
            ret = GetMetatileForResource(m_tilesetId, baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);
        else
            ret = GetTileForResource(m_tilesetId, baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgTileCacheXYZProvider.GetTile")
    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// render a tile and store it in the cache
MgByteReader* MgTileCacheXYZProvider::RenderAndCacheTile(CREFSTRING tilePathname, MgMap* map, INT32 scaleIndex,
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
        // Test for non-image formats first
        if (m_format == L"UTFGRID")
        {
            // generate the tile
            img = svcRendering->RenderTileUTFGrid(map, baseMapLayerGroupName, tileRow, tileColumn, scaleIndex, map->GetDisplayDpi());
        }
        else //Assume it must be image-based at this point
        {
            // generate the tile
            img = svcRendering->RenderTileXYZ(map, baseMapLayerGroupName, tileRow, tileColumn, scaleIndex, map->GetDisplayDpi(), m_format, m_tileExtentOffset, m_retinaScale);
        }
        // cache the tile
        if (!m_renderOnly)
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

INT32 MgTileCacheXYZProvider::GetDefaultTileSizeX()
{
    return 256 * m_retinaScale;
}

INT32 MgTileCacheXYZProvider::GetDefaultTileSizeY()
{
    return 256 * m_retinaScale;
}

STRING MgTileCacheXYZProvider::GetTileFormat()
{
    return m_format;
}

STRING MgTileCacheXYZProvider::GetBasePath()
{
    return GetBasePathFromResourceId(m_tilesetId, m_path);
}

STRING MgTileCacheXYZProvider::GetTileFileExtension()
{
    if (m_format == MgImageFormats::Jpeg)
    {
        return L"jpg";
    }
    else if (m_format == MgImageFormats::Gif)
    {
        return L"gif";
    }
    else if (m_format == L"UTFGRID")
    {
        return L"utfgrid";
    }
    else
    {
        return L"png";
    }
}

STRING MgTileCacheXYZProvider::CreateFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    assert(!basePath.empty());
    STRING fullPath = basePath;

    // Create base directory if it does not exist.
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create group directory if it does not exist.
    fullPath += L"/";
    fullPath += group;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create z directory if it does not exist.
    fullPath += L"/";
    STRING sZ;
    MgUtil::Int32ToString(scaleIndex, sZ);
    fullPath += sZ;
    MgFileUtil::CreateDirectory(fullPath, false);

    // Create x directory if it does not exist.
    fullPath += L"/";
    STRING sX;
    MgUtil::Int32ToString(tileRow, sX);
    fullPath += sX;
    MgFileUtil::CreateDirectory(fullPath, false);

    return fullPath;
}

// gets the full path to use with the tile cache for the given base path / scale index / group
STRING MgTileCacheXYZProvider::GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow)
{
    // Build full path
    //     <fullPath> = <basePath>/<group>/<z>/<x>/<y>.<format>
    assert(!basePath.empty());
    STRING fullPath = basePath;

    fullPath += L"/";
    fullPath += group;

    fullPath += L"/";
    STRING sZ;
    MgUtil::Int32ToString(scaleIndex, sZ);
    fullPath += sZ;

    fullPath += L"/";
    STRING sX;
    MgUtil::Int32ToString(tileRow, sX);
    fullPath += sX;

    return fullPath;
}

// Get the filename corresponding to the specified row and column.
// No file extension is added.
STRING MgTileCacheXYZProvider::GetTileName(int tileRow, int tileColumn)
{
    //row (x) is already part of the parent directory, so column (y) is the tile name
    STRING ret;
    MgUtil::Int32ToString(tileColumn, ret);
    return ret;
}

MgByteReader* MgTileCacheXYZProvider::GetMetatileForResource(MgResourceIdentifier* resource,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow,
    INT32 scaleIndex)
{
    Ptr<MgByteReader> returnedSubtile;
    FILE* lockFile = NULL;
    STRING tilePathname, lockPathname;
    // metatiling vars
    FILE* subTileLockFile[MG_METATILE_MAX_FACTOR][MG_METATILE_MAX_FACTOR];
    STRING subTileLockPathname[MG_METATILE_MAX_FACTOR][MG_METATILE_MAX_FACTOR], subTilePathname[MG_METATILE_MAX_FACTOR][MG_METATILE_MAX_FACTOR];
    STRING metaTileLockPathname, metaTilePathname;
    int maxX = -1, maxY = -1, subTileX = -1, subTileY = -1, metaTileColumn = -1, metaTileRow = -1;
    bool cacheHit = false;
    MgMetatileLockUtil metaLock;

    MG_TRY()

        // Generate tile and lock pathnames for later use
        //m_tileCache->GeneratePathnames(mapDefinition, scaleIndex, baseMapLayerGroupName,
        //    tileColumn, tileRow, tilePathname, lockPathname, false);
        GeneratePathNames(scaleIndex, baseMapLayerGroupName, tileColumn, tileRow, tilePathname, lockPathname, false);

    subTileX = subTileY = 0;
    metaTileColumn = tileColumn;
    metaTileRow = tileRow;

    /*
    ////// compute additional names and control variables to deal with meta tiles:
    // a meta tile is m_metaTileFactor^2 larger than the requested subtile so there are less tiles computed per map
    // the metatile coords are computed from original coords modulo metatilesize
    // e.g. with metatile=2 4 tiles (0,0)(0,1)(1,0)(1,1) get mapped to metatile(0,0)
    subTileX = abs(tileColumn % m_metaTileFactor);
    subTileY = abs(tileRow % m_metaTileFactor);
    // determine left top corner of metaTile having m_metaTileFactor**2 subtiles
    metaTileColumn = tileColumn - subTileX;
    metaTileRow = tileRow - subTileY;
    */
    maxX = maxY = m_metaTileFactor;  // init control vars
    //// iterate subtiles and create all pathnames of the created tiles
    for (int y = 0; y < maxY; y++)    // subtile rows
    {
        for (int x = 0; x < maxX; x++)    // subtile columns
        {   // Generate tile and lock pathnames (locknames are not used)
            subTileLockFile[x][y] = NULL;
            //m_tileCache->GeneratePathnames(mapDefinition, scaleIndex, baseMapLayerGroupName,
            //    metaTileColumn + x, metaTileRow + y, subTilePathname[x][y], subTileLockPathname[x][y], false);

            GeneratePathNames(scaleIndex, baseMapLayerGroupName,
                metaTileColumn + x, metaTileRow + y, subTilePathname[x][y], subTileLockPathname[x][y], false);
        }
    }

    // Generate tile and lock pathnames for meta tile in the larger grid mod xx 
    //m_tileCache->GeneratePathnames(mapDefinition, scaleIndex, baseMapLayerGroupName,
    //    metaTileColumn, metaTileRow, metaTilePathname, metaTileLockPathname, false);
    GeneratePathNames(scaleIndex, baseMapLayerGroupName,
        metaTileColumn, metaTileRow, metaTilePathname, metaTileLockPathname, false);

    // m_metaTileLockMethod == 0 is using 1 remapped metalockfile for all subtiles
    // m_metaTileLockMethod > 0 is using 1 an ace condition mapped by metatilename
    if (m_metaTileLockMethod)
    {
#ifdef _DEBUG
        {
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: " << "WaitForLock("
                << tileRow << L", " << tileColumn << L") (" << metaTilePathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
        }
#endif
        metaLock.WaitForLock(metaTilePathname);
    }
    else // this is the same locking strategie as with single tiles but using one mapped metatile locking all subtile requests
    {
        bool fileExist = DetectTileLockFile(metaTileLockPathname);
        if (fileExist)
        {   // TODO: Handle the exception by displaying a tile with an error message?
            MgFileUtil::DeleteFile(metaTileLockPathname, true);
#ifdef _DEBUG
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") METATILE: " << (fileExist ? L"DeletedDanglingLockfile(" : L"NoLockfile(")
                << metaTileRow << L", " << metaTileColumn << L") " << metaTileLockPathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        }
    }
    // try getting the tile from the cache first time
    cacheHit = returnedSubtile = Get(tilePathname);
#ifdef _DEBUG
    std::wstringstream text;
    text << L"(" << ACE_OS::thr_self() << (cacheHit ? L") CACHEHIT" : L") CACHEMISS") << L": GetTile("
        << tileRow << L", " << tileColumn << L") " << tilePathname << L"\n";
    ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
    // if the reader is NULL then the tile wasn't in the cache and we need to generate it
    while (NULL == returnedSubtile) // while for if to be able to use break to skip to end;
    {
        Ptr<MgMap> map;

        // Protect the serialized MgMap cache with a mutex.  Stream reading is not
        // thread safe so we need to deserialize the map within the mutex to ensure
        // that a Rewind() is not called in the middle of a Deserialize().
        // Lockfile test and creation is in same protected scope.
        { // ------------------------ Locking Context
            // Attempt to get the mapcache mutex .
            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_MetaTileMutex, NULL));
            if (m_metaTileLockMethod)
            {
                // we DONT need the second lock test here in a signal/wait algorithm
    //          m_tileCache->WaitForLock(metaTilePathname);
            }
            else
            {
                if (DetectTileLockFile(metaTileLockPathname)) //
                {
                    MgStringCollection arguments;
                    arguments.Add(metaTileLockPathname);
                    throw new MgFileIoException(L"MgServerTileService.GetTile",
                        __LINE__, __WFILE__, &arguments, L"MgUnableToLockMetaTileFile", NULL);
                }
            }

            // try getting the tile from the cache second time ???????????????????????????????????????????
            cacheHit = returnedSubtile = Get(tilePathname);

            if (NULL != returnedSubtile)
            {
#ifdef _DEBUG
                std::wstringstream text;
                text << L"(" << ACE_OS::thr_self() << L") CACHEHIT2: GetTile(" << tileRow << L", " << tileColumn << L") " << tilePathname << L"\n";
                ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif        
                break;  /////////////////////////////////// tile was in tileCache .. done.
            }
            else {
#ifdef _DEBUG
                std::wstringstream text;
                text << L"(" << ACE_OS::thr_self() << L") CACHEMISS2: GetTile(" << tileRow << L", " << tileColumn << L") " << tilePathname << L"\n";
                ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
            }
            // Create the directory structure all subtiles!
            for (int y = 0; y < maxY; y++)    // rows
            {
                for (int x = 0; x < maxX; x++)    // columns
                {
                    //m_tileCache->CreateFullPath(mapDefinition, scaleIndex, baseMapLayerGroupName, 
                    //    metaTileColumn + x, metaTileRow + y);
                    CreateFullPath(GetBasePath(), scaleIndex, baseMapLayerGroupName, metaTileColumn + x, metaTileRow + y);
                }
            }
            // ==================================================  LOCKING
            if (m_metaTileLockMethod)
            {
                metaLock.LockTile(metaTilePathname);    // ace lock once per metatile
            }
            else
            {
                // Create the lock file 
                lockFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(metaTileLockPathname), ACE_TEXT("wb"));

                if (NULL == lockFile)
                {
                    MgStringCollection arguments;
                    arguments.Add(metaTileLockPathname);
                    throw new MgFileIoException(L"MgServerTileService.GetTile",
                        __LINE__, __WFILE__, &arguments, L"MgUnableToCreateLockFile", NULL);
                }
                else
                {
                    ACE_OS::fclose(lockFile); // and close it right away.
#ifdef _DEBUG
                    std::wstringstream text;
                    text << L"(" << ACE_OS::thr_self() << L") METATILE: CreatedLockfile(" << metaTileRow << L", " << metaTileColumn << L") "
                        << metaTileLockPathname << L")\n";
                    ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
                }
            }
            //=========================================================================
            GetMapFromDefinition(resource, -1 /* Don't set view scale from index for XYZ*/, map, false);
        }   // end of mutex scope

        // Render the larger meta tile but do not cache it yet! (m_metaTileFactor [prohibits caching in GetTile)
        STRING metaTileName = L"META" + metaTilePathname;
#ifdef _DEBUG
        std::wstringstream text;
        text << L"(" << ACE_OS::thr_self() << L") METATILE: RenderMetaTile(" << metaTileRow << L", " << metaTileColumn << L") " << metaTileName << " using map 0x" << map << L"\n";
        ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        _ASSERT(NULL != serviceMan);
        Ptr<MgRenderingService> svcRendering = dynamic_cast<MgRenderingService*>(
            serviceMan->RequestService(MgServiceType::RenderingService));
        _ASSERT(NULL != svcRendering);

        INT32 tileDpi = map->GetDisplayDpi();
        Ptr<MgMetatile> metaTile = svcRendering->RenderMetatileXYZ(map, baseMapLayerGroupName, metaTileRow, metaTileColumn, scaleIndex, tileDpi, m_format, m_tileExtentOffset, m_metaTileFactor, m_retinaScale);
#ifdef _DEBUG
        Ptr<MgByteReader> mtContent = metaTile->GetImage();
        INT32 metaTileLen = mtContent->GetLength();
#endif
        // splitup the meta tiles
        for (int y = 0; y < maxY; y++)    // rows
        {
            for (int x = 0; x < maxX; x++)    // columns
            {
                // Collect the subtile from the metatile (we flip x/y for XYZ)
                Ptr<MgByteReader> img = svcRendering->RenderTileFromMetaTile(map, metaTile, x, y);
#ifdef _DEBUG
                INT32 tileLen = img->GetLength();
#endif          
                if ((subTileX == x) && (subTileY == y))
                    returnedSubtile = img;       // keep pointer for subtile which has triggered the metatile in the current thread
                _ASSERT(x < maxX && y < maxY);
#ifdef _DEBUG
                std::wstringstream text;    // streams for more compact debug statements
                text << L"(" << ACE_OS::thr_self();
                text << L") RenderTileFromMetaTile dim:" << 256 << L"x" << 256;
                text << L" format:" << m_format << L" subTile(" << x << L"," << y;
                text << L") metaTile(" << metaTileRow << L"," << metaTileColumn << L") " << metaTilePathname.c_str() << L"\n";
                ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
                STRING tilePath = subTilePathname[y][x];
                Set(img, tilePath);   // store all subtiles in tile cache

                // rewind the reader since setting the tile advances it to the end
                if (img)
                {
                    img->Rewind();
                }
#ifdef _DEBUG
                {
                    std::wstringstream text;
                    text << L"(" << ACE_OS::thr_self() << L") METATILE: " << (img ? L"SetTile" : L"NoTileReturned");
                    text << "FromMetaTile(" << tileRow << L", " << tileColumn << L") ";
                    text << tilePath << L" S(" << x << L", " << y << L")\n";
                    ACE_DEBUG((LM_DEBUG, text.str().c_str()));
                }
#endif
            }
        }
        break;
    }   // end of gettile loop end MgMap scope

    MG_CATCH(L"MgServerTileService.GetTile")

        // remove all lockfiles or conditions
        if (m_metaTileLockMethod)
        {
            metaLock.ClearLock(metaTilePathname);
        }
        else
        {
            if (NULL != lockFile)
            {
                MgFileUtil::DeleteFile(metaTileLockPathname, false);
                {
#ifdef _DEBUG
                    std::wstringstream text;
                    text << L"(" << ACE_OS::thr_self() << L") METATILE: DeletedLockfile(" << metaTileRow << L", " << metaTileColumn << L") "
                        << metaTileLockPathname << L"\n";
                    ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
                }
            }
        }

    MG_THROW()

        if (!cacheHit)
        {
#ifdef _DEBUG
            std::wstringstream text;
            text << L"(" << ACE_OS::thr_self() << L") Rendered: GetTILE(" << tileRow << L", " << tileColumn << L") "
                << L" S(" << subTileX << L", " << subTileY << L") (" << tilePathname << L")\n";
            ACE_DEBUG((LM_DEBUG, text.str().c_str()));
#endif
        }

    return returnedSubtile.Detach();
}