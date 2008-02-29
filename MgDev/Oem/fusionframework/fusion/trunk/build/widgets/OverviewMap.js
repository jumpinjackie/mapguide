/**
 * Fusion.Widget.OverviewMap
 *
 * $Id: OverviewMap.js 1196 2008-01-21 21:45:35Z madair $
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
 * Class: Fusion.Widget.OverviewMap
 *
 * A widget that displays an overview map showing the current view of the 
 * primary map.
 * **********************************************************************/

Fusion.Widget.OverviewMap = Class.create();
Fusion.Widget.OverviewMap.prototype = {
    oSize: null,
    nMinRatio : 4,
    nMaxRatio : 32,
    bDisplayed : false,
  
    initialize : function(widgetTag) {
        //console.log('OverviewMap.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        var json = widgetTag.extension;
        if (json.MinRatio) {
            this.nMinRatio = json.MinRatio[0];
        }
        if (json.MaxRatio) {
            this.nMaxRatio = json.MaxRatio[0];
        }

        var mapTag = null;
        if (json.MapId) {
          this.sMapGroupId = json.MapId;
          var mapGroup = Fusion.applicationDefinition.getMapGroup(this.sMapGroupId);
          mapTag = mapGroup.maps[0];    //TODO: always use the baselayer Map in the group?
        } else {
          var mainMap = this.getMap();
          mapTag = mainMap.mapGroup.maps[0];    //TODO: always use the baselayer Map in the group?
        }
        this.mapObject = eval("new Fusion.Maps."+mapTag.type+"(this.getMap(),mapTag,false)");
        this.mapObject.registerForEvent(Fusion.Event.MAP_LOADED, this.loadOverview.bind(this));

        //first set the size to the size of the DOM element if available
        if (this.domObj) {
              this.domObj.style.overflow = 'hidden';
              if (this.domObj.jxLayout) {
                  this.domObj.jxLayout.addSizeChangeListener(this);
              } else {
                  this.domObj.resize = this.sizeChanged.bind(this);
              }
        }
        
        this.oMapOptions = {};  //TODO: allow setting some mapOptions in AppDef

        //this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapWidgetLoaded.bind(this));
    },
    
    mapWidgetLoaded: function() 
    {
        var mapWidget = this.getMap();
        if (this.sMapGroupId && (mapWidget.projection == this.mapObject.projection) ) {
          this.loadOverview([this.mapObject.oLayerOL]);
        } else {
          //just use the base map layer
          var extent = this.oMap._oCurrentExtents;
          this.loadOverview([this.getMap().oMapOL.baseLayer.clone()]);
        }
    },

    keymapLoaded: function() 
    {
        this.mapObject.oLayerOL.isBaseLayer = true;  
    },

    loadOverview: function() 
    {
        if (this.control) {
          this.control.destroy();
        }
        
        var size = Element.getContentBoxSize(this.domObj);
        this.oSize = new OpenLayers.Size(size.width, size.height);
        
        this.mapObject.oLayerOL.isBaseLayer = true;  
        if (this.mapObject.oLayerOL.singleTile) {
          this.oMapOptions.numZoomLevels = 3;  //TODO: make this configurable?
        }

        this.mapObject.oLayerOL.ratio = 1.0;
        var mapOpts = {
          div: this.domObj,
          size: this.oSize,
          minRatio: this.nMinRatio,
          maxRatio: this.nMaxRatio,
          mapOptions: this.oMapOptions,
          layers: [this.mapObject.oLayerOL]
        };

        this.control = new OpenLayers.Control.OverviewMap(mapOpts);
        if (size.width == 0 || size.height == 0) {
          return;   //don't try to load if the container is not visible
        } else {
          this.getMap().oMapOL.addControl(this.control);
          this.bDisplayed = true;
        }
        //console.log('OverviewMap mapLoaded');
    },
    
    sizeChanged: function() {
        var size = Element.getContentBoxSize(this.domObj);
        this.oSize = new OpenLayers.Size(size.width, size.height);
        if (size.width == 0 || size.height == 0) {
          return;   //don't try to load if the container is not visible
        } 
        if (!this.bDisplayed && this.control) {
          this.getMap().oMapOL.addControl(this.control);
          this.bDisplayed = true;
        }
        if (this.control) {
            this.control.size = new OpenLayers.Size(size.width, size.height);
            this.control.mapDiv.style.width = this.oSize.w + 'px';
            this.control.mapDiv.style.height = this.oSize.h + 'px';
            this.control.ovmap.updateSize();
            this.control.update();
        }
    }

};
      
