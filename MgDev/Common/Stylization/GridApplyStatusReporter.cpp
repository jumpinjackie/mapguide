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
#include "GridApplyStatusReporter.h"
#include "GridStylizer.h"
#include <assert.h>

//
// GridApplyStatusReporter
//

GridApplyStatusReporter::GridApplyStatusReporter()
{
    m_stylizer = NULL;
    m_totalPixelCount = 0;
    m_currentPixelCount = 0;
    m_currentTime = 0;
    m_currentStep = 0;
    m_isTerminate = false;
}

void GridApplyStatusReporter::Init(
    GridStylizer *stylizer,
    double pixelCount)
{
    m_stylizer = stylizer;
    m_totalPixelCount = pixelCount;
    m_currentPixelCount = 0;
    m_currentTime = 0;
    m_currentStep = 0;
    m_isTerminate = false;

    double totalVal = pixelCount; // Shoue be an integer, but may be a very large integer, so use double.
    // -------------------------------------------------------------
    // The BOUNDARY is the max value of the range of the progess.
    // Set the reasonable value.
    // -------------------------------------------------------------
    const int BOUNDARY = 100;

    if (totalVal <= BOUNDARY)
    {
        m_times    = 1;
        m_beginPos = 0;
        m_endPos   = static_cast<int>(totalVal);
        m_stepVal  = 1;
    }
    else
    {
        assert((totalVal / BOUNDARY) <= INT_MAX);

        m_times    = static_cast<int>(totalVal / BOUNDARY);
        m_beginPos = 0;
        m_endPos   = BOUNDARY;
        m_stepVal  = 1;
    }
}

void GridApplyStatusReporter::Begin()
{
    m_stylizer->Fire_OnBeginApply(m_beginPos, m_endPos, m_stepVal);
}

void GridApplyStatusReporter::Commit()
{
    // That may be called when users cancel the process,
    // But the visitor had been finished the loop.
    // So the visitor called the StatusReport::Commit().
    // We should check m_isTerminate here to make sure
    // to cancel not to finish.
    if (m_isTerminate)
    {
        Rollback();
        return;
    }

    m_stylizer->Fire_OnEndApply();
}

void GridApplyStatusReporter::Rollback()
{
    m_stylizer->Fire_OnCancelApply();
}

bool GridApplyStatusReporter::Step(int stepSize)
{
    if (m_isTerminate)
    {
        // Users has set it to false, that means users want to
        // terminate the stylization transaction.
    }
    else
    {
        m_currentTime+= stepSize;
        if (m_currentTime >= m_times)
        {
            m_currentStep += (m_currentTime / m_times);
            m_currentTime = m_currentTime % m_times;
            // If the reactors return false, that means users want to terminate
            // the stylization transaction.
            m_isTerminate = !m_stylizer->Fire_OnStepApply(m_currentStep);
        }
    }

    return !m_isTerminate;
}

void GridApplyStatusReporter::SetTerminate(bool isTerminate)
{
    m_isTerminate = isTerminate;
}

bool GridApplyStatusReporter::IsTerminate() const
{
    return m_isTerminate;
}
