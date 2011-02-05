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
// DESCRIPTION: The MapView class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "MapView.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the MapView class.
//-------------------------------------------------------------------------
MapView::MapView() :
    m_height(0.0),
    m_rotation(0.0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
MapView::~MapView()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this MapView object.
//-------------------------------------------------------------------------
void MapView::SetHeight(double height)
{
    m_height = height;
}

void MapView::SetRotation(double rotation)
{
    m_rotation = rotation;
}

void MapView::SetModelUnits(const MdfString& modelUnits)
{
    m_modelUnits = modelUnits;
}
