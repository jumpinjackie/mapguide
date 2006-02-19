//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "float.h"
#include "CriticalSection.h"

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"
#include "CoordSysUtil.h"
#include "cpl_csv.h"
#include "cpl_conv.h"
#include "cpl_multiproc.h"

#ifndef _WIN32
#include <wctype.h>
#endif

#define EARTH_RADIUS_METERS 6378100.0
#define METERS_PER_DEGREE   111319.4908


// Dummy class used to automate initialization/uninitialization of CPL.
class CInitCPL
{
public:
    CInitCPL()
    {
        // Initialize the critical section class
        CriticalClass.Initialize();
    }
    ~CInitCPL()
    {
        // free PROJ4 resources
        pj_deallocate_grids();

        // free CPL resources
        CSVDeaccess(NULL);
        CPLFinderClean();
        CPLCleanupTLS();

        // free CoordinateSystem resources
        CCoordinateSystem::DeleteCatalog();
    }
};

static CInitCPL s_InitCPL;
CCoordinateSystemCatalog* CCoordinateSystem::m_catalog = NULL;

// Lat Lon
const STRING CCoordinateSystem::LatLonWkt = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.0174532925199433]]";

// Arbitrary X-Y Coordinate Support
const STRING CCoordinateSystem::ArbitraryXYCategoryDescription   = L"Arbitrary X-Y Coordinate Systems";
const STRING CCoordinateSystem::ArbitraryXYDescription           = L"Arbitrary X-Y Coordinates";
const STRING CCoordinateSystem::ArbitraryXYProjection            = L"*X-Y*";  // NOXLATE
const STRING CCoordinateSystem::ArbitraryXYProjectionDescription = L"Arbitrary X-Y Coordinate Projection";
const STRING CCoordinateSystem::ArbitraryXYDatum                 = L"";
const STRING CCoordinateSystem::ArbitraryXYDatumDescription      = L"No Datum Used";
const STRING CCoordinateSystem::ArbitraryXYEllipsoid             = L"";
const STRING CCoordinateSystem::ArbitraryXYEllipsoidDescription  = L"No Ellipsoid Used";
const int CCoordinateSystem::ArbitraryXYCoordinateSystemsCount = 39;
const ArbitraryCoordinateSystem CCoordinateSystem::ArbitraryXYCoordinateSystems[CCoordinateSystem::ArbitraryXYCoordinateSystemsCount] =
{
    {L"*XY-M*",   L"Meter",            1.0},                        // NOXLATE
    {L"*XY-FT*",  L"Foot",             0.30480060960121920243},     // NOXLATE
    {L"*XY-IN*",  L"Inch",             0.0254000508001016002},      // NOXLATE
    {L"*XY-IFT*", L"IFoot",            0.3048},                     // NOXLATE
    {L"*XY-CF*",  L"ClarkeFoot",       0.3047972651151},            // NOXLATE
    {L"*XY-IIN*", L"IInch",            0.0254},                     // NOXLATE
    {L"*XY-UI*",  L"MicroInch",        0.0000254},                  // NOXLATE
    {L"*XY-ML*",  L"Mil",              0.0000000254},               // NOXLATE
    {L"*XY-CM*",  L"Centimeter",       0.01},                       // NOXLATE
    {L"*XY-KM*",  L"Kilometer",     1000.0},                        // NOXLATE
    {L"*XY-YD*",  L"Yard",             0.91440182880365760731},     // NOXLATE
    {L"*XY-SY*",  L"SearsYard",        0.914398414616029},          // NOXLATE
    {L"*XY-MI*",  L"Mile",          1609.34721869443738887477},     // NOXLATE
    {L"*XY-IYD*", L"IYard",            0.9144},                     // NOXLATE
    {L"*XY-IMI*", L"IMile",         1609.344},                      // NOXLATE
    {L"*XY-KT*",  L"Knot",          1852.0},                        // NOXLATE
    {L"*XY-NM*",  L"NautM",         1852.0},                        // NOXLATE
    {L"*XY-DM*",  L"Decimeter",        0.1},                        // NOXLATE
    {L"*XY-MM*",  L"Millimeter",       0.001},                      // NOXLATE
    {L"*XY-DAM*", L"Decameter",       10.0},                        // NOXLATE
    {L"*XY-DK*",  L"Dekameter",       10.0},                        // NOXLATE
    {L"*XY-HM*",  L"Hectometer",     100.0},                        // NOXLATE
    {L"*XY-GM*",  L"GermanMeter",      1.0000135965},               // NOXLATE
    {L"*XY-CG*",  L"CaGrid",           0.999738},                   // NOXLATE
    {L"*XY-GC*",  L"GunterChain",     20.11684023368047},           // NOXLATE
    {L"*XY-CC*",  L"ClarkeChain",     20.1166194976},               // NOXLATE
    {L"*XY-BC*",  L"BenoitChain",     20.11678249438},              // NOXLATE
    {L"*XY-SC*",  L"SearsChain",      20.11676512155},              // NOXLATE
    {L"*XY-GL*",  L"GunterLink",       0.2011684023368047},         // NOXLATE
    {L"*XY-CL*",  L"ClarkeLink",       0.201166194976},             // NOXLATE
    {L"*XY-BL*",  L"BenoitLink",       0.2011678249438},            // NOXLATE
    {L"*XY-SL*",  L"SearsLink",        0.2011676512155},            // NOXLATE
    {L"*XY-RD*",  L"Rod",              5.02921005842012},           // NOXLATE
    {L"*XY-PE*",  L"Perch",            5.02921005842012},           // NOXLATE
    {L"*XY-PO*",  L"Pole",             5.02921005842012},           // NOXLATE
    {L"*XY-FU*",  L"Furlong",        201.1684023368046},            // NOXLATE
    {L"*XY-RO*",  L"Rood",             3.778266898},                // NOXLATE
    {L"*XY-CA*",  L"CapeFoot",         0.3047972615},               // NOXLATE
    {L"*XY-BR*",  L"Brealey",        375.0}                         // NOXLATE
};

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

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system.
///</summary>
CCoordinateSystem::CCoordinateSystem()
{
    m_ogcWkt = L"";
    m_coordinateSystemType = CCoordinateSystemType::Unknown;
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
CCoordinateSystem* CCoordinateSystem::Clone()
{
    CCoordinateSystem* coordSys = new CCoordinateSystem();
    if(NULL == coordSys)
    {
        throw new COutOfMemoryException(L"CCoordinateSystem.Clone", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystem.");
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
    if(coordSys->m_coordinateSystemType == CCoordinateSystemType::Projected)
    {
        coordSys->m_latLonSrs = m_latLonSrs->Clone();

        coordSys->m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(coordSys->m_ogrSrs, coordSys->m_latLonSrs, false);
        if(NULL == coordSys->m_transformForward)
        {
            delete coordSys;
            throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.Clone", __LINE__, __WFILE__, L"Could not create coordinate system forward transformation with specified coordinate systems.");
        }

        coordSys->m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(coordSys->m_latLonSrs, coordSys->m_ogrSrs, false);
        if(NULL == coordSys->m_transformInverse)
        {
            delete coordSys;
            throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.Clone", __LINE__, __WFILE__, L"Could not create coordinate system inverse transformation with specified coordinate systems.");
        }
    }

    return coordSys;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system to the specified OGC WKT.
///</summary>
CCoordinateSystem::CCoordinateSystem(CREFSTRING ogcWkt)
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

    // Initialize the catalog cache if it has not been created yet
    InitializeCatalog();

    try
    {
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
                    m_coordinateSystemType = CCoordinateSystemType::Arbitrary;
                }
                else if(m_ogrSrs->IsGeographic())
                {
                    m_coordinateSystemType = CCoordinateSystemType::Geographic;
                }
                else
                {
                    m_coordinateSystemType = CCoordinateSystemType::Projected;
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
                m_units = csUnits;
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

                if(m_coordinateSystemType == CCoordinateSystemType::Arbitrary)
                {
                    // Set the code
                    m_code = L"";

                    // Lookup the code in the arbitrary XY table
                    for (int j = 0; j < CCoordinateSystem::ArbitraryXYCoordinateSystemsCount; j++)
                    {
                        if(_wcsicmp(m_units.c_str(), CCoordinateSystem::ArbitraryXYCoordinateSystems[j].unitsCode) == 0)
                        {
                            m_code = CCoordinateSystem::ArbitraryXYCoordinateSystems[j].code;
                            break;
                        }
                    }

                    // Set the description
                    if(m_units.length() > 0)
                    {
                        // Determine description
                        // Description  = Arbitrary X-Y Coordinate (UnitsPlural)
                        m_description = CCoordinateSystem::ArbitraryXYDescription + L" (" + m_units + L")";
                    }
                    else
                    {
                        m_description = L"";
                    }

                    // Set the projection
                    m_projection = CCoordinateSystem::ArbitraryXYProjection;

                    // Set the projection description
                    m_projectionDescription = CCoordinateSystem::ArbitraryXYProjectionDescription;

                    // Set the datum
                    m_datum = CCoordinateSystem::ArbitraryXYDatum;

                    // Set the datum description
                    m_datumDescription = CCoordinateSystem::ArbitraryXYDatumDescription;

                    // Set ellipsoid
                    m_ellipsoid = CCoordinateSystem::ArbitraryXYEllipsoid;

                    // Set ellipsoid description
                    m_ellipsoidDescription = CCoordinateSystem::ArbitraryXYEllipsoidDescription;

                    // Set the category
                    m_category = CCoordinateSystem::ArbitraryXYCategoryDescription;
                }
                else
                {
                    // Check if this is a projected coordinate system
                    if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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
                                m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_ogrSrs, m_latLonSrs, false);
                                if(NULL == m_transformForward)
                                {
                                    throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not create coordinate system forward transformation with specified coordinate systems.");
                                }

                                m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_latLonSrs, m_ogrSrs, false);
                                if(NULL == m_transformInverse)
                                {
                                    throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not create coordinate system inverse transformation with specified coordinate systems.");
                                }
                            }
                            else
                            {
                                throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not parse the OGC WKT.");
                            }
                        }
                        else
                        {
                            throw new COutOfMemoryException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not allocate OGRSpatialReference.");
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

                        CCoordinateSystemCategoryCollection* categories;
                        categories = m_catalog->GetCoordinateSystemCategories();

                        size_t size = categories->GetCount();
                        for(size_t i=0;i<size;i++)
                        {
                            CCoordinateSystemCategory* category;
                            category = categories->GetItem(i);

                            size_t coordSysIndex = category->ContainsProj4(proj4Defn);
                            if(-1 != coordSysIndex)
                            {
                                CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystems();
                                CCoordinateSystemInformation* coordSys = coordSystems->at(coordSysIndex);

                                // Set the code
                                m_code = coordSys->m_code;

                                // Set the description
                                m_description = coordSys->m_description;

                                // Set the category
                                m_category = category->GetCategoryName();
                                break;
                            }
                        }
                    }

                    // Clean up
                    delete [] proj4;
                    proj4 = NULL;

                    if(OGRERR_NONE != error)
                    {
                        throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not convert OGC WKT to PROJ4.");
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
                throw new CInvalidCoordinateSystemException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not parse the OGC WKT.");
            }
        }
        else
        {
            throw new COutOfMemoryException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not allocate OGRSpatialReference.");
        }
    }
    catch (CException* ex)
    {
        // we must free resources if we encountered a problem
        Cleanup();

        throw ex;
    }
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
int CCoordinateSystem::GetType()
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

    try
    {
        // Check to see if the conversion is allowed, if not throw an exception
        double dCoords[3] = { lon, lat, 0.0 };

        // If the coordinate system is arbitrary or geographic there is no conversion
        if(m_coordinateSystemType == CCoordinateSystemType::Projected)
        {
            // Straight conversion no datum shift done.
            m_transformInverse->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
        }

        x = dCoords[0];
        y = dCoords[1];
    }
    catch(...)
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, L"Unexpected error.");
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
            if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, L"Unexpected error.");
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

    try
    {
        // Check to see if the conversion is allowed, if not throw an exception
        double dCoords[3] = { x, y, 0.0 };

        // If the coordinate system is arbitrary or geographic there is no conversion
        if(m_coordinateSystemType == CCoordinateSystemType::Projected)
        {
            // Straight conversion no datum shift done.
            m_transformForward->Transform(1, &dCoords[0], &dCoords[1], &dCoords[2]);
        }

        lon = dCoords[0];
        lat = dCoords[1];
    }
    catch(...)
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, L"Unexpected error.");
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
            if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, L"Unexpected error.");
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
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertCoordinateSystemUnitsToMeters", __LINE__, __WFILE__, L"Unexpected error.");
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
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertMetersToCoordinateSystemUnits", __LINE__, __WFILE__, L"Unexpected error.");
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
        throw new CCoordinateSystemMeasureFailedException(L"CCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, L"Unexpected error.");
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
    if(m_coordinateSystemType == CCoordinateSystemType::Arbitrary)
    {
        throw new CInvalidCoordinateSystemTypeException(L"CCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, L"MeasureGreatCircleDistance does not work with Arbitrary coordinate systems.");
    }

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

        if(m_coordinateSystemType == CCoordinateSystemType::Geographic)
        {
            lon1 = x1;
            lat1 = y1;
            lon2 = x2;
            lat2 = y2;
        }
        else if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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

        if(m_coordinateSystemType == CCoordinateSystemType::Projected)
        {
            // Convert the result in meters to native units
            distance = ConvertMetersToCoordinateSystemUnits(distance);
        }
    }
    catch(...)
    {
        throw new CCoordinateSystemMeasureFailedException(L"CCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, L"Unexpected error.");
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

    try
    {
        if(m_coordinateSystemType == CCoordinateSystemType::Arbitrary)
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

            if(m_coordinateSystemType == CCoordinateSystemType::Geographic)
            {
                lon1 = x1;
                lat1 = y1;
                lon2 = x2;
                lat2 = y2;
            }
            else if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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

            double angularDistance = ::acos(::sin(lat1Rads)*::sin(lat2Rads) + ::cos(lat1Rads)*::cos(lat2Rads)*::cos(lon2Rads-lon1Rads));
            double cosAzimuth = (::cos(lat1Rads)*::sin(lat2Rads) - ::sin(lat1Rads)*::cos(lat2Rads)*::cos(lon2Rads-lon1Rads)) / ::sin(angularDistance);
            double sinAzimuth = ::cos(lat2Rads)*::sin(lon2Rads-lon1Rads) / ::sin(angularDistance);

            azimuth = RAD_TO_DEG*::atan2(sinAzimuth, cosAzimuth);
        }
    }
    catch(...)
    {
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, L"Unexpected error.");
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

    try
    {
        if(m_coordinateSystemType == CCoordinateSystemType::Arbitrary)
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

            if(m_coordinateSystemType == CCoordinateSystemType::Geographic)
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
            else if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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

            if(m_coordinateSystemType == CCoordinateSystemType::Projected)
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
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, L"Unexpected error.");
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

    try
    {
        // Initialize the catalog cache if it has not been created yet
        InitializeCatalog();

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
                for (int i = 0; i < CCoordinateSystem::ArbitraryXYCoordinateSystemsCount; i++)
                {
                    if(_wcsicmp(csUnits, CCoordinateSystem::ArbitraryXYCoordinateSystems[i].unitsCode) == 0)
                    {
                        code = CCoordinateSystem::ArbitraryXYCoordinateSystems[i].code;
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

                        CCoordinateSystemCategoryCollection* categories;
                        categories = m_catalog->GetCoordinateSystemCategories();

                        size_t size = categories->GetCount();
                        for(size_t i=0;i<size;i++)
                        {
                            CCoordinateSystemCategory* category;
                            category = categories->GetItem(i);

                            size_t coordSysIndex = category->ContainsProj4(proj4Defn);
                            if(-1 != coordSysIndex)
                            {
                                CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystems();
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

                // Clean up
                delete [] proj4;
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
            delete [] proj4;
            proj4 = NULL;
        }

        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, L"Unexpected error.");
    }

    if(error != OGRERR_NONE)
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, L"Could not determine code because of internal OGR error.");
    }

    if(code.empty())
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, L"Could not determine OGC WKT from the code.");
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

    try
    {
        // Initialize the catalog cache if it has not been created yet
        InitializeCatalog();

        if(_wcsnicmp(csCode.c_str(), L"*XY-", 4) == 0)
        {
            // This is an arbitrary XY system
            STRING units = L"";
            double unitsValue = 1.0;

            // Lookup the code in the arbitrary XY table and return the Ogc WKT
            for (int j = 0; j < CCoordinateSystem::ArbitraryXYCoordinateSystemsCount; j++)
            {
                if(_wcsicmp(csCode.c_str(), CCoordinateSystem::ArbitraryXYCoordinateSystems[j].code) == 0)
                {
                    units = CCoordinateSystem::ArbitraryXYCoordinateSystems[j].unitsCode;
                    unitsValue = CCoordinateSystem::ArbitraryXYCoordinateSystems[j].conversionToMeters;
                    break;
                }
            }

            if(units.length() > 0)
            {
                STRING unitsValueString;

                char buffer[255];
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
                datum = L"LOCAL_DATUM[\"*X-Y*\",10000]";

                STRING axisX;
                axisX = L"AXIS[\"X\",EAST]";

                STRING axisY;
                axisY = L"AXIS[\"Y\",NORTH]";

                ogcWkt = L"LOCAL_CS";
                ogcWkt += L"[\"";
                ogcWkt += csCode;   //-- should be "*XY-<units>*"
                ogcWkt += L"\",";
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
            CCoordinateSystemCategoryCollection* categories;
            categories = m_catalog->GetCoordinateSystemCategories();

            size_t size = categories->GetCount();
            for(size_t i=0;i<size;i++)
            {
                CCoordinateSystemCategory* category;
                category = categories->GetItem(i);

                size_t coordSysIndex = category->ContainsCode(csCode);
                if(-1 != coordSysIndex)
                {
                    CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystems();
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

                                delete [] wkt;
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
            delete [] wkt;
            wkt = NULL;
        }

        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, L"Unexpected error.");
    }

    if(error != OGRERR_NONE)
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, L"Unsupported coordinate system code.");
    }

    if(ogcWkt.empty())
    {
        throw new CCoordinateSystemConversionFailedException(L"CCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, L"Unsupported coordinate system code.");
    }

    return ogcWkt;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets a list of the available coordinate system categories.
///</summary>
///<returns>
/// A list of the available catetories.
///</returns>
StringVector* CCoordinateSystem::EnumerateCategories()
{
    // Initialize the catalog cache if it has not been created yet
    InitializeCatalog();

    StringVector* categoryNames = new StringVector();
    if (categoryNames == NULL)
    {
        throw new COutOfMemoryException(L"CCoordinateSystem.EnumerateCategories", __LINE__, __WFILE__, L"Could not allocate StringVector.");
    }

    CCoordinateSystemCategoryCollection* categories;
    categories = m_catalog->GetCoordinateSystemCategories();

    size_t numCats = categories->GetCount();
    size_t i;
    for (i = 0; i < numCats; i++)
    {
        CCoordinateSystemCategory* category = NULL;
        category = categories->GetItem(i);
        categoryNames->push_back(category->GetCategoryName());
    }

    return categoryNames;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets a list of available coordinate systems in the specified category.
///</summary>
///<param name="category">
/// The category to retrieve the coordinate systems for.
///</param>
///<returns>
/// A coordinate system category object describing the coordinate systems
/// and their properties.
///</returns>
CCoordinateSystemCategory* CCoordinateSystem::EnumerateCoordinateSystems(CREFSTRING categoryName)
{
    if (categoryName.length() == 0)
    {
        throw new CInvalidArgumentException(L"CCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, L"[1] - Category not specified.");
    }

    CCoordinateSystemCategory* coordinateSystems = NULL;

    try
    {
        // Initialize the catalog cache if it has not been created yet
        InitializeCatalog();

        // Return the coordinate systems for the specified category
        CCoordinateSystemCategory* category = m_catalog->GetCoordinateSystemCategory(categoryName);
        if (category == NULL)
        {
            STRING message;
            message = L"[1] - Category not found - " + categoryName + L" .";
            throw new CInvalidArgumentException(L"CCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, message);
        }

        // Need to make a copy of the category and return it
        coordinateSystems = new CCoordinateSystemCategory(categoryName);
        if (coordinateSystems == NULL)
        {
            throw new COutOfMemoryException(L"CCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemCategory.");
        }

        CoordinateSystemInformationVector* coordSystems = category->GetCoordinateSystems();
        if (coordSystems == NULL)
        {
            throw new CNullReferenceException(L"CCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, L"CoordinateSystemInformationVector is NULL.");
        }

        for(size_t i=0;i<coordSystems->size();i++)
        {
            CCoordinateSystemInformation* coordSys = coordSystems->at(i);
            if(coordSys)
            {
                CCoordinateSystemInformation* coordSysCopy = coordSys->Clone();
                coordinateSystems->Add(coordSysCopy);
            }
        }
    }
    catch (CException* ex)
    {
        if(coordinateSystems)
        {
            delete coordinateSystems;
            coordinateSystems = NULL;
        }

        throw ex;
    }

    return coordinateSystems;
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

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system category that this coordinate
/// system belongs to.
/// </summary>
///<returns>
/// coordinate system category string
///</returns>
STRING CCoordinateSystem::GetCategory()
{
    return m_category;
}

void CCoordinateSystem::InitializeCatalog()
{
    // Perform Double-Checked Locking Optimization.
    // Do initial check for NULL without locking.
    // This is done so that once initialized we won't lock everytime we enter this method.
    if(m_catalog == NULL )
    {
        // Lock all threads while we create the catalog
        CriticalClass.Enter();
        // We have to check for NULL again because we might have been locked out by another
        // thread that just finished initializing the catalog.
        if(m_catalog == NULL )
        {
            // Create the catalog. We only want to do this once!
            m_catalog = new CCoordinateSystemCatalog();
        }
        CriticalClass.Leave();

        // Throw exception if we failed to initialize the catalog
        if (m_catalog == NULL)
        {
            throw new COutOfMemoryException(L"CCoordinateSystem.CCoordinateSystem", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemCatalog.");
        }
    }
}


void CCoordinateSystem::DeleteCatalog()
{
    if (m_catalog != NULL)
    {
        // Lock all threads while we delete the catalog
        CriticalClass.Enter();

        delete m_catalog;
        m_catalog = NULL;

        CriticalClass.Leave();
    }
}

STRING CCoordinateSystem::GetBaseLibrary()
{
    return CCoordinateSystem::BaseLibrary;
}
