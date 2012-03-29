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
#include "GridChannelBand.h"
#include "MathHelper.h"
#include "ChannelBand.h"

//
// GridChannelBand
//

GridChannelBand::GridChannelBand()
{
    Clear();
}

GridChannelBand::GridChannelBand(const MdfModel::ChannelBand &channel)
{
    *this = channel;
}

GridChannelBand::~GridChannelBand()
{
}

bool GridChannelBand::GetChannelValue(unsigned char& nChannel, double value) const
{
    if (m_bValid)
    {
        if (CompareDoubles(value, m_dLowBand) <= 0)
            nChannel = m_nLowChannel;
        else if (CompareDoubles(value, m_dHighBand) >= 0)
            nChannel = m_nHighChannel;
        else
            nChannel = (unsigned char)Double2Int((value - m_dLowBand) * m_dFactor + m_nLowChannel);
        return true;
    }
    return false;
}

GridChannelBand& GridChannelBand::operator = (const MdfModel::ChannelBand &channel)
{
    m_strBand = channel.GetBand();
    m_dLowBand = channel.GetLowBand();
    m_dHighBand = channel.GetHighBand();
    m_nLowChannel = channel.GetLowChannel();
    m_nHighChannel = channel.GetHighChannel();
    Validate();
    return *this;
}

void GridChannelBand::Clear()
{
    MdfModel::ChannelBand channelBand;
    *this = channelBand;
}

void GridChannelBand::Validate()
{
    int nCompResult = CompareDoubles(m_dLowBand, m_dHighBand);
    if (nCompResult == 0)
    {
        m_bValid = true;
        m_dFactor = 0;
    }
    else if (nCompResult > 0)
    {
        m_bValid = false;
        m_dFactor = 0;
    }
    else
    {
        m_bValid = true;
        m_dFactor = (m_nHighChannel - m_nLowChannel) / (m_dHighBand - m_dLowBand);
    }
}

const MdfModel::MdfString& GridChannelBand::GetBand() const
{
    return m_strBand;
}

void GridChannelBand::SetBand(const MdfModel::MdfString& strBandName)
{
    m_strBand = strBandName;
}

double GridChannelBand::GetLowBand() const
{
    return m_dLowBand;
}

void GridChannelBand::SetLowBand(double dLowBand)
{
    m_dLowBand = dLowBand;
    Validate();
}

double GridChannelBand::GetHighBand() const
{
    return m_dHighBand;
}

void GridChannelBand::SetHighBand(double dHighBand)
{
    m_dHighBand = dHighBand;
    Validate();
}

unsigned char GridChannelBand::GetLowChannel() const
{
    return m_nLowChannel;
}

void GridChannelBand::SetLowChannel(unsigned char nLowChannel)
{
    m_nLowChannel = nLowChannel;
    Validate();
}

unsigned char GridChannelBand::GetHighChannel() const
{
    return m_nHighChannel;
}

void GridChannelBand::SetHighChannel(unsigned char nHighChannel)
{
    m_nHighChannel = nHighChannel;
    Validate();
}
