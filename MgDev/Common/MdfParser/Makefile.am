AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = MdfParser.vcproj MdfParser.rc

INCLUDES = \
  -I../../Oem/dbxml-2.2.13/xerces-c-src/src/ \
  -I../../Common/MdfModel/

lib_LTLIBRARIES = libMgMdfParser.la

libMgMdfParser_la_SOURCES = \
  IOAreaRule.cpp \
  IOAreaSymbolization2D.cpp \
  IOAreaTypeStyle.cpp \
  IOBaseMapDefinition.cpp \
  IOBaseMapLayer.cpp \
  IOBaseMapLayerGroup.cpp \
  IOBlockSymbol.cpp \
  IOChannelBand.cpp \
  IODrawingLayerDefinition.cpp \
  IOExtra.cpp \
  IOFill.cpp \
  IOFontSymbol.cpp \
  IOGridColor.cpp \
  IOGridColorRule.cpp \
  IOGridColorStyle.cpp \
  IOGridLayerDefinition.cpp \
  IOGridScaleRange.cpp \
  IOGridSurfaceStyle.cpp \
  IOHillShade.cpp \
  IOImageSymbol.cpp \
  IOLabel.cpp \
  IOLineRule.cpp \
  IOLineSymbolization2D.cpp \
  IOLineTypeStyle.cpp \
  IOMapDefinition.cpp \
  IOMapLayerCommon.cpp \
  IOMapLayer.cpp \
  IOMapLayerGroupCommon.cpp \
  IOMapLayerGroup.cpp \
  IOMarkSymbol.cpp \
  IONameStringPair.cpp \
  IOPointRule.cpp \
  IOPointSymbolization2D.cpp \
  IOPointTypeStyle.cpp \
  IOResourceRef.cpp \
  IOStroke.cpp \
  IOSymbol.cpp \
  IOUtil.cpp \
  IOVectorLayerDefinition.cpp \
  IOVectorScaleRange.cpp \
  IOW2DSymbol.cpp \
  MdfParser.cpp \
  SAX2Parser.cpp

noinst_HEADERS = \
  IOAreaRule.h \
  IOAreaSymbolization2D.h \
  IOAreaTypeStyle.h \
  IOBaseMapDefinition.h \
  IOBaseMapLayerGroup.h \
  IOBaseMapLayer.h \
  IOBlockSymbol.h \
  IOChannelBand.h \
  IODrawingLayerDefinition.h \
  IOExtra.h \
  IOFill.h \
  IOFontSymbol.h \
  IOGridColor.h \
  IOGridColorRule.h \
  IOGridColorStyle.h \
  IOGridLayerDefinition.h \
  IOGridScaleRange.h \
  IOGridSurfaceStyle.h \
  IOHillShade.h \
  IOImageSymbol.h \
  IOLabel.h \
  IOLineRule.h \
  IOLineSymbolization2D.h \
  IOLineTypeStyle.h \
  IOMapDefinition.h \
  IOMapLayerCommon.h \
  IOMapLayerGroupCommon.h \
  IOMapLayerGroup.h \
  IOMapLayer.h \
  IOMarkSymbol.h \
  IONameStringPair.h \
  IOPointRule.h \
  IOPointSymbolization2D.h \
  IOPointTypeStyle.h \
  IOResourceRef.h \
  IOStroke.h \
  IOSymbol.h \
  IOUtil.h \
  IOVectorLayerDefinition.h \
  IOVectorScaleRange.h \
  IOW2DSymbol.h \
  MdfParser.h \
  SAX2ElementHandler.h \
  SAX2Parser.h \
  stdafx.h

libMgMdfParser_la_LIBADD = -lMgMdfModel -lxerces-c

libMgMdfParser_la_LDFLAGS = -version-info 7:0:0 \
  -L../MdfModel \
  -L../../Oem/dbxml-2.2.13/install/lib
