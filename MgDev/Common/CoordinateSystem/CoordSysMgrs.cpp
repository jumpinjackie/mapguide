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
#include "CoordSysMgrsZone.h"
#include "CoordSysMgrs.h"       //for CCoordinateSystemMgrs

using namespace CSLibrary;

const INT32 CCoordinateSystemMgrs::m_GridLineExceptionLevelK   = 40000000L;    // 40MB
const INT32 CCoordinateSystemMgrs::m_GridRegionExceptionLevelK = 60000000L;    // 60MB
const INT32 CCoordinateSystemMgrs::m_GridTickExceptionLevelK   = 20000000L;    // 20MB

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static constants.
const CCoordinateSystemMgrs::CCoordinateSystemMgrsSeries CCoordinateSystemMgrs::MgrsSeriesNormal [6] =
{
    {L"ABCDEFGH", L"ABCDEFGHJKLMNPQRSTUV" },
    {L"JKLMNPQR", L"FGHJKLMNPQRSTUVABCDE" },
    {L"STUVWXYZ", L"ABCDEFGHJKLMNPQRSTUV" },
    {L"ABCDEFGH", L"FGHJKLMNPQRSTUVABCDE" },
    {L"JKLMNPQR", L"ABCDEFGHJKLMNPQRSTUV" },
    {L"STUVWXYZ", L"FGHJKLMNPQRSTUVABCDE" }
};
const CCoordinateSystemMgrs::CCoordinateSystemMgrsSeries CCoordinateSystemMgrs::MgrsSeriesBessel [6] =
{
    {L"ABCDEFGH", L"LMNPQRSTUVABCDEFGHJK" },
    {L"JKLMNPQR", L"RSTUVABCDEFGHJKLMNPQ" },
    {L"STUVWXYZ", L"LMNPQRSTUVABCDEFGHJK" },
    {L"ABCDEFGH", L"RSTUVABCDEFGHJKLMNPQ" },
    {L"JKLMNPQR", L"LMNPQRSTUVABCDEFGHJK" },
    {L"STUVWXYZ", L"RSTUVABCDEFGHJKLMNPQ" }
};
const wchar_t CCoordinateSystemMgrs::MgrsSeriesPolarSouth [2][25] =
{
    {L"JKLPQRSTUXYZABCFGHJKLPQR" },
    {L"ABCDEFGHJKLMNPQRSTUVWXYZ" }
};
const wchar_t CCoordinateSystemMgrs::MgrsSeriesPolarNorth [2][15] =
{
    {L"RSTUXYZABCFGHJ" },
    {L"ABCDEFGHJKLMNP" }
};
const wchar_t CCoordinateSystemMgrs::MgrsGridZoneDesignation [25] = L"ABCDEFGHJKLMNPQRSTUVWXYZ";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMgrs::CCoordinateSystemMgrs(INT8 nLetteringScheme, bool bSetExceptionsOn)
                            :
                       m_nLetteringScheme         (nLetteringScheme),
                       m_bExceptionsOn            (bSetExceptionsOn),
                       m_bUseFrameDatum           (false),
                       m_GridLineExceptionLevel   (m_GridLineExceptionLevelK),
                       m_GridRegionExceptionLevel (m_GridRegionExceptionLevelK),
                       m_GridTickExceptionLevel   (m_GridTickExceptionLevelK),
                       m_nLastError               (0),
                       m_pCsTarget                (),
                       m_pCsMgrs                  (NULL),
                       m_GridBoundary             (),
                       m_ZoneCollection           (),
                       m_GraticuleUtm             (),
                       m_GraticuleUpsNorth        (),
                       m_GraticuleUpsSouth        ()
{
    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;
}
CCoordinateSystemMgrs::CCoordinateSystemMgrs(MgCoordinateSystem* pTargetCs,INT8 nLetteringScheme,
                                                                           bool bSetExceptionsOn)
                            :
                       m_nLetteringScheme         (nLetteringScheme),
                       m_bExceptionsOn            (bSetExceptionsOn),
                       m_bUseFrameDatum           (false),
                       m_GridLineExceptionLevel   (m_GridLineExceptionLevelK),
                       m_GridRegionExceptionLevel (m_GridRegionExceptionLevelK),
                       m_GridTickExceptionLevel   (m_GridTickExceptionLevelK),
                       m_nLastError               (0),
                       m_pCsTarget                (),
                       m_pCsMgrs                  (NULL),
                       m_GridBoundary             (),
                       m_ZoneCollection           (),
                       m_GraticuleUtm             (),
                       m_GraticuleUpsNorth        (),
                       m_GraticuleUpsSouth        ()

{
    m_pCsTarget = SAFE_ADDREF (pTargetCs);

    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMgrs::~CCoordinateSystemMgrs()
{
    CSdeleteMgrs(m_pCsMgrs);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::Init(double dEquatorialRadius, double dEccentricity)
{
    short sBessel=GetBesselFromLetteringScheme(m_nLetteringScheme);

    // Initialize the critical section class
    SmartCriticalClass critical(true);

    m_pCsMgrs=CSnewMgrs(dEquatorialRadius, dEccentricity, sBessel);
    if (!m_pCsMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.Init", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InitializationFailed;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::InitFromEllipsoid(CREFSTRING sEllipsoidCode)
{
    short sBessel=GetBesselFromLetteringScheme(m_nLetteringScheme);

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sEllipsoidCode.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        if (m_bExceptionsOn)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemMgrs.InitFromEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
            m_pCsMgrs=NULL;
            return;
        }
    }

    // Initialize the critical section class
    SmartCriticalClass critical(true);

    m_pCsMgrs=CSnewMgrsE(pStr, sBessel);
    //Free the converted string
    delete [] pStr;

    if (!m_pCsMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.InitFromEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InitializationFailed;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::InitFromDatum(CREFSTRING sDatumCode)
{
    short sBessel=GetBesselFromLetteringScheme(m_nLetteringScheme);

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sDatumCode.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        if (m_bExceptionsOn)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemMgrs.InitFromDatum", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
            m_pCsMgrs=NULL;
            return;
        }
    }

    // Initialize the critical section class
    SmartCriticalClass critical(true);

    m_pCsMgrs=CSnewMgrsD(pStr, sBessel);
    //Free the converted string
    delete [] pStr;

    if (!m_pCsMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.InitFromDatum", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InitializationFailed;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
short CCoordinateSystemMgrs::GetBesselFromLetteringScheme(INT8 nLetteringScheme)
{
    short sBessel=0; //for normal letteringScheme

    if (MgCoordinateSystemMgrsLetteringScheme::Alternative==nLetteringScheme)
    {
        sBessel=1;
    }
    return sBessel;
}

//PUBLISHED_API

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STRING CCoordinateSystemMgrs::ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision)
{
    STRING sMgrs;
    ConvertFromLonLat(dLongitude, dLatitude, nPrecision, sMgrs);

    //if exception mode is on and excetion is thrown internally we exit anyway
    //so safe to return sMgrs here and not test the m_bExceptionsOn value
    //m_nLastError is already set if a failure occurred and m_bExceptionsOn=false
    return sMgrs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STRING CCoordinateSystemMgrs::ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision)
{
    STRING sMgrs;
    ConvertFromLonLat(pLonLat, nPrecision, sMgrs);

    //if exception mode is on and excetion is thrown internally we exit anyway
    //so safe to return sMgrs here and not test the m_bExceptionsOn value
    //m_nLastError is already set if a failure occurred and m_bExceptionsOn=false
    return sMgrs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinate* CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs)
{
    MgCoordinate* lonLat;
    lonLat = ConvertToLonLat(sMgrs,MgCoordinateSystemMgrsGridSquarePosition::Center);
    return lonLat;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinate* CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs, INT32 grdSqrPosition)
{
    double dLongitude, dLatitude;
    INT32 nResult=ConvertToLonLat(sMgrs, dLongitude, dLatitude, grdSqrPosition);
    if (MgCoordinateSystemErrorCode::Ok==nResult)
    {
        MgCoordinate* pLonLat=new MgCoordinateXY(dLongitude, dLatitude);
        if (NULL==pLonLat)
        {
            if (m_bExceptionsOn)
            {
                throw new MgOutOfMemoryException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            else
            {
                m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
                return NULL;
            }
        }
        else
        {
            return pLonLat;
        }
    }

    //if exception mode is on and excetion is thrown internally we exit anyway
    //so safe to return NULL here and not test the m_bExceptionsOn value
    //m_nLastError is already set if a failure occurred and m_bExceptionsOn=false
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT8 CCoordinateSystemMgrs::GetLetteringScheme()
{
    return m_nLetteringScheme;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::InitMgrsSpecification (MgCoordinateSystemGridSpecification* pSpecification,
                                                   INT32 mgrsGridLevel)
{
    MG_TRY ()
        // Initialize the standard stuff.
        pSpecification->SetGridBase (0.0,0.0);
        if (mgrsGridLevel == MgCoordinateSystemMgrsGridLevel::MgrsUtm)
        {
            pSpecification->SetGridIncrement (6.0,8.0);
            pSpecification->SetTickIncrements  (1.0,1.0);
            pSpecification->SetUnits (MgCoordinateSystemUnitCode::Degree,MgCoordinateSystemUnitType::Angular);
            pSpecification->SetCurvePrecision (1.0E-05);
        }
        else if (mgrsGridLevel == MgCoordinateSystemMgrsGridLevel::MgrsUps)
        {
            pSpecification->SetGridIncrement (4.0,4.0);
            pSpecification->SetTickIncrements  (1.0,1.0);
            pSpecification->SetUnits (MgCoordinateSystemUnitCode::Degree,MgCoordinateSystemUnitType::Angular);
            pSpecification->SetCurvePrecision (1.0E-05);
        }
        else if (mgrsGridLevel >= MgCoordinateSystemMgrsGridLevel::Mgrs100Km &&
                 mgrsGridLevel <= MgCoordinateSystemMgrsGridLevel::Mgrs1m)
        {
            double gridIncrement;
            switch (mgrsGridLevel) {
            case MgCoordinateSystemMgrsGridLevel::Mgrs100Km:
                gridIncrement = 100000.0;
                break;
            case MgCoordinateSystemMgrsGridLevel::Mgrs10Km:
                gridIncrement = 10000.0;
                break;
            case MgCoordinateSystemMgrsGridLevel::Mgrs1Km:
                gridIncrement = 1000.0;
                break;
            case MgCoordinateSystemMgrsGridLevel::Mgrs100m:
                gridIncrement = 100.0;
                break;
            case MgCoordinateSystemMgrsGridLevel::Mgrs10m:
                gridIncrement = 10.0;
                break;
            case MgCoordinateSystemMgrsGridLevel::Mgrs1m:
                gridIncrement = 1.0;
                break;
            default:
                // to keep lint happy
                gridIncrement = 0.0;
                break;
            }
            pSpecification->SetGridIncrement (gridIncrement,gridIncrement);
            pSpecification->SetTickIncrements  (gridIncrement / 10.0,gridIncrement / 10.0);
            pSpecification->SetUnits (MgCoordinateSystemUnitCode::Meter,MgCoordinateSystemUnitType::Linear);
            pSpecification->SetCurvePrecision (0.25);
        }
        else
        {
            // Parameter error
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::InitMgrsSpecification")
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::GetSpecializationType ()
{
    return MgCoordinateSystemGridSpecializationType::MGRS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::SetBoundary(MgCoordinateSystemGridBoundary* pGridBoundary)
{
    // Save the boundary.
    m_GridBoundary = SAFE_ADDREF (pGridBoundary);

    // Create a CCoordinateSystemOneGrid which is appropriate for generating
    // MGRS graticule requests.
    m_ZoneCollection = FrameBoundaryToZones (m_GridBoundary,m_pCsTarget,m_bUseFrameDatum);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridBoundary* CCoordinateSystemMgrs::GetBoundary(void)
{
    // The following code should return NULL if the boundary has not been set.
    // That is the desired result.
    return SAFE_ADDREF (m_GridBoundary.p);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridLineCollection* CCoordinateSystemMgrs::GetGridLines (MgCoordinateSystemGridSpecification* specification)
{
    bool specIsGrid;

    INT32 index;
    INT32 unitType;
    INT32 zoneCount;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<MgCoordinateSystemGridLineCollection> aGridLineCollection;
    Ptr<CCoordinateSystemGridLineCollection> theGridLineCollection;

    MgCoordinateSystemFactory factory;

    if (m_GridBoundary == 0)
    {
        // Proceeding without a grid boundary will cause a crash.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.GetGridLines",
                                                                   __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_TRY ()
        theGridLineCollection = new CCoordinateSystemGridLineCollection (m_GridLineMemoryThreshold);

        // Determine the grid type.
        unitType = specification->GetUnitType();
        specIsGrid = (unitType ==  MgCoordinateSystemUnitType::Linear);

        // Process each zone in our zone collection.  Originally, we thought
        // we could just call the GetGridLines function of the OneGrid
        // base class, but there are just too many special cases to deal with.
        // So, rather than kludge a bunch of special stuff into OneGrid and
        // compromise its generality, we have our own functions in this MGRS
        // specific object which duplicates much of the functionality of the
        // OneGrid objoect.
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            if (specIsGrid)
            {
                // The specification calls for a grid.  The following function
                // is smart enough to deal with the special nature of zones
                // 31 through 37 at the higher northern latitudes.
                aGridLineCollection = mgrsZoneGrid->GetGridLines (m_GridBoundary,specification);
            }
            else
            {
                // The specification calls for a graticule  This object is
                // smart enough to deal with band X (which is 12 degrees high)
                // and the special nature of zones 31 through 37 at the higher
                // northern latitudes.
                aGridLineCollection = mgrsZoneGrid->GetGraticuleLines (m_GridBoundary,specification);
            }
            if (aGridLineCollection != 0)
            {
                // Add whatever we got to the collection we are accumulating.
                theGridLineCollection->AddCollection (aGridLineCollection);
                aGridLineCollection = 0;
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridLines")
    return static_cast<MgCoordinateSystemGridLineCollection*>(theGridLineCollection.Detach());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridRegionCollection* CCoordinateSystemMgrs::GetGridRegions (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<CCoordinateSystemGridRegionCollection> theGridRegionCollection;

    if (m_GridBoundary == 0)
    {
        // Proceeding without a grid boundary will cause a crash.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.GetGridRegions",
                                                                   __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_TRY ()
        theGridRegionCollection = new CCoordinateSystemGridRegionCollection (m_GridRegionMemoryThreshold);
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            Ptr<MgCoordinateSystemGridRegionCollection> aGridRegionCollection;
            aGridRegionCollection = mgrsZoneGrid->GetGridRegions (m_GridBoundary,specification);
            theGridRegionCollection->AddCollection (aGridRegionCollection);
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridRegions")
    return static_cast<MgCoordinateSystemGridRegionCollection*>(theGridRegionCollection.Detach());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridTickCollection* CCoordinateSystemMgrs::GetGridTicks (MgCoordinateSystemGridSpecification* specification)
{
    bool specIsGrid;

    INT32 index;
    INT32 unitType;
    INT32 zoneCount;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<MgCoordinateSystemGridTickCollection> aGridTickCollection;
    Ptr<CCoordinateSystemGridTickCollection> theGridTickCollection;

    if (m_GridBoundary == 0)
    {
        // Proceeding without a grid boundary will cause a crash.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.GetGridRegions",
                                                                   __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_TRY ()
        theGridTickCollection = new CCoordinateSystemGridTickCollection (m_GridTickMemoryThreshold);
        unitType = specification->GetUnitType();
        specIsGrid = (unitType ==  MgCoordinateSystemUnitType::Linear);
        if (specIsGrid)
        {
            zoneCount = m_ZoneCollection->GetCount ();
            for (index = 0;index < zoneCount;index += 1)
            {
                mgrsZoneGrid = m_ZoneCollection->GetItem (index);
                aGridTickCollection = mgrsZoneGrid->GetBoundaryTicks (specification);
                theGridTickCollection->AddCollection (aGridTickCollection);
            }
        }
        else
        {
            if (m_GraticuleUtm)
            {
                aGridTickCollection = m_GraticuleUtm->GetBoundaryTicks (specification);
                theGridTickCollection->AddCollection (aGridTickCollection);
            }
            if (m_GraticuleUpsNorth)
            {
                aGridTickCollection = m_GraticuleUpsNorth->GetBoundaryTicks (specification);
                theGridTickCollection->AddCollection (aGridTickCollection);
            }
            if (m_GraticuleUpsSouth)
            {
                aGridTickCollection = m_GraticuleUpsSouth->GetBoundaryTicks (specification);
                theGridTickCollection->AddCollection (aGridTickCollection);
            }
        }
        // TODO:  Need to remove from the collection any grid ticks which are
        // internal to the m_GridBoundary object of this object.  This will be
        // tricky, as the tick positions will usually be right on the boundary.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridTicks")
    return static_cast<MgCoordinateSystemGridTickCollection*>(theGridTickCollection.Detach());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMgrs::GetConvergenceAngle (MgCoordinate* location)
{
    return 0.0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMgrs::GetProjectiveGridScale (MgCoordinate* location)
{
    return 1.0;
}

INT32 CCoordinateSystemMgrs::ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    INT32 memoryGuess (-1);

    if (m_GridBoundary != 0)
    {
        memoryGuess = 0;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            memoryGuess += mgrsZoneGrid->ApproxGridLineMemoryUsage (specification);
        }
    }
    return memoryGuess;
}
INT32 CCoordinateSystemMgrs::ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    INT32 memoryUse;
    INT32 maxValue;
    INT32 memoryGuess (-1);

    if (m_GridBoundary != 0)
    {
        memoryGuess = 0;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            memoryUse = mgrsZoneGrid->ApproxGridRegionMemoryUsage (specification);
            maxValue = 0x7FFF0000 - memoryGuess;
            if (memoryUse < maxValue)
            {
                memoryGuess += memoryUse;
            }
            else
            {
                memoryGuess = 0x7FFFFFFF;
                break;
            }
        }
    }
    return memoryGuess;
}
INT32 CCoordinateSystemMgrs::ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    INT32 memoryGuess (-1);

    if (m_GridBoundary != 0)
    {
        memoryGuess = 0;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            memoryGuess += mgrsZoneGrid->ApproxGridTickMemoryUsage (specification);
        }
    }
    return memoryGuess;
}
INT32 CCoordinateSystemMgrs::SetGridLineExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridLineExceptionLevel;
    if (memoryUseMax > 0L)
    {
        m_GridLineExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridLineMemoryThreshold = (availableMemory > m_GridLineExceptionLevel) ? availableMemory - m_GridLineExceptionLevel : 0L;

    if (m_ZoneCollection != 0)
    {
        INT32 index;
        INT32 zoneCount;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            mgrsZoneGrid->ResetGridLineMemoryThreshold(m_GridLineMemoryThreshold);
        }
    }
    if (m_GraticuleUtm != 0)
    {
        m_GraticuleUtm->ResetGridLineMemoryThreshold(m_GridLineMemoryThreshold);
    }
    if (m_GraticuleUpsNorth != 0)
    {
        m_GraticuleUpsNorth->ResetGridLineMemoryThreshold(m_GridLineMemoryThreshold);
    }
    if (m_GraticuleUpsSouth != 0)
    {
        m_GraticuleUpsSouth->ResetGridLineMemoryThreshold(m_GridLineMemoryThreshold);
    }
    return rtnValue;
}
INT32 CCoordinateSystemMgrs::SetGridRegionExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridRegionExceptionLevel;
    if (memoryUseMax > 0L)
    {
        m_GridRegionExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridRegionMemoryThreshold = (availableMemory > m_GridRegionExceptionLevel) ? availableMemory - m_GridRegionExceptionLevel : 0L;

    if (m_ZoneCollection != 0)
    {
        INT32 index;
        INT32 zoneCount;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            mgrsZoneGrid->ResetGridRegionMemoryThreshold(m_GridRegionMemoryThreshold);
        }
    }
    return rtnValue;
}
INT32 CCoordinateSystemMgrs::SetGridTickExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridTickExceptionLevel;
    if (memoryUseMax > 0L)
    {
        m_GridTickExceptionLevel = memoryUseMax;
    }
    INT64 availableMemory = GetAvailableMemory();
    m_GridTickMemoryThreshold = (availableMemory > m_GridTickExceptionLevel) ? availableMemory - m_GridTickExceptionLevel : 0L;

    if (m_ZoneCollection != 0)
    {
        INT32 index;
        INT32 zoneCount;
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            mgrsZoneGrid->ResetGridTickMemoryThreshold(m_GridTickMemoryThreshold);
        }
    }
    if (m_GraticuleUtm != 0)
    {
        m_GraticuleUtm->ResetGridTickMemoryThreshold(m_GridTickMemoryThreshold);
    }
    if (m_GraticuleUpsNorth != 0)
    {
        m_GraticuleUpsNorth->ResetGridTickMemoryThreshold(m_GridTickMemoryThreshold);
    }
    if (m_GraticuleUpsSouth != 0)
    {
        m_GraticuleUpsSouth->ResetGridTickMemoryThreshold(m_GridTickMemoryThreshold);
    }
    return rtnValue;
}

//INTERNAL_API
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision, REFSTRING sMgrs)
{
    if (!m_pCsMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InitializationFailed;
            return m_nLastError;
        }
    }

    char szMgrs[16]; //max size for an MGRS string, e.g. 04QFJ1234567890
    double latLng [2];
    latLng [0]=dLongitude; //the "latLng" CsMap variable name is confusing. The order is really LonLat
    latLng [1]=dLatitude;

    nPrecision = nPrecision>5 ? 5 : nPrecision;
    nPrecision = nPrecision<0 ? 0 : nPrecision;

    int nResult=CScalcMgrsFromLl (m_pCsMgrs, szMgrs, sizeof(szMgrs), latLng, nPrecision);
    if (0==nResult)
    {
        //success, return the MGRS string
        wchar_t* pMgrs = Convert_UTF8_To_Wide(szMgrs);
        if (NULL == pMgrs)
        {
            if (m_bExceptionsOn)
            {
                throw new MgOutOfMemoryException(L"MgCoordinateSystemMgrs.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
            }
            else
            {
                m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
                return m_nLastError;
            }
        }
        sMgrs=pMgrs;
        delete[] pMgrs;
    }
    else
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemMgrs.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::ConversionFailed;
            return m_nLastError;
        }
    }

    return MgCoordinateSystemErrorCode::Ok;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision, REFSTRING sMgrs)
{
    if (!pLonLat)
    {
        if (m_bExceptionsOn)
        {
            CHECKARGUMENTNULL(pLonLat, L"MgCoordinateSystemMgrs.ConvertFromLonLat");
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::NullArgument;
            return m_nLastError;
        }
    }

    //no need to check the dimension of the MgCoordinate as we only care about X and Y
    return ConvertFromLonLat(pLonLat->GetX(), pLonLat->GetY(), nPrecision, sMgrs);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat)
{
    INT32 nResult;
    nResult = ConvertToLonLat(sMgrs,pLonLat,MgCoordinateSystemMgrsGridSquarePosition::Center);
    return nResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat, INT32 grdSqrPosition)
{
    if (!pLonLat)
    {
        if (m_bExceptionsOn)
        {
            CHECKARGUMENTNULL(pLonLat, L"MgCoordinateSystemMgrs.ConvertToLonLat");
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::NullArgument;
            return m_nLastError;
        }
    }

    //no need to check the dimension of the MgCoordinate as we only care about X and Y
    double dLongitude=pLonLat->GetX();
    double dLatitude=pLonLat->GetY();
    INT32 nResult=ConvertToLonLat(sMgrs, dLongitude, dLatitude, grdSqrPosition);
    if (nResult==MgCoordinateSystemErrorCode::Ok)
    {
        pLonLat->SetX(dLongitude);
        pLonLat->SetY(dLatitude);
    }
    return nResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs, double& dLongitude, double& dLatitude)
{
    INT32 nResult;
    nResult = ConvertToLonLat(sMgrs,dLongitude,dLatitude,MgCoordinateSystemMgrsGridSquarePosition::Center);
    return nResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs, double& dLongitude, double& dLatitude, INT32 grdSqrPosition)
{
    if (!m_pCsMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InitializationFailed;
            return m_nLastError;
        }
    }

    if (grdSqrPosition <= MgCoordinateSystemMgrsGridSquarePosition::None ||
        grdSqrPosition >= MgCoordinateSystemMgrsGridSquarePosition::Unknown)
    {
        if (m_bExceptionsOn)
        {
            throw new MgInvalidArgumentException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::InvalidArgument;
            return m_nLastError;
        }
    }

    //Convert to a char*
    char *pMgrs = Convert_Wide_To_UTF8(sMgrs.c_str()); //need to delete[] pStr
    if (NULL == pMgrs)
    {
        if (m_bExceptionsOn)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
            return m_nLastError;
        }
    }

    double latLng [2];
    int nResult=CScalcLlFromMgrsEx (m_pCsMgrs, latLng, pMgrs, grdSqrPosition);

    //Free the converted string
    delete [] pMgrs;

    if (0==nResult)
    {
        dLongitude=latLng [0]; //the "latLng" CsMap variable name is confusing. The order is really LonLat
        dLatitude=latLng [1];
    }
    else
    {
        if (m_bExceptionsOn)
        {
            throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
        }
        else
        {
            m_nLastError=MgCoordinateSystemErrorCode::ConversionFailed;
            return m_nLastError;
        }
    }

    return MgCoordinateSystemErrorCode::Ok;
}
INT32 CCoordinateSystemMgrs::GetLastError()
{
    return m_nLastError;
}
void CCoordinateSystemMgrs::ResetLastError()
{
    m_nLastError = 0;
}
bool CCoordinateSystemMgrs::AreExceptionsOn()
{
    return m_bExceptionsOn;
}
void CCoordinateSystemMgrs::SetExceptionsOn(bool bOn)
{
    m_bExceptionsOn = bOn;
}
///////////////////////////////////////////////////////////////////////////////
// This member returns a collection of CCoordinateSystemMgrsZone objects.
// These objects derive from CCoordinateSystemOneGrid, so they have both
// the grid and frame coordinate systems in them.
CCoordinateSystemMgrsZoneCollection* CCoordinateSystemMgrs::FrameBoundaryToZones (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                  MgCoordinateSystem* frameCRS,
                                                                                  bool useFrameDatum)
{
    bool northPole (false);
    bool southPole (false);

    INT32 zoneNbr;
    INT32 zoneMin;
    INT32 zoneMax;
    INT32 tmpInt32;

    double cm;                      // central meridian
    double eastLimit, westLimit;    // limits of a UTM zone
    double lngMin, lngMax;          // frame boundary extrema in 'LL84' (or 'LL')
    double latMin, latMax;          // frame boundary extrema in 'LL84' (or 'LL')
    double eastMin, eastMax;        // frame boundary extrema in frame CRS
    double northMin, northMax;      // frame boundary extrema in frame CRS

    Ptr<MgPolygon> pPolygon;
    Ptr<MgPolygon> pPolygonIntersection;
    Ptr<MgCoordinate> pSouthwest;
    Ptr<MgCoordinate> pNortheast;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystem> utmCRS;
    Ptr<MgCoordinateSystemTransform> toLlTransform;               // frame to 'LL' transform
    Ptr<MgCoordinateSystemTransform> toFrameTransform;            // 'LL' to frame transform
    Ptr<MgCoordinateSystemGridBoundary> llBoundary;
    Ptr<MgCoordinateSystemGridBoundary> reducedFrameBoundary;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<CCoordinateSystemMgrsZoneCollection> zoneCollection;
    STRING utmCsCode;

    // Maintenance Note: reducedFrameBoundary has the following properties:
    // 1> It is in frame (i.e. viewport) coordinates.
    // 2> It starts out as a boundary which defines the entire UTM zone
    //    (polar or otherwise) which is being created.
    // 3> It is reduced, by "Intersection" with, to the extents of the
    //    frame (i.e. viewport).
    // You could also describe this as object as "That portion of the
    // viewport which is included in the specific UTM zone we are
    // constructing an object for".

    // Maintenance Note: In extreme cases, the use of rectangular viewports
    // to model a spherical world can lead to the consideration of a zone which
    // does not intersect the viewport.  In such cases, the Intersection polygon
    // will be empty.  By MapGuide conventions, that means the pPolygonIntersection
    // pointer will be null.  In this case, we must not add the zone to the zone
    // collection.

    MG_TRY ()
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory ();
        zoneCollection = new CCoordinateSystemMgrsZoneCollection ();
        pSouthwest = new MgCoordinateXY ();
        pNortheast = new MgCoordinateXY ();

        // Release any graticule objects which may already exist.
        m_GraticuleUtm = 0;
        m_GraticuleUpsNorth = 0;
        m_GraticuleUpsSouth = 0;

        // Get the min/max of the frame in frame coordinates.
        frameBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);

        // Convert the polygon portion of the Grid Boundary object to long/lat
        // coordinates and extract the min/max from the resulting polygon.
        llCRS = csFactory->CreateFromCode (useFrameDatum ? L"LL" : L"LL84");
        toLlTransform = csFactory->GetTransform(frameCRS,llCRS);
        toLlTransform->IgnoreDatumShiftWarning (true);
        toLlTransform->IgnoreOutsideDomainWarning (true);
        toFrameTransform = csFactory->GetTransform(llCRS,frameCRS);
        toFrameTransform->IgnoreDatumShiftWarning (true);
        toFrameTransform->IgnoreOutsideDomainWarning (true);
        pPolygon = frameBoundary->GetBoundary (toLlTransform,1.0E-05);
        llBoundary = csFactory->GridBoundary (pPolygon);
        llBoundary->GetBoundaryExtents (lngMin,lngMax,latMin,latMax);

        // The llBoundary object has been created using rectangular techniques.
        // Since the earth is spherical, in certain rare cases the resulting
        // extents can exclude either (or both, presumably) pole when inclusion
        // is appropriate.  We try to deal with that issue here.  We use this
        // technique only for projections which we believe can properly
        // represent a pole for defensive purposes.

        if (CanDoPoles (frameCRS))
        {
            // Use the toFrameXform object to convert the north pole to the frame
            // coordinate system.  If the conversion is successful, and the result
            // resides within the original cartesian frame boundary, we add the
            // north pole to the extents generated by the GetBoundaryExtents
            // function call.
            if (latMax > 0.0)
            {
                Ptr<MgCoordinate> poleTest = new MgCoordinateXY (0.0,90.0);
                poleTest = toFrameTransform->Transform (poleTest);
                if (poleTest->GetX () >= eastMin  && poleTest->GetX () <= eastMax &&
                    poleTest->GetY () >= northMin && poleTest->GetY () <= northMax)
                {
                    northPole = true;
                }
            }
            if (latMin < 0.0)
            {
                Ptr<MgCoordinate> poleTest = new MgCoordinateXY (0.0,-90.0);
                poleTest = toFrameTransform->Transform (poleTest);
                if (poleTest->GetX () >= eastMin  && poleTest->GetX () <= eastMax &&
                    poleTest->GetY () >= northMin && poleTest->GetY () <= northMax)
                {
                    southPole = true;
                }
            }
        }

        // Use the latitude min/max to extract the UTM/UPS zones as necessary.
        if (latMin < -80.0 || southPole)
        {
            // There is a portion of the frame boundary in the south polar region.
            zoneNbr = -61;

            pPolygon = 0;
            if (southPole)
            {
                // The south pole is included within the provided frame boundary.
                // In this case, the rectangular technique used above would
                // produce a closed polygon which will look like a circular pie
                // with an infitessiammly smal slice in it.  This slice drives
                // the clipper crazy.  So, we need to develop a simple
                // circular closed boundary around, but not including the
                // pole.

                // Since we need to do the same thing for the northern pole as
                // well, we have a function that will do that for us.  THis
                // function will fail if the underlying frame CRS is not
                // suitable for polar aspects.
                pPolygon = ParallelPolygon (toFrameTransform,-80.0,720);
            }
            if (!pPolygon)
            {
                // The south pole is not within the frame boundary, the
                // generation of a circular boundary representing a parallel
                // failed, or the frame CRS is incapabale of representing
                // the pole.  Therefore, we have to use the standard rectangular
                // boundary technique and hope for the best.  This should work
                // fine if the region does not actually include the pole, and
                // the results should be decent in other cases.
                pSouthwest->SetX (lngMin);
                pSouthwest->SetY (latMin);
                pNortheast->SetX (lngMax);
                pNortheast->SetY (-80.0);
                llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            }
            if (pPolygon)
            {
                Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
                pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
                if (pPolygonIntersection != 0)
                {
                    reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                    mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme,
                                                                  m_GridLineMemoryThreshold,m_GridTickMemoryThreshold,m_GridRegionMemoryThreshold);
                    zoneCollection->Add (mgrsZoneGrid);

                    // Construct the m_GraticuleUpsSouth member, it may be needed.
                    m_GraticuleUpsSouth = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS,
                                                                        m_GridLineMemoryThreshold,m_GridTickMemoryThreshold);
                }
            }
        }
        if (latMax > 84.0 || northPole)
        {
            // There is a portion of the frame boundary in the north polar region.
            zoneNbr = 61;

            // Same as for the south pole, sans the laborious comments.
            pPolygon = 0;
            if (northPole)
            {
                pPolygon = ParallelPolygon (toFrameTransform,84.0,720);
            }
            if (pPolygon == 0)
            {
                pSouthwest->SetX (lngMin);
                pSouthwest->SetY (84.0);
                pNortheast->SetX (lngMax);
                pNortheast->SetY (latMax);
                llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            }
            if (pPolygon != 0)
            {
                Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
                pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
                if (pPolygonIntersection != 0)
                {
                    reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                    mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme,
                                                                  m_GridLineMemoryThreshold,m_GridTickMemoryThreshold,m_GridRegionMemoryThreshold);
                    zoneCollection->Add (mgrsZoneGrid);

                    // Construct the m_GraticuleUpsNorth member, it may be needed.
                    m_GraticuleUpsNorth = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS,
                                                                        m_GridLineMemoryThreshold,m_GridTickMemoryThreshold);
                }
            }
        }
        if (latMax > -80.0 && latMin < 84.0)
        {
            // A portion of the frame boundary is in the region covered by the
            // normal (i.e. non-polar) UTM zones.  Determine the particular UTM
            // zones we need to generate.
            tmpInt32 = static_cast <INT32>(floor (lngMin));
            zoneMin = ((tmpInt32 + 180) / 6) + 1;
            tmpInt32 = static_cast <INT32>(ceil (lngMax));
            zoneMax = ((tmpInt32 + 180) / 6) + 1;

            if (latMax > 0.0)
            {
                // There are some northern zones.

                // We will need to generate, for each UTM zone, the extents
                // of that particular UTM zone trimmed to the extents of the
                // provided frame boundary.  The north/south portions
                // are the same for each zone, so we do that once here outside
                // the loop.
                pSouthwest->SetY ((latMin <  0.0) ?  0.0 : latMin);
                pNortheast->SetY ((latMax > 84.0) ? 84.0 : latMax);

                // OK, generate a CCoordinateSystemMgrsZone object for each
                // UTM zone which intersects the frame boundary provided.
                // Usually only one, but we must be able to handle the
                // multiple zone case.
                for (zoneNbr = zoneMin;zoneNbr <= zoneMax;zoneNbr += 1)
                {
                    // 1> Compute the 'LL' boundary of this UTM zone.
                    //    North/South done above, here we need only deal
                    //    with east/west.
                    cm = static_cast<double>((zoneNbr * 6) - 183);
                    westLimit = cm - 3.0;
                    eastLimit = cm + 3.0;

                    // 2> Apply the extents of the provided frame boundary.
                    if (westLimit < lngMin) westLimit = lngMin;
                    if (eastLimit > lngMax) eastLimit = lngMax;

                    // 3> Create, in terms of LL coordinates, the frame
                    //    boundary as is appropriate for this particular zone.
                    pSouthwest->SetX (westLimit);
                    pNortheast->SetX (eastLimit);

                    // 4> Convert this reduced frame boundary back to frame
                    //    coordinates.
                    llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                    pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
                    Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
                    pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
                    if (pPolygonIntersection != 0)
                    {
                        reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                        mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme,
                                                                      m_GridLineMemoryThreshold,m_GridTickMemoryThreshold,m_GridRegionMemoryThreshold);
                        zoneCollection->Add (mgrsZoneGrid);
                    }
                 }
            }
            if (latMin < 0.0)
            {
                // Pretty much the same as the northern zones processed
                // above, but without the laborious comments.
                pSouthwest->SetY ((latMin < -80.0) ? -80.0 : latMin);
                pNortheast->SetY ((latMax >   0.0) ?   0.0 : latMax);
                for (zoneNbr = zoneMin;zoneNbr <= zoneMax;zoneNbr += 1)
                {
                    cm = static_cast<double>((zoneNbr * 6) - 183);
                    westLimit = cm - 3.0;
                    eastLimit = cm + 3.0;
                    if (westLimit < lngMin) westLimit = lngMin;
                    if (eastLimit > lngMax) eastLimit = lngMax;
                    pSouthwest->SetX (westLimit);
                    pNortheast->SetX (eastLimit);

                    llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                    pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
                    Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
                    pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
                    if (pPolygonIntersection != 0)
                    {
                        reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                        mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,-zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme,
                                                                      m_GridLineMemoryThreshold,m_GridTickMemoryThreshold,m_GridRegionMemoryThreshold);
                        zoneCollection->Add (mgrsZoneGrid);
                    }
                }
            }

            // Need a CCoordinateSystemOneGrid object which can produce the 6 x 8 graticule
            // for the entire regon.
            pSouthwest->SetX (lngMin);
            pNortheast->SetX (lngMax);
            pSouthwest->SetY ((latMin < -80.0) ? -80.0 : latMin);
            pNortheast->SetY ((latMax >  84.0) ?  84.0 : latMax);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
            pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
            if (pPolygonIntersection != 0)
            {
                reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                m_GraticuleUtm = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS,
                                                               m_GridLineMemoryThreshold,m_GridTickMemoryThreshold);
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::FrameBoundaryToZones")
    return zoneCollection.Detach ();
}
STRING CCoordinateSystemMgrs::GridSquareDesignation (INT32 utmZoneNbr,double easting,
                                                                      double northing,
                                                                      INT8 letteringScheme)
{
    INT32 iEasting, iNorthing;
    INT32 eastIndex, northIndex;
    INT32 seriesIndex;
    wchar_t squareDesignation [4];

    squareDesignation [0] = L'?';
    squareDesignation [1] = L'?';
    squareDesignation [2] = L'\0';
    squareDesignation [3] = L'\0';

    iEasting  = static_cast<INT32>(easting);
    iNorthing = static_cast<INT32>(northing);

    // For now we assume there is no difference between the northern and southern hemisphere.
    if (utmZoneNbr != 0 && abs(utmZoneNbr) <= 60)
    {
        if (iEasting < 100000) iEasting = 100000;
        if (iEasting > 1000000) iEasting = 1000000;
        if (iNorthing < 0) iNorthing = 0;
        if (iNorthing > 10000000) iEasting = 10000000;
        iNorthing %= 2000000;

        eastIndex  = static_cast<INT32>(iEasting - 100000) / 100000;
        northIndex = static_cast<INT32>(iNorthing) / 100000;
        if (eastIndex < 9 && northIndex < 21)
        {
            seriesIndex = (abs (utmZoneNbr) - 1) % 6;
            if (letteringScheme == MgCoordinateSystemMgrsLetteringScheme::Alternative)
            {
                squareDesignation [0] = MgrsSeriesBessel[seriesIndex].easting[eastIndex];
                squareDesignation [1] = MgrsSeriesBessel[seriesIndex].northing[northIndex];
            }
            else
            {
                squareDesignation [0] = MgrsSeriesNormal[seriesIndex].easting[eastIndex];
                squareDesignation [1] = MgrsSeriesNormal[seriesIndex].northing[northIndex];
            }
        }
    }
    else if (utmZoneNbr == 61)
    {
        // North UPS region
        eastIndex = (iEasting / 100000) - 13;
        northIndex = (iNorthing / 100000) - 13;
        if (eastIndex >= 0  && eastIndex  <= 13 &&
            northIndex >= 0 && northIndex <= 13)
        {
            squareDesignation [0] = MgrsSeriesPolarNorth [0][eastIndex];
            squareDesignation [1] = MgrsSeriesPolarNorth [1][northIndex];
        }
    }
    else if (utmZoneNbr == -61)
    {
        // South UPS region
        eastIndex = (iEasting / 100000) - 8;
        northIndex = (iNorthing / 100000) - 8;
        if (eastIndex >= 0  && eastIndex  <= 19 &&
            northIndex >= 0 && northIndex <= 19)
        {
            squareDesignation [0] = MgrsSeriesPolarSouth [0][eastIndex];
            squareDesignation [1] = MgrsSeriesPolarSouth [1][northIndex];
        }
    }

    STRING designation (squareDesignation);
    return designation;
}
//from MgDisposable
void CCoordinateSystemMgrs::Dispose()
{
    delete this;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Functions
STRING CCoordinateSystemMgrs::ZoneNbrToUtmCs (INT32 zoneNbr)
{
    wchar_t wcBufr [64];
    STRING utmCode;

    if (zoneNbr == 61)
    {
        utmCode = L"WGS84.UPSNorth";
    }
    else if (zoneNbr == -61)
    {
        utmCode = L"WGS84.UPSSouth";
    }
    else if (zoneNbr > 0 &&zoneNbr < 61)
    {
        swprintf (wcBufr,64,L"UTM84-%dN",zoneNbr);
        utmCode = wcBufr;
    }
    else if (zoneNbr < 0 && zoneNbr > -61)
    {
        swprintf (wcBufr,64,L"UTM84-%dS",-zoneNbr);
        utmCode = wcBufr;
    }
    else
    {
        // Invalid zone number.
        utmCode = L"<UnknownUtmZone>";
    }
    return utmCode;
}
INT32 CCoordinateSystemMgrs::GridZoneDesignationIndex (double latitude,double longitude)
{
    INT32 index (-1);

    if (longitude > 180.0) longitude -= 360.0;
    if (latitude < -80.0)
    {
        index = (longitude < 0.0) ? 0 : 1;
    }
    else if (latitude > 84.0)
    {
        index = (longitude < 0.0) ? 22 : 23;
    }
    else
    {
        index = (static_cast<INT32>(latitude) / 8) + 12;
        if (index == 22) index = 21;
    }
    return index;
}
wchar_t CCoordinateSystemMgrs::GridZoneDesignationLetter (INT32 index)
{
    wchar_t designationLetter;

    if (index >= 0 && index < 24)
    {
        designationLetter = MgrsGridZoneDesignation [index];
    }
    else
    {
        designationLetter = L'?';
    }
    return designationLetter;
}

// TODO:  This function probably should be a member of some other object, as
// its product is of value outside the realm of MGRS.
MgPolygon* CCoordinateSystemMgrs::ParallelPolygon (MgCoordinateSystemTransform* transformation,double latitude,
                                                                                               INT32 pointCount)
{
    bool ok (true);
    INT32 index;
    INT32 status;

    double deltaLng;
    double currentLng;

    Ptr<MgCoordinate> llPoint;
    Ptr<MgCoordinate> xyPoint;
    Ptr<MgCoordinate> firstPoint;
    Ptr<MgCoordinateCollection> xyCollection;
    Ptr<MgLinearRing> linearRing;
    Ptr<MgPolygon> polyPtr;
    MgPolygon* result;

    result = 0;
    if (pointCount < 3 || pointCount > 2048)
    {
        pointCount = 720;
    }
    deltaLng = 360.0 / static_cast<double>(pointCount);

    currentLng = -180.0;
    xyCollection = new MgCoordinateCollection ();
    llPoint = new MgCoordinateXY (currentLng,latitude);
    for (index = 0;ok && (index < pointCount);index += 1)
    {
        llPoint->SetX (currentLng);
        xyPoint = transformation->Transform (llPoint);
        status = transformation->GetLastTransformStatus ();
        ok = status == MgCoordinateSystemTransform::TransformOk;
        if (ok)
        {
            xyCollection->Add (xyPoint);
            if (index == 0)
            {
                firstPoint = SAFE_ADDREF (xyPoint.p);
            }
        }
        currentLng += deltaLng;
    }

    if (ok)
    {
        // Close the linear ring for sure.
        xyCollection->Add (firstPoint);

        linearRing = new MgLinearRing (xyCollection);
        polyPtr = new MgPolygon (linearRing,0);
        result = polyPtr.Detach ();
    }
    return result;
}
bool CCoordinateSystemMgrs::CanDoPoles (MgCoordinateSystem* frameCRS)
{
    // MENTOR_MAINTENANCE --> a new projection may need to be added to this list.
    static INT32 polarCapable [] =
    {
        MgCoordinateSystemProjectionCode::Tm,
        MgCoordinateSystemProjectionCode::Trmrs,
        MgCoordinateSystemProjectionCode::Trmeraf,
        MgCoordinateSystemProjectionCode::Sotrm,
        MgCoordinateSystemProjectionCode::Alber,
        MgCoordinateSystemProjectionCode::Azede,
        MgCoordinateSystemProjectionCode::Azmea,
        MgCoordinateSystemProjectionCode::Azmed,
        MgCoordinateSystemProjectionCode::Cassini,
        MgCoordinateSystemProjectionCode::Edcnc,
        MgCoordinateSystemProjectionCode::GaussK,
        MgCoordinateSystemProjectionCode::Hom1uv,
        MgCoordinateSystemProjectionCode::Hom2uv,
        MgCoordinateSystemProjectionCode::Hom1xy,
        MgCoordinateSystemProjectionCode::Hom2xy,
        MgCoordinateSystemProjectionCode::Krovak,
        MgCoordinateSystemProjectionCode::Rskew,
        MgCoordinateSystemProjectionCode::Rskewc,
        MgCoordinateSystemProjectionCode::Rskewo,
        MgCoordinateSystemProjectionCode::Lm1sp,
        MgCoordinateSystemProjectionCode::Lm2sp,
        MgCoordinateSystemProjectionCode::Lmblg,
        MgCoordinateSystemProjectionCode::Lmbrtaf,
        MgCoordinateSystemProjectionCode::Lmtan,
        MgCoordinateSystemProjectionCode::Mstero,
        MgCoordinateSystemProjectionCode::Obqcyl,
        MgCoordinateSystemProjectionCode::Plycn,
        MgCoordinateSystemProjectionCode::Pstro,
        MgCoordinateSystemProjectionCode::Pstrosl,
        MgCoordinateSystemProjectionCode::Unknown
    };

    bool canDoPoles (false);
    INT32 index;

    INT32 prjCode = frameCRS->GetProjectionCode ();
    for (index = 0;polarCapable [index] != MgCoordinateSystemProjectionCode::Unknown;index += 1)
    {
        if (prjCode == polarCapable [index])
        {
            canDoPoles = true;
            break;
        }
    }
    return canDoPoles;
}
