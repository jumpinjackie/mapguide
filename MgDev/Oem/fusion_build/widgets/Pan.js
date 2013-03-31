/**
 * Fusion.Widget.Pan
 *
 * $Id: Pan.js 2579 2012-09-07 09:20:12Z jng $
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
 * 
 * Inherits from:
 *  - <Fusion.Widget>
 * **********************************************************************/

Fusion.Widget.Pan = OpenLayers.Class(Fusion.Widget, {
    isExclusive: true,
    uiClass: Jx.Button,
    initializeWidget: function(widgetTag) {
        this.control = new OpenLayers.Control.DragPan();
        this.getMap().oMapOL.addControl(this.control);
        this.control.handler.keyMask = 0;
        var index = window.location.href.indexOf("?");
        var mainpath = window.location.href.substring(0,index);
        index = mainpath.lastIndexOf("/");
        mainpath = mainpath.substring(0,index+1);
        var grabpath= "url(" + mainpath + "images/grab.cur" + "), move";
        var grabbingpath = "url(" + mainpath + "images/grabbing.cur" + "), move";
        
        this.cursorNormal = [grabpath, 'grab', '-moz-grab', 'move'];
        this.cursorDrag = [grabbingpath, 'grabbing', '-moz-grabbing', 'move'];
    },

    activate : function() {
        this.control.activate();
        this.getMap().setCursor(this.cursorNormal);
    },
    
    deactivate: function() {
        /*console.log('Pan.deactivate');*/
        this.control.deactivate();
        this.getMap().setCursor('auto');
    }
});
