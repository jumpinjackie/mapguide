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

#ifndef VECTOR3D_H_
#define VECTOR3D_H_

#include "../MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The Vector3D class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API Vector3D
{
public:
    // Constructor(s)/Destructor
    Vector3D();
    Vector3D(double dX, double dY, double dZ);
    virtual ~Vector3D();

    // Operations
    double GetX() const;
    void SetX(double dX);

    double GetY() const;
    void SetY(double dY);

    double GetZ() const;
    void SetZ(double dZ);

private:
    // Data Members
    double m_dX;
    double m_dY;
    double m_dZ;
};

// Inline Methods
inline double Vector3D::GetX() const
{
    return m_dX;
}

inline double Vector3D::GetY() const
{
    return m_dY;
}

inline double Vector3D::GetZ() const
{
    return m_dZ;
}

END_NAMESPACE_MDFMODEL
#endif // VECTOR3D_H_
