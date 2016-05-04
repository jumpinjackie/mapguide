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

#ifndef CHANNELBAND_H_
#define CHANNELBAND_H_

#include <vector>
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-----------------------------------------------------------------------------
    // DESCRIPTION:
    // Class ChannelBand is used to map the band's value to a valid color channel value.
    // It defines how to scale numbers into a color channel.
    //-----------------------------------------------------------------------------
    class MDFMODEL_API ChannelBand : public MdfRootObject
    {
    public:
        ChannelBand();
        virtual ~ChannelBand();

        // Property : Band
        const MdfString& GetBand() const;
        void SetBand(const MdfString& strBandName);

        // Property : LowBand
        // Optional, the default value is the min value found in the band.
        double GetLowBand() const;
        void SetLowBand(double dLowBand);

        // Property : HighBand
        // Optional, the default value is the max value found in the band.
        double GetHighBand() const;
        void SetHighBand(double dHighBand);

        // Property : LowChannel
        // Optional, the default value is 0.
        unsigned char GetLowChannel() const;
        void SetLowChannel(unsigned char nLowChannel);

        // Property : HighChannel
        // Optional, the default value is 255.
        unsigned char GetHighChannel() const;
        void SetHighChannel(unsigned char nHighChannel);

    private:
        // Band
        MdfString     m_strBand;

        // Low Band
        double        m_dLowBand;

        // High Band
        double        m_dHighBand;

        // Low Channel
        unsigned char m_nLowChannel;

        // High Channel
        unsigned char m_nHighChannel;
    };

END_NAMESPACE_MDFMODEL
#endif // CHANNELBAND_H_
