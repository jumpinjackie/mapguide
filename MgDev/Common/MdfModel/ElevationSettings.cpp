//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
// The ElevationSettings class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ElevationSettings.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ElevationSettings class.
//-------------------------------------------------------------------------
ElevationSettings::ElevationSettings()
{
    // default values
    this->m_eUnit = MdfModel::Centimeters;
    this->m_elevationType = RelativeToGround;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
ElevationSettings::~ElevationSettings()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ZOffsetExpression property.
// RETURNS: String representation of the ZOffsetExpression property.
//-------------------------------------------------------------------------
const MdfString& ElevationSettings::GetZOffsetExpression() const
{
    return this->m_strZOffsetExpression;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets the ZOffsetExpression property.
// PARAMETERS:
//      Input:
//          strElevationProperty - The ZOffsetExpression property.
//-------------------------------------------------------------------------
void ElevationSettings::SetZOffsetExpression(const MdfString& strZOffsetExpression)
{
    this->m_strZOffsetExpression = strZOffsetExpression;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ZExtrusionExpression property.
// RETURNS: String representation of the ZExtrusionExpression property.
//-------------------------------------------------------------------------
const MdfString& ElevationSettings::GetZExtrusionExpression() const
{
    return this->m_strZExtrusionExpression;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets the ZExtrusionExpression property.
// PARAMETERS:
//      Input:
//          strElevationProperty - The ZExtrusionExpression property.
//-------------------------------------------------------------------------
void ElevationSettings::SetZExtrusionExpression(const MdfString& strZExtrusionExpression)
{
    this->m_strZExtrusionExpression = strZExtrusionExpression;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Unit property
//          The Unit enum defines the measurement unit for the offset and extrusion properties.
// RETURNS: The Unit enum value.
//-------------------------------------------------------------------------
LengthUnit ElevationSettings::GetUnit() const
{
    return this->m_eUnit;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Unit property
//          The Unit enum defines the measurement unit for the offset and extrusion properties.
// PARAMETERS:
//      Input:
//          eUnit - the measurement unit for the offset and extrusion properties.
//-------------------------------------------------------------------------
void ElevationSettings::SetUnit(LengthUnit eUnit)
{
    this->m_eUnit = eUnit;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ElevationType value of this ElevationSettings.
// RETURNS: The elevation type
//-------------------------------------------------------------------------
MdfModel::ElevationSettings::ElevationType ElevationSettings::GetElevationType() const
{
    return this->m_elevationType;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets the ElevationType value of this ElevationSettings.
// PARAMETERS:
//      Input:
//          elevationType - The value for the ElevationType
//-------------------------------------------------------------------------
void ElevationSettings::SetElevationType(ElevationType elevationType)
{
    this->m_elevationType = elevationType;
}
