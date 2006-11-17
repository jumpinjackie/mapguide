//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "stdafx.h"
#include "HillShade.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the HillShade class
//-------------------------------------------------------------------------
HillShade::HillShade()
{
    m_dAzimuth = 0;
    m_dAltitude = 90;
    m_dScaleFactor = 1;
    m_strBandName = L"";
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
HillShade::~HillShade()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// RETURNS: The Band
//-------------------------------------------------------------------------
const MdfString& HillShade::GetBand() const
{
    return m_strBandName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// PARAMETERS:
//      Input:
//          strBandName - The band's name.
//-------------------------------------------------------------------------
void HillShade::SetBand(const MdfString& strBandName)
{
    this->m_strBandName = strBandName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Azimuth property.
//          The Azimuth is a double in degrees to specify the sun's azimuth.
// RETURNS: The azimuth.
//-------------------------------------------------------------------------
double HillShade::GetAzimuth() const
{
    return m_dAzimuth;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Azimuth property.
//          The Azimuth is a double in degrees to specify the sun's azimuth.
// PARAMETERS:
//      Input:
//          dAzimuth - The azimuth.
//-------------------------------------------------------------------------
void HillShade::SetAzimuth(double dAzimuth)
{
    m_dAzimuth = dAzimuth;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Altitude property.
//          The Altitude is a double in degrees to specify the sun's altitude.
// RETURNS: The altitude.
//-------------------------------------------------------------------------
double HillShade::GetAltitude() const
{
    return m_dAltitude;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Altitude property.
//          The Altitude is a double in degrees to specify the sun's altitude.
// PARAMETERS:
//      Input:
//          dAltitude - The altitude.
//-------------------------------------------------------------------------
void HillShade::SetAltitude(double dAltitude)
{
    m_dAltitude = dAltitude;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleFactor property.
//          The ScaleFactor is a double. It is used to scale the band before
//          computing hillshade.
// RETURNS: The scale factor.
//-------------------------------------------------------------------------
double HillShade::GetScaleFactor() const
{
    return m_dScaleFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleFactor property.
//          The ScaleFactor is a double. It is used to scale the band before
//          computing hillshade.
// PARAMETERS:
//      Input:
//          dScaleFactor - The scale factor.
//-------------------------------------------------------------------------
void HillShade::SetScaleFactor(double dScaleFactor)
{
    m_dScaleFactor = dScaleFactor;
}
