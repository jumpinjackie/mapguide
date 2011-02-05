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

#ifndef _MGCOORDINATESYSTEMMEASURE_H_
#define _MGCOORDINATESYSTEMMEASURE_H_

/// \defgroup MgCoordinateSystemMeasure MgCoordinateSystemMeasure
/// \ingroup Coordinate_System_classes
/// \{

//////////////////////////////////////////////////////////////////
/// \brief
/// Provides operations for measuring the distance between 2
/// coordinates and the azimuth of 2 coordinates as well as
/// calculating a coordinate position given an initial
/// coordinate, an azimuth, and a distance.
///
/// \remarks
/// If the coordinate system supports it, the ellipsoid
/// definition is used in the operations.
/// \n
/// To verify that the operations yield correct results, do the
/// following:
/// <ol>
///   <li> get the distance between 2 coordinate.
///   <li> get azimuth12, the azimuth from coordinate1 to
///      coordinate2
///   <li> get azimuth21,the azimuth from coordinate2 to
///      coordinate1
///   <li> get coordinate12, given coordinate1, azimuth12 and
///      distance; coordinate 12 should be the same as coordinate2
///   <li> get coordinate21, given coordinate2, azimuth21 and
///      distance; coordinate 21 should be the same as coordinate1
/// </ol>
///
class MG_GEOMETRY_API MgCoordinateSystemMeasure : public MgMeasure
{
    DECLARE_CLASSNAME(MgCoordinateSystemMeasure)

PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes the distance between two coordinates.
    ///
    /// \remarks
    /// The x and y values in the coordinate arguments are in
    /// coordinate system units.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetDistance(MgCoordinate coord1, MgCoordinate coord2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coord1 (MgCoordinate)
    /// An MgCoordinate object that specifies the
    /// first coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate object that specifies the
    /// second coordinate.
    ///
    /// \return
    /// Returns the distance between coord1 and coord2 as a double
    /// in meters.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1 is New York City, and $coord2 is Boston,
    /// Massachusetts. $distance is 361777.95418396 meters.
    /// \code
    /// $geometryFactory = new MgGeometryFactory();
    /// $coord1 = $geometryFactory->CreateCoordinateXY(-74.806394, 40.714169);
    /// $coord2 = $geometryFactory->CreateCoordinateXY(-71.061342, 42.355892);
    /// $distance = $coordSysMeasure->Getdistance($coord1, $coord2);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private MgGeometryFactory geometryFactory;
    /// private MgCoordinate geogCSXYCoord;
    /// private MgCoordinate geogCSX1Y1Coord;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSDistance = 2.0;
    /// private double distance;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// geometryFactory = new MgGeometryFactory();
    /// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
    /// geogCSX1Y1Coord = geometryFactory.CreateCoordinateXY(geogCSX1, geogCSY1);
    /// distance = geogCSMeasure.GetDistance(geogCSXYCoord, geogCSX1Y1Coord);
    /// // distance has the value of geogCSDistance
    /// isEquivalent = Math.Abs(distance - geogCSDistance) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual double GetDistance(MgCoordinate* coord1, MgCoordinate* coord2)=0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes the distance between two coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetDistance(double x1, double y1, double x2, double y2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x1 (double)
    /// The x value in coordinate system units of the
    /// first coordinate.
    /// \param y1 (double)
    /// The y value in coordinate system units of the
    /// first coordinate.
    /// \param x2 (double)
    /// The x value in coordinate system units of the
    /// second coordinate.
    /// \param y2 (double)
    /// The y value in coordinate system units of the
    /// second coordinate.
    ///
    /// \return
    /// Returns the distance between (x1, y1) and (x2, y2) as a
    /// double in meters.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// (-74.806394, 40.714169) is New York City, and (-71.061342,
    /// 42.355892) is Boston, Massachusetts. $distance is
    /// 361777.95418396 meters.
    /// \code
    /// $distance = $coordSysMeasure->Getdistance(-74.806394, 40.714169, -71.061342, 42.355892);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ////
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSDistance = 2.0;
    /// private double distance;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// distance = geogCSMeasure.GetDistance(geogCSX, geogCSY, geogCSX1, geogCSY1);
    /// // distance has the value of geogCSDistance
    /// isEquivalent = Math.Abs(distance - geogCSDistance) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual double GetDistance(double x1, double y1, double x2, double y2)=0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes the angle with respect to north of a vector formed
    /// by two coordinates.
    ///
    /// \remarks
    /// A positive angle represents a measurement from north in a
    /// clockwise direction towards the vector. A negative angle
    /// represents a measurement from north in a counterclockwise
    /// direction towards the vector.
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
    /// An MgCoordinate in coordinate system units
    /// that specifies the first coordinate.
    /// \param coord2 (MgCoordinate)
    /// An MgCoordinate in coordinate system units
    /// that specifies the second coordinate.
    ///
    /// \return
    /// Returns the azimuth (Angle with respect to
    /// North) of the vector formed by coord1 and coord2.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// $coord1 is New York City, and $coord2 is Boston,
    /// Massachusetts. $azimuth12 is 58.507421025167, and $azimuth21
    /// = -119.00856518347.
    /// \code
    /// $geometryFactory = new MgGeometryFactory();
    /// $coord1 = $geometryFactory->CreateCoordinateXY(-74.806394, 40.714169);
    /// $coord2 = $geometryFactory->CreateCoordinateXY(-71.061342, 42.355892);
    /// $azimuth12 = $coordSysMeasure->GetAzimuth($coord1, $coord2);
    /// $azimuth21 = $coordSysMeasure->GetAzimuth($coord2, $coord1);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private MgGeometryFactory geometryFactory;
    /// private MgCoordinate geogCSXYCoord;
    /// private MgCoordinate geogCSX1Y1Coord;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSAzimuth = 10.0;
    /// private double azimuth;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// geometryFactory = new MgGeometryFactory();
    /// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
    /// geogCSX1Y1Coord = geometryFactory.CreateCoordinateXY(geogCSX1, geogCSY1);
    /// azimuth = geogCSMeasure.GetAzimuth(geogCSXYCoord, geogCSX1Y1Coord);
    /// // azimuth has the value of geogCSAzimuth
    /// isEquivalent = Math.Abs(azimuth - geogCSAzimuth) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)=0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes the angle with respect to the north of a vector
    /// formed by two coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual double GetAzimuth(double lon1, double lat1, double lon2, double lat2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual double GetAzimuth(double lon1, double lat1, double lon2, double lat2);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual double GetAzimuth(double lon1, double lat1, double lon2, double lat2);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x1 (double)
    /// The x value in coordinate system units of the
    /// first coordinate.
    /// \param y1 (double)
    /// The y value in coordinate system units of the
    /// first coordinate.
    /// \param x2 (double)
    /// The x value in coordinate system units of the
    /// second coordinate.
    /// \param y2 (double)
    /// The y value in coordinate system units of the
    /// second coordinate.
    ///
    /// \return
    /// Returns the azimuth (Angle with respect to the
    /// North) of the vector formed by (lon1, lat1) and (lon2, lat2).
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// (-74.806394, 40.714169) is New York City, and (-71.061342,
    /// 42.355892) is Boston, Massachusetts. $azimuth12 is
    /// 58.507421025167, and $azimuth21 = -119.00856518347.
    /// \code
    /// $azimuth12 = $coordSysMeasure->GetAzimuth(-74.806394, 40.714169, -71.061342, 42.355892);
    /// $azimuth21 = $coordSysMeasure->GetAzimuth(-71.061342, 42.355892, -74.806394, 40.714169);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSAzimuth = 10.0;
    /// private double azimuth;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// azimuth = geogCSMeasure.GetAzimuth(geogCSX, geogCSY, geogCSX1, geogCSY1);
    /// // azimuth has the value of geogCSDistance
    /// isEquivalent = Math.Abs(azimuth - geogCSAzimuth) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual double GetAzimuth(double x1, double y1, double x2, double y2)=0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate given a distance along a vector that is
    /// defined by a starting coordinate and an azimuth
    /// (Angle with respect to the North).
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
    /// An MgCoordinate object in coordinate system
    /// units that represents the start of the
    /// vector.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to the North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector to the desired
    /// coordinate in meters.
    ///
    /// \return
    /// Returns an MgCoordinate object containing the x and y values
    /// in coordinate system units of a coordinate that lies the
    /// given distance along the vector.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgCoordinateSystemMeasure::GetAzimuth MgCoordinateSystemMeasure::GetAzimuth Method (MgCoordinate*, MgCoordinate*) \endlink
    /// for the code creating $azimuth12 and $azimuth21. See \link MgCoordinateSystemMeasure::GetDistance MgCoordinateSystemMeasure::GetDistance Method (MgCoordinate*, MgCoordinate*) \endlink
    /// for the code computing the distance between the coordinates.
    /// The longitude and latitude of $coord12 is the same as the
    /// longitude and latitutde of $coord2, and the longitude and
    /// latitude of $coord21 is the same as the longitude and
    /// latitude of $coord1.
    /// \code
    /// $geometryFactory = new MgGeometryFactory();
    /// $x1 = -74.806394;
    /// $y1 = 40.714169;
    /// $coord1 = $geometryFactory->CreateCoordinateXY($x1, $y1);
    /// $coord12 = $coordSysMeasure->GetCoordinate($coord1, $azimuth12, $distance);
    /// $x2 = -71.061342;
    /// $y2 = 42.355892;
    /// $coord2 = $geometryFactory->CreateCoordinateXY($x2, $y2);
    /// $coord21 = $coordSysMeasure->GetCoordinate($coord2, $azimuth21, $distance);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private MgGeometryFactory geometryFactory;
    /// private MgCoordinate geogCSXYCoord;
    /// private MgCoordinate geogCSX1Y1Coord;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSAzimuth = 10.0;
    /// private double geogCSDistance = 2.0;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// geometryFactory = new MgGeometryFactory();
    /// geogCSXYCoord = geometryFactory.CreateCoordinateXY(geogCSX, geogCSY);
    /// geogCSX1Y1Coord = geogCSMeasure.GetCoordinate(geogCSXYCoord, geogCSAzimuth, geogCSDistance);
    /// // the X and Y values of geogCSX1Y1Coord are equal to geogCSX1 and geogCSY1
    /// isEquivalent = Math.Abs(geogCSX1 - geogCSX1Y1Coord.GetX()) < tolerance && Math.Abs(geogCSY1 - geogCSX1Y1Coord.GetY()) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
     virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance)=0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is
    /// defined by a starting coordinate and an azimuth
    /// (Angle with respect to the North).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double lon, double lat, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double lon, double lat, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate GetCoordinate(double lon, double lat, double azimuth, double distance);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param xStart (double)
    /// The x value in coordinate system units of the
    /// starting coordinate.
    /// \param yStart (double)
    /// The y value in coordinate system units of the
    /// starting coordinate.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to the North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector to the desired
    /// coordinate in meters.
    ///
    /// \return
    /// Returns an MgCoordinate object containing the x and y value
    /// in ecoordinate system units of a coordinate that lies the
    /// given distance along the vector.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgCoordinateSystemMeasure::GetAzimuth MgCoordinateSystemMeasure::GetAzimuth Method (MgCoordinate*, MgCoordinate*) \endlink
    /// for the code creating $azimuth12 and $azimuth21. See \link MgCoordinateSystemMeasure::GetDistance MgCoordinateSystemMeasure::GetDistance Method (MgCoordinate*, MgCoordinate*) \endlink
    /// for the code computing the distance between the coordinates.
    /// The longitude and latitude of $coord12 is (-71.061342,
    /// 42.355892), and the longitude and latitude of $coord21 is
    /// (-74.806394, 40.714169).
    /// \code
    /// $coord12 = $coordSysMeasure->GetCoordinate(-74.806394, 40.714169, $azimuth12, $distance);
    /// $coord21 = $coordSysMeasure->GetCoordinate(-71.061342, 42.355892, $azimuth21, $distance);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemMeasure geogCSMeasure;
    /// private MgCoordinate geogCSX1Y1Coord;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double geogCSX1 = -159.721535121256;
    /// private double geogCSY1 = 24.0028259520524;
    /// private double geogCSAzimuth = 10.0;
    /// private double geogCSDistance = 2.0;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// geogCSX1Y1Coord = geogCSMeasure.GetCoordinate(geogCSX, geogCSY, geogCSAzimuth, geogCSDistance);
    /// // the X and Y values of geogCSX1Y1Coord are equal to geogCSX1 and geogCSY1
    /// isEquivalent = Math.Abs(geogCSX1 - geogCSX1Y1Coord.GetX()) < tolerance && Math.Abs(geogCSY1 - geogCSX1Y1Coord.GetY()) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance)=0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes the envelope of the MgCoordinateSystemMeasure
    /// object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgEnvelope GetEnvelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgEnvelope GetEnvelope();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgEnvelope GetEnvelope();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// For this
    /// particular $coordSysMeasure, the value of $lowerLeft is
    /// (-180, -90) and the value of $upperRight is (180, 90). The
    /// width of the envelope is 360, and the height is 180.
    /// \code
    /// $envelope = $coordSysMeasure->GetEnvelope();
    /// $lowerLeft = $envelope->GetLowerLeftCoordinate();
    /// $upperRight = $envelope->GetUpperRightCoordinate();
    /// $width = $envelope->GetWidth();
    /// $height = $envelope->GetHeight();
    /// $depth = $envelope->GetDepth();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgGeometryFactory geometryFactory;
    /// private MgEnvelope geogCSEnvelope;
    /// private double geogCSMaxX = 180;
    /// private double geogCSMaxY = 90;
    /// private double geogCSMinX = -180;
    /// private double geogCSMinY = -90;
    /// private MgCoordinate lowerLeft;
    /// private MgCoordinate upperRight;
    /// private double tolerance = 0.001;
    /// private Boolean isEquivalent;
    ///
    /// envelope = geogCSMeasure.GetEnvelope();
    /// lowerLeft = envelope.GetLowerLeftCoordinate();
    /// upperRight = envelope.GetUpperRightCoordinate();
    /// // the lower left and upper right coordinates of the envelope are equal to the coordinate system's lower left and upper right coordinates
    /// isEquivalent = Math.Abs(geogCSMinX - lowerLeft.GetX()) < tolerance && Math.Abs(geogCSMinY - lowerLeft.GetY()) < tolerance;
    /// isEquivalent = Math.Abs(geogCSMaxX - upperRight.GetX()) < tolerance && Math.Abs(geogCSMaxY - upperRight.GetY()) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemMeasureFailedException
    ///
    virtual MgEnvelope* GetEnvelope()=0;  /// __get

INTERNAL_API:
    virtual MgCoordinateSystem* GetCoordSys()=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

/// Data Members
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemMeasure;
};
/// \}

#endif //_MGCOORDINATESYSTEMMEASURE_H_
