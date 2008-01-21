/**
 * Fusion.Maps.MapServer
 *
 * $Id: MapServer.js 1188 2008-01-15 16:10:51Z madair $
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

/******************************************************************************
 * Class: Fusion.Maps.MapServer 
 *
 * Implementation of the map widget for MapServer CGI interface services
*/
Fusion.Event.MAP_LAYER_ORDER_CHANGED = Fusion.Event.lastEventId++;

Fusion.Maps.MapServer = Class.create();
Fusion.Maps.MapServer.prototype = {
    arch: 'MapServer',
    session: [null],
    aShowLayers: null,
    aHideLayers: null,
    aShowGroups: null,
    aHideGroups: null,
    aRefreshLayers: null,
    sActiveLayer: null,
    selectionType: 'INTERSECTS',
    bSelectionOn: false,
    bDisplayInLegend: true,   //TODO: set this in AppDef?
    bExpandInLegend: true,   //TODO: set this in AppDef?
    oSelection: null,
    bMapLoaded : false,
    bIsMapWidgetLayer : true,  //Setthis to false for overview map layers
    bLayersReversed: true,     //MS returns layers bottom-most layer first, we treat layer order in reverse sense

    //the map file
    sMapFile: null,
    
    //imagetype
    _sImageType : 'png',
    
    initialize : function(map, mapTag, isMapWidgetLayer) {
        //console.log('Fusion.Maps.MapServer.initialize');
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
                
        this.registerEventID(Fusion.Event.MAP_SESSION_CREATED);
        this.registerEventID(Fusion.Event.MAP_SELECTION_ON);
        this.registerEventID(Fusion.Event.MAP_SELECTION_OFF);
        this.registerEventID(Fusion.Event.MAP_LOADED);
        this.registerEventID(Fusion.Event.MAP_LOADING);
        this.registerEventID(Fusion.Event.MAP_LAYER_ORDER_CHANGED);

        this.mapWidget = map;
        this.oSelection = null;
        if (isMapWidgetLayer != null) {
            this.bIsMapWidgetLayer = isMapWidgetLayer;
        }

        var extension = mapTag.extension;
        this.ratio = extension.MapRatio ? extension.MapRatio[0] : '1.0';
        //this.selectionType = extension.SelectionType ? extension.SelectionType[0] : 'INTERSECTS';
        
        rootOpts = {
          displayInLegend: this.bDisplayInLegend,
          expandInLegend: this.bExpandInLegend,
          legendLabel: this._sMapname,
          uniqueId: 'layerRoot',
          groupName: 'layerRoot',
          visible: true,
          actuallyVisible: true
          //TODO: set other opts for group initialization as required
        };
        this.layerRoot = new Fusion.Maps.MapServer.Group(rootOpts,this);
        
        this.sMapFile = extension.MapFile ? extension.MapFile[0] : '';

        this.bSingleTile = mapTag.singleTile;// this is set by the AppDef.Map object

        this.keepAliveInterval = parseInt(extension.KeepAliveInterval ? extension.KeepAliveInterval[0] : 300);

        if (mapTag.sid) {
            this.session[0] = mapTag.sid;
            this.mapSessionCreated();
        } else {
            this.createSession();
        }
    },

    createSession: function() {
        if (!this.session[0]) {
            this.session[0] = this;
            var sl = Fusion.getScriptLanguage();
            var scriptURL = this.arch + '/' + sl + '/CreateSession.' + sl;
            var options = {onComplete: this.createSessionCB.bind(this)};
            Fusion.ajaxRequest(scriptURL,options);  
        }
        if (this.session[0] instanceof Fusion.Maps.MapServer) {
            this.session[0].registerForEvent(Fusion.Event.MAP_SESSION_CREATED, this.mapSessionCreated.bind(this));
        } else {
            this.mapSessionCreated();
        }
    },
    
    createSessionCB : function(r, json) {
        if (r.status == 200 && json) {
            var o;
            eval('o='+r.responseText);
            this.session[0] = o.sessionId;
            this.triggerEvent(Fusion.Event.MAP_SESSION_CREATED);
        }
    },

    mapSessionCreated: function() {
        if (this.sMapFile != '') {
            this.loadMap(this.sMapFile);
        }
        window.setInterval(this.pingServer.bind(this), this.keepAliveInterval * 1000);
    },

    sessionReady: function() {
        return (typeof this.session[0] == 'string');
    },

    getSessionID: function() {
        return this.session[0];
    },
    
    getMapName: function() {
        return this._sMapname;
    },
    
    getMapTitle: function() {
        return this._sMapTitle;
    },
    
    loadMap: function(mapfile, options) {
        while (this.mapWidget.isBusy()) {
	        this.mapWidget._removeWorker(); 
		}
        this.bMapLoaded = false;
        //console.log('loadMap: ' + resourceId);
        /* don't do anything if the map is already loaded? */
        if (this._sMapFile == mapfile) {
            return;
        }

        if (!this.sessionReady()) {
            this.sMapFile = mapfile;
            return;
        }
        
        if (this.bIsMapWidgetLayer) {
            this.mapWidget.triggerEvent(Fusion.Event.MAP_LOADING);
        } else {
            this.triggerEvent(Fusion.Event.MAP_LOADING);
        }
        this.mapWidget._addWorker();
        
        this._fScale = -1;
        this._nDpi = 72;
        
        options = options || {};
        
        this._oMaxExtent = null;
        this.aVisibleLayers = options.showlayers || [];
        this.aVisibleGroups = options.showgroups || [];
        this.aLayers = [];
        
        this.oSelection = null;
        this.aSelectionCallbacks = [];
        this._bSelectionIsLoading = false;

        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/LoadMap.' + sl;
        
        var sessionid = this.getSessionID();
        
        var params = 'mapfile='+mapfile+"&session="+sessionid;
        var options = {onSuccess: this.mapLoaded.bind(this), 
                                     parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    mapLoaded: function(r, json) {
        if (json)  
        { 
            var o; 
            eval('o='+r.responseText); 
            this._sMapFile = o.mapId;
            this._sMapname = o.mapName; 
            this._sMapTitle = o.mapTitle;
            this._fMetersperunit = o.metersPerUnit; 
            this.mapWidget._fMetersperunit = this._fMetersperunit;
            this._sImageType = o.imagetype; 

            this._oMaxExtent = OpenLayers.Bounds.fromArray(o.extent); 
            
            this.layerRoot.clear();
            this.layerRoot.legendLabel = this._sMapTitle;
            
            this.parseMapLayersAndGroups(o);
      			var minScale = 1.0e10;
      			var maxScale = 0;
            for (var i=0; i<this.aLayers.length; i++) {
              if (this.aLayers[i].visible) {
                  this.aVisibleLayers.push(this.aLayers[i].layerName);
              }
      				minScale = Math.min(minScale, this.aLayers[i].minScale);
      				maxScale = Math.max(maxScale, this.aLayers[i].maxScale);
            }
            //a scale value of 0 is undefined
            if (minScale <= 0) {
              minScale = 1.0;
            }
            
            if (o.dpi) {
                OpenLayers.DOTS_PER_INCH = o.dpi;
            }

            var layerOptions = {
      				singleTile: true, 
      				ratio: this.ratio,
      				maxExtent : this._oMaxExtent,
              maxResolution : 'auto',
      				minScale : maxScale,	//OL interpretation of min/max scale is reversed from Fusion
      				maxScale : minScale
      			};

            //set OpenLayer projection units and code if supplied (OL defaults units to degrees)
            if (o.metersPerUnit == 0.0254)
               layerOptions.units = 'inches';
            else if (o.metersPerUnit == 0.3048)
               layerOptions.units = 'ft';
            else if (o.metersPerUnit == 1609.344)
               layerOptions.units = 'mi';
            else if (o.metersPerUnit == 1)
               layerOptions.units = 'm';
               //layerOptions.projection = 'EPSG:42304';  //TODO: not necessary, but can this be supplied by LoadMap?
            else if (o.metersPerUnit == 1000)
               layerOptions.units = 'km';
            else if (o.metersPerUnit == 111118.7516)
               layerOptions.units = 'dd';

            //this.mapWidget.setMapOptions(oMapOptions);

            //create the OL layer for this Map layer
            var params = {
              layers: this.aVisibleLayers.join(' '),
              session : this.getSessionID(),
              map : this._sMapFile,
              map_imagetype : this._sImageType
            };

            //remove this layer if it was already loaded
            if (this.oLayerOL) {
                this.oLayerOL.events.unregister("loadstart", this, this.loadStart);
                this.oLayerOL.events.unregister("loadend", this, this.loadEnd);
                this.oLayerOL.events.unregister("loadcancel", this, this.loadEnd);
                this.oLayerOL.destroy();
            }

            var url = Fusion.getConfigurationItem('mapserver', 'cgi');
            this.oLayerOL = new OpenLayers.Layer.MapServer( o.mapName, url, params, layerOptions);
            this.oLayerOL.events.register("loadstart", this, this.loadStart);
            this.oLayerOL.events.register("loadend", this, this.loadEnd);
            this.oLayerOL.events.register("loadcancel", this, this.loadEnd);

            if (this.bIsMapWidgetLayer) {
              this.mapWidget.addMap(this);
              this.mapWidget.oMapOL.setBaseLayer(this.oLayerOL);
              this.mapWidget._oInitialExtents = null;
              this.mapWidget.fullExtents();
              this.mapWidget.triggerEvent(Fusion.Event.MAP_LOADED);
            } else {
              this.triggerEvent(Fusion.Event.MAP_LOADED);
            }

            this.bMapLoaded = true;
        }  
        else 
        {
            Fusion.reportError( new Fusion.Error(Fusion.Error.FATAL, 
					'Failed to load requested map:\n'+r.responseText));
        }
        this.mapWidget._removeWorker();
    },
    
    reloadMap: function() {
        this.mapWidget._addWorker();
        this.aShowLayers = [];
        this.aHideLayers = [];
        this.aShowGroups = [];
        this.aHideGroups = [];
        this.aRefreshLayers = [];
        this.layerRoot.clear();
        this.aLayers = [];
        
        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/LoadMap.' + sl;
        
        var sessionid = this.getSessionID();
        
        var params = 'mapname='+this._sMapname+"&session="+sessionid;
        var options = {onSuccess: this.mapReloaded.bind(this), 
                                     parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    mapReloaded: function(r,json) {  /* update this with OL code */
        if (json) {
            var o;
            eval('o='+r.responseText);
            this.parseMapLayersAndGroups(o);
            this.aVisibleLayers = [];
            for (var i=0; i<this.aLayers.length; i++) {
                if (this.aLayers[i].visible) {
                    this.aVisibleLayers.push(this.aLayers[i].layerName);
                }
            }
            this.drawMap();
            this.mapWidget.triggerEvent(Fusion.Event.MAP_RELOADED);
        } else {
            Fusion.reportError( new Fusion.Error(Fusion.Error.FATAL, 
                OpenLayers.String.translate('mapLoadError', r.responseText)));
        }
        this.mapWidget._removeWorker();
    },
    
    reorderLayers: function(aLayerIndex) {
        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/SetLayers.' + sl;
        
        var sessionid = this.getSessionID();
        var params = 'mapname='+this._sMapname+"&session="+sessionid;
        params += '&layerindex=' + aLayerIndex.join();
		
        var options = {onSuccess: this.mapLayersReset.bind(this, aLayerIndex), 
                                     parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    mapLayersReset: function(aLayerIndex,r,json) {
      if (json) {
        var o;
        eval('o='+r.responseText);
  			if (o.success) {
  				var layerCopy = this.aLayers.clone();
  				this.aLayers = [];
  				this.aVisibleLayers = [];
  			
          for (var i=0; i<aLayerIndex.length; ++i) {
            this.aLayers.push( layerCopy[ aLayerIndex[i] ] );
            if (this.aLayers[i].visible) {
                this.aVisibleLayers.push(this.aLayers[i].layerName);
            }
  				}
  				//this.layerRoot.clear();
  			
  				this.drawMap();
  				this.triggerEvent(Fusion.Event.MAP_LAYER_ORDER_CHANGED);
  			} else {
          alert(OpenLayers.String.translate('setLayersError', o.layerindex));
  			}
      }
    },
			
    parseMapLayersAndGroups: function(o) {
        for (var i=0; i<o.groups.length; i++) {
            var group = new Fusion.Maps.MapServer.Group(o.groups[i], this);
            var parent;
            if (group.parentUniqueId != '') {
                parent = this.layerRoot.findGroup(group.parentUniqueId);
            } else {
                parent = this.layerRoot;
            }
            parent.addGroup(group, this.bLayersReversed);
        }

        for (var i=0; i<o.layers.length; i++) {
            var layer = new Fusion.Maps.MapServer.Layer(o.layers[i], this);
            var parent;
            if (layer.parentGroup != '') {
                parent = this.layerRoot.findGroup(layer.parentGroup);
            } else {
                parent = this.layerRoot;
            }
            parent.addLayer(layer, this.bLayersReversed);
            this.aLayers.push(layer);
        }
    },

    /**
     * Function: isMapLoaded
     * 
     * Returns true if the Map has been laoded succesfully form the server
     */
    isMapLoaded: function() {
        return this.bMapLoaded;
    },

    getScale : function() {
        return this.mapWidget.getScale();
    },
    
    updateLayer: function() {   //to be fleshed out, add query file to layer if selection, call this before draw
      if (this.hasSelection()) {
          this.oLayerOL.addOptions({queryfile: this._sQueryfile});
      }
    },
    
    drawMap: function() {
        if (!this.bMapLoaded) {
            return;
        }

        var params = { layers: this.aVisibleLayers.join(' '), ts : (new Date()).getTime()};
        if (this.hasSelection()) {
            params['queryfile']=this._sQueryfile;
        } else {
            params['queryfile'] = '';
        }
        this.oLayerOL.mergeNewParams(params);
    },
    
    showLayer: function( sLayer ) {
        this.aVisibleLayers.push(sLayer);
        this.drawMap();
    },

    hideLayer: function( sLayer ) {
        for (var i=0; i<this.aLayers.length; i++) {
            if (this.aVisibleLayers[i] == sLayer) {
                this.aVisibleLayers.splice(i,1);
                break;
            }
        }
        this.drawMap();
    },

    showGroup: function( sGroup ) {
      if (sGroup == 'layerRoot') {
        this.oLayerOL.setVisibility(true);
      } else {
        this.aVisibleGroups.push(sGroup);
        //TODO: manipulate aVisibleLayers based on layers in the group
        this.drawMap();
      }
    },

    hideGroup: function( sGroup ) {
      if (sGroup == 'layerRoot') {
        this.oLayerOL.setVisibility(false);
      } else {
        for (var i=0; i<this.aVisibleGroups.length; i++) {
            if (this.aVisibleGroups[i] == sGroup) {
                this.aVisibleGroups.splice(i,1);
                break;
            }
        }
        //TODO: manipulate aVisibleLayers based on layers in the group
        this.drawMap();
      }
    },

    refreshLayer: function( sLayer ) {
        this.drawMap();
    },

    hasSelection: function() { return this.bSelectionOn; },

    getSelectionCB : function(userFunc, layers, startend, r, json) {
      if (json) 
      {
          var o;
          eval("o="+r.responseText);

          var oSelection = new GxSelectionObject(o);
          userFunc(oSelection);
      }
    },
    
    /**
     * advertise a new selection is available and redraw the map
     */
    newSelection: function() {

        this.bSelectionOn = true;
        this.drawMap();
        this.triggerEvent(Fusion.Event.MAP_SELECTION_ON);
    },

    /**
     * Returns the number of features selected for this map layer
     */
    getSelectedFeatureCount : function() {
      var total = 0;
      for (var j=0; j<this.aLayers.length; ++j) {
        total += this.aLayers[j].selectedFeatureCount;
      }
      return total;
    },

    /**
     * Returns the number of features selected for this map layer
     */
    getSelectedLayers : function() {
      var layers = [];
      for (var j=0; j<this.aLayers.length; ++j) {
        if (this.aLayers[j].selectedFeatureCount>0) {
          layers.push(this.aLayers[j]);
        }
      }
      return layers;
    },

    /**
     * Returns the number of features selected for this map layer
     */
    getSelectableLayers : function() {
      var layers = [];
      for (var j=0; j<this.aLayers.length; ++j) {
        if (this.aLayers[j].selectable) {
          layers.push(this.aLayers[j]);
        }
      }
      return layers;
    },

    /**
     * asynchronously load the current selection.  When the current
     * selection changes, the selection is not loaded because it
     * could be a lengthy process.  The user-supplied function will
     * be called when the selection is available.
     *
     * @param userFunc {Function} a function to call when the
     *        selection has loaded
     * @param layers {string} Optional parameter.  A comma separated
     *        list of layer names (Roads,Parcels). If it is not
     *        given, all the layers that have a selection will be used  
     *
     * @param startcount {string} Optional parameter.  A comma separated
     *        list of a statinh index and the number of features to be retured for
     *        each layer given in the layers parameter. Index starts at 0
     *        (eg: 0:4,2:6 : return 4 elements for the first layers starting at index 0 and
     *         six elements for layer 2 starting at index 6). If it is not
     *        given, all the elemsnts will be returned.  
     */
    getSelection : function(userFunc, layers, startcount) {
        
        if (userFunc) 
        {
            var s = this.arch + '/' + Fusion.getScriptLanguage() + "/Selection." + Fusion.getScriptLanguage() ;
            var params = {parameters:'session='+this.getSessionID()+'&mapname='+ this._sMapname+ '&layers='+layers+'&startcount='+startcount+'&queryfile='+this._sQueryfile, 
                          onComplete: this.getSelectionCB.bind(this, userFunc, layers, startcount)};
            Fusion.ajaxRequest(s, params);
        }
        
    },

    /**
       Utility function to clear current selection
    */
    clearSelection : function() {
      if (!this.aLayers) return;
      
        //clear the selection count for the layers
        for (var j=0; j<this.aLayers.length; ++j) {
          this.aLayers[j].selectedFeatureCount = 0;
        }

        this.bSelectionOn = false;
        this._sQueryfile = "";
        this.triggerEvent(Fusion.Event.MAP_SELECTION_OFF);
        this.drawMap();
        this.oSelection = null;
    },


    /**
       Call back function when slect functions are called (eg queryRect)
    */
    processQueryResults : function(r, json) {
        this.mapWidget._removeWorker();
        if (json) {
            var o;
            eval("o="+r.responseText);
            if (!o.hasSelection) { 
                //this.drawMap();
                return;
            } else {
                this._sQueryfile = o.queryFile;
                for (var i=0; i<o.layers.length; ++i) {
                  var layerName = o.layers[i];
                  for (var j=0; j<this.aLayers.length; ++j) {
                    if (layerName == this.aLayers[j].layerName) {
                      this.aLayers[j].selectedFeatureCount = o[layerName].featureCount;
                    }
                  }
                }
                this.newSelection();
            }
        }
    },
    /**
       Do a query on the map
    */
    query : function(options) {
        this.mapWidget._addWorker();
        
        //clear the selection count for the layers
        for (var j=0; j<this.aLayers.length; ++j) {
          this.aLayers[j].selectedFeatureCount = 0;
        }

        var geometry = options.geometry || '';
        var maxFeatures = options.maxFeatures || -1;
        var bPersistant = options.persistent || true;
        var selectionType = options.selectionType || this.selectionType;
        var filter = options.filter ? '&filter='+options.filter : '';
        var layers = options.layers || '';
        /* if no layes are given, query only visible layers. This is ususally the most common case*/
        if (layers == '') {
          layers = this.aVisibleLayers.join(',');
        }
        var extend = options.extendSelection ? '&extendselection=true' : '';
        var computed = options.computedProperties ? '&computed=true' : '';

        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/Query.' + sl;

        var sessionid = this.getSessionID();

        var params = 'mapname='+this._sMapname+"&session="+sessionid+'&spatialfilter='+geometry+'&maxfeatures='+maxFeatures+filter+'&layers='+layers+'&variant='+selectionType+extend;
        var options = {onSuccess: this.processQueryResults.bind(this), 
                                     parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    loadStart: function() {
      this.mapWidget._addWorker();
    },
    
    loadEnd: function() {
      this.mapWidget._removeWorker();
    },
    
    pingServer: function() {
        var s = this.arch + '/' + Fusion.getScriptLanguage() + "/Common." + Fusion.getScriptLanguage() ;
        var params = {};
        params.parameters = 'session='+this.getSessionID();
        Fusion.ajaxRequest(s, params);
    }
};

    
/******************************************************************************
 * Class: Fusion.Maps.MapServer.Group
 *
 * Implements the map layer groups for MapServer CGI services
*/

Fusion.Maps.MapServer.Group = Class.create();
Fusion.Maps.MapServer.Group.prototype = {
    oMap: null,
    initialize: function(o, oMap) {
        this.uniqueId = o.uniqueId;
        Object.inheritFrom(this, Fusion.Widget.Map.Group.prototype, [this.uniqueId]);
        this.oMap = oMap;
        this.groupName = o.groupName;
        this.legendLabel = o.legendLabel;
        this.parentUniqueId = o.parentUniqueId;
        this.groupType = o.groupType;
        this.displayInLegend = o.displayInLegend;
        this.expandInLegend = o.expandInLegend;
        this.visible = o.visible;
        this.actuallyVisible = o.actuallyVisible;
    },
    
    clear: function() {
        Fusion.Widget.Map.Group.prototype.clear.apply(this, []);
        //this.oMap = null;
    },
     
    show: function() {
        this.oMap.showGroup(this.groupName);
        this.visible = true;
    },
    
    hide: function() {
        this.oMap.hideGroup(this.groupName);
        this.visible = false;
    },
    
    isVisible: function() {
        return this.visible;
    }
    
};

var MSLAYER_POINT_TYPE = 0;
var MSLAYER_LINE_TYPE = 1;
var MSLAYER_POLYGON_TYPE = 2;
var MSLAYER_SOLID_TYPE = 3;
var MSLAYER_RASTER_TYPE = 4;

/******************************************************************************
 * Class: Fusion.Maps.MapServer.Layer
 *
* Implements individual map legend layers for MapServer services
*/

Fusion.Maps.MapServer.Layer = Class.create();
Fusion.Maps.MapServer.Layer.prototype = {
    
    scaleRanges: null,
    
    oMap: null,
    
    initialize: function(o, oMap) {
        this.uniqueId = o.uniqueId;
        Object.inheritFrom(this, Fusion.Widget.Map.Layer.prototype, [this.uniqueId]);
        this.oMap = oMap;
        this.layerName = o.layerName;
        this.uniqueId = o.uniqueId;
        this.resourceId = o.resourceId;
        this.legendLabel = o.legendLabel;
        this.selectable = o.selectable;
        this.selectedFeatureCount = 0;
        this.layerTypes = [].concat(o.layerTypes);
        this.displayInLegend = o.displayInLegend;
        this.expandInLegend = o.expandInLegend;
        this.visible = o.visible;
        this.actuallyVisible = o.actuallyVisible;
        this.editable = o.editable;
        this.parentGroup = o.parentGroup;
        this.scaleRanges = [];
    		this.minScale = 1.0e10;
    		this.maxScale = 0;
        for (var i=0; i<o.scaleRanges.length; i++) {
            var scaleRange = new Fusion.Maps.MapServer.ScaleRange(o.scaleRanges[i], this.supportsType(4));
            this.scaleRanges.push(scaleRange);
      			this.minScale = Math.min(this.minScale, scaleRange.minScale);
      			this.maxScale = Math.max(this.maxScale, scaleRange.maxScale);
        }
    },
    
    clear: function() {
        Fusion.Widget.Map.Layer.prototype.clear.apply(this, []);
        this.oMap = null;
        this.legend = null;
    },
    
    supportsType: function(type) {
        for (var i=0; i<this.layerTypes.length; i++) {
            if (this.layerTypes[i] == type) {
                return true;
            }
        }
        return false;
    },
    
    getScaleRange: function(fScale) {
        for (var i=0; i<this.scaleRanges.length; i++) {
            if (this.scaleRanges[i].contains(fScale)) {
                return this.scaleRanges[i];
            }
        }
        return null;
    },

    show: function() {
        this.oMap.showLayer(this.layerName);
        this.set('visible', true);
    },

    hide: function() {
        this.oMap.hideLayer(this.layerName);
        this.set('visible',false);
    },

    isVisible: function() {
        return this.visible;
    }
};

/******************************************************************************
 * Class: Fusion.Maps.MapServer.ScaleRange
 *
* Implements a scale range object for MapServer services
*/

Fusion.Maps.MapServer.ScaleRange = Class.create();
Fusion.Maps.MapServer.ScaleRange.prototype = {
    styles: null,
    initialize: function(o, bRaster) {
        this.minScale = o.minScale;
        this.maxScale = o.maxScale;
        this.styles = [];
        if (!o.styles) {
            return;
        }
        
        /*special case : if there are no classes and it is a raster layer
          we set it to use the default static raster icon*/
        if (o.styles.length == 0 && bRaster)
        {
          var tmpsyle = {};
          tmpsyle.legendLabel = "raster";
          tmpsyle.filter = "";
          tmpsyle.index = 0;
          tmpsyle.staticIcon = true;
          var styleItem = new Fusion.Maps.MapServer.StyleItem(tmpsyle, tmpsyle.staticIcon);
          this.styles.push(styleItem);
        }    
        else
        {
          var staticIcon = o.styles.length>=1 ? false : bRaster;
          for (var i=0; i<o.styles.length; i++) {
            var styleItem = new Fusion.Maps.MapServer.StyleItem(o.styles[i], staticIcon);
            this.styles.push(styleItem);
          }
        }
    },
    contains: function(fScale) {
        return fScale >= this.minScale && fScale <= this.maxScale;
    }
};

/******************************************************************************
 * Class: Fusion.Maps.MapServer.StyleItem
 *
* Implements the legend style items to get a legend icon from the server
*/

Fusion.Maps.MapServer.StyleItem = Class.create();
Fusion.Maps.MapServer.StyleItem.prototype = {
    initialize: function(o, staticIcon) {
        this.legendLabel = o.legendLabel;
        this.filter = o.filter;
        this.index = o.index;
        this.staticIcon = staticIcon;
    },
    getLegendImageURL: function(fScale, layer) {
        var sl = Fusion.getScriptLanguage();
        var url = Fusion.getFusionURL() + '/' + layer.oMap.arch + '/' + sl  + '/LegendIcon.' + sl;
        var sessionid = layer.oMap.getSessionID();
        var params = 'mapname='+layer.oMap._sMapname+"&session="+sessionid + '&layername='+layer.resourceId + '&classindex='+this.index;
        return url + '?'+params;
    }
};
