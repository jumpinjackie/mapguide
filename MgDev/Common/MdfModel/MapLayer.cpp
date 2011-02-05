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
// MapLayer Class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MapLayer.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the MapLayer class.
// PARAMETERS:
//     Input:
//          strName - the unique MapLayer name. Cannot be an empty string.
//-------------------------------------------------------------------------
MapLayer::MapLayer(const MdfString& strName, const MdfString& strLayerResourceID)
: BaseMapLayer(strName, strLayerResourceID)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects created on the heap
//          or are adopted.
//-------------------------------------------------------------------------
MapLayer::~MapLayer()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the MapLayer Group name. The group name is an optional
//          property of MapLayers. If group is blank, the layer should be shown
//          at the root level of the tree.
//RETURNS:
//-------------------------------------------------------------------------
const MdfString& MapLayer::GetGroup() const
{
    return this->m_strGroup;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the optional MapLayer Group name.
//          If group is blank, the layer should be shown
//          at the root level of the tree.
// PARAMETERS:
//      Input:
//           pstrGroupName - the MapLayer Group name that is the group to which
//                          this MapLayer belongs. It may be a pointer to an
//                          empty string.
//-------------------------------------------------------------------------
void MapLayer::SetGroup(const MdfString& strGroup)
{
    this->m_strGroup = strGroup;
}
