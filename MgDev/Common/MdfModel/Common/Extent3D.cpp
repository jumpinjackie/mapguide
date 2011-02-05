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
// DESCRIPTION: The Extent3D class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Extent3D.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Extent3D class.
//-------------------------------------------------------------------------
Extent3D::Extent3D()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Extent3D class.
//-------------------------------------------------------------------------
Extent3D::Extent3D(const Point3D& minPt, const Point3D& maxPt) :
    m_minPt(minPt),
    m_maxPt(maxPt)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Extent3D::~Extent3D()
{
}
