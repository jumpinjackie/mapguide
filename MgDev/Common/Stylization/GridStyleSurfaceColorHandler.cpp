//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridStyleSurfaceColorHandler.cpp#1 $
//
//  Copyright (C) 2005 Autodesk, Inc. All Rights Reserved.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
//  CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
//  IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
//  DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
//  DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject
//  to restrictions set forth in FAR 52.227-19 (Commercial Computer
//  Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
//  (Rights in Technical Data and Computer Software), as applicable.
//

#include "stdafx.h"
#include "Band.h"
#include "GridData.h"
#include "GridStyleSurfaceColorHandler.h"
#include "Color.h"
#include <sstream>
#include <cassert>
#include "GridStatusReporter.h"

GridStyleSurfaceColorHandler::GridStyleSurfaceColorHandler()
{
    Clear();
}

GridStyleSurfaceColorHandler::~GridStyleSurfaceColorHandler()
{
    Clear();
}

bool GridStyleSurfaceColorHandler::Initialize(GridData *pGrid, const MdfModel::MdfString &sDefaultColor)
{
    Clear();
    bool bInit = false;
    Color defaultColor;
    std::wstringstream ss(sDefaultColor);
    ss >> defaultColor;
    // Set the default color to transparent for distinguish
    // that color from those normal colors.
    // At next time, we still know which pixels are transparent.
    defaultColor.SetTransparent();
    m_nDefaultColor = defaultColor.GetARGB();
    m_pColorBand = pGrid->GetColorBand();
    m_pElevationBand = pGrid->GetElevationBand();
    this->m_pNoHillShadeColorBand = pGrid->GetNoHillShadeColorBand();
    bInit = (!!ss) && (NULL != m_pColorBand); // In some cases, the elevation band may be NULL.
    if (!bInit)
        Clear();
    return bInit;
}

void GridStyleSurfaceColorHandler::Visit(unsigned int x, unsigned int y)
{
    if (m_pElevationBand && !m_pElevationBand->IsValid(x, y))
    {
        SetColorValue(x, y, m_nDefaultColor);
    }
    else
    {
        unsigned int colorvalue = m_pColorBand->GetColorValue(x, y);
        if (colorvalue)
        {
            Color color(colorvalue);
            if (color.IsTransparent())
                SetColorValue(x, y, m_nDefaultColor);
        }
    }
}

void GridStyleSurfaceColorHandler::Clear()
{
    m_pColorBand = NULL;
    m_pElevationBand = NULL;
    m_nDefaultColor = 0;
    m_pNoHillShadeColorBand = NULL;
}

void GridStyleSurfaceColorHandler::SetColorValue(unsigned int x, unsigned int y, unsigned int pixelcolor)
{
    this->m_pColorBand->SetValue(x, y, Band::UnsignedInt32, &pixelcolor);
    if (NULL != this->m_pNoHillShadeColorBand)
    {
        this->m_pNoHillShadeColorBand->SetValue(x, y, Band::UnsignedInt32, &pixelcolor);
    }
}

void GridStyleSurfaceColorHandler::SetStatusReporter(GridStatusReporter *pReporter)
{
    m_pReporter = pReporter;
}

bool GridStyleSurfaceColorHandler::Visit()
{
    // Status Reporter should be set before Visit()
    assert(m_pReporter != NULL);
    if (m_pReporter == NULL)
        return false;

    // Iterate through every pixel
    for (unsigned int y = 0; y < m_pColorBand->GetYCount(); ++y)
    {
        if (!(y%16) && !m_pReporter->Step(16)) // Notify that it finished one step.
        {
            // If the Step() returns false, that means the reporter encounters some unknown
            // errors. Such as users cancels this transaction.
            // So we break out of this loop and call Rollback() to inform the reporter that
            // it can rollback the transaction now.
            return false;
        }

        for (unsigned int x = 0; x < m_pColorBand->GetXCount(); ++x)
        {
            Visit(x, y);
        }
    }

    return true;
}
