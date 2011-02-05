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
// DrawingLayerDefinition Class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "DrawingLayerDefinition.h"
#include "VectorLayerDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the DrawingLayerDefinition class.
// PARAMETERS:
//     Input:
//          strName - the DrawingLayerDefinition name. Cannot be an empty string.
// Construction, destruction, initialization
//-------------------------------------------------------------------------
DrawingLayerDefinition::DrawingLayerDefinition(const MdfString& strDataResesourceID, const MdfString &strSheet)
: LayerDefinition(strDataResesourceID)
, m_strSheet(strSheet)
{
    // default values
    this->m_dMinScale = 0.0;
    this->m_dMaxScale = VectorScaleRange::MAX_MAP_SCALE;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
DrawingLayerDefinition::~DrawingLayerDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Sheet property. The Sheet property is
//          the sheet name to be used from the DWF (Drawing Web Format) file
//          that is the source of all the feature data.
// RETURNS: sheet name from the DWF file.
//-------------------------------------------------------------------------
const MdfString& DrawingLayerDefinition::GetSheet() const
{
    return this->m_strSheet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Sheet property. The Sheet property is
//          the sheet name to be used from the DWF (Drawing Web Format) file
//          that is the source of all the feature data.
// PARAMETERS:
//      Input:
//          strLayerFilter - sheet name from the DWF file.
//-------------------------------------------------------------------------
void DrawingLayerDefinition::SetSheet(const MdfString& strSheet)
{
    this->m_strSheet = strSheet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerFilter property. The LayerFilter
//          represents the layers to be extracted from the sheet.
// RETURNS: Names of layers to be extracted from the sheet
//-------------------------------------------------------------------------
const MdfString& DrawingLayerDefinition::GetLayerFilter() const
{
    return this->m_strLayerFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LayerFilter property. The LayerFilter
//          represents the layers to be extracted from the sheet.
// PARAMETERS:
//      Input:
//          strLayerFilter - Names of layers to be extracted from the sheet
//-------------------------------------------------------------------------
void DrawingLayerDefinition::SetLayerFilter(const MdfString& strLayerFilter)
{
    this->m_strLayerFilter = strLayerFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value component of this DrawingLayerDefinition.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the ScaleRange.
//          The min and max scale values are used to define a range of values
//          within which the DrawingLayerDefinition is drawn.
// RETURNS: The minimum value.
//-------------------------------------------------------------------------
double DrawingLayerDefinition::GetMinScale() const
{
    return this->m_dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the minimum value component of this DrawingLayerDefinition.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the ScaleRange.
//          The min and max scale values are used to define a range of values
//          within which the DrawingLayerDefinition is drawn.
// PARAMETERS:
//      Input:
//          dMinScale - the minimum value.
//-------------------------------------------------------------------------
void DrawingLayerDefinition::SetMinScale(const double& dMinScale)
{
    this->m_dMinScale = dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value component of this DrawingLayerDefinition.
//          The min and max scale values are used to define a range of values
//          within which the DrawingLayerDefinition is drawn.
// RETURNS: The maximum value.
//-------------------------------------------------------------------------
double DrawingLayerDefinition::GetMaxScale() const
{
    return this->m_dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the maximum value component of this DrawingLayerDefinition.
//          The min and max scale values are used to define a range of values
//          within which the DrawingLayerDefinition is drawn.
// PARAMETERS:
//      Input:
//          dMaxScale - The maximum value.
//-------------------------------------------------------------------------
void DrawingLayerDefinition::SetMaxScale(const double& dMaxScale)
{
    this->m_dMaxScale = dMaxScale;
}
