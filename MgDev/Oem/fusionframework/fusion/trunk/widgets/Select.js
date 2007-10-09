/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Select.js 868 2007-10-08 23:08:27Z assefa $
 * @purpose Select widget
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
 * perform a selection using the map guide web layout configuration file
 * 
 * **********************************************************************/

Fusion.Widget.Select = Class.create();
Fusion.Widget.Select.prototype =  {       
    selectionType: 'INTERSECTS',
    nTolerance : 3, //default pixel tolernace for a point click
    bActiveOnly: false, //only select feature(s) on the active layer?
    initialize : function(widgetTag) {
        //console.log('Select.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.Rectangle.prototype, []);
        this.asCursor = ['auto'];
        
        this.enable = Fusion.Widget.Select.prototype.enable;

        var json = widgetTag.extension;
        
        this.selectionType = json.SelectionType ? json.SelectionType[0] : 'INTERSECTS';
        
        if (json.Tolerance && (parseInt(json.Tolerance[0]) > 0)) {
            nTolerance = parseInt(json.Tolerance[0]);
        }

        this.bActiveOnly = (json.QueryActiveLayer &&
                           (json.QueryActiveLayer[0] == 'true' ||
                            json.QueryActiveLayer[0] == '1')) ? true : false;
        
        if (this.bActiveOnly) {
            this.getMap().registerForEvent(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED, this.enable.bind(this));
        }
        
    },
    
    enable: function() {
        if (this.bActiveOnly) {
            var layer = this.getMap().getActiveLayer();
            if (layer && layer.selectable) { 
                Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
            } else {
                this.disable();
            }
        } else {
            Fusion.Tool.ButtonBase.prototype.enable.apply(this,[]);
        }
    },
    
    /**
     * called when the button is clicked by the ButtonBase widget
     */
    activateTool : function() {
        this.getMap().activateWidget(this);
        //this.activate();
    },

    /**
     * activate the widget (listen to mouse events and change cursor)
     * This function should be defined for all functions that register
     * as a widget in the map
     */
    activate : function() {
        this.activateRectTool();
        this.getMap().setCursor(this.asCursor);
        /*icon button*/
        this._oButton.activateTool();
    },

    /**
     * deactivate the widget (listen to mouse events and change cursor)
     * This function should be defined for all functions that register
     * as a widget in the map
     **/
    deactivate : function() {
         this.deactivateRectTool();
         this.getMap().setCursor('auto');
         /*icon button*/
         this._oButton.deactivateTool();
    },

    /**
     *  set the extants of the map based on the pixel coordinates
     * passed
     * 
     * @param nLeft integer pixel coordinates of the left (minx)
     * @param nBottom integer pixel coordinates of the bottom (miny)
     * @param nRight integer pixel coordinates of the right (maxx)
     * @param nTop integer pixel coordinates of the top (maxy)
     **/
    execute : function(nLeft, nBottom, nRight, nTop) {

      /*ctrl click is used to launch a URL defined on the feature. See ClickCTRL widget*/
      if (this.event.ctrlKey) {
        return;
      }
        if (arguments.length == 2) {
            nRight = nLeft;
            nTop = nBottom;
        }
        var sMin = this.getMap().pixToGeo(nLeft,nBottom);
        var sMax = this.getMap().pixToGeo(nRight,nTop);
        var nXDelta = Math.abs(nLeft-nRight);
        var nYDelta = Math.abs(nBottom- nTop);
        
        var options = {};
        if (nXDelta <=this.nTolerance && nYDelta <=this.nTolerance) {
            var dfGeoTolerance = this.getMap().pixToGeoMeasure(this.nTolerance);
            sMin.x = sMin.x-dfGeoTolerance;
            sMin.y = sMin.y-dfGeoTolerance;
            sMax.x = sMax.x+dfGeoTolerance;
            sMax.y = sMax.y+dfGeoTolerance;
        }
        
        options.geometry = 'POLYGON(('+ sMin.x + ' ' +  sMin.y + ', ' +  sMax.x + ' ' +  sMin.y + ', ' + sMax.x + ' ' +  sMax.y + ', ' + sMin.x + ' ' +  sMax.y + ', ' + sMin.x + ' ' +  sMin.y + '))';
        options.selectionType = this.selectionType;

        if (this.bActiveOnly) {
            var layer = this.getMap().getActiveLayer();
            if (layer) {
                options.layers = layer.layerName;
            } else {
                return;
            }
        }
        
        if (this.event.shiftKey) {
            options.extendSelection = true;
        }
        
        this.getMap().query(options);
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
