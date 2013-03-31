/**
 * Fusion.Widget.BasemapSwitcher
 *
 * $Id: BasemapSwitcher.js 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

 /*****************************************************************************
 * Class: Fusion.Widget.BasemapSwitcher
 *
 * A widget to allow selection of the basemaps display under the overlay MapGuide layer
 * Currently, Google Street, Google Satellite, Google Hybrid, Yahoo Street, Yahoo Satellite,
 * Yahoo Hybrid, OpenStreetMap Mapnik, OpenStreetMap Cycle, OpenStreetMap Transport,
 * Bing Street, Bing Satellite and Bing Hybrid is supported. 
 * 
 *  Inherits from:
 *  - <Fusion.Widget>
 ****************************************************************************/

Fusion.Widget.BasemapSwitcher = OpenLayers.Class(Fusion.Widget, {
    uiClass: Jx.Menu,

    options: {},

    baseMaps: {},

    defaultBasemap: null,

    activeBasemap: null,

    menuItems: {},
    
    zoomOffsets: {
        'G_NORMAL_MAP': 0,
        'G_SATELLITE_MAP': 0,
        'G_HYBRID_MAP': 0,
        'G_PHYSICAL_MAP': 0,
        'YAHOO_MAP_REG': 0,
        'YAHOO_MAP_SAT': 0,
        'YAHOO_MAP_HYB': 0,
        'Road': -1,
        'Aerial': -1,
        'Hybrid': -1,
        'Mapnik': 0,
        'TransportMap': 0,
        'CycleMap': 0,
        'None': 0
    },

    initializeWidget: function(widgetTag) {
        this.getMap().registerForEvent(Fusion.Event.MAP_MAP_GROUP_LOADED, OpenLayers.Function.bind(this.setDefaultBasemap, this));
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, OpenLayers.Function.bind(this.checkZoomOffsets, this));
    },

    refreshSettings: function() {
        this.baseMaps = {};
        this.defaultBasemap = null;
        this.menuItems = {};
        this.options = {
            'G_NORMAL_MAP': null,
            'G_SATELLITE_MAP': null,
            'G_HYBRID_MAP': null,
            'G_PHYSICAL_MAP': null,
            'YAHOO_MAP_REG': null,
            'YAHOO_MAP_SAT': null,
            'YAHOO_MAP_HYB': null,
            'Road': null,
            'Aerial': null,
            'Hybrid': null,
            'Mapnik': null,
            'TransportMap': null,
            'CycleMap': null,
            'None': null
        };
    },

    generateOptions: function() {
        // Clear previous settings 
        this.refreshSettings();

        var maps = this.getMap().aMaps;
        for (var i = 0, len = maps.length; i < len; i++) {
            var map = maps[i];
            switch (map.layerType) {
                case 'MapGuide':
                    this.options['None'] = OpenLayers.i18n('noCommercialMapLayers');
                    this.baseMaps['None'] = map;
                    break;
                case 'Google':
                    // if user didn't indicate basemap types, use the default Google Street
                    if (!map.mapTag.extension.Options || !map.mapTag.extension.Options[0].type) {
                        this.options['G_NORMAL_MAP'] = OpenLayers.i18n('googleStreet');
                        this.baseMaps['G_NORMAL_MAP'] = map;

                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = "G_NORMAL_MAP";
                        }
                    }
                    else {
                        switch (map.mapTag.extension.Options[0].type[0]) {
                            case 'G_NORMAL_MAP':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['G_NORMAL_MAP'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['G_NORMAL_MAP'] = OpenLayers.i18n('googleStreet');
                                }
                                this.baseMaps['G_NORMAL_MAP'] = map;
                                break;
                            case 'G_SATELLITE_MAP':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['G_SATELLITE_MAP'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['G_SATELLITE_MAP'] = OpenLayers.i18n('googleSatellite');
                                }
                                this.baseMaps['G_SATELLITE_MAP'] = map;
                                break;
                            case 'G_HYBRID_MAP':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['G_HYBRID_MAP'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['G_HYBRID_MAP'] = OpenLayers.i18n('googleHybrid');
                                }
                                this.baseMaps['G_HYBRID_MAP'] = map;
                                break;
                            case 'G_PHYSICAL_MAP':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['G_PHYSICAL_MAP'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['G_PHYSICAL_MAP'] = OpenLayers.i18n('googleTerrain');
                                }
                                this.baseMaps['G_PHYSICAL_MAP'] = map;
                                break;
                            default:
                                break;
                        }

                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = map.mapTag.extension.Options[0].type[0];
                        }
                    }
                    break;

                case 'Yahoo':
                    // if user didn't indicate basemap types, use the default Yahoo Street
                    if (!map.mapTag.extension.Options || !map.mapTag.extension.Options[0].type) {
                        this.options['YAHOO_MAP_REG'] = OpenLayers.i18n('yahooStreet');
                        this.baseMaps['YAHOO_MAP_REG'] = map;

                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = "YAHOO_MAP_REG";
                        }
                    }
                    else {
                        switch (map.mapTag.extension.Options[0].type[0]) {
                            case 'YAHOO_MAP_REG':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['YAHOO_MAP_REG'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['YAHOO_MAP_REG'] = "Yahoo Street";
                                }
                                this.baseMaps['YAHOO_MAP_REG'] = map;
                                break;
                            case 'YAHOO_MAP_SAT':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['YAHOO_MAP_SAT'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['YAHOO_MAP_SAT'] = OpenLayers.i18n('yahooStreet');
                                }
                                this.baseMaps['YAHOO_MAP_SAT'] = map;
                                break;
                            case 'YAHOO_MAP_HYB':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['YAHOO_MAP_HYB'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['YAHOO_MAP_HYB'] = OpenLayers.i18n('yahooHybrid');
                                }
                                this.baseMaps['YAHOO_MAP_HYB'] = map;
                                break;
                            default:
                                break;
                        }
                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = map.mapTag.extension.Options[0].type[0];
                        }
                    }
                    break;
                case 'VirtualEarth':
                    // if user didn't indicate basemap types, use the default Bing Street
                    if (!map.mapTag.extension.Options || !map.mapTag.extension.Options[0].type) {
                        this.options['Road'] = OpenLayers.i18n('bingStreet');
                        this.baseMaps['Road'] = map;
                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = "Road";
                        }
                    }
                    else {

                        switch (map.mapTag.extension.Options[0].type[0]) {
                            case 'Road':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['Road'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['Road'] = OpenLayers.i18n('bingStreet');
                                }
                                this.baseMaps['Road'] = map;
                                break;
                            case 'Aerial':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['Aerial'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['Aerial'] = OpenLayers.i18n('bingSatellite');
                                }
                                this.baseMaps['Aerial'] = map;
                                break;
                            case 'Hybrid':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['Hybrid'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['Hybrid'] = OpenLayers.i18n('bingHybrid');
                                }
                                this.baseMaps['Hybrid'] = map;
                                break;
                            default:
                                break;
                        }
                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = map.mapTag.extension.Options[0].type[0];
                        }
                    }
                    break;
                case 'OpenStreetMap':
                    // if user didn't indicate basemap types, use the default OSM Mapnik render
                    if (!map.mapTag.extension.Options || !map.mapTag.extension.Options[0].type) {
                        this.options['Mapnik'] = OpenLayers.i18n('openStreetMap');
                        this.baseMaps['Mapnik'] = map;
                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = "Mapnik";
                        }
                    }
                    else {

                        switch (map.mapTag.extension.Options[0].type[0]) {
                            case 'TransportMap':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['TransportMap'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['TransportMap'] = OpenLayers.i18n('openStreetMapTransportMap');
                                }
                                this.baseMaps['TransportMap'] = map;
                                break;
                            case 'CycleMap':
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['CycleMap'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['CycleMap'] =  OpenLayers.i18n('openStreetMapCycleMap');
                                }
                                this.baseMaps['CycleMap'] = map;
                                break;
                            default: //Default to Mapnik tileset
                                if (map.mapTag.extension.Options[0].name) {
                                    this.options['Mapnik'] = map.mapTag.extension.Options[0].name[0];
                                }
                                else {
                                    this.options['Mapnik'] =  OpenLayers.i18n('openStreetMap');
                                }
                                this.baseMaps['Mapnik'] = map;
                                break;
                        }
                        // The first non-MapGuide basemap will be the default basemap
                        if (!this.defaultBasemap) {
                            this.defaultBasemap = map.mapTag.extension.Options[0].type[0];
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if (!this.defaultBasemap) {
            this.defaultBasemap = "None";
        }
    },

    setUiObject: function(uiObj) {
        Fusion.Widget.prototype.setUiObject.apply(this, [uiObj]);
        this.setDefaultBasemap();
    },
    
    checkZoomOffsets: function() {
        if (this.activeBasemap != null) {
            this.applyZoomOffset(this.activeBasemap);
        }
    },
    
    applyZoomOffset: function(baseMap) {
        var offset = 0;
        if (this.zoomOffsets[baseMap])
            offset = this.zoomOffsets[baseMap];
        for (var i = 0; i < this.getMap().aMaps.length; i++) {
            var layer = this.getMap().aMaps[i];
            if (layer.arch == "MapGuide" && layer.bUsesCommercialLayerScaleList) {
                layer.applyZoomOffset(offset);
            }
        }
    },

    setBasemap: function(baseMap) {
        if ("None" != baseMap && this.getMap().oMapOL.baseLayer.CLASS_NAME == "OpenLayers.Layer.MapGuide") {
            var visibility = this.baseMaps["None"].oLayerOL.visibility;
            this.getMap().oMapOL.setBaseLayer(this.baseMaps[baseMap].oLayerOL, false);
            this.applyZoomOffset(baseMap);
            // Keep the MapGuide layers visibility
            this.baseMaps["None"].oLayerOL.visibility = visibility;
            this.baseMaps["None"].oLayerOL.redraw();
        }
        else {
            this.getMap().oMapOL.setBaseLayer(this.baseMaps[baseMap].oLayerOL, false);
            this.applyZoomOffset(baseMap);
        }
        this.activeBasemap = baseMap;
    },

    setDefaultBasemap: function() {
        this.generateOptions();

        //clean current menu items
        this.uiObj.subDomObj.empty();

        //set up the root menu
        var buttonSet = new Jx.ButtonSet();
        for (var key in this.options) {
            if (this.options[key]) {
                var menuItem = new Jx.Menu.Item({
                    label: OpenLayers.i18n(this.options[key]),
                    toggle: true,
                    onDown: OpenLayers.Function.bind(this.setBasemap, this, key)
                });
                buttonSet.add(menuItem);
                this.uiObj.add(menuItem);
                this.menuItems[key] = menuItem;
            }
        }
        this.menuItems[this.defaultBasemap].setActive(true);
        this.setBasemap(this.defaultBasemap);
    }
});
