/**
 * Fusion.Widget.PanQuery
 *
 * $Id: PanQuery.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.PanQuery
 *
 * A widget that combines pan and query functionality.  If the mouse is moved
 * before being released, a pan is performedd, otherwise a query is executed.
 * 
 * **********************************************************************/

Fusion.Widget.PanQuery = Class.create();
Fusion.Widget.PanQuery.prototype = {
    selectionType: 'INTERSECTS',
    nTolerance: 3,
    bActiveOnly: false,
    initialize : function(widgetTag) {
        //console.log('PanQuery.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.Rectangle.prototype, []);
        
        var json = widgetTag.extension;
        /* selection type doesn't make sense?
        this.selectionType = json.SelectionType ? json.SelectionType[0] : 'INTERSECTS';
        */
        
        this.nTolerance = json.Tolerance ? Math.abs(parseInt(json.Tolerance)) : 3;

        var activeOnly = json.QueryActiveLayer ? json.QueryActiveLayer[0] : 'false';
        this.bActiveOnly = (activeOnly == 'true' || activeOnly == '1') ? true : false;
        
        this.cursorNormal = ['auto'];
        this.cursorDrag = ["url('images/grabbing.cur'),move", 'grabbing', '-moz-grabbing', 'move'];
    },

    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     */
    activateTool : function() {
        //console.log('PanQuery.activateTool');
        this.getMap().activateWidget(this);
    },
    
    activate : function() {
        /*console.log('PanQuery.activate');*/
        this.activateRectTool();
        /* override the default handling of the rect tool */
        this.oMap.stopObserveEvent('mousemove', this.mouseMoveCB);
        this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        
        this.getMap().setCursor(this.cursorNormal);
        /*button*/
        this._oButton.activateTool();
    },
    
    deactivate: function() {
        /*console.log('PanQuery.deactivate');*/
        this.deactivateRectTool();
        this.getMap().setCursor('auto');
        /*icon button*/
        this._oButton.deactivateTool();
    },

    /**
     * (private) gPan.MouseDown(e)
     *
     * handle mouse down events on the mapObj
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseDown: function(e) {
        if (Event.isLeftClick(e)) {
            var p = {x:Event.pointerX(e), y:Event.pointerY(e)};    
            this.startPos = p;
            Event.observe(document, 'mouseup', this.mouseUpCB);
            Event.observe(document, 'mousemove', this.mouseMoveCB);
            Event.observe(document, 'mouseout', this.mouseOutCB);
        }
        Event.stop(e);
    },

    /**
     * (private) gPan.MouseUp(e)
     *
     * handle mouseup events on the mapObj
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseUp: function(e) {
        if (this.startPos) {
            this.getMap().setCursor(this.cursorNormal);

            var p = {x:Event.pointerX(e), y:Event.pointerY(e)};    

            var dx = p.x - this.startPos.x;
            var dy = p.y - this.startPos.y;
            
            if (Math.abs(dx) > this.nTolerance || Math.abs(dy) > this.nTolerance) {
                var size = this.getMap().getPixelSize();

                var t = -dy;
                var l = -dx;
                var r = l + size.width;
                var b = t + size.height; 

                var min = this.getMap().pixToGeo(l,b); 
                var max = this.getMap().pixToGeo(r,t); 
                this.getMap().setExtents(new OpenLayers.Bounds(min.x,min.y,max.x,max.y)); 
                
            } else { 
                p = this.getMap().getEventPosition(e);
                var pos = this.getMap().pixToGeo(this.startPos.x,this.startPos.y);
                var options = {};
                var dfGeoTolerance = this.getMap().pixToGeoMeasure(this.nTolerance);
                var minx = pos.x-dfGeoTolerance; 
                var miny = pos.y-dfGeoTolerance; 
                var maxx = pos.x+dfGeoTolerance; 
                var maxy = pos.y+dfGeoTolerance;
                options.geometry = 'POLYGON(('+ minx + ' ' + miny + ', ' + maxx + ' ' + miny + ', ' + maxx + ' ' + maxy + ', ' + minx + ' ' + maxy + ', ' + minx + ' ' + miny + '))';
                options.selectionType = "INTERSECTS";

                if (this.bActiveOnly) {
                    var layer = this.getMap().getActiveLayer();
                    if (layer) {
                        options.layers = layer.layerName;
                    } else {
                        return;
                    }
                }

                if (e.shiftKey) {
                    options.extendSelection = true;
                }

                this.getMap().query(options);
            }
            this.startPos = null;
            Event.stopObserving(document, 'mouseup', this.mouseUpCB);
            Event.stopObserving(document, 'mousemove', this.mouseMoveCB);
            Event.stopObserving(document, 'mouseout', this.mouseOutCB);

            Event.stop(e);
        }
    },

    /**
     * (private) gPan.MouseMove(e)
     *
     * handle mousemove events on the mapObj by moving the
     * map image inside its parent object
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseMove: function(e) {
        if (!this.startPos) {
            return false;
        }
        var p = {x:Event.pointerX(e), y:Event.pointerY(e)};    

        var dx = p.x - this.startPos.x;
        var dy = p.y - this.startPos.y;
        
        if (Math.abs(dx) > this.nTolerance || Math.abs(dy) > this.nTolerance) {
            this.getMap().setCursor(this.cursorDrag);
            
            this.getMap()._oImg.style.top = dy + 'px';
            this.getMap()._oImg.style.left = dx + 'px';
        }

        Event.stop(e);
    },
    
    
    setParameter : function(param, value) {
        if (param == "Tolerance" && value > 0) {
            this.nTolerance = value;
        }
        if (param == 'SelectionType') {
            this.selectionType = value;
        }
    }
};