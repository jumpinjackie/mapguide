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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CoordSysUtil.h"       //for Convert_Wide_To_UTF8
#include "CriticalSection.h"    //for SmartCriticalClass

#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"
#include "CoordSysGridGeneric.h"       //for CCoordinateSystemGridGeneric

using namespace CSLibrary;

const INT32 CCoordinateSystemGridGeneric::m_GridLineExceptionLevelK   =  50000000L;     //   50MB
const INT32 CCoordinateSystemGridGeneric::m_GridRegionExceptionLevelK =  30000000L;      //  30MB
const INT32 CCoordinateSystemGridGeneric::m_GridTickExceptionLevelK   =  20000000L;      //  20MB

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemGridGeneric::CCoordinateSystemGridGeneric(bool bSetExceptionsOn)
                            :
                       m_bExceptionsOn            (bSetExceptionsOn),
                       m_GridLineExceptionLevel   (m_GridLineExceptionLevelK),
                       m_GridRegionExceptionLevel (m_GridRegionExceptionLevelK),
                       m_GridTickExceptionLevel   (m_GridTickExceptionLevelK),
                       m_nLastError    (0),
                       m_pCsSource     (),
                       m_pCsTarget     (),
                       m_FrameBoundary (),
                       m_TheGrid       (0)
{
    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;
}
CCoordinateSystemGridGeneric::CCoordinateSystemGridGeneric(MgCoordinateSystem* pSourceCs,
                                                           MgCoordinateSystem* pTargetCs,
                                                           bool bSetExceptionsOn)
                            :
                       m_bExceptionsOn            (bSetExceptionsOn),
                       m_GridLineExceptionLevel   (m_GridLineExceptionLevelK),
                       m_GridRegionExceptionLevel (m_GridRegionExceptionLevelK),
                       m_GridTickExceptionLevel   (m_GridTickExceptionLevelK),
                       m_nLastError    (0),
                       m_pCsSource     (),
                       m_pCsTarget     (),
                       m_FrameBoundary (),
                       m_TheGrid       (0)
{
    m_pCsSource = SAFE_ADDREF (pSourceCs);      // Grid coordinate system
    m_pCsTarget = SAFE_ADDREF (pTargetCs);      // Viewport coordinate system

    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemGridGeneric::~CCoordinateSystemGridGeneric()
{
    SAFE_RELEASE (m_TheGrid);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemGridGeneric::GetSpecializationType ()
{
    return MgCoordinateSystemGridSpecializationType::Generic;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemGridGeneric::SetBoundary(MgCoordinateSystemGridBoundary* pFrameBoundary)
{
    m_FrameBoundary = SAFE_ADDREF (pFrameBoundary);
    m_TheGrid = new CCoordinateSystemOneGrid (m_FrameBoundary,m_pCsSource,m_pCsTarget,
                                              m_GridLineMemoryThreshold,m_GridTickMemoryThreshold);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridBoundary* CCoordinateSystemGridGeneric::GetBoundary(void)
{
    return SAFE_ADDREF (m_FrameBoundary.p);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridLineCollection* CCoordinateSystemGridGeneric::GetGridLines (MgCoordinateSystemGridSpecification* specification)
{
    MgCoordinateSystemGridLineCollection* gridLines = 0;

    if (m_TheGrid != 0)
    {
       gridLines = m_TheGrid->GetGridLines (specification);
    }
    return gridLines;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridRegionCollection* CCoordinateSystemGridGeneric::GetGridRegions (MgCoordinateSystemGridSpecification* specification)
{
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridTickCollection* CCoordinateSystemGridGeneric::GetGridTicks (MgCoordinateSystemGridSpecification* specification)
{
    CCoordinateSystemGridTickCollection* gridTicks = m_TheGrid->GetBoundaryTicks (specification);
    return static_cast<MgCoordinateSystemGridTickCollection*>(gridTicks);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemGridGeneric::GetConvergenceAngle (MgCoordinate* location)
{
    double longitude;
    double latitude;
    double convergence;

    longitude = location->GetX ();
    latitude  = location->GetY ();
    convergence = m_pCsTarget->GetConvergence (longitude,latitude);
    return convergence;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemGridGeneric::GetProjectiveGridScale (MgCoordinate* location)
{
    double longitude;
    double latitude;
    double scale;

    longitude = location->GetX ();
    latitude  = location->GetY ();
    scale = m_pCsTarget->GetScale (longitude,latitude);
    return scale;
}
INT32 CCoordinateSystemGridGeneric::ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 memoryUse (-1);

    if (m_FrameBoundary != 0)
    {
        memoryUse = m_TheGrid->ApproxGridLineMemoryUsage (specification);
    }
    return memoryUse;
}
INT32 CCoordinateSystemGridGeneric::ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 memoryUse;

    memoryUse = (m_FrameBoundary != 0) ? 0 : -1;
    return memoryUse;
}
INT32 CCoordinateSystemGridGeneric::ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 memoryUse (-1);

    if (m_FrameBoundary != 0)
    {
        memoryUse = m_TheGrid->ApproxGridTickMemoryUsage (specification);
    }
    return memoryUse;
}
INT32 CCoordinateSystemGridGeneric::SetGridLineExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridLineExceptionLevel;
    if (memoryUseMax > 0)
    {
        m_GridLineExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;
    if (m_TheGrid != 0)
    {
        m_TheGrid->ResetGridLineMemoryThreshold(m_GridLineMemoryThreshold);
    }
    return rtnValue;
}
INT32 CCoordinateSystemGridGeneric::SetGridRegionExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridRegionExceptionLevel;
    if (memoryUseMax > 0)
    {
        m_GridRegionExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;
    return rtnValue;
}
INT32 CCoordinateSystemGridGeneric::SetGridTickExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridTickExceptionLevel;
    if (memoryUseMax > 0)
    {
        m_GridTickExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;
    if (m_TheGrid != 0)
    {
        m_TheGrid->ResetGridTickMemoryThreshold(m_GridTickMemoryThreshold);
    }
    return rtnValue;
}

INT32 CCoordinateSystemGridGeneric::GetLastError()
{
    return m_nLastError;
}
void CCoordinateSystemGridGeneric::ResetLastError()
{
    m_nLastError = 0;
}
bool CCoordinateSystemGridGeneric::AreExceptionsOn()
{
    return m_bExceptionsOn;
}
void CCoordinateSystemGridGeneric::SetExceptionsOn(bool bOn)
{
    m_bExceptionsOn = bOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//from MgDisposable
void CCoordinateSystemGridGeneric::Dispose()
{
    delete this;
}
