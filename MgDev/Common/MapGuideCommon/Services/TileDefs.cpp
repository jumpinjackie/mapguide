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
#include "TileDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Tile parameters.
///
/// NOTE: Changes to tileDPI must be accounted for in the HTML/AJAX viewer.
///       tileWidth and tileHeight are requested by the viewer during initialization.
///       The AJAX viewer supports both JPG and PNG tiles.
///
INT32  MgTileParameters::tileWidth  = 300;                    // width for all tiles
INT32  MgTileParameters::tileHeight = 300;                    // height for all tiles
const INT32  MgTileParameters::tileDPI    =  96;              // assumed DPI for all tiles
STRING MgTileParameters::tileFormat = MgImageFormats::Png;    // image format for all tiles
