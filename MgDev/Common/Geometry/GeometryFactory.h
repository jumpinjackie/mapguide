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

#ifndef _MGGEOMETRYFACTORY_H_
#define _MGGEOMETRYFACTORY_H_

class MgGeometryFactory;
template class MG_GEOMETRY_API Ptr<MgGeometryFactory>;

/// \defgroup MgGeometryFactory MgGeometryFactory
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////////
/// \brief
/// The MgGeometryFactory class is used to construct objects
/// whose classes are derived from MgGeometry.
///
/// \remarks
/// The starting point for the construction of the MgGeometry
/// objects is user-supplied numbers of type double or integer.
/// The lexical analyzer converts integers to doubles. The
/// numbers are passed to one of the CreateCoordinate????()
/// methods where ???? is one of XY, XYM, XYZ, or XYZM.
/// \n
/// The MgGeometryFactory methods do no spatial analysis. Errors
/// in the construction of the geometry objects are not detected
/// until some operation in the datastore requiring spatial
/// analysis is performed.
/// \n
/// The general rules for constructing geometry objects are as
/// follows:
/// <ul>
///   <li>add coordinates to a coordinate collection which defines
///     an MgLinearRing so that the first and last coordinates are
///     identical,</li>
///   <li>add coordinates to a coordinate collection which defines
///     an MgLinearRing to be used as an <b>exterior</b> ring in an
///     MgPolygon so that the direction of traversal is
///     counterclockwise,</li>
///   <li>add coordinates to a coordinate collection which defines
///     an MgLinearRing to be used as an <b>interior</b> ring in an
///     MgPolygon so that the direction of traversal is clockwise,</li>
///   <li>add MgCurveSegment objects (either MgArcSegment objects
///     or MgLinearSegment objects) to an MgCurveSegmentCollection
///     so that they are contiguous in the order added and so that
///     the last coordinate in an MgCurveSegment object is identical
///     to the first coordinate in the succeeding MgCurveSegment
///     object,</li>
///   <li>add MgCurveSegment objects (either MgArcSegment objects
///     or MgLinearSegment objects) to an MgCurveSegmentCollection
///     which defines an MgCurveRing to be used as an <b>exterior</b>
///     ring in an MgCurvePolygon so that the direction of traversal
///     is counterclockwise,</li>
///   <li>add MgCurveSegment objects (either MgArcSegment objects
///     or MgLinearSegment objects) to an MgCurveSegmentCollection
///     which defines an MgCurveRing to be used as an <b>interior</b>
///     ring in an MgCurvePolygon so that the direction of traversal
///     is clockwise.</li>
/// </ul>
/// With respect to the construction of linear and curve
/// polygons, the Autodesk Geometry Format (AGF) Binary
/// specification does not stipulate the direction of traversal
/// in a ring. However, some providers, for example, Oracle,
/// require that the direction of traversal be counterclockwise
/// in an exterior ring and clockwise in an interior ring.
/// \n
/// The following statements are represented in the graphic at
/// the bottom of this pane.
/// \n
/// MgCoordinate objects are used directly to create MgPoint
/// geometries and MgArcSegment geometry components.
/// \n
/// MgCoordinateCollection objects are used to create
/// MgLineString geometries, MgLinearRing geometry components and
/// MgLinearSegment geometry components.
/// \n
/// An MgPolygon geometry is constructed from an MgLinearRing
/// geometry component, which defines the polygon's external
/// boundary, and an optional MgLinearRingCollection of
/// MgLinearRing geometry components, which define "holes' in the
/// containing ring.
/// \n
/// An MgCurveSegmentCollection of MgLinearSegment objects and
/// MgArcSegment objects are used to create MgCurveString
/// geometries and MgCurveRing geometry components.
/// \n
/// An MgCurvePolygon geometry is constructed from an MgCurveRing
/// geometry component, which defines the curve polygon's
/// external boundary, and an optional MgCurveRingCollection of
/// MgCurveRing geometries, which define "holes" in the
/// containing ring.
/// \n
/// Each single geometry (point, line string, polygon, curve
/// string, and curve polygon) has a homogeneous multi-version.
/// For example, an MgMultiPoint geometry is composed of MgPoint
/// geometries. For each geometry type there is a helper
/// collection class used to construct the multi-version object.
/// \n
/// Finally an MgMultiGeometry object, which consists of a
/// heterogeneous collection of geometries, is constructed by
/// adding geometries to a helper collection class object and
/// passing that collection object to a constructor.
/// \n
/// \image html GeometryFactory.png
///
class MG_GEOMETRY_API MgGeometryFactory : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgGeometryFactory)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgGeometryFactory object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeometryFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeometryFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeometryFactory();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgGeometryFactory();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinate object with Dimension = XY, X = x, Y
    /// = y.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXY(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXY(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXY(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// X value for the coordinate.
    /// \param y (double)
    /// Y value for the coordinate.
    ///
    /// \return
    /// An initialized MgCoordinate instance.
    ///
    virtual MgCoordinate* CreateCoordinateXY(double x, double y);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinate object with Dimension = XYM, X = x, Y
    /// = y, M = m.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// X value for the coordinate.
    /// \param y (double)
    /// Y value for the coordinate.
    /// \param m (double)
    /// M value for the coordinate.
    ///
    /// \return
    /// An initialized MgCoordinate instance.
    ///
    virtual MgCoordinate* CreateCoordinateXYM(double x, double y, double m);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinate object with Dimension = XYZ, X = x, Y
    /// = y, Z = z.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZ(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZ(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZ(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// X value for the coordinate.
    /// \param y (double)
    /// Y value for the coordinate.
    /// \param z (double)
    /// Z value for the coordinate.
    ///
    /// \return
    /// An initialized MgCoordinate instance.
    ///
    virtual MgCoordinate* CreateCoordinateXYZ(double x, double y, double z);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinate object with Dimension = XYZM, X = x,
    /// Y = y, Z = z, M = m.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate CreateCoordinateXYZM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// X value for the coordinate.
    /// \param y (double)
    /// Y value for the coordinate.
    /// \param z (double)
    /// Z value for the coordinate.
    /// \param m (double)
    /// M value for the coordinate.
    ///
    /// \return
    /// An initialized MgCoordinate instance.
    ///
    virtual MgCoordinate* CreateCoordinateXYZM(double x, double y, double z, double m);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an arc segment from start, end, and control
    /// coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgArcSegment CreateArcSegment(MgCoordinate start, MgCoordinate end, MgCoordinate control);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgArcSegment CreateArcSegment(MgCoordinate start, MgCoordinate end, MgCoordinate control);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgArcSegment CreateArcSegment(MgCoordinate start, MgCoordinate end, MgCoordinate control);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param start (MgCoordinate)
    /// The MgCoordinate that specifies the start point
    /// of the arc.
    /// \param end (MgCoordinate)
    /// The MgCoordinate that specifies the end point
    /// of the arc.
    /// \param control (MgCoordinate)
    /// The MgCoordinate that specifies the control
    /// point of the arc.
    ///
    /// \return
    /// An instance of MgArcSegment.
    ///
    virtual MgArcSegment* CreateArcSegment(MgCoordinate* start, MgCoordinate* end, MgCoordinate* control);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a linear segment from a collection of coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLinearSegment CreateLinearSegment(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLinearSegment CreateLinearSegment(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLinearSegment CreateLinearSegment(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinates (MgCoordinateCollection)
    /// An MgCoordinateCollection that specifies the coordinates of the linear
    /// segment. The collection must contain at least two coordinates.
    ///
    /// \return
    /// An instance of MgLinearSegment.
    ///
    virtual MgLinearSegment* CreateLinearSegment(MgCoordinateCollection* coordinates);

    /////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a linear ring from a collection of coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLinearRing CreateLinearRing(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLinearRing CreateLinearRing(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLinearRing CreateLinearRing(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinates (MgCoordinateCollection)
    /// An MgCoordinateCollection that specifies the coordinates of the linear
    /// ring. The collection must contain at least three coordinates.
    ///
    /// \return
    /// An instance of MgLinearRing.
    ///
    virtual MgLinearRing* CreateLinearRing(MgCoordinateCollection* coordinates);

    ///////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a curve ring from a collection of curve segments.
    /// \remarks
    /// The use of this method is illustrated in the MgCurvePolygon example code.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveRing CreateCurveRing(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveRing CreateCurveRing(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveRing CreateCurveRing(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param curveSegments (MgCurveSegmentCollection)
    /// An MgCurveSegmentCollection that specifies the segments
    /// of the curve ring. The segments in the collection must
    /// form a closed shape. See the example php code for
    /// details about the construction of the segments so that
    /// the resulting shape is closed.
    ///
    /// \return
    /// An instance of MgCurveRing.
    ///
    virtual MgCurveRing* CreateCurveRing(MgCurveSegmentCollection* curveSegments);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a point from a coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPoint CreatePoint(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPoint CreatePoint(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPoint CreatePoint(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// An MgCoordinate that specifies the location of this point.
    ///
    /// \return
    /// An instance of MgPoint.
    ///
    virtual MgPoint* CreatePoint(MgCoordinate* coordinate);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a line string from a collection of coordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLineString CreateLineString(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLineString CreateLineString(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLineString CreateLineString(MgCoordinateCollection coordinates);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinates (MgCoordinateCollection)
    /// An MgCoordinateCollection that specifies the coordinates of the line
    /// string. The collection must contain at least two coordinates.
    ///
    /// \return
    /// An instance of MgLineString.
    ///
    virtual MgLineString* CreateLineString(MgCoordinateCollection* coordinates);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a curve string from a collection of curve segments.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveString CreateCurveString(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveString CreateCurveString(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveString CreateCurveString(MgCurveSegmentCollection curveSegments);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param curveSegments (MgCurveSegmentCollection)
    /// An MgCurveSegmentCollection that specifies the segments of the
    /// curve string. The collection must contain at least one segment.
    ///
    /// \return
    /// An instance of MgCurveString.
    ///
    virtual MgCurveString* CreateCurveString(MgCurveSegmentCollection* curveSegments);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a polygon from an outer ring and a collection of inner rings.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPolygon CreatePolygon(MgLinearRing outerRing, MgLinearRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPolygon CreatePolygon(MgLinearRing outerRing, MgLinearRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPolygon CreatePolygon(MgLinearRing outerRing, MgLinearRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param outerRing (MgLinearRing)
    /// An MgLinearRing that specifies the polygons outer boundary.
    /// \param innerRings (MgLinearRingCollection)
    /// An MgLinearRingCollection that specifies the polygons holes.
    /// This parameter may be null.
    ///
    /// \return
    /// An instance of MgPolygon.
    ///
    virtual MgPolygon* CreatePolygon(MgLinearRing* outerRing, MgLinearRingCollection* innerRings);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a curve polygon from an outer ring and a collection of inner
    /// rings.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurvePolygon CreateCurvePolygon(MgCurveRing outerRing, MgCurveRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurvePolygon CreateCurvePolygon(MgCurveRing outerRing, MgCurveRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurvePolygon CreateCurvePolygon(MgCurveRing outerRing, MgCurveRingCollection innerRings);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param outerRing (MgCurveRing)
    /// An MgCurveRing that specifies the polygons outer boundary.
    /// \param innerRings (MgCurveRingCollection)
    /// An MgCurveRingCollection that specifies the polygons holes.
    /// This parameter may be null.
    ///
    /// \return
    /// An instance of MgCurvePolygon.
    ///
    virtual MgCurvePolygon* CreateCurvePolygon(MgCurveRing* outerRing, MgCurveRingCollection* innerRings);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a multi point aggregate geometry from a collection of points.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiPoint CreateMultiPoint(MgPointCollection points);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiPoint CreateMultiPoint(MgPointCollection points);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiPoint CreateMultiPoint(MgPointCollection points);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param points (MgPointCollection)
    /// An MgGeometryCollection that specifies the points. All geometry
    /// objects in this collection must be of type MgPoint.
    ///
    /// \return
    /// An instance of MgMultiPoint.
    ///
    virtual MgMultiPoint* CreateMultiPoint(MgPointCollection* points);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a multi line string aggregate geometry from a collection of
    /// line strings.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiLineString CreateMultiLineString(MgLineStringCollection lineStrings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiLineString CreateMultiLineString(MgLineStringCollection lineStrings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiLineString CreateMultiLineString(MgLineStringCollection lineStrings);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param lineStrings (MgLineStringCollection)
    /// An MgGeometryCollection that specifies the line strings. All geometry
    /// objects in this collection must be of type MgLineString.
    ///
    /// \return
    /// An instance of MgMultiLineString.
    ///
    virtual MgMultiLineString* CreateMultiLineString(MgLineStringCollection* lineStrings);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a multi curve string aggregate geometry from a collection of
    /// curve strings.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiCurveString CreateMultiCurveString(MgCurveStringCollection curveStrings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiCurveString CreateMultiCurveString(MgCurveStringCollection curveStrings);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiCurveString CreateMultiCurveString(MgCurveStringCollection curveStrings);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param curveStrings (MgCurveStringCollection)
    /// An MgGeometryCollection that specifies the curve strings. All geometry
    /// objects in this collection must be of type MgCurveString.
    ///
    /// \return
    /// An instance of MgMultiCurveString.
    ///
    virtual MgMultiCurveString* CreateMultiCurveString(MgCurveStringCollection* curveStrings);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a multi polygon aggregate geometry from a collection of
    /// polygons.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiPolygon CreateMultiPolygon(MgPolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiPolygon CreateMultiPolygon(MgPolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiPolygon CreateMultiPolygon(MgPolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param polygons (MgPolygonCollection)
    /// An MgGeometryCollection that specifies the polygons. All geometry
    /// objects in this collection must be of type MgPolygon.
    ///
    /// \return
    /// An instance of MgMultiPolygon.
    ///
    virtual MgMultiPolygon* CreateMultiPolygon(MgPolygonCollection* polygons);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a multi curve polygon aggregate geometry from a collection of
    /// curve polygons.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiCurvePolygon CreateMultiCurvePolygon(MgCurvePolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiCurvePolygon CreateMultiCurvePolygon(MgCurvePolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiCurvePolygon CreateMultiCurvePolygon(MgCurvePolygonCollection polygons);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param polygons (MgCurvePolygonCollection)
    /// An MgGeometryCollection that specifies the curve polygons. All geometry
    /// objects in this collection must be of type MgCurvePolygon.
    ///
    /// \return
    /// An instance of MgMultiCurvePolygon.
    ///
    virtual MgMultiCurvePolygon* CreateMultiCurvePolygon(MgCurvePolygonCollection* polygons);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an aggregate geometry from a collection of geometry objects.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgMultiGeometry CreateMultiGeometry(MgGeometryCollection geometries);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgMultiGeometry CreateMultiGeometry(MgGeometryCollection geometries);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgMultiGeometry CreateMultiGeometry(MgGeometryCollection geometries);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometries (MgGeometryCollection)
    /// An MgGeometryCollection that specifies the geometries.
    ///
    /// \return
    /// An instance of MgMultiGeometry.
    ///
    virtual MgMultiGeometry* CreateMultiGeometry(MgGeometryCollection* geometries);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty geometry from its type identifier
    ///
    /// \return
    /// Geometry.
    ///
    static MgGeometry* CreateGeometry(INT32 geomId);

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes the factory
    ///
    static bool InitializeGeometryFactory();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Empty geometry creation functions
    ///
    static MgPoint* CreateEmptyPoint();
    static MgLineString* CreateEmptyLineString();
    static MgCurveString* CreateEmptyCurveString();
    static MgPolygon* CreateEmptyPolygon();
    static MgCurvePolygon* CreateEmptyCurvePolygon();
    static MgMultiPoint* CreateEmptyMultiPoint();
    static MgMultiLineString* CreateEmptyMultiLineString();
    static MgMultiCurveString* CreateEmptyMultiCurveString();
    static MgMultiPolygon* CreateEmptyMultiPolygon();
    static MgMultiCurvePolygon* CreateEmptyMultiCurvePolygon();
    static MgMultiGeometry* CreateEmptyMultiGeometry();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_GeometryFactory;

private:
    static bool m_factoryInitialized;
};
/// \}

#endif //_MGGEOMETRYFACTORY_H_
