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
// DESCRIPTION: The PrintLayoutElement class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "PrintLayoutElement.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the PrintLayoutElement class.
//-------------------------------------------------------------------------
PrintLayoutElement::PrintLayoutElement() :
    m_width(0.0),
    m_height(0.0),
    m_rotation(0.0),
    m_isVisible(true),
    m_opacity(1.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
PrintLayoutElement::~PrintLayoutElement()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this PrintLayoutElement object.
//-------------------------------------------------------------------------
void PrintLayoutElement::SetName(const MdfString& name)
{
    m_name = name;
}

void PrintLayoutElement::SetResourceId(const MdfString& resourceId)
{
    m_resourceId = resourceId;
}

void PrintLayoutElement::SetWidth(double width)
{
    m_width = width;
}

void PrintLayoutElement::SetHeight(double height)
{
    m_height = height;
}

void PrintLayoutElement::SetRotation(double rotation)
{
    m_rotation = rotation;
}

void PrintLayoutElement::SetUnits(const MdfString& units)
{
    m_units = units;
}

void PrintLayoutElement::SetIsVisible(bool isVisible)
{
    m_isVisible = isVisible;
}

void PrintLayoutElement::SetOpacity(double opacity)
{
    m_opacity = opacity;
}
