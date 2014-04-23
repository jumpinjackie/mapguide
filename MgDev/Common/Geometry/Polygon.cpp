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

MG_IMPL_DYNCREATE(MgPolygon)

//////////////////////////////////////////////////////////////////
// Construct a MgPolygon object
//
MgPolygon::MgPolygon(MgLinearRing* outerRing, MgLinearRingCollection* innerRings)
{
    CHECKARGUMENTNULL(outerRing, L"MgPolygon.MgPolygon");

    m_outerRing = (MgLinearRing*)outerRing->Copy();
    m_innerRings = innerRings != NULL? innerRings->Copy(): NULL;
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgPolygon object for deserialization
//
MgPolygon::MgPolygon()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the exterior ring of the polygon.
//
MgLinearRing* MgPolygon::GetExteriorRing()
{
    return SAFE_ADDREF((MgLinearRing*)m_outerRing);
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of interior rings in this polygon.
//
INT32 MgPolygon::GetInteriorRingCount()
{
    return (m_innerRings != NULL)? m_innerRings->GetCount(): 0;
}

///////////////////////////////////////////////////////////////////////////
// Gets the interior ring at the specified index. The indexing starts at
// zero.
//
MgLinearRing* MgPolygon::GetInteriorRing(INT32 index)
{
    if (m_innerRings == NULL)
        throw new MgArgumentOutOfRangeException(L"MgPolygon.GetInteriorRing", __LINE__, __WFILE__, NULL, L"", NULL);

    return m_innerRings->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgPolygon::GetGeometryType()
{
    return MgGeometryType::Polygon;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgPolygon::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgPolygon::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgPolygon::IsClosed()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgPolygon::Copy()
{
    return new MgPolygon(m_outerRing, m_innerRings);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgPolygon::Transform(MgTransform* transform)
{
    Ptr<MgLinearRing> newOuterRing = (MgLinearRing*)m_outerRing->Transform(transform);
    Ptr<MgLinearRingCollection> newInnerRings;

    if (m_innerRings != NULL)
    {
        newInnerRings = new MgLinearRingCollection();
        INT32 count = m_innerRings->GetCount();
        for (INT32 i=0; i<count; ++i)
        {
            Ptr<MgLinearRing> innerRing = m_innerRings->GetItem(i);
            Ptr<MgLinearRing> newInnerRing = (MgLinearRing*)innerRing->Transform(transform);
            newInnerRings->Add(newInnerRing);
        }
    }

    return new MgPolygon(newOuterRing, newInnerRings);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgPolygon::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgPolygon::ComputeEnvelope()
{
    if (m_envelope == NULL)
        m_envelope = m_outerRing->Envelope();

    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgPolygon::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgPolygon::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    //Get the coordinate dimension
    Ptr<MgCoordinateCollection> coordinates = m_outerRing->GetCoordinateCollection();
    Ptr<MgCoordinate> startCoord = coordinates->GetItem(0);
    INT32 coordinateDimension = startCoord->GetDimension();

    //Type
    stream->WriteInt32(MgGeometryType::Polygon);

    //Coordinate dimension
    stream->WriteInt32(coordinateDimension);

    //Num rings
    INT32 numInnerRings = GetInteriorRingCount();
    stream->WriteInt32(1 + numInnerRings);  //add outer ring

    //Outer ring
    MgGeometryUtil::WriteLinearRing(stream, m_outerRing);

    //Inner rings
    for (INT32 i=0; i<numInnerRings; ++i)
    {
        Ptr<MgLinearRing> ring = m_innerRings->GetItem(i);
        MgGeometryUtil::WriteLinearRing(stream, ring);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgPolygon::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format

    //Type
    INT32 type;
    stream->GetInt32(type);
    assert(type == MgGeometryType::Polygon);

    //Coordinate dimension
    INT32 coordinateDimension;
    stream->GetInt32(coordinateDimension);

    //Num rings
    INT32 numRings;
    stream->GetInt32(numRings);
    assert(numRings >= 1);

    //Outer ring
    m_outerRing = MgGeometryUtil::ReadLinearRing(stream, coordinateDimension);

    //Inner rings
    if (numRings > 1)
    {
        m_innerRings = new MgLinearRingCollection();
        for (INT32 i=1; i<numRings; ++i)
        {
            Ptr<MgLinearRing> innerRing = MgGeometryUtil::ReadLinearRing(stream, coordinateDimension);
            m_innerRings->Add(innerRing);
        }
    }
    else
        m_innerRings = NULL;
}

//////////////////////////////////////////////////////////////////
void MgPolygon::ToXml(std::string& str)
{
    str += "<Polygon>";

    if (m_outerRing != NULL)
    {
        str += "<OuterRing>";
        m_outerRing->ToXml(str);
        str += "</OuterRing>";
    }

    if (m_innerRings != NULL)
    {
        str += "<InnerRings>";

        INT32 numInnerRings = m_innerRings->GetCount();
        for (INT32 i=0; i<numInnerRings; ++i)
        {
            str += "<InnerRing>";

            Ptr<MgLinearRing> innerRing = m_innerRings->GetItem(i);
            innerRing->ToXml(str);

            str += "</InnerRing>";
        }

        str += "</InnerRings>";
    }

    str += "</Polygon>";
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Convert to AWKT representation
///</summary>
STRING MgPolygon::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"POLYGON " + coordDim + tempAwkt;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Convert to AWKT representation
///</summary>
void MgPolygon::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    if (m_outerRing != NULL)
        m_outerRing->ToAwkt(awktStr, coordDim, is2dOnly);

    if (GetInteriorRingCount() > 0)
    {
        awktStr += L", ";
        m_innerRings->ToAwkt(awktStr, coordDim, is2dOnly);
    }

    awktStr += L")";
}

//////////////////////////////////////////////////////////////////
MgCoordinateIterator* MgPolygon::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    Ptr<MgCoordinateIterator> outerCoords = m_outerRing->GetCoordinates();

    while (outerCoords->MoveNext())
    {
        coords->Add(Ptr<MgCoordinate>(outerCoords->GetCurrent()));
    }

    if (m_innerRings != NULL)
    {
        INT32 innerCount = m_innerRings->GetCount();
        for (INT32 i=0; i<innerCount; ++i)
        {
            Ptr<MgLinearRing> ring = m_innerRings->GetItem(i);
            Ptr<MgCoordinateIterator> innerCoords = ring->GetCoordinates();

            while (innerCoords->MoveNext())
            {
                coords->Add(Ptr<MgCoordinate>(innerCoords->GetCurrent()));
            }
        }
    }

    return new MgCoordinateIterator(coords);
}
