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

#include "GeometryCommon.h"
#include "Parse/ParseAwkt.h"

IMPLEMENT_CREATE_OBJECT(MgWktReaderWriter)

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The MgWktReaderWriter class supports reading geometry objects from a
/// string in OpenGIS Well-Known Text Format (WKT) and writing geometry
/// objects to a string in WKT format.
///</summary>

MgWktReaderWriter::MgWktReaderWriter()
{
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Reads a geometry object from a string WKT format.
///</summary>
///<param name="wkt">
/// A string that supplies the WKT representation.
///</param>
///<returns>
/// An instance of MgGeometry that cooresponds to the specified WKT.
///</returns>
MgGeometry* MgWktReaderWriter::Read(CREFSTRING wkt)
{
    MgParseAwkt parseAwkt;
    return parseAwkt.ParseAwkt((wchar_t*)wkt.c_str());
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Reads a geometry object from a string in WKT format applying
/// the specified transform during the read.
///</summary>
///<param name="wkt">
/// A string that supplies the WKT representation.
///</param>
///<param name="transform">
/// An MgTransform that is used to transform each coordinate read from the
/// WKT representation.
///</param>
///<returns>
/// An instance of MgGeometry that cooresponds to the specified WKT with
/// the specified transform applied.
///</returns>
MgGeometry* MgWktReaderWriter::Read(CREFSTRING wkt, MgTransform* transform)
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Writes a geometry object to a string in WKT format.
///</summary>
///<param name="geometry">
/// An MgGeometry object to write to WKT.
///</param>
///<returns>
/// A string that contains the WKT representation.
///</returns>
STRING MgWktReaderWriter::Write(MgGeometry* geometry)
{
    if (geometry == NULL)
    {
        throw new MgNullArgumentException(L"MgWktReaderWriter.Write", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return geometry->ToAwkt(false);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Writes a geometry object to a string in WKT format applying
/// the specified transform during the write.
///</summary>
///<param name="geometry">
/// An MgGeometry object to write to WKT.
///</param>
///<param name="transform">
/// An MgTransform that is used to transform each coordinate written to the
/// WKT representation.
///</param>
///<returns>
/// A string that contains the transformed WKT representation.
///</returns>
STRING MgWktReaderWriter::Write(MgGeometry* geometry, MgTransform* transform)
{
    return L"";
}
