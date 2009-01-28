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

#ifndef MGIMAGEFORMATS_H
#define MGIMAGEFORMATS_H

/// \ingroup Rendering_Service_Module

///////////////////////////////////////////////////////////
/// \brief
/// Image formats supported by the rendering service.
///
class MG_MAPGUIDE_API MgImageFormats
{
PUBLISHED_API:
    /// \internal
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.

    /// Gif format
    static const STRING Gif;    /// \if INTERNAL value("GIF") \endif

    /// Jpeg format
    static const STRING Jpeg;   /// \if INTERNAL value("JPG") \endif

    /// Png format
    static const STRING Png;    /// \if INTERNAL value("PNG") \endif

    /// Png format
    static const STRING Png8;   /// \if INTERNAL value("PNG8") \endif

    /// Tiff format
    static const STRING Tiff;   /// \if INTERNAL value("TIF") \endif

    /// Raw format
    static const STRING Raw;    /// \if INTERNAL value("RAW") \endif
};

#endif
