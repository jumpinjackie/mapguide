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
// DESCRIPTION:
// The MapDefinition class implementation.
// All the data objects in the MapDefinition (MapLayers, etc) are accessible through
// type safe collection classes. While these collection classes provide unfettered
// access to all the stored objects, they are owned by the MapDefinition object.
// Methods that have a prefix of "Adopt", imply that the object passed to them
// has to be created on the heap and the MapDefinition object is now responsible for its
// deletion. Methods that have a prefix of "Orphan" imply that the pointer returned,
// points to an object on the heap and its is the caller who is responsible for
// its deletion. Methods that have a "Get" prefix simply give access without a change
// of ownership.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MapDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the MapDefinition class.
// PARAMETERS:
//     Input:
//          strName - the MapDefinition name that is copied
//          strCoordinateSystem - the coordinate system string.
//                             The string is an SRS string
//-------------------------------------------------------------------------
MapDefinition::MapDefinition(const MdfString& strName,
                             const MdfString& strCoordinateSystem)
: m_strName(strName)
, m_strCoordSys(strCoordinateSystem)
, m_boxExtents(0.0, 0.0)
{
    // default values
    this->m_strBkGrnd = L"ffffffff"; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
MapDefinition::~MapDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Is an accessor method for the MapDefinition name.
// RETURNS: MapDefinition name.
//-------------------------------------------------------------------------
const MdfString& MapDefinition::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to specify a new MapDefinition name.
// PARAMETERS:
//     Input:
//         strName - MapDefinition name
//-------------------------------------------------------------------------
void MapDefinition::SetName(const MdfString& strName)
{
    this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Coordinate System string.
// RETURNS: Returns the Coordinate System SRS string.
//-------------------------------------------------------------------------
const MdfString& MapDefinition::GetCoordinateSystem() const
{
    return this->m_strCoordSys;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Coordinate System string.
// PARAMETERS:
//     Input:
//         strCoordinateSystem - Coordinate System SRS string
//-------------------------------------------------------------------------
void MapDefinition::SetCoordinateSystem(const MdfString& strCoordinateSystem)
{
    this->m_strCoordSys = strCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Gets the extents of the MapDefinition.
// RETURNS: Box2D
//-------------------------------------------------------------------------
const Box2D& MapDefinition::GetExtents() const
{
    return this->m_boxExtents;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to set default stored Extents. This is the value
//          used if the AutoAdjustExtents is set to false.
// PARAMETERS:
//     Input:
//         extents - self describing
//-------------------------------------------------------------------------
void MapDefinition::SetExtents(const Box2D& boxExtents)
{
    this->m_boxExtents = boxExtents;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the MapDefinition's background color.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& MapDefinition::GetBackgroundColor() const
{
    return this->m_strBkGrnd;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the MapDefinition's background color.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color.
// PARAMTERS:
//      Input:
//         strColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void MapDefinition::SetBackgroundColor(const MdfString& strColor)
{
    this->m_strBkGrnd = strColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a string that represents the Metadata property.
//          The metadata consists of xml content.
// RETURNS: Reference to a string.
//-------------------------------------------------------------------------
const MdfString& MapDefinition::GetMetadata() const
{
    return this->m_strMetadata;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to a string that represents the Metadata property.
//          This method does not check the incoming string to validate its xml
//          content. It is up to the user to ensure that the correct information
//          is passed in.
// PARAMETERS:
//      Input:
//          strMetadata - reference to a string that contains formatted xml
//                        with the appropriate elements and attributes.
//-------------------------------------------------------------------------
void MapDefinition::SetMetadata(const MdfString& strMetadata)
{
    this->m_strMetadata = strMetadata;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a list of MapLayer objects that represent
//          the Layers property.
// RETURNS: A pointer to a modifiable MapLayerCollection.
//-------------------------------------------------------------------------
MapLayerCollection* MapDefinition::GetLayers()
{
    return &this->m_listLayers;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a collection of LayerGroup objects that
//          represent the LayerGroups property.
// RETURNS: A pointer to a modifiable LayerGroupCollection.
//-------------------------------------------------------------------------
MapLayerGroupCollection* MapDefinition::GetLayerGroups()
{
    return &this->m_collLayerGroups;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a list of WatermarkInstance objects that represent
//          the Watermarks property.
// RETURNS: A pointer to a modifiable WatermarkInstanceCollection.
//-------------------------------------------------------------------------
WatermarkInstanceCollection* MapDefinition::GetWatermarks()
{
    return &this->m_listWatermarks;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a collection of doubles that
//          represent the scales that the base map tiles can be displayed at.
// RETURNS: A pointer to a modifiable LayerGroupCollection.
//-------------------------------------------------------------------------
DisplayScaleCollection* MapDefinition::GetFiniteDisplayScales()
{
    return &this->m_finiteDisplayScales;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the collection of base map groups; used to
// define tiles for the HTML viewer.
// RETURNS: A pointer to a modifiable BaseMapLayerGroupCollection.
//-------------------------------------------------------------------------
BaseMapLayerGroupCollection* MapDefinition::GetBaseMapLayerGroups()
{
    return &this->m_baseMapLayerGroups;
}

#ifdef _WIN32
#ifdef _DEBUG
int MapDefinition::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
