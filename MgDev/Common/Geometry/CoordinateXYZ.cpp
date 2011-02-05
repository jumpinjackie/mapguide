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

MG_IMPL_DYNCREATE(MgCoordinateXYZ)

///////////////////////////////////////////////////////////////////////////
// Construct a MgCoordinateXYZ object
//
MgCoordinateXYZ::MgCoordinateXYZ(double x, double y, double z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

///////////////////////////////////////////////////////////////////////////
// Construct an empty MgCoordinateXYZ object for deserialization
//
MgCoordinateXYZ::MgCoordinateXYZ()
{
}

///////////////////////////////////////////////////////////////////////////
// Defines the dimensions supported by this coordinate as a bit mask. The
// return can be a combination of the values of MgCoordinateDimension.
// See MgCoordinateDimension for an explaination of the possible return
// values.
//
INT32 MgCoordinateXYZ::GetDimension()
{
    return MgCoordinateDimension::XYZ;
}

///////////////////////////////////////////////////////////////////////////
// Gets the X value of this coordinate.
//
double MgCoordinateXYZ::GetX()
{
    return m_x;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Y value of this coordinate.
//
double MgCoordinateXYZ::GetY()
{
    return m_y;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Z value of this coordinate.
//
double MgCoordinateXYZ::GetZ()
{
    return m_z;
}

///////////////////////////////////////////////////////////////////////////
// Gets the M value of this coordinate.
//
double MgCoordinateXYZ::GetM()
{
    return MgCoordinate::NoM;
}

///////////////////////////////////////////////////////////////////////////
// Sets the X value of this coordinate.
//
void MgCoordinateXYZ::SetX(double x)
{
    m_x=x;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Y value of this coordinate.
//
void MgCoordinateXYZ::SetY(double y)
{
    m_y=y;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Z value of this coordinate.
//
void MgCoordinateXYZ::SetZ(double z)
{
    m_z=z;
}

///////////////////////////////////////////////////////////////////////////
// Sets the M value of this coordinate.
//
void MgCoordinateXYZ::SetM(double m)
{
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCoordinateXYZ::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this object
//
MgCoordinate* MgCoordinateXYZ::Copy()
{
    return new MgCoordinateXYZ(m_x, m_y, m_z);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgCoordinateXYZ::Serialize(MgStream* stream)
{
    stream->WriteDouble(m_x);
    stream->WriteDouble(m_y);
    stream->WriteDouble(m_z);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgCoordinateXYZ::Deserialize(MgStream* stream)
{
    stream->GetDouble(m_x);
    stream->GetDouble(m_y);
    stream->GetDouble(m_z);
}

//////////////////////////////////////////////////////////////////
// Test 2 coordinates for equality
// Note, both coordinates must be XYZ
//
bool MgCoordinateXYZ::Equals(MgCoordinate* coord)
{
    if(coord == NULL)
    {
        return false;
    }

    MgCoordinateXYZ* c = dynamic_cast<MgCoordinateXYZ*>(coord);
    if (!c)
    {
        return false;
    }

    return m_x == c->m_x &&
           m_y == c->m_y &&
           m_z == c->m_z;
}

// Convert to Xml representation
void MgCoordinateXYZ::ToXml(string &str)
{
    std::string doubleStr = "";

    // X co-ordinate
    str += "<X>";

    MgUtil::DoubleToString(m_x, doubleStr);
    str += doubleStr;

    str += "</X>";

    // Y co-ordinate
    str += "<Y>";

    MgUtil::DoubleToString(m_y, doubleStr);
    str += doubleStr;

    str += "</Y>";

    // Z co-ordinate
    str += "<Z>";

    MgUtil::DoubleToString(m_z, doubleStr);
    str += doubleStr;

    str += "</Z>";
}

void MgCoordinateXYZ::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    std::string coordAwkt, x, y, z;

    MgUtil::DoubleToString(m_x, x);
    MgUtil::DoubleToString(m_y, y);
    MgUtil::DoubleToString(m_z, z);

    if (!is2dOnly)
    {
        coordAwkt = x + GEOMETRY_SEP + y + GEOMETRY_SEP + z;
        awktStr += MgUtil::MultiByteToWideChar(coordAwkt);
        coordDim = L"XYZ ";
    }
    else
    {
        coordAwkt = x + GEOMETRY_SEP + y;
        awktStr += MgUtil::MultiByteToWideChar(coordAwkt);
        coordDim = L"";
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCoordinateXYZ::GetClassId()
{
    return m_cls_id;
}
