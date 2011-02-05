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

//-------------------------------------------------------------------------
// DESCRIPTION: The Vector3D class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Vector3D.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Vector3D class.
//-------------------------------------------------------------------------
Vector3D::Vector3D() :
    m_dX(0.0),
    m_dY(0.0),
    m_dZ(0.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Vector3D class.
//-------------------------------------------------------------------------
Vector3D::Vector3D(double dX, double dY, double dZ) :
    m_dX(dX),
    m_dY(dY),
    m_dZ(dZ)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Vector3D::~Vector3D()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this Vector3D object.
//-------------------------------------------------------------------------
void Vector3D::SetX(double dX)
{
    m_dX = dX;
}

void Vector3D::SetY(double dY)
{
    m_dY = dY;
}

void Vector3D::SetZ(double dZ)
{
    m_dZ = dZ;
}
