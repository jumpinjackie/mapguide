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

#ifndef MgSpatialUtility_H
#define MgSpatialUtility_H

/// \cond INTERNAL

/// \brief
/// Spatial utility class.
class MgSpatialUtility
{
public:

    /// Converts arcs to linestrings
    static MgGeometry* TesselateCurve(MgGeometry* curve);
    /// Converts arcs to linestrings
    static MgGeometryComponent* TesselateGeometryComponent(MgGeometryComponent* curve);

    /// \brief
    /// Approximates a geometry, which may contain parametric curve components,
    /// with a geometry that only contains straight, linear components.
    ///
    /// \remarks
    /// If the given geometry does not require approximation, this method returns
    /// the input geometry, and thus avoids unnecessary object creation.
    /// Tolerances are in same units as the geometry's ordinates.
    /// If the given tolerances are both zero, a tesselated geometry is produced,
    /// wherein arcs are represented using only their three defining positions.
    /// Math performed is all in implicit Cartesian (rectilinear) coordinate system.
    /// Measurement ("M") ordinates of interpolated positions undergo linear
    /// interpolation from an arc's start position to end position (possibly disregarding
    /// an "M" ordinate in the arc's mid position).
    /// Unless the arc is too short for any interpolation, the "mid" point parameter
    /// used to define arcs is not guaranteed to appear among interpolated points.
    ///
    /// \param geometry
    /// Input Geometry to approximate
    /// \param maxSpacing
    /// Input Maximum length of each line segment used to approximate parametric curves. Ignored if zero.
    /// \param maxOffset
    /// Input Maximum distance that a line segment may be from the curve that it approximates (error tolerance). Ignored if zero.
    /// \param geometryFactory
    /// Input Geometry factory for the creation of the approximated geometry.
    ///
    /// \return
    /// Returns An approximation of the given geometry.
    ///
    static MgGeometry * ApproximateGeometryWithLineStrings(
        MgGeometry * geometry,
        double maxSpacing,
        double maxOffset,
        MgGeometryFactory* geometryFactory);

    static bool ArePositionsEqualXYZ(
        MgCoordinate * p1,
        MgCoordinate * p2);

    static bool ArePositionsEqualXY(
        MgCoordinate * p1,
        MgCoordinate * p2);

    static double DistanceBetweenPositionsXYZ(
        MgCoordinate * p1,
        MgCoordinate * p2);

    static double DistanceBetweenPositionsXY(
        MgCoordinate * p1,
        MgCoordinate * p2);

    /// Unit normal from plane defined by vectors p1p2 to p2p3 using right-hand rule.
    /// This will return (0,0,0) for parallel vectors (collinear points) or zero-length
    /// vectors.
    static MgCoordinate * UnitNormalVector(
        MgCoordinate * p1,
        MgCoordinate * p2,
        MgCoordinate * p3);

    static void AppendPositionToDistinctCollection(
        MgCoordinateCollection * distinctPositions,
        MgCoordinate * positionToAppend);

    /// Return a new point with the location of the given point after rotation
    /// about the given axis and center.
    /// This method assumes that the involved data has XYZ dimensionality.
    /// <p><b>Note:</b> "axisVector" must be a unit normal to the plane of rotation.
    static MgCoordinate * RotatePosition(
        MgCoordinate * position,
        double angle,
        MgCoordinate * axisVector,
        MgCoordinate * center,
        bool includeM,
        double m);

    static MgLinearRing* CurveRingToLinearRing(MgCurveRing* curveRing, double maxSpacing, double maxOffset);
    static MgLinearSegment* ArcSegmentToLinearSegment(MgArcSegment* segment, double maxSpacing, double maxOffset);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Computes the intersection of two 2D line segments.  Intersection point,
    /// if any, is returned in the provided result coordinate which (of course)
    /// must already exist.
    /// </summary>
    /// <param name="result">
    /// The calculated intersection point is returned in this variable which,
    /// of course, must exist.
    /// <param name="seg1From">
    /// The initial point of the first line segment.
    /// <param name="seg1To">
    /// The end point of the first line segment.
    /// <param name="seg2From">
    /// The initial point of the second line segment.
    /// <param name="seg2To">
    /// The end point of the second line segment.
    /// <returns>
    /// Return status:
    /// <list type="table">
    /// <listheader>
    ///     <term>Status</term>
    ///     <description>Meaning</description>
    /// </listheader>
    /// <item>
    ///     <term>-1</term>
    ///     <description>no intersection, segments are parallel or collinear or
    ///        a segemnt is of zero length; result remains unchanged
    ///     </description>
    /// </item>
    /// <item><term> 0</term><desription>intersection exists, intersection point is not on either line</description></item>
    /// <item><term> 1</term><desription>intersection exists, intersection point is on segment 1 only</description></item>
    /// <item><term> 2</term><desription>intersection exists, intersection point is on segment 2 only</description></item>
    /// <item><term> 3</term><desription>intersection exists, intersection point is on both segments</description></item>
    /// </list>
    /// <remarks>
    /// In determining if the intersection point resides on a line, an intersection
    /// point identical to the 'to' point is considered on the line, but an
    /// intersection point identical to the 'from' point is _NOT_ considered to be
    /// on the line.  Such a convention is necessary to avoid the appearance
    /// of two intersections when indeed there is only one when processing a
    /// a line string, for example.
    /// </remarks>
    /// <exception cref="MgNullReferenceException">
    /// Thrown if any argument is null
    /// </exception>
    static INT32 SegmentIntersection (MgCoordinate* result,MgCoordinate* seg1From,
                                                           MgCoordinate* seg1To,
                                                           MgCoordinate* seg2From,
                                                           MgCoordinate* seg2To);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns a collection of coordinates with all of the intersection points of
    /// the provided segment with the polygon provided by the first argument.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator of the closed ring to which the provided segment is
    /// intersected.
    /// </param>
    /// <param name="segFrom">
    /// The initial point of the line segment to be intersected with the provided
    /// closed ring.
    /// </param>
    /// <param name="segTo">
    /// The ending point of the line segment to be intersected with the provided
    /// closed ring.
    /// </param>
    /// <returns>
    /// A 2D coordinate collection of all intersection points; can and often is
    /// an empty collection.
    /// </returns>
    /// <remarks>
    /// This is a 2D function only, Z and M coordinates are ignored; the returned
    /// point collection is a collection of <c>MgCoordinateXY</c> objects.<para>
    /// Note that the first argument is a CoordinateIterator; so it doesn't have to be
    /// an <c>MgPolygon</c>.  However, this function assumes that the iterator does indeed
    /// point to a closed ring.
    /// </remarks>
    static MgCoordinateCollection* PolySegIntersection (MgCoordinateIterator* polyItr,
                                                        MgCoordinate* segFrom,
                                                        MgCoordinate* segTo);


    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Adds a coordinate to a coordinate collection such that the resulting
    /// collection represents a collection of points in sequence from the
    /// [rpvoded base point.
    /// </summary>
    /// <param name="collection">
    /// The coordinate collection to which the <c>newPoint</c> is to be added.
    /// </param>
    /// <param name="newPoint">
    /// The point to be added to the provided collection.
    /// </param>
    /// <param name="basePoint">
    /// The base point which is used to order the collection.
    /// </param>
    /// <returns>
    /// the index at which the insertion occurred.
    /// </returns>
    /// <remarks>
    /// This is a 2D function only, Z and M coordinates are ignored; the provided
    /// point collection is expected to be a collection of <c>MgCoordinateXY</c> objects.<para>
    /// The purpose of this function is serve as a helper for the PolySegIntersection
    /// function.  It enables PolySegIntersection to return a point collection such
    /// that the points in the collection present an orderly sequence of points from
    /// the provided base point.  Thus, if the original line segment provided to
    /// PolySegIntersection proceeded in the south to north direction, the point
    /// collection returned would also be returned in that order; regardless of the
    /// shape of the polygon or the direction in which it proceeds.
    /// </remarks>
    static INT32 AddToCoordinateCollection (MgCoordinateCollection* collection,MgCoordinate* newPoint,
                                                                        MgCoordinate* basePoint);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Determines if the provided point is inside (or actually on) the closed
    /// ring provided bythe polyItr argument.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator of the closed ring which is the subject polygon.
    /// </param>
    /// <param name="queryPoint">
    /// The 2D point whose status is to be determined.
    /// </param>
    /// <returns>
    /// Returns true if the query point is inside or on the provided closed
    /// ring.
    /// </returns>
    /// <remarks>
    /// Currently, this function calculates the envelope of the provided closed
    /// ring in order to determine a point which is known to be outside of the
    /// closed ring.  An overloaded function which accepts a point known to be
    /// outside the closed ring would be a lot faster.
    /// </remarks>
    static bool PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* queryPoint);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Clips a line string to an arbitrary polygon returning a collection of line
    /// strings which represent the portions of the the provided line string which
    /// are inside the provided closed ring.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator to the closed ring to which the provided line string is to be
    /// clipped.
    /// </param>
    /// <param name="lineItr">
    /// An iterator for the line string which is to be clipped.
    /// </param>
    /// <returns>
    /// A collection of line string objects which represents the portions of the
    /// provided line string which are inside of the provided closed ring.  This
    /// collection may be empty.
    /// </returns>
    /// <remarks>
    /// This is a pure 2D function.  The line strings generated will contain
    /// collections of <c>MgCoordinateXY</c> objects.
    /// </remarks>
    static MgLineStringCollection* ClipStringToPolygon (MgCoordinateIterator* polyItr,
                                                        MgCoordinateIterator* lineItr);
                                                                                            
protected:

    MgSpatialUtility() {};
    ~MgSpatialUtility() {};

    static void AppendLinearizedCurveSegmentsToDistinctCollection(
        MgCoordinateCollection * distinctPositions,
        MgCurveSegmentCollection * curveSegments,
        double maxSpacing,
        double maxOffset);

    static void AppendPositionsToDistinctCollection(
        MgCoordinateCollection * distinctPositions,
        MgCoordinateCollection * positionsToAppend);
};
/// \endcond

#endif
