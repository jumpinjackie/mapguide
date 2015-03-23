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

#ifndef GRIDCOLORHANDLER_H_
#define GRIDCOLORHANDLER_H_

#include "MdfModel.h"
#include "GridColorRule.h"

// Forward declaration.
class Band;
class GridData;
class Color;
class GridColorThemeHandler;
class GridColorBandHandler;
class GridColorBandsHandler;
class GridColorNullHandler;

///<summary>
/// Base class to provide the uniform interface for getting the color
/// on each pixel when stylizing the band.
///</summary>
class GridColorHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridColorHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridColorHandler();

    ///<summary>
    /// Function to initialize the GridColorHandler object.
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
    virtual bool Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid) = 0;

    ///<summary>
    /// Function to get the color on the pixel.
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
    virtual bool GetColor(Color &color, unsigned int x, unsigned int y) = 0;

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear() = 0;

    ///<summary>
    /// Function to create the corresponding color handler according to the different pRules.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the detail info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns the pointer to the new created GridColorHandler.
    /// Caller must be responsible to release it.
    /// Returns NULL if the handlers fail to initialize.
    ///</returns>
    static GridColorHandler* Create(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to create the GridColorBandHandler.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the detail info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns the pointer to the new created GridColorBandHandler.
    /// Caller must be responsible to release it.
    /// Returns NULL if the handlers fail to initialize.
    ///</returns>
    static GridColorBandHandler* CreateBandHandler(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to create the GridColorBandsHandler.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the detail info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns the pointer to the new created GridColorBandsHandler.
    /// Caller must be responsible to release it.
    /// Returns NULL if the handlers fail to initialize.
    ///</returns>
    static GridColorBandsHandler* CreateBandsHandler(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to create the GridColorThemeHandler.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the theme info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns the pointer to the new created GridColorThemeHandler.
    /// Caller must be responsible to release it.
    /// Returns NULL if the handlers fail to initialize.
    ///</returns>
    static GridColorThemeHandler* CreateThemeHandler(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to create the GridColorNullHandler.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the detail info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that the styles are applied on.
    ///</param>
    ///<returns>
    /// Returns the pointer to the new created GridColorNullHandler.
    /// Caller must be responsible to release it.
    /// Returns NULL if the handlers fail to initialize.
    ///</returns>
    static GridColorNullHandler* CreateNullHandler(const MdfModel::RuleCollection *pRules, const GridData *pGrid);
};

#endif
