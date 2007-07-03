AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = Renderers.vcproj Renderers.rc

INCLUDES = \
  -I../MdfModel/ \
  -I../Stylization/ \
  -I$(map_fdo_include) \
  -I../../Oem/DWFTK7.1/develop/global/src/ \
  -I../../Oem/DWFTK7.1/develop/global/src/dwf/ \
  -I../../Oem/gd/gd/ \
  -I../../Oem/gd/freetype/include/

lib_LTLIBRARIES = libMgRenderers.la

libMgRenderers_la_SOURCES = \
  DWFRenderer.cpp \
  EMapRenderer.cpp \
  EPlotRenderer.cpp \
  EMapUpdateRenderer.cpp \
  KmlRenderer.cpp \
  KmlContent.cpp \
  KmlLineStyle.cpp \
  KmlPolyStyle.cpp \
  W2DRewriter.cpp \
  SymbolTrans.cpp \
  ObservationMesh.cpp \
  GDRenderer.cpp \
  GDW2DRewriter.cpp \
  GDFillPatterns.cpp \
  GDUtils.cpp \
  RS_ByteData.cpp \
  FontManager.cpp \
  complex_polygon_gd.cpp

noinst_HEADERS = \
  stdafx.h \
  Renderers.h \
  DWFRenderer.h \
  EMapRenderer.h \
  EPlotRenderer.h \
  EMapUpdateRenderer.h \
  KmlRenderer.h \
  KmlContent.h \
  KmlLineStyle.h \
  KmlPolyStyle.h \
  DWFRSInputStream.h \
  RSDWFInputStream.h \
  SymbolTrans.h \
  ObservationMesh.h \
  RSDWFOutputStream.h \
  whip_fill_library.h \
  whip_hatch_library.h \
  GDRenderer.h \
  GDW2DRewriter.h \
  GDFillPatterns.h \
  GDUtils.h \
  W2DRewriter.h \
  RS_ByteData.h \
  FontManager.h \
  complex_polygon_gd.h

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_BUILD_EXPAT

libMgRenderers_la_LIBADD = \
  ../../Oem/gd/gd/.libs/libgd.a \
  ../../Oem/gd/freetype/objs/.libs/libfreetype.a \
  ../../Oem/gd/jpeg/.libs/libjpeg.a \
  ../../Oem/gd/lpng/libpng.a

libMgRenderers_la_LDFLAGS = -version-info 7:0:0
