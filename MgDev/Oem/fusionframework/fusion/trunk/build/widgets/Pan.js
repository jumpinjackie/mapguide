/**
 * Fusion.Widget.Pan
 *
 * $Id: Pan.js 1252 2008-02-22 22:36:46Z madair $
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
 * Class: Fusion.Widget.Pan
 *
 * A widget that allows for naviagtion by panning
 * **********************************************************************/

Fusion.Widget.Pan = Class.create();
Fusion.Widget.Pan.prototype = {
    initialize : function(widgetTag) {
        //console.log('Pan.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        this.control = new OpenLayers.Control.DragPan();
        this.getMap().oMapOL.addControl(this.control);
        this.control.handler.keyMask = 0;
        
        this.cursorNormal = ["url('images/grab.cur'),move", 'grab', '-moz-grab', 'move'];
        this.cursorDrag = ["url('images/grabbing.cur'),move", 'grabbing', '-moz-grabbing', 'move'];
    },

    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     */
    activateTool : function() {
        /*console.log('Pan.activateTool');*/
        this.getMap().activateWidget(this);
    },
    
    activate : function() {
        this.control.activate();
        this.getMap().setCursor(this.cursorNormal);
        /*button*/
        this._oButton.activateTool();
    },
    
    deactivate: function() {
        /*console.log('Pan.deactivate');*/
        this.control.deactivate();
        this.getMap().setCursor('auto');
        /*icon button*/
        this._oButton.deactivateTool();
    }
};