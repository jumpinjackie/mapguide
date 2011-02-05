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
// LayerDefinition Class implementation. LayerDefinition is an abstract class.
// Since LayerDefinition has only two concrete subclasses, instead of using the
// Visitor pattern to determine their concrete types a different approach is
// used. The method IsFeatureLayer() is defined. As its name suggest, each
// concrete class simply returns either true or false.
// Note: In the future, if further types of Layers are defined then the above
// approach will be abandoned for the Visitor pattern.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "LayerDefinition.h"
#include "Base64.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize the LayerDefinition class.
// PARAMETERS:
//     Input:
//          strResourceID - the Resource name. Cannot be an empty string.
//-------------------------------------------------------------------------
LayerDefinition::LayerDefinition(const MdfString& strResourceID) :
    m_strResourceID(strResourceID)
{
    // default values
    this->m_opacity = 1.0;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Clean up all the objects that are created on the heap
//          or are adopted.
//-------------------------------------------------------------------------
LayerDefinition::~LayerDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the read only ResourceID property. The
//          ResourceID represents the name of the file that is the source
//          of all the data.
// RETURNS: Name of the file.
//-------------------------------------------------------------------------
const MdfString& LayerDefinition::GetResourceID() const
{
    return this->m_strResourceID;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ResourceID property. The
//          ResourceID property is name of the file that is the source of
//          all the data.
// PARAMETERS:
//      Input:
//          strSourceID - resource file name.
//-------------------------------------------------------------------------
void LayerDefinition::SetResourceID(const MdfString& strResourceID)
{
    this->m_strResourceID = strResourceID;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the double Opacity of the layer.
// RETURNS: The layer's opacity as a double.
//-------------------------------------------------------------------------
const double LayerDefinition::GetOpacity() const
{
    return this->m_opacity;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to a double Opacity property.
//          This method does not check the incoming value to validate that it falls
//          within the range 0 to 1. It is up to the user to ensure that the correct
//          information is passed in.
// PARAMETERS:
//      Input:
//          opacity - a double representing the opacity from 0 (transparent) to 1 (opaque).
//-------------------------------------------------------------------------
void LayerDefinition::SetOpacity(const double opacity)
{
    this->m_opacity = opacity;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for a list of WatermarkInstance objects that represent
//          the Watermarks property.
// RETURNS: A pointer to a modifiable WatermarkInstanceCollection.
//-------------------------------------------------------------------------
WatermarkInstanceCollection* LayerDefinition::GetWatermarks()
{
    return &this->m_listWatermarks;
}
