/**
 * Fusion.Widget
 *
 * $Id: Widget.js 1033 2007-11-21 00:17:45Z pspencer $
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
 
Fusion.Widget = Class.create();
Fusion.Widget.prototype = {
    bIsMutuallyExclusive: null,
    sName: null,
    sType: null,
    oMap: null,
    bEnabled: false,
    mapLoadedWatcher: null,
    paramRegister: null,
    groups: [],
    group: null,
    domObj: null,
    
    /**
     * initialize the widget
     * @param sName {string} the name of the widget
     */
    initialize: function(widgetTag, bMutEx) {
        this.bIsMutuallyExclusive = bMutEx;
        this.sType = widgetTag.type;
        this.sName = widgetTag.name;
        this.widgetTag = widgetTag;
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.registerEventID(Fusion.Event.WIDGET_STATE_CHANGED);
        
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
    },
    /**
     * set the map object that this widget is associated with
     * @param oMap {Object} the map
     */
    setMap: function(oMap) {
        if (this.mapLoadedWatcher) {
            this.oMap.deregisterForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
            this.mapLoadedWatcher = null;
        }
        
        this.oMap = oMap;
        if (oMap) {
            this.mapLoadedWatcher = this._mapLoaded.bind(this);
            oMap.registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
        }
        
        if (oMap && oMap.isLoaded()) {
            this.enable();
        } else {
            this.disable();
        }
    },
    /**
     * accessor to get the Map object that this widget is associated with
     * @return {object} the map
     */
    getMap: function() {
        return this.oMap;
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
     * set whether this widget is mutually exclusive on its map
     * @param bIsMutEx {boolean} is the widget mutually exclusive?
     */
    setMutEx: function(bIsMutEx) {
        this.bIsMutuallyExclusive = bIsMutEx;
    },
    
    /**
     * accessor to determine if the widget should be activated mutually
     * exclusively from other widgets on the map.
     * @return {boolean} true if the widget is mutually exclusive
     */
    isMutEx: function() {
        return this.bIsMutuallyExclusive;
    },
    
    /**
     * accessor to return the name of the widget.  Mostly for debugging
     * @return {string} the name of the widget
     */
    getName: function() {
        return this.sName;
    },
    
    /**
     * Method: getLocation
     *
     * returns the location of this widget relative to the installation
     * of fusion.  Can be used to construct URLs to resources that
     * the widget needs.
     *
     * Returns: {String} the location of this widget
     */
    getLocation: function() {
        return this.widgetTag.location;    
    },
    
    isEnabled: function() { return this.bEnabled; },
    
    enable: function() { this.bEnabled = true; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    disable: function() { this.bEnabled = false; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    setParameter : function(param, value){},

    registerParameter : function(param) {
      this.paramRegister.push(param);
    }
};
