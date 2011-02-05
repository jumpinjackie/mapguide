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

MG_IMPL_DYNCREATE(MgCurveString)

//////////////////////////////////////////////////////////////////
// Construct a MgCurveString object
//
MgCurveString::MgCurveString(MgCurveSegmentCollection* curveSegments)
{
    CHECKARGUMENTNULL(curveSegments, L"MgCurveString.MgCurveString");

    if(curveSegments->GetCount() == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgCurveString.MgCurveString",
            __LINE__, __WFILE__, &arguments, L"MgGeometryEmpty", NULL);
    }

    m_segments = curveSegments->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgCurveString object for deserialization
//
MgCurveString::MgCurveString()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of segments in this curve string.
//
INT32 MgCurveString::GetCount()
{
    return m_segments->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve segment at the specified index. The indexing starts at
// zero.
//
MgCurveSegment* MgCurveString::GetSegment(INT32 index)
{
    return m_segments->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the starting coordinate of the curve.
//
MgCoordinate* MgCurveString::GetStartCoordinate()
{
    Ptr<MgCurveSegment> segment = m_segments->GetItem(0);
    return segment->GetStartCoordinate();
}

///////////////////////////////////////////////////////////////////////////
// Gets the ending coordinate of the curve.
//
MgCoordinate* MgCurveString::GetEndCoordinate()
{
    Ptr<MgCurveSegment> segment = m_segments->GetItem(m_segments->GetCount() - 1);
    return segment->GetEndCoordinate();
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgCurveString::GetGeometryType()
{
    return MgGeometryType::CurveString;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgCurveString::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgCurveString::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgCurveString::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgCurveString::Copy()
{
    return new MgCurveString(m_segments);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgCurveString::Transform(MgTransform* transform)
{
    Ptr<MgCurveSegmentCollection> segments = new MgCurveSegmentCollection();

    INT32 count = m_segments->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCurveSegment> segment = m_segments->GetItem(i);
        Ptr<MgCurveSegment> newSegment = (MgCurveSegment*)segment->Transform(transform);
        segments->Add(newSegment);
    }

    return new MgCurveString(segments);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurveString::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgCurveString::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeCurveSegmentsEnvelope(m_segments);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurveString::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgCurveString::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    //Get the coordinate dimension
    Ptr<MgCurveSegment> segment = m_segments->GetItem(0);
    Ptr<MgCoordinate> coord1 = segment->GetStartCoordinate();
    INT32 coordinateDimension = coord1->GetDimension();

    //Type
    stream->WriteInt32(MgGeometryType::CurveString);

    //Coordinate Dimension
    stream->WriteInt32(coordinateDimension);

    //Start point
    coord1->Serialize(stream);

    //Num segments
    INT32 numElements = m_segments->GetCount();
    stream->WriteInt32(numElements);

    //Segments
    for(INT32 i = 0; i < numElements; i++)
    {
        Ptr<MgCurveSegment> segment = m_segments->GetItem(i);
        MgGeometryUtil::WriteCurveSegment(stream, segment);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgCurveString::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format

    //Type
    INT32 type;
    ((MgStreamReader*)stream)->GetInt32(type);
    assert(type == MgGeometryType::CurveString);

    //Coordinate dimension
    INT32 coordinateDimension;
    ((MgStreamReader*)stream)->GetInt32(coordinateDimension);

    //Start point
    Ptr<MgCoordinate> startCoord = MgGeometryUtil::ReadCoordinate(stream, coordinateDimension);

    //Num segments
    INT32 numElements;
    ((MgStreamReader*)stream)->GetInt32(numElements);
    assert(numElements >= 1);

    //Segments
    m_segments = new MgCurveSegmentCollection();

    for(INT32 i = 0; i < numElements; i++)
    {
        Ptr<MgCurveSegment> segment = MgGeometryUtil::ReadCurveSegment(stream, startCoord, coordinateDimension);
        m_segments->Add(segment);
        startCoord = segment->GetEndCoordinate();
    }
}

STRING MgCurveString::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return  L"CURVESTRING " + coordDim + tempAwkt;
}

void MgCurveString::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_segments->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

// Return curve segement collection INTERNAL USE ONLY
MgCurveSegmentCollection* MgCurveString::GetCurveSegments()
{
    return SAFE_ADDREF((MgCurveSegmentCollection*)m_segments);
}

MgCoordinateIterator* MgCurveString::GetCoordinates()
{
    INT32 count = m_segments->GetCount();
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgCurveSegment> segment = m_segments->GetItem(i);
        Ptr<MgCoordinateIterator> segCoords = segment->GetCoordinates();

        while (segCoords->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(segCoords->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
