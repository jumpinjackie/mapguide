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
#include "Spatial/SpatialUtility.h"

//////////////////////////////////////////////////////////////////
// Construct a MgArcSegment object
//
MgArcSegment::MgArcSegment(MgCoordinate* start, MgCoordinate* end, MgCoordinate* control)
{
    CHECKARGUMENTNULL(start, L"MgArcSegment.MgArcSegment");
    CHECKARGUMENTNULL(end, L"MgArcSegment.MgArcSegment");
    CHECKARGUMENTNULL(control, L"MgArcSegment.MgArcSegment");

    m_start = SAFE_ADDREF(start);
    m_end = SAFE_ADDREF(end);
    m_control = SAFE_ADDREF(control);
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgArcSegment object for deserialization
//
MgArcSegment::MgArcSegment()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry component.
//
INT32 MgArcSegment::GetComponentType()
{
    return MgGeometryComponentType::ArcSegment;
}

///////////////////////////////////////////////////////////////////////////
// Gets the control coordinate of the arc.
//
MgCoordinate* MgArcSegment::GetControlCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_control);
}

///////////////////////////////////////////////////////////////////////////
// Gets the starting coordinate of the curve segment.
//
MgCoordinate* MgArcSegment::GetStartCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_start);
}

///////////////////////////////////////////////////////////////////////////
// Gets the ending coordinate of the curve segment.
//
MgCoordinate* MgArcSegment::GetEndCoordinate()
{
    return SAFE_ADDREF((MgCoordinate*)m_end);
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgArcSegment::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgArcSegment::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgArcSegment::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgArcSegment::Copy()
{
    return new MgArcSegment(m_start, m_end, m_control);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgArcSegment::Transform(MgTransform* transform)
{
    Ptr<MgCoordinate> start = transform->Transform(m_start);
    Ptr<MgCoordinate> end = transform->Transform(m_end);
    Ptr<MgCoordinate> control = transform->Transform(m_control);

    return new MgArcSegment(start, end, control);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgArcSegment::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgArcSegment::Serialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are serialized by the containing geometries
    throw new MgNotImplementedException(L"MgArcSegment.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgArcSegment::Deserialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are deserialized by the containing geometries
    throw new MgNotImplementedException(L"MgArcSegment.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgArcSegment::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        double minX = m_start->GetX();
        double minY = m_start->GetY();
        double maxX = minX;
        double maxY = minY;

        double x = m_control->GetX();
        double y = m_control->GetY();

        if(x < minX) minX = x;
        else if(x > maxX) maxX = x;

        if(y < minY) minY = y;
        else if(y > maxY) maxY = y;

        x = m_end->GetX();
        y = m_end->GetY();

        if(x < minX) minX = x;
        else if(x > maxX) maxX = x;

        if(y < minY) minY = y;
        else if(y > maxY) maxY = y;

        Ptr<MgCoordinate> min = new MgCoordinateXY(minX, minY);
        Ptr<MgCoordinate> max = new MgCoordinateXY(maxX, maxY);
        m_envelope = new MgEnvelope(min, max);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgArcSegment::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry component is spatially equal to another geometry component.
//
bool MgArcSegment::Equals(MgGeometryComponent* other)
{
    if(other == NULL)
        return false;

    if(other->GetComponentType() != MgGeometryComponentType::ArcSegment)
        return false;

    MgArcSegment* arc = (MgArcSegment*)other;

    if(!m_control->Equals(arc->m_control))
        return false;

    return (m_start->Equals(arc->m_start) &&  m_end->Equals(arc->m_end)) ||
           (m_start->Equals(arc->m_end) &&  m_end->Equals(arc->m_start));
}

STRING MgArcSegment::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return tempAwkt;
}

void MgArcSegment::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    STRING startAwkt = L"";
    STRING controlAwkt = L"";
    STRING endAwkt = L"";

    awktStr += L"CIRCULARARCSEGMENT ";
    awktStr += L"(";

    // m_start->ToAwkt(startAwkt, coordDim); //TODO: Skip 1st co-ordinate why (findout)??

    m_control->ToAwkt(awktStr, coordDim, is2dOnly);
    awktStr += L", ";

    m_end->ToAwkt(awktStr, coordDim, is2dOnly);
    awktStr += L")";
}

MgCoordinateIterator* MgArcSegment::GetCoordinates()
{
    Ptr<MgGeometryComponent> arcToLine = MgSpatialUtility::TesselateGeometryComponent(this);
    return arcToLine->GetCoordinates();
}
