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
#include "CoordSysMgrsZone.h"
#include "CoordSysMgrs.h"       //for CCoordinateSystemMgrs

using namespace CSLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static constants.
const CCoordinateSystemMgrs::CCoordinateSystemMgrsSeries CCoordinateSystemMgrs::MgrsSeriesNormal [6] =
{
	{	L"ABCDEFGH", L"ABCDEFGHJKLMNPQRSTUV" },
	{	L"JKLMNPQR", L"FGHJKLMNPQRSTUVABCDE" },
	{	L"STUVWXYZ", L"ABCDEFGHJKLMNPQRSTUV" },
	{	L"ABCDEFGH", L"FGHJKLMNPQRSTUVABCDE" },
	{	L"JKLMNPQR", L"ABCDEFGHJKLMNPQRSTUV" },
	{	L"STUVWXYZ", L"FGHJKLMNPQRSTUVABCDE" }
};
const CCoordinateSystemMgrs::CCoordinateSystemMgrsSeries CCoordinateSystemMgrs::MgrsSeriesBessel [6] =
{
	{	L"ABCDEFGH", L"LMNPQRSTUVABCDEFGHJK" },
	{	L"JKLMNPQR", L"RSTUVABCDEFGHJKLMNPQ" },
	{	L"STUVWXYZ", L"LMNPQRSTUVABCDEFGHJK" },
	{	L"ABCDEFGH", L"RSTUVABCDEFGHJKLMNPQ" },
	{	L"JKLMNPQR", L"LMNPQRSTUVABCDEFGHJK" },
	{	L"STUVWXYZ", L"RSTUVABCDEFGHJKLMNPQ" }
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
                       m_nLetteringScheme(nLetteringScheme),
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_bUseFrameDatum (false),
                       m_nLastError (0),
                       m_pCsTarget (),
                       m_pCsMgrs (NULL),
                       m_GridBoundary (),
                       m_ZoneCollection (),
                       m_GraticuleUtm (),
                       m_GraticuleUpsNorth (),
                       m_GraticuleUpsSouth ()
{
}
CCoordinateSystemMgrs::CCoordinateSystemMgrs(MgCoordinateSystem* pTargetCs,INT8 nLetteringScheme,
                                                                           bool bSetExceptionsOn)
                            :
                       m_nLetteringScheme(nLetteringScheme),
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_bUseFrameDatum (false),
                       m_nLastError (0),
                       m_pCsTarget (),
                       m_pCsMgrs (NULL),
                       m_GridBoundary (),
                       m_ZoneCollection (),
                       m_GraticuleUtm (),
                       m_GraticuleUpsNorth (),
                       m_GraticuleUpsSouth ()

{
    m_pCsTarget = SAFE_ADDREF (pTargetCs);
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
    char *pStr = Convert_Wide_To_Ascii(sEllipsoidCode.c_str()); //need to delete[] pStr
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
    char *pStr = Convert_Wide_To_Ascii(sDatumCode.c_str()); //need to delete[] pStr
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
        nLetteringScheme=1;
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
    //m_nLastError is already set if a failure occured and m_bExceptionsOn=false
    return sMgrs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STRING CCoordinateSystemMgrs::ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision)
{
    STRING sMgrs;
    ConvertFromLonLat(pLonLat, nPrecision, sMgrs);

    //if exception mode is on and excetion is thrown internally we exit anyway
    //so safe to return sMgrs here and not test the m_bExceptionsOn value
    //m_nLastError is already set if a failure occured and m_bExceptionsOn=false
    return sMgrs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinate* CCoordinateSystemMgrs::ConvertToLonLat(CREFSTRING sMgrs)
{
    double dLongitude, dLatitude;
    INT32 nResult=ConvertToLonLat(sMgrs, dLongitude, dLatitude);
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
                m_nLastError=m_nLastError=MgCoordinateSystemErrorCode::OutOfMemory;
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
    //m_nLastError is already set if a failure occured and m_bExceptionsOn=false
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
    double gridIncrement;

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

    if (m_GridBoundary == 0)
    {
        // Proceeding without a grid boundary will cause a crash.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemMgrs.GetGridLines",
                                                                   __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_TRY ()
        theGridLineCollection = new CCoordinateSystemGridLineCollection ();
        unitType = specification->GetUnitType();
        specIsGrid = (unitType ==  MgCoordinateSystemUnitType::Linear);
        if (specIsGrid)
        {
            // The specification calls for a grid.
            zoneCount = m_ZoneCollection->GetCount ();
            for (index = 0;index < zoneCount;index += 1)
            {
                mgrsZoneGrid = m_ZoneCollection->GetItem (index);
                aGridLineCollection = mgrsZoneGrid->GetGridLines (specification);
                theGridLineCollection->AddCollection (aGridLineCollection);
            }
        }
        else
        {
            // The specification calls for a graticule.
            if (m_GraticuleUtm)
            {
                aGridLineCollection = m_GraticuleUtm->GetGridLines (specification);
                theGridLineCollection->AddCollection (aGridLineCollection);
            }
            if (m_GraticuleUpsNorth)
            {
                aGridLineCollection = m_GraticuleUpsNorth->GetGridLines (specification);
                theGridLineCollection->AddCollection (aGridLineCollection);
            }
            if (m_GraticuleUpsSouth)
            {
                aGridLineCollection = m_GraticuleUpsSouth->GetGridLines (specification);
                theGridLineCollection->AddCollection (aGridLineCollection);
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
        theGridRegionCollection = new CCoordinateSystemGridRegionCollection ();
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
        theGridTickCollection = new CCoordinateSystemGridTickCollection ();
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

    int nResult=CScalcMgrsFromLl (m_pCsMgrs, szMgrs, (int)strlen(szMgrs), latLng, nPrecision);
    if (0==nResult)
    {
        //success, return the MGRS string
        wchar_t* pMgrs = Convert_Ascii_To_Wide(szMgrs);
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
            throw new MgNullArgumentException(L"MgCoordinateSystemMgrs.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
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
    if (!pLonLat)
    {
        if (m_bExceptionsOn)
        {
            throw new MgNullArgumentException(L"MgCoordinateSystemMgrs.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
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
    INT32 nResult=ConvertToLonLat(sMgrs, dLongitude, dLatitude);
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

    //Convert to a char*
    char *pMgrs = Convert_Wide_To_Ascii(sMgrs.c_str()); //need to delete[] pStr
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
    int nResult=CScalcLlFromMgrs (m_pCsMgrs, latLng, pMgrs);

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
    INT32 zoneNbr;
    INT32 zoneMin;
    INT32 zoneMax;
    INT32 tmpInt32;

    double cm;                      // central meridian
    double eastLimit, westLimit;    // limits of a UTM zone
    double eastMin, eastMax;        // frame boundary extrema in 'LL84' (or 'LL')
    double northMin, northMax;      // frame boundary extrema in 'LL84' (or 'LL')

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

        // Convert the polygon portion of the Grid Boundary object to long/lat
        // coordinates and extract the min/max from the resulting polygon.
        llCRS = csFactory->CreateFromCode (useFrameDatum ? L"LL" : L"LL84");
        toLlTransform = csFactory->GetTransform(frameCRS,llCRS);
        toFrameTransform = csFactory->GetTransform(llCRS,frameCRS);
        pPolygon = frameBoundary->GetBoundary (toLlTransform,1.0E-05);
        llBoundary = csFactory->GridBoundary (pPolygon);
        llBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);

        // Use the latitude min/max to extract the UTM/UPS zones as necessary.
        if (northMin < -80.0)
        {
            // There is a portion of the frame boundary in the south polar region.
            zoneNbr = -61;
            pSouthwest->SetX (eastMin);
            pSouthwest->SetY (northMin);
            pNortheast->SetX (eastMax);
            pNortheast->SetY (-80.0);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
            pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
            if (pPolygonIntersection != 0)
            {
                reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                zoneCollection->Add (mgrsZoneGrid);

                // Construct the m_GraticuleUpsSouth member, it may be needed.
                m_GraticuleUpsSouth = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS);
            }
        }
        if (northMax > 84.0)
        {
            // There is a portion of the frame boundary in the north polar region.
            zoneNbr = 61;
            pSouthwest->SetX (eastMin);
            pSouthwest->SetY (84.0);
            pNortheast->SetX (eastMax);
            pNortheast->SetY (northMax);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
            pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
            if (pPolygonIntersection != 0)
            {
                reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                zoneCollection->Add (mgrsZoneGrid);

                // Construct the m_GraticuleUpsNorth member, it may be needed.
                m_GraticuleUpsNorth = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS);
            }
        }
        if (northMax > -80.0 && northMin < 84.0)
        {
            // A portion of the frame boundary is in the region covered by the
            // normal (i.e. non-polar) UTM zones.  Determine the particular UTM
            // zones we need to generate.
            tmpInt32 = static_cast <INT32>(floor (eastMin));
            zoneMin = ((tmpInt32 + 180) / 6) + 1;
            tmpInt32 = static_cast <INT32>(ceil (eastMax));
            zoneMax = ((tmpInt32 + 180) / 6) + 1;

            if (northMax > 0.0)
            {
                // There are some northern zones.

                // We will need to generate, for each UTM zone, the extents
                // of that particular UTM zone trimmed to the extents of the
                // provided frame boundary.  The north/south portions
                // are the same for each zone, so we do that once here outside
                // the loop.
                pSouthwest->SetY ((northMin <  0.0) ?  0.0 : northMin);
                pNortheast->SetY ((northMax > 84.0) ? 84.0 : northMax);

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
                    if (westLimit < eastMin) westLimit = eastMin;
                    if (eastLimit > eastMax) eastLimit = eastMax;

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
                        mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                        zoneCollection->Add (mgrsZoneGrid);
                    }
                 }
            }
            if (northMin < 0.0)
            {
                // Pretty much the same as the northern zones processed
                // above, but without the laborious comments.
                pSouthwest->SetY ((northMin < -80.0) ? -80.0 : northMin);
                pNortheast->SetY ((northMax >   0.0) ?   0.0 : northMax);
                for (zoneNbr = zoneMin;zoneNbr <= zoneMax;zoneNbr += 1)
                {
                    cm = static_cast<double>((zoneNbr * 6) - 183);
                    westLimit = cm - 3.0;
                    eastLimit = cm + 3.0;
                    if (westLimit < eastMin) westLimit = eastMin;
                    if (eastLimit > eastMax) eastLimit = eastMax;
                    pSouthwest->SetX (westLimit);
                    pNortheast->SetX (eastLimit);

                    llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                    pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
                    Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
                    pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
                    if (pPolygonIntersection != 0)
                    {
                        reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                        mgrsZoneGrid = new CCoordinateSystemMgrsZone (reducedFrameBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                        zoneCollection->Add (mgrsZoneGrid);
                    }
                }
            }

            // Need a CCoordinateSystemOneGrid object which can produce the 6 x 8 graticule
            // for the entire regon.
            pSouthwest->SetX (eastMin);
            pNortheast->SetX (eastMax);
            pSouthwest->SetY ((northMin < -80.0) ? -80.0 : northMin);
            pNortheast->SetY ((northMax >  84.0) ?  84.0 : northMax);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (toFrameTransform,1.0);
            Ptr<MgPolygon> pPolygonTemp = frameBoundary->GetBoundary ();
            pPolygonIntersection = dynamic_cast<MgPolygon*>(pPolygon->Intersection (pPolygonTemp));
            if (pPolygonIntersection != 0)
            {
                reducedFrameBoundary = csFactory->GridBoundary (pPolygonIntersection);
                m_GraticuleUtm = new CCoordinateSystemOneGrid (reducedFrameBoundary,llCRS,frameCRS);
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

    // For now we assume there is no difference between the northern and southern hemisphere.
    if (utmZoneNbr != 0 && abs(utmZoneNbr) <= 60)
    {
        iEasting  = static_cast<INT32>(easting);
        iNorthing = static_cast<INT32>(northing);
        if (iEasting < 100000) iEasting = 100000;
        if (iEasting > 1000000) iEasting = 1000000;
        if (iNorthing < 0) iNorthing = 0;
        if (iNorthing > 10000000) iEasting = 10000000;
        iNorthing %= 2000000;

        eastIndex  = static_cast<INT32>(iEasting - 100000) / 100000;
        northIndex = static_cast<INT32>(iNorthing) / 100000;
        if (eastIndex < 9 && northIndex < 21)
        {
            seriesIndex = abs (utmZoneNbr) % 6 - 1;
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
        swprintf (wcBufr,64,L"UTM84-%dS",zoneNbr);
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
//End of file.
