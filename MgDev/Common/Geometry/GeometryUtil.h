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

#ifndef _MGGEOMUTIL_H_
#define _MGGEOMUTIL_H_

/// \cond INTERNAL
class MgGeometryUtil
{
INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize a coordinate object from a stream
    ///
    static MgCoordinate* ReadCoordinate(MgStream* stream, INT32 dimension);

    //////////////////////////////////////////////////////////////////
    /// Read an MgCurveSegment object from a stream
    ///
    static MgCurveSegment* ReadCurveSegment(MgStream* stream, MgCoordinate* startCoord, INT32 dimension);

    //////////////////////////////////////////////////////////////////
    /// Read an ReadCurveRing object from a stream
    ///
    static MgCurveRing* ReadCurveRing(MgStream* stream, INT32 dimension);

    //////////////////////////////////////////////
    /// Read an MgCurveSegment object to a stream
    ///
    static MgLinearRing* ReadLinearRing(MgStream* stream, INT32 dimension);

    //////////////////////////////////////////////////////////////////
    /// Write an MgCurveSegment object to a stream
    ///
    static void WriteCurveSegment(MgStream* stream, MgCurveSegment* segment);

    //////////////////////////////////////////////////////////////////
    /// write an MgCurveRing object to a stream
    ///
    static void WriteCurveRing(MgStream* stream, MgCurveRing* ring);

    //////////////////////////////////////////////
    /// Write an MgLinearRing object to a stream
    ///
    static void WriteLinearRing(MgStream* stream, MgLinearRing* ring);

    //////////////////////////////////////////////
    /// Compute the envelope enclosing a line string
    ///
    static MgEnvelope* ComputeLineStringEnvelope(MgCoordinateCollection* coordinates);

    //////////////////////////////////////////////
    /// Compute the envelope enclosing a curve segment set
    ///
    static MgEnvelope* ComputeCurveSegmentsEnvelope(MgCurveSegmentCollection* segments);

    //////////////////////////////////////////////
    /// Compute the envelope enclosing a geometry aggregate
    ///
    static MgEnvelope* ComputeAggregateEnvelope(MgAggregateGeometry* aggrGeometry);

    //////////////////////////////////////////////
    /// Compare a set of coordinates
    ///
    static bool CoordinatesEqual(MgCoordinateCollection* coords1, MgCoordinateCollection* coords2);

    //////////////////////////////////////////////
    /// Compare a set of curve segments
    ///
    static bool CurveSegmentsEqual(MgCurveSegmentCollection* segs1, MgCurveSegmentCollection* segs2);

    //////////////////////////////////////////////
    /// Compare two aggregate geometries
    ///
    static bool AggregatesEqual(MgAggregateGeometry* geom1, MgAggregateGeometry* geom2);
};
/// \endcond

#endif // MGGEOMUTIL
