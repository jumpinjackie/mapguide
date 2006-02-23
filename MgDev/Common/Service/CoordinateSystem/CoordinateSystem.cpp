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

#include "CoordinateSystemCommon.h"
#include "CoordSysIncludes.h"

// Coordinate system information properties
const STRING MgCoordinateSystem::CoordinateSystemCode                  = L"Code";
const STRING MgCoordinateSystem::CoordinateSystemDescription           = L"Description";
const STRING MgCoordinateSystem::CoordinateSystemProjection            = L"Projection";
const STRING MgCoordinateSystem::CoordinateSystemProjectionDescription = L"Projection Description";
const STRING MgCoordinateSystem::CoordinateSystemDatum                 = L"Datum";
const STRING MgCoordinateSystem::CoordinateSystemDatumDescription      = L"Datum Description";
const STRING MgCoordinateSystem::CoordinateSystemEllipsoid             = L"Ellipsoid";
const STRING MgCoordinateSystem::CoordinateSystemEllipsoidDescription  = L"Ellipsoid Description";

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs an empty coordinate system.
///</summary>
MgCoordinateSystem::MgCoordinateSystem() :
    m_coordSys(NULL)
{
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system.
///</summary>
MgCoordinateSystem::MgCoordinateSystem(CREFSTRING ogcWkt) :
    m_coordSys(NULL)
{
    try
    {
        CCoordinateSystemFactory factory;
        m_coordSys = factory.Create(ogcWkt);
    }
    catch(COutOfMemoryException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.MgCoordinateSystem", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.MgCoordinateSystem", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        MgStringCollection argumentsBase;
        argumentsBase.Add(e->GetMessage());

        delete e;

        MgStringCollection arguments;
        arguments.Add(ogcWkt);

        throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.MgCoordinateSystem", __LINE__, __WFILE__, &arguments, L"MgFormatInnerExceptionMessage", &argumentsBase);
    }

    if (m_coordSys == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.MgCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystem::~MgCoordinateSystem()
{
    if (m_coordSys != NULL)
    {
        delete m_coordSys;
        m_coordSys = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Intializes a coordinate system with the OGC WKT specified.
///</summary>
void MgCoordinateSystem::Initialize(CREFSTRING ogcWkt)
{
    // Clear any previous coordinate system
    if (m_coordSys != NULL)
    {
        delete m_coordSys;
        m_coordSys = NULL;
    }

    try
    {
        CCoordinateSystemFactory factory;
        m_coordSys = factory.Create(ogcWkt);
    }
    catch(COutOfMemoryException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.Initialize", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.Initialize", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        MgStringCollection argumentsBase;
        argumentsBase.Add(e->GetMessage());

        delete e;

        MgStringCollection arguments;
        arguments.Add(ogcWkt);

        throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.Initialize", __LINE__, __WFILE__, &arguments, L"MgFormatInnerExceptionMessage", &argumentsBase);
    }

    if (m_coordSys == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The MgCoordinateSystem class supplies operations for working with
/// coordinates in that coordinate system, including the ability to convert
/// coordinates to and from longitude latitude values, work with linear units,
/// and compute distances.
///</summary>
INT32 MgCoordinateSystem::GetType()
{
    if(m_coordSys)
    {
        return m_coordSys->GetType();
    }
    else
    {
        return CCoordinateSystemType::Unknown;
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified longitude latitude to a coordinate in this
/// coordinate system. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from longitude latitude to this coordinate system is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="lonLat">
/// An MgCoordinate containing the longitude latitude value to convert.
///</param>
///<returns>
/// A new MgCoordinate converted from the specified coordinate.
///</returns>
MgCoordinate* MgCoordinateSystem::ConvertFromLonLat(MgCoordinate* lonLat)
{
    MgCoordinate* pCoordinate = NULL;

    if(NULL == lonLat)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double x = 0.0;
        double y = 0.0;

        m_coordSys->ConvertFromLonLat(lonLat->GetX(), lonLat->GetY(), x, y);

        // Create new converted coordinate
        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified longitude latitude to a coordinate in this
/// coordinate system. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
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
void MgCoordinateSystem::ConvertFromLonLat(double lon, double lat, double& x, double& y)
{
    // Initialize
    x = 0.0;
    y = 0.0;

    try
    {
        m_coordSys->ConvertFromLonLat(lon, lat, x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified longitude latitude to a coordinate in this
/// coordinate system. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
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
void MgCoordinateSystem::ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize)
{
    try
    {
        m_coordSys->ConvertFromLonLat(lon, lat, x, y, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate in this coordinate system to a
/// longitude latitude value. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
/// datum conversion is desired. An exception is thrown if the conversion
/// from this coordinate system to longitude latitude is not supported.
/// The longitude and latitude coordinate system used is based on WGS84.
///</summary>
///<param name="coordinate">
/// An MgCoordinate that contains the coordinate value to convert.
///</param>
///<returns>
/// A new MgCoordinate converted from the specified coordinate.
///</returns>
MgCoordinate* MgCoordinateSystem::ConvertToLonLat(MgCoordinate* coordinate)
{
    MgCoordinate* pCoordinate = NULL;

    if(NULL == coordinate)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double lat = 0.0;
        double lon = 0.0;

        m_coordSys->ConvertToLonLat(coordinate->GetX(), coordinate->GetY(), lon, lat);

        // Create new converted coordinate
        pCoordinate = new MgCoordinateXY(lon, lat);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate in this coordinate system to a
/// longitude latitude value. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
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
void MgCoordinateSystem::ConvertToLonLat(double x, double y, double& lon, double& lat)
{
    // Initialize
    lat = 0.0;
    lon = 0.0;

    try
    {
        m_coordSys->ConvertToLonLat(x, y, lon, lat);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertToLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Converts the specified coordinate in this coordinate system to a
/// longitude latitude value. Note that no datum conversion is applied in
/// this conversion. Use an instance of MgCoordinateSystemTransform if
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
void MgCoordinateSystem::ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize)
{
    try
    {
        m_coordSys->ConvertToLonLat(x, y, lon, lat, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertFromLonLat", __LINE__, __WFILE__, NULL, L"", NULL);
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
double MgCoordinateSystem::ConvertCoordinateSystemUnitsToMeters(double units)
{
    double meters = 0.0;

    try
    {
        meters = m_coordSys->ConvertCoordinateSystemUnitsToMeters(units);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemUnitsToMeters", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemUnitsToMeters", __LINE__, __WFILE__, NULL, L"", NULL);
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
double MgCoordinateSystem::ConvertMetersToCoordinateSystemUnits(double meters)
{
    double units = 0.0;

    try
    {
        units = m_coordSys->ConvertMetersToCoordinateSystemUnits(meters);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertMetersToCoordinateSystemUnits", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertMetersToCoordinateSystemUnits", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return units;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Measures the euclidian distance between two coordinates.
///</summary>
///<param name="coord1">
/// An MgCoordinate that defines the starting coordinate.
///</param>
///<param name="coord2">
/// An MgCoordinate that defines the ending coordinate.
///</param>
///<returns>
/// The euclidean distance from coord1 to coord2 in coordinate system units.
///</returns>
double MgCoordinateSystem::MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;

    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        distance = m_coordSys->MeasureEuclideanDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return distance;
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
double MgCoordinateSystem::MeasureEuclideanDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    try
    {
        distance = m_coordSys->MeasureEuclideanDistance(x1, y1, x2, y2);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureEuclideanDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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
double MgCoordinateSystem::MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;

    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        distance = m_coordSys->MeasureGreatCircleDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"", NULL);
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
double MgCoordinateSystem::MeasureGreatCircleDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    try
    {
        distance = m_coordSys->MeasureGreatCircleDistance(x1, y1, x2, y2);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.MeasureGreatCircleDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return distance;
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
double MgCoordinateSystem::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double azimuth = 0.0;

    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        azimuth = m_coordSys->GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return azimuth;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="x1">
/// The x value that defines the firstcoordinate.
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
double MgCoordinateSystem::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double azimuth = 0.0;

    try
    {
        azimuth = m_coordSys->GetAzimuth(x1, y1, x2, y2);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return azimuth;
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
MgCoordinate* MgCoordinateSystem::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    MgCoordinate* pCoordinate = NULL;

    if(NULL == coord)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double x = 0.0;
        double y = 0.0;

        m_coordSys->GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance, x, y);

        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
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
MgCoordinate* MgCoordinateSystem::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        double x = 0.0;
        double y = 0.0;

        m_coordSys->GetCoordinate(xStart, yStart, azimuth, distance, x, y);

        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystem.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
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
STRING MgCoordinateSystem::ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt)
{
    STRING code;

    try
    {
        code = CCoordinateSystem::ConvertWktToCoordinateSystemCode(ogcWkt);
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertWktToCoordinateSystemCode", __LINE__, __WFILE__, NULL, L"", NULL);
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
STRING MgCoordinateSystem::ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode)
{
    STRING ogcWkt;

    try
    {
        ogcWkt = CCoordinateSystem::ConvertCoordinateSystemCodeToWkt(csCode);
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystem.ConvertCoordinateSystemCodeToWkt", __LINE__, __WFILE__, NULL, L"", NULL);
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
MgStringCollection* MgCoordinateSystem::EnumerateCategories()
{
    Ptr<MgStringCollection> categoryNames;

    categoryNames = new MgStringCollection();
    if (categoryNames == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.EnumerateCategories", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    StringVector* categories = NULL;

    try
    {
        categories = CCoordinateSystem::EnumerateCategories();

        if (categories)
        {
            size_t numCats = categories->size();
            size_t i;
            for (i = 0; i < numCats; i++)
            {
                STRING categoryName = categories->at(i);
                categoryNames->Add(categoryName);
            }

            delete categories;
            categories = NULL;
        }
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        if(categories)
        {
            delete categories;
            categories = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.EnumerateCategories", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        if(categories)
        {
            delete categories;
            categories = NULL;
        }

        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;

        throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.EnumerateCategories", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }

    return categoryNames.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets a list of available coordinate systems in the specified category.
///</summary>
///<param name="category">
/// The category to retrieve the coordinate systems for.
///</param>
///<returns>
/// A batch property collection describing the available coordinate systems
/// and their properties.
///</returns>
MgBatchPropertyCollection* MgCoordinateSystem::EnumerateCoordinateSystems(CREFSTRING categoryName)
{
    Ptr<MgBatchPropertyCollection> coordinateSystems;
    coordinateSystems = NULL;

    coordinateSystems = new MgBatchPropertyCollection();
    if (coordinateSystems == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CCoordinateSystemCategory* coordSystemCategory = NULL;
    try
    {
        if(categoryName.length() > 0)
        {
            coordSystemCategory = CCoordinateSystem::EnumerateCoordinateSystems(categoryName);
            if(coordSystemCategory)
            {
                CoordinateSystemInformationVector* coordSystems = coordSystemCategory->GetCoordinateSystems();

                size_t i;
                for(i=0;i<coordSystems->size();i++)
                {
                    CCoordinateSystemInformation* coordSysInfo = coordSystems->at(i);
                    if(coordSysInfo)
                    {
                        // Add the coodinate system properties to the collection
                        Ptr<MgProperty> pProperty = (MgProperty*)NULL;
                        Ptr<MgPropertyCollection> coordSysProperties = new MgPropertyCollection();

                        // Coordinate system properties
                        STRING code;
                        STRING description;
                        STRING projection;
                        STRING projectionDescription;
                        STRING datum;
                        STRING datumDescription;
                        STRING ellipsoid;
                        STRING ellipsoidDescription;

                        // Add the Code
                        pProperty = new MgStringProperty(CoordinateSystemCode, coordSysInfo->m_code);
                        coordSysProperties->Add(pProperty);

                        // Add the Description
                        pProperty = new MgStringProperty(CoordinateSystemDescription, coordSysInfo->m_description);
                        coordSysProperties->Add(pProperty);

                        // Add the Projection Name
                        pProperty = new MgStringProperty(CoordinateSystemProjection, coordSysInfo->m_projection);
                        coordSysProperties->Add(pProperty);

                        // Add the Projection Description
                        pProperty = new MgStringProperty(CoordinateSystemProjectionDescription, coordSysInfo->m_projectionDescription);
                        coordSysProperties->Add(pProperty);

                        // Add the Datum Name
                        pProperty = new MgStringProperty(CoordinateSystemDatum, coordSysInfo->m_datum);
                        coordSysProperties->Add(pProperty);

                        // Add the Datum description
                        pProperty = new MgStringProperty(CoordinateSystemDatumDescription, coordSysInfo->m_datumDescription);
                        coordSysProperties->Add(pProperty);

                        // Add the Ellipsoid Name
                        pProperty = new MgStringProperty(CoordinateSystemEllipsoid, coordSysInfo->m_ellipsoid);
                        coordSysProperties->Add(pProperty);

                        // Add the Ellipsoid description
                        pProperty = new MgStringProperty(CoordinateSystemEllipsoidDescription, coordSysInfo->m_ellipsoidDescription);
                        coordSysProperties->Add(pProperty);

                        // Add the coordinate system properties to the collection of coordinate systems
                        coordinateSystems->Add(coordSysProperties);
                    }
                }

                delete coordSystemCategory;
                coordSystemCategory = NULL;
            }
        }
    }
    catch(CCoordinateSystemInitializationFailedException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;

        if(coordSystemCategory)
        {
            delete coordSystemCategory;
            coordSystemCategory = NULL;
        }

        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;

        if(coordSystemCategory)
        {
            delete coordSystemCategory;
            coordSystemCategory = NULL;
        }

        throw new MgInvalidCoordinateSystemException(L"MgCoordinateSystem.EnumerateCoordinateSystems", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }

    return coordinateSystems.Detach();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns the OGC WKT string representation of the coordinate system.
///</summary>
///<returns>
/// OGC WKT string.
///</returns>
STRING MgCoordinateSystem::ToString()
{
    if(m_coordSys)
    {
        return m_coordSys->ToString();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units.
/// </summary>
STRING MgCoordinateSystem::GetUnits()
{
    if(m_coordSys)
    {
        return m_coordSys->GetUnits();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system units scale.
/// </summary>
double MgCoordinateSystem::GetUnitScale()
{
    if(m_coordSys)
    {
        return m_coordSys->GetUnitScale();
    }
    else
    {
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum X value.
/// </summary>
double MgCoordinateSystem::GetMinX()
{
    if(m_coordSys)
    {
        return m_coordSys->GetMinX();
    }
    else
    {
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the minimum Y value.
/// </summary>
double MgCoordinateSystem::GetMinY()
{
    if(m_coordSys)
    {
        return m_coordSys->GetMinY();
    }
    else
    {
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum X value.
/// </summary>
double MgCoordinateSystem::GetMaxX()
{
    if(m_coordSys)
    {
        return m_coordSys->GetMaxX();
    }
    else
    {
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the maximum Y value.
/// </summary>
double MgCoordinateSystem::GetMaxY()
{
    if(m_coordSys)
    {
        return m_coordSys->GetMaxY();
    }
    else
    {
        return 0.0;
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system code.
/// </summary>
STRING MgCoordinateSystem::GetCode()
{
    if(m_coordSys)
    {
        return m_coordSys->GetCode();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system description.
/// </summary>
STRING MgCoordinateSystem::GetDescription()
{
    if(m_coordSys)
    {
        return m_coordSys->GetDescription();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system projection.
/// </summary>
STRING MgCoordinateSystem::GetProjection()
{
    if(m_coordSys)
    {
        return m_coordSys->GetProjection();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system projection description.
/// </summary>
STRING MgCoordinateSystem::GetProjectionDescription()
{
    if(m_coordSys)
    {
        return m_coordSys->GetProjectionDescription();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system datum.
/// </summary>
STRING MgCoordinateSystem::GetDatum()
{
    if(m_coordSys)
    {
        return m_coordSys->GetDatum();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system datum description.
/// </summary>
STRING MgCoordinateSystem::GetDatumDescription()
{
    if(m_coordSys)
    {
        return m_coordSys->GetDatumDescription();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system ellipsoid.
/// </summary>
STRING MgCoordinateSystem::GetEllipsoid()
{
    if(m_coordSys)
    {
        return m_coordSys->GetEllipsoid();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system ellipsoid description.
/// </summary>
STRING MgCoordinateSystem::GetEllipsoidDescription()
{
    if(m_coordSys)
    {
        return m_coordSys->GetEllipsoidDescription();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the coordinate system category that this coordinate
/// system belongs to.
/// </summary>
///<returns>
/// coordinate system category string
///</returns>
STRING MgCoordinateSystem::GetCategory()
{
    if(m_coordSys)
    {
        return m_coordSys->GetCategory();
    }
    else
    {
        return L"";
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgCoordinateSystem::GetClassId()
{
    return m_cls_id;
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystem::Dispose()
{
    delete this;
}

STRING MgCoordinateSystem::GetBaseLibrary()
{
    return CCoordinateSystem::GetBaseLibrary();
}

