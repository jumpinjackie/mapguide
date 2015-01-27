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
#include "CoordSysUtil.h"      //for Convert_Wide_To_UTF8
#include "CriticalSection.h"

#include "CoordSys.h"          //for CCoordinateSystem
#include "CoordSysMeasure.h"   //for CCoordinateSystemMeasure
#include "CoordSysEnumInteger32.h" //for CCoordinateSystemEnumInteger32
#include "MentorUtil.h"        //for ProjectionFromString

#ifdef _WIN32
#pragma message( "******************************************************************" )
#pragma message( "* Using CSMAP Dictionary Path Environment variable(If found): " MENTOR_DICTIONARY_PATH)
#pragma message( "******************************************************************" )
#else
#pragma message( "******************************************************************" )
#pragma message( "* Using CSMAP Dictionary Path: " MENTOR_DICTIONARY_PATH)
#pragma message( "******************************************************************" )
#endif

#ifndef _WIN32
#include <dlfcn.h>
#endif

//Mentor global variables used
extern "C"
{
    extern double cs_Km360;
    extern double cs_One;               /* 1.0                */
    extern double cs_Two_pi;            /* 2 pi */
}

using namespace CSLibrary;

// Coordinate system information properties
const STRING CCoordinateSystem::CoordinateSystemCode                  = L"Code";                    // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemDescription           = L"Description";             // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemProjection            = L"Projection";              // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemProjectionDescription = L"Projection Description";  // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemDatum                 = L"Datum";                   // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemDatumDescription      = L"Datum Description";       // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemEllipsoid             = L"Ellipsoid";               // NOXLATE
const STRING CCoordinateSystem::CoordinateSystemEllipsoidDescription  = L"Ellipsoid Description";   // NOXLATE

const STRING CCoordinateSystem::BaseLibrary = L"Mentor Coordinate System Library";                  // NOXLATE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystem::CCoordinateSystem(MgCoordinateSystemCatalog* pCatalog)
: m_bEncrypted(true)
{
    memset(&m_csprm, 0, sizeof(m_csprm));
    SetProjectionDefaults();
    SetCatalog(pCatalog);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystem::~CCoordinateSystem()
{
    Uninitialize();
}

///////////////////////////////////////////////////////////////////////////////
bool CCoordinateSystem::InitArbitrary(const cs_Csprm_ &csprm)
{
    //if an arbitrary coordinate system it has to DT and no EL
    //it uses the NERTH projection
    if (ProjectionIsNerthType (csprm.csdef.prj_knm))
    {
        //Everything worked okay!  Copy the data into our member variables.
        Uninitialize();
        m_csprm = csprm;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void CCoordinateSystem::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

///////////////////////////////////////////////////////////////////////////////
//If m_pCatalog is NULL this method initializes only the CS portion of the definition
//without loading the DT from the dictionary
//The DT can then be set via CCoordinateSystem::SetDatumDefinition
//
void CCoordinateSystem::InitFromCatalog(const cs_Csdef_& csdef,
                                        const map<STRING, Ptr<MgDisposable> >* const ellipsoidMap,
                                        const map<STRING, Ptr<MgDisposable> >* const datumMap)
{
    MG_TRY()

    //if an arbitrary coordinate system it has to DT and no EL
    //it uses the NERTH projection
    if (ProjectionIsNerthType (csdef.prj_knm))
    {
        //Everything worked okay!  Copy the data into our member variables.
        Uninitialize();

        bool bResult=BuildCsprmFromArbitraryDef(csdef, m_csprm);
        assert(bResult);
        if (!bResult)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        return;
    }

    m_csprm.csdef=csdef;

    //no catalog, we're done with the CS part of initialization
    if (!m_pCatalog)
    {
        return;
    }

    //Find out whether the coordinate system is geodetic
    bool bGeodetic=CsdefIsGeodetic(csdef);
    if (bGeodetic)
    {
        wchar_t* pwszDtName = Convert_UTF8_To_Wide(csdef.dat_knm);
        STRING sDtName(::ToLower(pwszDtName));

        delete[] pwszDtName;
        pwszDtName = NULL;

        Ptr<MgGuardDisposable> pDatum;

        //It's geodetic.  We need a datum definition.
        if (NULL != datumMap)
        {
            map<STRING, Ptr<MgDisposable> >::const_iterator datumMapIterator = datumMap->find(sDtName);
            if (datumMapIterator != datumMap->end())
            {
                Ptr<MgDisposable> disposableCsObject = datumMapIterator->second;
                MgCoordinateSystemDatum* datumInfo = dynamic_cast<MgCoordinateSystemDatum*>(disposableCsObject.p);
                if (NULL == datumInfo)
                    throw new MgInvalidArgumentException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
                
                pDatum = SAFE_ADDREF(datumInfo);
            }
        }

        if (NULL == pDatum)
        {
            Ptr<MgCoordinateSystemDatumDictionary> pDtDict = m_pCatalog->GetDatumDictionary();
            if (!pDtDict)
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumDictionaryException", NULL);
            }

            pDatum = pDtDict->Get(sDtName);
            assert(pDatum);
            if (!pDatum)
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumInDictionaryException", NULL);
            }
        }

        //We've got a datum object.  Complete the initialization.
        return SetDatumDefinition(dynamic_cast<MgCoordinateSystemDatum*>(pDatum.p));
    }   //if it's geodetic
    else
    {
        wchar_t* pwszElName = Convert_UTF8_To_Wide(csdef.elp_knm);
        STRING sElName(::ToLower(pwszElName));
        delete[] pwszElName;
        pwszElName = NULL;

        Ptr<MgGuardDisposable> pEllipsoid;

        if (NULL != ellipsoidMap)
        {
            map<STRING, Ptr<MgDisposable> >::const_iterator ellipsoidMapIterator = ellipsoidMap->find(sElName);
            if (ellipsoidMapIterator != ellipsoidMap->end())
            {
                Ptr<MgDisposable> disposableCsObject = ellipsoidMapIterator->second;
                MgCoordinateSystemEllipsoid* ellipsoidInfo = dynamic_cast<MgCoordinateSystemEllipsoid*>(disposableCsObject.p);
                if (NULL == ellipsoidInfo)
                    throw new MgInvalidArgumentException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
                
                pEllipsoid = SAFE_ADDREF(ellipsoidInfo);
            }
        }

        if (NULL == pEllipsoid)
        {
            //It's cartographic.  We need an ellipsoid definition.
            Ptr<MgCoordinateSystemEllipsoidDictionary> pElDict = m_pCatalog->GetEllipsoidDictionary();
            if (!pElDict)
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoEllipsoidDictionaryException", NULL);
            }

            pEllipsoid = pElDict->Get(sElName);
            assert(pEllipsoid);
            if (!pEllipsoid)
            {
                throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoEllipsoidInDictionaryException", NULL);
            }
        }

        //We've got the ellipsoid definition.  Complete the initialization.
        return SetEllipsoidDefinition(dynamic_cast<MgCoordinateSystemEllipsoid*>(pEllipsoid.p));
    }   //if it's cartographic

    MG_CATCH(L"MgCoordinateSystem.InitFromCatalog")
    if (mgException != NULL)
    {
        Uninitialize();
    }
    MG_THROW()
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The MgCoordinateSystem class supplies operations for working with
/// coordinates in that coordinate system, including the ability to convert
/// coordinates to and from longitude latitude values, work with linear units,
/// and compute distances.
///</summary>
INT32 CCoordinateSystem::GetType()
{
    INT32 nCoordinateSystemType=MgCoordinateSystemType::Unknown;

    MG_TRY()
    if (0==m_csprm.prj_code)
    {
        nCoordinateSystemType=MgCoordinateSystemType::Unknown;
    }
    //arbitrary systems, the one that use the NERTH projection
    else if(ProjectionIsNerthType (m_csprm.csdef.prj_knm))
    {
        nCoordinateSystemType = MgCoordinateSystemType::Arbitrary;
    }
    //Lat/Long coordinate systems
    else if(m_csprm.prj_code == cs_PRJCOD_UNITY)
    {
        nCoordinateSystemType = MgCoordinateSystemType::Geographic;
    }
    else
    {
        nCoordinateSystemType = MgCoordinateSystemType::Projected;
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetType")

    return nCoordinateSystemType;
}

//-----------------------------------------------------------------------------
MgCoordinate* CCoordinateSystem::ConvertFromLonLat(MgCoordinate* lonLat)
{
    MgCoordinate* pCoordinate = NULL;

    MG_TRY()
    if(lonLat->GetDimension() == MgCoordinateDimension::XY)
    {
        pCoordinate=ConvertFromLonLat(lonLat->GetX(), lonLat->GetY());
    }
    else if(lonLat->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        double x=0.,y=0.;
        ConvertPoint(CS_llchk, CS_ll2cs, lonLat->GetX(), lonLat->GetY(), 0., &x, &y, NULL);
        // 2D transform + measure which is ignored
        pCoordinate = new MgCoordinateXYM(x, y, lonLat->GetM());
        if (!pCoordinate)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else if(lonLat->GetDimension() == MgCoordinateDimension::XYZ)
    {
        pCoordinate=ConvertFromLonLat(lonLat->GetX(), lonLat->GetY(), lonLat->GetZ());
    }
    else if(lonLat->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        double x=0.,y=0.,z=0.;
        ConvertPoint(CS_llchk, CS_ll3cs, lonLat->GetX(), lonLat->GetY(), lonLat->GetZ(), &x, &y, &z);
        // 3D transform + measure which is ignored
        pCoordinate = new MgCoordinateXYZM(x, y, z, lonLat->GetM());
        if (!pCoordinate)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")

    return pCoordinate;
}

//-----------------------------------------------------------------------------
MgCoordinate* CCoordinateSystem::ConvertToLonLat(MgCoordinate* coordinate)
{
    MgCoordinate* pCoordinate = NULL;

    MG_TRY()
    if(coordinate->GetDimension() == MgCoordinateDimension::XY)
    {
        pCoordinate=ConvertToLonLat(coordinate->GetX(), coordinate->GetY());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        double dLongitude=0., dLatitude=0.;
        ConvertPoint(CS_xychk, CS_cs2ll, coordinate->GetX(), coordinate->GetY(), 0., &dLongitude, &dLatitude, NULL);
        // 2D transform + measure which is ignored
        pCoordinate = new MgCoordinateXYM(dLongitude, dLatitude, coordinate->GetM());
        if (!pCoordinate)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else if(coordinate->GetDimension() == MgCoordinateDimension::XYZ)
    {
        pCoordinate = ConvertToLonLat(coordinate->GetX(), coordinate->GetY(), coordinate->GetZ());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        double dLongitude=0., dLatitude=0., dZDest=0.;
        ConvertPoint(CS_xychk, CS_cs3ll, coordinate->GetX(), coordinate->GetY(), coordinate->GetZ(), &dLongitude, &dLatitude, &dZDest);
        // 3D transform + measure which is ignored
        pCoordinate = new MgCoordinateXYZM(dLongitude, dLatitude, dZDest, coordinate->GetM());
        if (!pCoordinate)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertToLonLat")

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified value in coordinate system units to
/// meters.
///</summary>
///<param name="units">
/// The value in coordinate system units to convert.
///</param>
///<returns>
/// The value in meters.
///</returns>
double CCoordinateSystem::ConvertCoordinateSystemUnitsToMeters(double units)
{
    double meters = 0.0;

    MG_TRY()
    meters = units * m_csprm.csdef.unit_scl;
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertCoordinateSystemUnitsToMeters")

    return meters;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the value in meters to coordinate system units.
///</summary>
///<param name="meters">
/// The value in meters to convert.
///</param>
///<returns>
/// The value in coordinate system units.
///</returns>
double CCoordinateSystem::ConvertMetersToCoordinateSystemUnits(double meters)
{
    double units = 0.0;

    MG_TRY()
    // This may throw a divide by zero exception
    units = meters / m_csprm.csdef.unit_scl;
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertMetersToCoordinateSystemUnits")

    return units;
}

///////////////////////////////////////////////////////////////////////////
double CCoordinateSystem::MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    CHECKARGUMENTNULL(coord1, L"MgCoordinateSystem.MeasureEuclideanDistance");
    CHECKARGUMENTNULL(coord2, L"MgCoordinateSystem.MeasureEuclideanDistance");

    return MeasureEuclideanDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Measures the euclidian distance between two coordinates.
///</summary>
///<param name="x1">
/// The x value that defines the starting coordinate.
///</param>
///<param name="y1">
/// The y value that defines the starting coordinate.
///</param>
///<param name="x2">
/// The x value that defines the ending coordinate.
///</param>
///<param name="y2">
/// The y value that defines the ending coordinate.
///</param>
///<returns>
/// The euclidean distance from coord1 to coord2 in coordinate system units.
///</returns>
double CCoordinateSystem::MeasureEuclideanDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    MG_TRY()
    distance = ::sqrt(::pow(x1 - x2, 2.0) + ::pow(y1 - y2, 2.0));
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.MeasureEuclideanDistance")

    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Measures the distance between two coordinates.
/// This function uses the coordinate systems ellipsoid
/// definition and a great circle algorithm to produce the result.
///</summary>
///<param name="coord1">
/// An MgCoordinate that defines the starting coordinate.
///</param>
///<param name="coord2">
/// An MgCoordinate that defines the ending coordinate.
///</param>
///<returns>
/// The great circle distance from coord1 to coord2 in coordinate system units.
///</returns>
double CCoordinateSystem::MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    CHECKARGUMENTNULL(coord1, L"MgCoordinateSystem.MeasureGreatCircleDistance");
    CHECKARGUMENTNULL(coord2, L"MgCoordinateSystem.MeasureGreatCircleDistance");

    return MeasureGreatCircleDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Measures the distance between two coordinates.
/// This function uses the coordinate systems ellipsoid
/// definition and a great circle algorithm to produce the result.
///</summary>
///<param name="double x1">
/// The x value that defines the first coordinate.
///</param>
///<param name="double y1">
/// The y value that defines the first coordinate.
///</param>
///<param name="double x2">
/// The x value that defines the second coordinate.
///</param>
///<param name="double y2">
/// The y value that defines the second coordinate.
///</param>
///<returns>
/// The great circle distance from coord1 to coord2 in coordinate system units.
///</returns>
double CCoordinateSystem::MeasureGreatCircleDistance(double x1, double y1, double x2, double y2)
{
    double dDistance = 0.0;

    MG_TRY()
    // This coordinate system must not be an arbitrary XY coordinate system,
    // and there must be a Great Circle path between the points.
    if(GetType() == MgCoordinateSystemType::Arbitrary)
    {
        throw new MgInvalidCoordinateSystemTypeException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMustNotBeArbitrary", NULL);
    }

    Ptr<MgCoordinate> pLonLat1=ConvertToLonLat(x1, y1);
    Ptr<MgCoordinate> pLonLat2=ConvertToLonLat(x2, y2);
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoid=GetEllipsoidDefinition();
    dDistance=pEllipsoid->GetGreatCircleDistance(pLonLat1->GetX(), pLonLat1->GetY(), pLonLat2->GetX(), pLonLat2->GetY());

    // Convert the distance into coordinate system units
    double unitScale = GetUnitScale();
    if(unitScale > 0)
    {
        dDistance = dDistance / unitScale;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.MeasureGreatCircleDistance")

    return dDistance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="coord1">
/// An MgCoordinate that specifies the first coordinate.
///</param>
///<param name="coord2">
/// An MgCoordinate that specifies the second coordinate.
///</param>
///<returns>
/// The azimuth (Angle with respect to the North) of the vector formed by
/// coord1 and coord2.
///</returns>
double CCoordinateSystem::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    CHECKARGUMENTNULL(coord1, L"MgCoordinateSystem.GetAzimuth");
    CHECKARGUMENTNULL(coord2, L"MgCoordinateSystem.GetAzimuth");

    return GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="double x1">
/// The x value that defines the first coordinate.
///</param>
///<param name="double y1">
/// The y value that defines the first coordinate.
///</param>
///<param name="double x2">
/// The x value that defines the second coordinate.
///</param>
///<param name="double y2">
/// The y value that defines the second coordinate.
///</param>
///<returns>
/// The azimuth (Angle with respect to the North) of the vector formed by
/// the 2 specified coordinates.
///</returns>
double CCoordinateSystem::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double dAzimuth = 0.0;

    MG_TRY()

    if(GetType() == MgCoordinateSystemType::Arbitrary)
    {
        double xDistance = x2 - x1;
        double yDistance = y2 - y1;

        // Convert azimuth to radians
        dAzimuth = ::atan(::fabs(yDistance/xDistance)) * RAD_TO_DEG;

        if(xDistance < 0.0)
        {
            dAzimuth = 180.0 - dAzimuth;
        }

        if(yDistance < 0.0)
        {
            dAzimuth *= -1.0;
        }
    }
    else
    {
        Ptr<MgCoordinate> pLonLat1=ConvertToLonLat(x1, y1);
        Ptr<MgCoordinate> pLonLat2=ConvertToLonLat(x2, y2);

        Ptr<MgCoordinateSystemEllipsoid> pEllipsoid=GetEllipsoidDefinition();
        dAzimuth=pEllipsoid->GetGreatCircleAzimuth(pLonLat1->GetX(), pLonLat1->GetY(), pLonLat2->GetX(), pLonLat2->GetY());
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetAzimuth")

    return dAzimuth;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes a coordinate given the distance along a vector that is defined
/// by a starting coordinate and an azimuth (Angle with respect to the
/// North).
///</summary>
///<param name="coord">
/// An MgCoordinate that represents the start of the vector.
///</param>
///<param name="azimuth">
/// An azimuth (Angle with respect to the North) that defines the direction
/// of the vector.
///</param>
///<param name="distance">
/// The distance along the vector of the desired coordinate in coordinate system units.
///</param>
///<returns>
/// An MgCoordinate that lies the given distance along the vector.
///</returns>
MgCoordinate* CCoordinateSystem::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    CHECKARGUMENTNULL(coord, L"MgCoordinateSystem.GetCoordinate");

    return GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes a coordinate a given distance along a vector that is defined
/// by a starting coordinate and an azimuth (Angle with respect to the
/// North).
///</summary>
///<param name="xStart">
/// The x value that represents the start of the vector.
///</param>
///<param name="yStart">
/// The y value that represents the start of the vector.
///</param>
///<param name="azimuth">
/// An azimuth (Angle with respect to the North) that defines the direction
/// of the vector.
///</param>
///<param name="distance">
/// The distance along the vector of the desired coordinate in coordinate system units.
///</param>
///<returns>
/// An MgCoordinate that lies the given distance along the vector.
///</returns>
MgCoordinate* CCoordinateSystem::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    MgCoordinate* pCoordinate = NULL;

    MG_TRY()

    double x = 0.0;
    double y = 0.0;

    if(GetType() == MgCoordinateSystemType::Arbitrary)
    {
        // Convert azimuth to radians
        double azimuthRads = azimuth * DEG_TO_RAD;

        x = xStart + ::cos(azimuthRads) * distance;
        y = yStart + ::sin(azimuthRads) * distance;
    }
    else
    {
        double dLongitude, dLatitude;
        ConvertPoint(CS_xychk, CS_cs2ll, xStart, yStart, 0., &dLongitude, &dLatitude, NULL);

        double lonLatPts[2][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
        lonLatPts[0][0] = dLongitude;
        lonLatPts[0][1] = dLatitude;

        // compute the eccentricty squared (see Snyder: Map Projections, a working
        // manual, p. 13)

        double eccenSqrd = 1.0 - ::pow(m_csprm.datum.p_rad / m_csprm.datum.e_rad, 2.0);

        // Convert distance from coordinate system units to meters
        distance = distance * m_csprm.csdef.unit_scl;

        // finally, compute and return the coordinate
        //no dictionary access here, no critical section needed
        ::CS_azddll(m_csprm.datum.e_rad, eccenSqrd, lonLatPts[0], azimuth, distance, lonLatPts[1]);

        ConvertPoint(CS_llchk, CS_ll2cs, lonLatPts[1][0], lonLatPts[1][1], 0., &x, &y, NULL);
    }

    pCoordinate = new MgCoordinateXY(x, y);

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetCoordinate")

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
//When a coordinate system is invalid, the internal Mentor function
//CS_cschk() provides a list of numeric error codes listing all the things
//that are wrong with it.  The MgCoordinateSystem::GetErrors() function provides
//access to that information.  If MgCoordinateSystem::IsValid() returns false,
//then this function can be called to provide an enumerator of all the Mentor
//error codes that resulted from it.  (If there are no errors-- i.e., the
//coordinate system definition is valid-- then the returned pointer will be
//set to NULL.)  The caller is responsible for freeing the returned
//enumerator via Release() when done with it.  The numeric values will
//consist of the various cs_CSQ_XXX constants that are #define'd in cs_map.h.
// Consult Mentor documentation for details.
//
//MENTOR_MAINTENANCE
//Also refer to the values defined in the class MgCoordinateSystemErrorCode
//to know the values stored in that enum. They are based on the Mentor values
//described above
//
MgCoordinateSystemEnumInteger32* CCoordinateSystem::GetErrors()
{
    Ptr<CCoordinateSystemEnumInteger32> pNewEnum;
    INT32 *pErrsInt32 = NULL;
    int *pErrs = NULL;

    MG_TRY()

    //Check the definition, see how many errors we get.
    CriticalClass.Enter();
    int nNumErrs = CS_cschk(&m_csprm.csdef, 0, NULL, 0);
    CriticalClass.Leave();
    if (0 == nNumErrs)
    {
        //Nothing wrong with the definition!
        return NULL;
    }
    assert(nNumErrs > 0);

    //Make an array of integers to hold the error codes.
    pErrs = new int[nNumErrs];
    if (NULL == pErrs)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetErrors", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    pErrsInt32 = new INT32[nNumErrs];
    if (NULL == pErrsInt32)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetErrors", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get the errors
    CriticalClass.Enter();
    int nNumErrsAgain = CS_cschk(&m_csprm.csdef, 0, pErrs, nNumErrs);
    CriticalClass.Leave();
    assert(nNumErrsAgain == nNumErrs);

    //Make a dword enumerator to hold the results
    pNewEnum = new CCoordinateSystemEnumInteger32;

    if (NULL == pNewEnum.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetErrors", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy in the results.  Mentor wants a list of ints, MgCoordinateSystemEnumInteger32
    //wants a list of UNIT32, so I'll just cast it from one to
    //the other.  I can get away with this because a DWORD and an
    //int are the same size (on a 32-bit system, anyway), and none
    //of Mentor's return values are negative numbers.
    for (int nErr=0; nErr < nNumErrs; nErr++)
    {
        assert(pErrs[nErr] >= 0);
        pErrsInt32[nErr]=pErrs[nErr];
    }

    pNewEnum->SetList(pErrsInt32, static_cast<UINT32>(nNumErrs));

    MG_CATCH(L"MgCoordinateSystem.GetErrors")

    delete [] pErrsInt32;
    delete [] pErrs;

    MG_THROW()

    return pNewEnum.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns the OGC WKT string representation of the coordinate system.
///</summary>
///<returns>
/// OGC WKT string.
///</returns>
STRING CCoordinateSystem::ToString()
{
    STRING sWkt;

    MG_TRY()
    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.ToString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    if (!pConverter)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.ToString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sWkt=pConverter->DefinitionToWkt(this, MgCoordinateSystemWktFlavor::Ogc);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ToString")

    return sWkt;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units.
/// </summary>
STRING CCoordinateSystem::GetUnits()
{
    wchar_t* pUnit = Convert_UTF8_To_Wide(m_csprm.csdef.unit);
    if (NULL == pUnit)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetUnits", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING sUnit(pUnit);
    delete[] pUnit;

    return sUnit;
}

//Gets the units which this coordinate system uses.
INT32 CCoordinateSystem::GetUnitCode()
{
    return UnitFromString(m_csprm.csdef.unit);
}

//Sets the units of the coordinate system to the specified projection type.
//Throws an exception if the supplied  unit code is
//unknown, MgCoordinateSystemMismatchException if the specified unit is linear when we were
//expecting angular (or vice versa).
//
void CCoordinateSystem::SetUnitCode(INT32 unit)
{
    MG_TRY()

    //Can't change read-only items
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetUnitCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    //Make sure the projection's been properly set
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (MgCoordinateSystemProjectionCode::Unknown == prj)
    {
        //Projection hasn't been properly set, so we can't set the unit type.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetUnitCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Make sure it's a unit type we know about
    INT32 unitType;
    double dScale;
    if (!GetUnitInfo(unit, &unitType, &dScale))
    {
        //Unknown unit type.
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetUnitCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(dScale > 0.0);
    assert(MgCoordinateSystemUnitType::Unknown != unitType);

    //Make sure the unit type is the right kind for this projection
    INT32 prjUnitType = GetProjectionUnitType(prj);
    assert(MgCoordinateSystemUnitType::Unknown != prjUnitType);
    if (prjUnitType != unitType)
    {
        //Wrong unit type for this projection.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetUnitCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Okay, we're all set.  Copy in the values...
    const char *kpStrUnit = StringFromUnit(unit);
    assert(NULL != kpStrUnit);
    assert(strlen(kpStrUnit) < sizeof(m_csprm.csdef.unit));
    memset(m_csprm.csdef.unit, 0, sizeof(m_csprm.csdef.unit));
    strcpy(m_csprm.csdef.unit, kpStrUnit);
    m_csprm.csdef.unit_scl = dScale;

    //matching behavior of CScsloc when generating the cs_Csprm_
    //structure
    if (MgCoordinateSystemProjectionCode::LL == prj)
    {
        /* Now we can compute the two different scale factors.  In
           the lat/long case (remember that this is not really
           a projection) we simply store the factor which, by
           multiplication, will convert degrees to the system
           unit (i.e. seconds, radians, greads, whatever). */

        m_csprm.csdef.scale = m_csprm.csdef.unit_scl;
        m_csprm.csdef.scale = cs_One / m_csprm.csdef.scale;

        /* The unit scale factor is used to convert text height,
           line widths, etc.  Therefore, we need to store a
           value which will convert the degrees/seconds/grads/whatever
           to meters.  This is a function of the ellipsoid in use. */

        //if the ellipsoid's radius has already been defined
        //maybe we are in the creation process and it hasn't been defined yet
        if (m_csprm.datum.e_rad > 0.)
        {
            m_csprm.csdef.unit_scl *= cs_Two_pi * m_csprm.datum.e_rad / 360.0;
        }
    }
    else
    {
        //map_scl could be 0. is the CS is in the process of being created
        //from scratch
        if (m_csprm.csdef.map_scl>0.)
        {
            /* Here for all cartesian cases. */
            m_csprm.csdef.scale = cs_One / (m_csprm.csdef.unit_scl *
                                            m_csprm.csdef.map_scl);
        }
    }

    //...And return success!

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetUnitCode")
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units scale.
/// </summary>
double CCoordinateSystem::GetUnitScale()
{
    return m_csprm.csdef.unit_scl;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum X value.
/// </summary>
double CCoordinateSystem::GetMinX()
{
    return m_csprm.min_xy[0];
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum Y value.
/// </summary>
double CCoordinateSystem::GetMinY()
{
    return m_csprm.min_xy[1];
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum X value.
/// </summary>
double CCoordinateSystem::GetMaxX()
{
    return m_csprm.max_xy[0];
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum Y value.
/// </summary>
double CCoordinateSystem::GetMaxY()
{
    return m_csprm.max_xy[1];
}

//-----------------------------------------------------------------------------
//Get the key name of the coordinate system definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Csdef_::key_nm.
//
STRING CCoordinateSystem::GetCsCode()
{
    STRING sName;

    MG_TRY()
    wchar_t* pName = Convert_UTF8_To_Wide(m_csprm.csdef.key_nm);
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetCsCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sName=pName;
    delete[] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetCsCode")

    return sName;
}

//-----------------------------------------------------------------------------
//Sets the key name of the coordinate system.
//Throws an exception MgInvalidArgumentException if the provided string is not
//a legal coordinate system name.
//
void CCoordinateSystem::SetCsCode(CREFSTRING sCode)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetCsCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    if (sCode.empty())
    {
        //null pointer is obviously not legal...
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetCsCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sCode.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.SetCsCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Try to make a legal name.  CS_nampp() legalizes the
    //string and returns 0 for success.  If the string can't
    //be made legal, it returns -1.
    int nResult = CS_nampp(pStr);

    if (0 != nResult)
    {
        //illegal string
        delete [] pStr;
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetCsCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy into the definition
    memset(m_csprm.csdef.key_nm, 0, cs_KEYNM_DEF);
    strncpy(m_csprm.csdef.key_nm, pStr, cs_KEYNM_MAX);

    //Free the converted string
    delete [] pStr;

    //And we're done!
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetCsCode")
}

//Checks whether the specified string is a legal
//coordinate system key name.
//
bool CCoordinateSystem::IsLegalCsCode(CREFSTRING sCode)
{
    return IsLegalMentorName(sCode.c_str());
}


//Gets the projection type which this coordinate system uses.
//
STRING CCoordinateSystem::GetProjection()
{
    STRING sProjection;

    MG_TRY()

    wchar_t* pProjection = Convert_UTF8_To_Wide(m_csprm.csdef.prj_knm);
    if (NULL == pProjection)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetProjection", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sProjection=pProjection;
    delete[] pProjection;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetProjection")

    return sProjection;
}

//Gets the projection type which this coordinate system uses.
//
INT32 CCoordinateSystem::GetProjectionCode()
{
    return ProjectionFromString(m_csprm.csdef.prj_knm);
}

//Sets the projection to the specified projection type.
//Throws an exception MgInvalidArgumentException if the supplied projection code is unknown.
//Note:  Calling this function resets all numeric parameters in the
//coordinate system to zero; so when a client app sets up a coordinate
//system def using this interface, it must call SetProjection() *before*
//calling the various functions for setting numeric parameters.
//It also has to be called before SetEllipsoidDefinition or SetDatumDefinition
//that have to be called last in the process of setting up a system
//
void CCoordinateSystem::SetProjectionCode(INT32 prjType)
{
    MG_TRY()

    if (Protected())
    {
        //Can't change read-only items
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetProjectionCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    if (MgCoordinateSystemProjectionCode::Unknown==prjType)
    {
        //cs_UNKWN_PROJ
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetProjectionCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Set the projection string
    const char *kpProj = StringFromProjection(prjType);
    assert(NULL != kpProj);
    if (!kpProj)
    {
        //cs_UNKWN_PROJ
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetProjectionCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //if same as the existing projection, do nothing
    if (0==strcmp(m_csprm.csdef.prj_knm, kpProj))
    {
        return;
    }

    assert(strlen(kpProj) < sizeof(m_csprm.csdef.prj_knm));
    memset(&m_csprm.csdef.prj_knm, 0, sizeof(m_csprm.csdef.prj_knm));
    strcpy(m_csprm.csdef.prj_knm, kpProj);

    struct cs_Prjtab_ *pp=GetMentorProjectionObject(kpProj);
    assert(pp);
    if (!pp)
    {
        //cs_UNKWN_PROJ
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetProjectionCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    m_csprm.prj_code = pp->code;
    m_csprm.prj_flags = pp->flags;

    //Now reset all parameters to default values.

    //see if we can keep the current unit
    INT32 unitTypeCurrent;
    double dScale;
    bool bResetUnit=false;
    if (GetUnitInfo(GetUnitCode(), &unitTypeCurrent, &dScale))
    {
        INT32 prjUnitType = GetProjectionUnitType(prjType);
        if (prjUnitType!=unitTypeCurrent)
        {
            bResetUnit=true;
        }
    }
    else
    {
        bResetUnit=true;
    }

    SetProjectionDefaults();
    if (bResetUnit)
    {
        memset(m_csprm.csdef.unit, 0, sizeof(m_csprm.csdef.unit));
        m_csprm.csdef.unit_scl = 0.0;
    }

    //in case of NERTH we have no datum and no ellipsoid
    if (ProjectionIsNerthType (prjType))
    {
        memset(m_csprm.csdef.dat_knm, 0, sizeof(m_csprm.csdef.dat_knm));
        memset(m_csprm.csdef.elp_knm, 0, sizeof(m_csprm.csdef.elp_knm));

        m_pDatum = NULL;
        m_pEllipsoid = NULL;
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetProjectionCode")
}

//gets the projection description
STRING CCoordinateSystem::GetProjectionDescription()
{
    return ProjectionDescriptionFromString(m_csprm.csdef.prj_knm);
}

//If the coordinate system is geodetically referenced, this function gets
//the name of the datum which this coordinate system references.  (If the
//coordinate system is cartographically referenced, the function returns
//a null pointer.)  The caller is responsible for deleting the returned
//string via ::SysFreeString().
//
STRING CCoordinateSystem::GetDatum()
{
    STRING sDatum;

    MG_TRY()

    if (ProjectionIsNerthType (m_csprm.csdef.prj_knm))
    {
        //It's an arbitrary system.
        assert(NULL==m_pDatum);
        assert(NULL==m_pEllipsoid);
        return L"Local Datum";
    }

    if (!CsdefIsGeodetic(m_csprm.csdef))
    {
        //It's cartographically referenced.
        assert(NULL==m_pDatum);
        assert(m_pEllipsoid);
        return L"";
    }

    wchar_t* pDatum = Convert_UTF8_To_Wide(m_csprm.csdef.dat_knm);
    if (NULL == pDatum)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetDatum", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sDatum=pDatum;
    delete[] pDatum;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetDatum")

    return sDatum;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system datum description.
/// </summary>
STRING CCoordinateSystem::GetDatumDescription()
{
    if (!IsGeodetic())
    {
        return L"";
    }
    assert(m_pDatum);
    if (!m_pDatum)
    {
        return L"";
    }
    return m_pDatum->GetDescription();
}

//If the coordinate system is cartographically referenced, this function
//gets the name of the ellipsoid which this coordinate system references.
//If the coordinate system is geodetically referenced, the function returns
//the name from the datum
//
STRING CCoordinateSystem::GetEllipsoid()
{
    STRING sEllipsoid;

    MG_TRY()

    if (ProjectionIsNerthType (m_csprm.csdef.prj_knm))
    {
        //It's an arbitrary system.
        assert(NULL==m_pDatum);
        assert(NULL==m_pEllipsoid);
        return L"";
    }

    if (CsdefIsGeodetic(m_csprm.csdef))
    {
        //It's geodetically referenced.
        assert(m_pDatum);
        assert(NULL==m_pEllipsoid);
        if (m_pDatum)
        {
            return m_pDatum->GetEllipsoid();
        }
        return L"";
    }

    wchar_t* pEllipsoid = Convert_UTF8_To_Wide(m_csprm.csdef.elp_knm);
    if (NULL == pEllipsoid)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sEllipsoid=pEllipsoid;
    delete[] pEllipsoid;
    assert(NULL==m_pDatum);

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetEllipsoid")

    return sEllipsoid;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system ellipsoid description.
/// </summary>
STRING CCoordinateSystem::GetEllipsoidDescription()
{
    if (IsGeodetic())
    {
        assert(m_pDatum);
        assert(NULL==m_pEllipsoid);
        if (m_pDatum)
        {
            Ptr<MgCoordinateSystemEllipsoid> pEllipsoid=m_pDatum->GetEllipsoidDefinition();
            if (pEllipsoid)
            {
                return pEllipsoid->GetDescription();
            }
        }
        return L"";
    }

    if (!m_pEllipsoid)
    {
        return L"";
    }
    return m_pEllipsoid->GetDescription();
}
/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the EPSG code for this system, else zero.
/// </summary>
INT32 CCoordinateSystem::GetEpsgCode (void)
{
    short sEpsg;
    INT32 iEpsg;

    sEpsg = m_csprm.csdef.epsgNbr;
    if (sEpsg > 0)
    {
        iEpsg = sEpsg;
    }
    else if (sEpsg < 0)
    {
        sEpsg = -sEpsg;
        iEpsg = 65536 - sEpsg;
    }
    else
    {
        iEpsg = 0;
    }
    return iEpsg;
}
/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the Oracle SRID code for this system, else zero.
/// </summary>
INT32 CCoordinateSystem::GetSridCode (void)
{
    short sSrid;
    INT32 iSrid;

    sSrid = m_csprm.csdef.srid;
    if (sSrid > 0)
    {
        iSrid = sSrid;
    }
    else if (sSrid < 0)
    {
        sSrid = -sSrid;
        iSrid = 65536 - sSrid;
    }
    else
    {
        iSrid = 0;
    }
    return iSrid;
}
/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the quadrant specification in the EPSG data set for
/// this definition.  The returned value is zero if the system
/// does not have an EPSG equivalent.  The format of the
/// returned value is in the same form as the CS-MAP quad value,
/// but the value may be different.  CS-MAP definitions often
/// ignore the swapping of axes as is indicated in the EPSG
/// database.  Not doing so produces rotated and mirroed displays
/// which are _NOT_ what the customer desires or expects.
/// </summary>
INT16 CCoordinateSystem::GetEpsgQuadrant (void)
{
     return static_cast<INT16>(m_csprm.csdef.epsg_qd);
}
///////////////////////////////////////////////////////////////////////////
MgStringCollection* CCoordinateSystem::GetCategories()
{
    Ptr<MgStringCollection> pCategoryNames;

    MG_TRY()

    Ptr<MgCoordinateSystemCategoryDictionary> pCtDict=m_pCatalog->GetCategoryDictionary();
    if (!pCtDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetCategories", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }
    //get the category dictionary pointer
    Ptr<MgCoordinateSystemEnum> pEnumCategories=pCtDict->GetEnum();
    if (!pEnumCategories)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }

   pCategoryNames = new MgStringCollection();
   if (!pCategoryNames)
   {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetCategories", __LINE__, __WFILE__, NULL, L"", NULL);
   }

    //list all the categories one by one
    for ( ; ; )
    {
        Ptr<MgDisposableCollection> pCtColl=pEnumCategories->Next(1);
        if (1 != pCtColl->GetCount()) break;
        Ptr<MgDisposable> pCt=pCtColl->GetItem(0);
        MgCoordinateSystemCategory* pCtDef=dynamic_cast<MgCoordinateSystemCategory*>(pCt.p);
        assert(pCtDef);
        //on the category gets the list of all the coordinate systems to see if we are in this one
        Ptr<MgCoordinateSystemEnum> pEnumSystemsInCategory=pCtDef->GetEnum();
        if (!pEnumSystemsInCategory) break;
        for ( ; ; )
        {
            Ptr<MgStringCollection> pCsNameColl=pEnumSystemsInCategory->NextName(1);
            if (1 != pCsNameColl->GetCount()) break;
            if (GetCsCode()==pCsNameColl->GetItem(0))
            {
                //we are in this category, let's get it and move on to the next category
                pCategoryNames->Add(pCtDef->GetName());
                break;
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetCategories")

    return pCategoryNames.Detach();
}

//MgDisposable
void CCoordinateSystem::Dispose()
{
    delete this;
}

//Gets whether the def is "valid."  Validity, in this case, is defined in
//terms of whether a catalog will accept this def when attempting to add it.
//An example of an invalid def would be a coordinate system definition whose
//numerical parameters are mathematically invalid for its projection.  If
//this function returns false, then attempting to add the def to a catalog
//will result in failure.
//
bool CCoordinateSystem::IsValid()
{
    bool bIsValid = false;

    MG_TRY()

    if (ProjectionIsNerthType (m_csprm.csdef.prj_knm))
    {
        return true;
    }

    //Check for mathematical integrity
    CriticalClass.Enter();
    int nNumErrs = CS_cschk(&m_csprm.csdef, 0, NULL, 0);
    CriticalClass.Leave();
    bIsValid = (0 == nNumErrs);
    if (!bIsValid)
    {
        return false;
    }

    //Make sure the name is legal
    bIsValid = IsLegalMentorName(m_csprm.csdef.key_nm);
    if (!bIsValid)
    {
        return false;
    }

    //Make sure that the datum name (if geodetic) or ellipsoid name
    //(if cartographic) is a legal name.
    bool bGeodetic = IsGeodetic();
    if (bGeodetic)
    {
        //It's geodetic, so check the datum name.
        bIsValid = IsLegalMentorName(m_csprm.csdef.dat_knm);
    }
    else
    {
        //It's cartographic, so check the ellipsoid name.
        bIsValid = IsLegalMentorName(m_csprm.csdef.elp_knm);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.IsValid")

    return bIsValid;
}

//Gets whether the def is usable in the context of the given catalog.  The
//IsValid() function, above, simply checks the def for internal validity in
//its own right.  This function, on the other hand, checks for other
//conditions such as unresolved references and so forth which can only be
//evaluated in a larger context.  For example, a coordinate system definition
//may specify that it uses datum "FOO", which is a perfectly legitimate datum
//name, so IsValid() would return true; but unless there is in fact a datum
//"FOO" present in the catalog where we try to use the coordinate system, it
//can't be used and IsUsable() would return false.  Note that this function
//will always return false if IsValid() returns false.
//
bool CCoordinateSystem::IsUsable(MgCoordinateSystemCatalog *pCatalog)
{
    bool bIsUsable = false;

    MG_TRY()

    assert(NULL != pCatalog);
    CHECKARGUMENTNULL(pCatalog, L"MgCoordinateSystem.IsUsable");

    //First, make sure it's valid in its own right; if it's
    //not, then it's not usable
    bIsUsable = IsValid();
    if (!bIsUsable)
    {
        //Nope, not valid.  We need go no further.
        return false;
    }

    //no DT or EL reference for NERTH
    if (ProjectionIsNerthType (m_csprm.csdef.prj_knm))
    {
        return true;
    }

    //Is it geodetic?
    bool bGeodetic = IsGeodetic();

    if (bGeodetic)
    {
        //It's geodetic, so make sure the catalog has a
        //datum by that name.

        //Get the appropriate interface from the catalog
        Ptr<MgCoordinateSystemDatumDictionary> pDtDict=pCatalog->GetDatumDictionary();
        if (!pDtDict)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsUsable", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumDictionaryException", NULL);
        }

        //Build a wchar_t* from the datum name
        wchar_t* pStrDt = Convert_UTF8_To_Wide(m_csprm.csdef.dat_knm);
        if (NULL == pStrDt)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.IsUsable", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING strDt(pStrDt);
        delete[] pStrDt;

        //Check to see if the datum is there
        bIsUsable = pDtDict->Has(strDt);
    }   //if it's geodetic
    else
    {
        //It's cartographic (not geodetic), so make sure
        //the catalog has an ellipsoid by that name

        //Get the appropriate interface from the catalog
        Ptr<MgCoordinateSystemEllipsoidDictionary> pElDict=pCatalog->GetEllipsoidDictionary();
        if (!pElDict)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsUsable", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoEllipsoidDictionaryException", NULL);
        }

        //Build a wchar_t* from the ellipsoid name
        wchar_t* pStrEl = Convert_UTF8_To_Wide(m_csprm.csdef.elp_knm);
        if (NULL == pStrEl)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.IsUsable", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING strEl(pStrEl);
        delete[] pStrEl;

        //Check to see if the ellipsoid is there
        bIsUsable = pElDict->Has(pStrEl);
    }   //if it's cartographic

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.IsUsable")

    return bIsUsable;
}

//Checks whether the specified def is the "same" as this one.
//(Key name is ignored for purposes of comparison.)
//
bool CCoordinateSystem::IsSameAs(MgGuardDisposable *pDef)
{
    MG_TRY()

    assert(NULL != pDef);
    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystem.IsSameAs");

    int i;
    double *pdParam1, *pdParam2;

    //Make sure it's a coordinate system definition
    MgCoordinateSystem* pCsDef=dynamic_cast<MgCoordinateSystem*>(pDef);
    if (!pCsDef)
    {
        return false;   //not a coordsys def!
    }

    //Build a Mentor struct from the other datum interface
    cs_Csdef_ def;
    bool bResult = BuildCsDefFromInterface(pCsDef, def);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //And compare with this one!

    if (0 != CS_stricmp(m_csprm.csdef.dat_knm, def.dat_knm)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.elp_knm, def.elp_knm)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.prj_knm, def.prj_knm)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.group, def.group)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.locatn, def.locatn)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.cntry_st, def.cntry_st)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.unit, def.unit)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.desc_nm, def.desc_nm)) return false;
    if (0 != CS_stricmp(m_csprm.csdef.source, def.source)) return false;

    pdParam1 = &m_csprm.csdef.prj_prm1;
    pdParam2 = &def.prj_prm1;
    for (i=0; i<knCsNumParams; i++)
    {
        if (pdParam1[i] != pdParam2[i]) return false;
    }

    if (m_csprm.csdef.org_lng != def.org_lng) return false;
    if (m_csprm.csdef.org_lat != def.org_lat) return false;
    if (m_csprm.csdef.x_off != def.x_off) return false;
    if (m_csprm.csdef.y_off != def.y_off) return false;
    if (m_csprm.csdef.scl_red != def.scl_red) return false;
    if (m_csprm.csdef.map_scl != def.map_scl) return false;

    //If the units match, the unit scale matches.
    assert(m_csprm.csdef.unit_scl == def.unit_scl);

    //Note:  We deliberately do not make any assertion
    //about m_csprm.csdef.scale being equal to def.scale.  They
    //*should* be equal, since at this point we've established
    //that unit_scl and map_scl are equal, and supposedly
    //scale = 1.0 / (unit_scl*map_scl).  However, the
    //dictionary isn't always tidy, and in practice it turns
    //out that defs often have incorrect values stored here.

    for (i=0; i<2; i++)
    {
        if (m_csprm.csdef.zero[i] != def.zero[i]) return false;
        if (m_csprm.csdef.ll_min[i] != def.ll_min[i]) return false;
        if (m_csprm.csdef.ll_max[i] != def.ll_max[i]) return false;
        if (m_csprm.csdef.xy_min[i] != def.xy_min[i]) return false;
        if (m_csprm.csdef.xy_max[i] != def.xy_max[i]) return false;
    }

    if (m_csprm.csdef.quad != def.quad) return false;

    //do the ellipsoid
    if (m_pEllipsoid)
    {
        //do the ellipsoid
        Ptr<MgCoordinateSystemEllipsoid> pElDef=pCsDef->GetEllipsoidDefinition();
        if (!pElDef)
        {
            return false;
        }

        if (!m_pEllipsoid->IsSameAs(pElDef))
        {
            return false;
        }
    }

    //do the datum
    if (m_pDatum)
    {
        //do the ellipsoid
        Ptr<MgCoordinateSystemDatum> pDtDef=pCsDef->GetDatumDefinition();
        if (!pDtDef)
        {
            return false;
        }

        if (!m_pDatum->IsSameAs(pDtDef))
        {
            return false;
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.IsSameAs")

    //That's it!  If we've made it this far without finding
    //any differences, then they're the same as each other.
    return true;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns a copy of the coordinate system.
///</summary>
MgCoordinateSystem* CCoordinateSystem::CreateClone()
{
    Ptr<CCoordinateSystem> pNew;

    MG_TRY()

    pNew = new CCoordinateSystem(m_pCatalog);

    //Make an object to be the copy
    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy data members into the new object.  NOTE:  if new
    //data members are added to CCoordinateSystem, this will
    //need to be updated.
    pNew->m_csprm = m_csprm;
    pNew->m_bEncrypted = m_bEncrypted;

    //unset the EPSG code; we've an editable instance now where we neither can guarantee the correctness of the EPSG code
    //nor is it currently supported by CsMap's NameMapper anyway
    pNew->m_csprm.csdef.epsgNbr = 0;

    if (m_pDatum)
    {
        Ptr<MgCoordinateSystemDatum> pDtClone=m_pDatum->CreateClone();
        if (NULL == pDtClone.p)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pNew->m_pDatum = pDtClone;
    }

    if (m_pEllipsoid)
    {
        Ptr<MgCoordinateSystemEllipsoid> pElClone=m_pEllipsoid->CreateClone();
        if (NULL == pElClone.p)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pNew->m_pEllipsoid = pElClone;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.CreateClone")

    //And we're done!  Return success.
    return pNew.Detach();
}

//Get the description of the coordinate system definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Csdef_::desc_nm.
//
STRING CCoordinateSystem::GetDescription()
{
    STRING sDesc;

    MG_TRY()
    wchar_t* pDesc = Convert_UTF8_To_Wide(m_csprm.csdef.desc_nm);
    if (NULL == pDesc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sDesc=pDesc;
    delete[] pDesc;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetDescription")

    return sDesc;
}

//Sets the description of the coordinate system.
//
void CCoordinateSystem::SetDescription(CREFSTRING sDesc)
{
    SetString(sDesc, &m_csprm.csdef.desc_nm[0], sizeof(m_csprm.csdef.desc_nm));
}

//Returns whether the specified string is a legal description.
//
bool CCoordinateSystem::IsLegalDescription(CREFSTRING sDesc)
{
    return IsLegalString(sDesc.c_str(), sizeof(m_csprm.csdef.desc_nm));
}

//Gets the group of the def (provided by Mentor for grouping purposes; not
//currently used by any Autodesk GIS products, although that could change in
//the future).  The caller is responsible for deleting the returned string
//via ::SysFreeString().
//
//Corresponds to cs_Csdef_::group.
//
STRING CCoordinateSystem::GetGroup()
{
    STRING sGroup;

    MG_TRY()

    wchar_t* pGroup = Convert_UTF8_To_Wide(m_csprm.csdef.group);
    if (NULL == pGroup)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sGroup=pGroup;
    delete[] pGroup;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetGroup")

    return sGroup;
}

//Sets the group of the coordinate system.
//
void CCoordinateSystem::SetGroup(CREFSTRING sGroup)
{
    SetString(sGroup, &m_csprm.csdef.group[0], sizeof(m_csprm.csdef.group));
}

//Returns whether the specified string is a legal group string.
//
bool CCoordinateSystem::IsLegalGroup(CREFSTRING sGroup)
{
    return IsLegalString(sGroup.c_str(), sizeof(m_csprm.csdef.group));
}

//Get the source of the coordinate system definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Csdef_::source.
//
STRING CCoordinateSystem::GetSource()
{
    STRING sSource;

    MG_TRY()

    wchar_t* pSource = Convert_UTF8_To_Wide(m_csprm.csdef.source);
    if (NULL == pSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sSource=pSource;
    delete[] pSource;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetSource")

    return sSource;
}

//Sets the source of the coordinate system.
//
void CCoordinateSystem::SetSource(CREFSTRING sSource)
{
    SetString(sSource, &m_csprm.csdef.source[0], sizeof(m_csprm.csdef.source));
}

//Returns whether the specified string is a legal source string
//
bool CCoordinateSystem::IsLegalSource(CREFSTRING sSource)
{
    return IsLegalString(sSource.c_str(), sizeof(m_csprm.csdef.source));
}

//Gets whether the item is protected or not.  "Read-only" items are defined
//as having their protect flag set to 1.
//
bool CCoordinateSystem::IsProtected()
{
    return Protected();
}

//Gets the age of the item, in days since last modification, if it's a
//user-defined item (i.e., isn't marked as protected).  If it's read-only,
//then returns -1.
//
INT16 CCoordinateSystem::GetAge()
{
    INT16 sAge = Protected()? -1 : m_csprm.csdef.protect;
    return sAge;
}

//Sets whether this item is protected or not.  When protected,
//methods which attempt to modify the object will
//throw an exception MgCoordinateSystemProtectedException
//(Except for SetProtectMode and SetEncryptMode,
//which can be called even on protected objects.)
//
void CCoordinateSystem::SetProtectMode(bool bIsProtected)
{
    m_csprm.csdef.protect = bIsProtected? 1 : 0;
}

//Returns whether this item is encrypted or not.  The only effect
//of its "encrypted" state is how it behaves when persisted to or
//from a stream.
//
bool CCoordinateSystem::IsEncrypted()
{
    return m_bEncrypted;
}

//Sets whether this item is encrypted or not.
//
void CCoordinateSystem::SetEncryptMode(bool bIsEncrypted)
{
    m_bEncrypted = bIsEncrypted;
}

//Gets the "location" field of the def (corresponds to cs_Csdef_.locatn).
//The caller is responsible for deleting the returned string via
//::SysFreeString().
//
STRING CCoordinateSystem::GetLocation()
{
    STRING sLoc;

    MG_TRY()

    wchar_t* pLoc = Convert_UTF8_To_Wide(m_csprm.csdef.locatn);
    if (NULL == pLoc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sLoc=pLoc;
    delete[] pLoc;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetLocation")

    return sLoc;
}

//Sets the "location" field of the def.
//Throws an exception with InvalidArgument if the specified string is not legal
//for a location field.
//
void CCoordinateSystem::SetLocation(CREFSTRING sLoc)
{
    SetString(sLoc, &m_csprm.csdef.locatn[0], sizeof(m_csprm.csdef.locatn));
}

//Tests the specified string to find out whether it's a legal
//location field for this def.
//
bool CCoordinateSystem::IsLegalLocation(CREFSTRING sLoc)
{
    return IsLegalString(sLoc.c_str(), sizeof(m_csprm.csdef.locatn));
}

//Gets the "cntry_st" field of the def (see Mentor discussion of
//cs_Csdef_ for details).  The caller is responsible for deleting
//the returned string via ::SysFreeString().
//
STRING CCoordinateSystem::GetCountryOrState()
{
    STRING sCntrySt;

    MG_TRY()

    wchar_t* pCntrySt = Convert_UTF8_To_Wide(m_csprm.csdef.cntry_st);
    if (NULL == pCntrySt)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sCntrySt=pCntrySt;
    delete[] pCntrySt;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetCountryOrState")

    return sCntrySt;
}

//Sets the "cntry_st" field of the def.
//Throws an exception with InvalidArgument if the specified string is not legal
//for a cntry_st field.
//
void CCoordinateSystem::SetCountryOrState(CREFSTRING sCountryOrState)
{
    SetString(sCountryOrState, &m_csprm.csdef.cntry_st[0], sizeof(m_csprm.csdef.cntry_st));
}

//Tests the specified string to find out whether it's a legal cntry_st
//field for this def.
//
bool CCoordinateSystem::IsLegalCountryOrState(CREFSTRING sCountryOrState)
{
    return IsLegalString(sCountryOrState.c_str(), sizeof(m_csprm.csdef.cntry_st));
}

//Gets whether this coordinate system definition is geodetically referenced
//(as opposed to cartographically referenced).  Geodetically referenced
//coordinate systems refer to a datum.  Cartographically referenced
//coordinate systems refer to an ellipsoid instead.
//
//Mentor itself makes this distinction by storing an empty string in dat_knm
//to indicate that the def is cartographic instead.
//
bool CCoordinateSystem::IsGeodetic()
{
    return CsdefIsGeodetic(m_csprm.csdef);
}

//Gets the number of projection parameters used by this system's projection
//it is a value that is between 1 and knCsNumParams
INT32 CCoordinateSystem::GetProjectionParameterCount()
{
    INT32 nNbParams=0;

    MG_TRY()

    //Make sure that this parameter is used by this projection
    INT32 nProjectionCode = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        //Projection type hasn't been properly initialized.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetProjectionParamCount", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    // Now for each possible parameter.
    // for each possible parameter.
    int parmIdx = 0;
    for (parmIdx = 0;parmIdx < knCsNumParams;parmIdx += 1)
    {
        int nResult = CS_prjprm(NULL, nProjectionCode, parmIdx);
        if (nResult <= 0) break; //returns +1 for a valid parameter
    }

    nNbParams=static_cast<INT32>(parmIdx);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetProjectionParamCount")
    return nNbParams;
}

//Gets the nth parameter value.  ulIndex must be in the range 1 to
//knCsNumParams, corresponding to the data members prj_prm1 through
//prj_prm24 of Mentor's cs_Csdef_ struct.
//Throws an exception MgInvalidArgumentException if ulIndex is not
//in the range 1 to knCsNumParams, or MgCoordinateSystemMismatchException if the parameter at
//the specified index is not used in this projection.
//
double CCoordinateSystem::GetProjectionParameter(INT32 nIndex)
{
    double dValue = 0.0;

    MG_TRY()

    //Make sure the parameter is in the valid range
    if ((nIndex < 1) || (nIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.GetProjectionParameter", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure that this parameter is used by this projection
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (MgCoordinateSystemProjectionCode::Unknown == prj)
    {
        //Projection type hasn't been properly initialized.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetProjectionParameter", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }
    short prjCode = prj;
    //The following function call returns 1 if this
    //parameter is used for this projection, 0 if
    //not, or -1 if the projection code or parameter
    //number or incorrect.  The latter case should
    //never happen, as we've been careful to set
    //things up correctly.
    int nResult = CS_prjprm(NULL, prjCode, nIndex-1);
    assert(-1 != nResult);
    if (1 != nResult)
    {
        //The parameter isn't used.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetProjectionParameter", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //The parameter is used.  Return it.
    double *pdParms = &m_csprm.csdef.prj_prm1;
    dValue = pdParms[nIndex-1];

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetProjectionParameter")

    return dValue;
}

//Sets the nth parameter value.  ulIndex must be in the range 1 to
//knCsNumParams, corresponding to the data members prj_prm1 through
//prj_prm24 of Mentor's cs_Csdef_ struct.
//Throws an exception MgInvalidArgumentException if ulIndex is
//not in the range 1 to knCsNumParams;
//Throws an exception MgCoordinateSystemMismatchException, if the specified
//parameter is not used by this projection; if the specified
//value is outside the valid mathematical bounds for this parameter.
//
void CCoordinateSystem::SetProjectionParameter(INT32 nIndex, double dValue)
{
    MG_TRY()

    //Can't change read-only items
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    //Make sure the parameter is in the valid range
    if ((nIndex < 1) || (nIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure this parameter is used by this projection
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (MgCoordinateSystemProjectionCode::Unknown == prj)
    {
        //Projection type hasn't been properly initialized.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }
    struct cs_Prjprm_ info;
    short prjCode = prj;
    //The following function call returns 1 if this
    //parameter is used for this projection, 0 if
    //not, or -1 if the projection code or parameter
    //number or incorrect.  The latter case should
    //never happen, as we've been careful to set
    //things up correctly.
    int nResult = CS_prjprm(&info, prjCode, nIndex-1);
    assert(-1 != nResult);
    if (1 != nResult)
    {
        //The specified parameter isn't used by this
        //projection.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Okay, at this point we know our projection type
    //is valid, that our index is valid, and that this
    //projection uses the specified parameter.  Make
    //sure that the value they've given us falls within
    //the allowed mathematical range for this parameter.
    if ((dValue < info.min_val) || (dValue > info.max_val))
    {
        //They've given us an illegal value!
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Okay, the value they've given us is legitimate, too.
    //Go ahead and set the value.
    double *pdParms = &m_csprm.csdef.prj_prm1;
    pdParms[nIndex-1] = dValue;

    //And return success!

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetProjectionParameter")
}

//Gets the origin longitude of the coordinate system.  (Not used by
//all projections; if the current projection doesn't use this parameter,
//the returned result will not be meaningful.)
//
double CCoordinateSystem::GetOriginLongitude()
{
    return m_csprm.csdef.org_lng;
}

//Sets the origin longitude of the coordinate system.  (Not used
//by all projections.)
//
void CCoordinateSystem::SetOriginLongitude(double dOrgLng)
{
    MG_TRY()

    //Make sure we're not read-only
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetOriginLongitude", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.csdef.org_lng = dOrgLng;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetOriginLongitude")
}

//Gets the origin latitude of the coordinate system.  (Not used by
//all projections; if the current projection doesn't use this
//parameter, the returned result will not be meaningful.)
//
double CCoordinateSystem::GetOriginLatitude()
{
    return m_csprm.csdef.org_lat;
}

//Sets the origin latitude of the coordinate system.  (Not used
//by all projections.)
//
void CCoordinateSystem::SetOriginLatitude(double dOrgLat)
{
    MG_TRY()

    //Make sure we're not read-only
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetOriginLatitude", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.csdef.org_lat = dOrgLat;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetOriginLatitude")
}

//Gets the X offset (false easting) and Y offset (false northing) of the
//coordinate system.
//
double CCoordinateSystem::GetOffsetX()
{
    return m_csprm.csdef.x_off;
}
double CCoordinateSystem::GetOffsetY()
{
    return m_csprm.csdef.y_off;
}

//Sets the X offset (false easting) and Y offset (false
//northing) of the coordinate system.
//
void CCoordinateSystem::SetOffsets(double dXOffset, double dYOffset)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetOffsets", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.csdef.x_off = dXOffset;
    m_csprm.csdef.y_off = dYOffset;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetOffsets")
}

//Gets the scale reduction factor of the coordinate system.  (Not used
//by all projections; if the current projection doesn't use this
//parameter, will return 1.0.)
//
double CCoordinateSystem::GetScaleReduction()
{
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    double dSclRed = 1.;
    if (ProjectionUsesSclRed(prj))
    {
        dSclRed = m_csprm.csdef.scl_red;
    }
    return dSclRed;
}

//Sets the scale reduction factor of the coordinate system.  (Not used
//by all projections; if the current projection doesn't use this
//parameter, the function will have no effect.)
//
void CCoordinateSystem::SetScaleReduction(double dSclRed)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetScaleReduction", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (ProjectionUsesSclRed(prj))
    {
        m_csprm.csdef.scl_red = dSclRed;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetScaleReduction")
}

//Gets the map scaling factor of the coordinate system.  (Generally used
//only when trying to reproduce inches, millimeters, etc. on an existing
//map, e.g. 24000 on a USGS topo quad map.  Otherwise, defaults to 1.0.)
//
double CCoordinateSystem::GetMapScale()
{
    return m_csprm.csdef.map_scl;
}

//Sets the map scaling factor of the coordinate system.
//
void CCoordinateSystem::SetMapScale(double dMapScale)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetMapScale", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.csdef.map_scl = dMapScale;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetMapScale")
}

//Gets the "zero limits" of the coordinate system.  Absolute values of X
//and Y which are smaller than this will be converted to hard zeroes.
//(This is to prevent values like 1.23E-11, which are generally undesirable.)
//
double CCoordinateSystem::GetZeroX()
{
    return m_csprm.csdef.zero[0];
}
double CCoordinateSystem::GetZeroY()
{
    return m_csprm.csdef.zero[1];
}

//Sets the "zero limits" of the coordinate system.  Absolute values of X
//and Y which are smaller than this will be converted to hard zeroes.
//Passing in values of zero or less will disable this feature.
//
void CCoordinateSystem::SetZeroes(double dXZero, double dYZero)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetZeroes", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.csdef.zero[0] = (dXZero < 0.0)? 0.0 : dXZero;
    m_csprm.csdef.zero[1] = (dYZero < 0.0)? 0.0 : dYZero;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetZeroes")
}

//Gets the latitude-longitude range for the coordinate system.  If
//latitude-longitude range checking hasn't been turned on (the default),
//all values will be set to zero.
//
double CCoordinateSystem::GetLonMin()
{
    return m_csprm.cent_mer + m_csprm.min_ll[0];
}
double CCoordinateSystem::GetLonMax()
{
    return m_csprm.cent_mer + m_csprm.max_ll[0];
}
double CCoordinateSystem::GetLatMin()
{
    return m_csprm.min_ll[1];
}
double CCoordinateSystem::GetLatMax()
{
    return m_csprm.max_ll[1];
}

//Quickie little utility function that makes the smallest go first
//and the biggest go last.
static void OrderMinMax(double& d1, double& d2)
{
    if (d1 > d2)
    {
        double dSwap = d1;
        d1 = d2;
        d2 = dSwap;
    }
}

//Turns on latitude-longitude range checking for the coordinate system,
//and sets the allowed bounds to the specified values.  If range checking
//is turned on, then using this coordinate system definition to work with
//points outside its allowed range will result in an error or warning.
//
void CCoordinateSystem::SetLonLatBounds(double dLonMin, double dLatMin, double dLonMax, double dLatMax)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetLonLatBounds", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    OrderMinMax(dLonMin, dLonMax);
    OrderMinMax(dLatMin, dLatMax);

    m_csprm.min_ll[0] = dLonMin - m_csprm.cent_mer;
    m_csprm.min_ll[1] = dLatMin;
    m_csprm.max_ll[0] = dLonMax - m_csprm.cent_mer;
    m_csprm.max_ll[1] = dLatMax;

    m_csprm.csdef.ll_min[0] = m_csprm.min_ll[0];
    m_csprm.csdef.ll_min[1] = m_csprm.min_ll[1];
    
    m_csprm.csdef.ll_max[0] = m_csprm.max_ll[0];
    m_csprm.csdef.ll_max[1] = m_csprm.max_ll[1];

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetLonLatBounds")
}

//Turns off latitude-longitude range checking for the coordinate system.
//Any previous boundary values, set with SetLonLatBounds(), will be lost.
//
void CCoordinateSystem::CancelLonLatBounds()
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.CancelLonLatBounds", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    //it will only be set on CsMap's transformation struct, i.e.
    //not on [m_csprm.csdef.ll_*]
    m_csprm.min_ll[0] = 0.0;
    m_csprm.min_ll[1] = 0.0;
    m_csprm.max_ll[0] = 0.0;
    m_csprm.max_ll[1] = 0.0;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.CancelLonLatBounds")
}

//Turns on XY range checking for the coordinate system, and sets the allowed
//bounds to the specified values.  If range checking is turned on, then
//using this coordinate system definition to work with points outside its
//allowed range will result in an error or warning.
//
void CCoordinateSystem::SetXYBounds(double dXMin, double dYMin, double dXMax, double dYMax)
{
    MG_TRY()

    //Make sure we're not read-only
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetXYBounds", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    OrderMinMax(dXMin, dXMax);
    OrderMinMax(dYMin, dYMax);

    m_csprm.min_xy[0] = dXMin;
    m_csprm.min_xy[1] = dYMin;
    m_csprm.max_xy[0] = dXMax;
    m_csprm.max_xy[1] = dYMax;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetXYBounds")
}

//Turns off XY range checking for the coordinate system.  Any previous
//boundary values, set with SetXYBounds(), will be lost.
//
void CCoordinateSystem::CancelXYBounds()
{
    MG_TRY()

    //Make sure we're not read-only
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.CancelXYBounds", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    m_csprm.min_xy[0] = 0.0;
    m_csprm.min_xy[1] = 0.0;
    m_csprm.max_xy[0] = 0.0;
    m_csprm.max_xy[1] = 0.0;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.CancelXYBounds")
}

//Gets the quadrant of the coordinate system definition.
//
INT16 CCoordinateSystem::GetQuadrant()
{
    return m_csprm.csdef.quad;
}

//Sets the quadrant of the coordinate system, in the range 1 to 4
//or -1 to -4.  Throws an exception MgInvalidArgumentException if an illegal value
//is provided.
//
void CCoordinateSystem::SetQuadrant(INT16 sQuad)
{
    MG_TRY()

    //Make sure we're not read-only
    if (Protected())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetQuadrant", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    if ((sQuad < -4) || (sQuad > 4) || (0 == sQuad))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetQuadrant", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_csprm.csdef.quad = sQuad;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetQuadrant")
}

//Gets a copy of the datum which the coordinate system uses.  Caller is
//responsible for freeing the returned pointer via Release().  If the
//MgCoordinateSystem has not been initialized, or if the coordinate system is
//cartographic rather than geodetic (and therefore doesn't reference
//a datum), the returned pointer will be null.
MgCoordinateSystemDatum* CCoordinateSystem::GetDatumDefinition()
{
    //Make sure we've been initialized
    if (!IsInitialized()) return NULL;
    if (MgCoordinateSystemType::Arbitrary==GetType()) return NULL;

    //Make sure we're geodetic
    if (!IsGeodetic()) return NULL;

    assert(m_pDatum);
    return SAFE_ADDREF(m_pDatum.p);
}

/*
//A system might be geodetic (uses a datum) or cartographic (uses only an ellipsoid)
//Calling this method defines the datum name only (geodetic coordinate system)
//no check of its existence is done
//It the system is currently non-geodetic (uses an ellipsoid only),
//this method will remove the ellipsoid name and put the datum name inside the coordinate system
void CCoordinateSystem::SetDatum(CREFSTRING sDatum)
{
    MG_TRY()
    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }
    //no DT or EL if NERTH projection is used
    if (ProjectionIsNerthType (GetProjectionCode()))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumIfArbitraryException", NULL);
    }
    if (!IsLegalMentorName(sDatum.c_str()))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetDatum", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    char *pStr = Convert_Wide_To_UTF8(sDatum.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.SetDatum", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    memset(m_csprm.csdef.dat_knm, 0, sizeof(m_csprm.csdef.key_nm));
    memset(m_csprm.csdef.elp_knm, 0, sizeof(m_csprm.csdef.key_nm));
    strcpy(m_csprm.csdef.dat_knm, pStr);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetDatum")
}

void CCoordinateSystem::SetEllipsoid(CREFSTRING sEllipsoid)
{
    MG_TRY()
    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoid", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }
    //no DT or EL if NERTH projection is used
    if (ProjectionIsNerthType (GetProjectionCode()))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoid", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumIfArbitraryException", NULL);
    }
    if (!IsLegalMentorName(sEllipsoid.c_str()))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    char *pStr = Convert_Wide_To_UTF8(sEllipsoid.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.SetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    memset(m_csprm.csdef.dat_knm, 0, sizeof(m_csprm.csdef.key_nm));
    memset(m_csprm.csdef.elp_knm, 0, sizeof(m_csprm.csdef.key_nm));
    strcpy(m_csprm.csdef.elp_knm, pStr);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetEllipsoid")
}
*/

//A system might be geodetic (uses a datum) or cartographic (uses only an ellipsoid)
//Calling this method sets up a geodetic coordinate system
//It the system is currently non-geodetic (uses an ellipsoid only),
//this method will remove the ellipsoid and put the datum inside the coordinate system
//This method needs to be called after the coordinate system parameters have been fully specified
//The datum also needs to be fully specified. That means all its parameters and its
//ellipsoid need to be specified before calling this method.
//If we start by setting the datum the method will fail because it needs the
//coordinate system parameters this is not just setting an internal pointer.
//Many other internal parameters are set up when this method is called
void CCoordinateSystem::SetDatumDefinition(MgCoordinateSystemDatum *pDatum)
{
    cs_Csprm_ *pCsprm = NULL;

    MG_TRY()

    assert(NULL != pDatum);
    CHECKARGUMENTNULL(pDatum, L"MgCoordinateSystem.SetDatumDefinition");
    //if we check the protection, we can never setup any system since all systems
    //are built via void CCoordinateSystem::InitFromCatalog(const cs_Csdef_& csdef)
    /*
    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }
    */

    //no DT or EL if NERTH projection is used
    if (ProjectionIsNerthType (GetProjectionCode()))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDatumIfArbitraryException", NULL);
    }

    //Get information out of the datum def
    cs_Dtdef_ dtdef;
    bool bResult = BuildDtDefFromInterface(pDatum, dtdef);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //If the coordinate system is not geodetic we need to switch it
    //to a geodetic system and get rid of the ellipsoid information
    if (!IsGeodetic())
    {
        //commenting out this assert that is anoying when we are building
        //a custom system from scratch. At this time there's no datum and no ellipsoid at all
        //assert(m_pEllipsoid);
        //it's cartographic.
        strcpy(m_csprm.csdef.elp_knm, "");
        m_pEllipsoid = NULL;
    }

    //update the datum name inside the Mentor csdef
    strcpy(m_csprm.csdef.dat_knm, dtdef.key_nm);

    //Make sure the datum name matches the coordinate system's
    //specified datum name.
    //
    //We have to run the names thru Mentor's legalization
    //routine first, since they might contain "defaultable"
    //characters (see definition of cs_DFLT_IDNTBEG in cs_map.h)
    if ((0 != CS_nampp(m_csprm.csdef.dat_knm))
        || (0 != CS_nampp(dtdef.key_nm))
        || (0 != CS_stricmp(m_csprm.csdef.dat_knm, dtdef.key_nm)))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Get information out of the ellipsoid def
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoid=pDatum->GetEllipsoidDefinition();
    assert(pEllipsoid);
    if (!pEllipsoid)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    cs_Eldef_ eldef;
    bResult = BuildElDefFromInterface(pEllipsoid, eldef);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Okay, at this point we have a cs_Csdef_, cs_Dtdef_, and cs_Eldef_
    //pulled out of our input parameters, so we can go ahead and
    //initialize.
    CriticalClass.Enter();
    pCsprm = CScsloc2(&m_csprm.csdef, &dtdef, &eldef);
    CriticalClass.Leave();
    if (NULL == pCsprm)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
    }

    //Everything worked okay!  Copy the data into our member variables.
    Uninitialize();

    Ptr<MgCoordinateSystemDatum> pDatumClone = pDatum->CreateClone();
    if (NULL == pDatumClone.p)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    m_pDatum = pDatumClone;

    assert(NULL == m_pEllipsoid);
    m_csprm = *pCsprm;

    //Make sure the coordinate system is valid
    if (!IsValid())
    {
         throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    MG_CATCH(L"MgCoordinateSystem.SetDatumDefinition")

    CS_free(pCsprm);

    MG_THROW()
}

//Gets a copy of the ellipsoid definition which the coordinate system
//uses.  Caller is responsible for freeing the returned pointer via
//Release().  Works for geodetic and cartographic system but not for
//arbitrary system in which case we have no ellipsoid and we return NULL.
//
MgCoordinateSystemEllipsoid* CCoordinateSystem::GetEllipsoidDefinition()
{
    //Make sure we've been initialized
    if (!IsInitialized()) return NULL;
    if (MgCoordinateSystemType::Arbitrary==GetType()) return NULL;

    //If we're cartographic
    if (IsGeodetic())
    {
        if (!m_pDatum)
        {
            return NULL;
        }
        MgCoordinateSystemEllipsoid* pEllipsoid=m_pDatum->GetEllipsoidDefinition();
        return pEllipsoid;
    }

    assert(m_pEllipsoid);
    return SAFE_ADDREF(m_pEllipsoid.p);
}

//A system might be geodetic (uses a datum) or cartographic (uses only an ellipsoid)
//Calling this method sets up a cartographic coordinate system
//It it uses a datum, this method will not set the datum's ellipsoid but will rather
//remove the datum and put the ellipsoid directly inside the coordinate system
//If you want to set the datum's ellipsoid, get the datum first using the method
//MgCoordinateSystem::GetDatum and then use
//MgCoordinateSystemDatum::SetEllipsoid(MgCoordinateSystemEllipsoid *pDef)
//This method needs to be called after the coordinate system parameters have been fully specified
//if we start by seeting the ellipsoid the method will fail because it needs the coordinate system parameters
//this is not just setting an internal pointer. Many other internal parameters are
//set up when this method is called
void CCoordinateSystem::SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoid)
{
    cs_Csprm_ *pCsprm=NULL;
    cs_Datum_ *pDatum=NULL;

    MG_TRY()

    assert(NULL != pEllipsoid);
    CHECKARGUMENTNULL(pEllipsoid, L"MgCoordinateSystem.SetEllipsoidDefinition");
    //if we check the protection, we can never setup any system since all systems
    //are built via void CCoordinateSystem::InitFromCatalog(const cs_Csdef_& csdef)
    /*
    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }
    */

    //no DT or EL if NERTH projection is used
    if (ProjectionIsNerthType (GetProjectionCode()))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoEllipsoidIfArbitraryException", NULL);
    }

    //Get information out of the Ellipsoid def
    cs_Eldef_ eldef;
    bool bResult = BuildElDefFromInterface(pEllipsoid, eldef);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //If the coordinate system is geodetic we need to switch it
    //to a non geodetic system and get rid of the datum information
    if (IsGeodetic())
    {
        //commenting out this assert that is anoying when we are building
        //a custom system from scratch. At this time there's no datum and no ellipsoid at all
        //assert(m_pDatum);
        //it's geodetic.
        strcpy(m_csprm.csdef.dat_knm, "");
        m_pDatum = NULL;
    }

    //update the Ellipsoid name inside the Mentor csdef
    strcpy(m_csprm.csdef.elp_knm, eldef.key_nm);

    //Make sure the datum name matches the coordinate system's
    //specified datum name.
    //
    //We have to run the names thru Mentor's legalization
    //routine first, since they might contain "defaultable"
    //characters (see definition of cs_DFLT_IDNTBEG in cs_map.h)
    if ((0 != CS_nampp(m_csprm.csdef.elp_knm))
        || (0 != CS_nampp(eldef.key_nm))
        || (0 != CS_stricmp(m_csprm.csdef.elp_knm, eldef.key_nm)))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Okay, at this point we have a valid cs_Csdef_ and cs_Eldef_.
    //Call CSdtloc2() to get an initialized cs_Datum_, then pass that
    //in to CScsloc() to get a cs_Csprm_.
    CriticalClass.Enter();
    pDatum = CSdtloc2(NULL, &eldef);
    CriticalClass.Leave();
    if (NULL == pDatum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
    }

    CriticalClass.Enter();
    pCsprm = CScsloc(&m_csprm.csdef, pDatum);
    CriticalClass.Leave();
    if (NULL == pCsprm)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
    }

    //We're all set.  Initialize our data members.
    Uninitialize();

    Ptr<MgCoordinateSystemEllipsoid> pEllipsoidClone = pEllipsoid->CreateClone();
    if (NULL == pEllipsoidClone.p)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    m_pEllipsoid = pEllipsoidClone;

    m_csprm = *pCsprm;

    //Make sure the coordinate system is valid
    if (!IsValid())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    MG_CATCH(L"MgCoordinateSystem.SetEllipsoidDefinition")

    CS_free(pDatum);
    CS_free(pCsprm);

    MG_THROW()
}

//Tests the specified X,Y point for validity in the coordinate
//system.  Returns false in any of the following cases:  the
//coordinate system has not been successfully initialized; the
//specified point is not mathematically valid for the source
//coordinate system; the coordinate system has a defined useful
//XY range, the converter has range checking turned on, and the
//specified point lies outside that range.  Otherwise, returns
//true.
//Throws an exception MgCoordinateSystemNotReadyException
//if the coordinate system has not
//been successfully initialized.
bool CCoordinateSystem::IsValidXY(double dX, double dY)
{
    return IsValidCoord(CS_xychk, dX, dY);
}

//Tests the specified longitude and latitude coordinate point
//for validity in the coordinate system.  Returns false in any
//of the following cases:  the coordinate system has not been
//successfully initialized; the specified point is not
//mathematically valid for the source coordinate system; the
//coordinate system has a defined useful latitude/longitude
//range, the converter has range checking turned on, and the
//specified point lies outside that range.  Otherwise, returns
//true.  Throws an exception MgCoordinateSystemNotReadyException
//if the coordinate system has not
//been successfully initialized.
//
bool CCoordinateSystem::IsValidLonLat(double dLongitude, double dLatitude)
{
    return IsValidCoord(CS_llchk, dLongitude, dLatitude);
}

//-----------------------------------------------------------------------------
void CCoordinateSystem::ConvertCoordinateFromLonLat(MgCoordinate* lonLatToCoordinate)
{
    MG_TRY()
    if(lonLatToCoordinate->GetDimension() == MgCoordinateDimension::XY ||
       lonLatToCoordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        double x=0.,y=0.;
        ConvertPoint(CS_llchk, CS_ll2cs, lonLatToCoordinate->GetX(), lonLatToCoordinate->GetY(), 0., &x, &y, NULL);
        lonLatToCoordinate->SetX(x);
        lonLatToCoordinate->SetY(y);
    }
    else if(lonLatToCoordinate->GetDimension() == MgCoordinateDimension::XYZ ||
            lonLatToCoordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        double x=0.,y=0.,z=0.;
        ConvertPoint(CS_llchk, CS_ll3cs, lonLatToCoordinate->GetX(), lonLatToCoordinate->GetY(), lonLatToCoordinate->GetZ(), &x, &y, &z);
        lonLatToCoordinate->SetX(x);
        lonLatToCoordinate->SetY(y);
        lonLatToCoordinate->SetZ(z);
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
}

//-----------------------------------------------------------------------------
void CCoordinateSystem::ConvertCoordinateToLonLat(MgCoordinate* coordinateToLonLat)
{
    MG_TRY()
    if(coordinateToLonLat->GetDimension() == MgCoordinateDimension::XY ||
       coordinateToLonLat->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        double dLongitude=0., dLatitude=0.;
        ConvertPoint(CS_xychk, CS_cs2ll, coordinateToLonLat->GetX(), coordinateToLonLat->GetY(), 0., &dLongitude, &dLatitude, NULL);
        coordinateToLonLat->SetX(dLongitude);
        coordinateToLonLat->SetY(dLatitude);
    }
    else if(coordinateToLonLat->GetDimension() == MgCoordinateDimension::XYZ ||
            coordinateToLonLat->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        double dLongitude=0., dLatitude=0., dZDest=0.;
        ConvertPoint(CS_xychk, CS_cs3ll, coordinateToLonLat->GetX(), coordinateToLonLat->GetY(), coordinateToLonLat->GetZ(), &dLongitude, &dLatitude, &dZDest);
        coordinateToLonLat->SetX(dLongitude);
        coordinateToLonLat->SetY(dLatitude);
        coordinateToLonLat->SetZ(dZDest);
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
}

//-----------------------------------------------------------------------------
//Converts the specified X,Y point in the coordinate system to
//latitude/longitude coordinates (in the same datum).
//If the specified X,Y coords are not
//mathematically valid for this coordinate system,
//Throws an exception InvalidArgument.
//
MgCoordinate* CCoordinateSystem::ConvertToLonLat(double dX, double dY)
{
    Ptr<MgCoordinate> pCoordinate;
    MG_TRY()
    double dLongitude=0., dLatitude=0.;
    ConvertPoint(CS_xychk, CS_cs2ll, dX, dY, 0., &dLongitude, &dLatitude, NULL);
    pCoordinate=new MgCoordinateXY(dLongitude, dLatitude);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertToLonLat")
    return pCoordinate.Detach();
}

//-----------------------------------------------------------------------------
MgCoordinate* CCoordinateSystem::ConvertToLonLat(double dX, double dY, double dZ)
{
    Ptr<MgCoordinate> pCoordinate;
    MG_TRY()
    double dLongitude=0., dLatitude=0., dZDest=0.;
    ConvertPoint(CS_xychk, CS_cs3ll, dX, dY, dZ, &dLongitude, &dLatitude, &dZDest);
    pCoordinate=new MgCoordinateXYZ(dLongitude, dLatitude, dZDest);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertToLonLat")
    return pCoordinate.Detach();
}

//-----------------------------------------------------------------------------
//Works the same as ConvertToLonLat(), except that it converts from
//latitude/longitude to coordinate system X,Y values instead.
//Success conditions and return values are the same as for
//ConvertToLonLat().  The only difference is that range checking will
//be based on the coordinate system's defined latitude/longitude
//boundaries (rather than X,Y boundaries), if any.
//
MgCoordinate* CCoordinateSystem::ConvertFromLonLat(double dLongitude, double dLatitude)
{
    Ptr<MgCoordinate> pCoordinate;
    MG_TRY()
    double dX=0., dY=0.;
    ConvertPoint(CS_llchk, CS_ll2cs, dLongitude, dLatitude, 0., &dX, &dY, NULL);
    pCoordinate=new MgCoordinateXY(dX, dY);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
    return pCoordinate.Detach();
}

//-----------------------------------------------------------------------------
MgCoordinate* CCoordinateSystem::ConvertFromLonLat(double dLongitude, double dLatitude, double dZ)
{
    Ptr<MgCoordinate> pCoordinate;
    MG_TRY()
    double dX=0., dY=0., dZDest=0.;
    ConvertPoint(CS_llchk, CS_ll3cs, dLongitude, dLatitude, dZ, &dX, &dY, &dZDest);
    pCoordinate=new MgCoordinateXYZ(dX, dY, dZDest);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
    return pCoordinate.Detach();
}

//Gets the convergence angle, in degrees east of north, of the
//coordinate system at the specified longitude/latitude point.
//Throws an exception MgCoordinateSystemNotReadyException
//if the coordinate system has not been
//successfully initialized; InvalidArgument of the
//specified coordinate is not mathematically valid.
//
double CCoordinateSystem::GetConvergence(double dLongitude, double dLatitude)
{
    return GetDouble(CS_cscnv, dLongitude, dLatitude);
}

//Gets the grid scale of the coordinate system at the specified
//longitude/latitude point.
//Throws an exception MgCoordinateSystemNotReadyException if the
//coordinate system has not been successfully initialized;
//InvalidArgument of the specified coordinate is not
//mathematically valid.
double CCoordinateSystem::GetScale(double dLongitude, double dLatitude)
{
    return GetDouble(CS_csscl, dLongitude, dLatitude);
}

//Gets the grid scale along meridians of the coordinate system at
//the specified longitude/latitude point.
double CCoordinateSystem::GetScaleH(double dLongitude, double dLatitude)
{
    return GetDouble(CS_cssch, dLongitude, dLatitude);
}

//Gets the grid scale along parallels of the coordinate system at
//the specified longitude/latitude point.
double CCoordinateSystem::GetScaleK(double dLongitude, double dLatitude)
{
    return GetDouble(CS_cssck, dLongitude, dLatitude);
}

//Sets the specified string destination to the specified source
//string.  Fails if the source string isn't legal.  Includes check
//to make sure this object is writable.
//
void CCoordinateSystem::SetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize)
{
    MG_TRY()

    assert(NULL != pDest);
    CHECKARGUMENTNULL(pDest, L"MgCoordinateSystem.SetString");

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.SetString", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);
    }

    if (!IsLegalString(sSrc.c_str(), nMaxSize))
    {
        //Can't set string, caller gave us an illegal value
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sSrc.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it in
    memset(pDest, 0, nMaxSize);
    strncpy(pDest, pStr, nMaxSize);

    //Clean up and return success.
    delete [] pStr;

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SetString")
}

//Private function that evaluates the protect data member of m_csprm.csdef
//to decide whether this object is protected or not.
//
bool CCoordinateSystem::Protected()
{
    return (1 == m_csprm.csdef.protect);
}



//Private function which sets all numeric parameters to appropriate
//defaults, based on the current projection.
void CCoordinateSystem::SetProjectionDefaults()
{
    //Set defaults which will be the same across all projection types
    m_csprm.csdef.org_lng = m_csprm.csdef.org_lat = 0.0;
    m_csprm.csdef.x_off = m_csprm.csdef.y_off = 0.0;
    m_csprm.csdef.scl_red = 1.0;
    m_csprm.csdef.map_scl = 1.0;
    m_csprm.csdef.zero[0] = m_csprm.csdef.zero[1] = 0.0;
    m_csprm.csdef.hgt_lng = m_csprm.csdef.hgt_lat = m_csprm.csdef.hgt_zz = m_csprm.csdef.geoid_sep = 0.0;
    int i=0;
    for (i=0; i<2; i++)
    {
        m_csprm.csdef.zero[i] = 0.0;
        m_csprm.csdef.ll_min[i] = m_csprm.csdef.ll_max[i] = 0.0;
        m_csprm.csdef.xy_min[i] = m_csprm.csdef.xy_max[i] = 0.0;

        m_csprm.min_ll[i] = m_csprm.max_ll[i] = 0.0;
        m_csprm.min_xy[i] = m_csprm.max_xy[i] = 0.0;
    }
    m_csprm.csdef.quad = 1;
    m_csprm.csdef.order = m_csprm.csdef.zones = 0;
    m_csprm.csdef.epsgNbr = m_csprm.csdef.srid = m_csprm.csdef.epsg_qd = 0;

    //Set defaults specifically according to the current projection
    //type.  If the current projection type is undefined, set to zero.
    double *pdParms = &m_csprm.csdef.prj_prm1;
    INT32 prj = ProjectionFromString(m_csprm.csdef.prj_knm);
    if (MgCoordinateSystemProjectionCode::Unknown == prj)
    {
        memset(pdParms, 0, knCsNumParams * sizeof(double));
        return;
    }
    //Okay, now set the defaults.
    short prjCode = prj;
    struct cs_Prjprm_ info;
    for (i=0; i<knCsNumParams; i++)
    {
        //The following function call returns 1 if this
        //parameter is used for this projection, 0 if
        //not, or -1 if the projection code or parameter
        //number or incorrect.  The latter case should
        //never happen, as we've been careful to set
        //things up correctly.
        int nResult = CS_prjprm(&info, prjCode, i);
        assert(-1 != nResult);
        if (1 == nResult)
        {
            //This parameter is used by this projection.
            //Set it to an appropriate default value.
            pdParms[i] = info.deflt;
        }
        else
        {
            //This parameter is not used by this projection.
            //Set it to zero.
            pdParms[i] = 0.0;
        }
    }   //for each of the knCsNumParams parameters
}

//-------------------- Private member functions ---------------------

//Returns whether the def is initialized or not.
bool CCoordinateSystem::IsInitialized()
{
    return (m_csprm.csdef.key_nm[0]!='\0');
}

//Utility function which restores the object to an
//uninitialized state.
void CCoordinateSystem::Uninitialize()
{
    m_pDatum = NULL;
    m_pEllipsoid = NULL;
    memset(&m_csprm, 0, sizeof(m_csprm));
}

//Private member function.  Checks either lat/long or XY
//coordinates, depending on the checker function pointer
//passed in.
//
bool CCoordinateSystem::IsValidCoord(chkfunc check, double dCoord1, double dCoord2)
{
    bool bIsValid = false;

    MG_TRY()

    assert(NULL != check);

    //Make sure we've been initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsValidCoord", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Find out whether we're in range
    double dCoords[3] = { dCoord1, dCoord2, 0.0 };
    CriticalClass.Enter();
    int nResult = check(&m_csprm, 1, &dCoords);
    CriticalClass.Leave();

    if (cs_CNVRT_OK == nResult)
    {
        //we're clear!
        bIsValid = true;
    }
    else if (!(nResult & cs_CNVRT_DOMN))
    {
        //We're within the mathematical domain, and range
        //checking is turned off.  So even though we may
        //be outside the useful range, return valid.
        bIsValid = true;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.IsValidCoord")

    return bIsValid;
}

//Private member function wich converts an individual point.
//pdDestCoord3=NULL if not needed
//
void CCoordinateSystem::ConvertPoint(chkfunc check, llcsfunc convert, double dSourceCoord1, double dSourceCoord2, double dSourceCoord3, double *pdDestCoord1, double *pdDestCoord2, double *pdDestCoord3)
{
    MG_TRY()

    assert(NULL != pdDestCoord1);
    assert(NULL != pdDestCoord2);
    CHECKARGUMENTNULL(pdDestCoord1, L"MgCoordinateSystem.ConvertPoint");
    CHECKARGUMENTNULL(pdDestCoord2, L"MgCoordinateSystem.ConvertPoint");

    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.ConvertPoint", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Do the conversion.
    int nResult;
    double dCoords[3] = { dSourceCoord1, dSourceCoord2, dSourceCoord3 };
    CriticalClass.Enter();
    nResult = convert(&m_csprm, dCoords, dCoords);
    CriticalClass.Leave();

    *pdDestCoord1 = dCoords[0];
    *pdDestCoord2 = dCoords[1];
    if (pdDestCoord3)
    {
        *pdDestCoord3=dCoords[2];
    }
    if (cs_CNVRT_NRML != nResult)
    {
        //The conversion failed, because we're outside the
        //mathematical domain of the coordinate system.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertPoint", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertPoint")
}

//Private member function which gets a floating-point
//value based on an input lat/long coord.
//
double CCoordinateSystem::GetDouble(doublefunc func, double dLongitude, double dLatitude)
{
    double dResult = 0.;

    MG_TRY()

    assert(NULL != func);
    CHECKARGUMENTNULL(func, L"MgCoordinateSystem.GetDouble");

    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.GetDouble", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Check the range.  We need to do this even if range checking
    //is turned off, since if the point is outside the mathematical
    //domain of the coordinate system, func() could blow up.
    //
    bool bWarn = false;
    int nResult;
    double dCoords3d[3] = { dLongitude, dLatitude, 0.0 };
    CriticalClass.Enter();
    nResult = CS_llchk(&m_csprm, 1, &dCoords3d);
    CriticalClass.Leave();
    if (nResult & cs_CNVRT_DOMN)
    {
        //Outside the mathematical domain!
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.GetDouble", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }

    //Do the conversion.
    double dCoords[2] = { dLongitude, dLatitude };
    CriticalClass.Enter();
    dResult = func(&m_csprm, dCoords);
    CriticalClass.Leave();

    //Return success, or (if we decided earlier that it's
    //necessary) a warning.
    if (bWarn)
    {
        //WARNING
        //should really be a warning but throwing exception
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.GetDouble", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetDouble")

    return dResult;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystem::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

//*****************************************************************************
MgCoordinateSystemMeasure* CCoordinateSystem::GetMeasure()
{
    CCoordinateSystemMeasure* pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemMeasure(this);
    if (!pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetMeasure", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetMeasure")
    return pNew;
}

//*****************************************************************************
void CCoordinateSystem::ConvertFromLonLat(double lon, double lat, double& x, double& y)
{
    MG_TRY()
    ConvertPoint(CS_llchk, CS_ll2cs, lon, lat, 0., &x, &y, NULL);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
}

//*****************************************************************************
void CCoordinateSystem::ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize)
{
    MG_TRY()
    for(int i=0;i<arraySize;i++)
    {
        ConvertPoint(CS_llchk, CS_ll2cs, lon[i], lat[i], 0., &x[i], &y[i], NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertFromLonLat")
}

//*****************************************************************************
void CCoordinateSystem::ConvertToLonLat(double x, double y, double& lon, double& lat)
{
    MG_TRY()
    ConvertPoint(CS_xychk, CS_cs2ll, x, y, 0., &lon, &lat, NULL);
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertToLonLat")
}

//*****************************************************************************
void CCoordinateSystem::ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize)
{
    MG_TRY()
    for(int i=0;i<arraySize;i++)
    {
        ConvertPoint(CS_xychk, CS_cs2ll, x[i], y[i], 0., &lon[i], &lat[i], NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystem.ConvertToLonLat")
}

//*****************************************************************************
UINT8* CCoordinateSystem::SerializeFrom(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    UINT8 nVersion=pStreamOut[0];

    if (kCsRelease0==nVersion)
    {
        pStreamOut++;

        //Read the def from the stream
        cs_Csdef_ def;
        UINT8* pBuf = reinterpret_cast<UINT8 *>(&def);
        memcpy(pBuf, pStreamOut, sizeof(def));
        pStreamOut = pStreamOut + sizeof(def);

        //Decrypt it if necessary
        unsigned char ucKey = def.fill[0];
        bool bEncrypted = ('\0' != ucKey);
        if (bEncrypted)
        {
            for (long i=0; i<sizeof(def); i++)
            {
                pBuf[i] ^= 0xFF;
            }
            def.fill[0] = '\0';
        }

        //Make sure it's valid
        bool bPrevEncrypted = m_bEncrypted;
        cs_Csdef_ prevDef = m_csprm.csdef;
        m_csprm.csdef = def;
        m_bEncrypted = bEncrypted;
        if (!IsValid())
        {
            m_bEncrypted = bPrevEncrypted;
            m_csprm.csdef = prevDef;
            throw new MgInvalidArgumentException(L"MgCoordinateSystem.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SerializeFrom")
    return pStreamOut;
}

//*****************************************************************************
UINT8* CCoordinateSystem::SerializeTo(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystem.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //save the version
    pStreamOut[0]=kCsRelease0;
    pStreamOut++;

    cs_Csdef_ def = m_csprm.csdef;
    char *pBuf = reinterpret_cast<char *>(&def);
    if (m_bEncrypted)
    {
        for (long i=0; i<sizeof(def); i++)
        {
            pBuf[i] ^= 0xFF;
        }
        def.fill[0] = 0x01;
    }

    memcpy(pStreamOut, pBuf, sizeof(def));
    pStreamOut = pStreamOut + sizeof(def);

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.SerializeTo")
    return pStreamOut;
}

//*****************************************************************************
UINT32 CCoordinateSystem::GetSizeSerialized()
{
    //size of the structure and the verison number
    size_t size=sizeof(m_csprm.csdef)+sizeof(UINT8);
    return static_cast<UINT32>(size);
}
