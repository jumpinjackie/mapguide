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
// DESCRIPTION: The Point3D class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Point3D.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Point3D class.
//-------------------------------------------------------------------------
Point3D::Point3D() :
    m_dX(0.0),
    m_dY(0.0),
    m_dZ(0.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Point3D class.
//-------------------------------------------------------------------------
Point3D::Point3D(double dX, double dY, double dZ) :
    m_dX(dX),
    m_dY(dY),
    m_dZ(dZ)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Point3D::~Point3D()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this Point3D object.
//-------------------------------------------------------------------------
void Point3D::SetX(double dX)
{
    m_dX = dX;
}

void Point3D::SetY(double dY)
{
    m_dY = dY;
}

void Point3D::SetZ(double dZ)
{
    m_dZ = dZ;
}
