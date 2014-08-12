//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "stdafx.h"
#include "TileSetDefinition.h"

// Construction, destruction, initialization.
TileSetDefinition::TileSetDefinition()
    : m_boxExtents(0.0, 0.0)
{

}

TileSetDefinition::~TileSetDefinition()
{

}

// Property: TileStoreParameters
// Defines the parameters to access the tile cache
TileStoreParameters* TileSetDefinition::GetTileStoreParameters()
{
    return &this->m_parameters;
}

// Property : Extents
// The extents to be used by the MapDefinition
const Box2D& TileSetDefinition::GetExtents() const
{
    return this->m_boxExtents;
}

void TileSetDefinition::SetExtents(const Box2D& boxExtents)
{
    this->m_boxExtents = boxExtents;
}

// Property : BaseMapLayerGroups
// The base map groups; used to define tiles for the HTML viewer.
BaseMapLayerGroupCollection* TileSetDefinition::GetBaseMapLayerGroups()
{
    return &this->m_baseMapLayerGroups;
}