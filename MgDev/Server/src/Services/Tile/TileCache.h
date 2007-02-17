//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MG_TILE_CACHE_H_
#define _MG_TILE_CACHE_H_

#include "ServerTileDllExport.h"

// File-system-based cache for base map layer group tiles generated by the rendering service.
class MG_SERVER_TILE_API MgTileCache : public MgDisposable
{
public:
    MgTileCache();

    static void Initialize();

    void GeneratePathnames(MgResourceIdentifier* mapDef, int scaleIndex,
        CREFSTRING group, int tileColumn, int tileRow,
        STRING& tilePathname, STRING& lockPathname, bool createFullPath);
    void GeneratePathnames(MgMap* map, int scaleIndex,
        CREFSTRING group, int tileColumn, int tileRow,
        STRING& tilePathname, STRING& lockPathname, bool createFullPath);

    STRING CreateFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING CreateFullPath(MgMap* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

    MgByteReader* Get(CREFSTRING tilePathname);
    void Set(MgByteReader* img, CREFSTRING tilePathname);

    void Clear(MgMap* map);
    void Clear(MgResourceIdentifier* mapDef);

protected:
    virtual void Dispose()
    {
        delete this;
    }

private:
    // Unimplemented Constructors/Methods
    MgTileCache(const MgTileCache&);
    MgTileCache& operator=(const MgTileCache&);

    STRING GetBasePath(MgResourceIdentifier* mapDef);
    STRING GetBasePath(MgMap* map);

    STRING GetFullPath(CREFSTRING basePath, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING GetFullPath(MgResourceIdentifier* mapDef, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);
    STRING GetFullPath(MgMap* map, int scaleIndex, CREFSTRING group, int tileColumn, int tileRow);

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
