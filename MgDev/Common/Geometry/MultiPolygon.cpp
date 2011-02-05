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

MG_IMPL_DYNCREATE(MgMultiPolygon)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiPolygon object
//
MgMultiPolygon::MgMultiPolygon(MgPolygonCollection* polygons)
{
    CHECKARGUMENTNULL(polygons, L"MgMultiPolygon.MgMultiPolygon");

    m_polygons = polygons->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiPolygon object for deserialization
//
MgMultiPolygon::MgMultiPolygon()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiPolygon::GetCount()
{
    return m_polygons->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the polygon at the specified index. The indexing starts at
// zero.
//
MgPolygon* MgMultiPolygon::GetPolygon(INT32 index)
{
    return m_polygons->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiPolygon::GetGeometryType()
{
    return MgGeometryType::MultiPolygon;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiPolygon::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiPolygon::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiPolygon::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiPolygon::Copy()
{
    Ptr<MgPolygonCollection> polygons = m_polygons->Copy();
    return new MgMultiPolygon(polygons);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiPolygon::Transform(MgTransform* transform)
{
    Ptr<MgPolygonCollection> polygons = new MgPolygonCollection();

    INT32 count = m_polygons->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgPolygon> polygon = m_polygons->GetItem(i);
        Ptr<MgPolygon> newPolygon = (MgPolygon*)polygon->Transform(transform);
        polygons->Add(newPolygon);
    }

    return new MgMultiPolygon(polygons);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiPolygon::Serialize(MgStream* stream)
{
    //Serialize to AGF format
    stream->WriteInt32(MgGeometryType::MultiPolygon);

    INT32 numPolygons = m_polygons->GetCount();
    stream->WriteInt32(numPolygons);
    for(INT32 i = 0; i < numPolygons; i++)
    {
        Ptr<MgPolygon> polygon = m_polygons->GetItem(i);
        polygon->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiPolygon::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format
    INT32 type;
    ((MgStreamReader*)stream)->GetInt32(type);
    assert(type == MgGeometryType::MultiPolygon);

    INT32 numPolygons;
    ((MgStreamReader*)stream)->GetInt32(numPolygons);

    m_polygons = new MgPolygonCollection();
    for(INT32 i = 0; i < numPolygons; i++)
    {
        Ptr<MgPolygon> polygon = new MgPolygon;
        polygon->Deserialize(stream);
        m_polygons->Add(polygon);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiPolygon::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiPolygon::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeAggregateEnvelope(this);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgMultiPolygon::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiPolygon::GetGeometryAt(INT32 index)
{
    return GetPolygon(index);
}

STRING MgMultiPolygon::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"MULTIPOLYGON " + coordDim + tempAwkt;
}

void MgMultiPolygon::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_polygons->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiPolygon::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_polygons->GetCount();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPolygon> polygon = m_polygons->GetItem(i);
        Ptr<MgCoordinateIterator> polyIterator = polygon->GetCoordinates();

        while (polyIterator->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(polyIterator->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
