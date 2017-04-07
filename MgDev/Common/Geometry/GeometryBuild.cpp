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

#include "AgfReaderWriter.cpp"
#include "AgfStream.cpp"
#include "AggregateGeometry.cpp"
#include "ArcSegment.cpp"
#include "Coordinate.cpp"
#include "CoordinateCollection.cpp"
#include "CoordinateIterator.cpp"
#include "CoordinateXY.cpp"
#include "CoordinateXYM.cpp"
#include "CoordinateXYZ.cpp"
#include "CoordinateXYZM.cpp"
#include "Curve.cpp"
#include "CurvePolygon.cpp"
#include "CurvePolygonCollection.cpp"
#include "CurveRing.cpp"
#include "CurveRingCollection.cpp"
#include "CurveSegmentCollection.cpp"
#include "CurveString.cpp"
#include "CurveStringCollection.cpp"
#include "Envelope.cpp"
#include "GeometricEntity.cpp"
#include "Geometry.cpp"
#include "GeometryCollection.cpp"
#include "GeometryFactory.cpp"
#include "GeometrySimplifier.cpp"
#include "GeometryUtil.cpp"
#include "LinearRing.cpp"
#include "LinearRingCollection.cpp"
#include "LinearSegment.cpp"
#include "LineString.cpp"
#include "LineStringCollection.cpp"
#include "MultiCurvePolygon.cpp"
#include "MultiCurveString.cpp"
#include "MultiGeometry.cpp"
#include "MultiLineString.cpp"
#include "MultiPoint.cpp"
#include "MultiPolygon.cpp"
#include "Point.cpp"
#include "PointCollection.cpp"
#include "Polygon.cpp"
#include "PolygonCollection.cpp"
#include "Region.cpp"
#include "Ring.cpp"
#include "WktReaderWriter.cpp"
#include "GeosUtil.cpp"

#include "Buffer/MgBuffer.cpp"
#include "Buffer/BorderWalker.cpp"
#include "Buffer/bufrutil.cpp"
#include "Buffer/eventarr.cpp"
#include "Buffer/FloatTransform.cpp"
#include "Buffer/GreatCircleBufferUtil.cpp"
#include "Buffer/interlst.cpp"
#include "Buffer/interprc.cpp"
#include "Buffer/LatLonBorderWalker.cpp"
#include "Buffer/ogeomtry.cpp"
#include "Buffer/oobject.cpp"
#include "Buffer/opline.cpp"
#include "Buffer/opolygon.cpp"
#include "Buffer/oppolygn.cpp"
#include "Buffer/oppolyln.cpp"
#include "Buffer/orpplygn.cpp"
#include "Buffer/ortree.cpp"
#include "Buffer/planeswp.cpp"
#include "Buffer/plgnbufr.cpp"
#include "Buffer/plgnsbak.cpp"
#include "Buffer/plinbufr.cpp"
#include "Buffer/pobjbufr.cpp"
#include "Buffer/ppgunion.cpp"
#include "Buffer/progcbck.cpp"
#include "Buffer/sedgaloc.cpp"
#include "Buffer/sweepedg.cpp"
#include "Buffer/tuplearr.cpp"
#include "Buffer/vertaloc.cpp"
#include "Buffer/wedgaloc.cpp"
#include "Buffer/wedgearr.cpp"
#include "Buffer/wingedge.cpp"
#include "Buffer/worklist.cpp"
#include "Buffer/xordtree.cpp"
#ifdef PERF_DUMPFFGF_
#include "Buffer/DumpFFGF.cpp"
#endif

#include "Parse/ArrayHelper.cpp"
#include "Parse/LexAwkt.cpp"
#include "Parse/ParseAwkt.cpp"
#include "Parse/ParseAwktUtil.cpp"
#include "Parse/StringUtility.cpp"

// Platform specific yacc generated parser files.
// Note: The only platform supported in addition to Win32 is Linux
#ifdef _WIN32
    #include "Parse/yyAwkt.cpp"
#else
    #include "Parse/yyAwkt_linux.cpp"
#endif

#include "Spatial/MathUtility.cpp"
#include "Spatial/SpatialUtility.cpp"
#include "Spatial/SpatialUtilityCircularArc.cpp"

#include "CoordinateSystem/CoordinateSystemCache.cpp"
#include "CoordinateSystem/CoordinateSystemFactory.cpp"

#include "Exception/CoordinateSystemComputationFailedException.cpp"
#include "Exception/CoordinateSystemConversionFailedException.cpp"
#include "Exception/CoordinateSystemInitializationFailedException.cpp"
#include "Exception/CoordinateSystemLoadFailedException.cpp"
#include "Exception/CoordinateSystemMeasureFailedException.cpp"
#include "Exception/CoordinateSystemTransformFailedException.cpp"
#include "Exception/GeometryException.cpp"
#include "Exception/InvalidCoordinateSystemException.cpp"
#include "Exception/InvalidCoordinateSystemTypeException.cpp"
#include "Exception/InvalidCoordinateSystemUnitsException.cpp"

#include "GeometryClassFactory.cpp"
