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

#ifndef GRIDCHANNELBAND_H_
#define GRIDCHANNELBAND_H_

#include "MdfModel.h"

// Forward declaration.
namespace MdfModel
{
    class ChannelBand;
}

///<summary>
/// Class GridChannelBand is used to wrap the MdfModel::ChannelBand.
/// It provides all the interfaces the MdfModel::ChannelBand provides.
/// And it also provides other interfaces to calculate the channel value
/// according to a double value.
///</summary>
class GridChannelBand
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridChannelBand();

    ///<summary>
    /// Destructor.
    ///</summary>
    ///<param name = "channel">
    /// Copy the argument's data.
    ///</param>
    GridChannelBand(const MdfModel::ChannelBand &channel);

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridChannelBand();

    ///<summary>
    /// Function to calcuate the channel value from a double value.
    ///</summary>
    ///<param name = "nChannel">
    /// [Out] The calculated channel value.
    ///</param>
    ///<param name = "value">
    /// The input double value.
    ///</param>
    ///<returns>
    /// Returns true if calcaute the channel value successfully.
    /// Otherwise, returns false.
    ///</returns>
    bool GetChannelValue(unsigned char& nChannel, double value) const;

    ///<summary>
    /// Function to provide assignment operator from MdfModel::ChannelBand to GridChannelBand.
    ///</summary>
    GridChannelBand& operator = (const MdfModel::ChannelBand &channel);

    ///<summary>
    /// Function to clear all the data info.
    ///</summary>
    void Clear();

    ///<summary>
    /// Function to retrieve the band name of the channel.
    ///</summary>
    const MdfModel::MdfString& GetBand() const;

    ///<summary>
    /// Set the band name of the channel.
    ///</summary>
    void SetBand(const MdfModel::MdfString& strBandName);

    ///<summary>
    /// Retrieve the low band value of the channel.
    ///</summary>
    double GetLowBand() const;

    ///<summary>
    /// Set the low band value of the channel.
    ///</summary>
    void SetLowBand(double dLowBand);

    ///<summary>
    /// Retrieve the high band value of the channel.
    ///</summary>
    double GetHighBand() const;

    ///<summary>
    /// Set the high band value of the channel.
    ///</summary>
    void SetHighBand(double dHighBand);

    ///<summary>
    /// Retrieve the low channel value of the channel.
    ///</summary>
    unsigned char GetLowChannel() const;

    ///<summary>
    /// Set the low channel value of the channel.
    ///</summary>
    void SetLowChannel(unsigned char nLowChannel);

    ///<summary>
    /// Retrieve the high channel value of the channel.
    ///</summary>
    unsigned char GetHighChannel() const;

    ///<summary>
    /// Set the high channel value of the channel.
    ///</summary>
    void SetHighChannel(unsigned char nHighChannel);

private:
    ///<summary>
    /// Function to valiate the data.
    ///</summary>
    void Validate();

private:
    // Factor to convert doubel value to channel value.
    double m_dFactor;

    // Whether the data are valid.
    bool   m_bValid;

    // Low Band
    double m_dLowBand;

    // High Band
    double m_dHighBand;

    // Low Channel
    unsigned char m_nLowChannel;

    // High Channel
    unsigned char m_nHighChannel;

    // Band Name
    MdfModel::MdfString m_strBand;
};

#endif
