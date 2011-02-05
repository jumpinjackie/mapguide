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

#ifndef GRIDSTYLESURFACEHANDLER_H_
#define GRIDSTYLESURFACEHANDLER_H_

#include "GridStyleHandler.h"
#include "Color.h"

// Forward declaraion
namespace MdfModel
{
    class GridSurfaceStyle;
}
class GridStatusReporter;

///<summary>
/// Class GridStyleSurfaceHandler is derived from GridStyleHandler.
/// It is used to handle the GridSurfaceStyle.
///</summary>
class GridStyleSurfaceHandler : public GridStyleHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridStyleSurfaceHandler();

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridStyleSurfaceHandler();

    ///<summary>
    /// Function to initialize the GridStyleSurfaceHandler object.
    ///</summary>
    ///<param name = "pGrid">
    /// The grid that this handlers applies on.
    ///</param>
    ///<param name = "pSurfaceStyle">
    /// The grid surface style that will be applied.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    bool Initialize(GridData *pGrid, const MdfModel::GridSurfaceStyle *pSurfaceStyle);

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear();

    ///<summary>
    /// Function to visit each pixel. The GridStyleSurfaceHandler
    /// calculates the new elevation data and writes it
    /// into the elevation band.
    ///</summary>
    virtual void Visit(unsigned int x, unsigned int y);

    ///<summary>
    /// Set GridStatusReporter
    ///</summary>
    virtual void SetStatusReporter(GridStatusReporter *pReporter);

    ///<summary>
    /// Function to visit all pixels.
    ///</summary>
    virtual bool Visit();

private:
    // Zero value
    double m_dZeroValue;

    // Scale factor
    double m_dScaleFactor;

    // Whether need adjust the elevation data.
    bool   m_bDoAdjust;

    // Source band
    Band *m_pSourceBand;

    // Dest elevation band
    Band *m_pElevationBand;

    // A referrence to Grid Apply Status Reporter. Need not to release it inside this handler
    GridStatusReporter *m_pReporter;
};

#endif
