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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"
#include "CoordSysUtil.h"

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an "null" envelope.
//
CEnvelope::CEnvelope()
{
    m_lowerLeftX = DoubleNan;
    m_lowerLeftY = DoubleNan;
    m_lowerLeftZ = DoubleNan;
    m_upperRightX = DoubleNan;
    m_upperRightY = DoubleNan;
    m_upperRightZ = DoubleNan;
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for the specified coordinate.
//
CEnvelope::CEnvelope(double x, double y)
{
    m_lowerLeftX = x;
    m_lowerLeftY = y;
    m_lowerLeftZ = DoubleNan;
    m_upperRightX = x;
    m_upperRightY = y;
    m_upperRightZ = DoubleNan;
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for the specified coordinate.
//
CEnvelope::CEnvelope(double x, double y, double z)
{
    m_lowerLeftX = x;
    m_lowerLeftY = y;
    m_lowerLeftZ = z;
    m_upperRightX = x;
    m_upperRightY = y;
    m_upperRightZ = z;
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for the specified coordinates.
//
CEnvelope::CEnvelope(double x1, double y1, double x2, double y2)
{
    if(x1 < x2)
    {
        // x1 is lower left
        m_lowerLeftX = x1;
        m_lowerLeftY = y1;
        m_lowerLeftZ = DoubleNan;

        m_upperRightX = x2;
        m_upperRightY = y2;
        m_upperRightZ = DoubleNan;
    }
    else if(x1 > x2)
    {
        // x2 is lower left
        m_lowerLeftX = x2;
        m_lowerLeftY = y2;
        m_lowerLeftZ = DoubleNan;

        m_upperRightX = x1;
        m_upperRightY = y1;
        m_upperRightZ = DoubleNan;
    }
    else if(x1 == x2)
    {
        if(y1 < y2)
        {
            // x1 is lower left
            m_lowerLeftX = x1;
            m_lowerLeftY = y1;
            m_lowerLeftZ = DoubleNan;

            m_upperRightX = x2;
            m_upperRightY = y2;
            m_upperRightZ = DoubleNan;
        }
        else
        {
            // x2 is lower left
            m_lowerLeftX = x2;
            m_lowerLeftY = y2;
            m_lowerLeftZ = DoubleNan;

            m_upperRightX = x1;
            m_upperRightY = y1;
            m_upperRightZ = DoubleNan;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope for the specified coordinates.
//
CEnvelope::CEnvelope(double x1, double y1, double z1, double x2, double y2, double z2)
{
    if(x1 < x2)
    {
        // x1 is lower left
        m_lowerLeftX = x1;
        m_lowerLeftY = y1;
        m_lowerLeftZ = z1;

        m_upperRightX = x2;
        m_upperRightY = y2;
        m_upperRightZ = z2;
    }
    else if(x1 > x2)
    {
        // x2 is lower left
        m_lowerLeftX = x2;
        m_lowerLeftY = y2;
        m_lowerLeftZ = z2;

        m_upperRightX = x1;
        m_upperRightY = y1;
        m_upperRightZ = z1;
    }
    else if(x1 == x2)
    {
        if(y1 < y2)
        {
            // x1 is lower left
            m_lowerLeftX = x1;
            m_lowerLeftY = y1;
            m_lowerLeftZ = z1;

            m_upperRightX = x2;
            m_upperRightY = y2;
            m_upperRightZ = z2;
        }
        else
        {
            // x2 is lower left
            m_lowerLeftX = x2;
            m_lowerLeftY = y2;
            m_lowerLeftZ = z2;

            m_upperRightX = x1;
            m_upperRightY = y1;
            m_upperRightZ = z1;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// Constructs and intializes an envelope that is a copy of another.
//
CEnvelope::CEnvelope(CEnvelope* envelope)
{
    if (envelope == NULL)
    {
        throw new CNullArgumentException(L"CEnvelope.CEnvelope", __LINE__, __WFILE__, L"[1] - CEnvelope pointer.");
    }

    m_lowerLeftX = envelope->GetLowerLeftXCoordinate();
    m_lowerLeftY = envelope->GetLowerLeftYCoordinate();
    m_lowerLeftZ = envelope->GetLowerLeftZCoordinate();
    m_upperRightX = envelope->GetUpperRightXCoordinate();
    m_upperRightY = envelope->GetUpperRightYCoordinate();
    m_upperRightZ = envelope->GetUpperRightZCoordinate();
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum X values.
//
double CEnvelope::GetWidth()
{
    if(IsNull())
        return DoubleNan;

    return m_upperRightX - m_lowerLeftX;
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum Y values.
//
double CEnvelope::GetHeight()
{
    if(IsNull())
        return DoubleNan;

    return m_upperRightY - m_lowerLeftY;
}

///////////////////////////////////////////////////////////////////////////
// Returns the difference between the maximum and minimum Z values.
//
double CEnvelope::GetDepth()
{
    //must have a Z dimension
    if(IsNull() || (IsDoubleNan(m_lowerLeftZ)) || (IsDoubleNan(m_upperRightZ)))
        return DoubleNan;

    return fabs(m_lowerLeftZ - m_upperRightZ);
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this Envelope is "null" or not.
//
bool CEnvelope::IsNull()
{
    //coordinates are either both NULL or none are NULL
    return m_lowerLeftX == DoubleNan;
}

///////////////////////////////////////////////////////////////////////////
// Makes this envelope a "null" envelope.
//
void CEnvelope::MakeNull()
{
    m_lowerLeftX = DoubleNan;
    m_lowerLeftY = DoubleNan;
    m_lowerLeftZ = DoubleNan;
    m_upperRightX = DoubleNan;
    m_upperRightY = DoubleNan;
    m_upperRightZ = DoubleNan;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Dimension of this Envelope.
//
int CEnvelope::GetDimension()
{
    if((IsDoubleNan(m_lowerLeftZ)) || (IsDoubleNan(m_upperRightZ)))
        return CCoordinateSystemDimension::XY;
    else
        return CCoordinateSystemDimension::XYZ;
}

///////////////////////////////////////////////////////////////////////////
// Enlarges the boundary of the envelope so that it contains the given
// coordinate.
//
void CEnvelope::ExpandToInclude(double x, double y)
{
    if (IsNull())
    {
        m_lowerLeftX = x;
        m_lowerLeftY = y;
        m_lowerLeftZ = DoubleNan;
        m_upperRightX = x;
        m_upperRightY = y;
        m_upperRightZ = DoubleNan;
    }
    else
    {
        m_lowerLeftX = m_lowerLeftX > x ? x : m_lowerLeftX;
        m_lowerLeftY = m_lowerLeftY > y ? y : m_lowerLeftY;
        m_lowerLeftZ = DoubleNan;

        m_upperRightX = m_upperRightX < x ? x : m_upperRightX;
        m_upperRightY = m_upperRightY < y ? y : m_upperRightY;
        m_upperRightZ = DoubleNan;
    }
}

///////////////////////////////////////////////////////////////////////////
// Enlarges the boundary of the envelope so that it contains the given
// coordinate.
//
void CEnvelope::ExpandToInclude(double x, double y, double z)
{
    if (IsNull())
    {
        m_lowerLeftX = x;
        m_lowerLeftY = y;
        m_lowerLeftZ = z;
        m_upperRightX = x;
        m_upperRightY = y;
        m_upperRightZ = z;
    }
    else
    {
        m_lowerLeftX = m_lowerLeftX > x ? x : m_lowerLeftX;
        m_lowerLeftY = m_lowerLeftY > y ? y : m_lowerLeftY;
        m_lowerLeftZ = m_lowerLeftZ > z ? z : m_lowerLeftZ;

        m_upperRightX = m_upperRightX < x ? x : m_upperRightX;
        m_upperRightY = m_upperRightY < y ? y : m_upperRightY;
        m_upperRightZ = m_upperRightZ < z ? z : m_upperRightZ;
    }
}

///////////////////////////////////////////////////////////////////////////
// Enlarges the boundary of the envelope so that it contains the given
// envelope.
//
void CEnvelope::ExpandToInclude(CEnvelope* envelope)
{
    if (envelope == NULL)
    {
        throw new CNullArgumentException(L"CEnvelope.ExpandToInclude", __LINE__, __WFILE__, L"[1] - CEnvelope pointer.");
    }

    if(envelope->GetDimension() == CCoordinateSystemDimension::XYZ)
    {
        ExpandToInclude(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate(), envelope->GetLowerLeftZCoordinate());
        ExpandToInclude(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate(), envelope->GetUpperRightZCoordinate());
    }
    else
    {
        ExpandToInclude(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate());
        ExpandToInclude(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate());
    }
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the given coordinate lies in or on this envelope.
//
bool CEnvelope::Contains(double x, double y)
{
    if (IsNull())
        return false;

    if(x < m_lowerLeftX || x > m_upperRightX || y < m_lowerLeftY || y > m_upperRightY)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the given coordinate lies in or on this envelope.
//
bool CEnvelope::Contains(double x, double y, double z)
{
    if (IsNull())
        return false;

    if(x < m_lowerLeftX || x > m_upperRightX || y < m_lowerLeftY || y > m_upperRightY || z < m_lowerLeftZ || z > m_upperRightZ)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the given envelope lies wholely inside this Envelope
// (inclusive of the boundary).
//
bool CEnvelope::Contains(CEnvelope* envelope)
{
    if (envelope == NULL)
    {
        throw new CNullArgumentException(L"CEnvelope.Contains", __LINE__, __WFILE__, L"[1] - CEnvelope pointer.");
    }

    if(envelope->GetDimension() == CCoordinateSystemDimension::XYZ)
    {
        return Contains(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate(), envelope->GetLowerLeftZCoordinate()) &&
               Contains(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate(), envelope->GetUpperRightZCoordinate());
    }
    else
    {
        return Contains(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate()) &&
               Contains(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate());
    }
}

///////////////////////////////////////////////////////////////////////////
// Tests if the region defined by another envelope overlaps (intersects)
// the region of this envelope.
//</returns>
bool CEnvelope::Intersects(CEnvelope* envelope)
{
    if (envelope == NULL)
    {
        throw new CNullArgumentException(L"CEnvelope.Intersects", __LINE__, __WFILE__, L"[1] - CEnvelope pointer.");
    }

    if(envelope->GetDimension() == CCoordinateSystemDimension::XYZ)
    {
        return Contains(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate(), envelope->GetLowerLeftZCoordinate()) ||
               Contains(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate(), envelope->GetUpperRightZCoordinate());
    }
    else
    {
        return Contains(envelope->GetLowerLeftXCoordinate(), envelope->GetLowerLeftYCoordinate()) ||
               Contains(envelope->GetUpperRightXCoordinate(), envelope->GetUpperRightYCoordinate());
    }
}
