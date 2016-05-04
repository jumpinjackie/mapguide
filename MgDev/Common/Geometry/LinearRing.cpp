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
MgLinearRing::MgLinearRing(MgCoordinateCollection* coordinates)
{
    CHECKARGUMENTNULL(coordinates, L"MgLinearRing.MgLinearRing");

    if (coordinates->GetCount() < 3)
    {
        STRING buffer;
        MgUtil::Int32ToString(coordinates->GetCount(), buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgLinearRing.MgLinearRing",
            __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryTooFewCoordinates", NULL);
    }

    m_coordinates = coordinates->Copy();
}

///////////////////////////////////////////////////////////////////////////
// Construct an empty MgLinearRing object for deserialization
//
MgLinearRing::MgLinearRing()
{
}

///////////////////////////////////////////////////////////////////////////
// Returns an iterator that can be used to enumerate each of the linear
// rings coordinates.
//
MgCoordinateIterator* MgLinearRing::GetCoordinates()
{
    return new MgCoordinateIterator(m_coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry component.
//
INT32 MgLinearRing::GetComponentType()
{
    return MgGeometryComponentType::LinearRing;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgLinearRing::GetDimension()
{
    return MgGeometricDimension::Region;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgLinearRing::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgLinearRing::IsClosed()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgLinearRing::Copy()
{
    return new MgLinearRing(m_coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgLinearRing::Transform(MgTransform* transform)
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();

    INT32 count = m_coordinates->GetCount();

    for (int i = 0; i < count; i++)
    {
        Ptr<MgCoordinate> coord = m_coordinates->GetItem(i);
        Ptr<MgCoordinate> newCoord = transform->Transform(coord);
        coords->Add(newCoord);
    }

    return new MgLinearRing(coords);
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgLinearRing::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeLineStringEnvelope(m_coordinates);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgLinearRing::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgLinearRing::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Returns the coordinates for this ring
// rings coordinates.
//
MgCoordinateCollection* MgLinearRing::GetCoordinateCollection()
{
    return SAFE_ADDREF((MgCoordinateCollection*)m_coordinates);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgLinearRing::Serialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are serialized by the containing geometries
    throw new MgNotImplementedException(L"MgLinearRing.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgLinearRing::Deserialize(MgStream* stream)
{
    //should not be called on geometry components
    //components are deserialized by the containing geometries
    throw new MgNotImplementedException(L"MgLinearRing.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry component is spatially equal to another geometry component.
//
bool MgLinearRing::Equals(MgGeometryComponent* other)
{
    if(other == NULL)
        return false;

    if(other->GetComponentType() != MgGeometryComponentType::LinearRing)
        return false;

    return MgGeometryUtil::CoordinatesEqual(m_coordinates, ((MgLinearRing*)other)->m_coordinates);
}

void MgLinearRing::ToXml(std::string& str)
{
    str += "<LinearRing>";

    str += "<Coordinates>";

    INT32 cnt = m_coordinates->GetCount();
    for (INT32 i=0; i < cnt; i++)
    {
        str += "<Coordinate>";
        Ptr<MgCoordinate> coord = m_coordinates->GetItem(i);
        coord->ToXml(str);
        str += "</Coordinate>";
    }

    str += "</Coordinates>";

    str += "</LinearRing>";
}

STRING MgLinearRing::ToAwkt(bool is2dOnly)
{
    STRING awktStr, coordDim;

    ToAwkt(awktStr, coordDim, is2dOnly);

    return awktStr;
}

void MgLinearRing::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";
    m_coordinates->ToAwkt(awktStr, coordDim, is2dOnly);
    awktStr += L")";
}
