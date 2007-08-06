AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Stylization.vcproj Stylization.rc

INCLUDES = \
  -I../MdfModel/ \
  -I$(map_fdo_include)

lib_LTLIBRARIES = libMgStylization.la

libMgStylization_la_SOURCES = \
  BooleanValue.cpp \
  DataValuePool.cpp \
  DateTimeValue.cpp \
  DefaultStylizer.cpp \
  DoubleValue.cpp \
  FilterExecutor.cpp \
  GeometryAdapter.cpp \
  Int64Value.cpp \
  KeyEncode.cpp \
  LabelRenderer.cpp \
  LabelRendererBase.cpp \
  LabelRendererLocal.cpp \
  LineBuffer.cpp \
  LineStyleDef.cpp \
  Matrix3D.cpp \
  Point2D.cpp \
  Point3D.cpp \
  PointAdapter.cpp \
  PolygonAdapter.cpp \
  PolygonUtils.cpp \
  PolylineAdapter.cpp \
  RasterAdapter.cpp \
  RS_FontEngine.cpp \
  SE_Bounds.cpp \
  SE_BufferPool.cpp \
  SE_ExpressionBase.cpp \
  SE_IdentityJoin.cpp \
  SE_LineBuffer.cpp \
  SE_LineStorage.cpp \
  SE_MiterJoin.cpp \
  SE_PositioningAlgorithms.cpp \
  SE_Renderer.cpp \
  SE_StyleVisitor.cpp \
  SE_SymbolDefProxies.cpp \
  SE_SymbolManager.cpp \
  SimpleOverpost.cpp \
  StringValue.cpp \
  StylizationEngine.cpp \
  Stylizer.cpp \
  Vector2D.cpp \
  Vector3D.cpp

noinst_HEADERS = \
  BooleanValue.h \
  Bounds.h \
  CSysTransformer.h \
  DataValue.h \
  DataValuePool.h \
  DataValueStack.h \
  DateTimeValue.h \
  DefaultStylizer.h \
  DoubleValue.h \
  FeatureTypeStyleVisitor.h \
  FilterExecutor.h \
  GeometryAdapter.h \
  Int64Value.h \
  KeyEncode.h \
  LabelRenderer.h \
  LabelRendererBase.h \
  LabelRendererLocal.h \
  LineBuffer.h \
  LineStyle.h \
  LineStyleDef.h \
  Matrix3D.h \
  Point2D.h \
  Point3D.h \
  PointAdapter.h \
  PolygonAdapter.h \
  PolygonUtils.h \
  PolylineAdapter.h \
  RasterAdapter.h \
  Renderer.h \
  RendererStyles.h \
  RS_BufferOutputStream.h \
  RS_FeatureReader.h \
  RS_Font.h \
  RS_FontEngine.h \
  RS_InputStream.h \
  RS_OutputStream.h \
  RS_Raster.h \
  RS_SymbolManager.h \
  SE_Bounds.h \
  SE_BufferPool.h \
  SE_ConvexHull.h \
  SE_ExpressionBase.h \
  SE_IdentityJoin.h \
  SE_LineBuffer.h \
  SE_LineStorage.h \
  SE_Matrix.h \
  SE_MiterJoin.h \
  SE_PiecewiseTransform.h \
  SE_PositioningAlgorithms.h \
  SE_Renderer.h \
  SE_RenderProxies.h \
  SE_StyleVisitor.h \
  SE_SymbolDefProxies.h \
  SE_SymbolManager.h \
  SimpleOverpost.h \
  SLDSymbols.h \
  stdafx.h \
  StringValue.h \
  Stylization.h \
  StylizationAPI.h \
  StylizationDefs.h \
  StylizationEngine.h \
  Stylizer.h \
  SymbolVisitor.h \
  Vector2D.h \
  Vector3D.h

libMgStylization_la_LIBADD =

libMgStylization_la_LDFLAGS = -version-info 7:0:0
