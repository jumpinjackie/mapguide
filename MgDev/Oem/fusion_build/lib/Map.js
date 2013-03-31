/**
 * Fusion.Widget.Map
 *
 * $Id: Map.js 2585 2012-09-07 14:01:25Z jng $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * Constant: Fusion.Event.MAP_EXTENTS_CHANGED
 */
Fusion.Event.MAP_EXTENTS_CHANGED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_BUSY_CHANGED
 */
Fusion.Event.MAP_BUSY_CHANGED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_GENERIC_EVENT
 */
Fusion.Event.MAP_GENERIC_EVENT = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_RESIZED
 */
Fusion.Event.MAP_RESIZED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_SELECTION_ON
 */
Fusion.Event.MAP_SELECTION_ON = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_SELECTION_OFF
 */
Fusion.Event.MAP_SELECTION_OFF = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_ACTIVE_LAYER_CHANGED
 */
Fusion.Event.MAP_ACTIVE_LAYER_CHANGED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_LOADED
 */
Fusion.Event.MAP_LOADED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_LOADING
 */
Fusion.Event.MAP_LOADING = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_RELOADED
 */
Fusion.Event.MAP_RELOADED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_SESSION_CREATED
 */
Fusion.Event.MAP_SESSION_CREATED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_MAPTIP_REQ_FINISHED
 */
Fusion.Event.MAP_MAPTIP_REQ_FINISHED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.WMS_LAYER_ADDED
 */
Fusion.Event.WMS_LAYER_ADDED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_SCALE_RANGE_LOADED
 */
Fusion.Event.MAP_SCALE_RANGE_LOADED = Fusion.Event.lastEventId++;
/**
 * Constant: Fusion.Event.MAP_MAP_GROUP_LOADED
 */
Fusion.Event.MAP_MAP_GROUP_LOADED = Fusion.Event.lastEventId++;

/**
 * Constant: Fusion.Constant.LAYER_POINT_TYPE
 */
Fusion.Constant.LAYER_POINT_TYPE = 0;
/**
 * Constant: Fusion.Constant.LAYER_POINT_TYPE
 */
Fusion.Constant.LAYER_POINT_TYPE = 1;
/**
 * Constant: Fusion.Constant.LAYER_POINT_TYPE
 */
Fusion.Constant.LAYER_POINT_TYPE = 2;
/**
 * Constant: Fusion.Constant.LAYER_POINT_TYPE
 */
Fusion.Constant.LAYER_POINT_TYPE = 3;
/**
 * Constant: Fusion.Constant.LAYER_POINT_TYPE
 */
Fusion.Constant.LAYER_POINT_TYPE = 4;
/**
 * Constant: Fusion.Constant.LAYER_DWF_TYPE
 */
Fusion.Constant.LAYER_DWF_TYPE = 5;

/****************************************************************************
 * Class: Fusion.Widget.Map
 *
 * generic class for map widgets. Provides common utility classes.
 * This class provides a wrapper around the {<OpenLayers.Map>} object.
 * 
 * Inherits from:
 *  - <Fusion.Lib.EventMgr>
 * **********************************************************************/
Fusion.Widget.Map = OpenLayers.Class(Fusion.Lib.EventMgr, {
    
    /** The DOM object that holds the map */
    _oDomObj: null,
    
    /** The id of DOM object */
    _sDomObj: '',
    
    /** The internal name used for the map, usually assigned by the server */
    _sMapname: '',
    
    /** conversion factor to convert from map units to meters */
    _fMetersperunit: -1,
    
    /** scale denominator value */
    _fScale: -1,
    
    /** Dots per inch setting for the server */
    _nDpi: 96,
    
    /** The current bounds of the map, this is updated on every map draw */
    _oCurrentExtents: null,
    
    /** The maximum extent of the map */
    maxExtent: new OpenLayers.Bounds(),
    
    /** counter of worker threads for the ActivityIndicator widget  */
    _nWorkers: 0,
    
    /** The context menu object for right click */
    oContextMenu: null,
    
    /** flag to disable the map context menu, set this in the Map extension  */
    bSupressContextMenu: false,
    
    /** an array to hold all the individual map layer objects */
    aMaps: null,
    
    /** flag to indicate if all the map layers are loaded */
    mapsLoaded: false,
    
    /** the root of the layer structure for the Legend */
    layerRoot: null,
    
    /** flag to indicate if the map is tiled or not, set in the Map extension */
    singleTile: true,
    
    /** flag to indicate if fractional zoom is supported, ie. allow any scale
    *   value rather than a fixed set of scales
    */
    fractionalZoom: true,
    
    /** The DOM object that holds the map */
    maxScale: null, //set this to a large number in AppDef to zoom out beyond maxExtent, e.g. 1 billion

    /**
     * Property: message
     * 
     * The {<Fusion.MapMessage>} notification bar
     */
    message: null,

    /**
     * construct a new view Fusion.Widget.Map class.
     */
    initialize: function(widgetTag, mapGroup, widgetSet) {
        this.widgetTag = widgetTag;
        var name = widgetTag.name;
        this.aMaps = [];
        this.buttonSet = new Jx.ButtonSet();
        this.widgetSet = widgetSet;
        this._sDomObj = name;
        this._oDomObj = $(this._sDomObj);
        this.layerRoot = new Fusion.Layers.Group({
            legendLabel: 'mapRoot',
            uniqueId: 'mapRoot',
            groupName: 'mapRoot',
            visible: true
        });

        var jxl = this._oDomObj.retrieve('jxLayout');
        if (jxl) {
            jxl.addEvent('sizeChange', OpenLayers.Function.bind(this.sizeChanged, this));
        }

        if (widgetTag.extension.FractionalZoom) {
          this.fractionalZoom = widgetTag.extension.FractionalZoom[0]=='false'?false:true;
        }

        //if there is a scales array specified, use that set and turn off fraczoom
        var scalesArray = null;
        if (widgetTag.extension.Scales) {
          scalesArray = widgetTag.extension.Scales[0].split(',');
          this.fractionalZoom = false;
        }

        //Set a MaxScale in MapWIdget extension to allow zoom out to that scale
        //otherwise, MaxScale is calculated automatically
        if (widgetTag.extension.MaxScale) {
          this.maxScale = parseInt(widgetTag.extension.MaxExtent[0]);
        }

        //set the maxExtent
        var maxExtent = null;
        if (widgetTag.extension.MaxExtent) {
          maxExtent = OpenLayers.Bounds.fromString(widgetTag.extension.MaxExtent[0]);
        }

        //create the OpenLayers Map object
        OpenLayers.DOTS_PER_INCH = this._nDpi;
        if (!this.oMapOL) {
            var options = {
                controls: [],
                fallThrough: true,
                scales: scalesArray,
                fractionalZoom: this.fractionalZoom
            };
            if (widgetTag.extension.ConstrainMapExtent) {
              this.bRestrictExtent = widgetTag.extension.ConstrainMapExtent[0]=='true'?true:false;
            }
            if (maxExtent) {
              options.maxExtent = maxExtent;
              this.maxExtent = maxExtent;
            }
            this.oMapOL = new OpenLayers.Map(this._sDomObj, options );
        }

        //Fusion specific modifications to the OL map window style
        this.oMapOL.viewPortDiv.style.position = 'absolute';  //not the top level container so set it to absolute
        this.oMapOL.viewPortDiv.style.zIndex = 0;   //must explicitly set the z-index for FF3

        this.handlers = [];

        //add in the handler for mouse wheel actions
        var useMouseWheel = true;
        if (widgetTag.extension.DisableMouseWheel &&
            widgetTag.extension.DisableMouseWheel[0] == 'true') {
            useMouseWheel = false;
        }
        if (useMouseWheel) {
          this.wheelHandler = new OpenLayers.Handler.MouseWheel(this,
                                            {"up"  : this.wheelUp,
                                             "down": this.wheelDown} );
          this.wheelHandler.map = this.oMapOL;
          this.wheelHandler.activate();
          this.handlers.push(this.wheelHandler);
        }

        $(name).widget = this;

        this.registerEventID(Fusion.Event.MAP_EXTENTS_CHANGED);
        this.registerEventID(Fusion.Event.MAP_BUSY_CHANGED);
        this.registerEventID(Fusion.Event.MAP_GENERIC_EVENT);
        this.registerEventID(Fusion.Event.MAP_RESIZED);
        this.registerEventID(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED);
        this.registerEventID(Fusion.Event.MAP_LOADED);
        this.registerEventID(Fusion.Event.MAP_LOADING);
        this.registerEventID(Fusion.Event.MAP_RELOADED);
        this.registerEventID(Fusion.Event.MAP_SELECTION_ON);
        this.registerEventID(Fusion.Event.MAP_SELECTION_OFF);
        this.registerEventID(Fusion.Event.MAP_MAPTIP_REQ_FINISHED);
        this.registerEventID(Fusion.Event.WMS_LAYER_ADDED);
        this.registerEventID(Fusion.Event.MAP_SCALE_RANGE_LOADED);
        this.registerEventID(Fusion.Event.MAP_MAP_GROUP_LOADED);

        this.registerForEvent(Fusion.Event.MAP_LOADED, OpenLayers.Function.bind(this.mapLoaded,this));
        this.registerForEvent(Fusion.Event.MAP_RELOADED, OpenLayers.Function.bind(this.mapLoaded,this));

        //register for OL map extent change events
        this.oMapOL.events.register('moveend', this, this.mapExtentsChanged);

        this._oDomObj.onselectstart = function() {return false;};
        this._oDomObj.oncontextmenu = function() {return false;};
        OpenLayers.Event.observe(this._oDomObj, 'contextmenu',
                            OpenLayers.Function.bind(this.onContextMenu, this));

        this.aSelectionCallbacks = [];
        this.bFetchingSelection = false;

        //create the 'Map' layer widgets defined in the MapGroup
        this.loadMapGroup(mapGroup);
    },

    /**
     * Function: mapLoaded
     *
     * Callback for when all maps have been loaded
     *
     * Parameter: none.
     *
     * Return: none
     */
    mapLoaded: function() {
      this.setViewOptions(this.getUnits());
      this.loadScaleRanges(OpenLayers.Function.bind(this.scaleRangesLoaded, this));
    },

    /**
     * Function: setMenu
     *
     * Attaches a context menu as defined in the AppDef to the map
     *
     * Parameter: none.
     *
     * Return: none
     */
    setMenu: function() {
        if (this.widgetTag.extension.MenuContainer) {
            var contextMenu = new Jx.Menu.Context();
            var container = this.widgetSet.getContainerByName(this.widgetTag.extension.MenuContainer[0]);
            if (container) {
                container.createWidgets(this.widgetSet, contextMenu);
                this.setContextMenu(contextMenu);
            }
        }
    },

    /**
     * Function: loadMapGroup
     *
     * Controls the creation of Layers based on the MapGroup object passed in
     *
     * Parameter: {Object} mapGroup - a representation of the MapGroup object
     * {Boolean} loadOverlays - if true, this will leave the first layer (usually
     * a commercial baselayer) on the map and just replace all the overlays
     *
     * Return: none
     */
    loadMapGroup: function(mapGroup, loadOverlays) {
        //clear any existing selection
        this.mapsLoaded = false;
        this.clearSelection();
        //this._addWorker();

        this.mapGroup = mapGroup;
        var startIndex = loadOverlays?1:0;
        for (var i = this.aMaps.length-1; i >= startIndex; i--) {
          if (this.aMaps[i].oLayerOL) {
            this.aMaps[i].oLayerOL.destroy();
            this.aMaps.splice(i,1);
          }
        }
 
        this._fMetersperunit = -1;
        this.oMapOL.center = null;
        this.maxExtent = new OpenLayers.Bounds();
        var storedBase;
        if (loadOverlays) {
          storedBase = this.layerRoot.layers[0];
        }
        this.layerRoot.clear();
        if (loadOverlays) {
          this.layerRoot.addLayer(storedBase);
        }

        for (var i=0; i<mapGroup.maps.length; ++i) {
          var mapTag = mapGroup.maps[i];

          if (Fusion.Layers[mapTag.type]) {
              this.aMaps.push(new Fusion.Layers[mapTag.type](this, mapTag, true));
              this.layerRoot.addGroup(this.aMaps[this.aMaps.length-1].layerRoot);
          } else {
              this.aMaps.push(new Fusion.Layers.Generic(this, mapTag, true));
              this.layerRoot.addLayer(this.aMaps[this.aMaps.length-1].layerRoot);
          }
          this.aMaps[this.aMaps.length-1].registerForEvent(Fusion.Event.LAYER_LOADED, OpenLayers.Function.bind(this.layerLoaded,this,mapTag));
        }
        this.triggerEvent(Fusion.Event.MAP_MAP_GROUP_LOADED);
    },

    /**
     * Function: layerLoaded
     *
     * Called after each layer is loaded in the OL viewport.  Nothing happens 
     * here until the last layer is laoded, then it sets the OL baselayer and
     * zooms to the initial extent.
     *
     * Parameter: none
     *
     * Return: none
     */
    layerLoaded: function(mapTag) {
      for (var i=0; i<this.aMaps.length; ++i) {
        if (!this.aMaps[i].isMapLoaded()) {
          return;
        }
      }
      this.mapsLoaded = true;
      if (this.aMaps.length == 1) {
        this.oMapOL.setBaseLayer(this.aMaps[0].oLayerOL);
      }
      
      var initialExtent = this.setInitialExtents();
      this.setExtents(initialExtent);

      this.triggerEvent(Fusion.Event.MAP_LOADED, mapTag);
    },

    /**
     * Function: wheelChange
     *
     * handles mouse wheel events by accummulating the events and setting a timer
     * to do the actual zoom in/out
     * 
     * Parameters:
     * evt - {Event} the mouse wheel event object
     * deltaZ - {Integer} the number of ticks wheeled
     */
    wheelChange: function(evt, deltaZ) {
        if (this.wheelTimer) {
            clearTimeout(this.wheelTimer);
        } else {
            this.cumulativeDelta = 0;
        }
        this.cumulativeDelta += (deltaZ < 0)?-1:1;
        
        this.wheelTimer = setTimeout(OpenLayers.Function.bind(function(){this.doWheelChange(evt, deltaZ);}, this), 200);
    },

    /**
     * Function: doWheelChange
     *
     * Carries out the actual map zoom based on the wheel movement
     *
     * Parameters:
     * evt - {Event} the mouse wheel event object
     * deltaZ - {Integer} the number of ticks wheeled
     *
     * Return: none
     */
    doWheelChange: function(evt, deltaZ) {
        this.wheelTimer = null;
        if (this.cumulativeDelta == 0) {
            return;
        }
        var size    = this.oMapOL.getSize();
        var deltaX  = size.w/2 - evt.xy.x;
        var deltaY  = evt.xy.y - size.h/2;

        var deltaRes = this.cumulativeDelta > 0 ? 1/(this.cumulativeDelta+1) : Math.abs(this.cumulativeDelta)+1;
        var newRes  = this.oMapOL.baseLayer.getResolution() * deltaRes;
        var zoomPoint = this.oMapOL.getLonLatFromPixel(evt.xy);
        var newCenter = new OpenLayers.LonLat(
                            zoomPoint.lon + deltaX * newRes,
                            zoomPoint.lat + deltaY * newRes );
        var newBounds = new OpenLayers.Bounds(
                            newCenter.lon - size.w*newRes/2,
                            newCenter.lat - size.h*newRes/2,
                            newCenter.lon + size.w*newRes/2,
                            newCenter.lat + size.h*newRes/2);
        this.setExtents(newBounds);
    },

    /**
     * Method: wheelUp
     * User spun scroll wheel up
     *
     * Parameters:
     * evt - {Event}
     */
    wheelUp: function(evt) {
        this.wheelChange(evt, 1);
    },

    /**
     * Method: wheelDown
     * User spun scroll wheel down
     *
     * Parameters:
     * evt - {Event}
     */
    wheelDown: function(evt) {
        this.wheelChange(evt, -1);
    },

    /**
     * Function: getDomObj
     *
     * returns the dom element for this widget
     *
     * Parameters: none
     *
     * Return: none
     */
    getDomObj: function() {
        return this._oDomObj;
    },


    /**
     * Function: getDomId
     *
     * returns the ID of dom element for this widget
     *
     * Parameters: none
     *
     * Return: {String} the id of the DOM element
     */
    getDomId: function() {
        return this._sDomObj;
    },

    /**
     * Function: setMapOptions
     *
     * sets options on the OpenLayers map object, a wrapper for the OL function
     *
     * Parameters: {Object} options - has of option key/value pairs
     *
     * Return: none
     */
    setMapOptions: function(options) {
        this.oMapOL.setOptions(options);
    },

    /**
     * Function: addMap
     *
     * adds a map/Layer to the map and registers some event handlers on the map
     *
     * Parameters: {Object} map - the map object to tbe added (really a layer object)
     *
     * Return: none
     */
    addMap: function(map) {

        if (map.mapTag.layerOptions.maxExtent) {
          this.maxExtent.extend(map.mapTag.layerOptions.maxExtent);
          this.oMapOL.setOptions({maxExtent: this.maxExtent});
        }

        //if bRestrictExtent is null, use the default OL behaviour with somewhat restricted map navigation
        //if bRestrictExtent is set to true, map navigation is limited to the map extent
        //if bRestrictExtent is set to false, map navigation is not restricted at all
        if (this.bRestrictExtent != null) {
          if (this.bRestrictExtent) {
            this.oMapOL.restrictedExtent = map.mapTag.layerOptions.maxExtent;
          } else {
            this.oMapOL.restrictedExtent = false;
          }
        }
        this.oMapOL.addLayer(map.oLayerOL);
        if (map.oLayerOL.isBaseLayer) {
          this.projection = map.oLayerOL.projection;
          this.units = map.oLayerOL.units;
          this.oMapOL.setOptions({
                  units: this.units,
                  projection: this.projection
          });
        }
        
        map.registerForEvent(Fusion.Event.MAP_SELECTION_OFF,
                OpenLayers.Function.bind(this.selectionHandler, this));
        map.registerForEvent(Fusion.Event.MAP_SELECTION_ON,
                OpenLayers.Function.bind(this.selectionHandler, this));
    },

    /**
     * Function: getAllMaps
     *
     * returns the array of map objects
     *
     * Parameters: none
     *
     * Return: {Array} the array of map Objects
     */
    getAllMaps: function() {
        return this.aMaps;
    },

    /**
     * Function: reloadMap
     *
     * Called to reload all the map objects.
     * this uses setTimeout so this method can be called from an IFRAME
     *
     * Parameters: none
     *
     * Return: none
     */
    reloadMap: function() {
      for (var i=0; i<this.aMaps.length; ++i) {
        var map = this.aMaps[i];
        window.setTimeout(OpenLayers.Function.bind(map.reloadMap, map),1);
      }
    },

    /**
     * Function: loadScaleRanges
     *
     * Loads the ScaleRange objects separate from the LoadMap sequence since
     * this also generates sprites and is a relatively expensive operation.
     *
     * Parameters: {Function} userFunc - a user defined function to be called as a
     *                                  callback
     *
     * Return: none
     */
    loadScaleRanges: function() {
      for (var i=0; i<this.aMaps.length; ++i) {
        var map = this.aMaps[i];
        map.loadScaleRanges();
      }
    },

    /**
     * Function: query
     *
     * dispatch query requests to the maps
     *
     * Parameters: {object} options - query options to be passed to each map
     */
    query: function(options) {
        this.lastQueryOptions = options;
        for (var i=0; i<this.aMaps.length; i++ ) {
            if (this.aMaps[i].query(options)) {
            }
        }
    },

    /**
     * Function: selectionHandler
     *
     * handle selection events from maps and republish for
     * widgets as appropriate
     *
     * Parameters: none
     *
     * Return: none
     */
    selectionHandler: function() {
        if (this.hasSelection()) {
            this.triggerEvent(Fusion.Event.MAP_SELECTION_ON);
        } else {
            this.triggerEvent(Fusion.Event.MAP_SELECTION_OFF);
        }
    },

    /**
     * Function: hasSelection
     *
     * cehck to see if any of the maps have an active selection
     *
     * Returns: {Boolean} - true if any map has a selection
     */
     hasSelection: function() {
         for (var i=0; i<this.aMaps.length; i++ ) {
             if (this.aMaps[i].hasSelection()) {
                 return true;
             }
         }
         return false;
     },

     /**
      * Function: clearSelection
      *
      * clear the selection on all maps
      */
     clearSelection: function() {
         this.oSelection = null;
         this.lastQueryOptions = null;
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].clearSelection();
         }
     },

     /**
      * Function: getSelection
      *
      * initiates a call to get the current selection asynchronously in case we
      * need to retrieve the details from the server
      */
     getSelection: function(callback, layers, startcount) {
       //console.log('map.js : getselection ' + layers);

       var layers = (arguments[1]) ? arguments[1] : '';
       var startcount = (arguments[2]) ? arguments[2] : '';
         this.aSelectionCallbacks.push(callback);
         if (this.bFetchingSelection) {
             return;
         }
         this.bFetchingSelection = true;
         this.oSelection = {};
         this.nSelectionMaps = 0;
         for (var i=0; i<this.aMaps.length; i++ ) {
           if (!this.aMaps[i].supports.query) 
         // if(this.aMaps[i].layerType == "Google" || this.aMaps[i].layerType == "Yahoo" ||this.aMaps[i].layerType == "VirtualEarth" )
               continue;
             this.nSelectionMaps++;
             this.aMaps[i].getSelection(
                    OpenLayers.Function.bind(this.accumulateSelection, this, this.aMaps[i]),
                    layers, startcount);
         }
     },

     /**
      * Function: setSelection
      *
      * sets a Selection XML back to the server for each of the maps
      *
      * Parameters: 
      * {String} selText - the selection string 
      * {Boolean} zoomTo - if set, will zoom to the selection on completion
      */
      setSelection: function(selText, zoomTo) {
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].setSelection(selText, zoomTo);
         }
      },

     /**
      * Function: accumulateSelection
      *
      * accumulate the selection results from each map and when all have
      * reported in, pass the results to the callback function
      *
      * Parameters:
      * {Object} map - the map object that has the selection
      * {Object} oSelection - the Fusion selection object
      */
     accumulateSelection: function(map, oSelection) {
         this.oSelection[map._sMapname] = oSelection;

         if (!--this.nSelectionMaps) {
             this.bFetchingSelection = false;
             for (var i=0; i<this.aSelectionCallbacks.length; i++) {
                 this.aSelectionCallbacks[i](this.oSelection);
             }
             this.aSelectionCallbacks = [];
         }
     },

     /**
      * Function: setActiveLayer
      *
      * sets the active layer for selection/manipulation
      *
      * Parameters:
      * {Object} oLayer - the layer to be set as active
      */
      setActiveLayer: function( oLayer ) {
          this.oActiveLayer = oLayer;
          if (oLayer) {
              this.oActiveMap = oLayer.map;
          }
          this.triggerEvent(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED, oLayer);
      },

     /**
      * Function: getActiveLayer
      *
      * returns the active layer for selection/manipulation
      *
      * Returns:
      * {Object} the active layer
      */
    getActiveLayer: function() {
        return this.oActiveLayer;
    },

    /**
     * Function: _addWorker
     * 
     * indicate that a new asynchronous process has started and make sure the
     * visual indicator is visible for the user.  This is intended to be used
     * internally by gMap but could be used by external tools if appropriate.
     */
    _addWorker: function() {
        this._nWorkers += 1;
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
        this._oDomObj.style.cursor = 'wait';
    },

    /**
     * Function: _removeWorker
     * 
     * indicate that an asynchronous process has completed and hide the
     * visual indicator if no remaining processes are active.  This is
     * intended to be used internally by gMap but could be used by
     * external tools if appropriate.  Only call this function if
     * addWorker was previously called
     */
    _removeWorker: function() {
        if (this._nWorkers > 0) {
            this._nWorkers -= 1;
        }
        this.setCursor(this.cursor);
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
    },

    /**
     * Function: mapExtentsChanged
     *
     * OpenLayers event handler for whenever the map changes either zoom level
     * of the center has changed.  This in turn triggers the Fusion event. 
     *
     * Parameters: none
     *
     * Return: none
     */
    mapExtentsChanged: function() {
        this._oCurrentExtents = this.oMapOL.getExtent();
        this.triggerEvent(Fusion.Event.MAP_EXTENTS_CHANGED);
    },

    /**
     * Function: isBusy
     *
     * check to see if there are any outstanding requests to the server
     *
     * Parameters: none
     *
     * Return: {Boolean} true if there are active requests
     */
    isBusy: function() {
        return this._nWorkers > 0;
    },

    /**
     * Function: sizeChanged
     *
     * callback for when the browser window size changes, simply calls resize 
     * on the map viewport layout object
     *
     * Parameters: none
     *
     * Return: none
     */
    sizeChanged: function() {
        this.resize();
    },

    /**
     * Function: resize
     *
     * handles the resizing of the maps and triggers a Fusion event
     *
     * Parameters: none
     *
     * Return: none
     */
    resize: function() {
      if (!this.mapsLoaded) {
        return;
      }
      //console.log('Fusion.Widget.Map.resize');
        this.oMapOL.updateSize();
        var d = $(this.getDomObj()).getContentBoxSize();
        if (this._oCurrentExtents) {
          this.setExtents(this._oCurrentExtents);
        }
        this.triggerEvent(Fusion.Event.MAP_RESIZED, this);
    },

    /**
     * Function: redraw
     *
     * redraws the map using current extents and zoom level.  A dummy parameter
     * is added to the map request to ensure the map request is not cached.
     *
     * Parameters: none
     *
     * Return: none
     */
    redraw: function() {
      for (var i=0; i<this.aMaps.length; i++ ) {
        if(this.aMaps[i].oLayerOL.params){
            this.aMaps[i].oLayerOL.params.ts = (new Date()).getTime();
            //mergeNewParams calls redraw on the layer, which will get called below anyway
            //this.aMaps[i].oLayerOL.mergeNewParams({ts : (new Date()).getTime()});
        }
      }
      this.oMapOL.setCenter(this.oMapOL.getCenter(), this.oMapOL.getZoom(), false, true);
    },

    /**
     * Function: setBackgroundColor
     *
     * sets the background color for the map
     *
     * Parameters: {String} color - the CSS color value to set as background
     *
     * Return: none
     */
    setBackgroundColor: function(color) {
        this._oDomObj.style.backgroundColor = color;
    },

    /**
     * Function: setExtents
     *
     * handle selection events from maps and republish for
     * widgets as appropriate
     *
     * Parameters: {Object} oExtents - an OpenLayers.Bounds object or an array 
     *                            of 4 values that will be converted to Bounds
     *
     * Return: none
     */
    setExtents: function(oExtents) {
        if (!oExtents) {
            Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING,
                                OpenLayers.i18n('nullExtents')));
        }
        if (oExtents instanceof Array && oExtents.length == 4) {
            oExtents = new OpenLayers.Bounds(oExtents[0], oExtents[1], oExtents[2], oExtents[3]);
        }

        //update the timestamp param to prevent caching
        for (var i=0; i<this.aMaps.length; i++ ) {
          if (this.aMaps[i].oLayerOL.params && this.aMaps[i].noCache) {
            this.aMaps[i].oLayerOL.params.ts = (new Date()).getTime();
          }
      }
        
        // when the parameter "oExtents" is a point which should be a rectangle, it will zoom to 1:1
        // this is often because the selected feature is a point feature, like tree
        // in this situation we can pan the point to the center, no need to zoom to 1:1
        if(oExtents.bottom == oExtents.top && oExtents.left == oExtents.right)
        {
            this.oMapOL.panTo(new OpenLayers.LonLat(oExtents.left, oExtents.top));
        }
        else
        {
            this.oMapOL.zoomToExtent(oExtents,true);
        }
        
        this._oCurrentExtents = this.oMapOL.getExtent();
    },

    /**
     * Function: setInitialExtents
     * 
     * determine the initialExtents of the map from (in order of precedence):
     * 1. a URL query parameter called 'extent'
     * 2. an <InitialView> specified in the MapGroup in AppDef
     * 3. the maxExtent as specified by the LoadMap call (default)
     *
     * Returns: {Object} an OpenLayers.Bounds object which is the initial extent
     */
    setInitialExtents: function() {
      var initialExtents;
      var bbox = Fusion.getQueryParam("extent");   //set as min x,y, max x,y
      if (bbox) {
        initialExtents = new OpenLayers.Bounds.fromArray(bbox.split(","));
      } else if (this.mapGroup.initialView) {
          var iv = this.mapGroup.getInitialView();
          if (iv.x != undefined) {
              initialExtents = this.getExtentFromPoint(iv.x, iv.y, iv.scale);
          } else if (iv.minX != undefined) {
              initialExtents = new OpenLayers.Bounds(iv.minX, iv.minY, iv.maxX, iv.maxY);
          }
          var mapProj = this.oMapOL.baseLayer.projection;
          if (iv.projection && !iv.projection.equals(mapProj)) {
            initialExtents.transform(iv.projection, mapProj);
          }
          if (!initialExtents.intersectsBounds(this.maxExtent)) {
            Fusion.reportError("AppDef initial view is outside map maxExtent, resetting initialView to maxExtent");
            initialExtents = this.maxExtent;
          }
      } else {
        initialExtents = this.getMapGroupExtent();
        if (!initialExtents) {
            initialExtents = this.getMapGroupExtent(true);
        }
      }
      if (!this.initialExtents) this.initialExtents = initialExtents;
      return initialExtents;
    },

    /**
     * Function: getMapGroupExtent
     *
     * gets the intitial extents from all maps with or without including the
     * extent of any Generic/commercial layers
     * 
     * Return: {Object} an OpenLayers.Bounds object which is the initial extent
     */
    getMapGroupExtent: function(includeGenericLayers) {
      var initialExtent;
      for (var i=0; i<this.aMaps.length; ++i) {
        if (!includeGenericLayers && this.aMaps[i].arch == 'Generic') {
            continue;
        }
        initialExtent = this.aMaps[i].getMaxExtent();
      }
      return initialExtent;
    },

    /**
     * Function: fullExtents
     * 
     * sets the extent of the map to the max as returned by loadMap
     */
    fullExtents: function() {
      var extents = this.maxExtent;
      this.setExtents(extents);
    },

    /**
     * Function: isMapLoaded
     * 
     * returns if the map has finished loading (based on setting the 
     * _oCurrentExtent value)
     *
     * Returns {Boolean} true if the map is loaded and extent has been set
     */
    isMapLoaded: function() {
        return (this._oCurrentExtents) ? true : false;
    },

    /**
     * Function: zoom
     * 
     * sets the map zoom and extent.
     *
     * Parameters:
     *   fX {Float} - new x coordinate value in map units
     *   fY {Float} - new y coordinate value in map units
     *   nFactor {Float} - zoom factor; positive values zoom in, negative out
     *                  - if set to 0 or 1, the map is just recentered
     *                  - if the map has fractional zoom enabled, the map resolution
     *                  will be modified by this factor
     *                  - with fixed scales, zoom up or down one level, depending on the sign
     *
     * Returns: none
     */
    zoom: function(fX, fY, nFactor) {
        //do this differntly with OL code??
        if (nFactor == 1 || nFactor == 0) {
            /*recenter*/
            this.oMapOL.panTo(new OpenLayers.LonLat(fX, fY));
        } else {
            var extent = this.oMapOL.getExtent();
            if (this.fractionalZoom) {
                var fDeltaX = extent.right - extent.left;
                var fDeltaY = extent.top - extent.bottom;
                var fMinX,fMaxX,fMinY,fMaxY;
                if (nFactor > 0) {
                    /*zoomin*/
                    fMinX = fX - (fDeltaX/2 / nFactor);
                    fMaxX = fX + (fDeltaX/2 / nFactor);
                    fMinY = fY - (fDeltaY/2 / nFactor);
                    fMaxY = fY + (fDeltaY/2 / nFactor);
                } else if (nFactor < 0) {
                    /*zoomout*/
                    fMinX = fX - ((fDeltaX/2) * Math.abs(nFactor));
                    fMaxX = fX + ((fDeltaX/2) * Math.abs(nFactor));
                    fMinY = fY - ((fDeltaY/2) * Math.abs(nFactor));
                    fMaxY = fY + ((fDeltaY/2) * Math.abs(nFactor));
                }
                this.setExtents(new OpenLayers.Bounds(fMinX, fMinY, fMaxX, fMaxY));
            } else {
                var currentZoomLevel = this.oMapOL.getZoom();
                if (nFactor > 1) {
                    this.oMapOL.zoomTo(currentZoomLevel+1);
                } else if (nFactor < 1) {
                    this.oMapOL.zoomTo(currentZoomLevel-1);
                }
            }
        }
    },

    /**
     * Function: zoomToScale
     * 
     * Zooms to the specified scale
     *
     * Parameters:
     *   fScale - {Float} The scale to zoom to
     *
     * Returns: none
     */
    zoomToScale: function(fScale) {
        var center = this.getCurrentCenter();
        var extent = this.getExtentFromPoint(center.x, center.y, fScale);
        this.setExtents(extent);
    },

    /**
     * Performs a rectangular query
     *
     * Parameters:
     *   fMinX {Float} - new minimum x coordinate value in map units
     *   fMinY {Float} - new minimum y coordinate value in map units
     *   fMaxX {Float} - new maximum x coordinate value in map units
     *   fMaxY {Float} - new maximum y coordinate value in map units
     *
     * Returns: none
     */
    queryRect: function(fMinX, fMinY, fMaxX, fMaxY) { },

    /**
     * Performs a point query
     *
     * Parameters:
     *   fX {Float} - new x coordinate value in map units
     *   fY {Float} - new y coordinate value in map units
     *
     * Returns: none
     */
    queryPoint: function(fX, fY) { },

    /**
     * Method: pixToGeo
     *
     * convert pixel coordinates into geographic coordinates.
     *
     * Parameters:
     * pX - {Integer} the x coordinate in pixel units
     * pY - {Integer} the y coordinate in pixel units
     *
     * Return:
     * an object with geographic coordinates in x and y properties of the object.
     */
    pixToGeo: function( pX, pY ) {
        var lonLat = this.oMapOL.getLonLatFromPixel( new OpenLayers.Pixel(pX,pY) );
        if (lonLat != null) {
          return {x:lonLat.lon, y:lonLat.lat};
        }
        return null;
    },

    /**
     * Method: geoToPix
     *
     * convert geographic coordinates into pixel coordinates.
     *
     * Parameters:
     * gX - {Integer} the x coordinate in geographic units
     * gY - {Integer} the y coordinate in geographic units
     *
     * Return:
     * an object with pixel coordinates in x and y properties of the object.
     */
    geoToPix: function( gX, gY ) {
        if (!(this._oCurrentExtents)) {
            return null;
        }
        var px = this.oMapOL.getPixelFromLonLat(new OpenLayers.LonLat(gX,gY));  //use getViewPortPxFromLonLat instead?
        return {x:Math.floor(px.x), y:Math.floor(px.y)};
    },

    /**
     * Method: pixToGeoMeasure
     *
     * convert pixel into geographic : used to measure.
     *
     * Parameters:
     * nPixels - {Integer} measures in pixel
     *
     * @return geographic measure
     */
    pixToGeoMeasure: function(nPixels) {
        var resolution = this.oMapOL.getResolution();
        return (nPixels*resolution);
    },

    /**
     * Method: setProjection
     *
     * initializes the OpenLayers projection object on the Map object
     *
     * Parameters:
     * projCode - {String} projection code
     * units - the units
     */
    setProjection: function(projCode, units) {
        this.projection = projCode;
        this.oMapOL.projection = projCode;
        this.oMapOL.units = units;
    },

    /**
     * Method: setMetersPerUnit
     *
     * initializes the meters per unit values when a new map is loaded.  Some systems make different
     * assumptions for the conversion of degrees to meters so this makes sure both Fusion and
     * OpenLayers are using the same value.
     *
     * Parameters:
     * metersPerUnit - {Float} the value returned by LoadMap.php for meters per unit
     */
    setMetersPerUnit: function(metersPerUnit) {
        if (this._fMetersperunit < 0) {
            Fusion.initUnits(metersPerUnit);
            this._fMetersperunit = metersPerUnit;
        } else {
            if (metersPerUnit != this._fMetersperunit) {
                Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING,
                                    'meters per unit value already set'));
            }
        }
    },

    /**
     * Method: getMetersPerUnit
     *
     * returns the meters per unit value
     *
     * Return:
     * metersPerUnit the value as set when the map initialized
     */
    getMetersPerUnit: function() {
        return this._fMetersperunit;
    },

    /**
     * Method: setViewOptions
     * 
     * initializes all widgets with the map units after the map has loaded
     *
     */
    setViewOptions: function(data) {
        this.setWidgetParam('Units', data);
    },

    /**
     * Method: setWidgetParam
     *
     * initializes all widgets with a parameter and value at runtime
     *
     */
    setWidgetParam: function(param, data) {
      for (var i=0; i<Fusion.applicationDefinition.widgetSets.length; ++i) {
        var widgetSet = Fusion.applicationDefinition.widgetSets[i];
        for (var j=0; j<widgetSet.widgetInstances.length; ++j) {
          var widget = widgetSet.widgetInstances[j];
          for (var k=0; k<widget.paramRegister.length; ++k) {
            if (widget.paramRegister[k] == param) {
              widget.setParameter(param, data);
            }
          }
        }
      }
    },

    /**
     * Method: geoToPixMeasure
     *
     * convert geographic into pixels.
     *
     * Parameters:
     * fGeo - {Float} distance in geographic units
     *
     * Return:
     * pixels
     */
    geoToPixMeasure: function(fGeo) {
        return parseInt(fGeo/this.oMapOL.getResolution());
    },

    /**
     * Function: getCurrentCenter
     *
     * returns the current center of the map view
     *
     * Return: 
     * {Object} an object with the following attributes
     * x - the x coordinate of the center
     * y - the y coordinate of the center
     */
    getCurrentCenter: function() {
        var c = this.getCurrentExtents().getCenterLonLat();
        return {x:c.lon, y:c.lat};
    },

    /**
     * Method: getCurrentExtents
     * 
     * returns the current extents
     */
    getCurrentExtents: function() {
        return this.oMapOL.getExtent();
    },

    /**
     * Function: getExtentFromPoint
     *
     * returns the Extent of the map given a center point and a scale (optional)
     *
     * Return: 
     * {<OpenLayers.Bounds>} the bounds for the map centered on a point
     */
    getExtentFromPoint: function(fX,fY,fScale) {
        if (!fScale) {
            fScale = this.getScale();
        }

        var res = OpenLayers.Util.getResolutionFromScale(fScale, this.oMapOL.baseLayer.units);
        var size = this.getSize();
        var w_deg = size.w * res;
        var h_deg = size.h * res;
        return new OpenLayers.Bounds(fX - w_deg / 2,
                                           fY - h_deg / 2,
                                           fX + w_deg / 2,
                                           fY + h_deg / 2);
    },

    /**
     * Function: getScale
     * 
     * Gets the current scale of the map
     * 
     * Returns:
     * The current map scale
     */
    getScale: function() {
        return this.oMapOL.getScale();
    },

    /**
     * Function: getResolution
     * 
     * Gets the current resolution of the map
     * 
     * Returns:
     * The current resolution of the map
     */
    getResolution: function() {
        return this.oMapOL.getResolution();
    },

    getUnits: function() {
        return this.oMapOL.baseLayer.units;
    },

    /**
     * Function: getSize
     * 
     * Gets the size of the map
     * 
     * Returns:
     * 
     *   return description
     */
    getSize: function() {
        return this.oMapOL.getSize();
    },

    getEventPosition: function(e) {
        return this.oMapOL.events.getMousePosition(e);
    },

    setCursor: function(cursor) {
        this.cursor = cursor;
        if (this.isBusy()) {
            return;
        }
        if (cursor && cursor.length && typeof cursor == 'object') {
            for (var i = 0; i < cursor.length; i++) {
                this._oDomObj.style.cursor = cursor[i];
                if (this._oDomObj.style.cursor == cursor[i]) {
                    break;
                }
            }
        } else if (typeof cursor == 'string') {
            this._oDomObj.style.cursor = cursor;
        } else {
            this._oDomObj.style.cursor = 'auto';
        }
    },
    /**
     *
     * Observe specified event on the event div of the map
     *
     * @param sEventName string event name (eg : mousemove')
     * @param fnCB function Call back function name
     *
     */
     observeEvent: function(sEventName, fnCB)
     {
         OpenLayers.Event.observe(this._oDomObj, sEventName, fnCB, false);
     },

     /**
     *
     * Stop observing specified event on the event div of the map
     *
     * @param sEventName string event name (eg : mousemove')
     * @param fnCB function Call back function name
     *
     */
     stopObserveEvent: function(sEventName, fnCB)
     {
         OpenLayers.Event.stopObserving(this._oDomObj, sEventName, fnCB, false);
     },

     /**
     *
     * call the Activate method on the widget
     * if widgets is set to be mutually exclusive,
     * all other widgets are deactivated
     *
     * @param nId integer widget id
     */
     activateWidget: function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.activateWidget ' + oWidget.getName());*/
         // if (oWidget.isMutEx()) {
         //     if (this.oActiveWidget) {
         //         this.deactivateWidget(this.oActiveWidget);
         //     }
         //     oWidget.activate();
         //     this.oActiveWidget = oWidget;
         // } else {
         //     oWidget.activate();
         // }
     },

     /**
     *
     * call the Activate method on the widget
     * if widgets is set to be mutually exclusive,
     * all other widgets are deactivated
     *
     * @param oWidget the widget to deactivate
     */
     deactivateWidget: function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.deactivateWidget ' + oWidget.getName());*/
         //oWidget.deactivate();
         //this.oActiveWidget = null;
     },

     /**
      */
     isLoaded: function() {
         return (this.oMapOL.getExtent() != null);
     },

     getLinkParams: function() {
        var queryParams = {
          extent: this.getCurrentExtents().toBBOX()
        };
        if (this.lastQueryOptions) {
          if (this.lastQueryOptions.filter) {
            queryParams.filter = this.lastQueryOptions.filter;
          }
          if (this.lastQueryOptions.geometry) {
            queryParams.spatialfilter = this.lastQueryOptions.geometry;
          }
          if (this.lastQueryOptions.queryHiddenLayers) {
            queryParams.queryHiddenLayers = this.lastQueryOptions.queryHiddenLayers;
          }
          if (this.lastQueryOptions.maxFeatures) {
            queryParams.maxfeatures = this.lastQueryOptions.maxFeatures;
          }
          if (this.lastQueryOptions.layers) {
            queryParams.selectlayer = this.lastQueryOptions.layers;
          }
          if (this.lastQueryOptions.selectionType) {
            queryParams.variant = this.lastQueryOptions.selectionType;
          }
        }
        for (var i=0; i< this.aMaps.length; ++i) {
          queryParams = OpenLayers.Util.extend(queryParams, this.aMaps[i].getLinkParams());
        }
        return OpenLayers.Util.getParameterString(queryParams);
     },
     
     supressContextMenu: function( bSupress ) {
         this.bSupressContextMenu = bSupress;
     },

     setContextMenu: function(menu) {                         
         //console.log('setcontextmenu');
         this.oContextMenu = menu;
     },

     onContextMenu: function(e) {
         //console.log('oncontextmenu');
         if (this.oContextMenu && !this.bSupressContextMenu && this.isLoaded()) {
             this.oContextMenu.show(new Event(e));
             this.contextMenuPosition = this.getEventPosition(e);
             OpenLayers.Event.stop(e);
         }
     },

     executeFromContextMenu: function(widget) {
         //console.log('executefromcontextmenu');
         widget.activate(this.contextMenuPosition.x, this.contextMenuPosition.y);
     }
});

/**
 * Class: Fusion.SelectionObject
 * 
 * Holds information about selected map features
 */
Fusion.SelectionObject = OpenLayers.Class({
    aLayers : null,

    initialize: function(o)
    {
        this.aLayers = [];
        this.nTotalElements =0;
        this.nLayers = 0;

        if ( o.layers &&  o.layers.length > 0)
        {
            this.fMinX =  o.extents.minx;
            this.fMinY =  o.extents.miny;
            this.fMaxX =  o.extents.maxx;
            this.fMaxY =  o.extents.maxy;

            this.nLayers =  o.layers.length;
            for (var i=0; i<o.layers.length; i++)
            {
                this.aLayers[i] = new Fusion.SelectionObject.Layer(o, o.layers[i]);
            }
        }
    },
    /**
     * Function: getNumElements
     * 
     * Gets the number of selected map features
     * 
     * Returns:
     * 
     *   the number of selected map features
     */
    getNumElements : function()
    {
        return this.nTotalElements;
    },

    /**
     * Function: getLowerLeftCoord
     * 
     * Gets the lower left coordinate of this selection's bounding box
     * 
     * Returns:
     * 
     *   the lower left coordinate
     */
    getLowerLeftCoord : function()
    {
        return {x:this.fMinX, y:this.fMinY};
    },

    /**
     * Function: getUpperRightCoord
     * 
     * Gets the upper right coordinate of this selection's bounding box
     * 
     * Returns:
     * 
     *   the upper right coordinate
     */
    getUpperRightCoord : function()
    {
        return {x:this.fMaxX, y:this.fMaxY};
    },

    /**
     * Function: getNumLayers
     * 
     * Gets the number of map layers included in this selection
     * 
     * Returns:
     *  The number of map layers
     */
    getNumLayers : function()
    {
        return this.nLayers;
    },

    /**
     * Function: getLayerByName
     * 
     * Gets the selected map layer by its name
     * 
     * Parameters:
     * 
     *   name - The name of the selected map layer
     * 
     * Returns:
     * 
     *   {<Fusion.SelectionObject.Layer>} The selected map layer
     */
    getLayerByName : function(name)
    {
        var oLayer = null;
        for (var i=0; i<this.nLayers; i++)
        {
            if (this.aLayers[i].getName() == name)
            {
                oLayer = this.aLayers[i];
                break;
            }
        }
        return oLayer;
    },

    /**
     * Function: getLayer
     * 
     * Gets the selected map layer by the specified index
     * 
     * Parameters:
     * 
     *   iIndice - The index of the selected map layer
     * 
     * Returns:
     * 
     *   {<Fusion.SelectionObject.Layer>} The selected map layer
     */
    getLayer : function(iIndice)
    {
        if (iIndice >=0 && iIndice < this.nLayers)
        {
            return this.aLayers[iIndice];
        }
        else
        {
            return null;
        }

    }
});

/**
 * Class: Fusion.SelectionObject.Layer
 * 
 * Defines a map layer in a {<Fusion.SelectionObject>}
 */
Fusion.SelectionObject.Layer = OpenLayers.Class({
    name: null,
    nElements: null,
    aElements: null,
    nProperties: null,
    aPropertiesName: null,
    aPropertiesTypes: null,

    type: null,
    area: null,
    distance: null,
    bbox: null,
    center: null,

    initialize: function(o, layerName)
    {
        this.name =  layerName;
        this.nElements =  o[layerName].numelements;

        this.aElements = [];

        this.nProperties = o[layerName].propertyvalues.length;

        this.aPropertiesName = [];
        this.aPropertiesName  = o[layerName].propertyvalues;

        this.aPropertiesTypes = [];
        this.aPropertiesTypes = o[layerName].propertytypes;

        //var oValueCollection = oNode.findNextNode('ValueCollection');

        this.area = 0;
        this.distance = 0;

        for (var i=0; i<o[layerName].values.length; i++)
        {
            this.aElements[i] =[];
            for (var j=0; j<o[layerName].values[i].length; j++)
            {
                this.aElements[i][j] = o[layerName].values[i][j];
            }
        }

        //loop over all features in the metadata array
        for (var i=0; i<o[layerName].metadata.length; i++) {
          var featureMetadata = o[layerName].metadata[i];
          var dim = featureMetadata[0];   //nothing to do with this one
          var bbox = featureMetadata[1];
          var center = featureMetadata[2];
          var area = featureMetadata[3];
          var length = featureMetadata[4];
          this.area += parseFloat(area);
          this.distance += parseFloat(length);
        }

        /*
        var iElement=0;
        while(oValueCollection)
        {
            this.aElements[iElement] = [];
            for (var i=0; i<oValueCollection.childNodes.length; i++)
            {
                oTmp = oValueCollection.childNodes[i].findFirstNode('v');
                this.aElements[iElement][i] = oTmp.textContent;

            }
            var type = oValueCollection.attributes['type'];
            var area = oValueCollection.attributes['area'];
            var distance = oValueCollection.attributes['distance'];
            var bbox = oValueCollection.attributes['bbox'];
            var center = oValueCollection.attributes['center'];

            this.aElements[iElement]['attributes'] = {};
            this.aElements[iElement]['attributes'].type = type;
            this.aElements[iElement]['attributes'].bbox = bbox;
            this.aElements[iElement]['attributes'].center = bbox;
            //console.log('type is ' + type);
            if (type > 1) {
                this.area += parseFloat(area);
                this.aElements[iElement]['attributes'].area = area;
            }
            if (type > 0) {
                this.aElements[iElement]['attributes'].distance = distance;
                this.distance += parseFloat(distance);
            }
            oValueCollection = oNode.findNextNode('ValueCollection');
            iElement++;
        }
        */
        //console.log( 'final area is ' + this.area);
        //console.log( 'final distance is ' + this.distance);

    },

    getName : function()
    {
        return this.name;
    },

    getNumElements : function()
    {
        return this.nElements;
    },

    getNumProperties : function()
    {
        return this.nProperties;
    },

    getPropertyNames : function()
    {
        return this.aPropertiesName;
    },

    getPropertyTypes : function()
    {
        return this.aPropertiesTypes;
    },

    getElementValue : function(iElement, iProperty)
    {
        if (iElement >=0 && iElement < this.nElements &&
            iProperty >=0 && iProperty < this.nProperties)
        {
            return this.aElements[iElement][iProperty];
        }
        else
        {
            return null;
        }
    }
});
