//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "CoordSysUtil.h"
#include "CriticalSection.h"

#include "CoordSys.h"                       //for CCoordinateSystem
#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"              //for CCoordinateSystemEnumDatum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysDictionary.h"             //for CCoordinateSystemDictionary
#include "CoordSysDatumDictionary.h"        //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary
#include "CoordSysCategoryDictionary.h"     //for CCoordinateSystemCategoryDictionary
#include "CoordSysCatalog.h"                //for CCoordinateSystemCatalog
#include "ArbitraryCoordsys.h"              //for CCsArbitraryCoordinateSystemUtil
#include "CoordSysMeasure.h"                //for CCoordinateSystemMeasure

#include "ogr_spatialref.h"
#include "proj_api.h"
#include "cpl_csv.h"
#include "cpl_conv.h"
#include "cpl_multiproc.h"

#include "CoordSysTransformation.h"
#include "CoordSysCategoryCollection.h"
#include "CoordSysCategory.h"
#include "CoordSysInformation.h"

#ifndef _WIN32
#include <wctype.h>
#else
#pragma warning(disable: 4996)
#endif

#define EARTH_RADIUS_METERS 6378100.0
#define METERS_PER_DEGREE   111319.4908
#define SMALL_ANGLE 1e-7

using namespace CSLibrary;

// Lat Lon
const STRING CCoordinateSystem::LatLonWkt = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.0174532925199433]]";

// Coordinate system information properties
const STRING CCoordinateSystem::CoordinateSystemCode                  = L"Code";
const STRING CCoordinateSystem::CoordinateSystemDescription           = L"Description";
const STRING CCoordinateSystem::CoordinateSystemProjection            = L"Projection";
const STRING CCoordinateSystem::CoordinateSystemProjectionDescription = L"Projection Description";
const STRING CCoordinateSystem::CoordinateSystemDatum                 = L"Datum";
const STRING CCoordinateSystem::CoordinateSystemDatumDescription      = L"Datum Description";
const STRING CCoordinateSystem::CoordinateSystemEllipsoid             = L"Ellipsoid";
const STRING CCoordinateSystem::CoordinateSystemEllipsoidDescription  = L"Ellipsoid Description";

const STRING CCoordinateSystem::BaseLibrary = L"PROJ4 Coordinate System Library";


CCoordinateSystem::CCoordinateSystem(MgCoordinateSystemCatalog* pCatalog)
{
    m_ogcWkt = L"";
    m_coordinateSystemType = MgCoordinateSystemType::Unknown;
    m_units = L"";
    m_unitScale = 0.0;
    m_minX = 0.0;
    m_minY = 0.0;
    m_maxX = 0.0;
    m_maxY = 0.0;

    m_code = L"";
    m_description = L"";
    m_projection = L"";
    m_projectionDescription = L"";
    m_datum = L"";
    m_datumDescription = L"";
    m_ellipsoid = L"";
    m_ellipsoidDescription = L"";
    m_category = L"";

    m_ogrSrs = NULL;
    m_latLonSrs = NULL;
    m_transformForward = NULL;
    m_transformInverse = NULL;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
CCoordinateSystem::~CCoordinateSystem()
{
    Cleanup();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system.
///</summary>
CCoordinateSystem::CCoordinateSystem()
{
    m_ogcWkt = L"";
    m_coordinateSystemType = MgCoordinateSystemType::Unknown;
    m_units = L"";
    m_unitScale = 0.0;
    m_minX = 0.0;
    m_minY = 0.0;
    m_maxX = 0.0;
    m_maxY = 0.0;

    m_code = L"";
    m_description = L"";
    m_projection = L"";
    m_projectionDescription = L"";
    m_datum = L"";
    m_datumDescription = L"";
    m_ellipsoid = L"";
    m_ellipsoidDescription = L"";
    m_category = L"";

    m_ogrSrs = NULL;
    m_latLonSrs = NULL;
    m_transformForward = NULL;
    m_transformInverse = NULL;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns a copy of the coordinate system.
///</summary>
MgCoordinateSystem* CCoordinateSystem::CreateClone()
{
    Ptr<CCoordinateSystem> coordSys = new CCoordinateSystem();

    if (NULL == coordSys.p)
    {
        STRING message = L"Could not allocate MgCoordinateSystem.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    coordSys->m_ogrSrs = NULL;
    coordSys->m_latLonSrs = NULL;
    coordSys->m_transformForward = NULL;
    coordSys->m_transformInverse = NULL;

    coordSys->m_ogcWkt = m_ogcWkt;
    coordSys->m_coordinateSystemType = m_coordinateSystemType;

    coordSys->m_units = m_units;
    coordSys->m_unitScale = m_unitScale;
    coordSys->m_minX = m_minX;
    coordSys->m_minY = m_minY;
    coordSys->m_maxX = m_maxX;
    coordSys->m_maxY = m_maxY;

    coordSys->m_code = m_code;
    coordSys->m_description = m_description;
    coordSys->m_projection = m_projection;
    coordSys->m_projectionDescription = m_projectionDescription;
    coordSys->m_datum = m_datum;
    coordSys->m_datumDescription = m_datumDescription;
    coordSys->m_ellipsoid = m_ellipsoid;
    coordSys->m_ellipsoidDescription = m_ellipsoidDescription;
    coordSys->m_category = m_category;

    coordSys->m_ogrSrs = m_ogrSrs->Clone();

    // Check if this is a projected coordinate system
    if(coordSys->m_coordinateSystemType == MgCoordinateSystemType::Projected)
    {
        coordSys->m_latLonSrs = m_latLonSrs->Clone();

        // No datum shift is done. You must use the CoordinateSystemTransform class for that.
        coordSys->m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(coordSys->m_ogrSrs, coordSys->m_latLonSrs, false);
        if(NULL == coordSys->m_transformForward)
        {
            delete coordSys;

            STRING message = L"Could not create coordinate system forward transformation with specified coordinate systems.";
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // No datum shift is done. You must use the CoordinateSystemTransform class for that.
        coordSys->m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(coordSys->m_latLonSrs, coordSys->m_ogrSrs, false);
        if(NULL == coordSys->m_transformInverse)
        {
            delete coordSys;

            STRING message = L"Could not create coordinate system inverse transformation with specified coordinate systems.";
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CreateClone", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    return coordSys.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system to the specified OGC WKT.
///</summary>
CCoordinateSystem::CCoordinateSystem(CREFSTRING ogcWkt)
{
    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        m_latLonSrs = NULL;
        m_transformForward = NULL;
        m_transformInverse = NULL;

        m_ogcWkt = ogcWkt;

        // Check for LOCALCS
        size_t position = m_ogcWkt.find(L"LOCALCS"); // NOXLATE
        if(position != wstring::npos)
        {
            // Substitute LOCAL_CS for LOCALCS
            m_ogcWkt = m_ogcWkt.replace(position, 7, L"LOCAL_CS"); // NOXLATE
        }

        m_ogrSrs = new OGRSpatialReference();
        if(m_ogrSrs)
        {
            // Use the OGR Conversion Library to import the OGC WKT
            char* wkt = Convert_Wide_To_Ascii(m_ogcWkt.c_str());
            char* temp = wkt;
            OGRErr error = m_ogrSrs->importFromWkt(&temp);

            // Free resources
            delete [] wkt;
            wkt = NULL;

            if(OGRERR_NONE == error)
            {
                // Set the coordinate sytem type
                if(m_ogrSrs->IsLocal())
                {
                    m_coordinateSystemType = MgCoordinateSystemType::Arbitrary;
                }
                else if(m_ogrSrs->IsGeographic())
                {
                    m_coordinateSystemType = MgCoordinateSystemType::Geographic;
                }
                else
                {
                    m_coordinateSystemType = MgCoordinateSystemType::Projected;
                }

                // Initialize all of the coordinate system information

                // Set the unit scale
                char* units = NULL;
                if(m_ogrSrs->IsGeographic())
                {
                    m_unitScale = m_ogrSrs->GetAngularUnits(&units) * RAD_TO_DEG * METERS_PER_DEGREE;
                }
                else
                {
                    m_unitScale = m_ogrSrs->GetLinearUnits(&units);
                }

                // Set the units
                wchar_t* csUnits = Convert_Ascii_To_Wide(units);
                m_units = (NULL == csUnits) ? L"" : csUnits;
                delete [] csUnits;
                csUnits = NULL;

                if(m_ogrSrs->IsGeographic())
                {
                    // Set the minimum X
                    m_minX = -180.0;

                    // Set the minimum Y
                    m_minY = -90.0;

                    // Set the maximum X
                    m_maxX = 180.0;

                    // Set the maximum Y
                    m_maxY = 90.0;
                }
                else
                {
                    // Set the minimum X
                    m_minX = -10000000.0;

                    // Set the minimum Y
                    m_minY = -10000000.0;

                    // Set the maximum X
                    m_maxX = 10000000.0;

                    // Set the maximum Y
                    m_maxY = 10000000.0;
                }

                if(m_coordinateSystemType == MgCoordinateSystemType::Arbitrary)
                {
                    // Set the code
                    m_code = L"";

                    // Lookup the code in the arbitrary XY table
                    for (int j = 0; j < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; j++)
                    {
                        if(_wcsicmp(m_units.c_str(), CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].unitsCode) == 0)
                        {
                            m_code = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].code;
                            break;
                        }
                    }

                    // Set the description
                    if(m_units.length() > 0)
                    {
                        // Determine description
                        // Description  = Arbitrary X-Y Coordinate (UnitsPlural)
                        m_description = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDescription + L" (" + m_units + L")";
                    }
                    else
                    {
                        m_description = L"";
                    }

                    // Set the projection
                    m_projection = CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjection;

                    // Set the projection description
                    m_projectionDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjectionDescription;

                    // Set the datum
                    m_datum = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatum;

                    // Set the datum description
                    m_datumDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatumDescription;

                    // Set ellipsoid
                    m_ellipsoid = CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoid;

                    // Set ellipsoid description
                    m_ellipsoidDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoidDescription;

                    // Set the category
                    m_category = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCategoryDescription;
                }
                else
                {
                    // Check if this is a projected coordinate system
                    if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
                    {
                        // Create a coordinate system transformation object here so that we are not always creating one on the fly
                        m_latLonSrs = new OGRSpatialReference();
                        if(m_latLonSrs)
                        {
                            wkt = Convert_Wide_To_Ascii(LatLonWkt.c_str());
                            temp = wkt;
                            error = m_latLonSrs->importFromWkt(&temp);

                            // Free resources
                            delete [] wkt;
                            wkt = NULL;

                            if(OGRERR_NONE == error)
                            {
                                // No datum shift is done. You must use the CoordinateSystemTransform class for that.
                                m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_ogrSrs, m_latLonSrs, false);
                                if(NULL == m_transformForward)
                                {
                                    STRING message = L"Could not create coordinate system forward transformation with specified coordinate systems.";
                                    MgStringCollection arguments;
                                    arguments.Add(message);
                                    throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
                                }

                                // No datum shift is done. You must use the CoordinateSystemTransform class for that.
                                m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_latLonSrs, m_ogrSrs, false);
                                if(NULL == m_transformInverse)
                                {
                                    STRING message = L"Could not create coordinate system inverse transformation with specified coordinate systems.";
                                    MgStringCollection arguments;
                                    arguments.Add(message);
                                    throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
                                }
                            }
                            else
                            {
                                STRING message = L"Could not parse the OGC WKT.";

                                const char* errMsg = CPLGetLastErrorMsg();
                                if(errMsg)
                                {
                                    wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
                                    message += L" ";
                                    message += strError;

                                    delete [] strError;
                                    strError = NULL;
                                }

                                MgStringCollection arguments;
                                arguments.Add(message);
                                throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
                            }
                        }
                        else
                        {
                            STRING message = L"Could not allocate OGRSpatialReference.";
                            MgStringCollection arguments;
                            arguments.Add(message);
                            throw new MgOutOfMemoryException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
                        }
                    }

                    char* proj4 = NULL;
                    error = m_ogrSrs->exportToProj4(&proj4);
                    if(OGRERR_NONE == error)
                    {
                        wchar_t* csProj4 = Convert_Ascii_To_Wide(proj4);
                        STRING proj4Defn = csProj4;
                        proj4Defn = TrimRight(proj4Defn);
                        delete [] csProj4;
                        csProj4 = NULL;

                        MgCoordinateSystemFactory factory;
                        Ptr<MgCoordinateSystemCatalog> pCatalog = factory.GetCatalog();

                        CCoordinateSystemCatalog* catalog = dynamic_cast<CCoordinateSystemCatalog*>(pCatalog.p);
                        if(NULL != catalog)
                        {
                            CCoordinateSystemCategoryCollection* categories;
                            categories = catalog->GetCoordinateSystemCategories();

                            size_t size = categories->GetCount();
                            for(size_t i=0;i<size;i++)
                            {
                                CCoordinateSystemCategory* category;
                                category = categories->GetItem(i);

                                size_t coordSysIndex = category->ContainsProj4(proj4Defn);
                                if(-1 != coordSysIndex)
                                {
                                    CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystemsInfo();
                                    CCoordinateSystemInformation* coordSys = coordSystems->at(coordSysIndex);

                                    // Set the code
                                    m_code = coordSys->m_code;

                                    // Set the description
                                    m_description = coordSys->m_description;

                                    // Set the category
                                    m_category = category->GetName();
                                    break;
                                }
                            }
                        }
                    }

                    // Clean up
                    CPLFree(proj4);
                    proj4 = NULL;

                    if(OGRERR_NONE != error)
                    {
                        STRING message = L"Could not convert OGC WKT to PROJ4.";

                        const char* errMsg = CPLGetLastErrorMsg();
                        if(errMsg)
                        {
                            wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
                            message += L" ";
                            message += strError;

                            delete [] strError;
                            strError = NULL;
                        }

                        MgStringCollection arguments;
                        arguments.Add(message);
                        throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
                    }

                    // We try and pull this information out of the WKT, but they are not guaranteed to be there.

                    // Set the projection
                    const char* projection = m_ogrSrs->GetAttrValue("PROJECTION");
                    wchar_t* csProjection = Convert_Ascii_To_Wide(projection);
                    if(csProjection)
                    {
                        m_projection = csProjection;
                    }
                    else
                    {
                        m_projection = L"Unknown";
                    }
                    delete [] csProjection;
                    csProjection = NULL;

                    // Set the projection description
                    const char* projectionDescription = m_ogrSrs->GetAttrValue("PROJECTION");
                    wchar_t* csProjectionDescription = Convert_Ascii_To_Wide(projection);
                    if(csProjectionDescription)
                    {
                        m_projectionDescription = csProjectionDescription;
                    }
                    else
                    {
                        m_projectionDescription = L"Unknown";
                    }
                    delete [] csProjectionDescription;
                    csProjectionDescription = NULL;

                    // Set the datum
                    const char* datum = m_ogrSrs->GetAttrValue("DATUM");
                    wchar_t* csDatum = Convert_Ascii_To_Wide(datum);
                    if(csDatum)
                    {
                        m_datum = csDatum;
                    }
                    else
                    {
                        m_datum = L"Unknown";
                    }
                    delete [] csDatum;
                    csDatum = NULL;

                    // Set the datum description
                    const char* datumDescription = m_ogrSrs->GetAttrValue("DATUM");
                    wchar_t* csDatumDescription = Convert_Ascii_To_Wide(datumDescription);
                    if(csDatumDescription)
                    {
                        m_datumDescription = csDatumDescription;
                    }
                    else
                    {
                        m_datumDescription = L"Unknown";
                    }
                    delete [] csDatumDescription;
                    csDatumDescription = NULL;

                    // Set ellipsoid
                    const char* ellipsoid = m_ogrSrs->GetAttrValue("SPHEROID");
                    wchar_t* csEllipsoid = Convert_Ascii_To_Wide(ellipsoid);
                    if(csEllipsoid)
                    {
                        m_ellipsoid = csEllipsoid;
                    }
                    else
                    {
                        m_ellipsoid = L"Unknown";
                    }
                    delete [] csEllipsoid;
                    csEllipsoid = NULL;

                    // Set ellipsoid description
                    const char* ellipsoidDescription = m_ogrSrs->GetAttrValue("SPHEROID");
                    wchar_t* csEllipsoidDescription = Convert_Ascii_To_Wide(ellipsoidDescription);
                    if(csEllipsoidDescription)
                    {
                        m_ellipsoidDescription = csEllipsoidDescription;
                    }
                    else
                    {
                        m_ellipsoidDescription = L"Unknown";
                    }
                    delete [] csEllipsoidDescription;
                    csEllipsoidDescription = NULL;
                }
            }
            else
            {
                STRING message = L"Could not parse the OGC WKT.";

                const char* errMsg = CPLGetLastErrorMsg();
                if(errMsg)
                {
                    wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
                    message += L" ";
                    message += strError;

                    delete [] strError;
                    strError = NULL;
                }

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
        else
        {
            STRING message = L"Could not allocate OGRSpatialReference.";
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgOutOfMemoryException(L"MgCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }
    catch (MgException* e)
    {
        // we must free resources if we encountered a problem
        Cleanup();

        e->Raise();
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Frees all heap-allocated member data.
///</summary>
void CCoordinateSystem::Cleanup()
{
    if(m_ogrSrs)
    {
        delete m_ogrSrs;
        m_ogrSrs = NULL;
    }

    if(m_latLonSrs)
    {
        delete m_latLonSrs;
        m_latLonSrs = NULL;
    }

    if(m_transformForward)
    {
        delete m_transformForward;
        m_transformForward = NULL;
    }

    if(m_transformInverse)
    {
        delete m_transformInverse;
        m_transformInverse = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystem class supplies operations for working with
/// coordinates in that coordinate system, including the ability to convert
/// coordinates to and from longitude latitude values, work with linear units,
/// and compute distances.
///</summary>
INT32 CCoordinateSystem::GetType()
{
    return m_coordinateSystemType;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified longitude latitude to a coordinate in this
/// coordinate system. Note that no datum conversion is applied in
/// this conversion. Use an instance of CCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from longitude latitude to this coordinate system is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="lon">
/// The longitude value to convert.
///</param>
///<param name="lat">
/// The latitude value to convert.
///</param>
///<param name="x">
/// The converted longitude value.
///</param>
///<param name="y">
/// The converted latitude value.
///</param>
///<returns>
/// Nothing.
///</returns>
void CCoordinateSystem::ConvertFromLonLat(double lon, double lat, double& x, double& y)
{
    // Initialize
    x = 0.0;
    y = 0.0;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        // Check to see if the conversion is allowed, if not throw an exception
        double dCoords[3] = { lon, lat, 0.0 };

        // If the coordinate system is arbitrary or geographic there is no conversion
        if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
        {
            // Straight conversion no datum shift done.
            m_transformInverse->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
        }

        x = dCoords[0];
        y = dCoords[1];
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified longitude latitude to a coordinate in this
/// coordinate system. Note that no datum conversion is applied in
/// this conversion. Use an instance of CCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from longitude latitude to this coordinate system is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="lon">
/// The longitude values to convert.
///</param>
///<param name="lat">
/// The latitude values to convert.
///</param>
///<param name="x">
/// The converted longitude values.
///</param>
///<param name="y">
/// The converted latitude values.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void CCoordinateSystem::ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize)
{
    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        double dCoords[3] = { 0.0, 0.0, 0.0 };

        for(int i=0;i<arraySize;i++)
        {
            // Initialize
            x[i] = 0.0;
            y[i] = 0.0;

            // Check to see if the conversion is allowed, if not throw an exception
            dCoords[0] = lon[i];
            dCoords[1] = lat[i];

            // If the coordinate system is arbitrary or geographic there is no conversion
            if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
            {
                // Straight conversion no datum shift done.
                m_transformInverse->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
            }

            x[i] = dCoords[0];
            y[i] = dCoords[1];
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate in this coordinate system to a
/// longitude latitude value. Note that no datum conversion is applied in
/// this conversion. Use an instance of CCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from this coordinate system to longitude latitude is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="x">
/// The value to convert to a longitude value.
///</param>
///<param name="y">
/// The value to convert to a latitude value.
///</param>
///<param name="lon">
/// The longitude value.
///</param>
///<param name="lat">
/// The latitude value.
///</param>
///<returns>
/// Nothing.
///</returns>
void CCoordinateSystem::ConvertToLonLat(double x, double y, double& lon, double& lat)
{
    // Initialize
    lat = 0.0;
    lon = 0.0;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        // Check to see if the conversion is allowed, if not throw an exception
        double dCoords[3] = { x, y, 0.0 };

        // If the coordinate system is arbitrary or geographic there is no conversion
        if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
        {
            // Straight conversion no datum shift done.
            m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
        }

        lon = dCoords[0];
        lat = dCoords[1];
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate in this coordinate system to a
/// longitude latitude value. Note that no datum conversion is applied in
/// this conversion. Use an instance of CCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from this coordinate system to longitude latitude is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="x">
/// The values to convert to a longitude values.
///</param>
///<param name="y">
/// The values to convert to a latitude values.
///</param>
///<param name="lon">
/// The longitude values.
///</param>
///<param name="lat">
/// The latitude values.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void CCoordinateSystem::ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize)
{
    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        double dCoords[3] = { 0.0, 0.0, 0.0 };

        for(int i=0;i<arraySize;i++)
        {
            // Initialize
            lon[i] = 0.0;
            lat[i] = 0.0;

            // Check to see if the conversion is allowed, if not throw an exception
            dCoords[0] = x[i];
            dCoords[1] = y[i];

            // If the coordinate system is arbitrary or geographic there is no conversion
            if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
            {
                // Straight conversion no datum shift done.
                m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
            }

            lon[i] = dCoords[0];
            lat[i] = dCoords[1];
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
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

    try
    {
        // Convert to meters
        if(m_ogrSrs->IsGeographic())
        {
            meters = METERS_PER_DEGREE * units;
        }
        else
        {
            meters = m_ogrSrs->GetLinearUnits() * units;
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemUnitsToMeters", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

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

    try
    {
        double linearUnits = 1.0;

        // This may throw a divide by zero exception
        if(m_ogrSrs->IsGeographic())
        {
            linearUnits = METERS_PER_DEGREE;
        }
        else
        {
            linearUnits = m_ogrSrs->GetLinearUnits();
        }

        // This may throw a divide by zero exception
        units = meters / linearUnits;
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertMetersToCoordinateSystemUnits", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return units;
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

    try
    {
        distance = ::sqrt(::pow(x1 - x2, 2.0) + ::pow(y1 - y2, 2.0));
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Measures the distance between two coordinates.
/// This function uses the coordinate systems ellipsoid
/// definition and a great circle algorithm to produce the result.
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
/// The great circle distance from coord1 to coord2 in coordinate system units.
///</returns>
double CCoordinateSystem::MeasureGreatCircleDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    // This coordinate system must not be an arbitrary XY coordinate system,
    // and there must be a Great Circle path between the points.
    if(m_coordinateSystemType == MgCoordinateSystemType::Arbitrary)
    {
        STRING message = L"MeasureGreatCircleDistance does not work with Arbitrary coordinate systems.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgInvalidCoordinateSystemTypeException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        // Calculate Great Circle distance using the following formula:
        //
        // Presuming a spherical Earth with radius R (6378100.0 m), and that the
        // locations of the two points in spherical coordinates (longitude and
        // latitude) are lon1,lat1 and lon2,lat2, then the Haversine Formula
        // (from R. W. Sinnott, "Virtues of the Haversine," Sky and Telescope,
        // vol. 68, no. 2, 1984, p. 159):
        //
        // dlon = lon2 - lon1
        // dlat = lat2 - lat1
        // a = (sin(dlat/2))^2 + cos(lat1) * cos(lat2) * (sin(dlon/2))^2
        // c = 2 * atan2(sqrt(a), sqrt(1-a))
        // d = R * c
        //
        // will give mathematically and computationally exact results. The
        // intermediate result c is the great circle distance in radians. The
        // great circle distance d will be in the same units as R.

        double lon1 = 0.0;
        double lat1 = 0.0;
        double lon2 = 0.0;
        double lat2 = 0.0;

        if(m_coordinateSystemType == MgCoordinateSystemType::Geographic)
        {
            lon1 = x1;
            lat1 = y1;
            lon2 = x2;
            lat2 = y2;
        }
        else if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
        {
            // convert from MCS to lon/lat
            double dCoords[3] = { x1, y1, 0.0 };

            // Convert 1st coordinate
            m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
            lon1 = dCoords[0];
            lat1 = dCoords[1];

            // Convert 2nd coordinate
            dCoords[0] = x2;
            dCoords[1] = y2;
            dCoords[2] = 0.0;

            m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
            lon2 = dCoords[0];
            lat2 = dCoords[1];
        }

        // Convert degrees to radians
        double lon1Rads = lon1 * DEG_TO_RAD;
        double lat1Rads = lat1 * DEG_TO_RAD;
        double lon2Rads = lon2 * DEG_TO_RAD;
        double lat2Rads = lat2 * DEG_TO_RAD;

        double dLon = lon2Rads - lon1Rads;
        double dLat = lat2Rads - lat1Rads;
        double a = ::pow(::sin(dLat/2.0), 2.0) + ::cos(lat1Rads) * cos(lat2Rads) * ::pow(::sin(dLon/2.0), 2.0);
        double c = 2.0 * ::atan2(::sqrt(a), sqrt(1.0-a));
        distance = EARTH_RADIUS_METERS / m_unitScale * c ;

        if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
        {
            // Convert the result in meters to native units
            distance = ConvertMetersToCoordinateSystemUnits(distance);
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="x1">
/// The x value that defines the first coordinate.
///</param>
///<param name="y1">
/// The y value that defines the first coordinate.
///</param>
///<param name="x2">
/// The x value that defines the second coordinate.
///</param>
///<param name="y2">
/// The y value that defines the second coordinate.
///</param>
///<returns>
/// The azimuth (Angle with respect to the North) of the vector formed by
/// coord1 and coord2.
///</returns>
double CCoordinateSystem::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double azimuth = 0.0;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        if(m_coordinateSystemType == MgCoordinateSystemType::Arbitrary)
        {
            double xDistance = x2 - x1;
            double yDistance = y2 - y1;

            // Convert azimuth to radians
            azimuth = ::atan(::fabs(yDistance/xDistance)) * RAD_TO_DEG;

            if(xDistance < 0.0)
            {
                azimuth = 180.0 - azimuth;
            }

            if(yDistance < 0.0)
            {
                azimuth *= -1.0;
            }
        }
        else
        {
            double lon1 = 0.0;
            double lat1 = 0.0;
            double lon2 = 0.0;
            double lat2 = 0.0;

            if(m_coordinateSystemType == MgCoordinateSystemType::Geographic)
            {
                lon1 = x1;
                lat1 = y1;
                lon2 = x2;
                lat2 = y2;
            }
            else if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
            {
                // convert from MCS to lon/lat
                double dCoords[3] = { x1, y1, 0.0 };

                // Convert 1st coordinate
                m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
                lon1 = dCoords[0];
                lat1 = dCoords[1];

                // Convert 2nd coordinate
                dCoords[0] = x2;
                dCoords[1] = y2;
                dCoords[2] = 0.0;

                m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
                lon2 = dCoords[0];
                lat2 = dCoords[1];
            }

            // Convert degrees to radians
            double lon1Rads = lon1 * DEG_TO_RAD;
            double lat1Rads = lat1 * DEG_TO_RAD;
            double lon2Rads = lon2 * DEG_TO_RAD;
            double lat2Rads = lat2 * DEG_TO_RAD;

            double deltax = fabs(lon2Rads-lon1Rads);
            double deltay = fabs(lat2Rads-lat1Rads);

            if (deltax < SMALL_ANGLE && deltay < SMALL_ANGLE)
            {
                //use small angle formula in case points are really close
                //formula is the same as the general case but small angle formulae are used
                //For small angles: sin(delta) ~ delta and cos(delta) ~ 1 - 0.5 * (delta)^2

                double delta = lon2Rads-lon1Rads;

                double sinAsinDist = delta * cos(lat2Rads);

                double cosAsinDist = cos(lat1Rads) * sin(lat2Rads) - sin(lat1Rads) * cos(lat2Rads);

                //only this term really contributes to cosAsinDist, the part above is
                //usually 0 or very very close to 0
                cosAsinDist += sin(lat1Rads) * cos(lat2Rads) * delta * delta / 2.0;

                azimuth = RAD_TO_DEG*::atan2(sinAsinDist, cosAsinDist);
            }
            else
            {
                double angularDistance = ::acos(::sin(lat1Rads)*::sin(lat2Rads) + ::cos(lat1Rads)*::cos(lat2Rads)*::cos(lon2Rads-lon1Rads));
                double cosAzimuth = (::cos(lat1Rads)*::sin(lat2Rads) - ::sin(lat1Rads)*::cos(lat2Rads)*::cos(lon2Rads-lon1Rads)) / ::sin(angularDistance);
                double sinAzimuth = ::cos(lat2Rads)*::sin(lon2Rads-lon1Rads) / ::sin(angularDistance);

                azimuth = RAD_TO_DEG*::atan2(sinAzimuth, cosAzimuth);
            }
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return azimuth;
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
void CCoordinateSystem::GetCoordinate(double xStart, double yStart, double azimuth, double distance, double&x, double& y)
{
    x = 0.0;
    y = 0.0;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        if(m_coordinateSystemType == MgCoordinateSystemType::Arbitrary)
        {
            // Convert azimuth to radians
            double azimuthRads = azimuth * DEG_TO_RAD;

            x = xStart + ::cos(azimuthRads) * distance;
            y = yStart + ::sin(azimuthRads) * distance;
        }
        else
        {
            double lon = 0.0;
            double lat = 0.0;
            double distanceRads = 0.0;

            if(m_coordinateSystemType == MgCoordinateSystemType::Geographic)
            {
                lon = xStart;
                lat = yStart;

                STRING units = L"";
                for(size_t i=0;i<m_units.length();i++)
                {
                    units += towupper(m_units[i]);
                }

                // Compare with upper case strings
                if(units.find(L"RADIAN") !=  STRING::npos)
                {
                    // Units are radians
                    distanceRads = distance;
                }
                else
                {
                    distanceRads = distance * m_unitScale / EARTH_RADIUS_METERS;
                }
            }
            else if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
            {
                // convert from MCS to lon/lat
                double dCoords[3] = { xStart, yStart, 0.0 };

                // Convert 1st coordinate
                m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
                lon = dCoords[0];
                lat = dCoords[1];

                distanceRads = ConvertCoordinateSystemUnitsToMeters(distance) * (m_unitScale / EARTH_RADIUS_METERS);
            }

            // Convert degrees to radians
            double lonRads = lon * DEG_TO_RAD;
            double latRads = lat * DEG_TO_RAD;
            double azimuthRads = azimuth * DEG_TO_RAD;

            y = ::asin(::cos(azimuthRads)*::sin(distanceRads)*::cos(latRads) + ::cos(distanceRads)*::sin(latRads));

            double sinLon = ::sin(azimuthRads)*::sin(distanceRads) / ::cos(y);
            double cosLon = (::cos(distanceRads)- ::sin(latRads)*::sin(y)) / (::cos(latRads)*::cos(y));

            // Convert to degrees
            // The new coordinate x should be between -180 and 180
            x = lon + RAD_TO_DEG*::atan2(sinLon, cosLon);
            if (x <= -180.0)
            {
                x += 360.0;
            }
            if (x > 180.0)
            {
                x-= 360.0;
            }

            y *= RAD_TO_DEG;

            if(m_coordinateSystemType == MgCoordinateSystemType::Projected)
            {
                // convert from lon/lat to MCS
                double dCoords[3] = { x, y, 0.0 };

                // Convert coordinate
                m_transformInverse->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
                x = dCoords[0];
                y = dCoords[1];
            }
        }
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified wkt string into the corresponding coordinate
/// system code.
///</summary>
///<param name="ogcWkt">
/// The wkt string to convert.
///</param>
///<returns>
/// String of the corresponding coordinate system code
///</returns>
STRING CCoordinateSystem::ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt)
{
    STRING code;
    OGRErr error = OGRERR_NONE;

    char* wkt = NULL;
    wchar_t* csUnits = NULL;
    char* proj4 = NULL;
    wchar_t* csProj4 = NULL;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        OGRSpatialReference ogrSrs;

        // Use the OGR Conversion Library to import the OGC WKT
        wkt = Convert_Wide_To_Ascii(ogcWkt.c_str());
        char* temp = wkt;
        error = ogrSrs.importFromWkt(&temp);

        // Clean up
        delete [] wkt;
        wkt = NULL;

        if(OGRERR_NONE == error)
        {
            if(ogrSrs.IsLocal())
            {
                // This is an Arbitrary XY coordinate system
                char* unit = NULL;
                ogrSrs.GetLinearUnits(&unit);
                csUnits = Convert_Ascii_To_Wide(unit);

                // Lookup units and find matching code
                for (int i = 0; i < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; i++)
                {
                    if(_wcsicmp(csUnits, CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].unitsCode) == 0)
                    {
                        code = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].code;
                        break;
                    }
                }

                delete [] csUnits;
                csUnits = NULL;
            }
            else
            {
                error = ogrSrs.exportToProj4(&proj4);
                if(OGRERR_NONE == error)
                {
                    if(proj4)
                    {
                        csProj4 = Convert_Ascii_To_Wide(proj4);
                        STRING proj4Defn = csProj4;
                        proj4Defn = TrimRight(proj4Defn);

                        delete [] csProj4;
                        csProj4 = NULL;

                        MgCoordinateSystemFactory factory;
                        Ptr<MgCoordinateSystemCatalog> catalog = factory.GetCatalog();

                        CCoordinateSystemCatalog* pCatalog = dynamic_cast<CCoordinateSystemCatalog*>(catalog.p);
                        if(NULL != pCatalog)
                        {
                            CCoordinateSystemCategoryCollection* categories;
                            categories = pCatalog->GetCoordinateSystemCategories();

                            size_t size = categories->GetCount();
                            for(size_t i=0;i<size;i++)
                            {
                                CCoordinateSystemCategory* category;
                                category = categories->GetItem(i);

                                size_t coordSysIndex = category->ContainsProj4(proj4Defn);
                                if(-1 != coordSysIndex)
                                {
                                    CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystemsInfo();
                                    CCoordinateSystemInformation* coordSys = coordSystems->at(coordSysIndex);
                                    if(coordSys)
                                    {
                                        // Set the code
                                        code = coordSys->m_code;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }

                // Clean up
                CPLFree(proj4);
                proj4 = NULL;
            }
        }
    }
    catch(...)
    {
        // Clean up
        if(wkt)
        {
            delete [] wkt;
            wkt = NULL;
        }

        if(csUnits)
        {
            delete [] csUnits;
            csUnits = NULL;
        }

        if(csProj4)
        {
            delete [] csProj4;
            csProj4 = NULL;
        }

        if(proj4)
        {
            CPLFree(proj4);
            proj4 = NULL;
        }

        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(error != OGRERR_NONE)
    {
        STRING message = L"Could not determine code because of internal OGR error.";

        const char* errMsg = CPLGetLastErrorMsg();
        if(errMsg)
        {
            wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
            message += L" ";
            message += strError;

            delete [] strError;
            strError = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(code.empty())
    {
        STRING message = L"Could not determine OGC WKT from the code.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return code;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate system code into the corresponding
/// wkt string.
///</summary>
///<param name="code">
/// The coordinate system code string to convert.
///</param>
///<returns>
/// String of the corresponding wkt
///</returns>
STRING CCoordinateSystem::ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode)
{
    STRING ogcWkt;
    OGRErr error = OGRERR_NONE;

    wchar_t* wBuffer = NULL;
    char* proj4Defn = NULL;
    wchar_t* csWkt = NULL;
    char* wkt = NULL;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        // Make code uppercase
        STRING ucCode = ToUpper(csCode);

        if((_wcsnicmp(ucCode.c_str(), L"*XY-", 4) == 0) ||  // NOXLATE
           (_wcsnicmp(ucCode.c_str(), L"XY-", 3) == 0))     // NOXLATE
        {
            STRING updatedCsCode = ucCode;

            size_t position = updatedCsCode.find(L"*XY-"); // NOXLATE
            if(position != wstring::npos)
            {
                updatedCsCode = updatedCsCode.replace(position, 4, L"XY-"); // NOXLATE

                position = updatedCsCode.find(L"*"); // NOXLATE
                if(position != wstring::npos)
                {
                    updatedCsCode = updatedCsCode.replace(position, 1, L""); // NOXLATE
                }
            }

            // This is an arbitrary XY system
            STRING units = L"";
            double unitsValue = 1.0;

            // Lookup the code in the arbitrary XY table and return the Ogc WKT
            for (int j = 0; j < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; j++)
            {
                if(_wcsicmp(updatedCsCode.c_str(), CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].code) == 0)
                {
                    units = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].unitsCode;
                    unitsValue = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[j].conversionToMeters;
                    break;
                }
            }

            if(units.length() > 0)
            {
                STRING unitsValueString;

                char buffer[255] = { 0 };
                sprintf(buffer, "%.10g", unitsValue);

                wBuffer = Convert_Ascii_To_Wide(buffer);
                if(wBuffer)
                {
                    unitsValueString = wBuffer;
                    delete [] wBuffer;
                    wBuffer = NULL;
                }
                else
                {
                    unitsValueString = L"1.0";
                }

                STRING datum;
                datum = L"LOCAL_DATUM[\"Local Datum\",0]";

                STRING axisX;
                axisX = L"AXIS[\"X\",EAST]";

                STRING axisY;
                axisY = L"AXIS[\"Y\",NORTH]";

                ogcWkt = L"LOCAL_CS";
                ogcWkt += L"[\"Non-Earth (";
                ogcWkt += units;
                ogcWkt += L")\",";
                ogcWkt += datum;
                ogcWkt += L",UNIT[\"";
                ogcWkt += units;
                ogcWkt += L"\", ";
                ogcWkt += unitsValueString;
                ogcWkt += L"],";
                ogcWkt += axisX;
                ogcWkt += L",";
                ogcWkt += axisY;
                ogcWkt += L"]";
            }
        }
        else
        {
            MgCoordinateSystemFactory factory;
            Ptr<MgCoordinateSystemCatalog> catalog = factory.GetCatalog();

            CCoordinateSystemCatalog* pCatalog = dynamic_cast<CCoordinateSystemCatalog*>(catalog.p);
            if(NULL != pCatalog)
            {
                CCoordinateSystemCategoryCollection* categories;
                categories = pCatalog->GetCoordinateSystemCategories();

                size_t size = categories->GetCount();
                for(size_t i=0;i<size;i++)
                {
                    CCoordinateSystemCategory* category;
                    category = categories->GetItem(i);

                    size_t coordSysIndex = category->ContainsCode(csCode);
                    if(-1 != coordSysIndex)
                    {
                        CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystemsInfo();
                        CCoordinateSystemInformation* coordSys = coordSystems->at(coordSysIndex);
                        if(coordSys)
                        {
                            OGRSpatialReference ogrSrs;

                            proj4Defn = Convert_Wide_To_Ascii(coordSys->m_proj4Definition.c_str());
                            if(proj4Defn)
                            {
                                error = ogrSrs.importFromProj4(proj4Defn);
                                if(OGRERR_NONE == error)
                                {
                                    error = ogrSrs.exportToWkt(&wkt);
                                    if(OGRERR_NONE == error)
                                    {
                                        if(wkt)
                                        {
                                            if(strlen(wkt) > 0)
                                            {
                                                csWkt = Convert_Ascii_To_Wide(wkt);
                                                if(csWkt)
                                                {
                                                    ogcWkt = csWkt;
                                                    delete [] csWkt;
                                                    csWkt = NULL;
                                                }
                                            }
                                        }
                                    }

                                    CPLFree(wkt);
                                    wkt = NULL;
                                }

                                delete [] proj4Defn;
                                proj4Defn = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    catch(...)
    {
        if(wBuffer)
        {
            delete [] wBuffer;
            wBuffer = NULL;
        }

        if(proj4Defn)
        {
            delete [] proj4Defn;
            proj4Defn = NULL;
        }

        if(csWkt)
        {
            delete [] csWkt;
            csWkt = NULL;
        }

        if(wkt)
        {
            CPLFree(wkt);
            wkt = NULL;
        }

        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(error != OGRERR_NONE)
    {
        STRING message = L"Unsupported coordinate system code: ";

        const char* errMsg = CPLGetLastErrorMsg();
        if(errMsg)
        {
            wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
            message += csCode;
            message += L". ";
            message += strError;

            delete [] strError;
            strError = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(ogcWkt.empty())
    {
        STRING message = L"Unsupported coordinate system code.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return ogcWkt;
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
    return m_ogcWkt;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units.
/// </summary>
STRING CCoordinateSystem::GetUnits()
{
    return m_units;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units scale.
/// </summary>
double CCoordinateSystem::GetUnitScale()
{
    return m_unitScale;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum X value.
/// </summary>
double CCoordinateSystem::GetMinX()
{
    return m_minX;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum Y value.
/// </summary>
double CCoordinateSystem::GetMinY()
{
    return m_minY;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum X value.
/// </summary>
double CCoordinateSystem::GetMaxX()
{
    return m_maxX;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum Y value.
/// </summary>
double CCoordinateSystem::GetMaxY()
{
    return m_maxY;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system code.
/// </summary>
STRING CCoordinateSystem::GetCode()
{
    return m_code;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system description.
/// </summary>
STRING CCoordinateSystem::GetDescription()
{
    return m_description;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system projection.
/// </summary>
STRING CCoordinateSystem::GetProjection()
{
    return m_projection;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system projection description.
/// </summary>
STRING CCoordinateSystem::GetProjectionDescription()
{
    return m_projectionDescription;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system datum.
/// </summary>
STRING CCoordinateSystem::GetDatum()
{
    return m_datum;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system datum description.
/// </summary>
STRING CCoordinateSystem::GetDatumDescription()
{
    return m_datumDescription;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system ellipsoid.
/// </summary>
STRING CCoordinateSystem::GetEllipsoid()
{
    return m_ellipsoid;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system ellipsoid description.
/// </summary>
STRING CCoordinateSystem::GetEllipsoidDescription()
{
    return m_ellipsoidDescription;
}

STRING CCoordinateSystem::GetBaseLibrary()
{
    return CCoordinateSystem::BaseLibrary;
}

STRING CCoordinateSystem::ConvertEpsgCodeToWkt(long code)
{
    STRING wkt;
    OGRErr error = OGRERR_NONE;
    char* epsgWkt = NULL;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        OGRSpatialReference ogrSrs;

        // Use the OGR Conversion Library to import the EPSG code
        error = ogrSrs.importFromEPSG(code);
        if(OGRERR_NONE == error)
        {
            error = ogrSrs.exportToWkt(&epsgWkt);
            if(OGRERR_NONE == error)
            {
                if(epsgWkt)
                {
                    if(strlen(epsgWkt) > 0)
                    {
                        wchar_t* csWkt = Convert_Ascii_To_Wide(epsgWkt);
                        if(csWkt)
                        {
                            wkt = csWkt;
                            delete [] csWkt;
                            csWkt = NULL;
                        }
                    }
                }
            }
        }
    }
    catch(MgException* e)
    {
        if(epsgWkt)
        {
            CPLFree(epsgWkt);
            epsgWkt = NULL;
        }

        e->Raise();
    }
    catch(...)
    {
        if(epsgWkt)
        {
            CPLFree(epsgWkt);
            epsgWkt = NULL;
        }

        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(epsgWkt)
    {
        CPLFree(epsgWkt);
        epsgWkt = NULL;
    }

    if(OGRERR_NONE != error)
    {
        const char* errMsg = CPLGetLastErrorMsg();

        char buffer[255] = { 0 };
        #ifdef _WIN32
        itoa(code, buffer, 10);
        #else
        snprintf(buffer, 255, "%d", code);
        #endif

        wchar_t* strCode = Convert_Ascii_To_Wide(buffer);

        STRING message = L"Failed to convert EPSG code \"";
        message += strCode;
        message += L"\" to WKT.";

        delete [] strCode;
        strCode = NULL;

        if(errMsg)
        {
            wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
            message += L" ";
            message += strError;

            delete [] strError;
            strError = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertEpsgCodeToWkt", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return wkt;
}

long CCoordinateSystem::ConvertWktToEpsgCode(CREFSTRING wkt)
{
    long code;
    OGRErr error = OGRERR_NONE;

    // Lock all threads
    AutoCriticalClass acc;

    try
    {
        OGRSpatialReference ogrSrs;

        // Use the OGR Conversion Library to import the OGC WKT
        char* epsgWkt = Convert_Wide_To_Ascii(wkt.c_str());
        char* temp = epsgWkt;
        error = ogrSrs.importFromWkt(&temp);

        // Free resources
        delete [] epsgWkt;
        epsgWkt = NULL;

        if(OGRERR_NONE == error)
        {
            const char* authroityCode = NULL;

            if(ogrSrs.IsGeographic())
            {
                authroityCode = ogrSrs.GetAuthorityCode("GEOGCS");
            }
            else if(ogrSrs.IsProjected())
            {
                authroityCode = ogrSrs.GetAuthorityCode("PROJCS");
            }
            else if(ogrSrs.IsLocal())
            {
                authroityCode = ogrSrs.GetAuthorityCode("LOCAL_CS");
            }

            if(authroityCode)
            {
                code = ::atoi(authroityCode);
                if(code == 0)
                {
                    STRING message = L"Failed to convert WKT to EPSG code.";
                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToEpsgCode", __LINE__, __WFILE__, &arguments, L"", NULL);
                }
            }
            else
            {
                STRING message = L"Failed to convert WKT to EPSG code.";
                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToEpsgCode", __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToEpsgCode", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if(OGRERR_NONE != error)
    {
        const char* errMsg = CPLGetLastErrorMsg();

        char buffer[255] = { 0 };
        #ifdef _WIN32
        itoa(code, buffer, 10);
        #else
        snprintf(buffer, 255, "%d", code);
        #endif
        wchar_t* strCode = Convert_Ascii_To_Wide(buffer);

        STRING message = L"Failed to convert WKT to EPSG code.";

        delete [] strCode;
        strCode = NULL;

        if(errMsg)
        {
            wchar_t* strError = Convert_Ascii_To_Wide(errMsg);
            message += L" ";
            message += strError;

            delete [] strError;
            strError = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToEpsgCode", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return code;
}

MgCoordinate* CCoordinateSystem::ConvertFromLonLat(MgCoordinate* lonLat)
{
    Ptr<MgCoordinateXY> coord = new MgCoordinateXY(0.0, 0.0);

    if (NULL == lonLat)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.ConvertFromLonLat",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double x,y = 0.0;

        ConvertFromLonLat(lonLat->GetX(), lonLat->GetY(), x, y);
        coord->SetX(x);
        coord->SetY(y);
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.ConvertFromLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return coord.Detach();
}

MgCoordinate* CCoordinateSystem::ConvertToLonLat(MgCoordinate* coordinate)
{
    Ptr<MgCoordinateXY> coord = new MgCoordinateXY(0.0, 0.0);

    if (NULL == coordinate)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.ConvertToLonLat",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double lon,lat = 0.0;

        ConvertToLonLat(coordinate->GetX(), coordinate->GetY(), lon, lat);
        coord->SetX(lon);
        coord->SetY(lat);
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.ConvertToLonLat", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return coord.Detach();
}

double CCoordinateSystem::MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;

    if (NULL == coord1 || NULL == coord2)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.MeasureEuclideanDistance",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        distance = MeasureEuclideanDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.MeasureEuclideanDistance", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return distance;
}

double CCoordinateSystem::MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;

    if (NULL == coord1 || NULL == coord2)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.MeasureGreatCircleDistance",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        distance = MeasureGreatCircleDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.MeasureGreatCircleDistance", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return distance;
}

double CCoordinateSystem::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double azimuth = 0.0;

    if (NULL == coord1 || NULL == coord2)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.GetAzimuth",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        azimuth = GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        STRING message = L"Unexpected error.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.GetAzimuth", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return azimuth;
}

MgCoordinate* CCoordinateSystem::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    if (NULL == coord)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystem.GetCoordinate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance);
}

MgCoordinate* CCoordinateSystem::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    Ptr<MgCoordinate> pCoordinate = new MgCoordinateXY(0.0, 0.0);

    double x, y = 0.0;
    GetCoordinate(xStart, yStart, azimuth, distance, x, y);
    pCoordinate->SetX(x);
    pCoordinate->SetY(y);

    return pCoordinate.Detach();
}

MgCoordinateSystemMeasure* CCoordinateSystem::GetMeasure()
{
    MgCoordinateSystemMeasure* measure = new CCoordinateSystemMeasure(this);
    return measure;
}

MgStringCollection* CCoordinateSystem::GetCategories()
{
    Ptr<MgStringCollection> pCategoryNames;

    MG_TRY()

    pCategoryNames = new MgStringCollection();
    if (!pCategoryNames)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.GetCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pCategoryNames->Add(m_category);

    MG_CATCH_AND_THROW(L"MgCoordinateSystem.GetCategories")

    return pCategoryNames.Detach();
}

MgCoordinateSystemCatalog* CCoordinateSystem::GetCatalog()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetCode(CREFSTRING sCode)
{
    m_code = sCode;
}

bool CCoordinateSystem::IsLegalCode(CREFSTRING sCode)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsValid()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsUsable(MgCoordinateSystemCatalog *pCatalog)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsUsable", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsSameAs(MgGuardDisposable *pDef)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetDescription(CREFSTRING sDesc)
{
    m_description = sDesc;
}

bool CCoordinateSystem::IsLegalDescription(CREFSTRING sDesc)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystem::GetGroup()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsLegalGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystem::GetSource()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsLegalSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsProtected()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsProtected", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT16 CCoordinateSystem::GetAge()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetAge", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetProtectMode(bool bIsProtected)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetProtectMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsEncrypted()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsEncrypted", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetEncryptMode(bool bIsEncrypted)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetEncryptMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemEnumInteger32* CCoordinateSystem::GetErrors()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetErrors", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystem::GetLocation()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetLocation(CREFSTRING sLoc)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsLegalLocation(CREFSTRING sLoc)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystem::GetCountryOrState()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetCountryOrState(CREFSTRING sCountryOrState)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsLegalCountryOrState(CREFSTRING sCountryOrState)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsLegalCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsGeodetic()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsGeodetic", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT32 CCoordinateSystem::GetProjectionCode()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetProjectionCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetProjectionCode(INT32  prjType)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetProjectionCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT32 CCoordinateSystem::GetUnitCode()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetUnitCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetUnitCode(INT32 unit)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetUnitCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT32 CCoordinateSystem::GetProjectionParameterCount()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetProjectionParameterCount", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetProjectionParameter(INT32 nIndex)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetProjectionParameter", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetProjectionParameter(INT32 nIndex, double dValue)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetProjectionParameter", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetOriginLongitude()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetOriginLongitude", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetOriginLongitude(double dOrgLng)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetOriginLongitude", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetOriginLatitude()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetOriginLatitude", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetOriginLatitude(double dOrgLat)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetOriginLatitude", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetOffsetX()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetOffsetX", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetOffsetY()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetOffsetY", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetOffsets(double dXOffset, double dYOffset)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetOffsets", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetScaleReduction()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetScaleReduction", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetScaleReduction(double dSclRed)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetScaleReduction", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetMapScale()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetMapScale", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetMapScale(double dMapScale)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetMapScale", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetZeroX()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetZeroX", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetZeroY()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetZeroY", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetZeroes(double dXZero, double dYZero)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetZeroes", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetLonMin()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetLonMin", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetLonMax()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetLonMax", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetLatMin()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetLatMin", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetLatMax()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetLatMax", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetLonLatBounds(double dLonMin, double dLatMin, double dLonMax, double dLatMax)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetLonLatBounds", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::CancelLonLatBounds()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.CancelLonLatBounds", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetXYBounds(double dXMin, double dYMin, double dXMax, double dYMax)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetXYBounds", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::CancelXYBounds()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.CancelXYBounds", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT16 CCoordinateSystem::GetQuadrant()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetQuadrant", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetQuadrant(INT16 sQuad)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetQuadrant", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemDatum* CCoordinateSystem::GetDatumDefinition()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetDatumDefinition(MgCoordinateSystemDatum *pDatum)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetDatumDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemEllipsoid* CCoordinateSystem::GetEllipsoidDefinition()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoid)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsValidXY(double dX, double dY)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsValidXY", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystem::IsValidLonLat(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.IsValidLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::ConvertCoordinateFromLonLat(MgCoordinate* lonLatToCoordinate)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertCoordinateFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystem::ConvertCoordinateToLonLat(MgCoordinate* coordinateToLonLat)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertCoordinateToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinate* CCoordinateSystem::ConvertToLonLat(double dX, double dY)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinate* CCoordinateSystem::ConvertToLonLat(double dX, double dY, double dZ)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinate* CCoordinateSystem::ConvertFromLonLat(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinate* CCoordinateSystem::ConvertFromLonLat(double dLongitude, double dLatitude, double dZ)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetConvergence(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetConvergence", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetScale(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetScale", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetScaleH(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetScaleH", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystem::GetScaleK(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetScaleK", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT8* CCoordinateSystem::SerializeFrom(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT8* CCoordinateSystem::SerializeTo(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT32 CCoordinateSystem::GetSizeSerialized()
{
    throw new MgNotImplementedException(L"MgCoordinateSystem.GetSizeSerialized", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgDisposable
void CCoordinateSystem::Dispose()
{
    delete this;
}
