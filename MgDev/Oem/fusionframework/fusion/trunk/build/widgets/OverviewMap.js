/********************************************************************** * 
 * @project Fusion
 * @revision $Id: OverviewMap.js 954 2007-10-16 01:29:03Z pspencer $
 * @purpose Key map widget
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
 * **********************************************************************/
Fusion.Widget.OverviewMap = Class.create();
Fusion.Widget.OverviewMap.prototype = {
    oSize: null,
    nMinRatio : 32,
    nMaxRatio : 128,
  
    initialize : function(widgetTag) {
        //console.log('OverviewMap.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        var json = widgetTag.extension;

        if (json.MapId) {
          this.sMapGroupId = json.MapId;
        }
        if (json.MinRatio) {
            this.nMinRatio = json.MinRatio[0];
        }
        if (json.MaxRatio) {
            this.nMaxRatio = json.MaxRatio[0];
        }

        //first set the size to the size of the DOM element if available
        if (this.domObj) {
              var size = Element.getContentBoxSize(this.domObj);
              this.oSize = new OpenLayers.Size(size.width, size.height);
              this.domObj.style.overflow = 'hidden';
              if (this.domObj.jxLayout) {
                  this.domObj.jxLayout.addSizeChangeListener(this);
              } else {
                  this.domObj.resize = this.sizeChanged.bind(this);
              }
        }

        this.oMapOptions = {};  //TODO: allow setting some mapOptions in AppDef

        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapWidgetLoaded.bind(this));
    },
    
    mapWidgetLoaded: function() 
    {
        if (this.sMapGroupId) {
          var mapGroup = Fusion.applicationDefinition.getMapGroup(this.sMapGroupId);
          var mapTag = mapGroup.maps[0];    //TODO: always use the baselayer Map in the group?
          this.mapObject = eval("new Fusion.Maps."+mapTag.type+"(this.getMap(),mapTag,false)");
          this.mapObject.registerForEvent(Fusion.Event.MAP_LOADED, this.keymapLoaded.bind(this));
	} else {
          //just use the base map layer
          var extent = this.oMap._oCurrentExtents;
          this.loadOverview([this.getMap().oMapOL.baseLayer.clone()]);
        }
    },

    keymapLoaded: function() 
    {
        this.mapObject.oLayerOL.isBaseLayer = true;  
        this.loadOverview([this.mapObject.oLayerOL]);
    },

    loadOverview: function(aLayers) 
    {
        aLayers[0].ratio = 1.0;

        var mapOpts = {
          div: this.domObj,
          size: this.oSize,
          minRatio: this.nMinRatio,
          maxRatio: this.nMaxRatio,
          mapOptions: this.oMapOptions,
          layers: aLayers
        };

        this.control = new OpenLayers.Control.OverviewMap(mapOpts);
        this.getMap().oMapOL.addControl(this.control);
        //console.log('OverviewMap mapLoaded');
    },
    
    sizeChanged: function() {
        var size = Element.getContentBoxSize(this.domObj);
        this.oSize = new OpenLayers.Size(size.width, size.height);
        if (this.control) {
            this.control.size = new OpenLayers.Size(size.width, size.height);
            this.control.mapDiv.style.width = this.oSize.w + 'px';
            this.control.mapDiv.style.height = this.oSize.h + 'px';
            this.control.ovmap.updateSize();
            this.control.update();
        }
    }

};
      
