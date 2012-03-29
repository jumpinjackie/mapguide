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
    /// Extracts the greatest magnitude of the all the coordinates in a
    /// coordinate collection.  Return value is always positive.
    /// </summary>
    /// <param name="coordinteIterator">
    /// An iterator dervied from the target coordinate collection.
    /// </param>
    /// <returns>
    /// Returns a positive value which is the largest magnitude of all
    /// ordinates in the collection.
    /// </returns>
    /// <remarks>
    /// Intended to be used to determine a suitable tolerance value for such
    /// things as coordinate comparisons.
    /// </remarks>
    static double GreatestMagnitude (MgCoordinateIterator* coordinateIterator);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Computes the intersection of two 2D line segments.  Intersection point,
    /// if any, is returned in the provided result coordinate which (of course)
    /// must already exist.  All point arguments are expected (not required)
    /// to be MgCoordinateXY objects.
    /// </summary>
    /// <param name="result">
    /// The calculated intersection point is returned in this variable which,
    /// of course, must exist.  Coordinates of <c>result</c> remain unaltered
    /// if an intersection does not exist.  Only X and Y are returned.
    /// </param>
    /// <param name="seg1From">
    /// The initial point of the first line segment.
    /// </param>
    /// <param name="seg1To">
    /// The end point of the first line segment.
    /// </param>
    /// <param name="seg2From">
    /// The initial point of the second line segment.
    /// </param>
    /// <param name="seg2To">
    /// The end point of the second line segment.
    /// </param>
    /// <param name="magnitude">
    /// A value which represents the greatest coordinate value in the dataset
    /// being processed.  Used to calculate an appropriate "fuzzy" value for
    /// coordinate comparisions, etc.
    /// </param>
    /// <returns>
    /// Return status of the intersection of the two segments as a bit map of
    /// values as defined in <c>SpatialUtilityStatus.h</c>
    /// </returns>
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
                                                           MgCoordinate* seg2To,
                                                           double magnitude = 1.0E+10);

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
    /// A new collection of <c>MgCoordinateXYM</c> points, one such point for each
    /// intersection is returned.  This collection can be, and often is, an empty
    /// collection.  A null pointer is never returned.<para>
    /// The X and Y ordinates of each point in the returned collection represent
    /// the actual location of the intersection.  The M ordinate will carry, as
    /// a double, the status of the intersection as returned by the
    /// <c>SegmentIntersection</c> function.
    /// </returns>
    static MgCoordinateCollection* PolySegIntersection (MgCoordinateIterator* polyItr,
                                                        MgCoordinate* segFrom,
                                                        MgCoordinate* segTo);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Adds a coordinate to a coordinate collection such that the resulting
    /// collection represents a collection of points in sequence from the
    /// provided base point.
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
    /// The purpose of this function is serve as a helper for the PolySegIntersection
    /// function.  It enables PolySegIntersection to return a point collection such
    /// that the points in the collection present an orderly sequence of points from
    /// the provided base point.  Thus, if the original line segment provided to
    /// PolySegIntersection proceeded in the south to north direction, the point
    /// collection returned would also be returned in that order; regardless of the
    /// shape of the polygon or the direction in which it proceeds.<para>
    /// The determination as to the insertion point of <c>newPoint</c> is based solely
    /// on the X and Y ordinates of <c>newPoint</c> and the points which already
    /// exist in the provided collection.  The type of <c>newPoint</c> and the points
    /// in the collection is immaterial.
    /// </remarks>
    static INT32 AddToCoordinateCollection (MgCoordinateCollection* collection,MgCoordinate* newPoint,
                                                                               MgCoordinate* basePoint);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Determines if the provided point is inside (or actually on) the closed
    /// ring provided by the <c>polyItr</c> argument.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator of the closed ring which is the subject polygon.
    /// </param>
    /// <param name="queryPoint">
    /// The 2D point whose status is to be determined.
    /// </param>
    /// <returns>
    /// Returns zero if the query point is outside the closed ring, +1 if the
    /// query point is precisely on the closed ring (per MgMathUtility::DblCmp),
    /// or +2 if the query point is inside the closed ring.
    /// </returns>
    /// <remarks>
    /// Currently, this function calculates the envelope of the provided closed
    /// ring in order to determine a point which is known to be outside of the
    /// closed ring.  An overloaded function which accepts a point known to be
    /// outside the closed ring which is a lot faster is also available.
    /// </remarks>
    static INT32 PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* queryPoint);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Determines a point guaranteed to be ouside the provided polygon.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator of the closed ring which is the subject polygon.
    /// </param>
    /// <returns>
    /// Returns an <c>MgCoordinateXY</c> point which is guaranteed to be ooutside
    /// the provided polygon.
    /// </returns>
    static MgCoordinate* PointOutsidePolygon (MgCoordinateIterator* polyItr);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Determines if the provided point is inside (or actually on) the closed
    /// ring provided by the <c>polyItr</c> argument.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator of the closed ring which is the subject polygon.
    /// </param>
    /// <param name="outsidePoint">
    /// A 2D point which is known to be outside of the provided polygon.  Can
    /// be obtained from <c>PointOutsidePolygon</c>.
    /// <param name="queryPoint">
    /// The 2D point whose status is to be determined.
    /// </param>
    /// <returns>
    /// Returns zero if the query point is outside the closed ring, +1 if the
    /// query point is precisely on the closed ring (per MgMathUtility::DblCmp),
    /// or +2 if the query point is inside the closed ring.
    /// </returns>
    /// <remarks>
    /// This function requires that the calling module supply a point known to
    /// be outside the polygon.  Thus, higher performance can be obtained when
    /// multiple queries against the same polygon are required.
    /// </remarks>
    static INT32 PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* outsidePoint,
                                                                 MgCoordinate* queryPoint);

    ///////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Clips a line string to an arbitrary polygon returning a collection of line
    /// strings which represent the portions of the provided line string which are
    /// inside the provided closed ring.
    /// </summary>
    /// <param name="polyItr">
    /// An iterator to the closed ring to which the provided line string is to be
    /// clipped.
    /// </param>
    /// <param name="lineItr">
    /// An iterator for the line string which is to be clipped.
    /// </param>
    /// <param name="includeCollinear">
    /// A true value indicates that segments, and/or portions of segments,
    /// of the line string to be clipped which are collinear with the clipping
    /// boundary are to be considered to be inside the clipping boundary.  A
    /// false value causes collinear segments (or portions thereof) to be
    /// outside the clipping boundary and thus excluded from the returned
    /// line string collection.
    /// </param>
    /// <returns>
    /// A collection of line string objects which represents the portions of the
    /// provided line string which are inside of the provided closed ring.  This
    /// collection may be empty; a null pointer is never returned.
    /// </returns>
    static MgLineStringCollection* ClipStringToPolygon (MgCoordinateIterator* polyItr,
                                                        MgCoordinateIterator* lineItr,
                                                        bool includeCollinear = false);
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

    // Values which are stored in the M member of the internally used point
    // collection; i.e. the status of each point relative to the clip ploygon.
    static const INT32 StringPointNotDetermined = 0;
    static const INT32 StringPointIsOutside     = 1;
    static const INT32 StringPointIsOnBoundary  = 2;
    static const INT32 StringPointIsInside      = 3;

    // Values which are stored in the Z member of the internally used point
    // collection; i.e. the status of each segment relative to the clip
    // ploygon.  The value stored refers to the segment which starts at
    // the point in which the segment status is stored.
    static const INT32 StringSegNotDetermined = 0;
    static const INT32 StringSegIsOutside     = 1;
    static const INT32 StringSegIsCollinear   = 2;
    static const INT32 StringSegIsInside      = 3;

    static MgCoordinateCollection* StringClipPhaseOne (MgCoordinateIterator* lineString,
                                                       MgCoordinateIterator* itrPolygon);
    static void StringClipPhaseTwo (MgCoordinateIterator* lineString,MgCoordinateIterator* itrPolygon);
    static void StringClipPhaseThree (MgCoordinateIterator* lineString,MgCoordinateIterator* itrPolygon);
    static MgLineStringCollection* StringClipPhaseFour (MgCoordinateIterator* lineString,bool collinear = false,
                                                                                         bool outside = false);
};
/// \endcond

#endif
