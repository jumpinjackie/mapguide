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
// The VectorScaleRange class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "VectorScaleRange.h"

using namespace MDFMODEL_NAMESPACE;

// we must undefine the max macro so the limits max function compiles correctly
#include <limits>
using namespace std;
#undef max

//-------------------------------------------------------------------------
// Contants : Represents the maximum value for the map scale.
//-------------------------------------------------------------------------
const double VectorScaleRange::MAX_MAP_SCALE = 1000000000000.0; // one trillion

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the VectorScaleRange class.
//-------------------------------------------------------------------------
VectorScaleRange::VectorScaleRange()
{
    // default values
    this->m_dMinScale = 0.0;
    this->m_dMaxScale = MAX_MAP_SCALE;
    this->m_elevationSettings = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
VectorScaleRange::~VectorScaleRange()
{
    delete this->m_elevationSettings;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value component of this VectorScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The minimum value.
//-------------------------------------------------------------------------
double VectorScaleRange::GetMinScale() const
{
    return this->m_dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the minimum value component of this VectorScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMinScale - the minimum value.
//-------------------------------------------------------------------------
void VectorScaleRange::SetMinScale(const double& dMinScale)
{
    this->m_dMinScale = dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value component of this VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The maximum value.
//-------------------------------------------------------------------------
double VectorScaleRange::GetMaxScale() const
{
    return this->m_dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the maximum value component of this VectorScaleRange.
//          VectorScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMaxScale - The maximum value.
//-------------------------------------------------------------------------
void VectorScaleRange::SetMaxScale(const double& dMaxScale)
{
    this->m_dMaxScale = dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the collection of FeatureTypeStyles
//          defined in this VectorScaleRange.
// RETURNS: A pointer to a modifiable FeatureTypeStyleCollection.
//-------------------------------------------------------------------------
FeatureTypeStyleCollection* VectorScaleRange::GetFeatureTypeStyles()
{
    return &this->m_collFeatureTypeStyles;
}

//-------------------------------------------------------------------------
// PURPOSE: Retrieves the elevation settings for this scale range
// RETURNS: A pointer to the ElevationSettings object
//-------------------------------------------------------------------------
ElevationSettings* VectorScaleRange::GetElevationSettings()
{
    return this->m_elevationSettings;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt an ElevationSettings object
// PARAMETERS:
//      Input:
//         elevationSettings - A pointer to the ElevationSettings object
//-------------------------------------------------------------------------
void VectorScaleRange::AdoptElevationSettings(ElevationSettings* elevationSettings)
{
    if (this->m_elevationSettings != elevationSettings)
    {
        delete this->m_elevationSettings;
        this->m_elevationSettings = elevationSettings;
    }
}
