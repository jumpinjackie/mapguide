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

#include "PlatformBase.h"
#include "RasterTypes.h"

///////////////////////////////////////////////////////////////////////////////
/// Raster types
///
const INT32 MgRasterDataModelType::Unknown  = 0;
const INT32 MgRasterDataModelType::Data     = 1;
const INT32 MgRasterDataModelType::Bitonal  = 2;
const INT32 MgRasterDataModelType::Gray     = 3;
const INT32 MgRasterDataModelType::RGB      = 4;
const INT32 MgRasterDataModelType::RGBA     = 5;
const INT32 MgRasterDataModelType::Palette  = 6;
