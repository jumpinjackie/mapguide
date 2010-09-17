//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
// The WatermarkAppearance class implementation.
// WatermarkAppearance class is to define the appearance of watermark.
//-------------------------------------------------------------------------

#include <cmath>
#include "stdafx.h"
#include "WatermarkAppearance.h"

using namespace MDFMODEL_NAMESPACE;

const double WatermarkAppearance::doubleTolerance = 0.01;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the WatermarkAppearance class.
//-------------------------------------------------------------------------
WatermarkAppearance::WatermarkAppearance()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
WatermarkAppearance::~WatermarkAppearance()
{
}

//-------------------------------------------------------------------------
// Returns the transparency (0 - 100). O means totally opaque. 100 means totally
// transparent. 
//-------------------------------------------------------------------------
double WatermarkAppearance::GetTransparency() const
{
    return this->m_transparency;
}

//-------------------------------------------------------------------------
// Set the transparency (0 - 100). O means totally opaque. 100 means totally
// transparent. If the parameter is more than 100, treat it as 100. If the
// parameter is less than 0, treat it as 0.
//-------------------------------------------------------------------------
void WatermarkAppearance::SetTransparency(const double& dTransparency)
{
    this->m_transparency = (dTransparency > 100) ? 100 : (dTransparency < 0 ? 0 : dTransparency);
}

//-------------------------------------------------------------------------
// Returns the rotation (0 - 360). O means no rotation. 90 means rotating 90 degree
// anticlockwise. 360 should be the same as 0 as it turns a whole circle.
//-------------------------------------------------------------------------
double WatermarkAppearance::GetRotation() const
{
    return this->m_rotation;
}

//-------------------------------------------------------------------------
// Set the rotation (0 - 360). O means no rotation. 90 means rotating 90 degree
// anticlockwise. 360 should be the same as 0 as it turns a whole circle. 
// If the parameter is more than 360, treat it as 360. If the
// parameter is less than 0, treat it as 0.
//-------------------------------------------------------------------------
void WatermarkAppearance::SetRotation(const double& dRotation)
{
    this->m_rotation = (dRotation > 360) ? 360 : (dRotation < 0 ? 0 : dRotation);
}

bool WatermarkAppearance::Equals(WatermarkAppearance* another)
{
    return another
            && fabs(this->m_transparency - another->m_transparency) < doubleTolerance
            && fabs(this->m_rotation - another->m_rotation) < doubleTolerance;
}

#ifdef _WIN32
#ifdef _DEBUG
int WatermarkAppearance::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
