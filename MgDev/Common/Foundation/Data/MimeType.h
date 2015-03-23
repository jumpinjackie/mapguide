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

#ifndef MGMIMETYPE_H
#define MGMIMETYPE_H

/// \defgroup MgMimeType MgMimeType
/// \ingroup Common_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Mime types for data to be sent to clients.
class MG_FOUNDATION_API MgMimeType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// \brief
    /// application/agf
    static const STRING Agf;      ///\if INTERNAL value("application/agf") \endif

    /// \brief
    /// application/octet-stream
    static const STRING Binary;   ///\if INTERNAL value("application/octet-stream") \endif

    /// \brief
    /// model/vnd.dwf
    static const STRING Dwf;      ///\if INTERNAL value("model/vnd.dwf") \endif

    /// \brief
    /// image/gif
    static const STRING Gif;      ///\if INTERNAL value("image/gif") \endif

    /// \brief
    /// image/jpeg
    static const STRING Jpeg;     ///\if INTERNAL value("image/jpeg") \endif

    /// \brief
    /// image/png
    static const STRING Png;      ///\if INTERNAL value("image/png") \endif

    /// \brief
    /// text/plain
    static const STRING Text;     ///\if INTERNAL value("text/plain") \endif

    /// \brief
    /// image/tiff
    static const STRING Tiff;     ///\if INTERNAL value("image/tiff") \endif

    /// \brief
    /// text/xml
    static const STRING Xml;      ///\if INTERNAL value("text/xml") \endif

    /// \brief
    /// application/json
    static const STRING Json;      ///\if INTERNAL value("application/json") \endif

    /// \brief
    /// text/html
    static const STRING Html;     ///\if INTERNAL value("text/html") \endif

    /// \brief
    /// application/vnd.google-earth.kml+xml
    static const STRING Kml;      ///\if INTERNAL value("application/vnd.google-earth.kml+xml") \endif

    /// \brief
    /// application/vnd.google-earth.kmz
    static const STRING Kmz;      ///\if INTERNAL value("application/vnd.google-earth.kmz") \endif
};
/// \}

#endif
