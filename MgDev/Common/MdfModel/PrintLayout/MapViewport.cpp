//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
// DESCRIPTION: The MapViewport class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MapViewport.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the MapViewport class.
//-------------------------------------------------------------------------
MapViewport::MapViewport() :
    m_orientation(0.0),
    m_isLocked(false),
    m_isOn(false)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
MapViewport::~MapViewport()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this MapViewport object.
//-------------------------------------------------------------------------
void MapViewport::SetOrientation(double orientation)
{
    m_orientation = orientation;
}

void MapViewport::SetMapName(const MdfString& mapName)
{
    m_mapName = mapName;
}

void MapViewport::SetIsLocked(bool isLocked)
{
    m_isLocked = isLocked;
}

void MapViewport::SetIsOn(bool isOn)
{
    m_isOn = isOn;
}
