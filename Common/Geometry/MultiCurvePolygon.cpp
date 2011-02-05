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

MG_IMPL_DYNCREATE(MgMultiCurvePolygon)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiCurvePolygon object
//
MgMultiCurvePolygon::MgMultiCurvePolygon(MgCurvePolygonCollection* polygons)
{
    CHECKARGUMENTNULL(polygons, L"MgMultiCurvePolygon.MgMultiCurvePolygon");

    m_polygons = polygons->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiCurvePolygon object for deserialization
//
MgMultiCurvePolygon::MgMultiCurvePolygon()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiCurvePolygon::GetCount()
{
    return m_polygons->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the polygon at the specified index. The indexing starts at
// zero.
//
MgCurvePolygon* MgMultiCurvePolygon::GetCurvePolygon(INT32 index)
{
    return m_polygons->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiCurvePolygon::GetGeometryType()
{
    return MgGeometryType::MultiCurvePolygon;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiCurvePolygon::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiCurvePolygon::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiCurvePolygon::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiCurvePolygon::Copy()
{
    Ptr<MgCurvePolygonCollection> polygons = m_polygons->Copy();
    return new MgMultiCurvePolygon(polygons);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiCurvePolygon::Transform(MgTransform* transform)
{
    Ptr<MgCurvePolygonCollection> polygons = new MgCurvePolygonCollection();

    INT32 count = m_polygons->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCurvePolygon> polygon = m_polygons->GetItem(i);
        Ptr<MgCurvePolygon> newPolygon = (MgCurvePolygon*)polygon->Transform(transform);
        polygons->Add(newPolygon);
    }

    return new MgMultiCurvePolygon(polygons);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiCurvePolygon::Serialize(MgStream* stream)
{
    //Serialize to AGF format
    stream->WriteInt32(MgGeometryType::MultiCurvePolygon);

    INT32 numPolygons = m_polygons->GetCount();
    stream->WriteInt32(numPolygons);
    for(INT32 i = 0; i < numPolygons; i++)
    {
        Ptr<MgCurvePolygon> polygon = m_polygons->GetItem(i);
        polygon->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiCurvePolygon::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format
    INT32 type;
    ((MgStreamReader*)stream)->GetInt32(type);
    assert(type == MgGeometryType::MultiCurvePolygon);

    INT32 numPolygons;
    ((MgStreamReader*)stream)->GetInt32(numPolygons);

    m_polygons = new MgCurvePolygonCollection();
    for(INT32 i = 0; i < numPolygons; i++)
    {
        Ptr<MgCurvePolygon> polygon = new MgCurvePolygon;
        polygon->Deserialize(stream);
        m_polygons->Add(polygon);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiCurvePolygon::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiCurvePolygon::ComputeEnvelope()
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
void MgMultiCurvePolygon::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiCurvePolygon::GetGeometryAt(INT32 index)
{
    return GetCurvePolygon(index);
}


STRING MgMultiCurvePolygon::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"MULTICURVEPOLYGON " + coordDim + tempAwkt;
}

void MgMultiCurvePolygon::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_polygons->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiCurvePolygon::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 polygonCount = m_polygons->GetCount();

    for (INT32 i = 0; i < polygonCount; i++)
    {
        Ptr<MgCurvePolygon> polygon = m_polygons->GetItem(i);
        Ptr<MgCoordinateIterator> polyIterator = polygon->GetCoordinates();

        while (polyIterator->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(polyIterator->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
