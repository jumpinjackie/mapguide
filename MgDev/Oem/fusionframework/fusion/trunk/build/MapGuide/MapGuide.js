/**
 * Fusion.Maps.MapGuide
 *
 * $Id: MapGuide.js 1233 2008-02-15 19:30:53Z madair $
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

/***************************************************************************
* Class: Fusion.Maps.MapGuide
*
* Implements the map widget for MapGuide Open Source services.
*/

Fusion.Maps.MapGuide = Class.create();
Fusion.Maps.MapGuide.prototype = {
    arch: 'MapGuide',
    session: [null],
    bSingleTile: null,
    aShowLayers: null,
    aHideLayers: null,
    aShowGroups: null,
    aHideGroups: null,
    aRefreshLayers: null,
    sActiveLayer: null,
    selectionType: 'INTERSECTS',
    bSelectionOn: false,
    oSelection: null,
    bDisplayInLegend: true,   //TODO: set this in AppDef?
    bExpandInLegend: true,   //TODO: set this in AppDef?
    bMapLoaded : false,
    bIsMapWidgetLayer : true,  //Setthis to false for overview map layers
    bLayersReversed: false,     //MGOS returns layers top-most layer first

    //the resource id of the current MapDefinition
    _sResourceId: null,
    
    initialize : function(map, mapTag, isMapWidgetLayer) {
        // console.log('MapGuide.initialize');
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
                
        this.registerEventID(Fusion.Event.MAP_SESSION_CREATED);
        this.registerEventID(Fusion.Event.MAP_SELECTION_ON);
        this.registerEventID(Fusion.Event.MAP_SELECTION_OFF);
        this.registerEventID(Fusion.Event.MAP_LOADED);
        this.registerEventID(Fusion.Event.MAP_LOADING);

        this.mapWidget = map;
        this.oSelection = null;
        if (isMapWidgetLayer != null) {
            this.bIsMapWidgetLayer = isMapWidgetLayer;
        }
        
        var extension = mapTag.extension; //TBD: this belongs in layer tag?
        this.selectionType = extension.SelectionType ? extension.SelectionType[0] : 'INTERSECTS';
        this.ratio = extension.MapRatio ? extension.MapRatio[0] : 1.0;
        
        this.sMapResourceId = mapTag.resourceId ? mapTag.resourceId : '';
        
        rootOpts = {
          displayInLegend: this.bDisplayInLegend,
          expandInLegend: this.bExpandInLegend,
          legendLabel: this._sMapname,
          groupName: 'layerRoot'
          //TODO: set other opts for group initialization as required
        };
        this.layerRoot = new Fusion.Maps.MapGuide.Group(rootOpts,this);
        
        this.bSingleTile = mapTag.singleTile; //this is set in thhe AppDef.Map class

        this.keepAliveInterval = parseInt(extension.KeepAliveInterval ? extension.KeepAliveInterval[0] : 300);
        
        var sid = Fusion.sessionId;
        if (sid) {
            this.session[0] = sid;
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
        if (this.session[0] instanceof Fusion.Maps.MapGuide) {
            // console.log('register for event');
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
        if (this.sMapResourceId != '') {
            this.loadMap(this.sMapResourceId);
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
    
    loadMap: function(resourceId, options) {
        this.bMapLoaded = false;

        /* don't do anything if the map is already loaded? */
        if (this._sResourceId == resourceId) {
            return;
        }

        if (!this.sessionReady()) {
            this.sMapResourceId = resourceId;
            return;
        }
        
        if (this.bIsMapWidgetLayer) {
            this.mapWidget.triggerEvent(Fusion.Event.MAP_LOADING);
        } else {
          this.triggerEvent(Fusion.Event.MAP_LOADING);
        }
        this.mapWidget._addWorker();
        
        this._fScale = -1;
        this._nDpi = 96;
        
        options = options || {};
        
        this._oMaxExtent = null;
        this.aShowLayers = options.showlayers || [];
        this.aHideLayers = options.hidelayers || [];
        this.aShowGroups = options.showgroups || [];
        this.aHideGroups = options.hidegroups || [];
        this.aRefreshLayers = options.refreshlayers || [];
        this.aLayers = [];

        this.oSelection = null;
        this.aSelectionCallbacks = [];
        this._bSelectionIsLoading = false;

        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/LoadMap.' + sl;
        
        var sessionid = this.getSessionID();
        
        var params = 'mapid='+resourceId+"&session="+sessionid;
        var options = {onSuccess: this.mapLoaded.bind(this), parameters:params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    mapLoaded: function(r, json) {
        if (json) {
            var o;
            eval('o='+r.responseText);
            this._sResourceId = o.mapId;
            this._sMapname = o.mapName;
            this._sMapTitle = o.mapTitle;
            this._fMetersperunit = o.metersPerUnit;
            this.mapWidget._fMetersperunit = this._fMetersperunit;

            this._oMaxExtent = OpenLayers.Bounds.fromArray(o.extent); 

            this.layerRoot.clear();
            this.layerRoot.legendLabel = this._sMapTitle;
            
            this.parseMapLayersAndGroups(o);
            
            this.minScale = 1.0e10;
            this.maxScale = 0;
            for (var i=0; i<this.aLayers.length; i++) {
              this.minScale = Math.min(this.minScale, this.aLayers[i].minScale);
              this.maxScale = Math.max(this.maxScale, this.aLayers[i].maxScale);
            }
            //a scale value of 0 is undefined
            if (this.minScale <= 0) {
              this.minScale = 1.0;
            }
            
            for (var i=0; i<this.aShowLayers.length; i++) {
                var layer =  this.layerRoot.findLayerByAttribute('layerName', this.aShowLayers[i]);
                if (layer) {
                    this.aShowLayers[i] = layer.uniqueId;
                } else {
                    this.aShowLayers[i] = '';
                }
            }
            for (var i=0; i<this.aHideLayers.length; i++) {
                var layer =  this.layerRoot.findLayerByAttribute('layerName', this.aHideLayers[i]);
                if (layer) {
                    this.aHideLayers[i] = layer.uniqueId;
                } else {
                    this.aHideLayers[i] = '';
                }
            }
            
            for (var i=0; i<this.aShowGroups.length; i++) {
                var group =  this.layerRoot.findGroupByAttribute('groupName', this.aShowGroups[i]);
                if (group) {
                    this.aShowGroups[i] = group.uniqueId;
                } else {
                    this.aShowGroups[i] = '';
                }
            }
            
            for (var i=0; i<this.aHideGroups.length; i++) {
                var group =  this.layerRoot.findGroupByAttribute('groupName', this.aHideGroups[i]);
                if (group) {
                    this.aHideGroups[i] = group.uniqueId;
                } else {
                    this.aHideGroups[i] = '';
                }
            }
            
            if (!this.bSingleTile) {
              if (o.groups.length >0) {
                this.bSingleTile = false;
                this.groupName = o.groups[0].groupName  //assumes only one group for now
              } else {
                this.bSingleTile = true;
              }
            }

            //TODO: get this from the layerTag.extension
            //this.oMapInfo = Fusion.oConfigMgr.getMapInfo(this._sResourceId);

            //set projection units and code if supplied
            //TODO: consider passing the metersPerUnit value into the framework
            //to allow for scaling that doesn't match any of the pre-canned units
            this.units = this.getClosestUnits(o.metersPerUnit);
            
            //add in scales array if supplied
            if (o.FiniteDisplayScales && o.FiniteDisplayScales.length>0) {
              this.scales = o.FiniteDisplayScales;
            }
            
            //remove this layer if it was already created
            if (this.oLayerOL) {
                this.oLayerOL.events.unregister("loadstart", this, this.loadStart);
                this.oLayerOL.events.unregister("loadend", this, this.loadEnd);
                this.oLayerOL.events.unregister("loadcancel", this, this.loadEnd);
                this.oLayerOL.destroy();
            }

            this.oLayerOL = this.createOLLayer(this._sMapname, true, this.bSingleTile);
            this.oLayerOL.events.register("loadstart", this, this.loadStart);
            this.oLayerOL.events.register("loadend", this, this.loadEnd);
            this.oLayerOL.events.register("loadcancel", this, this.loadEnd);
            
            //this is to distinguish between a regular map and an overview map
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
        } else {
            Fusion.reportError( new Fusion.Error(Fusion.Error.FATAL, 
                OpenLayers.String.translate('mapLoadError', r.responseText)));
        }
        this.mapWidget._removeWorker();
    },
    
    getClosestUnits: function(metrsPerUnit) {
        
        var units = "degrees";
        var minDiff = 100000000;
        for (var key in OpenLayers.INCHES_PER_UNIT)        
        {
            var newDiff = Math.abs((metrsPerUnit * 39.3701) - OpenLayers.INCHES_PER_UNIT[key]);
            if(newDiff < minDiff)
            {
                minDiff = newDiff;
                units = key;
            }
        }
        return units;
    },

//TBD: this function not yet converted for OL    
    reloadMap: function() {
        
        this.mapWidget._addWorker();
        //console.log('loadMap: ' + resourceId);
        this.aShowLayers = [];
        this.aHideLayers = [];
        this.aShowGroups = [];
        this.aHideGroups = [];
        this.aRefreshLayers = [];
        this.layerRoot.clear();
        this.oldLayers = this.aLayers.clone();
        this.aLayers = [];
        
        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/LoadMap.' + sl;
        
        var sessionid = this.getSessionID();
        
        var params = 'mapname='+this._sMapname+"&session="+sessionid;
        var options = {onSuccess: this.mapReloaded.bind(this), 
                      onException: this.reloadFailed.bind(this),
                      parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
        
        
    },

    reloadFailed: function(r) {
      Fusion.reportError( new Fusion.Error(Fusion.Error.FATAL, 
        OpenLayers.String.translate('mapLoadError', r.transport.responseText)));
      this.mapWidget._removeWorker();
    },

//TBD: this function not yet converted for OL    
    mapReloaded: function(r,json) {
        if (json) {
            var o;
            eval('o='+r.responseText);
            this.parseMapLayersAndGroups(o);
            for (var i=0; i<this.aLayers.length; ++i) {
              var newLayer = this.aLayers[i];
              for (var j=0; j<this.oldLayers.length; ++j){
                if (this.oldLayers[j].uniqueId == newLayer.uniqueId) {
                  newLayer.selectedFeatureCount = this.oldLayers[j].selectedFeatureCount;
                  break;
                }
              }
            }
            this.oldLayers = null;
            this.mapWidget.triggerEvent(Fusion.Event.MAP_RELOADED);
            this.drawMap();
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
            
                this.drawMap();
                this.triggerEvent(Fusion.Event.MAP_LAYER_ORDER_CHANGED);
            } else {
                alert(OpenLayers.String.translate('setLayersError', o.layerindex));
            }
        }
    },
            
    parseMapLayersAndGroups: function(o) {
        for (var i=0; i<o.groups.length; i++) {
            var group = new Fusion.Maps.MapGuide.Group(o.groups[i], this);
            var parent;
            if (group.parentUniqueId != '') {
                parent = this.layerRoot.findGroupByAttribute('uniqueId', group.parentUniqueId);
            } else {
                parent = this.layerRoot;
            }
            parent.addGroup(group, this.bLayersReversed);
        }

        for (var i=0; i<o.layers.length; i++) {
            var layer = new Fusion.Maps.MapGuide.Layer(o.layers[i], this);
            var parent;
            if (layer.parentGroup != '') {
                parent = this.layerRoot.findGroupByAttribute('uniqueId', layer.parentGroup);
            } else {
                parent = this.layerRoot;
            }
            parent.addLayer(layer, this.bLayersReversed);
            this.aLayers.push(layer);
        }
    },
    
    drawMap: function() {
        if (!this.bMapLoaded) {
            return;
        }
        
        var options = {
          showLayers : this.aShowLayers.length > 0 ? this.aShowLayers.toString() : null,
          hideLayers : this.aHideLayers.length > 0 ? this.aHideLayers.toString() : null,
          showGroups : this.aShowGroups.length > 0 ? this.aShowGroups.toString() : null,
          hideGroups : this.aHideGroups.length > 0 ? this.aHideGroups.toString() : null,
          refreshLayers : this.aRefreshLayers.length > 0 ? this.aRefreshLayers.toString() : null
        };
        this.aShowLayers = [];
        this.aHideLayers = [];
        this.aShowGroups = [];
        this.aHideGroups = [];
        this.aRefreshLayers = [];

        this.oLayerOL.addOptions(options);
        this.oLayerOL.mergeNewParams({ts : (new Date()).getTime()});
        if (this.queryLayer) this.queryLayer.redraw();
    },

    /**
     * Function: createOLLayer
     * 
     * Returns an OpenLayers MapGuide layer object
     */
    createOLLayer: function(layerName, bIsBaseLayer, bSingleTile) {
      var layerOptions = {
        units : this.units,
        isBaseLayer : bIsBaseLayer,
        maxExtent : this._oMaxExtent,
        maxResolution : 'auto',
        ratio : this.ratio
      };

      //add in scales array if supplied
      if (this.scales && this.scales.length>0) {
        layerOptions.scales = this.scales;
      }
      if (this.maxScale != Infinity) {
        layerOptions.minScale = this.maxScale;    //OL interpretation of min/max scale is reversed from Fusion
      }
      layerOptions.maxScale = this.minScale;

      layerOptions.singleTile = bSingleTile;   
      
      var params = {};
      if ( bSingleTile ) {
        params = {        //single tile params
          session : this.getSessionID(),
          mapname : this._sMapname
        };
        layerOptions.showLayers = this.aShowLayers.length > 0 ? this.aShowLayers.toString() : null;
        layerOptions.hideLayers = this.aHideLayers.length > 0 ? this.aHideLayers.toString() : null;
        layerOptions.showGroups = this.aShowGroups.length > 0 ? this.aShowGroups.toString() : null;
        layerOptions.hideGroups = this.aHideGroups.length > 0 ? this.aHideGroups.toString() : null;
        layerOptions.refreshLayers = this.aRefreshLayers.length > 0 ? this.aRefreshLayers.toString() : null;

      } else {
        params = {      //tiled version
          mapdefinition: this._sResourceId,
          basemaplayergroupname: this.groupName  //assumes only one group for now
        };
      }

      var url = Fusion.getConfigurationItem('mapguide', 'mapAgentUrl');
      var oLayerOL = new OpenLayers.Layer.MapGuide( layerName, url, params, layerOptions );
      return oLayerOL;
    },
            
    /**
     * Function: getLayerByName
     * 
     * Returns the MapGuide layer object as identified by the layer name
     */
    getLayerByName : function(name)
    {
        var oLayer = null;
        for (var i=0; i<this.aLayers.length; i++)
        {
            if (this.aLayers[i].layerName == name)
            {
                oLayer = this.aLayers[i];
                break;
            }
        }
        return oLayer;
    },

    /**
     * Function: isMapLoaded
     * 
     * Returns true if the Map has been laoded succesfully form the server
     */
    isMapLoaded: function() {
        return this.bMapLoaded;
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
        if (this.oSelection) {
            this.oSelection = null;
        }
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
      * Function: zoomToSelection
      *
      * sets a Selection XML back to the server
      */
    zoomToSelection: function(r) {
      var xmlDoc = r.responseXML.documentElement;
      var x = xmlDoc.getElementsByTagName('X');
      var y = xmlDoc.getElementsByTagName('Y');
      //double the veiwport
      var extent = new OpenLayers.Bounds(x[0].firstChild.nodeValue,y[0].firstChild.nodeValue,x[1].firstChild.nodeValue,y[1].firstChild.nodeValue);
      var center = extent.getCenterPixel();
      var size = extent.getSize();
      extent.left = center.x - 2*size.w;
      extent.right = center.x + 2*size.w;
      extent.bottom = center.y - 2*size.h;
      extent.top = center.y + 2*size.h;
      this.mapWidget.setExtents(extent);
    },  

    processSelection: function(sel, requery, zoomTo, json) {
      if (requery) {
        //xmlDoc = (new DOMParser()).parseFromString(r.responseXML, "text/xml");
        //this.processFeatureInfo(xmlDoc.documentElement, false, 1);
        //this.processFeatureInfo(xmlOut, false, 2);
      }
      this.newSelection();
      if (zoomTo) {
        var mgRequest = new Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope(this.getSessionID(), this.getMapName(), sel );
        Fusion.oBroker.dispatchRequest(mgRequest, this.zoomToSelection.bind(this));
      } else {
        this.mapWidget.redraw();
      }
    },

    setSelection: function (selText, requery, zoomTo) {
      var sl = Fusion.getScriptLanguage();
      var setSelectionScript = this.arch + '/' + sl  + '/SetSelection.' + sl;
      var params = 'mapname='+this.getMapName()+"&session="+this.getSessionID();
      params += '&selection=' + encodeURIComponent(selText);
      params += '&queryinfo=' + (requery? "1": "0");
      params += '&seq=' + Math.random();
      var options = {onSuccess: this.processSelection.bind(this, selText, requery, zoomTo), parameters:params, asynchronous:false};
      Fusion.ajaxRequest(setSelectionScript, options);
    },


     /**
     * asynchronously load the current selection.  When the current
     * selection changes, the selection is not loaded because it
     * could be a lengthy process.  The user-supplied function will
     * be called when the selection is available.
     *
     * @param userFunc {Function} a function to call when the
     *        selection has loaded
     *
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

      /*for now always go back to server to fetch selection */
       
      if (userFunc) 
      {
          //this.aSelectionCallbacks.push(userFunc);
      
      
          //this.mapWidget._addWorker();
          // this._bSelectionIsLoading = true;
          var s = this.arch + '/' + Fusion.getScriptLanguage() + "/Selection." + Fusion.getScriptLanguage() ;
          var params = {parameters:'session='+this.getSessionID()+'&mapname='+ this._sMapname +'&layers='+layers+'&startcount='+startcount, 
                        onComplete: this.getSelectionCB.bind(this, userFunc, layers, startcount)};
          Fusion.ajaxRequest(s, params);
      }
    },

    /**
       Call back function when selection is cleared
    */
    selectionCleared : function()
    {
        //clear the selection count for the layers
        for (var j=0; j<this.aLayers.length; ++j) {
          this.aLayers[j].selectedFeatureCount = 0;
        }

        this.bSelectionOn = false;
        if (this.queryLayer) {
          this.queryLayer.setVisibility(false);
        }
        this.triggerEvent(Fusion.Event.MAP_SELECTION_OFF);
        this.drawMap();
        this.oSelection = null;
    },

    /**
       Utility function to clear current selection
    */
    clearSelection : function() {
      if (this.hasSelection()) {
        var s = this.arch + '/' + Fusion.getScriptLanguage() + "/ClearSelection." + Fusion.getScriptLanguage() ;
        var params = {parameters:'session='+this.getSessionID()+'&mapname='+ this._sMapname, onComplete: this.selectionCleared.bind(this)};
        Fusion.ajaxRequest(s, params);
      }
    },

    /**
       removes the queryLayer from the map
    */
    removeQueryLayer : function() {
      if (this.queryLayer) {
        this.queryLayer.destroy();
        this.queryLayer = null;
      }
    },


    /**
       Call back function when slect functions are called (eg queryRect)
    */
    processQueryResults : function(r) {
        this.mapWidget._removeWorker();
        if (r.responseText) {   //TODO: make the equivalent change to MapServer.js
            var oNode;
            eval('oNode='+r.responseText);
            
            if (oNode.hasSelection) {
              if (!this.bSingleTile) {
                if (!this.queryLayer) {
                  this.queryLayer = this.createOLLayer("query layer", false, true);
                  this.mapWidget.oMapOL.addLayer(this.queryLayer);
                  this.mapWidget.registerForEvent(Fusion.Event.MAP_LOADING, this.removeQueryLayer.bind(this));
                } else {
                  this.queryLayer.setVisibility(true);
                }
              }

              // set the feature count on each layer making up this map
              for (var i=0; i<oNode.layers.length; ++i) {
                var layerName = oNode.layers[i];
                for (var j=0; j<this.aLayers.length; ++j) {
                  if (layerName == this.aLayers[j].layerName) {
                    this.aLayers[j].selectedFeatureCount = oNode[layerName].featureCount;
                  }
                }
              }
              this.newSelection();
            } else {
              //this.drawMap();
              return;
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
        var maxFeatures = options.maxFeatures || 0; //zero means select all features
        var bPersistant = options.persistent || true;
        var selectionType = options.selectionType || this.selectionType;
        var filter = options.filter ? '&filter='+options.filter : '';
        var layers = options.layers || '';
        var extend = options.extendSelection ? '&extendselection=true' : '';
        var computed = options.computedProperties ? '&computed=true' : '';
        var sl = Fusion.getScriptLanguage();
        var loadmapScript = this.arch + '/' + sl  + '/Query.' + sl;

        var sessionid = this.getSessionID();

        var params = 'mapname='+this._sMapname+"&session="+sessionid+'&spatialfilter='+geometry+'&maxfeatures='+maxFeatures+filter+'&layers='+layers+'&variant='+selectionType+extend+computed;
        var options = {onSuccess: this.processQueryResults.bind(this), 
                                     parameters: params};
        Fusion.ajaxRequest(loadmapScript, options);
    },
    
    showLayer: function( layer ) {
        if (this.oMapInfo && this.oMapInfo.layerEvents[layer.layerName]) {
            var layerEvent = this.oMapInfo.layerEvents[layer.layerName];
            for (var i=0; i<layerEvent.onEnable.length; i++) {
                var l = this.layerRoot.findLayer(layerEvent.onEnable[i].name);
                if (l) {
                    if (layerEvent.onEnable[i].enable) {
                        l.show();
                    } else {
                        l.hide();
                    }
                }
            }
        }
        this.aShowLayers.push(layer.uniqueId);
        this.drawMap();
    },
    
    hideLayer: function( layer ) {
        if (this.oMapInfo && this.oMapInfo.layerEvents[layer.layerName]) {
            var layerEvent = this.oMapInfo.layerEvents[layer.layerName];
            for (var i=0; i<layerEvent.onDisable.length; i++) {
                var l = this.layerRoot.findLayer(layerEvent.onDisable[i].name);
                if (l) {
                    if (layerEvent.onDisable[i].enable) {
                        l.show();
                    } else {
                        l.hide();
                    }
                }
            }
        }        
        this.aHideLayers.push(layer.uniqueId);
        this.drawMap();
    },
    showGroup: function( group ) {
      if (group.groupName == 'layerRoot') {
        this.oLayerOL.setVisibility(true);
      } else {
        this.aShowGroups.push(group.uniqueId);
        this.drawMap();
      }
    },
    hideGroup: function( group ) {
      if (group.groupName == 'layerRoot') {
        this.oLayerOL.setVisibility(false);
      } else {
        this.aHideGroups.push(group.uniqueId);
        this.drawMap();
      }
    },
    refreshLayer: function( layer ) {
        this.aRefreshLayers.push(layer.uniqueId);        
        this.drawMap();
    },
    setParameter : function(param, value) {
        if (param == 'SelectionType') {
            this.selectionType = value;
        }
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
    
/***************************************************************************
* Class: Fusion.Maps.MapGuide.Group
*
* Implements the map layer groups for MapGuide services
*/

Fusion.Maps.MapGuide.Group = Class.create();
Fusion.Maps.MapGuide.Group.prototype = {
    oMap: null,
    initialize: function(o, oMap) {
        this.uniqueId = o.uniqueId;
        Object.inheritFrom(this, Fusion.Widget.Map.Group.prototype, [o.groupName]);
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
    
    show: function() {
        if (this.visible) {
            return;
        }
        this.oMap.showGroup(this);
        this.visible = true;
        if (this.legend && this.legend.checkBox) {
            this.legend.checkBox.checked = true;
        }
    },
    
    hide: function() {
        if (!this.visible) {
            return;
        }
        this.oMap.hideGroup(this);
        this.visible = false;
        if (this.legend && this.legend.checkBox) {
            this.legend.checkBox.checked = false;
        }
    },
    
    isVisible: function() {
        return this.visible;
    }
};

/***************************************************************************
* Class: Fusion.Maps.MapGuide
*
* Implements individual map legend layers for MapGuide services
*/

Fusion.Maps.MapGuide.Layer = Class.create();
Fusion.Maps.MapGuide.Layer.prototype = {
    
    scaleRanges: null,
    oMap: null,
    
    initialize: function(o, oMap) {
        this.uniqueId = o.uniqueId;
        Object.inheritFrom(this, Fusion.Widget.Map.Layer.prototype, [o.layerName]);
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
        
        //determine the layer type so that the correct icon can be displayed in the legend
        this.layerType = null;
        if (this.supportsType(Fusion.Constant.LAYER_RASTER_TYPE)) {   //raster layers
          this.layerType = Fusion.Constant.LAYER_RASTER_TYPE;
        } else if (this.supportsType(Fusion.Constant.LAYER_DWF_TYPE)) {  //DWF layers
          this.layerType = Fusion.Constant.LAYER_DWF_TYPE;
        }
        
        this.parentGroup = o.parentGroup;
        this.scaleRanges = [];
        this.minScale = 1.0e10;
        this.maxScale = 0;
        for (var i=0; i<o.scaleRanges.length; i++) {
          var scaleRange = new Fusion.Maps.MapGuide.ScaleRange(o.scaleRanges[i], 
                                this.layerType);
          this.scaleRanges.push(scaleRange);
          this.minScale = Math.min(this.minScale, scaleRange.minScale);
          this.maxScale = Math.max(this.maxScale, scaleRange.maxScale);
        }
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
        if (this.visible) {
            return;
        }
        this.oMap.showLayer(this);
        this.set('visible', true);
        if (this.legend && this.legend.checkBox) {
            this.legend.checkBox.checked = true;
        }
    },

    hide: function() {
        if (!this.visible) {
            return;
        }
        this.oMap.hideLayer(this);
        this.set('visible',false);
        if (this.legend && this.legend.checkBox) {
            this.legend.checkBox.checked = false;
        }
    },

    isVisible: function() {
        return this.visible;
    }
};

/***************************************************************************
* Class: Fusion.Maps.MapGuide
*
* Implements a scale range object for MapGuide services
*/

Fusion.Maps.MapGuide.ScaleRange = Class.create();
Fusion.Maps.MapGuide.ScaleRange.prototype = {
    styles: null,
    initialize: function(o, layerType) {
        this.minScale = o.minScale;
        this.maxScale = o.maxScale;
        if (this.maxScale == 'infinity') {
          this.maxScale = Infinity;
        }
        this.styles = [];
        if (!o.styles) {
          var styleItem = new Fusion.Maps.MapGuide.StyleItem({legendLabel:'DWF'}, layerType);
          this.styles.push(styleItem);
          return;
        }
        var staticIcon = o.styles.length>1 ? false : layerType;
        for (var i=0; i<o.styles.length; i++) {
            var styleItem = new Fusion.Maps.MapGuide.StyleItem(o.styles[i], staticIcon);
            this.styles.push(styleItem);
        }
    },
    contains: function(fScale) {
        return fScale >= this.minScale && fScale <= this.maxScale;
    }
};

/***************************************************************************
* Class: Fusion.Maps.MapGuide
*
* Implements the legend style items to get a legend icon from the server
*/

Fusion.Maps.MapGuide.StyleItem = Class.create();
Fusion.Maps.MapGuide.StyleItem.prototype = {
    initialize: function(o, staticIcon) {
        this.legendLabel = o.legendLabel;
        this.filter = o.filter;
        this.geometryType = o.geometryType;
        if (this.geometryType == '') {
            this.geometryType = -1;
        }
        this.categoryIndex = o.categoryIndex;
        if (this.categoryindex == '') {
            this.categoryindex = -1;
        }
        this.staticIcon = staticIcon;
    },
    getLegendImageURL: function(fScale, layer) {
        var url = Fusion.getConfigurationItem('mapguide', 'mapAgentUrl');
        return url + "OPERATION=GETLEGENDIMAGE&SESSION=" + layer.oMap.getSessionID() + "&VERSION=1.0.0&SCALE=" + fScale + "&LAYERDEFINITION=" + encodeURIComponent(layer.resourceId) + "&THEMECATEGORY=" + this.categoryIndex + "&TYPE=" + this.geometryType;
    }
};
