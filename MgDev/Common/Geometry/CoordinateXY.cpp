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

MG_IMPL_DYNCREATE(MgCoordinateXY)

///////////////////////////////////////////////////////////////////////////
// Construct a MgCoordinateXY object
//
MgCoordinateXY::MgCoordinateXY(double x, double y)
{
    m_x = x;
    m_y = y;
}

///////////////////////////////////////////////////////////////////////////
// Construct an empty MgCoordinateXY object for deserialization
//
MgCoordinateXY::MgCoordinateXY()
{
}

///////////////////////////////////////////////////////////////////////////
// Defines the dimensions supported by this coordinate as a bit mask. The
// return can be a combination of the values of MgCoordinateDimension.
// See MgCoordinateDimension for an explaination of the possible return
// values.
//
INT32 MgCoordinateXY::GetDimension()
{
    return MgCoordinateDimension::XY;
}

///////////////////////////////////////////////////////////////////////////
// Gets the X value of this coordinate.
//
double MgCoordinateXY::GetX()
{
    return m_x;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Y value of this coordinate.
//
double MgCoordinateXY::GetY()
{
    return m_y;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Z value of this coordinate.
//
double MgCoordinateXY::GetZ()
{
    return MgCoordinate::NoZ;
}

///////////////////////////////////////////////////////////////////////////
// Gets the M value of this coordinate.
//
double MgCoordinateXY::GetM()
{
    return MgCoordinate::NoM;
}

///////////////////////////////////////////////////////////////////////////
// Sets the X value of this coordinate.
//
void MgCoordinateXY::SetX(double x)
{
    m_x=x;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Y value of this coordinate.
//
void MgCoordinateXY::SetY(double y)
{
    m_y=y;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Z value of this coordinate.
//
void MgCoordinateXY::SetZ(double z)
{
}
///////////////////////////////////////////////////////////////////////////
// Sets the M value of this coordinate.
//
void MgCoordinateXY::SetM(double m)
{
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCoordinateXY::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this object
//
MgCoordinate* MgCoordinateXY::Copy()
{
    return new MgCoordinateXY(m_x, m_y);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgCoordinateXY::Serialize(MgStream* stream)
{
    stream->WriteDouble(m_x);
    stream->WriteDouble(m_y);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgCoordinateXY::Deserialize(MgStream* stream)
{
    stream->GetDouble(m_x);
    stream->GetDouble(m_y);
}

//////////////////////////////////////////////////////////////////
// Test 2 coordinates for equality
// Note, both coordinates must be XZ
//
bool MgCoordinateXY::Equals(MgCoordinate* coord)
{
    if (coord == NULL)
    {
        return false;
    }

    MgCoordinateXY* c = dynamic_cast<MgCoordinateXY*>(coord);
    if (!c)
    {
        return false;
    }

    return m_x == c->m_x &&
           m_y == c->m_y;
}

// Convert to Xml representation
void MgCoordinateXY::ToXml(string &str)
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
}


void MgCoordinateXY::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    std::string coordAwkt, x, y;

    MgUtil::DoubleToString(m_x, x);
    MgUtil::DoubleToString(m_y, y);

    coordAwkt = x + GEOMETRY_SEP + y;

    awktStr += MgUtil::MultiByteToWideChar(coordAwkt);
    coordDim = L"";
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCoordinateXY::GetClassId()
{
    return m_cls_id;
}
