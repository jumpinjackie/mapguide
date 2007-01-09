AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = MdfParser.vcproj MdfParser.rc

INCLUDES = \
  -I../../Oem/dbxml-2.2.13/xerces-c-src/src/ \
  -I../../Common/MdfModel/

lib_LTLIBRARIES = libMgMdfParser.la

libMgMdfParser_la_SOURCES = \
  FSDSAX2Parser.cpp \
  IOAreaRule.cpp \
  IOAreaSymbolization2D.cpp \
  IOAreaTypeStyle.cpp \
  IOAttributeRelate.cpp \
  IOBaseMapDefinition.cpp \
  IOBaseMapLayer.cpp \
  IOBaseMapLayerGroup.cpp \
  IOBlockSymbol.cpp \
  IOCalculatedProperty.cpp \
  IOChannelBand.cpp \
  IODrawingLayerDefinition.cpp \
  IOExtension.cpp \
  IOExtra.cpp \
  IOFeatureSource.cpp \
  IOFill.cpp \
  IOFontSymbol.cpp \
  IOGridColor.cpp \
  IOGridColorBands.cpp \
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
  IOMapLayer.cpp \
  IOMapLayerCommon.cpp \
  IOMapLayerGroup.cpp \
  IOMapLayerGroupCommon.cpp \
  IOMarkSymbol.cpp \
  IONameStringPair.cpp \
  IOPointRule.cpp \
  IOPointSymbolization2D.cpp \
  IOPointTypeStyle.cpp \
  IORelateProperty.cpp \
  IOResourceRef.cpp \
  IOStroke.cpp \
  IOSupplementalSpatialContextInfo.cpp \
  IOSymbol.cpp \
  IOUnknown.cpp \
  IOUtil.cpp \
  IOVectorLayerDefinition.cpp \
  IOVectorScaleRange.cpp \
  IOW2DSymbol.cpp \
  MdfParser.cpp \
  SAX2ElementHandler.cpp \
  SAX2Parser.cpp

noinst_HEADERS = \
  FSDSAX2Parser.h \
  IOAreaRule.h \
  IOAreaSymbolization2D.h \
  IOAreaTypeStyle.h \
  IOAttributeRelate.h \
  IOBaseMapDefinition.h \
  IOBaseMapLayer.h \
  IOBaseMapLayerGroup.h \
  IOBlockSymbol.h \
  IOCalculatedProperty.h \
  IOChannelBand.h \
  IODrawingLayerDefinition.h \
  IOExtension.h \
  IOExtra.h \
  IOFeatureSource.h \
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
  IOMapLayer.h \
  IOMapLayerCommon.h \
  IOMapLayerGroup.h \
  IOMapLayerGroupCommon.h \
  IOMarkSymbol.h \
  IONameStringPair.h \
  IOPointRule.h \
  IOPointSymbolization2D.h \
  IOPointTypeStyle.h \
  IORelateProperty.h \
  IOResourceRef.h \
  IOStroke.h \
  IOSupplementalSpatialContextInfo.h \
  IOSymbol.h \
  IOUnknown.h \
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
