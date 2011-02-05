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

#ifndef GRIDCOLORBANDHANDLER_H_
#define GRIDCOLORBANDHANDLER_H_

#include "GridColorHandler.h"

///<summary>
/// Class GridColorBandHandler is used to handle the situation when
/// the GridColor's type is Band. It gets the band's value and return
/// it as the color value.
///</summary>
class GridColorBandHandler : public GridColorHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridColorBandHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridColorBandHandler();

    ///<summary>
    /// Function to initialize the GridColorBandHandler object.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the band info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that includes the band.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to get the color on the pixel.
    /// It gets the band value on the pixel and set it
    /// to output color.
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

    Band* GetColorBand()
    {
        return m_pColorBand;
    }

private:
    // Band name
    MdfModel::MdfString   m_sBandName;

    // Color band
    Band              *m_pColorBand;
};

#endif
