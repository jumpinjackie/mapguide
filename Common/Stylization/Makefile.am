AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Stylization.vcproj Stylization.rc

INCLUDES = \
  -I../MdfModel/ \
  -I$(map_fdo_include) \
  -I../../Oem/DWFTK7.1/develop/global/src/ \
  -I../../Oem/DWFTK7.1/develop/global/src/dwf/ \
  -I../../Oem/DWFTK7.1/develop/global/src/dwfemap \
  -I../../Oem/gd/gd \
  -I../../Oem/gd/freetype/include

lib_LTLIBRARIES = libMgStylization.la

libMgStylization_la_SOURCES = \
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
  DWFRenderer.cpp \
  EMapRenderer.cpp \
  EPlotRenderer.cpp \
  EMapUpdateRenderer.cpp \
  Centroid.cpp \
  W2DRewriter.cpp \
  SymbolTrans.cpp \
  ObservationMesh.cpp \
  KeyEncode.cpp \
  GDRenderer.cpp \
  GDW2DRewriter.cpp \
  GDFillPatterns.cpp \
  GDUtils.cpp \
  LabelRenderer.cpp \
  LabelRendererLocal.cpp \
  SimpleOverpost.cpp \
  RS_ByteData.cpp \
  RS_Font.cpp \
  FontManager.cpp


noinst_HEADERS = \
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
  DWFRenderer.h \
  EMapRenderer.h \
  EPlotRenderer.h \
  EMapUpdateRenderer.h \
  Centroid.h \
  DWFRSInputStream.h \
  RSDWFInputStream.h \
  RS_InputStream.h \
  RS_OutputStream.h \
  CSysTransformer.h \
  SymbolManager.h \
  SymbolTrans.h \
  RS_FeatureReader.h \
  RS_Raster.h \
  FeatureTypeStyleVisitor.h \
  ObservationMesh.h \
  RSDWFOutputStream.h \
  RSBufferOutputStream.h \
  KeyEncode.h \
  whip_fill_library.h \
  whip_hatch_library.h \
  GDRenderer.h \
  GDW2DRewriter.h \
  GDFillPatterns.h \
  GDUtils.h \
  W2DRewriter.h \
  LabelRenderer.h \
  LabelRendererBase.h \
  LabelRendererLocal.h \
  SimpleOverpost.h \
  RS_ByteData.h \
  RS_Font.h \
  FontManager.h \
  SymbolVisitor.h \
  SLDSymbols.h

CXXFLAGS = @CXXFLAGS@ -DDWFTK_BUILD_EXPAT

libMgStylization_la_LIBADD = \
  ../../Oem/gd/gd/libgd.a \
  ../../Oem/gd/freetype/objs/libfreetype.a \
  ../../Oem/gd/jpeg/lib/libjpeg.a \
  ../../Oem/gd/lpng/libpng.a

libMgStylization_la_LDFLAGS = -version-info 7:0:0
