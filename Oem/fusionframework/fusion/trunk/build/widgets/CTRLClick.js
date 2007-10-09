/********************************************************************** * 
 * @project Fusion
 * @revision $Id: $
 * @purpose CTRLClick
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
 * Launch a window with a CTRL click  if a URL expression is set on the layer
 * **********************************************************************/

Fusion.Widget.CTRLClick = Class.create();
Fusion.Widget.CTRLClick.prototype = 
{
    aLayers: null,

    initialize : function(widgetTag)
    {
        //console.log('CTRLClick.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        var json = widgetTag.extension;

        this.aLayers = [];
        if (json.Layer) {
            for (var i=0; i<json.Layer.length; i++) {
                this.aLayers.push(json.Layer[i]);
            }
        }

        this.getMap().observeEvent('mouseup', this.mouseUpCRTLClick.bind(this));
    },

    /**
     * called when there is a click on the map: query features at that postion.
     **/
    mouseUpCRTLClick: function(e) {
        if (e.ctrlKey) 
        {
            var map = this.getMap();
            var p = map.getEventPosition(e);
            var min = map.pixToGeo(p.x, p.y);
            var max = map.pixToGeo(p.x, p.y);
            if (!min) {
              return;
            }   
            var oBroker = Fusion.oBroker;
            var cellSize = map._nCellSize;
            //cell size not set in map file. Use default value
            cellSize = 1e-6;

            min.x -= cellSize;
            min.y -= cellSize;

            max.x += cellSize;
            max.y += cellSize;

            var sGeometry = 'POLYGON(('+ min.x + ' ' +  min.y + ', ' +  min.x + ' ' +  max.y + ', ' + max.x + ' ' +  max.y + ', ' + max.x + ' ' +  min.y + ', ' + min.x + ' ' +  min.y + '))';

            //var sGeometry = 'POINT('+ min.x + ' ' +  min.y + ')';

            var maxFeatures = 1;
            var persist = 0;
            var selection = 'INTERSECTS';
            var maps = this.getMap().getAllMaps();
            //TODO: possibly make the layer names configurable?
            var layerNames = this.aLayers.toString();
            var r = new Fusion.Lib.MGRequest.MGQueryMapFeatures(maps[0].getSessionID(),
                                                                maps[0]._sMapname,
                                                                sGeometry,
                                                                maxFeatures, persist, selection, layerNames);
            oBroker.dispatchRequest(r, 
                                    this._display.bind(this));
        }
    },

    /**
     * open a window if a URL is defined for the feature.
     **/
    _display: function(r) {
        //console.log('ctrlclcik  _display');
        if (r.responseXML) 
        {
            var d = new DomNode(r.responseXML);
            var h = d.getNodeText('Hyperlink');
            if (h != '') 
            {
                window.open(h, "");
            }
        
        }
    }

};
