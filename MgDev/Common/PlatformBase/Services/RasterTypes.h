//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGRASTERTYPES_H
#define MGRASTERTYPES_H


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerated values used to determine the type
/// of units for plot decorations (scale bar).
class MG_PLATFORMBASE_API MgRasterDataModelType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    // These correspond to FDO RasterDataModelType definitions

    /// Data is organized in an unknown or provider specific manner.
    static const INT32 Unknown;     /// value(0)

    /// Data is rectangular grid, floating point data model.
    static const INT32 Data;        /// value(1)

    /// Data is one bit (black/white - bitonal) pixels.
    static const INT32 Bitonal;     /// value(2)

    /// Data is monochrome (black->gray->white) pixels.
    /// It is necessary to know the BitsPerPixel value to interpret the data.
    static const INT32 Gray;        /// value(3)

    /// Data is red/green/blue (in that order) pixels.
    /// It is necessary to know the BitsPerPixel value to interpret the data.
    static const INT32 RGB;         /// value(4)

    /// Data is red/green/blue/alpha (in that order) pixels; RGB with transparency.
    /// It is necessary to know the BitsPerPixel value to interpret the data.
    static const INT32 RGBA;        /// value(5)

    /// Data is monochrome but backed by a pallette (value->pallette->colour) pixels.
    /// It is necessary to know the BitsPerPixel value to interpret the data.
    static const INT32 Palette;     /// value(6)
};

#endif
