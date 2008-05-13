/**
 * Fusion.Widget.Map
 *
 * $Id: Map.js 1401 2008-05-09 17:32:10Z madair $
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

 /****************************************************************************
 * Class: Fusion.Widget.Map 
 *
 * generic class for map widgets. Provides common utility classes.
 * **********************************************************************/

Fusion.Event.MAP_EXTENTS_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_BUSY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_GENERIC_EVENT = Fusion.Event.lastEventId++;
Fusion.Event.MAP_RESIZED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SELECTION_ON = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SELECTION_OFF = Fusion.Event.lastEventId++;
Fusion.Event.MAP_ACTIVE_LAYER_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_LOADED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_LOADING = Fusion.Event.lastEventId++;
Fusion.Event.MAP_RELOADED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SESSION_CREATED = Fusion.Event.lastEventId++;


Fusion.Constant.LAYER_POINT_TYPE = 0;
Fusion.Constant.LAYER_LINE_TYPE = 1;
Fusion.Constant.LAYER_POLYGON_TYPE = 2;
Fusion.Constant.LAYER_SOLID_TYPE = 3;
Fusion.Constant.LAYER_RASTER_TYPE = 4;
Fusion.Constant.LAYER_DWF_TYPE = 5;

Fusion.Widget.Map = Class.create();
Fusion.Widget.Map.prototype =
{

    _oDomObj : null,
    _sDomObj : '',
    _sMapname : '',  
    _nWidth : -1,
    _nHeight : -1,  
    _fMetersperunit : -1,
    _fScale : -1,
    _nDpi : 96,
    _oCurrentExtents: null,
    _oInitialExtents: null,
    _nWorkers: 0,
    oContextMenu: null,
    bSupressContextMenu: false,
    
    aMaps: null,
    layerRoot: null,
    singleTile: true,
    
    /**
     * construct a new view Fusion.Widget.Map class.  
     */
    initialize : function(widgetTag, mapGroup, widgetSet) {    
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.widgetTag = widgetTag;
        var name = widgetTag.name;
        
        this.widgetSet = widgetSet;
        this._nCellSize = -1;
        this._sDomObj = name;
        this._oDomObj = $(this._sDomObj);
        this.layerRoot = new Fusion.Widget.Map.Group();
        
        if (this._oDomObj.jxLayout) {
            this._oDomObj.jxLayout.addSizeChangeListener(this);
        }
        
        OpenLayers.DOTS_PER_INCH = this._nDpi;
        if (!this.oMapOL) {
            var options = {controls: [], fallThrough: true};
            if (widgetTag.extension.ConstrainMapExtent) {
              this.bRestrictExtent = widgetTag.extension.ConstrainMapExtent[0]=='true'?true:false;
            }
            this.oMapOL = new OpenLayers.Map(this._sDomObj, options );
        }
        
        this.oMapOL.viewPortDiv.style.position = 'absolute';  //not the top level container so set it to absolute
        
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
        }
       
        //create the 'Map' layer widgets defined in the MapGroup
        this.aMaps = [];
        this.mapGroup = mapGroup;
        for (var i=0; i<mapGroup.maps.length; ++i) {
          var mapTag = mapGroup.maps[i];
          if (Fusion.Maps[mapTag.type]) {
              this.aMaps[i] = eval("new Fusion.Maps."+mapTag.type+"(this,mapTag)");
              this.layerRoot.addGroup(this.aMaps[i].layerRoot);
              
          } else {
              //TODO: we can add more OpenLayers layers ...
          }
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
        
        //register for OL map extent change events
        this.oMapOL.events.register('moveend', this, this.mapExtentsChanged);
        
        this._oDomObj.oncontextmenu = function() {return false;};
        OpenLayers.Event.observe(this._oDomObj, 'contextmenu', this.onContextMenu.bind(this));
        
        this.aSelectionCallbacks = [];
        this.bFetchingSelection = false;
    },
    
    setMenu: function() {  
        if (this.widgetTag.extension.MenuContainer) {
            var contextMenu = new Jx.ContextMenu();
            var container = this.widgetSet.getContainerByName(this.widgetTag.extension.MenuContainer[0]);
            if (container) {
                container.createWidgets(this.widgetSet, contextMenu);
                this.setContextMenu(contextMenu);
            }
            
        }
        
    },
    
    loadMapGroup: function(mapGroup) {
        //clear any existing workers since loading a new map cancels all previous requests
        while (this.isBusy()) {
          this._removeWorker();
        }
        
        //clear any existing selection
        this.clearSelection();
        
        this.mapGroup = mapGroup;
        for (var i=0; i<this.aMaps.length; i++) {
            this.aMaps[i].oLayerOL.destroy();
        }
        
        this.aMaps = [];
        this.layerRoot = new Fusion.Widget.Map.Group();
        for (var i=0; i<mapGroup.maps.length; ++i) {
          var mapTag = mapGroup.maps[i];
          if (Fusion.Maps[mapTag.type]) {
              this.aMaps[i] = eval("new Fusion.Maps."+mapTag.type+"(this,mapTag)");
              this.layerRoot.addGroup(this.aMaps[i].layerRoot);
              
          } else {
              //TODO: we can add more OpenLayers layers ...
          }
        }
    },
    
    /**
     * Method: wheelChange  
     *
     * Parameters:
     * evt - {Event}
     */
    wheelChange: function(evt, deltaZ) {
        var size    = this.oMapOL.getSize();
        var deltaX  = size.w/2 - evt.xy.x;
        var deltaY  = evt.xy.y - size.h/2;
        
        var deltaRes = deltaZ > 0 ? 0.5 : 2;  //TODO: use some sort of factor here
        var newRes  = this.oMapOL.baseLayer.getResolution()*deltaRes;
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
     * returns the dom element 
     */
    getDomObj : function() {
        return this._oDomObj;
    },


    getMapName : function() {  
        //TODO: what is the mapname in the case of multiple map layer objects?
        //just return baselayer mapname for now
        return this.aMaps[0].getMapName();
    },

    getMapTitle : function() {  
        //TODO: what is the map title in the case of multiple map layer objects?
        //just return baselayer mapTitle for now
        return this.aMaps[0]._sMapTitle;
    },

    getSessionID : function() {  
        //TODO: what is the mapname in the case of multiple map layer objects?
        //just return baselayer session ID for now
        return this.aMaps[0].getSessionID();
    },

    getDomId : function() {  
        return this._sDomObj;
    },

    setMapOptions: function(options) {
        this.oMapOL.setOptions(options);
    },

    addMap: function(map) {
        if (!map.bSingleTile) {
            this.singleTile = false;
        }
        this.projection = map.projection;
        
        //if bRestrictExtent is null, use the default OL behaviour with somewhat restricted map navigation
        //if bRestrictExtent is set to true, map navigation is limited to the map extent
        //if bRestrictExtent is set to false, map navigation is not restricted at all        
        if (this.bRestrictExtent != null) {
          if (this.bRestrictExtent) {
            this.oMapOL.restrictedExtent = map._oMaxExtent;
          } else {
            this.oMapOL.restrictedExtent = false;
          }
        }
        this.oMapOL.addLayer(map.oLayerOL);
        map.registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.selectionHandler.bind(this));
        map.registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.selectionHandler.bind(this));
    },

    getAllMaps: function() {
        return this.aMaps;
    },
    
    //this uses setTimeout so this method can be called from an IFRAME
    reloadMap: function() {
      for (var i=0; i<this.aMaps.length; ++i) {
        var map = this.aMaps[i];
        window.setTimeout(map.reloadMap.bind(map),1);
      }
    },
    
    /**
     * Function: query
     *
     * dispatch query requests to maps
     */
    query: function(options) {
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
     * returns true if any map has a selection
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
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].clearSelection();
         }
     },
     
     /**
      * Function: getSelection
      *
      * returns the current selection asynchronously in case we
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
             this.nSelectionMaps++;
             this.aMaps[i].getSelection(this.accumulateSelection.bind(this, this.aMaps[i]), layers, startcount);
         }
     },

     /**
      * Function: setSelectionXML
      *
      * sets a Selection XML back to the server
      */
      setSelection: function(selText, requery, zoomTo) {
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].setSelection(selText, requery, zoomTo);
         }
      },

     /**
      * Function: accumulateSelection
      *
      * accumulate the selection results from each map and when all have
      * reported in, pass the results to the callback function
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
      */
    setActiveLayer: function( oLayer ) {
        this.oActiveLayer = oLayer;
        this.oActiveMap = oLayer.map;
        this.triggerEvent(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED, oLayer);
    },

     /**
      * Function: getActiveLayer
      *
      * returns the active layer for selection/manipulation
      */
    getActiveLayer: function() {
        return this.oActiveLayer;
    },

    /**
     * indicate that a new asynchronous process has started and make sure the
     * visual indicator is visible for the user.  This is intended to be used
     * internally by gMap but could be used by external tools if appropriate.
     */
    _addWorker : function() {
        this._nWorkers += 1;
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
        this._oDomObj.style.cursor = 'wait';  
    },

    /**
     * indicate that an asynchronous process has completed and hide the
     * visual indicator if no remaining processes are active.  This is 
     * intended to be used internally by gMap but could be used by 
     * external tools if appropriate.  Only call this function if
     * addWorker was previously called
     */
    _removeWorker : function() {
        if (this._nWorkers > 0) {
            this._nWorkers -= 1;
        }
        this.setCursor(this.cursor);
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
    },
    
    mapExtentsChanged: function() {
        this._oCurrentExtents = this.oMapOL.getExtent();
        this.triggerEvent(Fusion.Event.MAP_EXTENTS_CHANGED);
    },

    isBusy: function() {
        return this._nWorkers > 0;
    },

    sizeChanged: function() {
        this.resize();
    },
    
    resize : function() {
      //console.log('Fusion.Widget.Map.resize');
        this.oMapOL.updateSize();
        var d = Element.getDimensions(this.getDomObj());
        this._nWidth = d.width;
        this._nHeight = d.height;
        if (this._oCurrentExtents) {
          this.setExtents(this._oCurrentExtents);
        } else if (this._oInitialExtents) {
          this.setExtents(this._oInitialExtents);
        }
        this.triggerEvent(Fusion.Event.MAP_RESIZED, this);
    },
    
    redraw: function() {
      for (var i=0; i<this.aMaps.length; i++ ) {
        this.aMaps[i].oLayerOL.params.ts = (new Date()).getTime();
        //mergeNewParams calls redraw on the layer, which will get called below anyway
        //this.aMaps[i].oLayerOL.mergeNewParams({ts : (new Date()).getTime()});
      }
      this.oMapOL.setCenter(this.oMapOL.getCenter(), this.oMapOL.getZoom(), false, true);
    },
    
    setExtents : function(oExtents) {
        if (!oExtents) {
            Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING, 
                                OpenLayers.String.translate('nullExtents')));
        }
        if (oExtents instanceof Array && oExtents.length == 4) {
            oExtents = new OpenLayers.Bounds(oExtents[0], oExtents[1], oExtents[2], oExtents[3]);
        }
        
        if (this.aMaps[0].bSingleTile) {
            var viewSize = this.oMapOL.getSize();
            var idealResolution = Math.max( oExtents.getWidth()  / viewSize.w,
                                            oExtents.getHeight() / viewSize.h,
                                          this.oMapOL.baseLayer.minResolution);
            idealResolution = Math.min( idealResolution, this.oMapOL.baseLayer.maxResolution);
            
            this.oMapOL.baseLayer.resolutions = [idealResolution];
            this.oMapOL.zoom = 1;
        }
    
        //update the timestamp param to prevent caching
        for (var i=0; i<this.aMaps.length; i++ ) {
          this.aMaps[i].oLayerOL.params.ts = (new Date()).getTime();
        }
        this.oMapOL.zoomToExtent(oExtents);
    },

    fullExtents : function() {
      //determine the initialExtents
      if (!this._oInitialExtents) {
        var bbox = Fusion.getQueryParam("extent");   //set as min x,y, max x,y
        if (bbox) {
          this._oInitialExtents = new OpenLayers.Bounds.fromArray(bbox.split(","));
        } else if (this.mapGroup.initialView) {
            var iv = this.mapGroup.getInitialView();
            if (iv.x) {
                this._oInitialExtents = this.getExtentFromPoint(iv.x, iv.y, iv.scale);                
            } else if (iv.minX) {
                this._oInitialExtents = new OpenLayers.Bounds(iv.minX, iv.minY, iv.maxX, iv.maxY);
            }
        } else {
          var viewSize = this.oMapOL.getSize();
          var oExtents = this.oMapOL.getMaxExtent();
          var center = oExtents.getCenterLonLat();
          var initRes = Math.max( oExtents.getWidth()  / viewSize.w,
                                  oExtents.getHeight() / viewSize.h);
          var w_deg = viewSize.w * initRes/2;
          var h_deg = viewSize.h * initRes/2;
          this._oInitialExtents = new OpenLayers.Bounds(center.lon - w_deg,
                                             center.lat - h_deg,
                                             center.lon + w_deg,
                                             center.lat + h_deg);
        }
      }
      this.setExtents(this._oInitialExtents); 
    },

    isMapLoaded: function() {
        return (this._oCurrentExtents) ? true : false;
    },

    zoom : function(fX, fY, nFactor) {
        //do this differntly with OL code??
        var extent = this.oMapOL.getExtent();
        var fDeltaX = extent.right - extent.left;
        var fDeltaY = extent.top - extent.bottom;
        var fMinX,fMaxX,fMinY,fMaxY;
        if (nFactor == 1 || nFactor == 0) {
            /*recenter*/
            fMinX = fX - (fDeltaX/2);
            fMaxX = fX + (fDeltaX/2);
            fMinY = fY - (fDeltaY/2);
            fMaxY = fY + (fDeltaY/2);
        } else if (nFactor > 0) {
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
    },
    
    zoomToScale: function(fScale) {
        var center = this.getCurrentCenter();
        var extent = this.getExtentFromPoint(center.x, center.y, fScale);
        this.setExtents(extent);
    },
    
    queryRect : function(fMinX, fMinY, fMaxX, fMaxY) { },
    
    queryPoint : function(fX, fY) { },
    
    /**
     *
     * convert pixel coordinates into geographic coordinates.
     *
     * @paran pX int the x coordinate in pixel units
     * @param pY int the y coordinate in pixel units
     *
     * @return an object with geographic coordinates in x and y properties of the 
     *         object.
     */
    pixToGeo : function( pX, pY ) {
        var lonLat = this.oMapOL.getLonLatFromPixel( new OpenLayers.Pixel(pX,pY) );
        if (lonLat != null) {
          return {x:lonLat.lon, y:lonLat.lat}; 
        }
        return null;
    },

    /**
     *
     * convert geographic coordinates into pixel coordinates.
     *
     * @paran gX int the x coordinate in geographic units
     * @param gY int the y coordinate in geographic units
     *
     * @return an object with pixel coordinates in x and y properties of the 
     *         object.
     */
    geoToPix : function( gX, gY ) {
        if (!(this._oCurrentExtents)) {
            return null;
        }
        var px = this.oMapOL.getPixelFromLonLat(new OpenLayers.LonLat(gX,gY));  //use getViewPortPxFromLonLat instead?
        return {x:Math.floor(px.x), y:Math.floor(px.y)};
    },

    /**
     *
     * convert pixel into geographic : used to measure.
     *
     * @param nPixels int measures in pixel
     *
     * @return geographic measure
     */
    pixToGeoMeasure : function(nPixels) {
        var resolution = this.oMapOL.getResolution();
        return (nPixels*resolution);
    },
    
  /**
     *
     * initializes the meters per unit values when a new map is loaded.  Some systems make different 
     * assumptions for the conversion of degrees to meters so this makes sure both Fusion and
     * OpenLayers are using the same value.
     *
     * @param metersPerUnit the value returned by LoadMap.php for meters per unit
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
     *
     * returns the meters per unit value
     *
     * @return metersPerUnit the value as set when the map initialized
     */
    getMetersPerUnit: function() {
        return this._fMetersperunit;
    },
    
    /**
     *
     * convert geographic into pixels.
     *
     * @param fGeo float distance in geographic units
     *
     * @return pixels
     */
    geoToPixMeasure : function(fGeo) {
        return parseInt(fGeo/this.oMapOL.getResolution());
    },
    
    /**
     * Function: getCurrentCenter
     *
     * returns the current center of the map view
     *
     * Return: {Object} an object with the following attributes
     * x - the x coordinate of the center
     * y - the y coordinate of the center
     */
    getCurrentCenter : function() {
        var c = this.getCurrentExtents().getCenterLonLat();
        return {x:c.lon, y:c.lat};
    },

    /**
     *
     * returns the current extents
     */
    getCurrentExtents : function() {
        return this.oMapOL.getExtent();
    },

    /**
     *
     * returns initial extents
    */
    getInitialExtents : function(){
        return this._oInitialExtents;
    },

    /**
     * Function: getExtentFromPoint
     *
     * returns the Extent of the map given a center point and a scale (optional)
     *
     * Return: {OpenLayers.Bounds} the bounds for the map centered on a point
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
    
    getScale : function() {
        return this.oMapOL.getScale();
    },
    
    getResolution : function() {
        return this.oMapOL.getResolution();
    },
    
    getUnits : function() {
        return this.oMapOL.baseLayer.units;
    },
    
    getSize: function() {
        return this.oMapOL.getSize();
    },

    getEventPosition : function(e) {
        return this.oMapOL.events.getMousePosition(e);
    },

    setCursor : function(cursor) {
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
     observeEvent  : function(sEventName, fnCB)
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
     stopObserveEvent : function(sEventName, fnCB)
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
     activateWidget : function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.activateWidget ' + oWidget.getName());*/
         if (oWidget.isMutEx()) {
             if (this.oActiveWidget) {
                 this.deactivateWidget(this.oActiveWidget);
             }
             oWidget.activate();
             this.oActiveWidget = oWidget;
         } else {
             oWidget.activate();
         }
     },

     /**
     *
     * call the Activate method on the widget
     * if widgets is set to be mutually exclusive,
     * all other widgets are deactivated
     *
     * @param oWidget the widget to deactivate
     */
     deactivateWidget : function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.deactivateWidget ' + oWidget.getName());*/
         oWidget.deactivate();
         this.oActiveWidget = null;
     },
     
     /**
      */
     isLoaded: function() {
         return (this.oMapOL.getExtent() != null);
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
             this.oContextMenu.show(e);
             this.contextMenuPosition = this.getEventPosition(e);
             OpenLayers.Event.stop(e);
         }
     },
     
     executeFromContextMenu: function(widget) {
         //console.log('executefromcontextmenu');
         widget.execute(this.contextMenuPosition.x, this.contextMenuPosition.y);
     }
};

 /****************************************************************************
 * Class: Fusion.Widget.Map.Layer
 *
 * generic class for map layers.  This class is primarily for legends.
 * **********************************************************************/
Fusion.Event.LAYER_PROPERTY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Widget.Map.Layer = Class.create();
Fusion.Widget.Map.Layer.prototype = {
    name: null,
    initialize: function(name) {
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.name = name;
        this.registerEventID(Fusion.Event.LAYER_PROPERTY_CHANGED);
    },
    clear: function() {},
    set: function(property, value) {
        this[property] = value;
        this.triggerEvent(Fusion.Event.LAYER_PROPERTY_CHANGED, this);
    }
};

 /****************************************************************************
 * Class: Fusion.Widget.Map.Group
 *
 * generic class for map layer groups.  This class is primarily for legends.
 * **********************************************************************/
Fusion.Event.GROUP_PROPERTY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Widget.Map.Group = Class.create();
Fusion.Widget.Map.Group.prototype = {
    name: null,
    groups: null,
    layers: null,
    initialize: function(name) {
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.name = name;
        this.groups = [];
        this.layers = [];
        this.registerEventID(Fusion.Event.GROUP_PROPERTY_CHANGED);
    },
    clear: function() {
        for (var i=0; i<this.groups.length; i++) {
            this.groups[i].clear();
        }
        for (var i=0; i<this.layers.length; i++) {
            this.layers[i].clear();
        }
        this.groups = [];
        this.layers = [];
    },
    set: function(property, value) {
        this[property] = value;
        this.triggerEvent(Fusion.Event.GROUP_PROPERTY_CHANGED, this);
    },
    addGroup: function(group,reverse) {
        group.parentGroup = this;
        if (reverse) {
          this.groups.unshift(group);
        } else {
          this.groups.push(group);
        }
    },
    addLayer: function(layer,reverse) {
        layer.parentGroup = this;
        if (reverse) {
          this.layers.unshift(layer);
        } else {
          this.layers.push(layer);
        }
    },
    findGroup: function(name) {
        return this.findGroupByAttribute('name', name);
    },
    findGroupByAttribute: function(attribute, value) {
        if (this[attribute] == value) {
            return this;
        }
        for (var i=0; i<this.groups.length; i++) {
            var group = this.groups[i].findGroupByAttribute(attribute, value);
            if (group) {
                return group;
            }
        }
        return null;
    },
    findLayer: function(name) {
        return this.findLayerByAttribute('name', name);
    },
    findLayerByAttribute: function(attribute, value) {
        for (var i=0; i<this.layers.length; i++) {
            if (this.layers[i][attribute] == value) {
                return this.layers[i];
            }
        }
        for (var i=0; i<this.groups.length; i++) {
            var layer = this.groups[i].findLayerByAttribute(attribute,value);
            if (layer) {
                return layer;
            }
        }
        return null;
    }
};


/**
 * SelectionObject
 *
 * Utility class to hold slection information
 *
 */
var GxSelectionObject = Class.create();
GxSelectionObject.prototype = 
{
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
                this.aLayers[i] = new GxSelectionObjectLayer(o, o.layers[i]);
            }
        }
    },

    getNumElements : function()
    {
        return this.nTotalElements;
    },

    getLowerLeftCoord : function()
    {
        return {x:this.fMinX, y:this.fMinY};
    },

    getUpperRightCoord : function()
    {
        return {x:this.fMaxX, y:this.fMaxY};
    },

    getNumLayers : function()
    {
        return this.nLayers;
    },
    
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
};


var GxSelectionObjectLayer = Class.create();
GxSelectionObjectLayer.prototype = {

    sName: null,
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
        this.sName =  layerName;
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
        return this.sName;
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
};
