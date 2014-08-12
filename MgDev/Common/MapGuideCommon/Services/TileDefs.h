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

#ifndef MGTILEDEFS_H
#define MGTILEDEFS_H

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Tile Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgTileServiceOpId
{
INTERNAL_API:
    static const int GetTile              = 0x1111E801;
    static const int SetTile              = 0x1111E802;
    static const int ClearCache           = 0x1111E803;
    static const int GetDefaultTileSizeX  = 0x1111E804;
    static const int GetDefaultTileSizeY  = 0x1111E805;
    static const int ClearCache2          = 0x1111E806;
    static const int GetDefaultTileSizeX2 = 0x1111E807;
    static const int GetDefaultTileSizeY2 = 0x1111E808;
    static const int GetTileProviders     = 0x1111E809;
};
/// \endcond


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Default tile parameters.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgTileParameters
{
INTERNAL_API:
    static INT32 tileWidth;       // width for all tiles
    static INT32 tileHeight;      // height for all tiles
    static const INT32 tileDPI;   // assumed DPI for all tiles
    static STRING tileFormat;     // image format for all tiles
    static STRING tileCachePath;  // tile cache root path
};
/// \endcond

#define MG_TILE_PROVIDER_DEFAULT L"Default"
#define MG_TILE_PROVIDER_XYZ L"XYZ"
#define MG_TILE_PROVIDER_COMMON_PARAM_TILEPATH         L"TilePath"
#define MG_TILE_PROVIDER_COMMON_PARAM_TILEWIDTH        L"TileWidth"
#define MG_TILE_PROVIDER_COMMON_PARAM_TILEHEIGHT       L"TileHeight"
#define MG_TILE_PROVIDER_COMMON_PARAM_TILEFORMAT       L"TileFormat"
#define MG_TILE_PROVIDER_COMMON_PARAM_RENDERONLY       L"RenderOnly"
#define MG_TILE_PROVIDER_COMMON_PARAM_COORDINATESYSTEM L"CoordinateSystem"
#define MG_TILE_PROVIDER_COMMON_PARAM_FINITESCALELIST  L"FiniteScaleList"

#endif
