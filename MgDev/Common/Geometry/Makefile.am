AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Geometry.vcproj Geometry.rc Parse/script Parse/script_linux Parse/yyAwkt.y

lib_LTLIBRARIES = libMgGeometry.la

libMgGeometry_la_SOURCES = \
  GeometryBuild.cpp

include_SOURCES = \
  AgfReaderWriter.cpp \
  AgfStream.cpp \
  AggregateGeometry.cpp \
  ArcSegment.cpp \
  Coordinate.cpp \
  CoordinateCollection.cpp \
  CoordinateIterator.cpp \
  CoordinateXY.cpp \
  CoordinateXYM.cpp \
  CoordinateXYZ.cpp \
  CoordinateXYZM.cpp \
  Curve.cpp \
  CurvePolygon.cpp \
  CurvePolygonCollection.cpp \
  CurveRing.cpp \
  CurveRingCollection.cpp \
  CurveSegmentCollection.cpp \
  CurveString.cpp \
  CurveStringCollection.cpp \
  Envelope.cpp \
  DumpFFGF.cpp \
  GeometricEntity.cpp \
  Geometry.cpp \
  GeometryClassFactory.cpp \
  GeometryCollection.cpp \
  GeometryFactory.cpp \
  GeometryUtil.cpp \
  GeosUtil.cpp \
  LinearRing.cpp \
  LinearRingCollection.cpp \
  LinearSegment.cpp \
  LineString.cpp \
  LineStringCollection.cpp \
  MultiCurvePolygon.cpp \
  MultiCurveString.cpp \
  MultiGeometry.cpp \
  MultiLineString.cpp \
  MultiPoint.cpp \
  MultiPolygon.cpp \
  Point.cpp \
  PointCollection.cpp \
  Polygon.cpp \
  PolygonCollection.cpp \
  Region.cpp \
  Ring.cpp \
  WktReaderWriter.cpp\
  Buffer/BorderWalker.cpp \
  Buffer/bufrutil.cpp \
  Buffer/eventarr.cpp \
  Buffer/FloatTransform.cpp \
  Buffer/GreatCircleBufferUtil.cpp \
  Buffer/interlst.cpp \
  Buffer/interprc.cpp \
  Buffer/LatLonBorderWalker.cpp \
  Buffer/MgBuffer.cpp \
  Buffer/ogeomtry.cpp \
  Buffer/oobject.cpp \
  Buffer/opline.cpp \
  Buffer/opolygon.cpp \
  Buffer/oppolygn.cpp \
  Buffer/oppolyln.cpp \
  Buffer/orpplygn.cpp \
  Buffer/ortree.cpp \
  Buffer/planeswp.cpp \
  Buffer/plgnbufr.cpp \
  Buffer/plgnsbak.cpp \
  Buffer/plinbufr.cpp \
  Buffer/pobjbufr.cpp \
  Buffer/ppgunion.cpp \
  Buffer/progcbck.cpp \
  Buffer/sedgaloc.cpp \
  Buffer/sweepedg.cpp \
  Buffer/tuplearr.cpp \
  Buffer/vertaloc.cpp \
  Buffer/wedgaloc.cpp \
  Buffer/wedgearr.cpp \
  Buffer/wingedge.cpp \
  Buffer/worklist.cpp \
  Buffer/xordtree.cpp \
  CoordinateSystem/CoordinateSystemCache.cpp \
  CoordinateSystem/CoordinateSystemFactory.cpp \
  Exception/CoordinateSystemComputationFailedException.cpp \
  Exception/CoordinateSystemConversionFailedException.cpp \
  Exception/CoordinateSystemInitializationFailedException.cpp \
  Exception/CoordinateSystemLoadFailedException.cpp \
  Exception/CoordinateSystemMeasureFailedException.cpp \
  Exception/CoordinateSystemTransformFailedException.cpp \
  Exception/GeometryException.cpp\
  Exception/InvalidCoordinateSystemException.cpp \
  Exception/InvalidCoordinateSystemTypeException.cpp \
  Exception/InvalidCoordinateSystemUnitsException.cpp \
  Parse/ArrayHelper.cpp \
  Parse/LexAwkt.cpp \
  Parse/ParseAwkt.cpp \
  Parse/ParseAwktUtil.cpp \
  Parse/StringUtility.cpp \
  Parse/yyAwkt.cpp \
  Parse/yyAwkt_linux.cpp \
  Spatial/MathUtility.cpp \
  Spatial/SpatialUtility.cpp \
  Spatial/SpatialUtilityCircularArc.cpp

noinst_HEADERS = $(include_SOURCES) \
  AgfReaderWriter.h \
  AgfStream.h \
  AggregateGeometry.h \
  ArcSegment.h \
  CloseInstruction.h \
  Coordinate.h \
  CoordinateCollection.h \
  CoordinateDimension.h \
  CoordinateIterator.h \
  CoordinateXY.h \
  CoordinateXYM.h \
  CoordinateXYZ.h \
  CoordinateXYZM.h \
  Curve.h \
  CurvePolygon.h \
  CurvePolygonCollection.h \
  CurveRing.h \
  CurveRingCollection.h \
  CurveSegment.h \
  CurveSegmentCollection.h \
  CurveString.h \
  CurveStringCollection.h \
  DumpFFGF.h \
  Envelope.h \
  GeometricDimension.h \
  GeometricEntity.h \
  GeometricEntityType.h \
  GeometricPathInstruction.h \
  GeometricPathInstructionType.h \
  GeometricPathIterator.h \
  Geometry.h \
  GeometryClassId.h \
  GeometryCollection.h \
  GeometryCommon.h \
  GeometryComponent.h \
  GeometryComponentCollection.h \
  GeometryComponentType.h \
  GeometryExceptionDef.h \
  GeometryFactory.h \
  GeometryType.h \
  GeometryUtil.h \
  GeosUtil.h \
  LinearRing.h \
  LinearRingCollection.h \
  LinearSegment.h \
  LineString.h \
  LineStringCollection.h \
  LineToInstruction.h \
  Measure.h \
  MoveToInstruction.h \
  MultiCurvePolygon.h \
  MultiCurveString.h \
  MultiGeometry.h \
  MultiLineString.h \
  MultiPoint.h \
  MultiPolygon.h \
  Point.h \
  PointCollection.h \
  Polygon.h \
  PolygonCollection.h \
  QuadToInstruction.h \
  Region.h \
  Ring.h \
  Transform.h \
  WktReaderWriter.h \
  Buffer/BorderWalker.h \
  Buffer/buffer.h \
  Buffer/bufraloc.h \
  Buffer/bufrsort.h \
  Buffer/bufrutil.h \
  Buffer/eventarr.h \
  Buffer/eventint.h \
  Buffer/FloatTransform.h \
  Buffer/GreatCircleBufferUtil.h \
  Buffer/interlst.h \
  Buffer/interprc.h \
  Buffer/LatLonBorderWalker.h \
  Buffer/MgBuffer.h \
  Buffer/oarray.h \
  Buffer/oextent.h \
  Buffer/ogeomtry.h \
  Buffer/olinep.h \
  Buffer/oobject.h \
  Buffer/opline.h \
  Buffer/opoint.h \
  Buffer/opolygnp.h \
  Buffer/opolygon.h \
  Buffer/oppolygn.h \
  Buffer/oppolyln.h \
  Buffer/opsprimitive.h \
  Buffer/orpplygn.h \
  Buffer/ortree.h \
  Buffer/performanceOptions.h \
  Buffer/planeswp.h \
  Buffer/plgnbufr.h \
  Buffer/plgnsbak.h \
  Buffer/plinbufr.h \
  Buffer/pobjbufr.h \
  Buffer/ppgunion.h \
  Buffer/progcbck.h \
  Buffer/sedgaloc.h \
  Buffer/sweepedg.h \
  Buffer/tuplearr.h \
  Buffer/vertaloc.h \
  Buffer/wedgaloc.h \
  Buffer/wedgearr.h \
  Buffer/wingedge.h \
  Buffer/worklist.h \
  Buffer/xordtree.h \
  CoordinateSystem/CoordinateSystem.h \
  CoordinateSystem/CoordinateSystemCommon.h \
  CoordinateSystem/CoordinateSystemCache.h \
  CoordinateSystem/CoordinateSystemFactory.h \
  CoordinateSystem/CoordinateSystemMeasure.h \
  CoordinateSystem/CoordinateSystemTransform.h \
  CoordinateSystem/CoordinateSystemType.h \
  Exception/CoordinateSystemComputationFailedException.h \
  Exception/CoordinateSystemConversionFailedException.h \
  Exception/CoordinateSystemInitializationFailedException.h \
  Exception/CoordinateSystemLoadFailedException.h \
  Exception/CoordinateSystemMeasureFailedException.h \
  Exception/CoordinateSystemTransformFailedException.h \
  Exception/GeometryException.h \
  Exception/InvalidCoordinateSystemException.h \
  Exception/InvalidCoordinateSystemTypeException.h \
  Exception/InvalidCoordinateSystemUnitsException.h \
  Parse/Array.h \
  Parse/ArrayHelper.h \
  Parse/LexAwkt.h \
  Parse/ParseAwkt.h \
  Parse/ParseAwktUtil.h \
  Parse/StringUtility.h \
  Parse/yyAwkt.h \
  Spatial/MathUtility.h \
  Spatial/SpatialUtility.h \
  Spatial/SpatialUtilityCircularArc.h \
  Spatial/SpatialUtilityVectorAngle.h


INCLUDES = \
  -I../Security \
  -I../MdfParser \
  -I../MdfModel \
  -I../Foundation \
  -I../Geometry \
  -I../../Oem/ACE/ACE_wrappers \
  -I../../Oem/geos-2.2.0/source/headers \
  -I../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../Oem/proj-4.5.0/src \
  -I../../Oem/gdal-1.4.0/ogr \
  -I../../Oem/gdal-1.4.0/port \
  -I$(top_builddir)/$(map_coordsys_include_path)

libMgGeometry_la_LIBADD = \
  -lACE \
  -lMgCoordinateSystem \
  -lMgFoundation \
  ../../Oem/geos-2.2.0/source/geom/.libs/libgeos.a
  

libMgGeometry_la_LDFLAGS = -version-info 7:0:0 \
  -L../../Oem/ACE/ACE_wrappers/ace \
  -L../Foundation \
  -L$(top_builddir)/$(map_coordsys_dir_path)
  
