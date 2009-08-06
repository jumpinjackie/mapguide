//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "CoordSysUtil.h"       //for Convert_Wide_To_Ascii
#include "CriticalSection.h"    //for SmartCriticalClass

#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"
#include "CoordSysGridGeneric.h"       //for CCoordinateSystemGridGeneric

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemGridGeneric::CCoordinateSystemGridGeneric(bool bSetExceptionsOn)
                            :
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_nLastError    (0),
                       m_pCsSource     (),
                       m_pCsTarget     (),
                       m_FrameBoundary (),
                       m_TheGrid       (0)
{
}
CCoordinateSystemGridGeneric::CCoordinateSystemGridGeneric(MgCoordinateSystem* pSourceCs,
                                                           MgCoordinateSystem* pTargetCs,
                                                           bool bSetExceptionsOn)
                            :
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_nLastError    (0),
                       m_pCsSource     (),
                       m_pCsTarget     (),
                       m_FrameBoundary (),
                       m_TheGrid       (0)
{
    m_pCsSource = SAFE_ADDREF (pSourceCs);      // Grid coordinate system
    m_pCsTarget = SAFE_ADDREF (pTargetCs);      // Viewport coordinate system
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemGridGeneric::~CCoordinateSystemGridGeneric()
{
    delete m_TheGrid;
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
    m_TheGrid = new CCoordinateSystemOneGrid (m_FrameBoundary,m_pCsSource,m_pCsTarget);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridBoundary* CCoordinateSystemGridGeneric::GetBoundary(void)
{
    return SAFE_ADDREF (m_FrameBoundary.p);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridLineCollection* CCoordinateSystemGridGeneric::GetGridLines (MgCoordinateSystemGridSpecification* specification)
{
    MgCoordinateSystemGridLineCollection* gridLines;

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
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemGridGeneric::GetConvergenceAngle (MgCoordinate* location)
{
    return 0.0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemGridGeneric::GetProjectiveGridScale (MgCoordinate* location)
{
    return 1.0;
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

//End of file.
