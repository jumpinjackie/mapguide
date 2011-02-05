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

#ifndef GRIDCOLORTHEMEHANDLER_H_
#define GRIDCOLORTHEMEHANDLER_H_

#include "StylizationDefs.h"
#include "GridColorHandler.h"

// Forward declaration.
class GridTheme;
class GridThemeHashTable;

///<summary>
/// Class GridColorThemeHandler is derived from GridColorHandler.
/// It is used to handle the situtation when stylizer themes a band
/// according to the aspect, slope, or height value.
/// It calculates the geometry value and searchs the corresponding color
/// in its generated hash table.
///</summary>
class GridColorThemeHandler : public GridColorHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridColorThemeHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridColorThemeHandler();

    ///<summary>
    /// Function to initialize the GridColorThemeHandler object.
    ///</summary>
    ///<param name = "pRules">
    /// The rules includes the theme info.
    ///</param>
    ///<param name = "pGrid">
    /// The grid that includes the theme band.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    virtual bool Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid);

    ///<summary>
    /// Function to get the color on the pixel. This function calculates
    /// the geometry value first, then it searchs the corresponding color int
    /// the hash table with contains the color mappings info.
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

private:
    // Theme information extracted from the rules.
    std::auto_ptr<GridTheme>          m_spTheme;

    // Hash table generated from the theme info.
    std::auto_ptr<GridThemeHashTable> m_spHashTable;

    // Then band to be themed.
    const Band *m_pThemeBand;
};

#endif
