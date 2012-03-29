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

#ifndef GRIDSTYLESURFACECOLORHANDLER_H_
#define GRIDSTYLESURFACECOLORHANDLER_H_

#include "GridStyleHandler.h"

// Forward declaraion
class GridStatusReporter;

///<summary>
/// Class GridStyleSurfaceColorHandler is derived from GridStyleHandler.
/// It is used to set the transparent color to the default color.
/// And it also set the invalid pixel to the default color.
///</summary>
class GridStyleSurfaceColorHandler : public GridStyleHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridStyleSurfaceColorHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridStyleSurfaceColorHandler();

    ///<summary>
    /// Function to initialize the GridStyleSurfaceColorHandler object.
    ///</summary>
    ///<param name = "pGrid">
    /// The grid that this handlers applies on.
    ///</param>
    ///<param name = "sDefaultColor">
    /// The default color that is set to transparent color and invalid pixel.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    bool Initialize(GridData *pGrid, const MdfModel::MdfString &sDefaultColor);

    ///<summary>
    /// Function to visit each pixel. The GridStyleSurfaceColorHandler
    /// set the transparent color and the invalid pixel to the default color.
    ///</summary>
    virtual void Visit(unsigned int x, unsigned int y);

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear();

    ///<summary>
    /// Set GridStatusReporter
    ///</summary>
    virtual void SetStatusReporter(GridStatusReporter *pReporter);

    ///<summary>
    /// Function to visit all pixels.
    ///</summary>
    virtual bool Visit();

private:
    ///<summary>
    /// Now we have to set the color value to two color bands:
    /// one is the color band with the hillshade effect.
    /// the other is the color band without the hillshade effect.
    /// Because such set statements will appear multiple times, define a helper function
    /// to simple the logic.
    ///</summary>
    void SetColorValue(unsigned int x, unsigned int y, unsigned int pixelcolor);

private:
    // Default color
    unsigned int m_nDefaultColor;

    // Color band
    Band *m_pColorBand;

    // Elevation band
    Band *m_pElevationBand;

    // NoHillShade color band, means that color band
    // contains the color value without the hillshade effect.
    Band *m_pNoHillShadeColorBand;

    // A referrence to Grid Apply Status Reporter. Need not to delete it inside this handler
    GridStatusReporter *m_pReporter;
};

#endif
