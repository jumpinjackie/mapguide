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

#include "GeometryCommon.h"

MG_IMPL_DYNCREATE(MgLineString)

//////////////////////////////////////////////////////////////////
// Construct a MgLineString object
//
MgLineString::MgLineString(MgCoordinateCollection* coordinates)
{
    CHECKARGUMENTNULL(coordinates, L"MgLineString.MgLineString");

    if (coordinates->GetCount() < 2)
    {
        STRING buffer;
        MgUtil::Int32ToString(coordinates->GetCount(), buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgLineString.MgLineString",
            __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryTooFewCoordinates", NULL);
    }

    m_coordinates = coordinates->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgLineString object for deserialization
//
MgLineString::MgLineString()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the starting coordinate of the curve.
//
MgCoordinate* MgLineString::GetStartCoordinate()
{
    return m_coordinates->GetItem(0);
}

///////////////////////////////////////////////////////////////////////////
// Gets the ending coordinate of the curve.
//
MgCoordinate* MgLineString::GetEndCoordinate()
{
    return m_coordinates->GetItem(m_coordinates->GetCount() - 1);
}

///////////////////////////////////////////////////////////////////////////
// Returns an iterator that can be used to enumerate each of the line
// strings coordinates.
//
MgCoordinateIterator* MgLineString::GetCoordinates()
{
    return new MgCoordinateIterator(m_coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgLineString::GetGeometryType()
{
    return MgGeometryType::LineString;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgLineString::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgLineString::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgLineString::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgLineString::Copy()
{
    return new MgLineString(m_coordinates);
}


///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgLineString::Transform(MgTransform* transform)
{
    Ptr<MgCoordinateCollection> lineString = new MgCoordinateCollection();

    INT32 count = m_coordinates->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCoordinate> coord = m_coordinates->GetItem(i);
        Ptr<MgCoordinate> newCoord = transform->Transform(coord);
        lineString->Add(newCoord);
    }

    return new MgLineString(lineString);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgLineString::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgLineString::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeLineStringEnvelope(m_coordinates);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgLineString::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgLineString::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    //Type
    stream->WriteInt32(MgGeometryType::LineString);

    //Coordinate dimension
    Ptr<MgCoordinate> coord1 = m_coordinates->GetItem(0);
    stream->WriteInt32(coord1->GetDimension());

    //Num Points
    INT32 numPoints = m_coordinates->GetCount();
    stream->WriteInt32(numPoints);

    //Points
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgCoordinate> coord = m_coordinates->GetItem(i);
        coord->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
//
void MgLineString::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format

    //Type
    INT32 type;
    stream->GetInt32(type);
    assert(type == MgGeometryType::LineString);

    //Coordinate dimension
    INT32 coordinateDimension;
    stream->GetInt32(coordinateDimension);

    //Num points
    INT32 numPoints;
    stream->GetInt32(numPoints);
    assert(numPoints >= 2);

    //Points
    m_coordinates = new MgCoordinateCollection();
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgCoordinate> coord = MgGeometryUtil::ReadCoordinate(stream, coordinateDimension);
        m_coordinates->Add(coord);
    }
}

STRING MgLineString::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"LINESTRING " + coordDim + tempAwkt;
}

void MgLineString::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";
    m_coordinates->ToAwkt(awktStr, coordDim, is2dOnly);
    awktStr += L")";
}
