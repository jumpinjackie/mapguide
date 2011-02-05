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

#ifdef _WIN32
#define NOMINMAX
#endif

#include "stdafx.h"
#include "GridStyleColorHandler.h"
#include "GridColorStyle.h"
#include "GridData.h"
#include "Band.h"
#include "GeometryAlgorithms.h"
#include "GridStatusReporter.h"
#include "GridColorBandHandler.h"
#include "GridColorBandsHandler.h"

//
//GridStyleColorHandler
//

GridStyleColorHandler::GridStyleColorHandler()
{
    Clear();
    m_fOpacity = 1.0; // Not Transparent
    m_bDoOpacity = (m_fOpacity != 1.0);
}

GridStyleColorHandler::GridStyleColorHandler(double fOpacity)
{
    Clear();

    m_fOpacity = fOpacity;
    if (m_fOpacity < 0.0)
        m_fOpacity = 0.0;
    else if (m_fOpacity > 1.0)
        m_fOpacity = 1.0;

    m_bDoOpacity = (m_fOpacity != 1.0);
}

GridStyleColorHandler::~GridStyleColorHandler()
{
    Clear();
}

bool GridStyleColorHandler::IsHillShadeEqual(const MdfModel::HillShade &left, const MdfModel::HillShade &right) const
{
    return (left.GetAltitude() == right.GetAltitude()
            && left.GetAzimuth() == right.GetAzimuth()
            && left.GetScaleFactor() == right.GetScaleFactor());
}

bool GridStyleColorHandler::Initialize(GridData* pGrid, const MdfModel::GridColorStyle* pColorStyle)
{
    Clear();

    if (NULL == pColorStyle)
        return false;

    MdfModel::RuleCollection* pGridRules = const_cast<MdfModel::GridColorStyle*>(pColorStyle)->GetRules();

    m_spColorHandler.reset(GridColorHandler::Create(pGridRules, pGrid));
    if (NULL == m_spColorHandler.get())
    {
        Clear();
        return false;
    }

    m_pColorBand = pGrid->GetColorBand();
    if (NULL == m_pColorBand)
    {
        Clear();
        return false;
    }

    m_dBrightnessFactor = pColorStyle->GetBrightnessFactor();
    m_dContrastFactor = pColorStyle->GetContrastFactor();
    bool bDoBrightness = (CompareDoubles(m_dBrightnessFactor, -50) >= 0
                          && CompareDoubles(m_dBrightnessFactor, 50) <= 0
                          && CompareDoubles(m_dBrightnessFactor, 0) != 0);
    if (!bDoBrightness) // Reset the factor to 0 if the input value is invalid or 0.
        m_dBrightnessFactor = 0.0;
    bool bDoContrast = (CompareDoubles(m_dContrastFactor, -50) >= 0
                        && CompareDoubles(m_dContrastFactor, 50) <= 0
                        && CompareDoubles(m_dContrastFactor, 0) != 0);
    if (!bDoContrast) // Reset the factor to 0 if the inptu value is invalid or 0.
        m_dContrastFactor = 0.0;

    // Because the range of our brightness and contrast is [-50, 50],
    // And the the range in Raster Desgin is [0, 100],
    // So here we just plus the m_dBrightnessFactor and m_dContrastFactor by 50.
    if (m_bDoBrightAndContrast = bDoBrightness || bDoContrast)
        m_adjuster.setFactors(m_dBrightnessFactor + 50, m_dContrastFactor + 50);

    // Check whether to calculate the hillshade
    if (NULL != pColorStyle->GetHillShade())
    {
        Band *pHillShadeBand = pGrid->GetBand(pColorStyle->GetHillShade()->GetBand());
        if (NULL != pHillShadeBand)
        {
            const Band* pCacheHS = pGrid->GetCacheHillShadeBand(pColorStyle->GetHillShade());
            if (NULL != pCacheHS)
            {
                // Has cached
                this->m_bCalcHillShade = false;
                this->m_spCacheHillShade.reset(const_cast<Band*>(pCacheHS));
            }
            else
            {
                // Need to calculate here
                this->m_bCalcHillShade = true;
                this->m_spCacheHillShade.reset(new Band(Band::Double32, pGrid));
                this->m_spCacheHillShade->SetAllToValue(Band::Double32, (void*)&DBL_NAN);
                this->m_pCalcMdfHillShade = pColorStyle->GetHillShade();
                // Initialize the values for calculating hillshade
                GeometryAlgorithms::CalculateVector(
                    m_sun,
                    pColorStyle->GetHillShade()->GetAzimuth(),
                    pColorStyle->GetHillShade()->GetAltitude());
                m_pHillShadeBand = pHillShadeBand;
                m_dHillShadeScaleFactor = pColorStyle->GetHillShade()->GetScaleFactor();
            }
        }
    }

    // Apply hillshade when bDoHillShade is true and the color style includes a hillshade element
    m_bDoHillShade = (NULL != this->m_spCacheHillShade.get());

    if (this->m_bDoHillShade)
    {
        // Need set the nohillshade color band to gis grid
        if (NULL == pGrid->GetNoHillShadeColorBand())
        {
            // We create one for it
            pGrid->SetNoHillShadeColorBand(new Band(Band::UnsignedInt32, pGrid));
        }
        this->m_pNoHillShadeColorBand = const_cast<Band*>(pGrid->GetNoHillShadeColorBand());
        assert(NULL != this->m_pNoHillShadeColorBand);
    }
    else
    {
        // Reset the nohillshade color band of gis grid to null
        pGrid->SetNoHillShadeColorBand(NULL);
    }

    std::wstringstream ss(pColorStyle->GetTransparencyColor());
    ss >> m_transparencyColor;
    m_bDoTransparencyColor = !!ss;

    return true;
}

void GridStyleColorHandler::Clear()
{
    m_spColorHandler.reset();
    m_bDoBrightAndContrast = false;
    m_dBrightnessFactor = 0.0;
    m_dContrastFactor = 0.0;
    m_bDoHillShade = false;
    m_pColorBand = NULL;
    m_pHillShadeBand = NULL;
    m_dHillShadeScaleFactor = 1.0;
    m_bDoTransparencyColor = false;
    m_transparencyColor.SetNull();
    m_sun.x = m_sun.y = m_sun.z = 0.0;
    m_adjuster.setFactors(50.0, 50.0);
    m_bCalcHillShade = false;
    m_spCacheHillShade.reset();
    m_pCalcMdfHillShade = NULL;
    m_pNoHillShadeColorBand = NULL;

    // It's only a reference. Need not to delete it inside handler
    m_pReporter = NULL;
}

void GridStyleColorHandler::Visit(unsigned int x, unsigned int y)
{
    Color pixelcolor, noHillShadeColor;

    int index = x + y * m_pColorBand->GetXCount();

    if (!m_spColorHandler->GetColor(pixelcolor, x, y))
    {
        unsigned int argb = Color::GetNullColor().GetARGB();
        ((unsigned int*)m_pColorBand->GetRawPointer())[index] = argb;
        if (m_bDoHillShade)
            ((unsigned int*)m_pNoHillShadeColorBand->GetRawPointer())[index] = argb;

        return;
    }
    // Calculate hillshade value
    if (this->m_bCalcHillShade)
    {
        Vector3D normal;
        if (!m_pHillShadeBand->GetNormal(x, y, normal, m_dHillShadeScaleFactor))
        {
            ((int*)m_spCacheHillShade->GetRawPointer())[index] = fltNaN;
        }
        else
        {
            // Calculate hillshade value -- assume both normal and sun vector are normalized, which
            //they are
            float hillshade = (float)GeometryAlgorithms::CalculateHillShadeNormalized(normal, m_sun);
            ((float*)m_spCacheHillShade->GetRawPointer())[index] = hillshade;
        }
    }
    // Scale the R G B values by the hillshade value.
    if (m_bDoHillShade)
    {
        double hillshade = 1;

        bool ret = this->m_spCacheHillShade->GetValueAsDouble(x, y, hillshade);

        if (!ret)
        {
            unsigned int argb = Color::GetNullColor().GetARGB();
            ((unsigned int*)m_pColorBand->GetRawPointer())[index] = argb;
            if (m_bDoHillShade)
                ((unsigned int*)m_pNoHillShadeColorBand->GetRawPointer())[index] = argb;
            return;
        }

        int hillshade255 = (int)(hillshade * 256.0);

        // Save the no hillshade value
        noHillShadeColor = pixelcolor;
        // scale the pixel color
        pixelcolor.SetR((unsigned char)((pixelcolor.GetR() * hillshade255) >> 8));
        pixelcolor.SetG((unsigned char)((pixelcolor.GetG() * hillshade255) >> 8));
        pixelcolor.SetB((unsigned char)((pixelcolor.GetB() * hillshade255) >> 8));
    }
    // Adjust brightness and contrast of the pixel's color.
    if (m_bDoBrightAndContrast)
    {
        double cv = m_adjuster.adjust(pixelcolor.GetR());
        pixelcolor.SetR((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));
        cv = m_adjuster.adjust(pixelcolor.GetG());
        pixelcolor.SetG((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));
        cv = m_adjuster.adjust(pixelcolor.GetB());
        pixelcolor.SetB((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));

        // Also adjust the no hillshade value
        if (this->m_bDoHillShade)
        {
            double cv = m_adjuster.adjust(noHillShadeColor.GetR());
            noHillShadeColor.SetR((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));
            cv = m_adjuster.adjust(noHillShadeColor.GetG());
            noHillShadeColor.SetG((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));
            cv = m_adjuster.adjust(noHillShadeColor.GetB());
            noHillShadeColor.SetB((cv < 0 ? 0 : (cv > 255 ? 255 : cv)));
        }
    }

    // Compare with the transparency color. If it equals to,  then set the pixel color transparent.
    if (m_bDoTransparencyColor)
    {
        if (pixelcolor == m_transparencyColor)
        {
            pixelcolor.SetTransparent();

            unsigned int argb = pixelcolor.GetARGB();
            ((unsigned int*)m_pColorBand->GetRawPointer())[index] = argb;
            if (m_bDoHillShade)
                ((unsigned int*)m_pNoHillShadeColorBand->GetRawPointer())[index] = argb;
            return;
        }
    }

    if (m_bDoOpacity)
    {
        // Set Opacity
        pixelcolor.SetA(pixelcolor.GetA()*m_fOpacity);
    }

    // Set the pixel color into pColorBand.
    unsigned int argb = pixelcolor.GetARGB();
    ((unsigned int*)m_pColorBand->GetRawPointer())[index] = argb;

    // Set the value to no hillshade color band
    if (m_bDoHillShade)
    {
        assert(NULL != this->m_pNoHillShadeColorBand);
        argb = noHillShadeColor.GetARGB();
        ((unsigned int*)m_pNoHillShadeColorBand->GetRawPointer())[index] = argb;
    }
}

void GridStyleColorHandler::Finished(bool bSuccessful)
{
    if (!this->m_bCalcHillShade)
    { // As it has been token by the hillshade band
        this->m_spCacheHillShade.release();
    }

    if (bSuccessful)
    { // Set the cache hillshade to the Gisband
        if (this->m_bCalcHillShade
            && NULL != this->m_pHillShadeBand
            && NULL != this->m_pCalcMdfHillShade
            && NULL != this->m_spCacheHillShade.get())
        {
            if (NULL != this->m_pHillShadeBand->GetOwnerGrid()
                && this->m_pHillShadeBand->GetOwnerGrid()->SetCacheHillShadeBand(
                        this->m_pCalcMdfHillShade,
                        this->m_spCacheHillShade.get()))
            {
                this->m_spCacheHillShade.release();
            }
        }
    }
}

void GridStyleColorHandler::SetColorValue(unsigned int x, unsigned int y, const Color &pixelcolor)
{
    unsigned int argb = pixelcolor.GetARGB();
    this->m_pColorBand->SetValue(x, y, Band::UnsignedInt32, &argb);
    if (this->m_bDoHillShade)
    {
        this->m_pNoHillShadeColorBand->SetValue(x, y, Band::UnsignedInt32, &argb);
    }
}

void GridStyleColorHandler::SetStatusReporter(GridStatusReporter *pReporter)
{
    m_pReporter = pReporter;
}

//==================================================
// Multithreaded stylize
//==================================================
int GetNumProcessors()
{
#ifdef _WIN32
    DWORD_PTR proc_mask;
    DWORD_PTR sys_mask;
    GetProcessAffinityMask(GetCurrentProcess(), &proc_mask, &sys_mask);

    int num_cpus;

    //count how many bits are set in proc_mask
    for (num_cpus=0; proc_mask; ++num_cpus)
        proc_mask &= proc_mask - 1; // clear the least significant bit set
#else
    // TODO: Linux implementation here
    int num_cpus = 1;
#endif

    return num_cpus;
}

#ifdef _WIN32
struct GridStyleContext
{
    GridStyleColorHandler* handler;
    unsigned int w;
    unsigned int j;
    unsigned int count;
    DWORD thid;
};


DWORD WINAPI GridStyleWorker(LPVOID param)
{
    GridStyleContext* ctx = (GridStyleContext*)param;

    int end = ctx->j + ctx->count;

    for (int j=ctx->j; j<end; ++j)
        for (int i=0; i<(int)ctx->w; ++i)
            ctx->handler->Visit(i, j);

    return 0;
}
#else
    // Linux implementation here
#endif


//==================================================
// END Multithreaded stylize
//==================================================


bool GridStyleColorHandler::Visit()
{
    // Status Reporter should be set before Visit()
    assert(m_pReporter != NULL);
    if (m_pReporter == NULL)
        return false;

    // If there are no stylization rules for transparency, hillshade and bright/contrast, it's probably
    // a normal raster image, whereara optimization could be done for the first time when such a layer is
    // added to MAP.In this case, if the raster data is RGBA or Gray, there is actually none stylization
    // rules. All we need to do is to fill color band in GisGrid with direct memcpy.
    if (!m_bDoTransparencyColor && !m_bCalcHillShade && !m_bDoHillShade && !m_bDoBrightAndContrast)
    {
        // optimization for RGBA model
        GridColorBandHandler* pBandHandler = dynamic_cast<GridColorBandHandler*>(m_spColorHandler.get());
        if (pBandHandler)
        {
            Band* pSourceBand = NULL;
            pSourceBand = pBandHandler->GetColorBand();

            if (pSourceBand != NULL && (Band::UnsignedInt32 == pSourceBand->GetDataType() || Band::Int32 == pSourceBand->GetDataType()))
            {
                memcpy(m_pColorBand->GetRawPointer(), pSourceBand->GetRawPointer(),
                    pSourceBand->GetXCount() * pSourceBand->GetYCount() * 4);

                // If opacity has been set and is not equal to 1.0, modify aplpha value for each pixel.
                if (m_fOpacity != 1.0)
                {
                    unsigned char* pData = m_pColorBand->GetRawPointer();
                    int nPixelSize = m_pColorBand->GetXCount() * m_pColorBand->GetYCount();
                    for (int i = 0; i < nPixelSize; ++i)
                    {
                        pData[4*i+3] = (unsigned char)(pData[4*i+3]*m_fOpacity);
                    }
                }
                return true;
            }
        }

        // optimization for gray model
        GridColorBandsHandler* pBandsHandler = dynamic_cast<GridColorBandsHandler*>(m_spColorHandler.get());
        if (pBandsHandler)
        {
            Band* pRedBand = pBandsHandler->GetRedBand();
            Band* pGreenBand = pBandsHandler->GetGreenBand();
            Band* pBlueBand = pBandsHandler->GetBlueBand();

            if (pRedBand != NULL && pBandsHandler->IsGray()
                && (Band::UnsignedInt8 == pRedBand->GetDataType() || Band::Int8 == pRedBand->GetDataType())
                && pGreenBand != NULL && (Band::UnsignedInt8 == pGreenBand->GetDataType() || Band::Int8 == pGreenBand->GetDataType())
                && pBlueBand != NULL && (Band::UnsignedInt8 == pBlueBand->GetDataType() || Band::Int8 == pBlueBand->GetDataType()))
            {
                unsigned char* pDst       = m_pColorBand->GetRawPointer();
                unsigned char* pRedData   = pRedBand->GetRawPointer();
                unsigned char* pGreenData = pGreenBand->GetRawPointer();
                unsigned char* pBlueData  = pBlueBand->GetRawPointer();

                int nPixelSize = pRedBand->GetXCount() * pRedBand->GetYCount();

                // reset all to be 0xFF
                double fAlpha = (Color::kChannelMax) * m_fOpacity;
                memset(pDst, (unsigned char)fAlpha, nPixelSize * 4);

                // copy to destination in the order of 0xAARRGGBB
                for (int i = 0; i < nPixelSize; ++i)
                {
                    pDst[4*i]   = pBlueData[i];
                    pDst[4*i+1] = pGreenData[i];
                    pDst[4*i+2] = pRedData[i];
                }

                return true;
            }
        }
    }

    // oops, we can't do any optimization. We have to iterate pixel by pixel.
    unsigned int width  = m_pColorBand->GetXCount();
    unsigned int height = m_pColorBand->GetYCount();

    //see if we have more than one CPU available
    int num_cpus = GetNumProcessors();

    if (num_cpus > 1)
    {
#ifdef _WIN32
        //Yes -- use all the cpus we have.

        GridStyleContext* ctx = (GridStyleContext*)alloca(sizeof(GridStyleContext)*num_cpus);
        HANDLE* threads = (HANDLE*)alloca(sizeof(HANDLE)*num_cpus);

        const int step = 128; //each thread will process that many rows at a time

        for (int i=0; i<num_cpus; ++i)
        {
            ctx[i].count = step;
            ctx[i].handler = this;
            ctx[i].w = width;
            ctx[i].thid = 0;

            threads[i] = NULL;
        }

        for (unsigned int y = 0; y < height; )
        {
            int thcount = 0;

            //kick off N threads, and give each a strip of the grid to work on
            for (int i=0; i<num_cpus; ++i)
            {
                ctx[i].j = y;

                if (y + step <= height)
                    ctx[i].count = step;
                else
                    ctx[i].count = height - y;

                y += step;

                threads[i] = CreateThread(NULL, 0, GridStyleWorker, &ctx[i], 0, &ctx[i].thid);
                thcount++;

                if (y >= height)
                    break;
            }


            //wait for the threads to finish
            WaitForMultipleObjects(thcount, threads, TRUE, INFINITE);

            bool ret = true;

            //update status and kill the threads
            for (int i=0; i<thcount; ++i)
            {
                CloseHandle(threads[i]);
                threads[i] = NULL;
                ctx[i].thid = 0;
                ret = ret && m_pReporter->Step(ctx[i].count);
            }

            if (!ret) return false; //did user cancel?
        }
#endif
    }
    else
    {
        //case where we only have 1 CPU -- don't bother with threads

        for (unsigned int y = 0; y < height; ++y)
        {
            if (!m_pReporter->Step()) // Notify that it finished one step.
            {
                // If the Step() returns false, that means the reporter encounters some unknown
                // errors. Such as users cancels this transaction.
                // So we break out of this loop and call Rollback() to inform the reporter that
                // it can rollback the transaction now.
                return false;
            }

            for (unsigned int x = 0; x < width; ++x)
            {
                Visit(x, y);
            }
        }
    }

    return true;
}


// Implemenation of the classes for brightness and contrast adjustment.
// The following code is copied from Raster Desgin.
// URL: Raster_Desgin_Root\Main\proj\AeciIb\ImgDraw.cpp.
namespace ImageAdjust
{
    const int kMaxLimitEffect = 0;
    const int kMinLimitEffect = -1;
    const int kMaxControlValue = 100;
    const int kNeutralValue = 50;
    const double kEpsilon = 1.0e-10;
    const double CV_BIAS = 0.05;
    const double CV_RANGE = (255 + 2 * CV_BIAS);
    const double CV_HALFRANGE = (CV_RANGE / 2.0);

    BCAdjuster::BCAdjuster()
    {
        setFactors(50, 50);
    }

    BCAdjuster::~BCAdjuster()
    {
    }

    void BCAdjuster::setFactors(const double &dBrightness, const double &dContrast)
    {
        mBrightness = this->computeGammaExponent(dBrightness);
        mContrast = this->computeGammaExponent(dContrast);
    }

    double BCAdjuster::computeGammaExponent(const double& value) const
    {
        double exponent = 1.0; // Identity map.
        if (fabs(value - kNeutralValue) > kEpsilon) {
            if (value < kEpsilon) {
                // Darkness - force all to 0.
                exponent = kMinLimitEffect;
            }
            else if (value > kMaxControlValue - kEpsilon) {
                // Lightness - force all to 255.
                exponent = kMaxLimitEffect;
            }
            else {
                exponent = log(0.5) / log((kMaxControlValue - value) / kMaxControlValue);
            }
        }
        return exponent;
    }

    double BCAdjuster::adjust ( unsigned char inValue ) const
    {
        // The normalized input color value is biased away from 0 and 1 so that
        // even input values of 0 and 255 are affected by the brightness and
        // contrast functions.
        //
        double normIn = (inValue + CV_BIAS) / CV_RANGE;
        double brightOut = normIn;

        // BRIGHTNESS - This is simple "gamma correction".  output = pow(input, exponent).
        //
        if ( fabs(mBrightness - 1.0) > kEpsilon ) {
            if (mBrightness == kMaxLimitEffect) {
                // Full brightness.
                brightOut = (255 + CV_BIAS) / CV_RANGE;
            } else if (mBrightness == kMinLimitEffect) {
                // Full darkness
                brightOut = CV_BIAS / CV_RANGE;
            } else {
                brightOut = pow(normIn, mBrightness);
            }
        }

        // CONTRAST - The contrast function is two gamma curves arranged
        // symmetrically about brightOut = 0.5, producing an S-shaped curve.
        //
        if ( fabs(mContrast - 1.0) > kEpsilon ) {
            if (mContrast == kMaxLimitEffect) {
                // Maximum contrast.
                //
                return brightOut >= 0.5 ? 255.0 : 0.0;
            } else if (mContrast == kMinLimitEffect) {
                // Minimum contrast
                //
                return 127.5;
            } else {
                // The normalized output value lies between -1 and 1.
                // Since the values are slightly biased away from the two extremes,
                // even the saturated values are influenced by the function.
                //
                double contrastIn = 2.0 * fabs(brightOut - 0.5);
                double contrastOut = CV_HALFRANGE * pow(contrastIn, mContrast);
                if (brightOut < 0.5) {
                    contrastOut = std::max((127.5 - contrastOut), (kEpsilon - 0.5));
                } else {
                    contrastOut = std::min((127.5 + contrastOut), (255.5 - kEpsilon));
                }
                return contrastOut;
            }
        }
        else {
            // No contrast effect.  Return rescaled brightness effect.
            //
            return std::max(std::min(CV_RANGE * brightOut - CV_BIAS, 255.0), 0.0);
        }
    }
}
