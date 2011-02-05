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
// The MapLayerGroupCommon class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MapLayerGroupCommon.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the MapLayerGroupCommon class.
// PARAMETERS:
//     Input:
//          strName - the unique MapLayerGroupCommon name. Cannot be an empty string.
//-------------------------------------------------------------------------
MapLayerGroupCommon::MapLayerGroupCommon(const MdfString& strName)
: m_strName(strName)
{
    // default values
    this->m_bShowInLegend = true;
    this->m_bVisible = true;
    this->m_bExpandInLegend = true;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects created on the heap or
//          are adopted.
//-------------------------------------------------------------------------
MapLayerGroupCommon::~MapLayerGroupCommon()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the MapLayerGroupCommon name.
//-------------------------------------------------------------------------
const MdfString& MapLayerGroupCommon::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the MapLayerGroupCommon name.
// PARAMETERS:
//     Input:
//          strName - unique MapLayerGroupCommon name that is not an empty string.
//-------------------------------------------------------------------------
void MapLayerGroupCommon::SetName(const MdfString& strName)
{
    if (strName.length() > 0)
        this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the Visibility property of this MapLayerGroupCommon.
//          The visibility determines whether the features in the layers of
//          this MapLayerGroupCommon will be rendered when the group initially
//          comes into range.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool MapLayerGroupCommon::IsVisible() const
{
    return this->m_bVisible;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the MapLayerGroupCommon Visibility property.
//          The visibility determines whether the features in the layers of
//          this MapLayerGroupCommon will be rendered when the group initially
//          comes into range.
// PARAMETERS:
//      Input:
//          bVisibile - True to set Visibility on .
//-------------------------------------------------------------------------
void MapLayerGroupCommon::SetVisible(bool bVisible)
{
    this->m_bVisible = bVisible;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ShowInLegend property.  This property
//          determines whether this group should be shown in the legend or
//          not.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool MapLayerGroupCommon::IsShowInLegend() const
{
    return this->m_bShowInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ShowInLegend property.  This property
//          determines whether this group should be shown in the legend or
//          not.
// PARAMETERS:
//      Input:
//          bShowInLegend - if true then the group will be displayed in the legend.
//-------------------------------------------------------------------------
void MapLayerGroupCommon::SetShowInLegend(bool bShowInLegend)
{
    this->m_bShowInLegend = bShowInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Boolean flag that determines if the
//          Group should be initially expanded when shown in the viewer.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool MapLayerGroupCommon::IsExpandInLegend() const
{
    return this->m_bExpandInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Boolean flag that determines if the
//          Group should be initially expanded when shown in the viewer.
// PARAMETERS:
//      Input:
//          bExpandInLegend - if true then the Group will be initially
//                               expanded when shown in the viewer.
//
//-------------------------------------------------------------------------
void MapLayerGroupCommon::SetExpandInLegend(bool bExpandInLegend)
{
    this->m_bExpandInLegend = bExpandInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the optional LegendLabel. The legend label
//          for this group.
//-------------------------------------------------------------------------
const MdfString& MapLayerGroupCommon::GetLegendLabel() const
{
    return this->m_strLegendLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LegendLabel.
// PARAMETERS:
//      Input:
//          strLegendLabel - the new Legend Label.
//-------------------------------------------------------------------------
void MapLayerGroupCommon::SetLegendLabel(const MdfString& strLegendLabel)
{
    this->m_strLegendLabel = strLegendLabel;
}
