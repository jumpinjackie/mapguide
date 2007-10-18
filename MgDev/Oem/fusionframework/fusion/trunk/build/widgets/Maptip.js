/**
 * Fusion.Widget.Maptip
 *
 * $Id: Maptip.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.Maptip
 *
 * Displays tooltips over the map when the mouse is hovered for some 
 * time.  You must configure tooltips for each layer using Studio
 * or Web Studio by editing the LayerDefinition Settings and
 * specifying an expression for the tooltip.
 *
 *
 * Delay (optional)
 *
 * This is the delay, in milliseconds, that the user must keep the mouse
 * in the same position in order for the maptip to appear.  The default,
 * if not specified, is 350 milliseconds.
 *
 * Layer (optional, multiple)
 *
 * This is the name of a layer from the MapDefinition to get the tooltip
 * from.  If no Layer elements are specified, then all layers will be
 * queried and the top-most one will be displayed.  Multiple Layer tags
 * can be added, allowing tooltips to come from different layers.
 *
 * **********************************************************************/

Fusion.Widget.Maptip = Class.create();
Fusion.Widget.Maptip.prototype = 
{
    oCurrentPosition: null,
    oMapTipPosition: null,
    nTimer: null,
    delay: null,
    aLayers: null,
    bOverTip: false,
    
    initialize : function(widgetTag)
    {
      //console.log('Maptip.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        var json = widgetTag.extension;
        
        this.delay = json.Delay ? parseInt(json.Delay[0]) : 350;
        
        this.aLayers = [];
        if (json.Layer) {
            for (var i=0; i<json.Layer.length; i++) {
                this.aLayers.push(json.Layer[i]);
            }
        }
        
        this.domObj.parentNode.removeChild(this.domObj);
        this.domObj.style.position = 'absolute';
        this.domObj.style.display = 'none';
        this.domObj.style.top = '0px';
        this.domObj.style.left = '0px';
        this.domObj.style.zIndex = 101;
        
        Event.observe(this.domObj, 'mouseover', this.mouseOverTip.bind(this));
        Event.observe(this.domObj, 'mouseout', this.mouseOutTip.bind(this));
        
        var oDomElem =  this.getMap().getDomObj();
        oDomElem.appendChild(this.domObj);
        
        this.getMap().observeEvent('mousemove', this.mouseMove.bind(this));
        this.getMap().observeEvent('mouseout', this.mouseOut.bind(this));
        
    },
    
    mouseOut: function(e) {
      //console.log('maptip mouseOut');
        if (this.nTimer) {
            window.clearTimeout(this.nTimer);
            if (!this.nHideTimer) {
                /*console.log('mouseOut: set hide timer');*/
                this.nHideTimer = window.setTimeout(this.hideMaptip.bind(this), 250);
            }
        }
    },
    
    mouseMove: function(e) {
      //console.log('map tip mouseMove');
        if (this.bOverTip) {
            return;
        }
        var p = this.getMap().getEventPosition(e);
        if (!this.oCurrentPosition) {
            this.oCurrentPosition = p;
        } else {
            window.clearTimeout(this.nTimer);
            this.nTimer = null;
            if (this.oMapTipPosition && this.bIsVisible &&
                Math.abs(this.oMapTipPosition.x -this.oCurrentPosition.x) > 3 &&
                Math.abs(this.oMapTipPosition.y - this.oCurrentPosition.y) > 3) {
                /*console.log('mouseMove: set hide timer');*/
                this.nHideTimer = window.setTimeout(this.hideMaptip.bind(this), 250);
            }
            this.oCurrentPosition = p;
        }
        this.nTimer = window.setTimeout(this.showMaptip.bind(this), this.delay);
        //Event.stop(e);
    },
    
    showMaptip: function(r) {
      //console.log('showMaptip');
        var map = this.getMap();
        if (map == null) {
          return;
        }
        var cellSize = map._nCellSize;
        cellSize = 1e-6;

        var oBroker = Fusion.oBroker;
        var x = this.oCurrentPosition.x;
        var y = this.oCurrentPosition.y;
        var min = map.pixToGeo(x, y);
        //this can fail if no map is loaded
        if (!min) {
            return;
        }
        min.x -= cellSize;
        min.y -= cellSize;
        var max = map.pixToGeo(x, y);
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

    },
    _display: function(r) {
      //console.log('maptip _display');
        if (r.responseXML) {
            this.bIsVisible = true;
            this.oMapTipPosition = {x:this.oCurrentPosition.x, y: this.oCurrentPosition.y};
            var d = new DomNode(r.responseXML);
            var t = d.getNodeText('Tooltip');
            if (t != '') {
                t = t.replace(/\\n/g, "<br>");
                this.domObj.innerHTML = t;
                this.domObj.style.visibility = 'hidden';
                this.domObj.style.display = 'block';
                var size = Element.getDimensions(this.domObj);
                this.domObj.style.top = (this.oCurrentPosition.y - size.height) + 'px';
                this.domObj.style.left = this.oCurrentPosition.x + 'px';
                this.domObj.style.visibility = 'visible';
            }
        } else {
            this.bIsVisible = false;
        }
    },
    
    hideMaptip: function() {
      //console.log('hideMaptip');
        this.bIsVisible = false;
        this.hideTimer = window.setTimeout(this._hide.bind(this),250);
    },
    
    _hide: function() {
      //console.log('maptip _hide');
        this.hideTimer = null;
        this.domObj.style.display = 'none';
        this.domObj.innerHTML = '&nbsp;';
        this.oMapTipPosition = null;
    },
    
    mouseOverTip: function() {
      //console.log('mouseOverTip');
        window.clearTimeout(this.nHideTimer);
        this.nHideTimer = null;
        this.bOverTip = true;
    },
    
    mouseOutTip: function() {
      //console.log('mouseOutTip');
        this.nHideTimer = window.setTimeout(this.hideMaptip.bind(this), 250);
        this.bOverTip = false;
    }
    
};
