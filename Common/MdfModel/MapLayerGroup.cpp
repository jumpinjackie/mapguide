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
// The MapLayerGroup class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MapLayerGroup.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the MapLayerGroup class.
// PARAMETERS:
//     Input:
//          strName - the unique MapLayerGroup name. Cannot be an empty string.
//-------------------------------------------------------------------------
MapLayerGroup::MapLayerGroup(const MdfString& strName)
: MapLayerGroupCommon(strName)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects created on the heap or
//          are adopted.
//-------------------------------------------------------------------------
MapLayerGroup::~MapLayerGroup()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the MapLayerGroup Group name. The group name is an
//          optional property of MapLayerGroups. It defines a MapLayerGroup
//          that is the parent of this MapLayerGroup.
//RETURNS:
//-------------------------------------------------------------------------
const MdfString& MapLayerGroup::GetGroup() const
{
    return this->m_strGroup;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the optional MapLayerGroup Group name. It defines
//          a MapLayerGroup that is the parent of this MapLayerGroup.
// PARAMETERS:
//      Input:
//           strGroupName - the MapLayerGroup name that is the group to which
//                          this MapLayerGroup belongs. It may be
//                          an empty string (does not belong to a group).
//-------------------------------------------------------------------------
void MapLayerGroup::SetGroup(const MdfString& strGroupName)
{
    this->m_strGroup = strGroupName;
}
