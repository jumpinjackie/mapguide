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
#include "CoordSysMgrs.h"       //for CCoordinateSystemMgrs

using namespace CSLibrary;

// Grid specifications for commonly used specialized grids for convenience.
//static const MgCoordinateSystemGridSpecification MgrsUtm     = { 0.0,  0.0,      6.0,       8.0,  6,  8, MgCoordinateSystemUnitType::Angular, MgCoordinateSystemUnitCode::Degree, 0.0};
//static const MgCoordinateSystemGridSpecification MgrsUps     = { 0.0,  0.0,      4.0,       4.0,  4,  4, MgCoordinateSystemUnitType::Angular, MgCoordinateSystemUnitCode::Degree, 0.0};
//static const MgCoordinateSystemGridSpecification Mgrs100Km   = { 0.0,  0.0, 100000.0,  100000.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};
//static const MgCoordinateSystemGridSpecification Mgrs10Km    = { 0.0,  0.0,  10000.0,   10000.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};
//static const MgCoordinateSystemGridSpecification Mgrs1Km     = { 0.0,  0.0,   1000.0,    1000.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};
//static const MgCoordinateSystemGridSpecification Mgrs100m    = { 0.0,  0.0,    100.0,     100.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};
//static const MgCoordinateSystemGridSpecification Mgrs10m     = { 0.0,  0.0,     10.0,      10.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};
//static const MgCoordinateSystemGridSpecification Mgrs1m      = { 0.0,  0.0,      1.0,       1.0, 10, 10, MgCoordinateSystemUnitType::Linear,  MgCoordinateSystemUnitCode::Meter,  0.0};

// USNG and MGRS are almost indistinguishable, certainly at this level.  The following
// is perhaps overkill, but we implement here anyway as it doesn't really cost anything.
#define UsngUtm   MgrsUtm
#define UsngUps   MgrsUps
#define Usng100Km Mgrs100Km
#define Usng10Km  Mgrs10Km
#define Usng1Km   Mgrs1Km
#define Usng100m  Mgrs100m
#define Usng10m   Mgrs10m
#define Usng1m    Mgrs1m

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMgrs::CCoordinateSystemMgrs(INT8 nLetteringScheme, bool bSetExceptionsOn)
                            :
                       m_nLetteringScheme(nLetteringScheme),
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_nLastError (0),
                       m_pCsTarget (),
                       m_pCsMgrs (NULL),
                       m_GridBoundary ()
{
}
CCoordinateSystemMgrs::CCoordinateSystemMgrs(MgCoordinateSystem* pTargetCs,INT8 nLetteringScheme, bool bSetExceptionsOn)
                            :
                       m_nLetteringScheme(nLetteringScheme),
                       m_bExceptionsOn (bSetExceptionsOn),
                       m_nLastError (0),
                       m_pCsTarget (),
                       m_pCsMgrs (NULL),
                       m_GridBoundary ()
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
STRING CCoordinateSystemMgrs::ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision)
{
    STRING sMgrs;
    ConvertFromLonLat(dLongitude, dLatitude, nPrecision, sMgrs);

    //if exception mode is on and excetion is thrown internally we exit anyway
    //so safe to return sMgrs here and not test the m_bExceptionsOn value
    //m_nLastError is already set if a failure occured and m_bExceptionsOn=false
    return sMgrs;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STRING CCoordinateSystemMgrs::ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision)
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
INT32 CCoordinateSystemMgrs::GetSpecializationType ()
{
    return MgCoordinateSystemGridSpecializationType::MGRS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemMgrs::SetBoundary(MgCoordinateSystemGridBoundary* pGridBoundary)
{
    PtrAssign (&m_GridBoundary,pGridBoundary);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridBoundary* CCoordinateSystemMgrs::GetBoundary(void)
{
    return SAFE_ADDREF (m_GridBoundary.p);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridLineCollection* CCoordinateSystemMgrs::GetGridLines (MgCoordinateSystemGridSpecification* specification)
{
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridRegionCollection* CCoordinateSystemMgrs::GetGridRegions (MgCoordinateSystemGridSpecification* specification)
{
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MgCoordinateSystemGridTickCollection* CCoordinateSystemMgrs::GetGridTicks (MgCoordinateSystemGridSpecification* specification)
{
    return NULL;
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
INT32 CCoordinateSystemMgrs::ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision, REFSTRING sMgrs)
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
INT32 CCoordinateSystemMgrs::ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision, REFSTRING sMgrs)
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//from MgDisposable
void CCoordinateSystemMgrs::Dispose()
{
    delete this;
}

//End of file.
