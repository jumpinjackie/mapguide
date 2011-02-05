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
// DESCRIPTION: The Color class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "Color.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the Color class.
//-------------------------------------------------------------------------
Color::Color() :
    m_red(0),
    m_green(0),
    m_blue(0),
    m_alpha(0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Color class.
//-------------------------------------------------------------------------
Color::Color(int red, int green, int blue, int alpha) :
    m_red(red),
    m_green(green),
    m_blue(blue),
    m_alpha(alpha)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Color::~Color()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this Color object.
//-------------------------------------------------------------------------
void Color::SetRed(int red)
{
    m_red = red;
}

void Color::SetGreen(int green)
{
    m_green = green;
}

void Color::SetBlue(int blue)
{
    m_blue = blue;
}

void Color::SetAlpha(int alpha)
{
    m_alpha = alpha;
}
