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
#include "ImageFormats.h"

///////////////////////////////////////////////////////////////////////////////
/// Image formats supported by the rendering service.
///
/// The exact list of supported formats is not defined yet.
/// At least PNG will be supported.
///
const STRING MgImageFormats::Gif    = L"GIF";
const STRING MgImageFormats::Jpeg   = L"JPG";
const STRING MgImageFormats::Png    = L"PNG";
const STRING MgImageFormats::Png8   = L"PNG8";
const STRING MgImageFormats::Tiff   = L"TIF";
const STRING MgImageFormats::Raw    = L"RAW";
