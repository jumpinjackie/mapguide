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

//////////////////////////////////////////////////////////////////
// Construct a MgLinearSegment object
//
MgLinearSegment::MgLinearSegment(MgCoordinateCollection* coordinates)
{
    CHECKARGUMENTNULL(coordinates, L"MgLinearSegment.MgLinearSegment");

    //at least 2 coordinates required to make a segment
    if (coordinates->GetCount() < 2)
    {
        STRING buffer;
        MgUtil::Int32ToString(coordinates->GetCount(), buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgLinearSegment.MgLinearSegment",
            __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryTooFewCoordinates", NULL);
    }

    m_coordinates = coordinates->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgLinearSegment object for deserialization
//
MgLinearSegment::MgLinearSegment()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry component.
//
INT32 MgLinearSegment::GetComponentType()
{
    return MgGeometryComponentType::LinearSegment;
}

///////////////////////////////////////////////////////////////////////////
// Returns an iterator that can be used to enumerate each of the linear
// segments coordinates.
//
MgCoordinateIterator* MgLinearSegment::GetCoordinates()
{
    return new MgCoordinateIterator(m_coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Gets the starting coordinate of the curve segment.
//
MgCoordinate* MgLinearSegment::GetStartCoordinate()
{
    return (MgCoordinate*)m_coordinates->GetItem(0);
}

///////////////////////////////////////////////////////////////////////////
// Gets the ending coordinate of the curve segment.
//
MgCoordinate* MgLinearSegment::GetEndCoordinate()
{
    return (MgCoordinate*)m_coordinates->GetItem(m_coordinates->GetCount() - 1);
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgLinearSegment::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgLinearSegment::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgLinearSegment::IsClosed()
{
    Ptr<MgCoordinate> start = GetStartCoordinate();
    Ptr<MgCoordinate> end = GetEndCoordinate();

    return (start->GetX() == end->GetX() &&
            start->GetY() == end->GetY());
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgLinearSegment::Copy()
{
    return new MgLinearSegment(m_coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgLinearSegment::Transform(MgTransform* transform)
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();

    INT32 count = m_coordinates->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCoordinate> coord = m_coordinates->GetItem(i);
        Ptr<MgCoordinate> newCoord = transform->Transform(coord);
        coords->Add(newCoord);
    }

    return new MgLinearSegment(coords);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgLinearSegment::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
//
void MgLinearSegment::Serialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are serialized by the containing geometries
    throw new MgNotImplementedException(L"MgLinearSegment.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
//
void MgLinearSegment::Deserialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are deserialized by the containing geometries
    throw new MgNotImplementedException(L"MgLinearSegment.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgLinearSegment::ComputeEnvelope()
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
void MgLinearSegment::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry component is spatially equal to another geometry component.
//
bool MgLinearSegment::Equals(MgGeometryComponent* other)
{
    if(other == NULL)
        return false;

    if(other->GetComponentType() != MgGeometryComponentType::LinearSegment)
        return false;

    MgLinearSegment* seg = (MgLinearSegment*)other;

    return MgGeometryUtil::CoordinatesEqual(m_coordinates, seg->m_coordinates);
}

STRING MgLinearSegment::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return tempAwkt;
}

void MgLinearSegment::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    STRING tempAwkt = L"";

    assert(m_coordinates != NULL);

    Ptr<MgCoordinateCollection> coordCol = m_coordinates->Copy();
    assert(coordCol->GetCount() != 0);

    coordCol->RemoveAt(0); // Skip first co-ordinate

    awktStr += L"LINESTRINGSEGMENT ";
    awktStr += L"(";

    coordCol->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

// Return the internal co-ordinate collection INTERNAL USE ONLY
MgCoordinateCollection* MgLinearSegment::GetCoordinateCollection()
{
    return SAFE_ADDREF((MgCoordinateCollection*)m_coordinates);
}
