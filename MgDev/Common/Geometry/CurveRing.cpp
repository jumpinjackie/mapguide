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

///////////////////////////////////////////////////////////////////////////
// Construct a MgLinearRing object
//
MgCurveRing::MgCurveRing(MgCurveSegmentCollection* segments)
{
    CHECKARGUMENTNULL(segments, L"MgCurveRing.MgCurveRing");

    m_segments = segments->Copy();
}

///////////////////////////////////////////////////////////////////////////
// Construct an empty MgLinearRing object for deserialization
//
MgCurveRing::MgCurveRing()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry component.
//
INT32 MgCurveRing::GetComponentType()
{
    return MgGeometryComponentType::CurveRing;
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of curve segments that define the ring.
//
INT32 MgCurveRing::GetCount()
{
    return m_segments->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve segment at the specified index. The indexing starts at
// zero.
//
MgCurveSegment* MgCurveRing::GetSegment(INT32 index)
{
    return m_segments->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgCurveRing::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgCurveRing::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgCurveRing::IsClosed()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgCurveRing::Copy()
{
    return new MgCurveRing(m_segments);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgCurveRing::Transform(MgTransform* transform)
{
    Ptr<MgCurveSegmentCollection> segments = new MgCurveSegmentCollection();

    INT32 count = m_segments->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCurveSegment> segment = m_segments->GetItem(i);
        Ptr<MgCurveSegment> newSegment = (MgCurveSegment*)segment->Transform(transform);
        segments->Add(newSegment);
    }

    return new MgCurveRing(segments);
}


//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgCurveRing::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeCurveSegmentsEnvelope(m_segments);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurveRing::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurveRing::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
//
void MgCurveRing::Serialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are serialized by the containing geometries
    throw new MgNotImplementedException(L"MgCurveRing.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Deerialize data from TCP/IP stream
//
void MgCurveRing::Deserialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are deserialized by the containing geometries
    throw new MgNotImplementedException(L"MgCurveRing.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry component is spatially equal to another geometry component.
//
bool MgCurveRing::Equals(MgGeometryComponent* other)
{
    if(other == NULL)
        return false;

    if(other->GetComponentType() != MgGeometryComponentType::CurveRing)
        return false;

    return MgGeometryUtil::CurveSegmentsEqual(m_segments, ((MgCurveRing*)other)->m_segments);
}

STRING MgCurveRing::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return tempAwkt;
}

void MgCurveRing::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_segments->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

// Return curve segement collection INTERNAL USE ONLY
MgCurveSegmentCollection* MgCurveRing::GetCurveSegments()
{
    return SAFE_ADDREF((MgCurveSegmentCollection*)m_segments);
}

MgCoordinateIterator* MgCurveRing::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_segments->GetCount();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgCurveSegment> segment = m_segments->GetItem(i);
        Ptr<MgCoordinateIterator> segIterator = segment->GetCoordinates();

        while (segIterator->MoveNext())
        {
            Ptr<MgCoordinate> coord = segIterator->GetCurrent();
            coords->Add(coord);
        }
    }

    return new MgCoordinateIterator(coords);
}
