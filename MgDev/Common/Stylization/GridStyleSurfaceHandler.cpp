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
#include "GridStyleSurfaceHandler.h"
#include "Band.h"
#include "GridData.h"
#include "GridSurfaceStyle.h"
#include "GridStatusReporter.h"

//
//GridStyleSurfaceHandler
//

GridStyleSurfaceHandler::GridStyleSurfaceHandler()
{
    Clear();
}

GridStyleSurfaceHandler::~GridStyleSurfaceHandler()
{
    Clear();
}

bool GridStyleSurfaceHandler::Initialize(GridData* pGrid, const MdfModel::GridSurfaceStyle* pSurfaceStyle)
{
    Clear();

    m_dZeroValue = pSurfaceStyle->GetZeroValue();
    m_dScaleFactor = pSurfaceStyle->GetScaleFactor();
    m_bDoAdjust = !(m_dZeroValue == 0 && m_dScaleFactor == 1);
    if (!m_bDoAdjust)
    {
        pGrid->SetElevationBand(pSurfaceStyle->GetBand());

        // We don't need to visit each pixel, so just set reset.
        Clear();
        return false;
    }

    m_pSourceBand = pGrid->GetBand(pSurfaceStyle->GetBand());
    if (NULL == m_pSourceBand)
    {
        Clear();
        return false;
    }

    m_pElevationBand = pGrid->GetElevationBandDataForStylization();
    if (NULL == m_pElevationBand)
    {
        Clear();
        return false;
    }

    double dnull = 0.0;
    Band::BandDataType type = m_pSourceBand->GetNullValue(&dnull);

    //make sure the type of the null value is correct and also
    //check for NAN in case of floating point numbers since it requires special handling
    //TODO: this functionality could be folded into SetNullValue
    if (type == Band::Double32)
    {
        if (ISNAN(*(float*)&dnull))
            dnull = DBL_NAN; //promote from float nan to double nan
    }

    m_pElevationBand->SetNullValue(Band::Double64, &dnull);

    pGrid->SetElevationBand(m_pElevationBand);

    return true;
}

void GridStyleSurfaceHandler::Clear()
{
    m_dZeroValue = 0;
    m_dScaleFactor = 1;
    m_bDoAdjust = false;
    m_pSourceBand = NULL;
    m_pElevationBand = NULL;

    // It's only a referrence. Need not to delete it inside handler
    m_pReporter = NULL;
}

void GridStyleSurfaceHandler::Visit(unsigned int x, unsigned int y)
{
    assert(m_bDoAdjust);
    double value = 0.0;

    if (m_pSourceBand->GetValueAsDouble(x, y, value))
    {
        value -= m_dZeroValue;
        value *= m_dScaleFactor;

        m_pElevationBand->SetValue(x, y, Band::Double64, &value);
    }
}

void GridStyleSurfaceHandler::SetStatusReporter(GridStatusReporter *pReporter)
{
    m_pReporter = pReporter;
}

bool GridStyleSurfaceHandler::Visit()
{
    // Status Reporter should be set before Visit()
    assert(m_pReporter != NULL);
    if (m_pReporter == NULL)
        return false;

    assert(m_bDoAdjust);
    if (m_bDoAdjust == false)
        return false;

    // Iterate through every pixel
    for (unsigned int y = 0; y < m_pSourceBand->GetYCount(); ++y)
    {
        if (!m_pReporter->Step()) // Notify that it finished one step.
        {
            // If the Step() returns false, that means the reporter encounters some unknown
            // errors. Such as users cancels this transaction.
            // So we break out of this loop and call Rollback() to inform the reporter that
            // it can rollback the transaction now.
            return false;
        }

        for (unsigned int x = 0; x < m_pSourceBand->GetXCount(); ++x)
        {
            Visit(x, y);
        }
    }

    return true;
}
