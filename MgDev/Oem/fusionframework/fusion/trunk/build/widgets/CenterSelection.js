/********************************************************************** * 
 * @project Fusion
 * @revision $Id: CenterSelection.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose ClearSelection widget
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
 * 
 ********************************************************************
 *
 * Center the current selection, if any, but maintain the current scale
 * if possible.  Zoom out if not.
 *
 * To put a CenterSelection widget in your application, you first need to add
 * a widget to your WebLayout as follows:
 *
 * <Command xsi:type="FusionCommandType">
 *   <Name>MyCenterSelection</Name>
 *   <Label>Center Selection</Label>
 *   <TargetViewer>All</TargetViewer>
 *   <Action>CenterSelection</Action>
 *   <ImageURL>images/icon_zoom_selected.png</ImageURL>
 * </Command>
 *
 * The important parts of this Command are:
 *
 * Name (string, mandatory) 
 * 
 * an element with an id that is the same as this name must be in
 * the application.  For instance:
 *
 * <div id="MyCenterSelection"></div>
 *
 * A button that activates this widget will appear inside the
 * element you provide.
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
