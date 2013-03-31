/**
 * Fusion.Widget
 *
 * $Id: Widget.js 2579 2012-09-07 09:20:12Z jng $
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

 /* ***************************************************************************
 * Class: Fusion.Widget
 *
 * This is the base class for all widgets.  It provides some basic
 * functionality that all widgets should need.
 *
 * ***************************************************************************/
 
Fusion.Event.WIDGET_STATE_CHANGED = Fusion.Event.lastEventId++;
 
Fusion.Widget = OpenLayers.Class(Fusion.Lib.EventMgr, {
    isExclusive: false,
    name: null,
    type: null,
    oMap: null,
    widgetLayerId: null,   //the AppDef <Map> object associated with this widget 
    enabled: false,
    mapLoadedWatcher: null,
    paramRegister: null,
    groups: [],
    group: null,
    domObj: null,
    uiClass: null,
    widgetUniqueId: null,
    
    /**
     * initialize the widget
     * @param name {string} the name of the widget
     */
    initialize: function(widgetTag) {
        this.type = widgetTag.type;
        this.name = widgetTag.name;
        this.widgetTag = widgetTag;
        this.registerEventID(Fusion.Event.WIDGET_STATE_CHANGED);
        if (!Fusion.Widget.uiInstances[this.type]) {
            Fusion.Widget.uiInstances[this.type] = [];
        }
        
        var group = widgetTag.extension.Group ? widgetTag.extension.Group[0] : '';
        if (group != '') {
            if (!this.groups[group]) {
                this.groups[group] = [];
            }
            this.groups[group].push(this);
            this.group = group;
        }
        this.setMap(widgetTag.getMapWidget());
        
        if (widgetTag.name) {
            this.domObj = $(widgetTag.name);
        }

        this.paramRegister = [];
        
        //check to see if this widget is associated with a specific Map
        var json = widgetTag.extension;
        this.widgetLayerId = json.WidgetLayerId ? json.WidgetLayerId[0] : null;
        this.widgetUniqueId = OpenLayers.Util.createUniqueID("FusionWidget_");
        
        this.initializeWidget(widgetTag);
    },

    initializeWidget: function() {},

    /**
     * Function: activate
     * 
     * Activates this widget
     * 
     */
    activate: function() { },

    /**
     * Function: deactivate
     * 
     * Deactivates this widget
     */
    deactivate: function() { },

    setUiObject: function(uiObj) {
        Fusion.Widget.uiInstances[this.type].push(this);
        if (this.widgetTag.tooltip) {
          if (uiObj.setTooltip) {
              uiObj.setTooltip(this.widgetTag.tooltip);
          } else if (uiObj.button && uiObj.button.setTooltip){
              uiObj.button.setTooltip(this.widgetTag.tooltip);
          }
        }
        if (this.isExclusive) {
        	if (Fusion.Widget.uiInstances[this.type].length==1) {
            this.getMap().buttonSet.add(uiObj);
          } else {
						var instances = Fusion.Widget.uiInstances[this.type];
						for (var i=0; i<instances.length; i++) {
							if (!this.shouldActivateWith(instances[i])) {
								this.getMap().buttonSet.add(uiObj);
								break;
							}
						}
					}
        }
        if (uiObj.options.active) {
            this.activate();
        }
        if (uiObj.addEvents) {
            if (Fusion.Widget.uiInstances[this.type][0].uiObj &&
                Fusion.Widget.uiInstances[this.type][0].uiObj.options.active &&
                Fusion.Widget.uiInstances[this.type][0].shouldActivateWith(this) &&
                uiObj.setActive) {
                uiObj.setActive(true);
            }
            
            uiObj.addEvents({
                'click': (function() {
                    this.activate();
                }).bind(this),
                'up': (function() {
                    var instances = Fusion.Widget.uiInstances[this.type];
                    for (var i=0; i<instances.length; i++) {
                        var instance = instances[i];
                        if (instance.shouldActivateWith(this) &&
                            instance.uiObj && instance.uiObj.setActive) {
                            instance.uiObj.setActive(false);
                        }
                    }
                    this.deactivate();
                }).bind(this),
                'down': (function() {
                    var instances = Fusion.Widget.uiInstances[this.type];
                    for (var i=0; i<instances.length; i++) {
                        var instance = instances[i];
                        if (instance.shouldActivateWith(this) &&
                            instance.uiObj && instance.uiObj.setActive) {
                            instance.uiObj.setActive(true);
                        }
                    }
                    this.activate();
                }).bind(this)
            });
        }
        this.uiObj = uiObj; 
    },
    
    /**
     * Method: shouldActivateWith
     *
     */
    shouldActivateWith: function(widget) {
        return true;
    },

    /**
     * Method: setMap
     *
     * set the map object that this widget is associated with
     * 
     * Parameters:
     * 
     *   oMap - {<Fusion.Widget.Map>} the map
     */
    setMap: function(oMap) {
        if (this.mapLoadedWatcher) {
            this.oMap.deregisterForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
            this.mapLoadedWatcher = null;
        }
        
        this.oMap = oMap;
        if (oMap) {
            this.mapLoadedWatcher = OpenLayers.Function.bind(this._mapLoaded, this);
            oMap.registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
        }
        
        if (oMap && oMap.isLoaded()) {
            this.enable();
        } else {
            this.disable();
        }
    },
    /**
     * Method: getMap
     *
     * accessor to get the Map object that this widget is associated with
     * 
     * Return: 
     * {<Fusion.Widget.Map>} the map
     */
    getMap: function() {
        return this.oMap;
    },
    
    /**
     * Method: getMapLayer
     *
     * accessor to get the implementation-specific Layer object that this widget is associated with
     * 
     * Return: 
     * {<Fusion.Layers>} the implementation-specific layer
     */
    getMapLayer: function() {
      if (this.widgetLayerId) {
        var map = this.getMap();
        for (var i=0; i<map.aMaps.length; ++i) {
          if (map.aMaps[i].id && (map.aMaps[i].id == this.widgetLayerId) ) {
            return map.aMaps[i];
          }
        }
        Fusion.reportError("Requested widgetLayerId not found:"+this.widgetLayerId);
        return null;
      } else {
        return this.getMap().aMaps[0];
      }
    },
    
    /**
     * Method: addControl
     * 
     * utility method to add an {<OpenLayers.Control>} control to the internal {<OpenLayers.Map>} object
     * 
     * Parameters:
     * control - {<OpenLayers.Control>} the control to add
     */
    addControl: function(control) {
        this.getMap().oMapOL.addControl(control);
    },
    
    /**
     */
    _mapLoaded: function() {
        if (this.oMap && this.oMap.isLoaded()) {
            //console.log('enable');
            this.enable();
        } else {
            //console.log('disable');
            this.disable();
        }
    },
    
    /** 
     * Method: setMutEx
     *
     * sets whether this widget is mutually exclusive on its map
     * 
     * Parameters:
     * bIsMutEx - {boolean} is the widget mutually exclusive?
     */
    setMutEx: function(bIsMutEx) {
        this.isExclusive = bIsMutEx;
    },
    
    /**
     * Method: isMutEx
     * 
     * accessor to determine if the widget should be activated mutually
     * exclusively from other widgets on the map.
     * 
     * Return: 
     * {boolean} true if the widget is mutually exclusive
     */
    isMutEx: function() {
        return this.isExclusive;
    },
    
    /**
     * Method: getName
     *
     * accessor to return the name of the widget.  Mostly for debugging
     * 
     * Return: 
     * {string} the name of the widget
     */
    getName: function() {
        return this.name;
    },
    
    /**
     * Method: getLocation
     *
     * returns the location of this widget relative to the installation
     * of fusion.  Can be used to construct URLs to resources that
     * the widget needs.
     *
     * Return: 
     * {String} the location of this widget
     */
    getLocation: function() {
        return this.widgetTag.location;    
    },
    
    /**
     * Method: isEnabled
     * 
     * Gets whether this widget is enabled
     */
    isEnabled: function() { 
        return this.enabled; 
    },
    
    /**
     * Method: enabled
     * 
     * Enables this widget. Triggers the {<Fusion.Event.WIDGET_STATE_CHANGED>} event
     */
    enable: function() { 
        this.enabled = true; 
        this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);
    },

    /**
     * Method: disable
     * 
     * Disables this widget. Triggers the {<Fusion.Event.WIDGET_STATE_CHANGED>} event
     */
    disable: function() { 
        this.enabled = false; 
        this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);
    },

    setParameter : function(param, value){},

    registerParameter : function(param) {
      this.paramRegister.push(param);
    }
});

Fusion.Widget.uiInstances = {};
