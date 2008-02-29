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

#include "GeometryCommon.h"

MG_IMPL_DYNCREATE(MgCurvePolygon)

//////////////////////////////////////////////////////////////////
// Construct a MgCurvePolygon object
//
MgCurvePolygon::MgCurvePolygon(MgCurveRing* outerRing, MgCurveRingCollection* innerRings)
{
    CHECKARGUMENTNULL(outerRing, L"MgPolygon.MgPolygon");

    m_outerRing = (MgCurveRing*)outerRing->Copy();
    m_innerRings = innerRings != NULL? innerRings->Copy(): NULL;
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgCurvePolygon object for deserialization
//
MgCurvePolygon::MgCurvePolygon()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the exterior ring of the polygon.
//
MgCurveRing* MgCurvePolygon::GetExteriorRing()
{
    return SAFE_ADDREF((MgCurveRing*)m_outerRing);
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of interior rings in this polygon.
//
INT32 MgCurvePolygon::GetInteriorRingCount()
{
    return m_innerRings != NULL? m_innerRings->GetCount(): 0;
}

///////////////////////////////////////////////////////////////////////////
// Gets the interior ring at the specified index. The indexing starts at
// zero.
//
MgCurveRing* MgCurvePolygon::GetInteriorRing(INT32 index)
{
    if(m_innerRings == NULL)
        throw new MgArgumentOutOfRangeException(L"MgCurvePolygon.GetInteriorRing", __LINE__, __WFILE__, NULL, L"", NULL);

    return m_innerRings->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgCurvePolygon::GetGeometryType()
{
    return MgGeometryType::CurvePolygon;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgCurvePolygon::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgCurvePolygon::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgCurvePolygon::IsClosed()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgCurvePolygon::Copy()
{
    return new MgCurvePolygon(m_outerRing, m_innerRings);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgCurvePolygon::Transform(MgTransform* transform)
{
    Ptr<MgCurveRing> ring = (MgCurveRing*)m_outerRing->Transform(transform);
    Ptr<MgCurveRingCollection> innerRings;

    if (m_innerRings != NULL)
    {
        innerRings = new MgCurveRingCollection();
        INT32 count = m_innerRings->GetCount();
        for (int i = 0; i < count; i++)
        {
            Ptr<MgCurveRing> innerRing = m_innerRings->GetItem(i);
            Ptr<MgCurveRing> newRing = (MgCurveRing*)innerRing->Transform(transform);
            innerRings->Add(newRing);
        }
    }

    return new MgCurvePolygon(ring, innerRings);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurvePolygon::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgCurvePolygon::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = m_outerRing->Envelope();
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurvePolygon::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgCurvePolygon::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    //Get the coordinate dimension for this polygon
    Ptr<MgCurveSegment> segment = m_outerRing->GetSegment(0);
    Ptr<MgCoordinate> startCoord = segment->GetStartCoordinate();
    INT32 coordinateDimension = startCoord->GetDimension();

    //Type
    stream->WriteInt32(MgGeometryType::CurvePolygon);

    //Dimension
    stream->WriteInt32(coordinateDimension);

    //Num rings
    INT32 numRings = m_innerRings != NULL? m_innerRings->GetCount(): 0;
    stream->WriteInt32(numRings + 1);   //add outer ring

    //Outer ring
    MgGeometryUtil::WriteCurveRing(stream, m_outerRing);

    //Inner rings
    for(INT32 i = 1; i < numRings; i++)
    {
        Ptr<MgCurveRing> ring = m_innerRings->GetItem(i);
        MgGeometryUtil::WriteCurveRing(stream, ring);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgCurvePolygon::Deserialize(MgStream* stream)
{
    //Deerialize from AGF format
    //Type
    INT32 type;
    stream->GetInt32(type);
    assert(type == MgGeometryType::CurvePolygon);

    //Dimension
    INT32 coordinateDimension;
    stream->GetInt32(coordinateDimension);

    //Num rings
    INT32 numRings;
    stream->GetInt32(numRings);   //include outer ring
    assert(numRings >= 1);

    //Outer ring
    m_outerRing = MgGeometryUtil::ReadCurveRing(stream, coordinateDimension);

    //Inner rings
    if(numRings > 1)
    {
        m_innerRings = new MgCurveRingCollection;
        for(INT32 i = 1; i < numRings - 1; i++)
        {
            Ptr<MgCurveRing> ring = MgGeometryUtil::ReadCurveRing(stream, coordinateDimension);
            m_innerRings->Add(ring);
        }
    }
}


STRING MgCurvePolygon::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"CURVEPOLYGON " + coordDim + tempAwkt;
}

void MgCurvePolygon::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_outerRing->ToAwkt(awktStr, coordDim, is2dOnly);

    if (GetInteriorRingCount() > 0)
    {
        awktStr += L", ";
        m_innerRings->ToAwkt(awktStr, coordDim, is2dOnly);
    }

    awktStr += L")";
}

MgCoordinateIterator* MgCurvePolygon::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    Ptr<MgCoordinateIterator> outerIterator = m_outerRing->GetCoordinates();

    while (outerIterator->MoveNext())
    {
        coords->Add(Ptr<MgCoordinate>(outerIterator->GetCurrent()));
    }

    if (m_innerRings != NULL)
    {
        INT32 innerCount = m_innerRings->GetCount();
        for (INT32 i = 0; i < innerCount; i++)
        {
            Ptr<MgCurveRing> ring = m_innerRings->GetItem(i);
            Ptr<MgCoordinateIterator> ringIterator = ring->GetCoordinates();

            while (ringIterator->MoveNext())
            {
                coords->Add(Ptr<MgCoordinate>(ringIterator->GetCurrent()));
            }
        }
    }

    return new MgCoordinateIterator(coords);
}
