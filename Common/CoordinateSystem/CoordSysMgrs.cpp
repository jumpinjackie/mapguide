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
                       m_ZoneCollection ()
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
                       m_ZoneCollection ()
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
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridLines")
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 CCoordinateSystemMgrs::GetSpecializationType ()
{
    return MgCoordinateSystemGridSpecializationType::MGRS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::SetBoundary(MgCoordinateSystemGridBoundary* pGridBoundary)
{
    m_GridBoundary = SAFE_ADDREF (pGridBoundary);
    m_ZoneCollection = FrameBoundaryToZones (m_GridBoundary,m_pCsTarget,m_bUseFrameDatum);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridBoundary* CCoordinateSystemMgrs::GetBoundary(void)
{
    return SAFE_ADDREF (m_GridBoundary.p);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridLineCollection* CCoordinateSystemMgrs::GetGridLines (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<CCoordinateSystemGridLineCollection> theGridLineCollection;

    MG_TRY ()
        theGridLineCollection = new CCoordinateSystemGridLineCollection ();
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            Ptr<MgCoordinateSystemGridLineCollection> aGridLineCollection;
            aGridLineCollection = mgrsZoneGrid->GetGridLines (specification);
            theGridLineCollection->AddCollection (aGridLineCollection);
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

    MG_TRY ()
        theGridRegionCollection = new CCoordinateSystemGridRegionCollection ();
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            Ptr<MgCoordinateSystemGridRegionCollection> aGridRegionCollection;
            aGridRegionCollection = mgrsZoneGrid->GetGridRegions (specification);
            theGridRegionCollection->AddCollection (aGridRegionCollection);
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridRegions")
    return static_cast<MgCoordinateSystemGridRegionCollection*>(theGridRegionCollection.Detach());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridTickCollection* CCoordinateSystemMgrs::GetGridTicks (MgCoordinateSystemGridSpecification* specification)
{
    INT32 index;
    INT32 zoneCount;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<CCoordinateSystemGridTickCollection> theGridTickCollection;

    MG_TRY ()
        theGridTickCollection = new CCoordinateSystemGridTickCollection ();
        zoneCount = m_ZoneCollection->GetCount ();
        for (index = 0;index < zoneCount;index += 1)
        {
            mgrsZoneGrid = m_ZoneCollection->GetItem (index);
            Ptr<MgCoordinateSystemGridTickCollection> aGridTickCollection;
            aGridTickCollection = mgrsZoneGrid->GetBoundaryTicks (specification);
            theGridTickCollection->AddCollection (aGridTickCollection);
            // TODO:  Need to remove from the collection any grid ticks which are
            // internal to the m_GridBoundary object of this object. 
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrs::GetGridRTicks")
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
// Returns a collection of CCoordinateSYstemMgrsZone objects.
// These objects derive from OneGrid, so they have both coordinate systems in them.
// 
CCoordinateSystemMgrsZoneCollection* CCoordinateSystemMgrs::FrameBoundaryToZones (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                  MgCoordinateSystem* frameCRS,
                                                                                  bool useFrameDatum)
{
    INT32 zoneNbr;
    INT32 zoneMin;
    INT32 zoneMax;

    double cm;                      // central meridian;
    double eastLimit, westLimit;    // limits of a UTM zone
    double eastMin, eastMax;        // frame boundary extrema
    double northMin, northMax;      // frame boundary extrema

    Ptr<MgPolygon> pPolygon;
    Ptr<MgCoordinate> pSouthwest;
    Ptr<MgCoordinate> pNortheast;
    Ptr<MgCoordinateSystem> llCS;
    Ptr<MgCoordinateSystem> utmCRS;
    Ptr<MgCoordinateSystemTransform> llTransform;
    Ptr<MgCoordinateSystemTransform> utmTransform;
    Ptr<MgCoordinateSystemGridBoundary> utmBoundary;
    Ptr<MgCoordinateSystemGridBoundary> llBoundary;
    Ptr<CCoordinateSystemMgrsZone> mgrsZoneGrid;
    Ptr<CCoordinateSystemMgrsZoneCollection> zoneCollection;
    STRING utmCsCode;

    MG_TRY ()
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory ();
        zoneCollection = new CCoordinateSystemMgrsZoneCollection ();
        pSouthwest = new MgCoordinateXY ();
        pNortheast = new MgCoordinateXY ();

        // Convert the polygon portion of the Grid Boundary object to long/lat
        // coordinates and extract the min/max from the resulting polygon.
        llCS = csFactory->CreateFromCode (L"LL");
        llTransform = csFactory->GetTransform(frameCRS,llCS);
        pPolygon = frameBoundary->GetBoundary (llTransform,1.0E-05);
        llBoundary = csFactory->GridBoundary (pPolygon);
        llBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);

        // Use the latitude min/max to extract the UTM/UPS zones as necessary.
        if (northMin < -80.0)
        {
            // There is a portion of the frame boundary in the south polar region.
            zoneNbr = -61;
            utmCsCode = ZoneNbrToUtmCs (zoneNbr);
            utmCRS = csFactory->CreateFromCode (utmCsCode);
            utmTransform = csFactory->GetTransform (llCS,utmCRS);
            pSouthwest->SetX (eastMin);
            pSouthwest->SetY (northMin);
            pNortheast->SetX (eastMax);
            pNortheast->SetY (-80.0);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (utmTransform,1.0);
            utmBoundary = csFactory->GridBoundary (pPolygon);
            mgrsZoneGrid = new CCoordinateSystemMgrsZone (utmBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
            zoneCollection->Add (mgrsZoneGrid);
        }
        if (northMax > 84.0)
        {
            // There is a portion of the frame boundary in the south polar region.
            zoneNbr = 61;
            utmCsCode = ZoneNbrToUtmCs (zoneNbr);
            utmCRS = csFactory->CreateFromCode (utmCsCode);
            utmTransform = csFactory->GetTransform (llCS,utmCRS);
            pSouthwest->SetX (eastMin);
            pSouthwest->SetY (84.0);
            pNortheast->SetX (eastMax);
            pNortheast->SetY (northMax);
            llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
            pPolygon = llBoundary->GetBoundary (utmTransform,1.0);
            utmBoundary = csFactory->GridBoundary (pPolygon);
            mgrsZoneGrid = new CCoordinateSystemMgrsZone (utmBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
            zoneCollection->Add (mgrsZoneGrid);
        }
        if (northMax > -80.0 && northMin < 84.0)
        {
            // There is a portion of the frame boundary in the non-polar region
            // covered by normal UTM zones.  These can be either northern zones,
            // or southern zones; perhaps both.  We'll do the northern zones
            // first.
            if (northMax > 0.0)
            {
                // There are some northern zones.  The northMin and northMax
                // are the same for all of these.  What changes is the central
                // meridian which means the eastern and western limits change.
                zoneMin = ((static_cast<INT32>(eastMin) + 180) / 6) + 1;
                zoneMax = ((static_cast<INT32>(eastMax) + 180) / 6) + 1;

                pSouthwest->SetY ((northMin <=  0.0) ?  0.0 : northMin);
                pNortheast->SetY ((northMax >= 84.0) ? 84.0 : northMax);
                for (zoneNbr = zoneMin;zoneNbr <= zoneMax;zoneNbr += 1)
                {
                    // Here once for each normal UTM zone in the northern
                    // hemisphere which intersects the provided frame grid
                    // boundary.
                    utmCsCode = ZoneNbrToUtmCs (zoneNbr);
                    utmCRS = csFactory->CreateFromCode (utmCsCode);
                    utmTransform = csFactory->GetTransform (llCS,utmCRS);
                    cm = static_cast<double>((zoneNbr * 6) - 183);
                    westLimit = cm - 3.0;
                    if (westLimit < eastMin) westLimit = eastMin;
                    eastLimit = cm + 3.0;
                    if (eastLimit > eastMax) eastLimit = eastMax;
                    pSouthwest->SetX (westLimit);
                    pNortheast->SetX (eastLimit);
                    llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                    pPolygon = llBoundary->GetBoundary (utmTransform,1.0);
                    utmBoundary = csFactory->GridBoundary (pPolygon);
                    mgrsZoneGrid = new CCoordinateSystemMgrsZone (utmBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                    zoneCollection->Add (mgrsZoneGrid);
                }
            }
            if (northMin < 0.0)
            {
                // There are some southern zones.  The northMin and northMax
                // are the same for all of these.  What changes is the central
                // meridian which means the eastern and western limits change.
                // There are some northern zones.  The northMin and northMax
                // are the same for all of these.  What changes is the central
                // meridian which means the eastern and western limits change.
                zoneMin = ((static_cast<INT32>(eastMin) + 180) / 6) + 1;
                zoneMax = ((static_cast<INT32>(eastMax) + 180) / 6) + 1;

                pSouthwest->SetY ((northMin < -80.0) ? -80.0 : northMin);
                pNortheast->SetY ((northMax >   0.0) ?   0.0 : northMax);
                for (zoneNbr = zoneMin;zoneNbr <= zoneMax;zoneNbr += 1)
                {
                    // Here once for each normal UTM zone in the southern
                    // hemisphere which intersects the provided frame grid
                    // boundary.
                    utmCsCode = ZoneNbrToUtmCs (-zoneNbr);
                    utmCRS = csFactory->CreateFromCode (utmCsCode);
                    utmTransform = csFactory->GetTransform (llCS,utmCRS);
                    cm = static_cast<double>((zoneNbr * 6) - 183);
                    westLimit = cm - 3.0;
                    if (westLimit < eastMin) westLimit = eastMin;
                    eastLimit = cm + 3.0;
                    if (eastLimit > eastMax) eastLimit = eastMax;
                    pSouthwest->SetX (westLimit);
                    pNortheast->SetX (eastLimit);
                    llBoundary = csFactory->GridBoundary (pSouthwest,pNortheast);
                    pPolygon = llBoundary->GetBoundary (utmTransform,1.0);
                    utmBoundary = csFactory->GridBoundary (pPolygon);
                    mgrsZoneGrid = new CCoordinateSystemMgrsZone (utmBoundary,zoneNbr,useFrameDatum,frameCRS,m_nLetteringScheme);
                    zoneCollection->Add (mgrsZoneGrid);
                }
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridLines")
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
        utmCode = L"UPS-N";
    }
    else if (zoneNbr == -61)
    {
        utmCode = L"UPS-S";
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
        index = (static_cast<INT32>(latitude) % 8) + 12;
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
