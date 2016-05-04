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
// DESCRIPTION: The Size2D class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Size2D.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Size2D class.
//-------------------------------------------------------------------------
Size2D::Size2D() :
    m_width(0.0),
    m_height(0.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Size2D class.
//-------------------------------------------------------------------------
Size2D::Size2D(double width, double height) :
    m_width(width),
    m_height(height)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Size2D::~Size2D()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this Size2D object.
//-------------------------------------------------------------------------
void Size2D::SetWidth(double width)
{
    m_width = width;
}

void Size2D::SetHeight(double height)
{
    m_height = height;
}
