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

#ifndef _CCOORDINATESYSTEM_H_
#define _CCOORDINATESYSTEM_H_

#ifdef _WIN32
#pragma warning( disable: 4251 )
#else
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp
#endif

class OGRSpatialReference;
class OGRCoordinateTransformation;

namespace CSLibrary
{
class CCoordinateSystemCatalog;
class CCoordinateSystemCategory;

struct ArbitraryCoordinateSystem {
    wchar_t code[50];
    wchar_t unitsCode[50];
    double conversionToMeters;
};

enum CsLibStatus
{
    cslsInitialized    = 0,
    cslsInitializationFailed,
    cslsLoadFailed
};

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystem class supplies operations for working with
/// coordinates in that coordinate system, including the ability to convert
/// coordinates to and from longitude latitude values, work with linear units,
/// and compute distances.
///</summary>

class COORDINATE_SYSTEM_API CCoordinateSystem
{
public:
    CCoordinateSystem();
    CCoordinateSystem(CREFSTRING ogcWkt);
    virtual ~CCoordinateSystem();

    static void InitializeCatalog();
    static void DeleteCatalog();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the base coordinate system library being used.
    ///</summary>
    static STRING GetBaseLibrary();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the coordinate system library initialization status.
    ///</summary>
    static CsLibStatus GetLibraryStatus();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Sets the coordinate system library initialization status.
    ///</summary>
    static void SetLibraryStatus(CsLibStatus status);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns a copy of the coordinate system.
    ///</summary>
    CCoordinateSystem* Clone();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the type of the coordinate system, arbitrary, geographic, or
    /// projected.
    ///</summary>
    ///<returns>
    /// One of the values defined in CCoordinateSystemType.
    ///</returns>
    virtual int GetType();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified value in coordinate system units to
    /// meters.
    ///</summary>
    ///<param name="double units">
    /// The value in coordinate system units to convert.
    ///</param>
    ///<returns>
    /// The value in meters.
    ///</returns>
    virtual double ConvertCoordinateSystemUnitsToMeters(double units);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the value in meters to coordinate system units.
    ///</summary>
    ///<param name="double meters">
    /// The value in meters to convert.
    ///</param>
    ///<returns>
    /// The value in coordinate system units.
    ///</returns>
    virtual double ConvertMetersToCoordinateSystemUnits(double meters);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Measures the euclidian distance between two coordinates.
    ///</summary>
    ///<param name="double x1">
    /// The x value that defines the starting coordinate.
    ///</param>
    ///<param name="double y1">
    /// The y value that defines the starting coordinate.
    ///</param>
    ///<param name="double x2">
    /// The x value that defines the ending coordinate.
    ///</param>
    ///<param name="double y2">
    /// The y value that defines the ending coordinate.
    ///</param>
    ///<returns>
    /// The euclidean distance from coord1 to coord2 in coordinate system units.
    ///</returns>
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Measures the distance between two coordinates.
    /// This function uses the coordinate systems ellipsoid
    /// definition and a great circle algorithm to produce the result.
    ///</summary>
    ///<param name="double x1">
    /// The x value that defines the starting coordinate.
    ///</param>
    ///<param name="double y1">
    /// The y value that defines the starting coordinate.
    ///</param>
    ///<param name="double x2">
    /// The x value that defines the ending coordinate.
    ///</param>
    ///<param name="double y2">
    /// The y value that defines the ending coordinate.
    ///</param>
    ///<returns>
    /// The great circle distance from coord1 to coord2 in coordinate system units.
    ///</returns>
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);

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
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Computes a coordinate a given distance along a vector that is defined
    /// by a starting coordinate and an azimuth (Angle with respect to the
    /// North).
    ///</summary>
    ///<param name="double xStart">
    /// The x value that represents the start of the vector.
    ///</param>
    ///<param name="double yStart">
    /// The y value that represents the start of the vector.
    ///</param>
    ///<param name="double azimuth">
    /// An azimuth (Angle with respect to the North) that defines the direction
    /// of the vector.
    ///</param>
    ///<param name="double distance">
    /// The distance along the vector of the desired coordinate in coordinate system units.
    ///</param>
    ///<param name="double& x">
    /// The x value that represents the resultant coordinate.
    ///</param>
    ///<param name="double& y">
    /// The y value that represents the resultant coordinate.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void GetCoordinate(double xStart, double yStart, double azimuth, double distance, double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the OGC WKT string representation of the coordinate system.
    ///</summary>
    ///<returns>
    /// OGC WKT string.
    ///</returns>
    virtual STRING ToString();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system units.
    /// </summary>
    ///<returns>
    /// coordinate system units string
    ///</returns>
    STRING GetUnits();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system units scale.
    /// </summary>
    ///<returns>
    /// coordinate system unit scale
    ///</returns>
    double GetUnitScale();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the minimum X value.
    /// </summary>
    ///<returns>
    /// coordinate system minimum X value
    ///</returns>
    double GetMinX();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the minimum Y value.
    /// </summary>
    ///<returns>
    /// coordinate system minimum Y value
    ///</returns>
    double GetMinY();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the maximum X value.
    /// </summary>
    ///<returns>
    /// coordinate system maximum X value
    ///</returns>
    double GetMaxX();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the maximum Y value.
    /// </summary>
    ///<returns>
    /// coordinate system maximum Y value
    ///</returns>
    double GetMaxY();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system code.
    /// </summary>
    ///<returns>
    /// coordinate system code string
    ///</returns>
    STRING GetCode();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system description.
    /// </summary>
    ///<returns>
    /// coordinate system description string
    ///</returns>
    STRING GetDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system projection.
    /// </summary>
    ///<returns>
    /// coordinate system projection string
    ///</returns>
    STRING GetProjection();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system projection description.
    /// </summary>
    ///<returns>
    /// coordinate system projection description string
    ///</returns>
    STRING GetProjectionDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system datum.
    /// </summary>
    ///<returns>
    /// coordinate system datum string
    ///</returns>
    STRING GetDatum();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system datum description.
    /// </summary>
    ///<returns>
    /// coordinate system datum description string
    ///</returns>
    STRING GetDatumDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system ellipsoid.
    /// </summary>
    ///<returns>
    /// coordinate system ellipsoid string
    ///</returns>
    STRING GetEllipsoid();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system ellipsoid description.
    /// </summary>
    ///<returns>
    /// coordinate system ellipsoid description string
    ///</returns>
    STRING GetEllipsoidDescription();

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
    static STRING ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt);

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
    static STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Gets a list of the available coordinate system categories.
    ///</summary>
    ///<returns>
    /// A list of the available catetories.
    ///</returns>
    static StringVector* EnumerateCategories();

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
    static CCoordinateSystemCategory* EnumerateCoordinateSystems(CREFSTRING category);

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system category that this coordinate
    /// system belongs to.
    /// </summary>
    ///<returns>
    /// coordinate system category string
    ///</returns>
    STRING GetCategory();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the internal coordinate system.
    /// </summary>
    OGRSpatialReference* GetInternalCoordinateSystem() { return m_ogrSrs; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double lon">
    /// The longitude value to convert.
    ///</param>
    ///<param name="double lat">
    /// The latitude value to convert.
    ///</param>
    ///<param name="double& x">
    /// The converted longitude value.
    ///</param>
    ///<param name="double& y">
    /// The converted latitude value.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertFromLonLat(double lon, double lat, double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double lon[]">
    /// The longitude values to convert.
    ///</param>
    ///<param name="double lat[]">
    /// The latitude values to convert.
    ///</param>
    ///<param name="double x[]">
    /// The converted longitude values.
    ///</param>
    ///<param name="double y[]">
    /// The converted latitude values.
    ///</param>
    ///<param name="int arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double x">
    /// The value to convert to a longitude value.
    ///</param>
    ///<param name="double y">
    /// The value to convert to a latitude value.
    ///</param>
    ///<param name="double& lon">
    /// The longitude value.
    ///</param>
    ///<param name="double& lat">
    /// The latitude value.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertToLonLat(double x, double y, double& lon, double& lat);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double x[]">
    /// The values to convert to a longitude values.
    ///</param>
    ///<param name="double y[]">
    /// The values to convert to a latitude values.
    ///</param>
    ///<param name="double lon[]">
    /// The longitude values.
    ///</param>
    ///<param name="double lat[]">
    /// The latitude values.
    ///</param>
    ///<param name="int arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified EPSG coordinate system code into the corresponding
    /// wkt string.
    ///</summary>
    ///<param name="code">
    /// The EPSG coordinate system code to convert.
    ///</param>
    ///<returns>
    /// String of the corresponding wkt
    ///</returns>
    static STRING ConvertEpsgCodeToWkt(long code);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified wkt string into the corresponding
    /// EPSG code.
    ///</summary>
    ///<param name="wkt">
    /// The wkt string to convert.
    ///</param>
    ///<returns>
    /// The corresponding EPSG code
    ///</returns>
    static long ConvertWktToEpsgCode(CREFSTRING wkt);

    // Arbitrary X-Y Coordinate Support
    static const STRING ArbitraryXYCategoryDescription;
    static const STRING ArbitraryXYDescription;
    static const STRING ArbitraryXYProjection;
    static const STRING ArbitraryXYProjectionDescription;
    static const STRING ArbitraryXYDatum;
    static const STRING ArbitraryXYDatumDescription;
    static const STRING ArbitraryXYEllipsoid;
    static const STRING ArbitraryXYEllipsoidDescription;
    static const int ArbitraryXYCoordinateSystemsCount;
    static const ArbitraryCoordinateSystem ArbitraryXYCoordinateSystems[];

    // Coordinate system information properties
    static const STRING CoordinateSystemCode;
    static const STRING CoordinateSystemDescription;
    static const STRING CoordinateSystemProjection;
    static const STRING CoordinateSystemProjectionDescription;
    static const STRING CoordinateSystemDatum;
    static const STRING CoordinateSystemDatumDescription;
    static const STRING CoordinateSystemEllipsoid;
    static const STRING CoordinateSystemEllipsoidDescription;

    static const STRING LatLonWkt;

    static const STRING BaseLibrary;

private:
    void Cleanup();

private:
    STRING m_ogcWkt;
    int m_coordinateSystemType;
    OGRSpatialReference* m_ogrSrs;
    OGRSpatialReference* m_latLonSrs;
    OGRCoordinateTransformation* m_transformForward;
    OGRCoordinateTransformation* m_transformInverse;

    STRING m_units;
    double m_unitScale;
    double m_minX;
    double m_minY;
    double m_maxX;
    double m_maxY;
    STRING m_code;
    STRING m_description;
    STRING m_projection;
    STRING m_projectionDescription;
    STRING m_datum;
    STRING m_datumDescription;
    STRING m_ellipsoid;
    STRING m_ellipsoidDescription;
    STRING m_category;

    static CCoordinateSystemCatalog* m_catalog;
    static CsLibStatus m_status;

};

} // End of namespace

#endif //_CCOORDINATESYSTEM_H_
