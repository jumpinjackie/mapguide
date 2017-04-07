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
#include "GeometrySimplifier.h"
#include "GeometrySimplificationAlgorithmType.h"

#include "CoordinateSystem/CoordinateSystem.h"
#include "CoordinateSystem/CoordinateSystemDatum.h"
#include "CoordinateSystem/CoordinateSystemGeodeticTransformGridFile.h"
#include "CoordinateSystem/CoordinateSystemGeodeticTransformDefParams.h"
#include "CoordinateSystem/CoordinateSystemGeodeticInterpolationTransformDefParams.h"
#include "CoordinateSystem/CoordinateSystemGeodeticMultipleRegressionTransformDefParams.h"
#include "CoordinateSystem/CoordinateSystemGeodeticAnalyticalTransformDefParams.h"
#include "CoordinateSystem/CoordinateSystemGeodeticStandaloneTransformDefParams.h"
#include "CoordinateSystem/CoordinateSystemGeodeticTransformDefType.h"
#include "CoordinateSystem/CoordinateSystemGeodeticDirection.h"
#include "CoordinateSystem/CoordinateSystemGeodeticStandaloneTransformationMethod.h"
#include "CoordinateSystem/CoordinateSystemGeodeticAnalyticalTransformationMethod.h"
#include "CoordinateSystem/CoordinateSystemGeodeticMultipleRegressionTransformationMethod.h"
#include "CoordinateSystem/CoordinateSystemGeodeticTransformDef.h"
#include "CoordinateSystem/CoordinateSystemGeodeticPathElement.h"
#include "CoordinateSystem/CoordinateSystemGeodeticPath.h"
#include "CoordinateSystem/CoordinateSystemGeodeticTransformation.h"
#include "CoordinateSystem/CoordinateSystemEllipsoid.h"
#include "CoordinateSystem/CoordinateSystemCategory.h"
#include "CoordinateSystem/CoordinateSystemMeasure.h"
#include "CoordinateSystem/CoordinateSystemTransform.h"
#include "CoordinateSystem/CoordinateSystemType.h"
#include "CoordinateSystem/CoordinateSystemDictionaryBase.h"                //for MgCoordinateSystemDictionaryBase
#include "CoordinateSystem/CoordinateSystemDictionary.h"                    //for MgCoordinateSystemDictionary
#include "CoordinateSystem/CoordinateSystemDatumDictionary.h"               //for MgCoordinateSystemDatumDictionary
#include "CoordinateSystem/CoordinateSystemEllipsoidDictionary.h"           //for MgCoordinateSystemEllipsoidDictionary
#include "CoordinateSystem/CoordinateSystemCategoryDictionary.h"            //for MgCoordinateSystemCategoryDictionary
#include "CoordinateSystem/CoordinateSystemGeodeticTransformDefDictionary.h"    //for MgCoordinateSystemGeodeticTransformDefDictionary
#include "CoordinateSystem/CoordinateSystemGeodeticPathDictionary.h"            //for MgCoordinateSystemGeodeticPathDictionary
#include "CoordinateSystem/CoordinateSystemDictionaryUtility.h"             //for MgCoordinateSystemDictionaryUtility
#include "CoordinateSystem/CoordinateSystemCatalog.h"
#include "CoordinateSystem/CoordinateSystemFilter.h"                        //for MgCoordinateSystemFilter
#include "CoordinateSystem/CoordinateSystemEnum.h"                          //for MgCoordinateSystemEnum
#include "CoordinateSystem/CoordinateSystemMathComparator.h"                //for MgCoordinateSystemMathComparator
#include "CoordinateSystem/CoordinateSystemWktFlavor.h"                     //for MgCoordinateSystemWktFlavor
#include "CoordinateSystem/CoordinateSystemCodeFormat.h"                    //for MgCoordinateSystemCodeFormat
#include "CoordinateSystem/CoordinateSystemFormatConverter.h"               //for MgCoordinateSystemFormatConverter
#include "CoordinateSystem/CoordinateSystemUnitCode.h"                      //for MgCoordinateSystemUnitCode
#include "CoordinateSystem/CoordinateSystemUnitType.h"                      //for MgCoordinateSystemUnitType
#include "CoordinateSystem/CoordinateSystemFilterInteger32.h"               //for MgCoordinateSystemFilterInteger32
#include "CoordinateSystem/CoordinateSystemEnumInteger32.h"                 //for MgCoordinateSystemEnumInteger32
#include "CoordinateSystem/CoordinateSystemUnitInformation.h"               //for MgCoordinateSystemUnitInformation
#include "CoordinateSystem/CoordinateSystemProjectionCode.h"                //for MgCoordinateSystemProjectionCode
#include "CoordinateSystem/CoordinateSystemProjectionInformation.h"         //for MgCoordinateSystemProjectionInformation
#include "CoordinateSystem/CoordinateSystemGeodeticTransformationMethod.h"  //for MgCoordinateSystemGeodeticTransformationMethod
#include "CoordinateSystem/CoordinateSystemProjectionLogicalType.h"         //for MgCoordinateSystemProjectionLogicalType
#include "CoordinateSystem/CoordinateSystemProjectionFormatType.h"          //for MgCoordinateSystemProjectionFormatType
#include "CoordinateSystem/CoordinateSystemProjectionParameterType.h"       //for MgCoordinateSystemProjectionParameterType
#include "CoordinateSystem/CoordinateSystemErrorCode.h"                     //for MgCoordinateSystemErrorCode
#include "CoordinateSystem/CoordinateSystemGridOrientation.h"               //for MgCoordinateSystemGridOrientation
#include "CoordinateSystem/CoordinateSystemGridSpecializationType.h"        //for MgCoordinateSystemGridSpecializationType
#include "CoordinateSystem/CoordinateSystemGrids.h"                         //for MgCoordinateSystemGridSpecification
#include "CoordinateSystem/CoordinateSystemGridGeneric.h"                   //for MgCoordinateSystemGridGeneric
#include "CoordinateSystem/CoordinateSystemMgrsGridLevel.h"                 //for MgCoordinateSystemMgrsGridLevel
#include "CoordinateSystem/CoordinateSystemMgrsLetteringScheme.h"           //for MgCoordinateSystemMgrsLetteringScheme
#include "CoordinateSystem/CoordinateSystemMgrsGridSquarePosition.h"        //for MgCoordinateSystemMgrsGridSquarePosition
#include "CoordinateSystem/CoordinateSystemMgrs.h"                          //for MgCoordinateSystemMgrs
#include "CoordinateSystem/CoordinateSystemFactory.h"

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
