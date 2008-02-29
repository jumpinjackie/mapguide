/**
 * Fusion.Widget.ClearSelection
 *
 * $Id: ClearSelection.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.ClearSelection
 *
 * Clears the current selection, if any.
 * **********************************************************************/



Fusion.Widget.ClearSelection = Class.create();
Fusion.Widget.ClearSelection.prototype = {
    initialize : function(widgetTag) {
        //console.log('ClearSelection.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        
        this.enable = Fusion.Widget.ClearSelection.prototype.enable;
        
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.enable.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.disable.bind(this));
    },
    
    /**
     * clears slection on map.
     */
    execute : function() {
        this.getMap().clearSelection();
    },
    
    enable: function() {
        if (this.oMap && this.oMap.hasSelection()) {
            Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
        } else {
            this.disable();
        }
    }
};
