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
  EMapUpdateRenderer.cpp \
  EPlotRenderer.cpp \
  FontManager.cpp \
  GDFillPatterns.cpp \
  GDRenderer.cpp \
  GDUtils.cpp \
  GDW2DRewriter.cpp \
  KmlContent.cpp \
  KmlLineStyle.cpp \
  KmlPolyStyle.cpp \
  KmlRenderer.cpp \
  ObservationMesh.cpp \
  RS_ByteData.cpp \
  SymbolTrans.cpp \
  W2DRewriter.cpp \
  complex_polygon_gd.cpp

noinst_HEADERS = \
  DWFRenderer.h \
  DWFRSInputStream.h \
  EMapRenderer.h \
  EMapUpdateRenderer.h \
  EPlotRenderer.h \
  FontManager.h \
  GDFillPatterns.h \
  GDRenderer.h \
  GDUtils.h \
  GDW2DRewriter.h \
  KmlContent.h \
  KmlLineStyle.h \
  KmlPolyStyle.h \
  KmlRenderer.h \
  ObservationMesh.h \
  Renderers.h \
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
  ../../Oem/gd/lpng/libpng.a

libMgRenderers_la_LDFLAGS = -version-info 7:0:0
