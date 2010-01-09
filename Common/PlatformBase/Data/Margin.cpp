//
//  Copyright (C) 2005-2010 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "Margin.h"

MG_IMPL_DYNCREATE(MgMargin);

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the MgMargin class with
// the specified components.
MgMargin::MgMargin(const double left, const double top, const double right, const double bottom)
{
    m_left = left;
    m_right = right;
    m_top = top;
    m_bottom = bottom;
}

// Copy constructor
MgMargin::MgMargin(const MgMargin& margin)
{
    m_left = margin.m_left;
    m_right = margin.m_right;
    m_top = margin.m_top;
    m_bottom = margin.m_bottom;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgMargin object.
///
MgMargin::MgMargin()
{
    m_left = 0.0;
    m_right = 0.0;
    m_top = 0.0;
    m_bottom = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgMargin object.
///
MgMargin::~MgMargin()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgMargin::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgMargin::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgMargin::Serialize(MgStream* stream)
{
    stream->WriteDouble(m_left);
    stream->WriteDouble(m_right);
    stream->WriteDouble(m_top);
    stream->WriteDouble(m_bottom);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgMargin::Deserialize(MgStream* stream)
{
    stream->GetDouble(m_left);
    stream->GetDouble(m_right);
    stream->GetDouble(m_top);
    stream->GetDouble(m_bottom);
}

//-------------------------------------------------------
// MgMargin implementation
//-------------------------------------------------------

// Returns whether this margin is equal to the supplied margin.
bool MgMargin::isEqualTo(const MgMargin& margin, double tolerance) const
{
    double aDiff = m_left - margin.m_left;
    double bDiff = m_right - margin.m_right;
    double cDiff = m_top - margin.m_top;
    double dDiff = m_bottom - margin.m_bottom;
    return aDiff*aDiff + bDiff*bDiff + cDiff*cDiff + dDiff*dDiff <= tolerance*tolerance;
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
MgMargin& MgMargin::operator=(const MgMargin& margin)
{
    m_left = margin.m_left;
    m_right = margin.m_right;
    m_top = margin.m_top;
    m_bottom = margin.m_bottom;
    return *this;
}

// Operator that determines whether two margins are equal.  Two margins
// are equal if they have the same components.
bool MgMargin::operator==(const MgMargin& margin) const
{
    return isEqualTo(margin);
}

// Operator that determines whether two margins are unequal.  Two margins
// are unequal if they have one or more different components.
bool MgMargin::operator!=(const MgMargin& margin) const
{
    return !(operator==(margin));
}

/// Gets the left component of the margin.
double MgMargin::GetLeft() const
{
    return m_left;
}

/// Gets the right component of the margin.
double MgMargin::GetRight() const
{
    return m_right;
}

/// Gets the height component of the margin.
double MgMargin::GetTop() const
{
    return m_top;
}

/// Gets the bottom component of the margin.
double MgMargin::GetBottom() const
{
    return m_bottom;
}

