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

#include "stdafx.h"
#include "GridStylizer.h"
#include "Band.h"
#include "Color.h"
#include "GridStylizerReactor.h"
#include "GridData.h"
#include "GridApplyStatusReporter.h"
#include "GridStyleColorHandler.h"
#include "GridStyleSurfaceHandler.h"
#include "GridStyleSurfaceColorHandler.h"
#include "GeometryAlgorithms.h"
#include "GridLayerDefinition.h"
#include "GridColorStyle.h"
#include "GridSurfaceStyle.h"
#include <algorithm>

//
// GridStylizer
//

GridStylizer::GridStylizer()
{
    m_spReporter.reset();
}

GridStylizer::~GridStylizer()
{
    m_spReporter.reset();
}

bool GridStylizer::ApplyStyles(
                              GridData *pGrid,
                              const MdfModel::GridSurfaceStyle *pSurfaceStyle,
                              const MdfModel::GridColorStyle   *pColorStyle,
                              double fOpacity)
{
    //clock_t t0 = clock();

    // Initialize: Set the elevation band to NULL.
    // Reset the elevation band and return a generated elevation band
    // which can be got by GetElevationBand().
    pGrid->SetElevationBand(static_cast<Band*>(NULL));
    // Initialize: Set the values of color band to NULL.
    // Reset the color band's values, if there is on color style, the result
    // color is the surface style's default color.
    unsigned int c = 0;
    pGrid->GetColorBand()->SetAllToValue(Band::UnsignedInt32, &c);
    MdfModel::MdfOwnerCollection<GridStyleHandler> styleHandlers;
    if (NULL != pColorStyle)
    {
        // Add handler to deal with color style.
        std::auto_ptr<GridStyleColorHandler> spHandler(new GridStyleColorHandler(fOpacity));
        if (spHandler->Initialize(pGrid, pColorStyle))
            styleHandlers.Adopt(spHandler.release());
    }
    if (NULL != pSurfaceStyle)
    {
        // Add handler to deal with surface style
        std::auto_ptr<GridStyleSurfaceHandler> spHandler(new GridStyleSurfaceHandler());
        if (spHandler->Initialize(pGrid, pSurfaceStyle))
            styleHandlers.Adopt(spHandler.release());
        // Add handler to deal with the color on null pixels.
        std::auto_ptr<GridStyleSurfaceColorHandler> spSurfaceColorHandler(new GridStyleSurfaceColorHandler());
        if (spSurfaceColorHandler->Initialize(pGrid, pSurfaceStyle->GetDefaultColor()))
            styleHandlers.Adopt(spSurfaceColorHandler.release());
    }
    bool ret = VisitStyleHandlers(styleHandlers, pGrid->GetXCount(), pGrid->GetYCount());

    //clock_t t1 = clock();
    //FILE* fp = fopen("c:\\gstime.txt", "a");
    //fprintf(fp, "%d\n", t1 - t0);
    //fclose(fp);

    return ret;
}

bool GridStylizer::ApplyColorStyle(
                                   GridData *pGrid,
                                   const MdfModel::GridColorStyle *pStyle,
                                   const MdfModel::MdfString &sDefaultColor)
{
    // Suppose the users didn't modify the surface style.
    // We will modify the color band and set the new transparency pixel to the previous
    // default color.
    MdfModel::MdfOwnerCollection<GridStyleHandler> styleHandlers;
    // Add handler to deal with the color style.
    std::auto_ptr<GridStyleColorHandler> spHandler(new GridStyleColorHandler());
    if (spHandler->Initialize(pGrid, pStyle))
        styleHandlers.Adopt(spHandler.release());
    // Add handler to deal with the color on null pixels.
    std::auto_ptr<GridStyleSurfaceColorHandler> spSurfaceColorHandler(new GridStyleSurfaceColorHandler());
    if (spSurfaceColorHandler->Initialize(pGrid, sDefaultColor))
        styleHandlers.Adopt(spSurfaceColorHandler.release());
    return VisitStyleHandlers(styleHandlers, pGrid->GetXCount(), pGrid->GetYCount());
}

bool GridStylizer::ApplySurfaceStyle(
                                     GridData *pGrid,
                                     const MdfModel::GridSurfaceStyle *pStyle)
{
    // Suppose the users didn't modify the color style.
    // We will modify the elevation band and set the previous transparency pixel to the new
    // default color.
    MdfModel::MdfOwnerCollection<GridStyleHandler> styleHandlers;
    // Add handler to deal with surface style
    std::auto_ptr<GridStyleSurfaceHandler> spHandler(new GridStyleSurfaceHandler());
    if (spHandler->Initialize(pGrid, pStyle))
        styleHandlers.Adopt(spHandler.release());
    // Add handler to deal with the color on null pixels.
    std::auto_ptr<GridStyleSurfaceColorHandler> spSurfaceColorHandler(new GridStyleSurfaceColorHandler());
    if (spSurfaceColorHandler->Initialize(pGrid, pStyle->GetDefaultColor()))
        styleHandlers.Adopt(spSurfaceColorHandler.release());
    return VisitStyleHandlers(styleHandlers, pGrid->GetXCount(), pGrid->GetYCount());
}


bool GridStylizer::ApplyHillShade(Band* pColorBand, const MdfModel::HillShade* pHS)
{
    if (NULL == pColorBand || NULL == pHS || NULL == pColorBand->GetOwnerGrid())
        return false;

    GridData* pGrid = pColorBand->GetOwnerGrid();
    unsigned int height = pColorBand->GetYCount();
    unsigned int width = pColorBand->GetXCount();
    unsigned int x, y;

    // Tried to get the cache hillshade values
    const Band* pHSBand = pGrid->GetCacheHillShadeBand(pHS);
    if (NULL == pHSBand)
    {
        // Not cached, then we generate one
        assert(false);
        std::auto_ptr<Band> spNewHSBand(new Band(Band::Double32, pGrid));
        Band* pHillShadeBand = pGrid->GetBand(pHS->GetBand());
        if (NULL == pHillShadeBand)
            return false;

        Vector3D vecNormal;
        Vector3D vecSun;
        GeometryAlgorithms::CalculateVector(vecSun, pHS->GetAzimuth(), pHS->GetAltitude());
        double dScale = pHS->GetScaleFactor();

        for (y = 0; y < height; ++y)
        {
            for (x = 0; x < width; ++x)
            {
                if (pHillShadeBand->GetNormal(x, y, vecNormal, dScale))
                {
                    float fHillShade = (float)GeometryAlgorithms::CalculateHillShadeNormalized(vecNormal, vecSun);
                    spNewHSBand->SetValue(x, y, Band::Double32, &fHillShade);
                }
                else
                {
                    // That pixel is NULL, do nothing.
                    spNewHSBand->SetValue(x, y, Band::Double32, (void*)&FLT_NAN);
                }
            }
        }

        if (pGrid->SetCacheHillShadeBand(pHS, spNewHSBand.get()))
            spNewHSBand.release();
    }

    // Get again
    pHSBand = pGrid->GetCacheHillShadeBand(pHS);
    assert(NULL != pHSBand);
    double hillshade = 1.0;
    Color pixelcolor;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            bool ret = pHSBand->GetValueAsDouble(x, y, hillshade);
            if (ret)
            {
                pixelcolor.SetARGB(pColorBand->GetColorValue(x, y));
                pixelcolor.SetR(pixelcolor.GetR() * hillshade);
                pixelcolor.SetG(pixelcolor.GetG() * hillshade);
                pixelcolor.SetB(pixelcolor.GetB() * hillshade);
                unsigned int c = pixelcolor.GetARGB();
                pColorBand->SetValue(x, y, Band::UnsignedInt32, &c);
            }
        }
    }

    return true;
}

bool GridStylizer::VisitStyleHandlers(const MdfModel::MdfOwnerCollection<GridStyleHandler> &styleHandlers,
                                      unsigned int /*width*/,
                                      unsigned int height)
{
    int nHandlerCount = styleHandlers.GetCount();

    if (nHandlerCount == 0)
        return false;

    m_spReporter.reset(new GridApplyStatusReporter);
    m_spReporter->Init(this, height*nHandlerCount);
    m_spReporter->Begin();

    bool result = true;
    for (int k = 0; k < nHandlerCount; ++k)
    {
        GridStyleHandler *pHandler = styleHandlers.GetAt(k);

        // GridStatusReporter must be set before Visit()
        pHandler->SetStatusReporter(m_spReporter.get());
        result = pHandler->Visit();
    }

    if (result)
        m_spReporter->Commit();
    else
        m_spReporter->Rollback();

    for (int k = 0; k < nHandlerCount; ++k)
        styleHandlers.GetAt(k)->Finished(result);

    m_spReporter.reset(); // Clear the reporter.
    return result;
}

void GridStylizer::CancelApply()
{
    // It the current visitor includes a reporter, then call SetTerminate()
    // to let the stylizer stop applying.
    if (NULL != m_spReporter.get())
    {
        m_spReporter->SetTerminate(true);
    }
    // Otherwise, the applying transaction has been finished, or not started.
    // Thus, call Fire_OnCancelApply() to let the outer users detetmine what to do next.
    else
    {
        Fire_OnCancelApply();
    }
}

bool GridStylizer::AddReactor(GridStylizerReactor *reactor)
{
    ReactorCollection::iterator it = std::find(m_reactors.begin(), m_reactors.end(), reactor);
    if (it == m_reactors.end())
    {
        m_reactors.push_back(reactor);
        return true;
    }
    return false;
}

bool GridStylizer::RemoveReactor(GridStylizerReactor *reactor)
{
    ReactorCollection::iterator it = std::find(m_reactors.begin(), m_reactors.end(), reactor);
    if (it != m_reactors.end())
    {
        m_reactors.remove(reactor);
        return true;
    }
    return false;
}

void GridStylizer::Fire_OnBeginApply(int begin, int end, int step)
{
    ReactorCollection::iterator it = m_reactors.begin();
    ReactorCollection::iterator endIter = m_reactors.end();
    for (; it != endIter; ++it)
    {
        (*it)->OnBeginApply(begin, end, step);
    }
}

bool GridStylizer::Fire_OnStepApply(int position)
{
    ReactorCollection::iterator it = m_reactors.begin();
    ReactorCollection::iterator endIter = m_reactors.end();
    for (; it != endIter; ++it)
    {
        if (!(*it)->OnStepApply(position))
        {
            // If there is one reactor returns false,
            // Stylizer thinks it should cancel this stylization.
            return false;
        }
    }
    return true;
}

void GridStylizer::Fire_OnEndApply()
{
    ReactorCollection::iterator it = m_reactors.begin();
    ReactorCollection::iterator endIter = m_reactors.end();
    for (; it != endIter; ++it)
    {
        (*it)->OnEndApply();
    }
}

void GridStylizer::Fire_OnCancelApply()
{
    ReactorCollection::iterator it = m_reactors.begin();
    ReactorCollection::iterator endIter = m_reactors.end();
    for (; it != endIter; ++it)
    {
        (*it)->OnCancelApply();
    }
}
