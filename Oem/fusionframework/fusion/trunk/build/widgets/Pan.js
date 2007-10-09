/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Pan.js 804 2007-09-10 20:17:21Z madair $
 * @purpose Pan
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
 ********************************************************************
 *
 * extended description
 * **********************************************************************/

Fusion.Widget.Pan = Class.create();
Fusion.Widget.Pan.prototype = {
    initialize : function(widgetTag) {
        //console.log('Pan.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.Rectangle.prototype, []);
        
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
        /*console.log('Pan.activate');*/
        this.activateRectTool();
        /* override the default handling of the rect tool */
        this.oMap.stopObserveEvent('mousemove', this.mouseMoveCB);
        this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        
        this.getMap().setCursor(this.cursorNormal);
        /*button*/
        this._oButton.activateTool();
    },
    
    deactivate: function() {
        /*console.log('Pan.deactivate');*/
        this.deactivateRectTool();
        this.getMap().setCursor('auto');
        /*icon button*/
        this._oButton.deactivateTool();
    },

    execute : function(nX, nY) {
        var sGeoPoint = this.getMap().pixToGeo(nX,nY);
        this.getMap().zoom(sGeoPoint.x, sGeoPoint.y, 1);
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
            this.getMap().setCursor(this.cursorDrag);
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
            
            var dx = this.startPos.x - p.x;
            var dy = this.startPos.y - p.y;

            var olMap = this.getMap().oMapOL;
            var size = olMap.getSize();
            var newXY = new OpenLayers.Pixel(size.w / 2 + dx, size.h / 2 + dy);
            var newCenter = olMap.getLonLatFromPixel( newXY ); 
            this.getMap().zoom(newCenter.lon, newCenter.lat, 1);
            this.startPos = null;
            Event.stop(e);
        }
        OpenLayers.Event.stopObserving(document, 'mouseup', this.mouseUpCB);
        OpenLayers.Event.stopObserving(document, 'mousemove', this.mouseMoveCB);
        OpenLayers.Event.stopObserving(document, 'mouseout', this.mouseOutCB);
        
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

        var dx = this.startPos.x - p.x;
        var dy = this.startPos.y - p.y;

        var olMap = this.getMap().oMapOL;
        var size = olMap.getSize();
        var newXY = new OpenLayers.Pixel(size.w / 2 + dx, size.h / 2 + dy);
        var newCenter = olMap.getLonLatFromViewPortPx( newXY ); 
        olMap.setCenter(newCenter, null, true);
        this.startPos = p;

        Event.stop(e);
    }
};