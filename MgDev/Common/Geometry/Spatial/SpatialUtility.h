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
