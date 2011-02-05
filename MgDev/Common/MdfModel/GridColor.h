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

#ifndef GRIDCOLOR_H_
#define GRIDCOLOR_H_

#include <vector>
#include "ChannelBand.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The base class for all the grid colors.
    // The grid color is used to stylize the pixel using the given or calculated
    // color.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridColor : public MdfRootObject
    {
    public:
        virtual ~GridColor();

    protected:
        GridColor();
    };

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Class GridColorExplicit stores an explicit color, which is set by users.
    // It uses this predefined color to stylize the pixel always.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridColorExplicit : public GridColor
    {
    public:
        GridColorExplicit();
        virtual ~GridColorExplicit();

        const MdfString& GetExplicitColor() const;
        void SetExplicitColor(const MdfString &strColor);

    private:
        // Hidden copy constructor and assignment operator.
        GridColorExplicit(const GridColorExplicit&);
        GridColorExplicit& operator = (const GridColorExplicit&);

        // Explicit Color
        MdfString m_strExplicitColor;
    };

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Class GridColorBand stores an band name, which is set by users.
    // It uses this band's value as the color to stylize the pixel.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridColorBand : public GridColor
    {
    public:
        GridColorBand();
        virtual ~GridColorBand();

        // Property : Band
        const MdfString& GetBand() const;
        void SetBand(const MdfString& strBand);

    private:
        // Hidden copy constructor and assignment operator.
        GridColorBand(const GridColorBand&);
        GridColorBand& operator = (const GridColorBand&);

        // Band
        MdfString m_strBand;
    };

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Class GridColorBands stores three channels.
    // It gets the adjusted values from those three channels, and merges them
    // to be a color value and stylize the pixel with that color.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridColorBands : public GridColor
    {
    public:
        GridColorBands();
        virtual ~GridColorBands();

        // Property : RedBand
        const ChannelBand& GetRedBand() const;
        void SetRedBand(const ChannelBand &redBand);

        // Property : GreenBand
        const ChannelBand& GetGreenBand() const;
        void SetGreenBand(const ChannelBand &greenBand);

        // Property : BlueBand
        const ChannelBand& GetBlueBand() const;
        void SetBlueBand(const ChannelBand &blueBand);

    private:
        // Hidden copy constructor and assignment operator.
        GridColorBands(const GridColorBands&);
        GridColorBands& operator = (const GridColorBands&);

        // Red Band
        ChannelBand m_redBand;

        // Green Band
        ChannelBand m_greenBand;

        // Blue Band
        ChannelBand m_blueBand;
    };

END_NAMESPACE_MDFMODEL
#endif // GRIDCOLOR_H_
