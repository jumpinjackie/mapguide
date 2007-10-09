/*****************************************************************************
 * @project Fusion
 * @revision $Id: Widget.js 818 2007-09-14 20:57:20Z madair $
 * @purpose Base Class for all widgets
 * @author pspencer@dmsolutions.ca
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * ***************************************************************************
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
        this.sName = widgetTag.type;
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
    
    isEnabled: function() { return this.bEnabled; },
    
    enable: function() { this.bEnabled = true; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    disable: function() { this.bEnabled = false; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    setParameter : function(param, value){},

    registerParameter : function(param) {
      this.paramRegister.push(param);
    }
};
