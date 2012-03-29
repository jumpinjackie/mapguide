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

#include "stdafx.h"
#include "GridColor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColor class
//-------------------------------------------------------------------------
GridColor::GridColor()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColor::~GridColor()
{
}

//
// GridColorExplicit
//

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColorExplicit class
//-------------------------------------------------------------------------
GridColorExplicit::GridColorExplicit()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColorExplicit::~GridColorExplicit()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ExplicitColor property.
//          The Explicit Color is a string to specify the color.
// RETURNS: The ExplicitColor
//-------------------------------------------------------------------------
const MdfString& GridColorExplicit::GetExplicitColor() const
{
    return this->m_strExplicitColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ExplicitColor property.
//          The Explicit Color is a string to specify the color.
// PARAMETERS:
//      Input:
//          strColor - The ExplicitColor.
//-------------------------------------------------------------------------
void GridColorExplicit::SetExplicitColor(const MdfString &strColor)
{
    this->m_strExplicitColor = strColor;
}

//
// GridColorBand
//

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColorBand class
//-------------------------------------------------------------------------
GridColorBand::GridColorBand()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColorBand::~GridColorBand()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// RETURNS: The Band
//-------------------------------------------------------------------------
const MdfString& GridColorBand::GetBand() const
{
    return this->m_strBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// PARAMETERS:
//      Input:
//          strBand - The band's name.
//-------------------------------------------------------------------------
void GridColorBand::SetBand(const MdfString& strBand)
{
    this->m_strBand = strBand;
}

//
// GridColorBands
//

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColorBands class
//-------------------------------------------------------------------------
GridColorBands::GridColorBands()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColorBands::~GridColorBands()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RedBand property.
//          The RedBand is a ChannelBandType to specify the red channel's name.
// RETURNS: The RedBand
//-------------------------------------------------------------------------
const ChannelBand& GridColorBands::GetRedBand() const
{
    return this->m_redBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RedBand property.
//          The RedBand is a ChannelBandType to specify the red channel's name.
// PARAMETERS:
//      Input:
//         redBand - The RedBand.
//-------------------------------------------------------------------------
void GridColorBands::SetRedBand(const ChannelBand &redBand)
{
    this->m_redBand = redBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GreenBand property.
//          The GreenBand is a ChannelBandType to specify the green channel's name.
// RETURNS: The GreenBand
//-------------------------------------------------------------------------
const ChannelBand& GridColorBands::GetGreenBand() const
{
    return this->m_greenBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GreenBand property.
//          The GreenBand is a ChannelBandType to specify the green channel's name.
// PARAMETERS:
//      Input:
//          greenBand - The GreenBand
//-------------------------------------------------------------------------
void GridColorBands::SetGreenBand(const ChannelBand &greenBand)
{
    this->m_greenBand = greenBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BlueBand property.
//          The BlueBand is a ChannelBandType to specify the blue channel's name.
// RETURNS: The BlueBand
//-------------------------------------------------------------------------
const ChannelBand& GridColorBands::GetBlueBand() const
{
    return this->m_blueBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BlueBand property.
//          The BlueBand is a ChannelBandType to specify the blue channel's name.
// PARAMETERS:
//      Input:
//          blueBand - The BlueBand
//-------------------------------------------------------------------------
void GridColorBands::SetBlueBand(const ChannelBand &blueBand)
{
    this->m_blueBand = blueBand;
}
