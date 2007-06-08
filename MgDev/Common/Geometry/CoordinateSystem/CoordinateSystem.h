//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
/// is of type MgCoordinateSystemType::Projected.
/// \n
/// MeasureEuclideanDistance, MeasureGreatCircleDistance, and
/// GetCoordinate return values expressed in native units, that
/// is, the value of \<name\> parameter in the UNIT[] clause in
/// the coordinate system WKT string. See \link WKTofSRS WKTofSRS \endlink
/// and \link CSTestValues CSTestValues \endlink.
/// \n
/// To get the distance between two coordinates in a projected or geography
/// coordinate system, you can use either MeasureGreatCircleDistance and
/// MeasureEuclideanDistance. Here are some results.
/// \li Geography CS Euclidean: 224367.088836791
/// \li Projected CS Euclidean: 222509.763506044
/// \li Geography CS Great Circle: 222638.981586225
/// \li Projected CS Great Circle: 222574.361771088
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
    /// $xGeog = -71.061342;
    /// $yGeog = 42.355892;
	/// $geometryFactory = new MgGeometryFactory();
    /// $coordinate = $geometryFactory->CreateCoordinateXY($xGeog, $yGeog);
	/// // See the example code for the creation of the $coordSys object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $convertedCoordinate = $coordSys->ConvertFromLonLat($coordinate);
    /// $xConv = $convertedCoordinate->GetX();
    /// $yConv = $convertedCoordinate->GetY();
    /// echo  "($xGeog, $yGeog) to ($xConv, $yConv)\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem projCS;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinate projCSLonLatXYCoord;
	/// // the longitude value to be converted
	/// private double projCSLonX = -160.101421317;
	/// // the latitutde value to be converted
    /// private double projCSLatY = 22.0234263273;
	/// 
	/// // the X value expected from the conversion
	/// private double projCSX = 386323.97632;
	/// // the Y value expected from the conversion
    /// private double projCSY = 2435829.67936;
	///
	/// private MgCoordinate ConvertedCoord;
	/// private Boolean isEquivalent;
	/// private double tolerance = 0.001;
	///
	/// geometryFactory = new MgGeometryFactory();
	/// projCSLonLatXYCoord = geometryFactory.CreateCoordinateXY(projCSLonX, projCSLatY);
	/// // See the example code for the creation of the projCS MgCoordinateSystem object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// ConvertedCoord = projCS.ConvertFromLonLat(projCSLonLatXYCoord);
	/// isEquivalent = Math.Abs(projCSX - ConvertedCoord.GetX()) < tolerance && Math.Abs(projCSY - ConvertedCoord.GetY()) < tolerance;
	/// \endcode
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
    /// \code
	/// $geometryFactory = new MgGeometryFactory();
    /// $xProj = 824399.31463498;
    /// $yProj = 4696596.0339483;
    /// $coordinate = $geometryFactory->CreateCoordinateXY($xProj, $yProj);
	/// // See the example code for the creation of the $coordSys object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $convertedCoordinate = $coordSys->ConvertToLonLat($coordinate);
    /// $xConv = $convertedCoordinate->GetX();
    /// $yConv = $convertedCoordinate->GetY();
    /// echo  "($xProj, $yProj) to ($xConv, $yConv)\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem projCS;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinate projCSXYCoord;
	/// // the X value to be converted
	/// private double projCSX = 386323.97632;
	/// // the Y value to be converted
    /// private double projCSY = 2435829.67936;
	///
	/// // the longitude value expected from conversion
	/// private double projCSLonX = -160.101421317;
	/// // the latitude value expected from conversion
    /// private double projCSLatY = 22.0234263273;
	/// private MgCoordinate ConvertedCoord;
	/// private Boolean isEquivalent;
	/// private double tolerance = 0.001;
	///
	/// geometryFactory = new MgGeometryFactory();
	/// projCSXYCoord = geometryFactory.CreateCoordinateXY(projCSX, projCSY);
	/// projCSLonLatXYCoord = geometryFactory.CreateCoordinateXY(projCSLonX, projCSLatY);
	/// // See the example code for the creation of the projCS MgCoordinateSystem object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// ConvertedCoord = projCS.ConvertToLonLat(projCSXYCoord);
	/// isEquivalent = Math.Abs(projCSLonX - ConvertedCoord.GetX()) < tolerance && Math.Abs(projCSLatY - ConvertedCoord.GetY()) < tolerance;
	/// \endcode
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
	/// // See the example code for the creation of the projCS MgCoordinateSystem object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $csUnits = 0.0089831528411952;
    /// $meters = $coordSys->ConvertCoordinateSystemUnitsToMeters($csUnits);
    /// echo "$meters\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem projCS;
	/// private MgCoordinateSystem geogCS;
	/// private double geogCSDegreeInMeters = 111319.49079327358;
	///
	/// // See the example code for the creation of the projCS and geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	///
	/// // convert one degree to meters
	/// // The conversion factor specified by the well-known text specification for the geographic coordinate system
	/// // in the UNIT clause is the scalar factor to convert degrees to radians
	/// // Here we convert a degree into meters at the earths surface
	/// // by dividing the earth's circumference by 360
	/// // the result in the meters variable should be approximately equal to the
	/// // value in the geogCSDegreeInMeters variable
	/// double meters = geogCS.ConvertCoordinateSystemUnitsToMeters(1);
	///
	/// // The conversion factor specified by the well-known text specification for the projected coordinate system
	/// // in the UNIT clause is the scalar factor to convert meters to meters
	/// // So the value of the units variable is 1.0.
	/// double units = projCS.ConvertMetersToCoordinateSystemUnits(1);
	/// \endcode
    virtual double ConvertCoordinateSystemUnitsToMeters(double units);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the value in meters to coordinate system units.
    ///+
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
    /// This code converts 1 meter into 0.0000089831528411952 decimal degrees.
    /// \code
	/// // See the example code for the creation of the projCS MgCoordinateSystem object
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $csUnits = $coordSys->ConvertMetersToCoordinateSystemUnits(1000);
    /// echo "$csUnits\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem projCS;
	/// private MgCoordinateSystem geogCS;
	/// private double geogCSDegreeInMeters = 111319.49079327358;
	///
	/// // See the example code for the creation of the projCS and geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	///
	/// // convert 111319.49079327358 meters to degrees
	/// // The conversion factor specified by the well-known text specification for the geographic coordinate system
	/// // in the UNIT clause is the scalar factor to convert degrees to radians
	/// // Here we convert 111319.49079327358 meters at the earths surface into degrees
	/// // 111319.49079327358 meters is an accepted result of dividing the earth's circumference by 360
	/// // the result in the units variable should be 1.0.
	/// double units = geogCS.ConvertMetersToCoordinateSystemUnits(geogCSDegreeInMeters);
	///
	/// // The conversion factor specified by the well-known text specification for the projected coordinate system
	/// // in the UNIT clause is the scalar factor to convert meters to meters
	/// // So the value of the meters variable is 1.0.
	/// double meters = projCS.ConvertCoordinateSystemUnitsToMeters(1);
	/// \endcode
    virtual double ConvertMetersToCoordinateSystemUnits(double meters);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance between two coordinates in coordinate system units.
    ///
	/// \remarks
	/// The measurement of euclidean distance is specific to a projected coordinate system.
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
    /// An MgCoordinate that defines the starting coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate that defines the ending coordinate.
    ///
    /// \return
    /// Returns the euclidean distance from coord1 to coord2 in
    /// coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Proj contains the longitude and latitude of Boston.
    /// $coord2Proj contains the longitude and latitude of New York City.
    /// \code
	/// $geometryFactory = new MgGeometryFactory();
    /// $x1Proj = 824390.29381426;
    /// $y1Proj = 4696809.9055963;
    /// $coord1Proj = $geometryFactory->CreateCoordinateXY($x1Proj, $y1Proj);
    /// $x2Proj = 516352.70285172;
    /// $y2Proj = 4507045.9198563;
    /// $coord2Proj = $geometryFactory->CreateCoordinateXY($x2Proj, $y2Proj);
	///
	/// // See the example code for the creation of the $coordSysProj MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // $distance is 361797.633671 meters.
    /// $distance = $coordSysProj->MeasureEuclideanDistance($coord1Proj, $coord2Proj);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinateSystem projCS;
	/// private MgCoordinate projCSXYCoord;
	/// private double projCSX = 386323.97632;
	/// private double projCSY = 2435829.67936;
	/// private MgCoordinate projCSX1Y1Coord;
	/// private double projCSX1 = 426600.47562833;
	/// private double projCSY1 = 2654663.85964037;
	/// private double projCSDistance = 222509.763506044;

	/// geometryFactory = new MgGeometryFactory();
	/// projCSXYCoord = geometryFactory.CreateCoordinateXY(projCSX, projCSY);
	/// projCSX1Y1Coord = geometryFactory.CreateCoordinateXY(projCSX1, projCSY1);
	/// // The value of the distance variable is that of the projCSDistance variable in meters.
	/// double distance = projCS.MeasureEuclideanDistance(projCSXYCoord, projCSX1Y1Coord);
	/// \endcode
	///
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance between (x1, y1) and (x2, y2)
	/// in coordinate system units.
	///
	/// \remarks
	/// The measurement of euclidean distance is specific to a projected coordinate system.
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
    /// The x value of the starting coordinate.
    /// \param y1 (double)
    /// The y value of the starting coordinate.
    /// \param x2 (double)
    /// The x value of the ending coordinate.
    /// \param y2 (double)
    /// The y value of the ending coordinate.
    ///
    /// \return
    /// Returns the euclidean distance from (x1, y1) to (x2, y2) in
    /// coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1Proj, $y1Proj) is the longitude and latitude of Boston.
    /// ($x2Proj, $y2Proj) is the longitude and latitude of New York City.
    /// \code
    /// $x1Proj = 824390.29381426;
    /// $y1Proj = 4696809.9055963;
    /// $x2Proj = 516352.70285172;
    /// $y2Proj = 4507045.9198563;
	///
	/// // See the example code for the creation of the $coordSysProj MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // $distance is 361797.633671 meters.
    /// $distance = $coordSysProj->MeasureEuclideanDistance($x1Proj, $y1Proj, $x2Proj, $y2Proj);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem projCS;
	/// private double projCSX = 386323.97632;
	/// private double projCSY = 2435829.67936;
	/// private double projCSX1 = 426600.47562833;
	/// private double projCSY1 = 2654663.85964037;
	/// private double projCSDistance = 222509.763506044;
	///
	/// // See the example code for the creation of the projCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the distance variable is that of the projCSDistance variable in meters.
	/// double distance = projCS.MeasureEuclideanDistance(projCSX, projCSY, projCSX1, projCSY1);
	/// \endcode
	///
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the great circle distance between two coordinates
	/// in coordinate system units.
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system.
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
    /// An MgCoordinate that defines the starting coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate that defines the ending coordinate.
    ///
    /// \return
    /// Returns the great circle distance from coord1 to coord2 in coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Geog contains the longitude and latitude of Boston.
    /// $coord2Geog contains the longitude and latitude of New York City.
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $coord2Geog = $geometryFactory->CreateCoordinateXY($x2Geog, $y2Geog);
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// // $distance is 361777.95418396 meters.
    /// $distance = $coordSysGeog->MeasureGreatCircleDistance($coord1Geog, $coord2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinateSystem geogCS;
	/// private MgCoordinate geogCSXYCoord;
	/// private MgCoordinate geogCSX1Y1Coord;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSDistance = 2.0;
	///
	/// geometryFactory = new MgGeometryFactory();
	/// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
	/// geogCSX1Y1Coord = geometryFactory.CreateCoordinateXY(geogCSX1, geogCSY1);
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the distance variable is that of the geogCSDistance variable in degrees.
	/// double distance = geogCS.MeasureGreatCircleDistance(geogCSXYCoord, geogCSX1Y1Coord);
	/// \endcode
	///
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the distance between two coordinates (x1, y1) and (x2, y2)
	/// in coordinate system units.
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system.
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
    /// The x value of the starting coordinate.
    /// \param y1 (double)
    /// The y value of the starting coordinate.
    /// \param x2 (double)
    /// The x value of the ending coordinate.
    /// \param y2 (double)
    /// The y value of the ending coordinate.
    ///
    /// \return
    /// Returns the great circle distance from (x1, y1) to (x2, y2)
    /// in coordinate system units.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// ($x1Geog, $y1Geog) is the longitude and latitude of Boston.
    /// ($x2Geog, $y2Geog) is the longitude and latitude of New York City.
    /// \code
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// // $distance is 361777.95418396 meters.
    /// $distance = $coordSysGeog->MeasureGreatCircleDistance($x1Geog, $y1Geog, $x2Geog, $y2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem geogCS;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSDistance = 2.0;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the distance variable is that of the geogCSDistance variable in degrees.
	/// double distance = geogCS.MeasureGreatCircleDistance(geogCSX, geogCSY, geogCSX1, geogCSY1);
	/// \endcode
	///
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by
    /// two coordinates.
	///
	/// \remarks
	/// This method is specific to a geographic coordinate system.
	/// The angle is positive moving clockwise from North and
	/// negative moving counterclockwise from North.
	/// If the first coordinate is above and to the left of the second,
	/// or below and to the left, the movement is clockwise.
	/// If the first coordinate is above and to the right,
	/// or below and to the right, the movement is counterclockwise. 
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
    /// An MgCoordinate object that specifies the first coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate object that specifies the
    /// second coordinate.
    ///
    /// \return
    /// Returns the azimuth (Angle with respect to 
    /// North) of the vector formed by coord1 and coord2.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1Geog contains the longitude and latitude of Boston,
    /// and $coord2Geog contains the longitude and latitude of New York City.
	/// The azimuth of the vector formed by these two
    /// coordinates in the direction Boston to New York City is
    /// -119.00856517599 degrees (moving counterclockwise from
    /// North).
    /// \code
	/// $geometryFactory = new MgGeometryFactory();
    /// $x1Geog = -71.061342;
    /// $y1Geog = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $x2Geog = -74.806394;
    /// $y2Geog = 40.714169;
    /// $coord2Geog = $geometryFactory->CreateCoordinateXY($x2Geog, $y2Geog);
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $azimuth = $coordSysGeog->GetAzimuth($coord1Geog, $coord2Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinateSystem geogCS;
	/// private MgCoordinate geogCSXYCoord;
	/// private MgCoordinate geogCSX1Y1Coord;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSAzimuth = 10.0;
	///
	/// geometryFactory = new MgGeometryFactory();
	/// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
	/// geogCSX1Y1Coord = geometryFactory.CreateCoordinateXY(geogCSX1, geogCSY1);
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the azimuth variable is that of the geogCSAzimuth variable in degrees,
	/// // moving clockwise from North.
	/// double azimuth = geogCS.GetAzimuth(geogCSXYCoord, geogCSX1Y1Coord);
	/// \endcode
	///
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by
    /// two coordinates.
    ///
	/// \remarks
	/// This method is specific to a geographic coordinate system.
	/// The angle is positive moving clockwise from North and
	/// negative moving counterclockwise from North.
	/// If the first coordinate is above and to the left of the second,
	/// or below and to the left, the movement is clockwise.
	/// If the first coordinate is above and to the right,
	/// or below and to the right, the movement is counterclockwise. 
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
    /// The x value of the first coordinate.
    /// \param y1 (double)
    /// The y value of  the first coordinate.
    /// \param x2 (double)
    /// The x value of the second coordinate.
    /// \param y2 (double)
    /// The y value of the second coordinate.
    ///
    /// \return
    /// Returns the azimuth (Angle with respect to the
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
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $azimuth = $coordSysGeog->GetAzimuth($x2Geog, $y2Geog, $x1Geog, $y1Geog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem geogCS;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSAzimuth = 10.0;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the azimuth variable is that of the geogCSAzimuth variable in degrees
	/// // moving clockwise from North.
	/// double azimuth = geogCS.GetAzimuth(geogCSX, geogCSY, geogCSX1, geogCSY1);
	/// \endcode
	///
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is
    /// defined by a starting coordinate and an azimuth
    /// (Angle with respect to North).
    ///
    /// \remarks
	/// This method is specific to a geographic coordinate system.
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
    /// The distance in meters along the vector to the
    /// desired coordinate.
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
	/// $geometryFactory = new MgGeometryFactory();
    /// $x1 = -71.061342;
    /// $y1 = 42.355892;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $azimuth = -119.00856517599;
    /// $distance = 361777.95418396;
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $coord2Geog =  $coordSysGeog->GetCoordinate($coord1Geog, $azimuth, $distance);
    /// $x2 = $coord2Geog->GetX();
    /// $y2 = $coord2Geog->GetY();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinateSystem geogCS;
	/// private MgCoordinate geogCSXYCoord;
	/// private MgCoordinate geogCSX1Y1Coord;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSDistance = 2.0;
	/// private double geogCSAzimuth = 10.0;
	///
	/// geometryFactory = new MgGeometryFactory();
	/// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The X and Y values of the geogCSX1Y1Coord variable are approximately the values of geogCSX1 and geogCSY1.
	/// geogCSX1Y1Coord = geogCS.GetCoordinate(geogCSXYCoord, geogCSAzimuth, geogCSDistance);
	/// \endcode
	///
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);  

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is
    /// defined by a starting (x, y) coordinate and an azimuth
    /// (Angle with respect to North).
    ///
    /// \remarks
	/// This method is specific to a geographic coordinate system.
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
    /// The x value of a coordinate that represents the start of
    /// the vector.
    /// \param yStart (double)
    /// The y value of a coordinate that represents the start of
    /// the vector.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance in meters along the vector to the
    /// desired coordinate.
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
	/// $geometryFactory = new MgGeometryFactory();
    /// $x1 = -74.806394;
    /// $y1 = 40.714169;
    /// $coord1Geog = $geometryFactory->CreateCoordinateXY($x1Geog, $y1Geog);
    /// $azimuth = 58.507421025167;
    /// $distance = 361777.95418396;
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $coord = $coordSysGeog->GetCoordinate($x1, $y1, $azimuth, $distance);
    /// $x2 = $coord->GetX();
    /// $y2 = $coord->GetY();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgGeometryFactory geometryFactory;
	/// private MgCoordinateSystem geogCS;
	/// private MgCoordinate geogCSX1Y1Coord;
	/// private double geogCSX = -160.101421317;
	/// private double geogCSY = 22.0234263273;
	/// private double geogCSX1 = -159.721535121256;
	/// private double geogCSY1 = 24.0028259520524;
	/// private double geogCSDistance = 2.0;
	/// private double geogCSAzimuth = 10.0;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The X and Y values of the geogCSX1Y1Coord variable are approximately the values of geogCSX1 and geogCSY1.
	/// geogCSX1Y1Coord = geogCS.GetCoordinate(geogCSX, geogCSY, geogCSAzimuth, geogCSDistance);
	/// \endcode
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
    /// $LLWkt = 'GEOGCS["LL",DATUM[...]...]';
    /// $coordSysLL = $coordSysFactory->Create($LLWkt);
    /// $LLCode = $coordSysLL->GetCode();
    /// echo "$LLCode\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private String geogCSWkt = "GEOGCS[\"LL83\",DATUM[...]...]";
	/// private String geogCSCSCode = "LL83";
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the code variable is the value of the geogCSCSCode variable.
	/// String code = geogCS.GetCode();
	/// \endcode
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
    /// Returns the coordinate system code string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $sysCode is "Longitude / Latitude (NAD 83)".
    /// \code
    /// $wktGeog = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM [...]...]';
    /// $coordSysGeog = $coordSysFactory->Create($wktGeog);
    /// $sysCode = $coordSysGeog->ConvertWktToCoordinateSystemCode($wktGeog);
    /// echo "$sysCode\n");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private String geogCSWkt = "GEOGCS[\"LL83\",DATUM[...]...]";
	/// private String geogCSCSCode = "LL83";
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the code variable is the value of the geogCSCSCode variable.
	/// String code = geogCS.ConvertWktToCoordinateSystemCode(geogCSWkt);
	/// \endcode
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
    /// $wktGeog = 'GEOGCS [ "Longitude / Latitude (NAD 83)", DATUM [...]...]';
    /// $coordSysGeog = $coordSysFactory->Create($wktGeog);
    ///
    /// $sysCode = "Longitude / Latitude (NAD 83)";
    /// $wkt = $coordSysGeog->ConvertCoordinateSystemCodeToWkt($sysCode);
    ///
    /// echo "$wkt\n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private String geogCSWkt = "GEOGCS[\"LL83\",DATUM[...]...]";
	/// private String geogCSCSCode = "LL83";
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // The value of the wkt variable is the value of the geogCSWkt variable.
	/// String wkt = geogCS.ConvertCoordinateSystemCodeToWkt(geogCSCSCode);
	/// \endcode
	///
    STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of the available coordinate system categories.
    ///
    /// \remarks
    /// This is a set of categories for partitioning the available
    /// coordinate systems into groups.
    ///
    /// \return
    /// A list of the available categories.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
	/// The following is a partial list of the categories:
    /// 0: Arbitrary X-Y Coordinate Systems
    /// 1: Lat Longs
    /// 2: Albania
    /// ...
    /// 221: World/Continental
    /// 222: Obsolete Coordinate Systems
    /// 223: Test Only
    /// \code
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $categories = $coordSysGeog->EnumerateCategories();
    /// $count = $categories->GetCount();
    /// for($i=0;$i<$count;$i++) {
    ///    $category = $categories->GetItem($i);
    ///    echo "$i: $category\n");
    /// }
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem geogCS;
	/// private MgStringCollection strCollection = null;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // Two of the categories found in the output are "Lat Longs" and "UTM, NAD83 Datum".
	/// strCollection = geogCS.EnumerateCategories();
	/// \endcode
	///
    MgStringCollection* EnumerateCategories();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available coordinate systems in the specified
    /// category.
    ///
    /// \param category
    /// The category to retrieve the coordinate systems for.
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
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $batchCollection = $coordSys->EnumerateCoordinateSystem($category);
    /// $count = $batchCollection->GetCount();
    /// echo "$count\n";
    /// $collection = $batchCollection->GetItem(0);
    /// printPropertyCollection($collection);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem geogCS;
	/// private String geogCSCategory = "Lat Longs";
	/// private MgBatchPropertyCollection batchPropCollection;
	/// private MgPropertyCollection propCollection;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // Each MgPropertyCollection in the MgBatchPropertyCollection contains a set of
	/// // MgProperty objects related to a Lat/Long coordinate system
	/// // One of the properties is the system code.
	/// // Two of the codes found in the output are "LL83" and "LL84".
	/// batchPropCollection = geogCS.EnumerateCoordinateSystems(geogCSCategory);
	/// \endcode
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
	/// // See the example code for the creation of the $coordSysGeog MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
    /// $LLWkt = $coordSysGeog->ConvertCoordinateSystemCodeToWkt("LL");
    /// echo "$LLWktn");
    /// $coordSysLL = $coordSysFactory->Create($LLWkt);
    /// $LLCategory = $coordSysLL->GetCategory();
    /// echo "$LLCategoryn";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
	/// <h3>C#</h3>
	/// \code
	/// using OSGeo.MapGuide;
	/// private MgCoordinateSystem geogCS;
	/// private String geogCSCategory = "Lat Longs";
	/// String category;
	///
	/// // See the example code for the creation of the geogCS MgCoordinateSystem objects
	/// // in the comments on the Create method of the MgCoordinateSystemFactory class.
	/// // category has the value of geogCSCategory.
	/// category = geogCS.GetCategory();
	/// \endcode
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
