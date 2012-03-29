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
// BaseMapLayer Class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "BaseMapLayer.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the BaseMapLayer class.
// PARAMETERS:
//     Input:
//          strName - the unique BaseMapLayer name. Cannot be an empty string.
//-------------------------------------------------------------------------
BaseMapLayer::BaseMapLayer(const MdfString& strName, const MdfString& strLayerResourceID)
: m_strName(strName)
, m_strLayerResourceID(strLayerResourceID)
{
    // default values
    this->m_bVisible = true;
    this->m_bExpandInLegend = true;
    this->m_plyrLayer = NULL;
    this->m_bSelectable = true;
    this->m_bShowInLegend = true;
}


//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects created on the heap
//          or are adopted.
//-------------------------------------------------------------------------
BaseMapLayer::~BaseMapLayer()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the unique BaseMapLayer name.
//-------------------------------------------------------------------------
const MdfString& BaseMapLayer::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the BaseMapLayer name.
// PARAMETERS:
//     Input:
//          strName - unique BaseMapLayer name that is not a blank string.
//-------------------------------------------------------------------------
void BaseMapLayer::SetName(const MdfString& strName)
{
    if (strName.length() > 0)
        this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the LayerDefinition resourceID that this BaseMapLayer references.
// RETURNS:
//-------------------------------------------------------------------------
const MdfString& BaseMapLayer::GetLayerResourceID() const
{
    return this->m_strLayerResourceID;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the LayerDefinition Uri that this BaseMapLayer references.
// PARAMETERS:
//      Input:
//          strLayerResourceID - the Layer resourceID for this BaseMapLayer.Cannot be an empty
//                        string
//-------------------------------------------------------------------------
void BaseMapLayer::SetLayerResourceID(const MdfString& strLayerResourceID)
{
    if (strLayerResourceID.length() > 0)
        this->m_strLayerResourceID = strLayerResourceID;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the LayerDefinition object that this BaseMapLayer references.
// RETURNS: The LayerDefinition pointer.
//-------------------------------------------------------------------------
LayerDefinition* BaseMapLayer::GetLayerDefinition()
{
    return this->m_plyrLayer;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the LayerDefinition object that this BaseMapLayer references
// PARAMETERS:
//      Input:
//         plyrLayer - LayerDefinition pointer to a representation of the resource
//                     repository reference that is part of the BaseMapLayer
//                     element schema.
//-------------------------------------------------------------------------
void BaseMapLayer::SetLayerDefinition(LayerDefinition* plyrLayer)
{
    this->m_plyrLayer = plyrLayer;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the Visibility property of this Layer.
//          The visibility determines whether the features in the layers of
//          this Layer will be rendered when the group initially
//          comes into range.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool BaseMapLayer::IsVisible() const
{
    return this->m_bVisible;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the Layer Visibility property.
//          The visibility determines whether the features in the layers of
//          this LayerGroup will be rendered when the group initially
//          comes into range.
// PARAMETERS:
//      Input:
//          bVisibile - True to set Visibility on .
//-------------------------------------------------------------------------
void BaseMapLayer::SetVisible(bool bVisibile)
{
    this->m_bVisible = bVisibile;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BaseMapLayers selectable flag. This property
//          determines if a layers features may be selected.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool BaseMapLayer::IsSelectable() const
{
    return this->m_bSelectable;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the BaseMapLayers selectable flag. This property
//          determines if a layers features may be selected.
// PARAMETERS:
//      Input:
//          bSelectable - this value must be false for Terrain and Raster
//                        Layers.
//-------------------------------------------------------------------------
void BaseMapLayer::SetSelectable(bool bSelectable)
{
    this->m_bSelectable = bSelectable;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ShowInLegend property.  This property
//          determines whether this layer should be shown in the legend or
//          not.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool BaseMapLayer::IsShowInLegend() const
{
    return this->m_bShowInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ShowInLegend property.  This property
//          determines whether this layer should be shown in the legend or
//          not.
// PARAMETERS:
//      Input:
//          bShowInLegend - if true then the group will be displayed in the legend.
//-------------------------------------------------------------------------
void BaseMapLayer::SetShowInLegend(bool bShowInLegend)
{
    this->m_bShowInLegend = bShowInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LegendLabel.
//-------------------------------------------------------------------------
const MdfString& BaseMapLayer::GetLegendLabel() const
{
    return this->m_strLegendLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LegendLabel.
// PARAMETERS:
//      Input:
//          strLegendLabel - the new Legend Label.
//-------------------------------------------------------------------------
void BaseMapLayer::SetLegendLabel(const MdfString& strLegendLabel)
{
    this->m_strLegendLabel = strLegendLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Boolean flag that determines
//          whether the layer's styles are shown in the legend.
// RETURNS: Default is true.
//-------------------------------------------------------------------------
bool BaseMapLayer::IsExpandInLegend() const
{
    return this->m_bExpandInLegend;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Boolean flag that determines if theming
//          will be displayed in the legend.
// PARAMETERS:
//      Input:
//          bExpandInLegend - if true then theming will be displayed in the
//                            legend.
//-------------------------------------------------------------------------
void BaseMapLayer::SetExpandInLegend(bool bExpandInLegend)
{
    this->m_bExpandInLegend = bExpandInLegend;
}
