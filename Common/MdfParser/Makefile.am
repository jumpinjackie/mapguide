AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = MdfParser.vcproj MdfParser.rc

INCLUDES = \
  -I../../Oem/dbxml-2.3.10/xerces-c-src/src/ \
  -I../MdfModel/ \
  -I../MdfModel/Common/ \
  -I../MdfModel/PrintLayout/ \
  -I./Common/ \
  -I./PrintLayout/

lib_LTLIBRARIES = libMgMdfParser.la

libMgMdfParser_la_SOURCES = \
  FSDSAX2Parser.cpp \
  IOAreaRule.cpp \
  IOAreaSymbolization2D.cpp \
  IOAreaTypeStyle.cpp \
  IOAreaUsage.cpp \
  IOAttributeRelate.cpp \
  IOBaseMapDefinition.cpp \
  IOBaseMapLayer.cpp \
  IOBaseMapLayerGroup.cpp \
  IOBlockSymbol.cpp \
  IOCalculatedProperty.cpp \
  IOChannelBand.cpp \
  IOCompositeRule.cpp \
  IOCompositeSymbolization.cpp \
  IOCompositeTypeStyle.cpp \
  IOCompoundSymbolDefinition.cpp \
  IODrawingLayerDefinition.cpp \
  IOElevationSettings.cpp \
  IOExtension.cpp \
  IOExtra.cpp \
  IOFeatureSource.cpp \
  IOFill.cpp \
  IOFontSymbol.cpp \
  IOGraphicElement.cpp \
  IOGraphicElementCollection.cpp \
  IOGridColor.cpp \
  IOGridColorBands.cpp \
  IOGridColorRule.cpp \
  IOGridColorStyle.cpp \
  IOGridLayerDefinition.cpp \
  IOGridScaleRange.cpp \
  IOGridSurfaceStyle.cpp \
  IOHillShade.cpp \
  IOImage.cpp \
  IOImageSymbol.cpp \
  IOLabel.cpp \
  IOLineRule.cpp \
  IOLineSymbolization2D.cpp \
  IOLineTypeStyle.cpp \
  IOLineUsage.cpp \
  IOMapDefinition.cpp \
  IOMapLayer.cpp \
  IOMapLayerCommon.cpp \
  IOMapLayerGroup.cpp \
  IOMapLayerGroupCommon.cpp \
  IOMarkSymbol.cpp \
  IONameStringPair.cpp \
  IOOverride.cpp \
  IOOverrideCollection.cpp \
  IOParameter.cpp \
  IOParameterCollection.cpp \
  IOPath.cpp \
  IOPointRule.cpp \
  IOPointSymbolization2D.cpp \
  IOPointTypeStyle.cpp \
  IOPointUsage.cpp \
  IORelateProperty.cpp \
  IOResizeBox.cpp \
  IOResourceRef.cpp \
  IOSimpleSymbol.cpp \
  IOSimpleSymbolDefinition.cpp \
  IOStroke.cpp \
  IOSupplementalSpatialContextInfo.cpp \
  IOSymbol.cpp \
  IOSymbolInstance.cpp \
  IOText.cpp \
  IOTextFrame.cpp \
  IOThemeLabel.cpp \
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
  IOAreaUsage.h \
  IOAttributeRelate.h \
  IOBaseMapDefinition.h \
  IOBaseMapLayer.h \
  IOBaseMapLayerGroup.h \
  IOBlockSymbol.h \
  IOCalculatedProperty.h \
  IOChannelBand.h \
  IOCompositeRule.h \
  IOCompositeSymbolization.h \
  IOCompositeTypeStyle.h \
  IOCompoundSymbolDefinition.h \
  IODrawingLayerDefinition.h \
  IOElevationSettings.h \
  IOExtension.h \
  IOExtra.h \
  IOFeatureSource.h \
  IOFill.h \
  IOFontSymbol.h \
  IOGraphicElement.h \
  IOGraphicElementCollection.h \
  IOGridColor.h \
  IOGridColorRule.h \
  IOGridColorStyle.h \
  IOGridLayerDefinition.h \
  IOGridScaleRange.h \
  IOGridSurfaceStyle.h \
  IOHillShade.h \
  IOImage.h \
  IOImageSymbol.h \
  IOLabel.h \
  IOLineRule.h \
  IOLineSymbolization2D.h \
  IOLineTypeStyle.h \
  IOLineUsage.h \
  IOMapDefinition.h \
  IOMapLayer.h \
  IOMapLayerCommon.h \
  IOMapLayerGroup.h \
  IOMapLayerGroupCommon.h \
  IOMarkSymbol.h \
  IONameStringPair.h \
  IOOverride.h \
  IOOverrideCollection.h \
  IOParameter.h \
  IOParameterCollection.h \
  IOPath.h \
  IOPointRule.h \
  IOPointSymbolization2D.h \
  IOPointTypeStyle.h \
  IOPointUsage.h \
  IORelateProperty.h \
  IOResizeBox.h \
  IOResourceRef.h \
  IOSimpleSymbol.h \
  IOSimpleSymbolDefinition.h \
  IOStroke.h \
  IOSupplementalSpatialContextInfo.h \
  IOSymbol.h \
  IOSymbolInstance.h \
  IOText.h \
  IOTextFrame.h \
  IOThemeLabel.h \
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
  -L../../Oem/dbxml-2.3.10/install/lib
