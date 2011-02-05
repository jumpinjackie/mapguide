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
#include "ChannelBand.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ChannelBand class
//-------------------------------------------------------------------------
ChannelBand::ChannelBand()
{
    // default values
    this->m_dLowBand = 0.0;
    this->m_dHighBand = 0.0;
    this->m_nLowChannel = 0;
    this->m_nHighChannel = 255;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
ChannelBand::~ChannelBand()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// RETURNS: The Band
//-------------------------------------------------------------------------
const MdfString& ChannelBand::GetBand() const
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
void ChannelBand::SetBand(const MdfString& strBand)
{
    this->m_strBand = strBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LowBand property.
//          The LowBand is a double to specify the minimize valid band value.
// RETURNS: The LowBand
//-------------------------------------------------------------------------
double ChannelBand::GetLowBand() const
{
    return this->m_dLowBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LowBand property.
//          The LowBand is a double to specify the minimize valid band value.
// PARAMETERS:
//      Input:
//          dLowBand - The value of LowBand
//-------------------------------------------------------------------------
void ChannelBand::SetLowBand(double dLowBand)
{
    this->m_dLowBand = dLowBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HighBand property.
//          The HighBand is a double to specify the maximize valid band value.
// RETURNS: The HighBand
//-------------------------------------------------------------------------
double ChannelBand::GetHighBand() const
{
    return this->m_dHighBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HighBand property.
//          The HighBand is a double to specify the maximize valid band value.
// PARAMETERS:
//      Input:
//          dHighBand - The value of HighBand
//-------------------------------------------------------------------------
void ChannelBand::SetHighBand(double dHighBand)
{
    this->m_dHighBand = dHighBand;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LowChannel property.
//          The LowChannel is a byte to specify the minimize valid channel value.
// RETURNS: The LowChannel
//-------------------------------------------------------------------------
unsigned char ChannelBand::GetLowChannel() const
{
    return this->m_nLowChannel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LowChannel property.
//          The LowChannel is a byte to specify the minimize valid channel value.
// PARAMETERS:
//      Input:
//          nLowChannel - The value of Low Channel
//-------------------------------------------------------------------------
void ChannelBand::SetLowChannel(unsigned char nLowChannel)
{
    this->m_nLowChannel = nLowChannel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HighChannel property.
//          The HighChannel is a byte to specify the maximize valid channel value.
// RETURNS: The HighChannel
//-------------------------------------------------------------------------
unsigned char ChannelBand::GetHighChannel() const
{
    return this->m_nHighChannel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HighChannel property.
//          The HighChannel is a byte to specify the maximize valid channel value.
// PARAMETERS:
//      Input:
//          nHighChannel - The value of High Channel.
//-------------------------------------------------------------------------
void ChannelBand::SetHighChannel(unsigned char nHighChannel)
{
    this->m_nHighChannel = nHighChannel;
}
