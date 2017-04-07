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

// Predefined ranges for each library
#define GEOMETRY_ID                     20000
#define GEOMETRY_COORDINATE_SYSTEM_ID   20500
#define GEOMETRY_EXCEPTION_ID           21000

// Geometry API
#define Geometry_Point                                    GEOMETRY_ID+0
#define Geometry_Envelope                                 GEOMETRY_ID+1
#define Geometry_GeometryFactory                          GEOMETRY_ID+2
#define Geometry_Coordinate                               GEOMETRY_ID+3
#define Geometry_AgfReaderWriter                          GEOMETRY_ID+4
#define Geometry_AggregateGeometry                        GEOMETRY_ID+5
#define Geometry_ArcSegment                               GEOMETRY_ID+6
#define Geometry_CloseInstruction                         GEOMETRY_ID+7
#define Geometry_CoordinateCollection                     GEOMETRY_ID+8
#define Geometry_CoordinateIterator                       GEOMETRY_ID+9
#define Geometry_Curve                                    GEOMETRY_ID+10
#define Geometry_CurvePolygon                             GEOMETRY_ID+11
#define Geometry_CurveRing                                GEOMETRY_ID+12
#define Geometry_CurveSegment                             GEOMETRY_ID+13
#define Geometry_CurveString                              GEOMETRY_ID+14
#define Geometry_GeometricDimension                       GEOMETRY_ID+15
#define Geometry_GeometricEntity                          GEOMETRY_ID+16
#define Geometry_GeometricPathInstruction                 GEOMETRY_ID+17
#define Geometry_GeometricPathIterator                    GEOMETRY_ID+18
#define Geometry_Geometry                                 GEOMETRY_ID+19
#define Geometry_GeometryCollection                       GEOMETRY_ID+20
#define Geometry_GeometryComponent                        GEOMETRY_ID+21
//unused                                                  GEOMETRY_ID+22
#define Geometry_LinearRing                               GEOMETRY_ID+23
#define Geometry_LinearSegment                            GEOMETRY_ID+24
#define Geometry_LinearString                             GEOMETRY_ID+25
#define Geometry_LineToInstrution                         GEOMETRY_ID+26
#define Geometry_Measure                                  GEOMETRY_ID+27
#define Geometry_MoveToInstruction                        GEOMETRY_ID+28
#define Geometry_MultiCurvePolygon                        GEOMETRY_ID+29
#define Geometry_MultiCurveString                         GEOMETRY_ID+30
#define Geometry_MultiGeometry                            GEOMETRY_ID+31
#define Geometry_MultiLineString                          GEOMETRY_ID+32
#define Geometry_MultiPoint                               GEOMETRY_ID+33
#define Geometry_MultiPolygon                             GEOMETRY_ID+34
#define Geometry_Polygon                                  GEOMETRY_ID+35
#define Geometry_QuadToInstruction                        GEOMETRY_ID+36
#define Geometry_Region                                   GEOMETRY_ID+37
#define Geometry_Ring                                     GEOMETRY_ID+38
#define Geometry_Transform                                GEOMETRY_ID+39
#define Geometry_WktReaderWriter                          GEOMETRY_ID+40
#define Geometry_CurveSegmentCollection                   GEOMETRY_ID+41
#define Geometry_LineString                               GEOMETRY_ID+42
#define Geometry_CurveStringCollection                    GEOMETRY_ID+43
#define Geometry_LineStringCollection                     GEOMETRY_ID+44
#define Geometry_PointCollection                          GEOMETRY_ID+45
#define Geometry_PolygonCollection                        GEOMETRY_ID+46
#define Geometry_CurvePolygonCollection                   GEOMETRY_ID+47
#define Geometry_CoordinateXY                             GEOMETRY_ID+48
#define Geometry_CoordinateXYZ                            GEOMETRY_ID+49
#define Geometry_CoordinateXYZM                           GEOMETRY_ID+50
#define Geometry_CoordinateXYM                            GEOMETRY_ID+51
#define Geometry_CurveRingCollection                      GEOMETRY_ID+52
#define Geometry_LinearRingCollection                     GEOMETRY_ID+53

// Simplifier
#define Geometry_GeometrySimplifier                       GEOMETRY_ID+54

// CoordinateSystem API
#define CoordinateSystem_CoordinateSystem                           GEOMETRY_COORDINATE_SYSTEM_ID+0
#define CoordinateSystem_CoordinateSystemFactory                    GEOMETRY_COORDINATE_SYSTEM_ID+1
#define CoordinateSystem_CoordinateSystemMeasure                    GEOMETRY_COORDINATE_SYSTEM_ID+2
#define CoordinateSystem_CoordinateSystemTransform                  GEOMETRY_COORDINATE_SYSTEM_ID+3
#define CoordinateSystem_CoordinateSystemCatalog                    GEOMETRY_COORDINATE_SYSTEM_ID+4
#define CoordinateSystem_CoordinateSystemFormatConverter            GEOMETRY_COORDINATE_SYSTEM_ID+5
#define CoordinateSystem_CoordinateSystemCategory                   GEOMETRY_COORDINATE_SYSTEM_ID+6
#define CoordinateSystem_CoordinateSystemDatum                      GEOMETRY_COORDINATE_SYSTEM_ID+7
#define CoordinateSystem_CoordinateSystemGeodeticTransformation     GEOMETRY_COORDINATE_SYSTEM_ID+8
#define CoordinateSystem_CoordinateSystemEllipsoid                  GEOMETRY_COORDINATE_SYSTEM_ID+9
#define CoordinateSystem_CoordinateSystemCategoryDictionary         GEOMETRY_COORDINATE_SYSTEM_ID+10
#define CoordinateSystem_CoordinateSystemDictionary                 GEOMETRY_COORDINATE_SYSTEM_ID+11
#define CoordinateSystem_CoordinateSystemDatumDictionary            GEOMETRY_COORDINATE_SYSTEM_ID+12
#define CoordinateSystem_CoordinateSystemEllipsoidDictionary        GEOMETRY_COORDINATE_SYSTEM_ID+13
#define CoordinateSystem_CoordinateSystemEnum                       GEOMETRY_COORDINATE_SYSTEM_ID+14
#define CoordinateSystem_CoordinateSystemEnumInteger32              GEOMETRY_COORDINATE_SYSTEM_ID+15
#define CoordinateSystem_CoordinateSystemFilter                     GEOMETRY_COORDINATE_SYSTEM_ID+16
#define CoordinateSystem_CoordinateSystemFilterInteger32            GEOMETRY_COORDINATE_SYSTEM_ID+17
#define CoordinateSystem_CoordinateSystemMathComparator             GEOMETRY_COORDINATE_SYSTEM_ID+18
#define CoordinateSystem_CoordinateSystemUnitInformation            GEOMETRY_COORDINATE_SYSTEM_ID+19
#define CoordinateSystem_CoordinateSystemProjectionInformation      GEOMETRY_COORDINATE_SYSTEM_ID+20
#define CoordinateSystem_CoordinateSystemDictionaryUtility          GEOMETRY_COORDINATE_SYSTEM_ID+21
#define CoordinateSystem_CoordinateSystemMgrs                       GEOMETRY_COORDINATE_SYSTEM_ID+22

#define CoordinateSystem_CoordinateSystemGridGeneric                GEOMETRY_COORDINATE_SYSTEM_ID+23
#define CoordinateSystem_CoordinateSystemGridBoundary               GEOMETRY_COORDINATE_SYSTEM_ID+24
#define CoordinateSystem_CoordinateSystemGridSpecification          GEOMETRY_COORDINATE_SYSTEM_ID+25
#define CoordinateSystem_CoordinateSystemGridLineCollection         GEOMETRY_COORDINATE_SYSTEM_ID+26
#define CoordinateSystem_CoordinateSystemGridRegionCollection       GEOMETRY_COORDINATE_SYSTEM_ID+27
#define CoordinateSystem_CoordinateSystemGridTickCollection         GEOMETRY_COORDINATE_SYSTEM_ID+28
#define CoordinateSystem_CoordinateSystemGridLine                   GEOMETRY_COORDINATE_SYSTEM_ID+29
#define CoordinateSystem_CoordinateSystemGridRegion                 GEOMETRY_COORDINATE_SYSTEM_ID+30
#define CoordinateSystem_CoordinateSystemGridTick                   GEOMETRY_COORDINATE_SYSTEM_ID+31
#define CoordinateSystem_CoordinateSystemGridBase                   GEOMETRY_COORDINATE_SYSTEM_ID+32

#define CoordinateSystem_CoordinateSystemGeodeticPath               GEOMETRY_COORDINATE_SYSTEM_ID+33
#define CoordinateSystem_CoordinateSystemGeodeticPathElement        GEOMETRY_COORDINATE_SYSTEM_ID+34
#define CoordinateSystem_CoordinateSystemGeodeticPathDictionary     GEOMETRY_COORDINATE_SYSTEM_ID+35

#define CoordinateSystem_CoordinateSystemGeodeticTransformDef                       GEOMETRY_COORDINATE_SYSTEM_ID+36
#define CoordinateSystem_CoordinateSystemGeodeticAnalyticalTransformDef             GEOMETRY_COORDINATE_SYSTEM_ID+37
#define CoordinateSystem_CoordinateSystemGeodeticInterpolationTransformDef          GEOMETRY_COORDINATE_SYSTEM_ID+38
#define CoordinateSystem_CoordinateSystemGeodeticMultipleRegressionTransformDef     GEOMETRY_COORDINATE_SYSTEM_ID+39
#define CoordinateSystem_CoordinateSystemGeodeticTransformDefDictionary             GEOMETRY_COORDINATE_SYSTEM_ID+40

#define CoordinateSystem_CoordinateSystemGeodeticTransformDefParams                     GEOMETRY_COORDINATE_SYSTEM_ID+41
#define CoordinateSystem_CoordinateSystemGeodeticAnalyticalTransformDefParams           GEOMETRY_COORDINATE_SYSTEM_ID+42
#define CoordinateSystem_CoordinateSystemGeodeticInterpolationTransformDefParams        GEOMETRY_COORDINATE_SYSTEM_ID+43
#define CoordinateSystem_CoordinateSystemGeodeticMultipleRegressionTransformDefParams   GEOMETRY_COORDINATE_SYSTEM_ID+44
#define CoordinateSystem_CoordinateSystemGeodeticTransformGridFile                      GEOMETRY_COORDINATE_SYSTEM_ID+45
#define CoordinateSystem_CoordinateSystemGeodeticStandaloneTransformDefParams           GEOMETRY_COORDINATE_SYSTEM_ID+46

// Exceptions
#define Geometry_Exception_MgCoordinateSystemComputationFailedException     GEOMETRY_EXCEPTION_ID+0
#define Geometry_Exception_MgCoordinateSystemConversionFailedException      GEOMETRY_EXCEPTION_ID+1
#define Geometry_Exception_MgCoordinateSystemInitializationFailedException  GEOMETRY_EXCEPTION_ID+2
#define Geometry_Exception_MgCoordinateSystemLoadFailedException            GEOMETRY_EXCEPTION_ID+3
#define Geometry_Exception_MgCoordinateSystemMeasureFailedException         GEOMETRY_EXCEPTION_ID+4
#define Geometry_Exception_MgCoordinateSystemTransformFailedException       GEOMETRY_EXCEPTION_ID+5
#define Geometry_Exception_MgGeometryException                              GEOMETRY_EXCEPTION_ID+6
#define Geometry_Exception_MgInvalidCoordinateSystemException               GEOMETRY_EXCEPTION_ID+7
#define Geometry_Exception_MgInvalidCoordinateSystemTypeException           GEOMETRY_EXCEPTION_ID+8
#define Geometry_Exception_MgInvalidCoordinateSystemUnitsException          GEOMETRY_EXCEPTION_ID+9



