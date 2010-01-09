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
#include "Size2D.h"

MG_IMPL_DYNCREATE(MgSize2D);

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the MgSize2D class with
// the specified components.
MgSize2D::MgSize2D(const double height, const double width)
{
    m_height = height;
    m_width = width;
}

// Copy constructor
MgSize2D::MgSize2D(const MgSize2D& size)
{
    m_height = size.m_height;
    m_width = size.m_width;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgSize2D object.
///
MgSize2D::MgSize2D()
{
    m_height = 0.0;
    m_width = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgSize2D object.
///
MgSize2D::~MgSize2D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgSize2D::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgSize2D::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgSize2D::Serialize(MgStream* stream)
{
    stream->WriteDouble(m_height);
    stream->WriteDouble(m_width);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgSize2D::Deserialize(MgStream* stream)
{
    stream->GetDouble(m_height);
    stream->GetDouble(m_width);
}

//-------------------------------------------------------
// MgSize2D implementation
//-------------------------------------------------------

// Returns whether this vector is equal to the supplied vector.
bool MgSize2D::isEqualTo(const MgSize2D& size, double tolerance) const
{
    double xDiff = m_height - size.m_height;
    double yDiff = m_width - size.m_width;
    return xDiff*xDiff + yDiff*yDiff <= tolerance*tolerance;
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
MgSize2D& MgSize2D::operator=(const MgSize2D& size)
{
    m_height = size.m_height;
    m_width = size.m_width;
    return *this;
}

// Operator that determines whether two vectors are equal.  Two vectors
// are equal if they have the same components.
bool MgSize2D::operator==(const MgSize2D& size) const
{
    return isEqualTo(size);
}

// Operator that determines whether two vectors are unequal.  Two vectors
// are unequal if they have one or more different components.
bool MgSize2D::operator!=(const MgSize2D& size) const
{
    return !(operator==(size));
}

/// Gets the height component of the size.
double MgSize2D::GetHeight() const
{
    return m_height;
}

/// Gets the width component of the size.
double MgSize2D::GetWidth() const
{
    return m_width;
}
