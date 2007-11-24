/**
 * Fusion.Widget.CenterSelection
 *
 * $Id: CenterSelection.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.CenterSelection
 *
 * Center the current selection, if any, but maintain the current scale
 * if possible.  Zoom out if not.
 *
 * **********************************************************************/



Fusion.Widget.CenterSelection = Class.create();
Fusion.Widget.CenterSelection.prototype = {
    initialize : function(widgetTag) {
        //console.log('CenterSelection.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        this.enable = Fusion.Widget.CenterSelection.prototype.enable;
        
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.enable.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.disable.bind(this));
    },

    /**
     * get the selection from the map (which may not be loaded yet).
     * zoomToSelection is called when the selection is ready.
     */
    execute : function() {
        this.getMap().getSelection(this.centerSelection.bind(this));
    },

    /**
     * set the extents of the map based on the pixel coordinates
     * passed
     * 
     * @param selection the active selection, or null if there is none
     */
    centerSelection : function(selection) {
        var extents = this.getMap().getCurrentExtents();
        var curWidth = extents[2] - extents[0];
        var curHeight = extents[3] - extents[1];
        
        var ll = selection.getLowerLeftCoord();
        var ur = selection.getUpperRightCoord();
        
        var newWidth = ur.x - ll.x;
        var newHeight = ur.y - ll.y;
        
        if (newWidth < curWidth && newHeight < curHeight) {
            var cx = (ur.x + ll.x) / 2;
            var cy = (ur.y + ll.y) / 2;
            this.getMap().zoom(cx,cy,1);
        } else {
            var buffer = 0.1;
            var minx = ll.x-newWidth*buffer;
            var miny = ll.y-newHeight*buffer;
            var maxx = ur.x+newWidth*buffer;
            var maxy = ur.y+newHeight*buffer;
            this.getMap().setExtents(new OpenLayers.Bounds(minx,miny,maxx,maxy));
        }
    },

    enable: function() {
        if (this.oMap && this.oMap.hasSelection()) {
            Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
        } else {
            this.disable();
        }
    }
};
