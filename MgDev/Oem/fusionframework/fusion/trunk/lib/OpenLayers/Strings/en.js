OpenLayers.Strings.en = {
'test1': 'this is a test',
'test2': 'and another test',
'test3': 'arg one:{0} arg two: {1}',
'unhandledRequest': "Unhandled request return {0}",
'permalink': "Permalink",
'overlays': "Overlays",
'baseLayer': "Base Layer",
'sameProjection': "The overview map only works when it is in the same projection as the main map",
'readNotImplemented': "Read not implemented.",
'writeNotImplemented': "Write not implemented.",
'noFID': "Can't update a feature for which there is no FID.",
'errorLoadingGML': "Error in loading GML file {0}",
'browserNotSupported': "Your browser does not support vector rendering. Currently supported renderers are:\n{0}",
'componentShouldBe': "addFeatures : component should be an {0}",
'getFeatureError': "getFeatureFromEvent called on layer with no renderer. " +
                   "This usually means you destroyed a layer, but not some handler which is associated with it.",
'minZoomLevelError': "The minZoomLevel property is only intended for use " +
                    "with the FixedZoomLevels-descendent layers. That this " +
                    "wfs layer checks for minZoomLevel is a relic of the" +
                    "past. We cannot, however, remove it without possibly " +
                    "breaking OL based applications that may depend on it." +
                    " Therefore we are deprecating it -- the minZoomLevel " +
                    "check below will be removed at 3.0. Please instead " +
                    "use min/max resolution setting as described here: " +
                    "http://trac.openlayers.org/wiki/SettingZoomLevels",
'commitSuccess': "WFS Transaction: SUCCESS {0}",
'commitFailed': "WFS Transaction: FAILED {0}",
'googleWarning': "The Google Layer was unable to load correctly.<br><br>" +
                "To get rid of this message, select a new BaseLayer " +
                "in the layer switcher in the upper-right corner.<br><br>" +
                "Most likely, this is because the Google Maps library " +
                "script was either not included, or does not contain the " +
                "correct API key for your site.<br><br>" +
                "Developers: For help getting this working correctly, " +
                "<a href='http://trac.openlayers.org/wiki/Google' " +
                "target='_blank'>click here</a>",
'getLayerWarning': "The {0} Layer was unable to load correctly.<br><br>" +
                "To get rid of this message, select a new BaseLayer " +
                "in the layer switcher in the upper-right corner.<br><br>" +
                "Most likely, this is because the {0} library " +
                "script was either not correctly included.<br><br>" +
                "Developers: For help getting this working correctly, " +
                "<a href='http://trac.openlayers.org/wiki/{1}' " +
                "target='_blank'>click here</a>",
'scale': "Scale = 1 : {0}",
'layerAlreadyAdded': "You tried to add the layer: {0} to the map, but it has already been added",
'reprojectDeprecated': "You are using the 'reproject' option " +
                "on the {0} layer. This option is deprecated: " +
                "its use was designed to support displaying data over commercial " + 
                "basemaps, but that functionality should now be achieved by using " +
                "Spherical Mercator support. More information is available from " +
                "http://trac.openlayers.org/wiki/SphericalMercator.",
'methodDeprecated': "This method has been deprecated and will be removed in 3.0. " +
                "Please use {0} instead.",
'boundsAddError': "You must pass both x and y values to the add function.",
'lonlatAddError': "You must pass both lon and lat values to the add function.",
'pixelAddError': "You must pass both x and y values to the add function.",
'unsupportedGeometryType': "Unsupported geometry type: {0}",
'clearArrayDeprecated': "OpenLayers.Util.clearArray() is Deprecated." +
                " Please use 'array.length = 0' instead.",
'getArgsDeprecated': "The getArgs() function is deprecated and will be removed " +
                "with the 3.0 version of OpenLayers. Please instead use " +
                "OpenLayers.Util.getParameters().",
'pagePositionFailed': "OpenLayers.Util.pagePosition failed: element with id {0} may be misplaced.",
                
'end': ''
};