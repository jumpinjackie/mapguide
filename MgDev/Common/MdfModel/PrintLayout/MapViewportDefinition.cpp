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
// DESCRIPTION: The MapViewportDefinition class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "MapViewportDefinition.h"

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the MapViewportDefinition class.
//-------------------------------------------------------------------------
MapViewportDefinition::MapViewportDefinition() :
    m_isLocked(false),
    m_isOn(true)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
MapViewportDefinition::~MapViewportDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this MapViewportDefinition object.
//-------------------------------------------------------------------------
const MdfString& MapViewportDefinition::GetType()
{
    return PrintLayoutElementDefinitionType::MapViewportDefinition;
}

void MapViewportDefinition::SetMapName(const MdfString& mapName)
{
    m_mapName = mapName;
}

void MapViewportDefinition::SetIsLocked(bool isLocked)
{
    m_isLocked = isLocked;
}

void MapViewportDefinition::SetIsOn(bool isOn)
{
    m_isOn = isOn;
}
