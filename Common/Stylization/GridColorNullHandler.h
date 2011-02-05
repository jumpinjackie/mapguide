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

#ifndef GRIDCOLORNULLHANDLER_H_
#define GRIDCOLORNULLHANDLER_H_

#include "GridColorHandler.h"

///<summary>
/// Class GridColorNullHandler is derived from GridColorHandler.
/// It is used to handle the situtation when there is on rule.
///</summary>
class GridColorNullHandler : public GridColorHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridColorNullHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridColorNullHandler();

    ///<summary>
    /// Function to initialize the GridColorNullHandler object.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the detail info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to get the color on the pixel.
    /// This function alwasy return a white color.
    ///</summary>
    ///<param name = "color">
    /// [Out] Stores the result color, it is always white.
    ///</param>
    ///<param name = "x">
    /// The X axis position of the pixel
    ///</param>
    ///<param name = "y">
    /// The Y axis position of the pixel
    ///</param>
    ///<returns>
    /// Returns true if it gets the color.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool GetColor(Color &color, unsigned int x, unsigned int y);

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear();
};

#endif
