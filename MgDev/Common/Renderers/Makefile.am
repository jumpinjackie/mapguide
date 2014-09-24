AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Renderers.vcproj Renderers.rc

INCLUDES = \
  -I../MdfModel/ \
  -I../Stylization/ \
  -I$(map_fdo_include) \
  -I$(map_fdo_include)/ExpressionEngine \
  -I../../Oem/DWFTK/develop/global/src/ \
  -I../../Oem/DWFTK/develop/global/src/dwf/ \
  -I../../Oem/gd/gd/ \
  -I../../Oem/gd/lpng \
  -I../../Oem/gd/freetype/include/ \
  -I../../Oem/agg-2.4/include \
  -I../../Oem/agg-2.4/font_freetype

lib_LTLIBRARIES = libMgRenderers.la

libMgRenderers_la_SOURCES = \
  AGGRenderer.cpp \
  AGGImageIO.cpp \
  AGGW2DRewriter.cpp \
  AGGFillPatterns.cpp \
  ../../Oem/agg-2.4/font_freetype/agg_font_freetype.cpp \
  CriticalSection.cpp \
  DWFRenderer.cpp \
  EPlotRenderer.cpp \
  FontManager.cpp \
  GDFillPatterns.cpp \
  GDRenderer.cpp \
  GDUtils.cpp \
  GDW2DRewriter.cpp \
  KmlContent.cpp \
  KmlLineStyle.cpp \
  KmlPolyStyle.cpp \
  KmlIconStyle.cpp \
  KmlRenderer.cpp \
  MapQuantization.cpp \
  ObservationMesh.cpp \
  RenderUtil.cpp \
  RS_ByteData.cpp \
  SymbolTrans.cpp \
  W2DRewriter.cpp \
  complex_polygon_gd.cpp

noinst_HEADERS = \
  AGGRenderer.h \
  AGGImageIO.h \
  agg_context.h \
  agg_mg_overloads.h \
  AGGW2DRewriter.h \
  AGGFillPatterns.h \
  CriticalSection.h \
  DWFRenderer.h \
  DWFRSInputStream.h \
  EPlotRenderer.h \
  FontManager.h \
  GDFillPatterns.h \
  GDRenderer.h \
  GDUtils.h \
  GDW2DRewriter.h \
  KmlContent.h \
  KmlLineStyle.h \
  KmlPolyStyle.h \
  KmlIconStyle.h \
  KmlRenderer.h \
  MapQuantization.h \
  ObservationMesh.h \
  Renderers.h \
  RenderUtil.h \
  RS_ByteData.h \
  RSDWFInputStream.h \
  RSDWFOutputStream.h \
  stdafx.h \
  SymbolTrans.h \
  W2DRewriter.h \
  whip_fill_library.h \
  whip_hatch_library.h \
  complex_polygon_gd.h

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_BUILD_EXPAT

libMgRenderers_la_LIBADD = \
  ../../Oem/gd/gd/.libs/libgd.a \
  ../../Oem/gd/freetype/objs/.libs/libfreetype.a \
  ../../Oem/gd/jpeg/.libs/libjpeg.a \
  ../../Oem/gd/lpng/libpng.a \
  ../../Oem/agg-2.4/src/libagg.a \
  -ldwfcore \
  -ldwftk

libMgRenderers_la_LDFLAGS = -release $(PACKAGE_VERSION) \
  -L../../Oem/DWFTK/develop/global/src/dwfcore/.libs \
  -L../../Oem/DWFTK/develop/global/src/dwf/.libs
