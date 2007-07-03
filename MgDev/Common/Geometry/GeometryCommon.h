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

#ifndef _MG_GEOMETRY_COMMON_H_
#define _MG_GEOMETRY_COMMON_H_

#include <limits>
#include <float.h>
#include <math.h>

#include "Foundation.h"

// Geometry exports
#ifdef _WIN32
#ifdef GEOMETRY_EXPORTS
#    define MG_GEOMETRY_API __declspec(dllexport)
#else
#    define MG_GEOMETRY_API __declspec(dllimport)
#endif
#else
#define MG_GEOMETRY_API
#endif

#include "GeometryClassId.h"

#include "CoordinateDimension.h"
#include "GeometricDimension.h"
#include "Coordinate.h"
#include "CoordinateXY.h"
#include "CoordinateXYZ.h"
#include "CoordinateXYZM.h"
#include "CoordinateXYM.h"
#include "CoordinateCollection.h"
#include "CoordinateIterator.h"
#include "Transform.h"
#include "Envelope.h"
#include "Measure.h"

#include "GeometricPathInstructionType.h"
#include "GeometricPathInstruction.h"
#include "CloseInstruction.h"
#include "MoveToInstruction.h"
#include "LineToInstruction.h"
#include "QuadToInstruction.h"
#include "GeometricPathIterator.h"

#include "GeometricEntityType.h"
#include "GeometricEntity.h"
#include "GeometryType.h"
#include "Geometry.h"
#include "GeometryComponentType.h"
#include "GeometryComponent.h"

#include "GeometryCollection.h"

#include "Point.h"
#include "Curve.h"
#include "Region.h"

#include "CurveSegment.h"
#include "CurveSegmentCollection.h"
#include "Ring.h"
#include "ArcSegment.h"
#include "LinearSegment.h"
#include "CurveRing.h"
#include "CurveRingCollection.h"
#include "LinearRing.h"
#include "LinearRingCollection.h"

#include "CurveString.h"
#include "LineString.h"
#include "Polygon.h"
#include "CurvePolygon.h"

#include "CurveStringCollection.h"
#include "LineStringCollection.h"
#include "PolygonCollection.h"
#include "CurvePolygonCollection.h"
#include "PointCollection.h"

#include "AggregateGeometry.h"
#include "MultiCurvePolygon.h"
#include "MultiCurveString.h"
#include "MultiGeometry.h"
#include "MultiLineString.h"
#include "MultiPoint.h"
#include "MultiPolygon.h"

#include "AgfReaderWriter.h"
#include "WktReaderWriter.h"

#include "GeometryFactory.h"
#include "GeometryUtil.h"

#include "CoordinateSystem/CoordinateSystem.h"
#include "CoordinateSystem/CoordinateSystemFactory.h"
#include "CoordinateSystem/CoordinateSystemMeasure.h"
#include "CoordinateSystem/CoordinateSystemTransform.h"
#include "CoordinateSystem/CoordinateSystemType.h"

#include "Exception/CoordinateSystemComputationFailedException.h"
#include "Exception/CoordinateSystemConversionFailedException.h"
#include "Exception/CoordinateSystemInitializationFailedException.h"
#include "Exception/CoordinateSystemLoadFailedException.h"
#include "Exception/CoordinateSystemMeasureFailedException.h"
#include "Exception/CoordinateSystemTransformFailedException.h"
#include "Exception/GeometryException.h"
#include "Exception/InvalidCoordinateSystemException.h"
#include "Exception/InvalidCoordinateSystemTypeException.h"
#include "Exception/InvalidCoordinateSystemUnitsException.h"

// Represent NAN for double
#define DoubleNan std::numeric_limits<double>::quiet_NaN()

// Checks whether value is NAN or not
#ifdef _WIN32
#define IsDoubleNan(x)  _isnan(x)
#else
#define IsDoubleNan(x)  isnan(x)
#endif

#define GEOMETRY_SEP    " "
#define POINT_SEPARATOR L", "

#endif
