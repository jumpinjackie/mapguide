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

#ifndef _MGCOORDINATESYSTEM_H_
#define _MGCOORDINATESYSTEM_H_

namespace CSLibrary
{
    class CCoordinateSystem;
}
using namespace CSLibrary;

enum LibraryStatus
{
    lsInitialized    = 0,
    lsInitializationFailed,
    lsLoadFailed
};

/// \defgroup MgCoordinateSystem  MgCoordinateSystem
/// \ingroup Coordinate_System_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Provides operations for the following tasks:
///   * getting the MgCoordinateSystemType type of the coordinate
///     system,
///   * converting from geographic to projected coordinates,
///   * converting from projected to geographic coordinates,
///   * converting measurements to and from meters,
///   * measuring euclidean and great circle distances,
///   * computing the azimuth of a vector formed by two
///     coordinates,
///   * computing a coordinate position given a starting
///     coordinate, an azimuth, and a distance along the vector
///     formed by the coordinate and the azimuth,
///   * get information about the coordinate system.
///
/// \remarks
/// Methods ConvertFromLonLat, ConvertToLonLat, and
/// MeasureEuclideanDistance are valid if the coordinate system
/// is of type MgCoordinateSystemType::Projected. Method
/// MeasureGreatCircleDistance is valid if the coordinate system
/// is of type MgCoordinateSystemType::Geographic. Methods
/// GetAzimuth and GetCoordinate are valid for both types.
/// \n
/// MeasureEuclideanDistance, MeasureGreatCircleDistance, and
/// GetCoordinate return values expressed in native units, that
/// is, the value of \<name\> parameter in the UNIT[] clause in
/// the coordinate system WKT string. See \link WKTofSRS WKTofSRS \endlink
/// and \link CSTestValues CSTestValues \endlink.
///
class MG_GEOMETRY_API MgCoordinateSystem : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystem)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the coordinate system: arbitrary,
    /// geographic, or projected.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual int GetType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual int GetType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual int GetType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns one of the values defined in MgCoordinateSystemType.
    ///
    virtual INT32 GetType();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified longitude / latitude coordinate to a
    /// coordinate in this projected coordinate system.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \remarks
    /// Use this method for converting longitude / latitude
    /// coordinates to the equivalent in a projected coordinate
    /// system. The conversion is based on the parameters of the
    /// geographic coordinate system that this coordinate system is a
    /// projection of.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate ConvertFromLonLat(MgCoordinate lonLat);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate ConvertFromLonLat(MgCoordinate lonLat);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate ConvertFromLonLat(MgCoordinate lonLat);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param lonLat (MgCoordinate)
    /// An MgCoordinate containing the longitude
    /// latitude value to convert.
    ///
    /// \return
    /// Returns an MgCoordinate object with projected coordinate
    /// values that correspond to the given longitude latitude
    /// values.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($xGeog, $yGeog) is (-71.061342, 42.355892), and $xConv,
    /// $yConv) is (824399.31463498, 4696596.0339483).
    /// \code
    /// $wktProj = 'PROJCS["UTM Zone 18 (NAD 83)", GEOGCS ["NAD 83 (Continental US)", DATUM ["NAD 83 (Continental US)", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]], PROJECTION ["Transverse Mercator"], PARAMETER ["Scale_Factor", 0.999600], PARAMETER ["Central_Meridian", -75.000000], PARAMETER ["False_Easting", 500000.000000], UNIT ["Meter", 1.000000000000]]';
    /// $coordSysProj = $coordSysFactory->Create($wktProj);
    /// $xGeog = -71.061342;
    /// $yGeog = 42.355892;
    /// $coordinate = $geometryFactory->CreateCoordinateXY($xGeog, $yGeog);
    /// $convertedCoordinate = $coordSys->ConvertFromLonLat($coordinate);
    /// $xConv = $convertedCoordinate->GetX();
    /// $yConv = $convertedCoordinate->GetY();
    /// echo  "($xGeog, $yGeog) to ($xConv, $yConv)n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinate* ConvertFromLonLat(MgCoordinate* lonLat);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinate with projected ordinate
    /// values to a coordinate with longitude and latitude values.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \remarks
    /// Use this method to convert a coordinate value from a
    /// projected coordinate system into the equivalent longitude and
    /// latitude values of the geographic coordinate system that the
    /// coordinate system is a projection of.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate ConvertToLonLat(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate ConvertToLonLat(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate ConvertToLonLat(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// An MgCoordinate that contains the
    /// coordinate value to convert.
    ///
    /// \return
    /// Returns an MgCoordinate that contains the longitude and
    /// latitude values that correspond to the given coordinate.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($xConv, $yConv) is (-71.061342, 42.355892), and $xProj,
    /// $yProj) is (824399.31463498, 4696596.0339483).
    /// <pre>
    /// <c>$wktProj = 'PROJCS["UTM Zone 18 (NAD 83)", GEOGCS ["NAD 83 (Continental US)", DATUM ["NAD 83 (Continental US)", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]], PROJECTION ["Transverse Mercator"], PARAMETER ["Scale_Factor", 0.999600], PARAMETER ["Central_Meridian", -75.000000], PARAMETER ["False_Easting", 500000.000000], UNIT ["Meter", 1.000000000000]]';</c>
    /// <c>$coordSysProj = $coordSysFactory->Create($wktProj);</c>
    /// <c>$xProj = 824399.31463498;</c>
    /// <c>$yProj = 4696596.0339483;</c>
    /// <c>$coordinate = $geometryFactory->CreateCoordinateXY($xProj, $yProj);</c>
    /// <c>$convertedCoordinate = $coordSys->ConvertToLonLat($coordinate);</c>
    /// <c>$xConv = $convertedCoordinate->GetX();</c>
    /// <c>$yConv = $convertedCoordinate->GetY();</c>
    /// <c>echo  "($xProj, $yProj) to ($xConv, $yConv)n";</c>
    /// </pre>
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinate* ConvertToLonLat(MgCoordinate* coordinate);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified value in coordinate system units to
    /// meters.
    ///
    /// \remarks
    /// The WKT definition for the coordinate system specifies the
    /// coordinate system unit using the keyword UNIT. See \link WKTofSRS WKTofSRS \endlink
    /// and \link CSTestValues CSTestValues \endlink. If the coordinate system uses
    /// decimal degrees as its unit, then calling this method on the
    /// corresponding MgCoordinateSystem object converts the argument
    /// in decimal degrees into a return value in meters.
     /// The conversion factor is 111319.4908 meters/degree.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double ConvertCoordinateSystemUnitsToMeters(double units);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double ConvertCoordinateSystemUnitsToMeters(double units);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double ConvertCoordinateSystemUnitsToMeters(double units);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param units (double)
    /// The value in coordinate system units to
    /// convert.
    ///
    /// \return
    /// Returns the value in meters.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This code converts 0.0089831528411952 decimal degrees into
    /// 1000 meters.
    /// \code
    /// $wkt = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM ["NAD 83", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]]';
    /// $coordSys = $coordSysFactory->Create($wkt);
    /// $csUnits = 0.0089831528411952;
    /// $meterss = $coordSys->ConvertCoordinateSystemUnitsToMeters($csUnits);
    /// echo "$metersn";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double ConvertCoordinateSystemUnitsToMeters(double units);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the value in meters to coordinate system units.
    ///
    /// \remarks
    /// The WKT definition for the coordinate system specifies the
    /// coordinate system unit using the keyword UNIT. See \link WKTofSRS WKTofSRS \endlink
    /// and \link CSTestValues CSTestValues \endlink. If the coordinate system uses
    /// decimal degrees as its unit, then calling this method on the
    /// corresponding MgCoordinateSystem object converts the argument
    /// in meters into a return value in decimal degrees.
     /// The conversion factor is 8.98312528411952e-6 degrees/meter.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double ConvertMetersToCoordinateSystemUnits(double meters);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double ConvertMetersToCoordinateSystemUnits(double meters);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double ConvertMetersToCoordinateSystemUnits(double meters);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param meters (double)
    /// The value in meters to convert.
    ///
    /// \return
    /// Returns the value in coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This code converts 1 meters into 0.0000089831528411952
    /// decimal degrees.
    /// \code
    /// $wkt = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM ["NAD 83", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]]';
    /// $coordSys = $coordSysFactory->Create($wkt);
    /// $csUnits = $coordSys->ConvertMetersToCoordinateSystemUnits(1000);
    /// echo "$csUnitsn";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double ConvertMetersToCoordinateSystemUnits(double meters);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance between two coordinates.
    ///
    /// \remarks
    /// The x and y values contained in the coordinate arguments, and
    /// the distance returned is are in coordinate system units.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord1 (MgCoordinate)
    /// An MgCoordinate that defines the
    /// starting coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate that defines the ending
    /// coordinate.
    ///
    /// \return
    /// Returns the euclidean distance from coord1 to coord2 in
    /// coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Proj contains the longitude and latitude of Boston.
    /// $coord2Proj contains the longitude and latitude of New York
    /// City. $coordSysProj was created using the WKT for a projected
    /// coordinate system, UTM Zone 18 (NAD 83). $distance is
    /// 361797.633671 meters.
    /// \code
    /// $x1Proj = 824390.29381426;
    /// $y1Proj = 4696809.9055963;
    /// $coord1Proj = $geometryFactory->CreateCoordinateXY($x1Proj, $y1Proj);
    /// $x2Proj = 516352.70285172;
    /// $y2Proj = 4507045.9198563;
    /// $coord2Proj = $geometryFactory->CreateCoordinateXY($x2Proj, $y2Proj);
    /// $distance = $coordSysProj->MeasureEuclideanDistance($coord1Proj, $coord2Proj);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance in meters between (x1, y1)
    /// and (x2, y2).
    ///
    /// \remarks
    /// The x and y values and the distance returned are in coordinate
     /// system units.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x1 (double)
    /// The x value that defines the starting
    /// coordinate.
    /// \param y1 (double)
    /// The y value that defines the starting
    /// coordinate.
    /// \param x2 (double)
    /// The x value that defines the ending
    /// coordinate.
    /// \param y2 (double)
    /// The y value that defines the ending
    /// coordinate.
    ///
    /// \return
    /// Returns the euclidean distance from (x1, y1) to (x2, y2) in
    /// coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1Proj, $y1Proj) is the longitude and latitude of Boston.
    /// ($x2Proj, $y2Proj) is the longitude and latitude of New York
    /// City. $coordSysProj was created using the WKT for a projected
    /// coordinate system, UTM Zone 18 (NAD 83). $distance is
    /// 361797.633671 meters.
    /// \code
    /// $x1Proj = 824390.29381426;
    /// $y1Proj = 4696809.9055963;
    /// $x2Proj = 516352.70285172;
    /// $y2Proj = 4507045.9198563;
    /// $distance = $coordSysProj->MeasureEuclideanDistance($x1Proj, $y1Proj, $x2Proj, $y2Proj);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the great circle distance in meters between two
    /// coordinates.
    ///
    /// \remarks
    /// This method uses the coordinate system's ellipsoid definition
    /// and a great circle algorithm to produce the result.
    ///
    /// \remarks
    /// The x and y values contained in the coordinates are in
    /// coordinate system units. The distance returned is in meters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord1 (MgCoordinate)
    /// An MgCoordinate that defines the
    /// starting coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate that defines the ending
    /// coordinate.
    ///
    /// \return
    /// The great circle distance from coord1 to coord2 in coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Geog contains the longitude and latitude of Boston.
    /// $coord2Geog contains the longitude and latitude of New York
    /// City. $coordSysGeog was created using the WKT for a
    /// geographic coordinate system, Longitude / Latitude (NAD 83).
    /// $distance is 361777.95418396 meters.
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $coord2Geog = $geometryFactory->CreateCoordinateXY($x2Geog, $y2Geog);
    /// $distance = $coordSysGeog->MeasureGreatCircleDistance($coord1Geog, $coord2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the distance in meters between two (x1, y1) and (x2,
    /// y2).
    ///
    /// \remarks
    /// This method uses the coordinate systems ellipsoid definition
    /// and a great circle algorithm to produce the result. The x and
    /// y values are in coordinate system units. The distance
    /// returned is in meters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x1 (double)
    /// The x value that defines the starting
    /// coordinate.
    /// \param y1 (double)
    /// The y value that defines the starting
    /// coordinate.
    /// \param x2 (double)
    /// The x value that defines the ending
    /// coordinate.
    /// \param y2 (double)
    /// The y value that defines the ending
    /// coordinate.
    ///
    /// \return
    /// Returns the great circle distance from (x1, y1) to (x2, y2)
    /// in meters.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1Geog, $y1Geog) is the longitude and latitude of Boston.
    /// ($x2Geog, $y2Geog) is the longitude and latitude of New York
    /// City. $coordSysGeog was created using the WKT for a
    /// geographic coordinate system, Longitude / Latitude (NAD 83).
    /// $distance is 361777.95418396 meters.
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $distance = $coordSysGeog->MeasureGreatCircleDistance($x1Geog, $y1Geog, $x2Geog, $y2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by
    /// two coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetAzimuth(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetAzimuth(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetAzimuth(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord1 (MgCoordinate)
    /// An MgCoordinate that specifies the
    /// first coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate that specifies the
    /// second coordinate.
    ///
    /// \return
    /// Returns the \link azimuth azimuth \endlink (Angle with respect to the
    /// North) of the vector formed by coord1 and coord2.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Geog contains the longitude and latitude of Boston,
    /// and $coord2Geog contains the longitude and latitude of New
    /// York City. The azimuth of the vector formed by these two
    /// coordinates in the direction Boston to New York City is
    /// -119.00856517599 degrees (moving counterclockwise from
    /// North).
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $coord2Geog = $geometryFactory->CreateCoordinateXY($x2Geog, $y2Geog);
    /// $azimuth = $coordSysGeog->GetAzimuth($coord1Geog, $coord2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by
    /// two coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x1 (double)
    /// The x value that defines the first
    /// coordinate.
    /// \param y1 (double)
    /// The y value that defines the first
    /// coordinate.
    /// \param x2 (double)
    /// The x value that defines the second
    /// coordinate.
    /// \param y2 (double)
    /// The y value that defines the second
    /// coordinate.
    ///
    /// \return
    /// Returns the \link azimuth azimuth \endlink (Angle with respect to the
    /// North) of the vector formed by (x1, y1) and (x2, y2).
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1Geog, $y1Geog) is the longitude and latitude of Boston,
    /// and ($x2Geog, $y2Geog) is the longitude and latitude of New
    /// York City. The azimuth of the vector formed by these two
    /// coordinates in the direction New York City to Boston is
    /// 58.507421025167 degrees (moving clockwise from North).
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $azimuth = $coordSysGeog->GetAzimuth($x2Geog, $y2Geog, $x1Geog, $y1Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is
    /// defined by a starting coordinate and an \link azimuth azimuth \endlink
    /// (Angle with respect to North).
    ///
    /// \remarks
    /// The values contained in the given and returned coordinates
    /// are in units native to the coordinate system as specified in
    /// the \<name\> parameter in the UNIT[] clause of the coordinate
    /// system WKT. The azimuth is always in decimal degrees, and the
    /// distance is always in meters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(MgCoordinate coord, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(MgCoordinate coord, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(MgCoordinate coord, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord (MgCoordinate)
    /// An MgCoordinate that represents the start
    /// of the vector.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector of the
    /// desired coordinate in meters.
    ///
    /// \return
    /// Returns an MgCoordinate that lies the given distance along
    /// the vector.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Geog contains the longitude and latitude of Boston.
    /// The azimuth is -119.00856517599 degrees. That is the angle
    /// between a vector formed by Boston and Geographic North and a
    /// vector formed by Boston and the destination coordinate is
    /// 119.00856517599 degrees moving counterclockwise from North.
    /// The distance is 361777.95418396 meters. ($x2, $y2) is
    /// (-74.806394, 40.714169), the longitude and latitude of New
    /// York City.
    /// \code
    /// $x1 = -71.061342;
    /// $y1 = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $azimuth = -119.00856517599;
    /// $distance = 361777.95418396;
    /// $coord2Geog =  $coordSysGeog->GetCoordinate($coord1Geog, $azimuth, $distance);
    /// $x2 = $coord2Geog->GetX();
    /// $y2 = $coord2Geog->GetY();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);  

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is
    /// defined by a starting (x, y) coordinate and an \link azimuth azimuth \endlink
    /// (Angle with respect to North).
    ///
    /// \remarks
    /// The values contained in the given and returned coordinates
    /// are in units native to the coordinate system as specified in
    /// the \<name\> parameter in the UNIT[] clause of the coordinate
    /// system WKT. The azimuth is always in decimal degrees, and the
    /// distance is always in meters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double xStart, double yStart, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double xStart, double yStart, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double xStart, double yStart, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param xStart (double)
    /// The x value that represents the start of
    /// the vector.
    /// \param yStart (double)
    /// The y value that represents the start of
    /// the vector.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector of the
    /// desired coordinate in meters.
    ///
    /// \return
    /// Returns an MgCoordinate that lies the given distance along
    /// the vector.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1, $y1) the longitude and latitude of New York City. The
    /// azimuth is 58.507421025167 degrees. That is, the angle
    /// between a vector formed by New York City and Geographic North
    /// and a vector formed by New York City and the destination
    /// coordinate is 58.507421025167 degrees moving clockwise from
    /// North. The distance is 361777.95418396 meters. ($x2, $y2) is
    /// (-71.061342, 42.355892), the longitude and latitude of
    /// Boston.
    /// \code
    /// $x1 = -74.806394;
    /// $y1 = 40.714169;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $azimuth = 58.507421025167;
    /// $distance = 361777.95418396;
    /// $coord = $coordSysGeog->GetCoordinate($x1, $y1, $azimuth, $distance);
    /// $x2 = $coord->GetX();
    /// $y2 = $coord->GetY();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance);  

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the OGC WKT string representation of the coordinate
    /// system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string ToString();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the OGC WKT string.
    ///
    virtual STRING ToString();

    ////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system units.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetUnits();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system units string.
    ///
    STRING GetUnits();  /// __get

    //////////////////////////////////////////////
    /// \brief
    /// Gets the factor used to convert coordinate system units to meters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetUnitScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetUnitScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetUnitScale();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the factor used to convert coordinate system units to meters.
    ///
    double GetUnitScale();  /// __get

    ///////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum X value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetMinX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetMinX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetMinX();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system minimum X value.
    ///
    double GetMinX();  /// __get

    ///////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum Y value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetMinY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetMinY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetMinY();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system minimum Y value.
    ///
    double GetMinY();  /// __get

    ///////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum X value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetMaxX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetMaxX();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetMaxX();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system maximum X value.
    ///
    double GetMaxX();  /// __get

    ///////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum Y value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetMaxY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetMaxY();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetMaxY();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system maximum Y value.
    ///
    double GetMaxY();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system code.
    ///
    /// \remarks
    /// The system code corresponds to \<name\>, the first parameter,
    /// in the BNF for the various coordinate system terminal
    /// identifiers in the OGC WKT BNF. See \link WKTofSRS WKTofSRS \endlink.
    /// For example, the terminal for the geographic coordinate
    /// system definition is \<geographic cs\>, and its definition is
    /// "\<geographic cs\> = GEOGCS["\<name\>", \<datum\>, \<prime
    /// meridian\>, \<angular unit\> {,\<twin axes\>}
    /// {,\<authority\>}]."
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetCode();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetCode();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetCode();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system code string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $LLCode is "LL".
    /// \code
    /// $LLWkt = 'GEOGCS["LL",DATUM["",SPHEROID["WGS 84",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["Degrees",0.01745329252],PARAMETER["ADSK_minimum_longitude", -180],PARAMETER["ADSK_maximum_longitude", 180],]';
    /// $coordSysLL = $coordSysFactory->Create($LLWkt);
    /// $LLCode = $coordSysLL->GetCode();
    /// echo "$LLCoden";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    STRING GetCode();  /// __get

    //////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system description string.
    ///
    STRING GetDescription();  /// __get

    /////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system projection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetProjection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetProjection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetProjection();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system projection string.
    ///
    STRING GetProjection();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system projection description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetProjectionDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetProjectionDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetProjectionDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system projection description string.
    ///
    STRING GetProjectionDescription();  /// __get

    ////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system datum.
    ///
    /// \remarks
    /// The returned value comes from the first parameter (\<name\>)
    /// of the DATUM[] clause in the coordinate system WKT. See \link WKTofSRS WKTofSRS \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetDatum();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetDatum();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetDatum();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system datum string.
    ///
    ///
    STRING GetDatum();  /// __get

    ////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system datum description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetDatumDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetDatumDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetDatumDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system datum description string.
    ///
    STRING GetDatumDescription();  /// __get

    ////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system ellipsoid.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetEllipsoid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetEllipsoid();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetEllipsoid();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system ellipsoid string.
    ///
    STRING GetEllipsoid();  /// __get

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system ellipsoid description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetEllipsoidDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetEllipsoidDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetEllipsoidDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system ellipsoid description string.
    ///
    STRING GetEllipsoidDescription();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified wkt string into the corresponding
    /// coordinate system code.
    ///
    /// \remarks
    /// The origins of the system code is explained in the\link MgCoordinateSystem::ConvertCoordinateSystemCodeToWkt MgCoordinateSystem::ConvertCoordinateSystemCodeToWkt Method \endlink.
    ///
    /// \param ogcWkt
    /// The wkt string to convert.
    ///
    /// \return
    /// Returns the string of the corresponding coordinate system
    /// code.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $sysCode is "Longitude / Latitude (NAD 83)".
    /// \code
    /// $wktGeog = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM ["NAD 83", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]]';
    /// $coordSysGeog = $coordSysFactory->Create($wktGeog);
    /// $sysCode = $coordSysGeog->ConvertWktToCoordinateSystemCode($wktGeog);
    /// echo "$sysCoden");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    STRING ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinate system code into the
    /// corresponding wkt string.
    ///
    /// \remarks
    /// The system code corresponds to \<name\>, the first parameter,
    /// in the BNF for the various coordinate system terminal
    /// identifiers in the OGC WKT BNF. See \link WKTofSRS WKTofSRS \endlink.
    /// For example, the terminal for the geographic coordinate
    /// system definition is \<geographic cs\>, and its definition is
    /// "\<geographic cs\> = GEOGCS["\<name\>", \<datum\>, \<prime
    /// meridian\>, \<angular unit\> {,\<twin axes\>}
    /// {,\<authority\>}]."
    ///
    /// \param csCode
    /// The coordinate system code string to convert.
    ///
    /// \return
    /// Returns the string of the corresponding wkt.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $wkt is the same as $wktGeog.
    /// \code
    /// $wktGeog = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM ["NAD 83", SPHEROID ["GRS 80", 6378137, 298.257222101]], PRIMEM [ "Greenwich", 0.000000 ], UNIT ["Decimal Degree", 0.01745329251994330]]';
    /// $coordSysGeog = $coordSysFactory->Create($wktGeog);
    ///
    /// $sysCode = "Longitude / Latitude (NAD 83)";
    /// $wkt = $coordSysGeog->ConvertCoordinateSystemCodeToWkt($sysCode);
    ///
    /// echo "$wkn";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of the available coordinate system categories.
    ///
    /// \remarks
    /// This is a set of categories for partitioning the available
    /// coordinate systems into groups. Ideally this would be a
    /// static method but a limitation in the implementing technology
    /// dictates that it is a normal method.
    ///
    /// \return
    /// A list of the available categories.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// 0: Arbitrary X-Y Coordinate Systems
    ///
    /// 1: Lat Longs
    ///
    /// 2: Albania
    ///
    /// ...
    ///
    /// 221: World/Continental
    ///
    /// 222: Obsolete Coordinate Systems
    ///
    /// 223: Test Only
    /// \code
    /// $categories = $coordSysGeog->EnumerateCategories();
    /// $count = $categories->GetCount();
    /// for($i=0;$i<$count;$i++) {
    ///    $category = $categories->GetItem($i);
    ///    echo "$i: $categoryn");
    /// \}
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgStringCollection* EnumerateCategories();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available coordinate systems in the specified
    /// category.
    ///
    /// \remarks
    /// Ideally this would be a static method but a limitation in the
    /// implementing technology dictates that it is a normal method.
    ///
    /// \param category
    /// The category to retrieve the coordinate
    /// systems for.
    ///
    /// \return
    /// A batch property collection describing the available
    /// coordinate systems and their properties.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This code gets the collection of coordinate systems grouped
    /// under the category "Lat Longs". There are $count of them. The
    /// attributes of the coordinate system is expressed as a
    /// collection of properties (MgPropertyCollection). The property
    /// definitions used to express the coordinate system attributes
    /// are the same as those used to retrieve feature information
    /// from a data store.
    /// \n
    /// Here is some formatted output of the properties of one of the
    /// "Lat Longs" coordinate systems:
    /// <pre>
    /// Code is string "LL"
    /// Description is string "No datum, Latitude-Longitude; Degrees -180 to +180"
    /// Projection is string "LL"
    /// Projection Description is string "Null Projection, produces/processes Latitude & Longitude"
    /// Datum is string ""
    /// Datum Description is string "No Datum, cartographically referenced directly to an ellipsoid."
    /// Ellipsoid is string "WGS84"
    /// Ellipsoid Description is string "World Geodetic System of 1984, GEM 10C"
    /// </pre>
    /// Here is the code.
    /// \code
    /// $category = "Lat Longs";
    /// $batchCollection = $coordSys->EnumerateCoordinateSystem($category);
    /// $count = $batchCollection->GetCount();
    /// echo "$countn";
    /// $collection = $batchCollection->GetItem(0);
    /// printPropertyCollection($collection);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgBatchPropertyCollection* EnumerateCoordinateSystems(CREFSTRING category);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system category that this coordinate
    /// system belongs to.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual string GetCategory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual String GetCategory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetCategory();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// coordinate system category string
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $LLCategory is "Lat Long".
    /// \code
    /// $LLWkt = $coordSysGeog->ConvertCoordinateSystemCodeToWkt("LL");
    /// echo "$LLWktn");
    /// $coordSysLL = $coordSysFactory->Create($LLWkt);
    /// $LLCategory = $coordSysLL->GetCategory();
    /// echo "$LLCategoryn";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    STRING GetCategory();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified EPSG coordinate system code into the corresponding
    /// wkt string.
    ///
    /// \param code
    /// The EPSG coordinate system code to convert.
    ///
    /// \return
    /// String of the corresponding wkt
    ///
    STRING ConvertEpsgCodeToWkt(INT32 code);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified wkt string into the corresponding
    /// EPSG code string.
    ///
    /// \param wkt
    /// The wkt string to convert.
    ///
    /// \return
    /// Value of the corresponding EPSG code
    ///
    INT32 ConvertWktToEpsgCode(CREFSTRING wkt);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the base coordinate system library being used.
    ///
    STRING GetBaseLibrary();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the specified coordinate system is valid.
    ///
    /// \param ogcWkt
    /// The wkt string to validate.
    ///
    /// \return
    /// true if the specified coordinate system is valid, false otherwise.
    ///
    bool IsValid(CREFSTRING ogcWkt);

INTERNAL_API:
    MgCoordinateSystem();
    MgCoordinateSystem(CREFSTRING ogcWkt);
    virtual ~MgCoordinateSystem();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the library status.
    ///
    LibraryStatus GetLibraryStatus();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a coordinate system with the OGC WKT specified.
    /// Any existing definition will be cleared.
    ///
    void Initialize(CREFSTRING ogcWkt);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the internal coordinate system.
    ///
    const CCoordinateSystem* GetInternalCoordinateSystem() { return m_coordSys; }

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param lon
    /// The longitude value to convert.
    /// \param lat
    /// The latitude value to convert.
    /// \param x
    /// The converted longitude value.
    /// \param y
    /// The converted latitude value.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertFromLonLat(double lon, double lat, double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param lon
    /// The longitude values to convert.
    /// \param lat
    /// The latitude values to convert.
    /// \param x
    /// The converted longitude values.
    /// \param y
    /// The converted latitude values.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param x
    /// The value to convert to a longitude value.
    /// \param y
    /// The value to convert to a latitude value.
    /// \param lon
    /// The longitude value.
    /// \param lat
    /// The latitude value.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertToLonLat(double x, double y, double& lon, double& lat);

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param x
    /// The values to convert to a longitude values.
    /// \param y
    /// The values to convert to a latitude values.
    /// \param lon
    /// The longitude values.
    /// \param lat
    /// The latitude values.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize);

    /// Coordinate system information properties
    static const STRING CoordinateSystemCode;
    static const STRING CoordinateSystemDescription;
    static const STRING CoordinateSystemProjection;
    static const STRING CoordinateSystemProjectionDescription;
    static const STRING CoordinateSystemDatum;
    static const STRING CoordinateSystemDatumDescription;
    static const STRING CoordinateSystemEllipsoid;
    static const STRING CoordinateSystemEllipsoidDescription;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

private:
/// Data Members
    CCoordinateSystem* m_coordSys;

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystem;
};
/// \}

#endif //_MGCOORDINATESYSTEM_H_
