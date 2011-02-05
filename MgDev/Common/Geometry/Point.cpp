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

MG_IMPL_DYNCREATE(MgPoint)

//////////////////////////////////////////////////////////////////
// Construct a MgPoint object
//
MgPoint::MgPoint(MgCoordinate* coordinate)
{
    CHECKARGUMENTNULL(coordinate, L"MgPoint.MgPoint");

    m_coord = SAFE_ADDREF((MgCoordinate*)coordinate);
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgPoint object for deserialization
//
MgPoint::MgPoint()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgPoint::GetGeometryType()
{
    return MgGeometryType::Point;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgPoint::GetDimension()
{
    return MgGeometricDimension::Point;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgPoint::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgPoint::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Gets the coordinate that defines this point.
//
MgCoordinate* MgPoint::GetCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_coord);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgPoint::Copy()
{
    return new MgPoint(m_coord);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgPoint::Transform(MgTransform* transform)
{
    Ptr<MgCoordinate> coord = transform->Transform(m_coord);
    return new MgPoint(coord);
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgPoint::ComputeEnvelope()
{
    return new MgEnvelope(m_coord, m_coord);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgPoint::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgPoint::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgPoint::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    //Type
    stream->WriteInt32(MgGeometryType::Point);

    //Coordinate dimension
    stream->WriteInt32(m_coord->GetDimension());

    //Position
    m_coord->Serialize(stream);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgPoint::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format

    //Type
    INT32 type;
    stream->GetInt32(type);
    assert(type == MgGeometryType::Point);

    //Coordinate dimension
    INT32 coordinateDimension;
    stream->GetInt32(coordinateDimension);

    //Position
    m_coord = MgGeometryUtil::ReadCoordinate(stream, coordinateDimension);
}

STRING MgPoint::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"POINT " + coordDim + L"(" + tempAwkt + L")";
}

void MgPoint::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    m_coord->ToAwkt(awktStr, coordDim, is2dOnly);
}

MgCoordinateIterator* MgPoint::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    Ptr<MgCoordinate> coordCopy = m_coord->Copy();
    coords->Add(coordCopy);

    return new MgCoordinateIterator(coords);
}
