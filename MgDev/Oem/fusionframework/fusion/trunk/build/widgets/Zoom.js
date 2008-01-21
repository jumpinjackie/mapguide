/**
 * Fusion.Widget.Zoom
 *
 * $Id: Zoom.js 1121 2007-12-13 22:13:01Z madair $
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

 /********************************************************************
 * Class: Fusion.Widget.Zoom
 *
 * A widget that will zoom the map in or out.
 * 
 * **********************************************************************/

Fusion.Widget.Zoom = Class.create();
Fusion.Widget.Zoom.prototype = 
{
    nTolerance : 5,
    nFactor : 2,
    zoomIn: true,
    keyModifiers: 0,    //set during event handling to indicate modifier key states
    
    initialize : function(widgetTag)
    {
        //console.log('Zoom.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        this.asCursor = ["url('images/zoomin.cur'),auto",'-moz-zoom-in', 'auto'];
        var json = widgetTag.extension;
        this.nTolerance = json.Tolerance ? json.Tolerance[0] : this.nTolerance;
        this.nFactor = json.Factor ? json.Factor[0] : this.nFactor;
        this.zoomIn = (json.Direction && json.Direction[0] == 'out') ? false : true;
        this.zoomInCursor = ["url('images/zoomin.cur'),auto",'-moz-zoom-in', 'auto'];
        this.zoomOutCursor = ["url('images/zoomout.cur'),auto",'-moz-zoom-out', 'auto'];
        
        this.keypressWatcher = this.keypressHandler.bind(this);
        
        this.map = this.getMap().oMapOL;
        this.handler = new OpenLayers.Handler.Box(this, {done: this.execute});
        this.handler.dragHandler.up = this.setModifiers.bind(this);
        this.handler.dragHandler.down = this.clearModifiers.bind(this);
    },

   /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     */
    activateTool : function()
    {
        //console.log('Zoom.activateTool');
        this.getMap().activateWidget(this);
        Event.observe(document, 'keypress', this.keypressWatcher);
        
    },

    /**
     * activate the widget (listen to mouse events and change cursor)
     * This function should be defined for all functions that register
     * as a widget in the map
     */
    activate : function()
    {
        //console.log('Zoom.activate');
        this.handler.activate();
        /*cursor*/
        if (this.zoomIn) {
            this.getMap().setCursor(this.zoomInCursor);
        } else {
            this.getMap().setCursor(this.zoomOutCursor);
        }
        /*icon button*/
        this._oButton.activateTool();
    },

    /**
     * deactivate the widget (listen to mouse events and change cursor)
     * This function should be defined for all functions that register
     * as a widget in the map
     **/
    deactivate : function()
    {
        //console.log('Zoom.deactivate');
        this.handler.deactivate();
        this.getMap().setCursor('auto');
        /*icon button*/
        this._oButton.deactivateTool();
        Event.stopObserving(document, 'keypress', this.keypressWatcher);
        
    },

    /**
     * Method: zoomBox
     *
     * Parameters:
     * position - {<OpenLayers.Bounds>} or {<OpenLayers.Pixel>}
     */
    execute: function (position) {
        /* if the last event had a shift modifier, swap the sense of this
                tool - zoom in becomes out and zoom out becomes in */
        var zoomIn = this.zoomIn;
        if (this.keyModifiers & OpenLayers.Handler.MOD_SHIFT) {
            zoomIn = !zoomIn;
        }
        if (position instanceof OpenLayers.Bounds) {
            var minXY = this.map.getLonLatFromPixel(
                            new OpenLayers.Pixel(position.left, position.bottom));
            var maxXY = this.map.getLonLatFromPixel(
                            new OpenLayers.Pixel(position.right, position.top));
            var bounds = new OpenLayers.Bounds(minXY.lon, minXY.lat,
                                            maxXY.lon, maxXY.lat);
            if (zoomIn) {
                this.getMap().setExtents(bounds);
            } else {
                var newWidth = bounds.getWidth();
                var newHeight = bounds.getHeight();
                var currentExtents = this.getMap().getCurrentExtents();
                var currentWidth = currentExtents.getWidth();
                var currentHeight = currentExtents.getHeight();
                var factor = Math.min(newWidth/currentWidth, newHeight/currentHeight);
                var center = bounds.getCenterLonLat();
                this.getMap().zoom(center.lon, center.lat, factor);
            }
        } else { // it's a pixel
            var center = this.map.getLonLatFromPixel(position);
            var factor;
            if(!zoomIn && this.nFactor > 1) {
                factor = 1/this.nFactor;
            } else {
                factor = this.nFactor;
            }
            this.getMap().zoom(center.lon, center.lat, factor);
        }
    },

    /**
        * calculate the keyboard modifier mask for this event 
        *
        * Parameters:
        * evt - the OpenLayers.Event object that is being responded to
        */
    setModifiers: function(evt) {
        this.keyModifiers =
            (evt.shiftKey ? OpenLayers.Handler.MOD_SHIFT : 0) |
            (evt.ctrlKey  ? OpenLayers.Handler.MOD_CTRL  : 0) |
            (evt.altKey   ? OpenLayers.Handler.MOD_ALT   : 0);
    },
    
    /**
        * clears the keyboard modifier mask for this event 
        *
        * Parameters:
        * evt - the OpenLayers.Event object that is being responded to
        */
    clearModifiers: function(evt) {
      this.keyModifiers = 0;
    },

    /**
        * allows run-time setting of widget parameters 
        *
        * Parameters:
        * param - the widget parameter name to set; for the Zoom widget these may be:
        *               'Factgor'
        * value - the value to use for the parameter
        */
    setParameter : function(param, value)
    {
        if (param == "Factor" && value > 0)
        {
            this.nFactor = value;
        }
    },
    
    keypressHandler: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.handler.deactivate();
            this.handler.activate();
        }
    }
};
