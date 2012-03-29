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

#ifndef GRIDSTYLECOLORHANDLER_H_
#define GRIDSTYLECOLORHANDLER_H_

#include "GridStyleHandler.h"
#include "GridColorHandler.h"
#include "Vector3D.h"
#include "HillShade.h"
#include "Color.h"


// Forward declaration
namespace MdfModel
{
    class GridColorStyle;
    class HillShade;
}

class GridStatusReporter;

// Forward declaration of the classes for brightness and contrast adjustment.
// The following code is copied from Raster Desgin.
// URL: Raster_Desgin_Root\Main\proj\AeciIb\ImgDraw.h.
namespace ImageAdjust
{
    class BCAdjuster
    {
    public:
        BCAdjuster();
        ~BCAdjuster();
        void setFactors(const double &dBrightness, const double &dContrast);
        double computeGammaExponent(const double& value) const;
        double adjust ( unsigned char inValue ) const;

    private:
        double mBrightness;
        double mContrast;
    };
}

///<summary>
/// Class GridStyleColorHandler is derived from GridStyleHandler.
/// It is used to handle the GridColorStyle.
///</summary>
class GridStyleColorHandler : public GridStyleHandler
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridStyleColorHandler();

    ///<summary>
    /// Constructor.
    ///</summary>
    GridStyleColorHandler(double fOpacity);

    ///<summary>
    /// Destructor.
    ///</summary>
    virtual ~GridStyleColorHandler();

    ///<summary>
    /// Function to initialize the GridStyleColorHandler object.
    ///</summary>
    ///<param name = "pGrid">
    /// The grid that this handlers applies on.
    ///</param>
    ///<param name = "pColorStyle">
    /// The grid color style that will be applied.
    ///</param>
    ///<returns>
    /// Returns true if it is intialized successfully.
    /// Othrewise, returns false.
    ///</returns>
    bool Initialize(GridData *pGrid, const MdfModel::GridColorStyle *pColorStyle);

    ///<summary>
    /// Function to clear the data info of the handler.
    ///</summary>
    virtual void Clear();

    ///<summary>
    /// Function to visit each pixel. The GridStyleSurfaceColorHandler
    /// set the transparent color and the invalid pixel to the default color.
    ///</summary>
    virtual void Visit(unsigned int x, unsigned int y);

    ///<summary>
    /// Finished visiting all the pixels
    ///</summary>
    virtual void Finished(bool bSuccessful);

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
    /// Check whetehr those two HillShade value is the same or not.
    ///</summary>
    ///<retruns>
    /// Returns true if left is equal to right.
    /// Otherwise, returns false.
    ///</returns>
    bool IsHillShadeEqual(const MdfModel::HillShade &left, const MdfModel::HillShade &right) const;

    ///<summary>
    /// Now we have to set the color value to two color bands:
    /// one is the color band with the hillshade effect.
    /// the other is the color band without the hillshade effect.
    /// Because such set statements will appear multiple times, define a helper function
    /// to simple the logic.
    ///</summary>
    void SetColorValue(unsigned int x, unsigned int y, const Color &pixelcolor);

private:
    // Handler for the GridColor
    std::auto_ptr<GridColorHandler> m_spColorHandler;

    // Sun vector
    Vector3D m_sun;

    // Color Band
    Band *m_pColorBand;

    // Whether it need to adjust brightness and contrast.
    bool m_bDoBrightAndContrast;

    // Brightness factor
    double m_dBrightnessFactor;

    // Contrast factor
    double m_dContrastFactor;

    // Whether to apply hillshade.
    bool m_bDoHillShade;
    bool m_bCalcHillShade; // Calculate hillshade for later usage.
    std::auto_ptr<Band>     m_spCacheHillShade;
    const MdfModel::HillShade *m_pCalcMdfHillShade;

    // NoHillShade color band, means that color band
    // contains the color value without the hillshade effect.
    Band                   *m_pNoHillShadeColorBand;

    // The Band to compute hillshade.
    Band *m_pHillShadeBand;

    // The scale factor for computing hillshade.
    double   m_dHillShadeScaleFactor;

    // Whether it need to handle the transparency color.
    bool     m_bDoTransparencyColor;

    // Transparency color
    Color    m_transparencyColor;

    // Adjuster to adjust brightness and contrast of an image.
    ImageAdjust::BCAdjuster m_adjuster;

    // A referrence to Grid Apply Status Reporter. Need not to delete it inside this handler
    GridStatusReporter *m_pReporter;

    // Record the Opacity value
    double m_fOpacity;
    bool m_bDoOpacity;
};

#endif
