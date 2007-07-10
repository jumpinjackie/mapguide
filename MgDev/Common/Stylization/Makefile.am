AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Stylization.vcproj Stylization.rc

INCLUDES = \
  -I../MdfModel/ \
  -I$(map_fdo_include)

lib_LTLIBRARIES = libMgStylization.la

libMgStylization_la_SOURCES = \
  RS_FontEngine.cpp \
  SE_Bounds.cpp \
  SE_IdentityJoin.cpp \
  SE_MiterJoin.cpp \
  SE_ExpressionBase.cpp \
  SE_LineBuffer.cpp \
  SE_BufferPool.cpp \
  SE_LineStorage.cpp \
  SE_PositioningAlgorithms.cpp \
  SE_Renderer.cpp \
  SE_StyleVisitor.cpp \
  SE_SymbolDefProxies.cpp \
  StylizationEngine.cpp \
  LineBuffer.cpp \
  LineStyleDef.cpp \
  DefaultStylizer.cpp \
  GeometryAdapter.cpp \
  PointAdapter.cpp \
  PolygonAdapter.cpp \
  PolylineAdapter.cpp \
  RasterAdapter.cpp \
  Stylizer.cpp \
  BooleanValue.cpp \
  DataValuePool.cpp \
  DateTimeValue.cpp \
  DoubleValue.cpp \
  FilterExecutor.cpp \
  Int64Value.cpp \
  StringValue.cpp \
  PolygonUtils.cpp \
  KeyEncode.cpp \
  LabelRenderer.cpp \
  LabelRendererBase.cpp \
  LabelRendererLocal.cpp \
  SimpleOverpost.cpp \
  Matrix3D.cpp \
  Point2D.cpp \
  Point3D.cpp \
  Vector2D.cpp \
  Vector3D.cpp

noinst_HEADERS = \
  RS_FontEngine.h \
  SE_Bounds.h \
  SE_ConvexHull.h \
  SE_PiecewiseTransform.h \
  SE_IdentityJoin.h \
  SE_MiterJoin.h \
  SE_ExpressionBase.h \
  SE_SymbolDefProxies.h \
  SE_RenderProxies.h \
  SE_LineBuffer.h \
  SE_BufferPool.h \
  SE_LineStorage.h \
  SE_Matrix.h \
  SE_PositioningAlgorithms.h \
  SE_Renderer.h \
  SE_StyleVisitor.h \
  SE_SymbolManager.h \
  StylizationEngine.h \
  Bounds.h \
  LineBuffer.h \
  LineStyle.h \
  LineStyleDef.h \
  Renderer.h \
  RendererStyles.h \
  DefaultStylizer.h \
  GeometryAdapter.h \
  PointAdapter.h \
  PolygonAdapter.h \
  PolylineAdapter.h \
  RasterAdapter.h \
  Stylizer.h \
  stdafx.h \
  Stylization.h \
  BooleanValue.h \
  DataValue.h \
  DataValueStack.h \
  DataValuePool.h \
  DateTimeValue.h \
  DoubleValue.h \
  FilterExecutor.h \
  Int64Value.h \
  StringValue.h \
  PolygonUtils.h \
  RS_InputStream.h \
  RS_OutputStream.h \
  CSysTransformer.h \
  SymbolManager.h \
  RS_FeatureReader.h \
  RS_Raster.h \
  FeatureTypeStyleVisitor.h \
  RSBufferOutputStream.h \
  KeyEncode.h \
  LabelRenderer.h \
  LabelRendererBase.h \
  LabelRendererLocal.h \
  SimpleOverpost.h \
  RS_Font.h \
  SymbolVisitor.h \
  SLDSymbols.h \
  Matrix3D.h \
  Point2D.h \
  Point3D.h \
  Vector2D.h \
  Vector3D.h

libMgStylization_la_LIBADD =

libMgStylization_la_LDFLAGS = -version-info 7:0:0
