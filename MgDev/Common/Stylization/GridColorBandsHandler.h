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

#ifndef GRIDCOLORBANDSHANDLER_H_
#define GRIDCOLORBANDSHANDLER_H_

#include "GridColorHandler.h"
#include "GridChannelBand.h"

///<summary>
/// Class GridColorBandsHandler is used to handle the situation when
/// the GridColor's type is Bands. It gets the channel data from the
/// three ChannelBands and merge them to a color value. That color value
/// is returned in the GetColor() function.
///</summary>
class GridColorBandsHandler : public GridColorHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridColorBandsHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridColorBandsHandler();

    ///<summary>
    /// Function to initialize the GridColorBandsHandler object.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the GridColorBands info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid includes those bands that are treated as the channels.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to get the color on the pixel.
    /// This funciton gets the RGB channel values first.
    /// Then it merges them to a color value. That color
    /// value is the result color.
    ///</summary>
    ///<param name = "color">
    /// [Out] Stores the result color.
    ///</param>
    ///<param name = "x">
    /// The X axis position of the pixel
    ///</param>
    ///<param name = "y">
    /// The Y axis position of the pixel
    ///</param>
    ///<returns>
    /// Returns true if it gets the corresponding color.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool GetColor(Color &color, unsigned int x, unsigned int y);

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear();

    ///<summary>
    /// Function to get red band
    ///</summary>
    virtual Band* GetRedBand();

    ///<summary>
    /// Function to get gree band
    ///</summary>
    virtual Band* GetGreenBand();

    ///<summary>
    /// Function to get blue band
    ///</summary>
    virtual Band* GetBlueBand();

    ///<summary>
    /// Function to tell whether it's all from the same gray image
    ///</summary>
    virtual bool IsGray();

private:
    // Red Channel
    GridChannelBand m_redChannel;

    // Green Channel
    GridChannelBand m_greenChannel;

    // Blue Channel
    GridChannelBand m_blueChannel;

    // Red Band
    Band  *m_pRedBand;

    // Green Band
    Band  *m_pGreenBand;

    // Blue Band
    Band  *m_pBlueBand;

    // Is Gray Model
    bool m_bGray;
};

#endif
