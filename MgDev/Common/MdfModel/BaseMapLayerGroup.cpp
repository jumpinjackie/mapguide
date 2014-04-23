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
// The BaseMapLayerGroup class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "BaseMapLayerGroup.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the BaseMapLayerGroup class.
// PARAMETERS:
//     Input:
//          strName - the unique BaseMapLayerGroup name. Cannot be an empty string.
//-------------------------------------------------------------------------
BaseMapLayerGroup::BaseMapLayerGroup(const MdfString& strName)
: MapLayerGroupCommon(strName)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects created on the heap or
//          are adopted.
//-------------------------------------------------------------------------
BaseMapLayerGroup::~BaseMapLayerGroup()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Get the list of BaseMapLayers that are in this group.
// RETURNS: A pointer to a modifiable BaseMapLayerCollection.
//-------------------------------------------------------------------------
BaseMapLayerCollection* BaseMapLayerGroup::GetLayers()
{
    return &this->m_layers;
}
