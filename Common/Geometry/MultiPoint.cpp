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

MG_IMPL_DYNCREATE(MgMultiPoint)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiPoint object
//
MgMultiPoint::MgMultiPoint(MgPointCollection* points)
{
    CHECKARGUMENTNULL(points, L"MgMultiPoint.MgMultiPoint");

    m_points = points->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiPoint object for deserialization
//
MgMultiPoint::MgMultiPoint()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiPoint::GetCount()
{
    return m_points->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the point at the specified index. The indexing starts at
// zero.
//
MgPoint* MgMultiPoint::GetPoint(INT32 index)
{
    return m_points->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiPoint::GetGeometryType()
{
    return MgGeometryType::MultiPoint;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiPoint::GetDimension()
{
    return MgGeometricDimension::Point;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiPoint::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiPoint::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiPoint::Copy()
{
    Ptr<MgPointCollection> points = m_points->Copy();
    return new MgMultiPoint(points);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiPoint::Transform(MgTransform* transform)
{
    Ptr<MgPointCollection> points = new MgPointCollection();

    INT32 count = m_points->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgPoint> point = m_points->GetItem(i);
        Ptr<MgPoint> newPoint = (MgPoint*)point->Transform(transform);
        points->Add(newPoint);
    }

    return new MgMultiPoint(points);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiPoint::Serialize(MgStream* stream)
{
    //Serialize to AGF format
    stream->WriteInt32(MgGeometryType::MultiPoint);

    INT32 numPoints = m_points->GetCount();
    stream->WriteInt32(numPoints);
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgPoint> point = m_points->GetItem(i);
        point->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiPoint::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format
    INT32 type;
    ((MgStreamReader*)stream)->GetInt32(type);
    assert(type == MgGeometryType::MultiPoint);

    INT32 numPoints;
    ((MgStreamReader*)stream)->GetInt32(numPoints);

    m_points = new MgPointCollection();
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgPoint> point = new MgPoint;
        point->Deserialize(stream);
        m_points->Add(point);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiPoint::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiPoint::ComputeEnvelope()
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
void MgMultiPoint::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiPoint::GetGeometryAt(INT32 index)
{
    return GetPoint(index);
}

STRING MgMultiPoint::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"MULTIPOINT " + coordDim + tempAwkt;
}

void MgMultiPoint::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_points->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiPoint::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_points->GetCount();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPoint> point = m_points->GetItem(i);
        coords->Add(Ptr<MgCoordinate>(point->GetCoordinate()));
    }

    return new MgCoordinateIterator(coords);
}
