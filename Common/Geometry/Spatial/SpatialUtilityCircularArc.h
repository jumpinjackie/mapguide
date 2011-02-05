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


#ifndef MgSpatialUtilityCircularArc_H
#define MgSpatialUtilityCircularArc_H

#include "SpatialUtilityVectorAngle.h"

/// \cond INTERNAL
///////////////////////////////////////////////////////////
// Circular arc with all possible parameters computed.
struct MgSpatialUtilityCircularArc
{
    Ptr<MgCoordinate>   m_start;
    Ptr<MgCoordinate>   m_mid;
    Ptr<MgCoordinate>   m_end;
    bool                            m_isValidArc;
    bool                            m_hasZ;
    Ptr<MgCoordinate>   m_unitNormalVector; // Always has Z!
    Ptr<MgCoordinate>   m_center;
    MgSpatialUtilityVectorAngle    m_startAngle;
    MgSpatialUtilityVectorAngle    m_endAngle;
    double                          m_radius;
    double                          m_length;
    bool                            m_isCircle;

    /// Constructor.  If the input parameters are valid points that do not describe
    /// an arc, the constructor leave m_isValidArc==false, rather than throwing
    /// an exception.
    MgSpatialUtilityCircularArc(
        MgCoordinate* start,
        MgCoordinate* mid,
        MgCoordinate* end);

    /// Returns true if arc is in or parallel to the XY plane.
    bool IsFlat();

    /// Returns true if arc is counter-clockwise in XY plane
    /// NOTE: This assumes that the arc is flat, though the answer is
    /// still usable for any arc that is not completely vertical.
    bool IsDirectionCounterClockWise();

    /// Assign interpolated ordinate values, forming a linestring.
    /// This method appends distinct points to the collection, which does not
    /// have to be empty as input.
    /// If the arc is not valid, or otherwise cannot be linearized using
    /// the given parameters, the arc's known points will simply be appended
    /// as if they described a LineString.
    void Linearize(
        MgCoordinateCollection * distinctPositions,
        double maxSpacing,
        double maxOffset);

protected:
    /// Protected default Constructor.
    MgSpatialUtilityCircularArc() {};

    void ComputeCenterRadiusAnglesFromThreePositionsAndNormal();
    void ComputeCenterFromThreePositions();
    void ComputeLengthFromCenterRadiusAnglesAndNormal();

    /// Compute the arc step length that can be used as the distance
    /// between interpolation points along a circular arc.  It is such that the
    /// interpolated string of chords is never in error by more than the
    /// given amount, but has no more points than are needed.
    /// This method imposes a lower bound of 2.0 * maximumError on the
    /// returned step length, as this is taken to be the resolution of
    /// the ordinates.
    /// This method returns 0.0 if the stepping length is too long for
    /// the arc.  In this case, the caller may elect to consider it an error,
    /// but a suitable approach during interpolation would be to simply use
    /// 2 or 3 known positions on the arc, including the start and end positions.
    double BestArcStep(double radius, double arcLength, double maximumError);
};
/// \endcond

#endif
