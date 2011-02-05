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

MG_IMPL_DYNCREATE(MgCoordinateXYM)

///////////////////////////////////////////////////////////////////////////
// Construct a MgCoordinateXYM object
//
MgCoordinateXYM::MgCoordinateXYM(double x, double y, double m)
{
    m_x = x;
    m_y = y;
    m_m = m;
}

///////////////////////////////////////////////////////////////////////////
// Construct an empty MgCoordinateXYM object for deserialization
//
MgCoordinateXYM::MgCoordinateXYM()
{
}

///////////////////////////////////////////////////////////////////////////
// Defines the dimensions supported by this coordinate as a bit mask. The
// return can be a combination of the values of MgCoordinateDimension.
// See MgCoordinateDimension for an explaination of the possible return
// values.
//
INT32 MgCoordinateXYM::GetDimension()
{
    return MgCoordinateDimension::XY | MgCoordinateDimension::M;
}

///////////////////////////////////////////////////////////////////////////
// Gets the X value of this coordinate.
//
double MgCoordinateXYM::GetX()
{
    return m_x;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Y value of this coordinate.
//
double MgCoordinateXYM::GetY()
{
    return m_y;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Z value of this coordinate.
//
double MgCoordinateXYM::GetZ()
{
    return MgCoordinate::NoZ;
}

///////////////////////////////////////////////////////////////////////////
// Gets the M value of this coordinate.
//
double MgCoordinateXYM::GetM()
{
    return m_m;
}

///////////////////////////////////////////////////////////////////////////
// Sets the X value of this coordinate.
//
void MgCoordinateXYM::SetX(double x)
{
    m_x=x;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Y value of this coordinate.
//
void MgCoordinateXYM::SetY(double y)
{
    m_y=y;
}

///////////////////////////////////////////////////////////////////////////
// Sets the Z value of this coordinate.
//
void MgCoordinateXYM::SetZ(double z)
{
}
///////////////////////////////////////////////////////////////////////////
// Sets the M value of this coordinate.
//
void MgCoordinateXYM::SetM(double m)
{
    m_m=m;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCoordinateXYM::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this object
//
MgCoordinate* MgCoordinateXYM::Copy()
{
    return new MgCoordinateXYM(m_x, m_y, m_m);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgCoordinateXYM::Serialize(MgStream* stream)
{
    stream->WriteDouble(m_x);
    stream->WriteDouble(m_y);
    stream->WriteDouble(m_m);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgCoordinateXYM::Deserialize(MgStream* stream)
{
    stream->GetDouble(m_x);
    stream->GetDouble(m_y);
    stream->GetDouble(m_m);
}

//////////////////////////////////////////////////////////////////
// Test 2 coordinates for equality
// Note, both coordinates must be XYM
//
bool MgCoordinateXYM::Equals(MgCoordinate* coord)
{
    if(coord == NULL)
    {
        return false;
    }

    MgCoordinateXYM* c = dynamic_cast<MgCoordinateXYM*>(coord);
    if (!c)
    {
        return false;
    }

    return m_x == c->m_x &&
           m_y == c->m_y &&
           m_m == c->m_m;
}

// Convert to Xml representation
void MgCoordinateXYM::ToXml(string &str)
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

    // M co-ordinate
    str += "<M>";

    MgUtil::DoubleToString(m_m, doubleStr);
    str += doubleStr;

    str += "</M>";
}

void MgCoordinateXYM::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    std::string coordAwkt, x, y, m;

    MgUtil::DoubleToString(m_x, x);
    MgUtil::DoubleToString(m_y, y);
    MgUtil::DoubleToString(m_m, m);

    if (!is2dOnly)
    {
        coordAwkt = x + GEOMETRY_SEP + y + GEOMETRY_SEP + m;
        awktStr += MgUtil::MultiByteToWideChar(coordAwkt);
        coordDim = L"XYM ";
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
INT32 MgCoordinateXYM::GetClassId()
{
    return m_cls_id;
}
