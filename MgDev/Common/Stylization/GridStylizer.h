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

#ifndef GRIDSTYLIZER_H_
#define GRIDSTYLIZER_H_

#include "Stylization.h"
#include "GridStyleHandler.h"
#include "MdfModel.h"
#include <list>

// Forward declaration.
namespace MdfModel
{
    class GridLayerDefinition;
    class GridStyle;
    class HillShade;
}

class GridData;
class GridStylizerReactor;
class GridStyleVisitor;
class GridApplyStatusReporter;

///<summary>
/// Class only for stylization with grid data and grid styles.
/// GridStylizer takes a grid, a grid color style, and a grid surface style.
/// Then it applies the color style on the grid and writes the result into
/// the color band of the grid. Moreover, it also applies the surface style
/// on the grid and writes the result into the elevation band of the grid.
///</summary>
class GridStylizer
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    STYLIZATION_API      GridStylizer();

    ///<summary>
    /// Destructor.
    ///</summary>
    STYLIZATION_API      ~GridStylizer();

    ///<summary>
    /// Stylizes a grid base on the given surface style and color style.
    /// It applies the surface style to generate a new elevation band to show
    /// 3d surface. And it also applies the color style to generate a new
    /// color band to show the texture of the 3d surface.
    ///</summary>
    ///<param name="pGrid">
    /// A grid is a set of bands. The stylizer read band data from the grid
    /// and write the result into the color band and elevation band.
    ///</param>
    ///<param name="pSurfaceStyle">
    /// The pointer of GridSurfaceStyle, which specifies how to calculate the elevation.
    ///</param>
    ///<param name="pColorStyle">
    /// The pointer of GridColorStyle, which specifies how to stylize each pixel.
    ///</param>
    ///</param>
    ///<param name="fOpacity">
    /// Opacity of this Grid Layer
    ///</param>
    ///<returns>
    /// Returns true to indicate the stylization transaction completes successfully.
    /// Returns false to indicate the stylization transaction encouters some errors and
    /// has been interruptted.
    ///</returns>
    STYLIZATION_API bool ApplyStyles(
                                     GridData *pGrid,
                                     const MdfModel::GridSurfaceStyle *pSurfaceStyle,
                                     const MdfModel::GridColorStyle *pColorStyle,
                                     double fOpacity = 1.0);

    ///<summary>
    /// Stylizes a grid base on the given color style and default color.
    /// It applies the color style to generate a new color band to show
    /// the texture of the 3d surface. And also it will set the transparent
    /// pixel to the default color.
    ///</summary>
    ///<param name="pGrid">
    /// A grid is a set of bands. The stylizer read band data from the grid
    /// and write the result into the color band.
    ///</param>
    ///<param name="pColorStyle">
    /// The pointer of GridColorStyle, which specifies how to stylize each pixel.
    ///</param>
    ///<param name="sDefaultColor">
    /// The default color for the tansparent pixel.
    ///</param>
    ///<returns>
    /// Returns true to indicate the stylization transaction completes successfully.
    /// Returns false to indicate the stylization transaction encouters some errors and
    /// has been interruptted.
    ///</returns>
    STYLIZATION_API bool ApplyColorStyle(
                                         GridData *pGrid,
                                         const MdfModel::GridColorStyle *pStyle,
                                         const MdfModel::MdfString &sDefaultColor);

    ///<summary>
    /// Stylizes a grid base on the given surface style.
    /// It applies the surface style to generate a new elevation band to show
    /// 3d surface.
    ///</summary>
    ///<param name="pGrid">
    /// A grid is a set of bands. The stylizer read band data from the grid
    /// and write the result into the color band and elevation band.
    ///</param>
    ///<param name="pSurfaceStyle">
    /// The pointer of GridSurfaceStyle, which specifies how to calculate the elevation.
    ///</param>
    ///<returns>
    /// Returns true to indicate the stylization transaction completes successfully.
    /// Returns false to indicate the stylization transaction encouters some errors and
    /// has been interruptted.
    ///</returns>
    STYLIZATION_API bool ApplySurfaceStyle(
                                           GridData *pGrid,
                                           const MdfModel::GridSurfaceStyle *pStyle);

    ///<summary>
    /// Applied the hillshade effect to a gis band
    ///</summary>
    ///<param name="pColorBand">
    /// It is the input and ouput, we read color from it, and apply hillshade, then write
    /// back to it.
    ///</param>
    ///<param name="pHS">
    /// It describes how to apply hillshade to the gis band.
    ///</param>
    ///<returns>
    /// Returns true to indicate the stylization transaction completes successfully.
    /// Returns false to indicate the stylization transaction encouters some errors and
    /// has been interruptted.
    ///</returns>
    STYLIZATION_API static bool ApplyHillShade(
                                               Band* pColorBand,
                                               const MdfModel::HillShade *pHS);

    ///<summary>
    /// Call this function to cancel the apply styles transaction.
    /// If stylizer is not applying styles, then it calls the
    /// reactors' OnCancelApply() function and returns.
    ///</summary>
    STYLIZATION_API void CancelApply();

    ///<summary>
    /// Add a reactor into the reactor list.
    ///</summary>
    ///<param name="reactor">
    /// The reactor pointer that the caller wants to add.
    ///</param>
    ///<returns>
    /// Returns true indicate add the reactor successfully.
    /// Returns false indicate the reactor has been in the reactor list.
    ///</returns>
    STYLIZATION_API bool AddReactor(GridStylizerReactor *reactor);

    ///<summary>
    /// Remove a reactor from the reactor list.
    ///</summary>
    ///<param name="reactor">
    /// The reactor pointer that the caller wants to remove.
    ///</param>
    ///<returns>
    /// Returns true indicate remove the reactor successfully.
    /// Returns false indicate the reactor is not in the reactor list.
    ///</returns>
    STYLIZATION_API bool RemoveReactor(GridStylizerReactor *reactor);

private:
    ///<summary>
    /// Function to visit the GridStyleHandlers in a loop.
    /// Ther stylizer delegates the stylization works to multi style handlers.
    /// Those style handlers apply the styles on each pixel.
    ///</summary>
    ///<param name = "styleHandlers">
    /// Stores the style handlers.
    ///</param>
    ///<param name = "width">
    /// The X resolution of the grid.
    ///</param>
    ///<param name = "height">
    /// The Y resolution of the grid.
    ///</param>
    ///<returns>
    /// Returns true to indicate the visit completes successfully.
    /// Returns false to indicate the visit encouters some errors and
    /// has been interruptted.
    ///</returns>
    bool                 VisitStyleHandlers(
                                            const MdfModel::MdfOwnerCollection<GridStyleHandler> &styleHandlers,
                                            unsigned int width,
                                            unsigned int height);

    ///<summary>
    /// Helper function that calls all the reactor's OnBeginApply() function.
    ///</summary>
    void                 Fire_OnBeginApply(int begin, int end, int step);

    ///<summary>
    /// Helper function that calls all the reactor's OnStepApply() function.
    /// If one reactor returns false, then this function interrupts iterating
    /// and returns false immediately.
    ///</summary>
    bool                 Fire_OnStepApply(int position);

    ///<summary>
    /// Helper function that calls all the reactor's OnEndApply() function.
    ///</summary>
    void                 Fire_OnEndApply();

    ///<summary>
    /// Helper function that calls all the reactor's OnCancelApply() function.
    ///</summary>
    void                 Fire_OnCancelApply();

private:
    /// Reactor Collection Type
    typedef std::list<GridStylizerReactor*> ReactorCollection;

    // Reactor list.
    ReactorCollection                       m_reactors;

    // Status reporter
    std::auto_ptr<GridApplyStatusReporter>  m_spReporter;

    // Prevent coping and assigning.
    GridStylizer(const GridStylizer &);
    GridStylizer& operator = (const GridStylizer &);

    // Friend class
    friend class GridApplyStatusReporter;
};

#endif
