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

#ifndef MgSpatialUtilityStatusValues_H
#define MgSpatialUtilityStatusValues_H

/// \cond INTERNAL
/// \brief
/// Spatial Utility Status Values.

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines the status values returned by various methods belonging to the
/// MgSpatialUtility class.
class MG_GEOMETRY_API MgSpatialUtilityStatus
{
public:
    //=========================================================================
    // Intersection status is returned as a bit map.  The bit map
    // consists of four sub bit maps, we'll call them groups.
    // Group 1 -> Segment Status
    //     If any bits in this group are set, an intersection point
    //     does not exist.  If all bits in this group are zero, an
    //     intersection does exists and the point is returned.
    // Group 2 -> Intersection Status
    //     The bits in this group indicate if the intersection point
    //     resides on weither (or both) segments.
    // Group 3 -> Intersection Point Status
    //     The bits in this group indicate the intersection points
    //     relationship to the end points of the two segments.
    // Group 4 -> Collinear Segment Status
    //     In the case of collinear segments, the bits in this
    //     section indicate to what extent the collinear lines
    //     actually overlap.

    //=========================================================================
    // Group 1 -> Segment Status
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Set if the segments are parallel but _NOT_ collinear.
    static const INT32 SegmentsParallel   = 0x0010;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Set only if the segments are collinear.
    static const INT32 SegmentsCollinear   = 0x0020;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Set if segment one is a null segement.
    static const INT32 DegenerateSeg1       = 0x0040;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Set if segment two is a null segement.
    static const INT32 DegenerateSeg2       = 0x0080;

    //=========================================================================
    // Group 2 -> Intersection Status
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Set if the intersection point resides on segment one, end
    /// points included.
    static const INT32 IntersectOnSeg1      = 0x0001;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// The intersection point resides on segment two, end
    /// points included.
    static const INT32 IntersectOnSeg2      = 0x0002;

    //=========================================================================
    // Group Three -> Intersection Point Status
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Intersection at segment 1 start point
    static const INT32 IntersectIsSeg1Start = 0x0100;
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Intersection at segment 1 end point
    static const INT32 IntersectIsSeg1End   = 0x0200;
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Intersection at segment 2 start point
    static const INT32 IntersectIsSeg2Start = 0x0400;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// Intersection at segment 2 end point
    static const INT32 IntersectIsSeg2End   = 0x0800;

    //=========================================================================
    // Group 4 -> Collinear Status
    // Note that in the case where there is no overlap of the
    // collinear lines, all four of these bits will be zero.
    // In the case of overlap, at least two of these bits must be
    // set; one indicating the start position of the overlap and
    // one indicating the end position of the overlap.  It is
    // (actually) possible for just one bit to be set, indicating that
    // the end of one segment is the beginning of the other.

    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// This bit is set if the segments are collinear, there is
    /// segment overlap, and the overlapping portion starts at the
    /// initial point of segment 1.
    static const INT32 CollinearSeg1Start   = 0x1000;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// This bit is set if the segments are collinear, there is
    /// segment overlap, and the overlapping portion starts at the
    /// initial point of segment 2.
    static const INT32 CollinearSeg2Start   = 0x2000;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// This bit is set if the segments are collinear, there is
    /// segment overlap, and the overlapping portion ends at the
    /// end point of segment 1.
    static const INT32 CollinearSeg1End     = 0x4000;
    /// \brief
    /// This constant defines the significance of a specific a
    /// bit in the bit map returned by the SegmentIntersection.
    /// This bit is set if the segments are collinear, there is
    /// segment overlap, and the overlapping portion ends at the
    /// end point of segment 2.
    static const INT32 CollinearSeg2End     = 0x8000;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// A mask for testing for extracting Segment Status from the
    /// bit map returned by the SegmentIntersection method.
    static const INT32 SegmentStatusMask    = SegmentsParallel  |
                                              SegmentsCollinear |
                                              DegenerateSeg1    |
                                              DegenerateSeg2;
    /// \brief
    /// A mask for testing for extracting Collinear Status from
    /// the bit map returned by the SegmentIntersection method.
    static const INT32 CollinearStatusMask  = CollinearSeg1Start |
                                              CollinearSeg1End   |
                                              CollinearSeg2Start |
                                              CollinearSeg2End;

    //=========================================================================
    /// \brief
    /// Value returned by the PointIsInPolygon method when the quesry point is
    /// outside of the provided polygon (outer ring only).
    static const INT32 PointIsOutside = 0;
    /// \brief
    /// Value returned by the PointIsInPolygon method when the quesry point is
    /// outside of the provided polygon (outer ring only).
    static const INT32 PointIsOnBoundary = 1;
    /// \brief
    /// Value returned by the PointIsInPolygon method when the quesry point is
    /// outside of the provided polygon (outer ring only).
    static const INT32 PointIsInside = 2;
};

#endif //_MGCOORDINATESYSTEMTYPE_H_
