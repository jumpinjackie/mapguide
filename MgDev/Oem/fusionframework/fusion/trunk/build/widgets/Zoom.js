/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Zoom.js 787 2007-09-08 19:05:39Z pspencer $
 * @purpose Zoom widget
 * @author yassefa@dmsolutions.ca
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
 * Zoom widget using the map guide web layout configuration file
 * 
 * **********************************************************************/

Fusion.Widget.Zoom = Class.create();
Fusion.Widget.Zoom.prototype = 
{
    nTolerance : 5,
    nFactor : 2,
    zoomIn: true,
    initialize : function(widgetTag)
    {
        //console.log('Zoom.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.Rectangle.prototype, []);

        this.asCursor = ["url('images/zoomin.cur'),auto",'-moz-zoom-in', 'auto'];
        var json = widgetTag.extension;
        this.nTolerance = json.Tolerance ? json.Tolerance[0] : this.nTolerance;
        this.nFactor = json.Factor ? json.Factor[0] : this.nFactor;
        this.zoomIn = (json.Direction && json.Direction[0] == 'out') ? false : true;
        this.zoomInCursor = ["url('images/zoomin.cur'),auto",'-moz-zoom-in', 'auto'];
        this.zoomOutCursor = ["url('images/zoomout.cur'),auto",'-moz-zoom-out', 'auto'];
        
        this.keypressWatcher = this.keypressHandler.bind(this);
        
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
        this.activateRectTool();
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
        this.deactivateRectTool();
        this.getMap().setCursor('auto');
        /*icon button*/
        this._oButton.deactivateTool();
        Event.stopObserving(document, 'keypress', this.keypressWatcher);
        
    },

    /**
     * set the extents of the map based on the pixel coordinates
     * passed
     * 
     * @param nLeft integer pixel coordinates of the left (minx)
     * @param nBottom integer pixel coordinates of the bottom (miny)
     * @param nRight integer pixel coordinates of the right (maxx)
     * @param nTop integer pixel coordinates of the top (maxy)
     */
    execute : function(nLeft, nBottom, nRight, nTop)
    {
        /* if the last event had a shift modifier, swap the sense of this
           tool - zoom in becomes out and zoom out becomes in */
        var map = this.getMap();
        var zoomIn = this.zoomIn;
        if (this.event && this.event.shiftKey) {
            zoomIn = !zoomIn;
        }
        if (arguments.length == 2) {
            nRight = nLeft;
            nTop = nBottom;
        }
        var ratio = this.nFactor;
        var pWidth = Math.abs(nRight-nLeft);
        var pHeight = Math.abs(nBottom-nTop);
        if (pWidth > this.nTolerance ||
            pHeight > this.nTolerance) {
            ratio = Math.min(map._nWidth/pWidth,map._nHeight/pHeight);
        }
        if (!zoomIn) {
            ratio = ratio * -1;
        }
        var gMin = map.pixToGeo(nLeft,nBottom);
        var gMax = map.pixToGeo(nRight,nTop);
        var gCenter = {x:(gMin.x+gMax.x)/2,y:(gMin.y+gMax.y)/2}; 
        map.zoom(gCenter.x, gCenter.y,ratio);
        //map.setExtents(new OpenLayers.Bounds(gMin.x,gMin.y,gMax.x,gMax.y));
    },

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
            this.deactivateRectTool();
            this.activateRectTool();
        }
    }
};
