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
#include "Point3D.h"

MG_IMPL_DYNCREATE(MgPoint3D);

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the MgPoint3D class with
// the specified components.
MgPoint3D::MgPoint3D(const double X, const double Y, const double Z)
{
    x = X;
    y = Y;
    z = Z;
}

// Copy constructor
MgPoint3D::MgPoint3D(const MgPoint3D& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPoint3D object.
///
MgPoint3D::MgPoint3D()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPoint3D object.
///
MgPoint3D::~MgPoint3D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPoint3D::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPoint3D::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPoint3D::Serialize(MgStream* stream)
{
    stream->WriteDouble(x);
    stream->WriteDouble(y);
    stream->WriteDouble(z);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPoint3D::Deserialize(MgStream* stream)
{
    stream->GetDouble(x);
    stream->GetDouble(y);
    stream->GetDouble(z);
}

//-------------------------------------------------------
// MgPoint3D implementation
//-------------------------------------------------------

// Returns whether this vector is equal to the supplied vector.
bool MgPoint3D::isEqualTo(const MgPoint3D& vec, double tolerance) const
{
    double xDiff = x - vec.x;
    double yDiff = y - vec.y;
    double zDiff = z - vec.z;
    return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
}

// Returns the normal of the vector.
MgPoint3D MgPoint3D::normal(const double& tolerance) const
{
    double nx = x;
    double ny = y;
    double nz = z;
    double magSq = x*x + y*y + z*z;
    if (magSq > tolerance && magSq != 1.0)
    {
        double lengthInv = 1.0 / sqrt(magSq);
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;
    }

    return MgPoint3D(nx, ny, nz);
}

// Normalize the vector.  This ensures the vector has unit length.
MgPoint3D& MgPoint3D::normalize(const double& tolerance)
{
    double magSq = x*x + y*y + z*z;
    if (magSq > tolerance && magSq != 1.0)
    {
        double lengthInv = 1.0 / sqrt(magSq);
        x *= lengthInv;
        y *= lengthInv;
        z *= lengthInv;
    }

    return *this;
}

// Returns the length of this vector.
double MgPoint3D::length() const
{
    return sqrt(x*x + y*y + z*z);
}

// Returns the length squared of this vector.
double MgPoint3D::lengthSqrd() const
{
    return x*x + y*y + z*z;
}

// Returns the dot product of this vector with the supplied vector.
double MgPoint3D::dotProduct(const MgPoint3D& vec) const
{
    return x*vec.x + y*vec.y + z*vec.z;
}

// Returns the cross product of this vector with the supplied vector.
MgPoint3D MgPoint3D::crossProduct(const MgPoint3D& vec) const
{
    return MgPoint3D(y*vec.z - z*vec.y,
                    z*vec.x - x*vec.z,
                    x*vec.y - y*vec.x);
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
MgPoint3D& MgPoint3D::operator=(const MgPoint3D& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

// Operator that determines whether two vectors are equal.  Two vectors
// are equal if they have the same components.
bool MgPoint3D::operator==(const MgPoint3D& vec) const
{
    return isEqualTo(vec);
}

// Operator that determines whether two vectors are unequal.  Two vectors
// are unequal if they have one or more different components.
bool MgPoint3D::operator!=(const MgPoint3D& vec) const
{
    return !(operator==(vec));
}

// Operator that scales a vector.
MgPoint3D MgPoint3D::operator*(const double scale) const
{
    return MgPoint3D(x * scale, y * scale, z * scale);
}

// Operator that inverse-scales a vector.
MgPoint3D MgPoint3D::operator/(const double scale) const
{
    return MgPoint3D(x / scale, y / scale, z / scale);
}

// Operator that adds two vectors.
MgPoint3D MgPoint3D::operator+(const MgPoint3D& vec) const
{
    return MgPoint3D(x + vec.x, y + vec.y, z + vec.z);
}

// Operator that subtracts two vectors.
MgPoint3D MgPoint3D::operator-(const MgPoint3D& vec) const
{
    return MgPoint3D(x - vec.x, y - vec.y, z - vec.z);
}

// Operator that negates a vector.
MgPoint3D MgPoint3D::operator-() const
{
    return MgPoint3D(-x, -y, -z);
}

// Operator to access x, y, z via indexer.  0 represents the X component of
// the vector, 1 represents the Y component of the vector, and 2 represents
// the Z component of the vector.
double MgPoint3D::operator[](const unsigned int i) const
{
    assert(i >= 0 && i <= 2);
    return *(&x+i);
}

// Operator to access x, y, z via indexer.  0 represents the X component of
// the vector, 1 represents the Y component of the vector, and 2 represents
// the Z component of the vector.
double& MgPoint3D::operator[](const unsigned int i)
{
    assert(i >= 0 && i <= 2);
    return *(&x+i);
}
