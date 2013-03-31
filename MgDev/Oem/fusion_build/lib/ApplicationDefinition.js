/**
 * Fusion.Lib.ApplicationDefinition
 *
 * $Id: ApplicationDefinition.js 2567 2012-08-08 15:30:54Z jng $
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
 * Class: Fusion.Lib.ApplicationDefinition
 *
 * Utility class to parse an application definition
 *
 */

Fusion.Lib.ApplicationDefinition = OpenLayers.Class({
    /**
     * Property: mapGroups
     *
     * array of map groups, parsed from ApplicationDefinition.  A MapGroup
     * consists of one or more Maps that can be combined into the same
     * OpenLayers Map object
     */
    mapGroups: null,

    /**
     * Property: widgetSets
     *
     * array of widget sets (each one corresponding to a map) parsed
     * from the ApplicationDefinition.
     */
    widgetSets: null,

    /**
     * Property: {Object} oBroker
     *
     * A Broker object that can communicate with a MapGuide instance
     * in the case we are running against a MapGuide server
     */
    oBroker: null,

    /**
     * Property: {Array} searchDefinitions
     *
     * An array of search definitions
     */
    searchDefinitions: null,

    /**
     * Property: {Array} searchCategories
     *
     * An array of search categories
     */
    searchCategories: null,

    /**
     * Constructor: ApplicationDefinition
     *
     * construct a new instance of the ApplicationDefinition object.  While
     * not enforced, this is intended to be a singleton.
     *
     * Parameter: sessionId
     *
     * an optional session id to initialize the application with, passed to
     * the map widgets when they are created.
     */

    initialize: function(sessionId) {
        //console.log('ApplicationDefinition initialize');
        this.sessionId = sessionId;
        this.oBroker = Fusion.getBroker();
        this.applicationDefinition =  Fusion.getApplicationDefinitionURL();

        this.widgetSets = [];
        this.mapGroups = {};
        this.searchDefinitions = [];
        this.searchCategories = [];
        this.parse();
    },

    /**
     * Function: parse
     *
     * start parsing the ApplicationDefinition file.  This happens
     * asynchronously since the ApplicationDefinition has to be
     * retrieved from the server or the MapGuide repository.  When
     * parsing is complete, an APPLICATIONDEFINITION_PARSED event
     * will be emitted.  This function returns true if parsing
     * will start, false if it will not (due to a missing
     * application definition for instance).
     */
    parse : function() {
        if (this.applicationDefinition == '') {
            //TODO: emit an error
            return null;
        }
        /* if the application definition is not in the mapguide server,
           just load the xml*/

        if ( (this.applicationDefinition.match('Library://') == null) &&
             (this.applicationDefinition.match('Session:') == null) ) {
            if (Fusion.appDefJson) {
                this.parseAppDef(Fusion.appDefJson);
            } else {
                Fusion.getXmlAsJson(this.applicationDefinition,
                              OpenLayers.Function.bind(this.getAppDefCB, this));
            }
        } else {
          //these need to be called via setTimeout so that the execution thread
          //is broken in IE and so that the ApplicationDefinition constructor
          //will return a valid object to Fusion in loadConfig before the loadState
          //gets incremented.
            if (!this.sessionId) {
              window.setTimeout(OpenLayers.Function.bind(this.createSessionThenGetAppDef,this),5);
            } else {
              window.setTimeout(OpenLayers.Function.bind(this.getAppDef,this),5);
            }
        }
        return true;
    },

    createSessionThenGetAppDef: function() {
      var sl = Fusion.getScriptLanguage();
      var scriptURL = 'layers/' + 'MapGuide' + '/' + sl + '/CreateSession.' + sl;
      var options = {onSuccess: OpenLayers.Function.bind(this.createSessionThenGetAppDefCB, this)};
      Fusion.ajaxRequest(scriptURL, options);
    },

    createSessionThenGetAppDefCB : function(xhr) {
      if (xhr && typeof(xhr) == "object" && xhr.responseText) {
        var o = Fusion.parseJSON(xhr.responseText);
        this.sessionId = o.sessionId;
        Fusion.sessionId = this.sessionId;
      }
      this.getAppDef();
    },

    getAppDef: function(){
      var r = new Fusion.Lib.MGRequest.MGGetResourceContent(this.applicationDefinition);
      r.parameters.session = this.sessionId;
      r.parameters.format = 'application/json';
      this.oBroker.dispatchRequest(r,
              OpenLayers.Function.bind(this.getAppDefCB, this));
    },

    getAppDefCB: function(xhr) {
        var o = Fusion.parseJSON(xhr.responseText);
        this.parseAppDef(o);
        Fusion.setLoadState(Fusion.LOAD_WIDGETS);
    },

    /**
     * Function: parseAppDef
     *
     * parse the ApplicationDefinition file into the appropriate Fusion objects
     *
     * Parameter: {XmlHttpRequest} xhr
     *
     * the XmlHttpRequest object
     */
    parseAppDef: function(json) {
        var appDef = json.ApplicationDefinition;

        /* Set the application title */
        if (appDef.Title) {
            var title = appDef.Title[0];
            document.title = title;
        }

        /* process Map nodes */
        if (appDef.MapSet) {
            var mapSet = appDef.MapSet[0];
            if (mapSet.MapGroup instanceof Array) {
                for (var i=0; i<mapSet.MapGroup.length; i++) {
                    var mapGroup = new Fusion.Lib.ApplicationDefinition.MapGroup(mapSet.MapGroup[i]);
                    this.mapGroups[mapGroup.mapId] = mapGroup;
                }
            }
        } else {
          Fusion.reportError(new Fusion.Error(Fusion.Error.FATAL,
                        OpenLayers.i18n('appDefParseError')));
        }

        /* process WIDGET sets */
        if (appDef.WidgetSet) {
            for (var i=0; i<appDef.WidgetSet.length; i++) {
                var widgetSet = new Fusion.Lib.ApplicationDefinition.WidgetSet(appDef.WidgetSet[i]);
                this.widgetSets.push(widgetSet);
            }
        } else {
          Fusion.reportError(new Fusion.Error(Fusion.Error.FATAL,
                      OpenLayers.i18n('widgetSetParseError')));
        }

        /* process extensions */
        if (appDef.Extension) {
            var extension = appDef.Extension[0];
            /* process search definitions */
            if (extension.SearchDefinitions instanceof Array) {
                var categories = extension.SearchDefinitions[0];
                if (categories.SearchCategory instanceof Array) {
                    for (var i=0; i<categories.SearchCategory.length; i++) {
                        var oCategory = {};
                        var category = categories.SearchCategory[i];
                        oCategory.id = category['@id'];
                        oCategory.name = category['@name'];
                        oCategory.layer = category.Layer ? category.Layer[0] : '';
                        oCategory.searchDefinitions = [];
                        this.searchCategories[oCategory.id] = oCategory;
                        var defns = category.SearchDefinition;
                        for (var k=0; k<defns.length; k++) {
                            var defn = new Fusion.Lib.ApplicationDefinition.SearchDefinition(defns[k]);
                            defn.category = oCategory;
                            oCategory.searchDefinitions[defn.id] = defn;
                            this.searchDefinitions[defn.id] = defn;
                        }
                    }
                }
            }

        }
    },

    /**
     * Function: create
     *
     * Create the application definition.  This actually triggers initializing
     * every widget and container.
     */
    create: function() {
        for (var i=0; i<this.widgetSets.length; i++) {
            this.widgetSets[i].create(this);
        }
    },

    /**
     * Function: getMapByName
     *
     * return a map widget with the given name
     *
     * Parameter: {String} name
     *
     * The map name to return
     *
     * Returns: {Object} a map object or null if not found.
     */
    getMapByName : function(name) {
        var map = null;
        for (var i=0; i<this.widgetSets.length; i++) {
            map = this.widgetSets[i].getMapByName(name);
            if (map) {
                break;
            }
        }
        return map;
    },

    /**
     * Function: getMapById
     *
     * return a map widget with the given id
     *
     * Parameter: {String} id
     *
     * The map id to return.  ID is distinct from map.name in that id is the
     * id of the HTML tag where the map widget is inserted.
     *
     * Returns: {Object} a map object or null if not found.
     */
    getMapById : function(id) {
        var map = null;
        for (var i=0; i<this.widgetSets.length; i++) {
            map = this.widgetSets[i].mapWidget;
            if (map.mapId == id) {
                break;
            }
        }
        return map;
    },

    /**
     * Function: getMapByIndice
     *
     * return the map widget at the given index
     *
     * Parameter: {String} indice
     *
     * The map indice to return
     *
     * Returns: {Object} a map object or null if not found.
     */
     getMapByIndice : function(indice) {
         var map = null;
         if (this.widgetSets.length > indice) {
             map = this.widgetSets[indice].getMapWidget();
         }
         return map;
     },

    /**
     * Function: getMapGroup
     *
     * return the specified map group from the application definition
     *
     * Parameter: {String} mapgroup
     *
     * The id of the MapGroup to return
     *
     * Returns: {Object} a MapGroup appdef or null if not found.
     */
     getMapGroup : function(mapGroupId) {
         return this.mapGroups[mapGroupId];
     },

     /**
      * Function getWidgetsByType
      *
      * returns an array of widgets by type.
      *
      * Parameter: {String} type
      *
      * the type of widget to get references to
      *
      * Returns: {Array} an array of widgets, which may be empty
      */
     getWidgetsByType: function(type) {
         var widgets = [];
         for (var i=0; i<this.widgetSets.length; i++) {
             widgets = widgets.concat(this.widgetSets[i].getWidgetsByType(type));
         }
         return widgets;
     }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.MapGroup
 *
 * Holds an internal representation of MapGroup objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.MapGroup = OpenLayers.Class({
    initialView: null,
    maps: null,

    initialize: function(jsonNode) {
        this.mapId = jsonNode['@id'][0];
        this.maps = [];
        /* parse InitialView */
        if (jsonNode.InitialView) {
            var iv = jsonNode.InitialView[0];
            if (iv.CenterX && iv.CenterY && iv.Scale) {
                this.setInitialView({x:parseFloat(iv.CenterX[0]),
                                     y:parseFloat(iv.CenterY[0]),
                                     scale:parseFloat(iv.Scale[0])});
            } else if (iv.MinX && iv.MinY && iv.MaxX && iv.MaxY) {
                this.setInitialView({minX:parseFloat(iv.MinX[0]),
                                     minY:parseFloat(iv.MinY[0]),
                                     maxX:parseFloat(iv.MaxX[0]),
                                     maxY:parseFloat(iv.MaxY[0])});
            } else {
                //TODO: emit warning that the initial view was incomplete
            }
        }
        /* parse maps */
        if (jsonNode.Map instanceof Array) {
            for (var i=0; i<jsonNode.Map.length; i++) {
                var map = new Fusion.Lib.ApplicationDefinition.Map(jsonNode.Map[i]);
                var links = {groups:[], layers:[]};
                var mapEvents = {layerEvents:{},groupEvents:{}};
                if (jsonNode.Map[i].Extension) {
                    var extension = jsonNode.Map[i].Extension[0];
                    if (extension.Links) {
                        /* process Groups */
                        if (extension.Links[0].Group instanceof Array) {
                            for (var j=0; j<extension.Links[0].Group.length; j++) {
                                var group = extension.Links[0].Group[j];
                                links.groups.push({name:group.Name[0],url:group.Url[0]});
                            }
                        }
                        if (extension.Links[0].Layer instanceof Array) {
                            for (var j=0; j<extension.Links[0].Layer.length; j++) {
                                var layer = extension.Links[0].Layer[j];
                                links.layers.push({name:layer.Name[0],url:layer.Url[0]});
                            }
                        }
                    }
                    /* process layer events */
                    //TODO: Should this be called MapEvents?
                    if (extension.MapEvents) {
                        if (extension.MapEvents[0].Layer instanceof Array) {
                            for (var j=0; j<extension.MapEvents[0].Layer.length; j++) {
                                var layer = extension.MapEvents[0].Layer[j];
                                var layerObj = {};
                                layerObj.name = layer.Name[0];
                                layerObj.onEnable = [];
                                if (layer.OnEnable instanceof Array) {
                                    layerObj.onEnable = this.parseMapEventSubBlock(layer.OnEnable[0]);
                                }
                                layerObj.onDisable = [];
                                if (layer.OnDisable instanceof Array) {
                                    layerObj.onDisable = this.parseMapEventSubBlock(layer.OnDisable[0]);
                                }
                                mapEvents.layerEvents[layerObj.name] = layerObj;
                            }
                        }
                        if (extension.MapEvents[0].Group instanceof Array) {
                            for (var j=0; j<extension.MapEvents[0].Group.length; j++) {
                                var group = extension.MapEvents[0].Group[j];
                                var groupObj = {};
                                groupObj.name = group.Name[0];
                                groupObj.onEnable = [];
                                if (layer.OnEnable instanceof Array) {
                                    groupObj.onEnable = this.parseMapEventSubBlock(group.OnEnable[0]);
                                }
                                groupObj.onDisable = [];
                                if (layer.OnDisable instanceof Array) {
                                    groupObj.onDisable = this.parseMapEventSubBlock(group.OnDisable[0]);
                                }
                                mapEvents.groupEvents[groupObj.name] = groupObj;
                            }
                        }
                    }
                }
                map.mapInfo = {links: links, mapEvents: mapEvents};
                this.maps.push(map);
            }
        } else {
            //TODO: do we need a warning that there are no layers in this map?
        }
    },

    parseMapEventSubBlock: function(block) {
        var a = [];
        if (block.Layer && block.Layer instanceof Array) {
            for (var i=0; i<block.Layer.length; i++) {
                var layer = block.Layer[i];
                a.push({type: 'layer', name:layer.Name[0], enable: layer.Enable[0] == 'true' ? true : false});
            }
        }
        if (block.Group && block.Group instanceof Array) {
            for (var i=0; i<block.Group.length; i++) {
                var group = block.Group[i];
                a.push({type: 'group', name:group.Name[0], enable: group.Enable[0] == 'true' ? true : false});
            }
        }
        return a;
    },

    getInitialView: function() {
        return this.initialView;
    },

    setInitialView: function(view) {
        this.initialView = view;
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Map
 *
 * Holds an internal representation of Map objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Map = OpenLayers.Class({
    type: null,
    singleTile: false,
    extension: null,
    
    initialize: function(jsonNode) {
        /* TODO: type can be any supported OpenLayers type */
        this.id = jsonNode['@id'] ? jsonNode['@id'][0] : null;
        this.type = jsonNode.Type[0];
        if (jsonNode.SingleTile) {
            var b = jsonNode.SingleTile[0].toLowerCase();
            this.singleTile = (b == "true") ? true : false;
        }
        if (jsonNode.Extension) {
            this.extension = jsonNode.Extension[0];
        } else {
            this.extension = {};
        }
        this.resourceId = this.extension.ResourceId ? this.extension.ResourceId[0] : '';

        var tagOptions = this.extension.Options;
        this.layerOptions = {};
        if (tagOptions && tagOptions[0]) {
          for (var key in tagOptions[0]) {
            var val = tagOptions[0][key][0];
            if (val.toFloat().toString() == val) {
              this.layerOptions[key] = val.toFloat();
            } else {
              if (val.toLowerCase() == 'true') {
                this.layerOptions[key] = true;
              } else if (val.toLowerCase() == 'false') {
                this.layerOptions[key] = false;
              } else {
                this.layerOptions[key] = val;
              }
            }
            if (key == 'maxExtent' || key == 'minExtent' || key == 'restrictedExtent') {
              this.layerOptions[key] = OpenLayers.Bounds.fromString(this.layerOptions[key]);
            }
            if (key == 'resolutions' || key == 'scales') {
              this.layerOptions[key] = this.layerOptions[key].split(',');
              for (var i=0; i<this.layerOptions[key].length; i++) {
                this.layerOptions[key][i] = this.layerOptions[key][i].toFloat();
              }
            }
          }
        }
        var tagParams = this.extension.Parameters;
        this.layerParams = {};
        if (tagParams && tagParams[0]) {
          for (var key in tagParams[0]) {
            var val = tagParams[0][key][0];
            if (val.toFloat().toString() == val) {
              val = val.toFloat();
            }
            this.layerParams[key] = val;
          }
        }
        
        //projection info from the extension
        //this.layerOptions.projection = "EPSG:4326";  //default to WGS84 lat long
        if (this.extension.ProjectionCode) {
          this.layerOptions.projection = this.extension.ProjectionCode[0];
        }
        if (this.extension.ProjectionDef) {
          var projDef = this.extension.ProjectionDef[0];
          if (!this.layerOptions.projection) {
            this.layerOptions.projection = "APP-DEF-PROJ";
          }
          Proj4js.defs[this.layerOptions.projection] = projDef;
        }
        if (!this.layerOptions.projection) {
          this.layerOptions.projection = "EPSG:4326";
        }

        switch (this.type) {
          case 'MapGuide':
          case 'MapServer':
            if ( !Fusion.Layers[this.type] ) {
                  Fusion.require('layers/' + this.type + '/' + this.type + '.js');
            }
            break;
          case "Google":
          case "Yahoo":
          case "VirtualEarth":
              this.layerOptions.isBaseLayer = true;
              this.layerOptions.sphericalMercator = true;
              this.layerOptions.displayProjection = "EPSG:4326";
              //no break here continue below to set EPSG:3785 as projCode
          default:
            if (this.layerOptions.sphericalMercator) {
              this.layerOptions.projection = "EPSG:900913";  //commercial map layer projection spherical mercator
            } else {
              this.layerOptions.projection = "EPSG:4326";  //WGS84 lat long
            }
            if ( !Fusion.Layers.Generic ) {
              Fusion.require('layers/Generic/Generic.js');
            }
            break;
        }
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.WidgetSet
 *
 * Holds an internal representation of WidgetSet objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.WidgetSet = OpenLayers.Class({
    containers: null,
    containersByName: null,
    widgetTags: null,
    widgetTagsByName: null,
    widgetInstances: null,
    mapWidget: null,
    mapId: null,
    initialize: function(jsonNode) {
        this.containers = [];
        this.widgetTags = [];
        this.widgetInstances = [];
        this.widgetTagsByName = {};
        this.containersByName = {};
        /* process map widgets */
        if (jsonNode.MapWidget) {
            for (var i=0; i<jsonNode.MapWidget.length; i++) {
                var widget = new Fusion.Lib.ApplicationDefinition.Widget(jsonNode.MapWidget[i]);
                widget.widgetSet = this;
                this.mapWidgetTag = widget;
                this.mapId = jsonNode.MapWidget[i].MapId[0];
            }
        }

        /* process widgets */
        if (jsonNode.Widget) {
            for (var i=0; i<jsonNode.Widget.length; i++) {
                var widget = new Fusion.Lib.ApplicationDefinition.Widget(jsonNode.Widget[i]);
                widget.widgetSet = this;
                this.widgetTags.push(widget);
                this.widgetTagsByName[widget.name] = widget;
            }
        }
        /* process containers */
        if (jsonNode.Container) {
            for (var i=0; i<jsonNode.Container.length; i++) {
                var container = new Fusion.Lib.ApplicationDefinition.Container(jsonNode.Container[i]);
                this.containers.push(container);
                this.containersByName[container.name] = container;
            }
        }

    },

    /**
     * Function: addWidgetInstance
     *
     * keep track of live widgets created in this widgetSet
     *
     * Parameter: {<Fusion.Widget>} widget
     *
     * the widget to add
     */
    addWidgetInstance: function(widget) {
        this.widgetInstances.push(widget);
    },

    /**
     * Function: getMapWidget
     *
     * return the map widget for this widget set
     *
     * Returns: {<Fusion.Lib.Map>} a map widget or null
     */
    getMapWidget: function() {
        return this.mapWidget;
    },

    /**
     * Function: create
     *
     * create all the things required by this widgetSet, including
     * containers and widgets.
     *
     * Parameter: {<Fusion.Lib.ApplicationDefinition>}
     *
     * the application definition that this widgetSet is part of
     */
    create: function(appDef) {

        //allow over-ride of mapId via URL parameter
        var mapGroup = null;
        var paramMapId = Fusion.getQueryParam('mapid');
        if (paramMapId.length>0) {
          mapGroup = appDef.getMapGroup(paramMapId);
          if (mapGroup) {
            this.mapId = paramMapId;
          } else {
            Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING,
              "can't find MapGroup: " + paramMapId + ' - reverting to default map'));
          }
        }

        if (!mapGroup) {
          mapGroup = appDef.getMapGroup(this.mapId);
        }

        //create the Map widget for this WidgetSet
        this.mapWidget = new Fusion.Widget.Map(this.mapWidgetTag,mapGroup,this);
        this.mapWidget.setMenu();
        $(this.mapWidgetTag.name).widget = this.mapWidget;

        //create the floating map message
        //not sure why this is here and it creates an area on the map where 
        //the mouse can't interact with the map
        // is this a debug widget? madair
        
        this.mapWidget.message = new Fusion.MapMessage(this.mapWidget.oMapOL.viewPortDiv);
        this.mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, (function(){
                if (this.message != null){
                    this.message.refreshLayout();
                }
            }).bind(this.mapWidget));
        
            
        //create all the other widgets for the widget set
        for (var i=0; i<this.widgetTags.length; i++) {
            this.widgetTags[i].create(this);
        }

        //create all the containers for the widget set
        for (var i=0; i<this.containers.length; i++) {
            this.containers[i].create(this);
        }
    },
    /**
     * Function: getMapByName
     *
     * return the map widget from this widget set if the map's name
     * matches the requested name, or null.
     *
     * Parameter: {String} name
     *
     * The map name to check
     *
     * Returns: {Object} a map object or null.
     */
    getMapByName : function(name) {
        var map = null;
        for (var i=0; i<this.mapWidget.aMaps.length; ++i) {
          var mapLayer = this.mapWidget.aMaps[i];
          if (mapLayer.getMapName() == name) {
              map = mapLayer;
          }
        }
        return map;
    },


    /**
     * Function getWidgetsByType
     *
     * returns an array of widgets by type.
     *
     * Parameter: {String} type
     *
     * the type of widget to get references to
     *
     * Returns: {Array} an array of widgets, which may be empty
     */
    getWidgetsByType: function(type) {
        var widgets = [];
        for (var i=0; i<this.widgetInstances.length; i++) {
            if (this.widgetInstances[i].type == type) {
                widgets.push(this.widgetInstances[i]);
            }
        }
        return widgets;
    },

    getWidgetByName: function(name) {
        return this.widgetTagsByName[name];
    },

    getContainerByName: function(name) {
        return this.containersByName[name];
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Container
 *
 * Holds an internal representation of Container objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Container = OpenLayers.Class({
    name: null,
    type: null,
    validPositions: ['top', 'left', 'bottom', 'right'],
    position: 'top',
    items: null,
    initialize: function(jsonNode) {
        this.type = jsonNode.Type[0];
        this.name = jsonNode.Name[0];
        var position = jsonNode.Position ? jsonNode.Position[0].toLowerCase() : this.position;
        for (var i=0; i<this.validPositions.length; i++) {
            if (this.validPositions[i] == position) {
                this.position = position;
                break;
            }
        }
        this.items = [];
        if (jsonNode.Item) {
            for (var i=0; i<jsonNode.Item.length; i++) {
                var item = new Fusion.Lib.ApplicationDefinition.Item(jsonNode.Item[i]);
                this.items.push(item);
            }
        } else {
            //TODO: is this a problem if there are no items?
        }
    },

    create: function(widgetSet) {
        var container;
        if (this.type == 'Toolbar' || this.type == 'Statusbar') {
            if ($(this.name)) {
                if ($(this.name).retrieve('jxToolbar')) {
                    container = $(this.name).retrieve('jxToolbar');
                } else {
                  container = new Jx.Toolbar({parent: this.name, position: this.position, scroll:true});
                }
                $(this.name).container = container;
                this.createWidgets(widgetSet, container);
            }
        } else if (this.type == 'Splitterbar') {
            if ($(this.name)) {
                container = new Jx.Splitter(this.name, {splitInto: this.items.length});
                for (var i=0; i<this.items.length; i++) {
                    container.elements[i].id = this.name + '_' + i;
                }
                $(this.name).container = container;
                this.createWidgets(widgetSet, container);
            }
        }
        if (container && container.domObj.jxLayout) {
            container.domObj.jxLayout.resize({forceResize: true});
        }
    },

    createWidgets: function(widgetSet, container) {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].create(widgetSet, container, this.name + '_' + i);
        }
    }

});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Widget
 *
 * Holds an internal representation of Widget objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Widget = OpenLayers.Class({
    name: null,
    type: null,
    statusText: null,
    location: null,
    imageUrl: null,
    imageClass: null,
    tooltip: null,
    label: null,
    disabled: null,
    extension: null,
    initialize: function(jsonNode) {
        if (jsonNode) {
            this.type = jsonNode.Type[0];
            this.name = jsonNode.Name ? jsonNode.Name[0] : '';
            this.statusText = jsonNode.StatusText ? jsonNode.StatusText[0] : '';
            this.statusText = OpenLayers.i18n(this.statusText);
            //TODO: this may be an extension
            this.location = jsonNode.Location ? jsonNode.Location[0] : 'widgets/';
            if (this.location.slice(-1) != '/') {
                this.location += '/';
            }
            this.imageUrl = jsonNode.ImageUrl ? jsonNode.ImageUrl[0] : '';
            this.imageClass = jsonNode.ImageClass ? jsonNode.ImageClass[0] : '';
            this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
            this.tooltip = OpenLayers.i18n(this.tooltip);
            this.label = jsonNode.Label ? jsonNode.Label[0] : '';
            this.label = OpenLayers.i18n(this.label);
            this.disabled = jsonNode.Disabled ? (jsonNode.Disabled[0].toLowerCase() == 'true' ? true : false) : false;

            //console.log('Widget: ' + this.type + ', ' + this.name + ', ' + this.description);

            if (jsonNode.Extension) {
                this.extension = jsonNode.Extension[0];
            } else {
                this.extension = {};
            }
            //require the widget code
            if ( !Fusion.Widget[this.type] ) {
              Fusion.require(this.location + this.type + '.js');
            }
        }
    },

    getMapWidget: function() {
        if (this.widgetSet) {
            return this.widgetSet.getMapWidget();
        } else {
            return null;
        }
    },

    /**
     * Function: create
     *
     * creates a new instance of the widget, optionally using a
     * different name during instantiation to accomodate
     * containers
     *
     * Parameter: name
     *
     * An optional name to use for the widget, overrides the
     * original name temporarily if passed.
     *
     * Returns: an instance of the widget represented by this
     * object.
     */
    create: function(widgetSet, widgetName) {
        var widget = null;
        this.widgetSet = widgetSet;
        var oldName = this.name;
        if (typeof widgetName == 'undefined') {
            widgetName = this.name;
        }
        /* create the widget if the name is not null and the name
         * is either an empty string (in the case of buttons in menus)
         * or something exists in the dom with the right id
         */
        if (widgetName != null && (widgetName == '' || $(widgetName) != null)) {
            this.name = widgetName;
            widget = new Fusion.Widget[this.type](this);
            widgetSet.addWidgetInstance(widget);
            if (this.name.length>0 && $(this.name)) {
                widget.id = this.name;
                $(this.name).widget = widget;
                if (widget.uiClass) {
                  widget.setUiObject(new widget.uiClass({
                      label: this.label,
                      image: this.imageUrl,
                      imageClass: this.imageClass,
                      toggle: this.isExclusive
                  }));
                    if (widget.uiObj.addTo) {
                        widget.uiObj.addTo($(this.name));
                    }
                    if (widget.uiObj.setEnabled) {
                        widget.uiObj.setEnabled(widget.isEnabled());
                        widget.registerForEvent(Fusion.Event.WIDGET_STATE_CHANGED, function() {
                            widget.uiObj.setEnabled(widget.isEnabled());
                        });
                    }
                }
            }
            this.name = oldName;
        }
        return widget;
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Item
 *
 * Holds an internal representation of Menu Item objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Item = OpenLayers.Class({
    uniqueId: [0],
    type: null,
    initialize: function(jsonNode) {
        this.type = jsonNode.Function[0];
        switch(this.type) {
            case 'Widget':
                this.widgetName = jsonNode.Widget[0];
                break;
            case 'Flyout':
                this.flyout = new Fusion.Lib.ApplicationDefinition.Flyout(jsonNode);
                break;
            case 'Multi':
                this.multi = new Fusion.Lib.ApplicationDefinition.Multi(jsonNode);
                break;
            case 'Separator':
                break;
        }
    },

    create: function(widgetSet, container, idx) {
        switch(this.type) {
            case 'Widget':
                var widgetTag = widgetSet.getWidgetByName(this.widgetName);
                if (widgetTag) {
                    var name = 'FusionItem'+this.uniqueId[0];
                    this.uniqueId[0]++;
                    if (container instanceof Jx.Toolbar) {
                        var widget = widgetTag.create(widgetSet, '');
                        if (!widget.uiClass) {
                            widget.uiClass = Jx.Toolbar.Item;
                            widget.setUiObject(new widget.uiClass());
                        } else if (widget.uiClass == Jx.Button || widget.uiClass == Jx.Button.Color || widget.uiClass == Jx.Menu || widget.uiClass == Jx.Button.Flyout) {
                            widget.setUiObject(new widget.uiClass({
                                label: widgetTag.label,
                                image: widgetTag.imageUrl,
                                imageClass: widgetTag.imageClass,
                                toggle: widget.isExclusive
                            }));
                        }
                        container.add(widget.uiObj);
                        if (widget.uiObj.setEnabled) {
                            widget.uiObj.setEnabled(widget.isEnabled());
                            widget.registerForEvent(Fusion.Event.WIDGET_STATE_CHANGED, function() {
                                widget.uiObj.setEnabled(widget.isEnabled());
                            });
                        }
                    } else if (container instanceof Jx.Splitter) {
                        var widget = widgetTag.create(widgetSet, idx);
                    } else if (container instanceof Jx.Menu ||
                               container instanceof Jx.Menu.Context ||
                               container instanceof Jx.Menu.SubMenu) {
                        var widget = widgetTag.create(widgetSet, '');
                        if (!widget.uiClass || widget.uiClass == Jx.Button) {
                            widget.uiClass = Jx.Menu.Item;
                        } else if (widget.uiClass == Jx.Menu) {
                            widget.uiClass = Jx.Menu.SubMenu;
                        }
                        widget.setUiObject(new widget.uiClass({
                            label: widgetTag.label,
                            image: widgetTag.imageUrl,
                            imageClass: widgetTag.imageClass,
                            toggle: widget.isExclusive
                        }));
                        container.add(widget.uiObj);
                        if (widget.uiObj.setEnabled) {
                            widget.uiObj.setEnabled(widget.isEnabled());
                            widget.registerForEvent(Fusion.Event.WIDGET_STATE_CHANGED, function() {
                                widget.uiObj.setEnabled(widget.isEnabled());
                            });
                        }
                    } else if (container instanceof Jx.Button.Multi) {
                        var widget = widgetTag.create(widgetSet, '');
                        widget.uiClass = Jx.Button;
                        widget.setUiObject(new widget.uiClass({
                            label: widgetTag.label,
                            image: widgetTag.imageUrl,
                            imageClass: widgetTag.imageClass,
                            toggle: widget.isExclusive
                        }));
                        container.add(widget.uiObj);
                        if (widget.uiObj.setEnabled) {
                            widget.uiObj.setEnabled(widget.isEnabled());
                            widget.registerForEvent(Fusion.Event.WIDGET_STATE_CHANGED, function() {
                                widget.uiObj.setEnabled(widget.isEnabled());
                                var b = !container.buttons.some(function(w) {
                                  return !w.isEnabled();
                                });
                                container.setEnabled(b);
                            });
                        }
                    }
                } else {
                  Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING,
                    "can't find widget: " + this.widgetName));
                 }
                break;
            case 'Flyout':
                /* create a menu */
                var menu;
                var opt = {
                    label: this.flyout.label,
                    tooltip: this.flyout.tooltip,
                    image: this.flyout.imageUrl,
                    imageClass: this.flyout.imageClass
                };
                if (container instanceof Jx.Toolbar) {
                    menu = new Jx.Menu(opt);
                } else if (container instanceof Jx.Menu ||
                           container instanceof Jx.Menu.Context ||
                           container instanceof Jx.Menu.SubMenu) {
                    menu = new Jx.Menu.SubMenu(opt);
                }
                container.add(menu);
                this.flyout.create(widgetSet, menu);

                break;
            case 'Multi':
                var multi;
                var opt = {
                    label: this.multi.label,
                    tooltip: this.multi.tooltip,
                    image: this.multi.imageUrl,
                    imageClass: this.multi.imageClass
                }
                multi = new Jx.Button.Multi(opt);
                container.add(multi);
                this.multi.create(widgetSet, multi);
                break;

            case 'Separator':
                if (container instanceof Jx.Toolbar) {
                    container.add(new Jx.Toolbar.Separator());
                } else if (container instanceof( Jx.Menu) ||
                           container instanceof(Jx.Menu.SubMenu) ||
                           container instanceof(Jx.Menu.Context)) {
                    container.add(new Jx.Menu.Separator());
                }
                break;
        }
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Flyout
 *
 * Holds an internal representation of Flyout objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Flyout = OpenLayers.Class({
    label: null,
    tooltip: null,
    description: null,
    imageUrl: null,
    items: null,

    initialize: function(jsonNode) {
        this.label = jsonNode.Label ? jsonNode.Label[0] : '';
        this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
        this.description = jsonNode.Description ? jsonNode.Description[0] : '';
        this.imageUrl = jsonNode.ImageUrl ? jsonNode.ImageUrl[0] : '';
        this.items = [];
        if (jsonNode.Item instanceof Array) {
            for (var i=0; i<jsonNode.Item.length; i++) {
                this.items.push(new Fusion.Lib.ApplicationDefinition.Item(jsonNode.Item[i]));
            }
        }
    },

    create: function(widgetSet, menu) {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].create(widgetSet, menu);
        }
    }

});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.Multi
 *
 * Holds an internal representation of Multi objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.Multi = OpenLayers.Class({
    label: null,
    tooltip: null,
    description: null,
    imageUrl: null,
    items: null,

    initialize: function(jsonNode) {
        this.label = jsonNode.Label ? jsonNode.Label[0] : '';
        this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
        this.description = jsonNode.Description ? jsonNode.Description[0] : '';
        this.imageUrl = jsonNode.ImageUrl ? jsonNode.ImageUrl[0] : '';
        this.items = [];
        if (jsonNode.Item instanceof Array) {
            for (var i=0; i<jsonNode.Item.length; i++) {
                this.items.push(new Fusion.Lib.ApplicationDefinition.Item(jsonNode.Item[i]));
            }
        }
    },

    create: function(widgetSet, multi) {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].create(widgetSet, multi);
        }
    }

});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.SearchDefinition
 *
 * Holds an internal representation of SearchDefinition objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.SearchDefinition = OpenLayers.Class({
    id: null,
    name: null,
    category: null,
    parameters: null,
    join: null,
    rule: null,

    initialize: function(json) {
        this.id = json['@id'];
        this.name = json['@name'];
        if (json.Join instanceof Array) {
            this.join = new Fusion.Lib.ApplicationDefinition.SearchJoin(json.Join[0]);
        }
        this.parameters = [];
        if (json.Parameter instanceof Array) {
            for (var i=0; i<json.Parameter.length; i++) {
                this.parameters.push(json.Parameter[i]['@name']);
            }
        }
        this.reports = [];
        if (json.Report instanceof Array) {
            for (var i=0; i<json.Report.length; i++) {
              var label = json.Report[i].Label[0]
              var link = json.Report[i].Link[0]
              this.reports.push({'label':label, 'link':link});
            }
        }
        var rule;
        if (json.SearchAnd instanceof Array) {
            this.rule = new Fusion.Lib.ApplicationDefinition.SearchRule('AND');
            rule = json.SearchAnd[0];
        } else if (json.SearchOr instanceof Array) {
            this.rule = new Fusion.Lib.ApplicationDefinition.SearchRule('OR');
            rule = json.SearchOr[0];
        }
        if (rule && rule.SearchCondition instanceof Array) {
            for (var i=0; i<rule.SearchCondition.length; i++) {
                this.rule.add(new Fusion.Lib.ApplicationDefinition.SearchCondition(rule.SearchCondition[i]));
            }
        }
    },

    getJoinUrl: function(params) {
        if (this.join) {
            return '&joinlayer='+this.join.layer+'&joinpk='+this.join.primaryKey+'&joinfk='+this.join.foreignKey;
        } else {
            return '';
        }
    },

    getFilterUrl: function(params) {
        return '&filter='+encodeURIComponent(this.rule.toString(params));
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.SearchJoin
 *
 * Holds an internal representation of SearchJoin objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.SearchJoin = OpenLayers.Class({
    layer: null,
    primaryKey: null,
    foreignKey: null,
    initialize: function(json) {
        this.layer = json.Layer ? json.Layer[0] : '';
        this.primaryKey = json.PrimaryKey ? json.PrimaryKey[0] : '';
        this.foreignKey = json.ForeignKey ? json.ForeignKey[0] : '';
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.SearchRule
 *
 * Holds an internal representation of SearchRule objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.SearchRule = OpenLayers.Class({
    type: null,
    conditions: null,
    initialize: function(type) {
        this.type = type;
        this.conditions = [];
    },

    add: function(condition) {
        this.conditions.push(condition);
    },

    remove: function(condition) {
        for (var i=0; i<this.conditions.length; i++) {
            if (this.conditions[i] == condition) {
                this.conditions.splice(i, 1);
                break;
            }
        }
    },

    toString: function(params) {
        var conditions = [];
        for (var i=0; i<this.conditions.length; i++) {
            this.conditions[i].setParams(params);
            var c = this.conditions[i].toString();
            if (c != '') {
                conditions.push(c);
            }
        }
        return '(' + conditions.join(') ' + this.type + ' (') + ')';
    }
});

/****************************************************************************
 * Class: Fusion.Lib.ApplicationDefinition.SearchCondition
 *
 * Holds an internal representation of SearchCondition objects as defined in the AppDef
 *
 */

Fusion.Lib.ApplicationDefinition.SearchCondition = OpenLayers.Class({
    column: null,
    operator: null,
    parameter: null,
    quote: null,
    value: null,
    operators: {eq:'=', like:'like', lt:'<', lte:'<=', gt:'>', gte:'>=', neq:'<>'},
    includeIfEmpty: false,

    initialize: function(json) {
        this.column = json.Column[0];
        this.operator = this.operators[json.Operator[0].toLowerCase()];
        this.parameter = json.Parameter[0];
        this.quote = json['@quote'] ? json['@quote'] : '';
        this.wildcard = json['@wildcard'] ? json['@wildcard'] : 'both';
        this.caseSensitive = true;
        if (json['@caseSensitive'] && json['@caseSensitive'] == 'false') {
            this.caseSensitive = false;
        }
    },

    setParams: function(p) {
        if (p[this.parameter]) {
            this.value = p[this.parameter];
        } else {
            this.value = '';
        }
    },

    toString: function() {
        var value = this.value ? this.value : '';
        if (value == '' && !this.includeIfEmpty) {
            return '';
        }
        var upper = '';
        if (!this.caseSensitive) {
            value = value.toUpperCase();
            upper = 'Upper';
        }
        var prewildcard = '';
        var prewildcard = '';
        var postwildcard = '';
        if (this.operator == 'like') {
            if (this.wildcard == 'before' || this.wildcard == 'both') {
                prewildcard = '*';
            }
            if (this.wildcard == 'after' || this.wildcard == 'both') {
                postwildcard = '*';
            }
        }
        var wildcard = this.operator == 'like' ? '*' : '';
        return upper + '('+this.column + ') ' + this.operator + ' ' + this.quote + prewildcard + value + postwildcard + this.quote;
    }
});
