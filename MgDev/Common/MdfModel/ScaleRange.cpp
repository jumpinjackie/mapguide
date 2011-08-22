//
//  Copyright (C) 2011 by Autodesk, Inc.
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
// The ScaleRange class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ScaleRange.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// Contants : Represents the maximum value for the map scale.
//-------------------------------------------------------------------------
const double ScaleRange::MAX_MAP_SCALE = 1000000000000.0; // one trillion

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the VectorScaleRange class.
//-------------------------------------------------------------------------
ScaleRange::ScaleRange()
{
    // default values
    this->m_dMinScale = 0.0;
    this->m_dMaxScale = MAX_MAP_SCALE;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
ScaleRange::~ScaleRange()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value component of this ScaleRange.
// RETURNS: The minimum value.
//-------------------------------------------------------------------------
double ScaleRange::GetMinScale() const
{
    return this->m_dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the minimum value component of this ScaleRange.
// PARAMETERS:
//      Input:
//          dMinScale - the minimum value.
//-------------------------------------------------------------------------
void ScaleRange::SetMinScale(const double& dMinScale)
{
    this->m_dMinScale = dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value component of this ScaleRange.
// RETURNS: The maximum value.
//-------------------------------------------------------------------------
double ScaleRange::GetMaxScale() const
{
    return this->m_dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the maximum value component of this ScaleRange.
// PARAMETERS:
//      Input:
//          dMaxScale - The maximum value.
//-------------------------------------------------------------------------
void ScaleRange::SetMaxScale(const double& dMaxScale)
{
    this->m_dMaxScale = dMaxScale;
}
