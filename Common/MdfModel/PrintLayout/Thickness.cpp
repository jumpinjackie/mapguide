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
// DESCRIPTION: The Thickness class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Thickness.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Thickness class.
//-------------------------------------------------------------------------
Thickness::Thickness() :
    m_left(0.0),
    m_top(0.0),
    m_right(0.0),
    m_bottom(0.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Thickness class.
//-------------------------------------------------------------------------
Thickness::Thickness(double left, double top, double right, double bottom) :
    m_left(left),
    m_top(top),
    m_right(right),
    m_bottom(bottom)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Thickness::~Thickness()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this Thickness object.
//-------------------------------------------------------------------------
void Thickness::SetLeft(double left)
{
    m_left = left;
}

void Thickness::SetTop(double top)
{
    m_top = top;
}

void Thickness::SetRight(double right)
{
    m_right = right;
}

void Thickness::SetBottom(double bottom)
{
    m_bottom = bottom;
}
