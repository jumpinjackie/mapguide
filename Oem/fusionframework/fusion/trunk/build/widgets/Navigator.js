/********************************************************************** * 
 * $Id: $
 *
 * In-map navigator
 *
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
 * 
 *
 * **********************************************************************/

Fusion.Widget.Navigator = Class.create();
Fusion.Widget.Navigator.prototype = {
    bInternalChange: false,
    zoomFactor: 2,
    panAmount: 50,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);

        var m = document.createElement('map');
        m.name = 'Navigator_ImageMap';
        m.id = 'Navigator_ImageMap';
        
        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = 'Pan East';
        a.title = 'Pan East';
        a.coords = '27,176, 27,177, 40,190, 44,182, 44,159';
        a.href='javascript:void(0);';
        a.onclick = this.pan.bind(this, this.panAmount/100, 0);
        m.appendChild(a);
        
        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = 'Pan West';
        a.title = 'Pan West';
        a.coords = '24,177, 24,176, 7,159, 7,182, 11,190';
        a.href='javascript:void(0);';
        a.onclick = this.pan.bind(this, -this.panAmount/100, 0);
        m.appendChild(a);
        
        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = 'Pan South';
        a.title = 'Pan South';
        a.coords = '25,178, 12,191, 21,197, 30,197, 39,191, 26,178';
        a.href='javascript:void(0);';
        a.onclick = this.pan.bind(this, 0, -this.panAmount/100);
        m.appendChild(a);
        
        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = 'Pan North';
        a.title = 'Pan North';
        a.coords = '26,175, 43,158, 8,158, 25,175';
        a.href='javascript:void(0);';
        a.onclick = this.pan.bind(this, 0, this.panAmount/100);
        m.appendChild(a);
        
        var a = document.createElement('area');
        a.shape = 'circle';
        a.alt = 'Zoom Out';
        a.title = 'Zoom Out';
        a.coords = '25,142,8';
        a.href='javascript:void(0);';
        a.onclick = this.zoom.bind(this, 1/this.zoomFactor);
        m.appendChild(a);
        
        var a = document.createElement('area');
        a.shape = 'circle';
        a.alt = 'Zoom In';
        a.title = 'Zoom In';
        a.coords = '25,34,8';
        a.href='javascript:void(0);';
        a.onclick = this.zoom.bind(this, this.zoomFactor);
        m.appendChild(a);
        
        this.domObj.appendChild(m);
        
        var sliderBg = document.createElement('img');
        sliderBg.src = Fusion.getFusionURL() + widgetTag.location + 'Navigator/sliderscale.png';
        sliderBg.className = 'png24';
        sliderBg.width = 51;
        sliderBg.height = 201;
        sliderBg.style.position = 'absolute';
        sliderBg.style.left = '0px';
        sliderBg.style.top = '0px';
        sliderBg.useMap = '#Navigator_ImageMap';
        this.domObj.appendChild(sliderBg);
        
        var handleDiv = document.createElement('div');
        handleDiv.style.position = 'absolute';
        handleDiv.style.top = '6px';
        handleDiv.style.left = '6px';
        handleDiv.style.width = '39px';
        handleDiv.style.height = '16px';
        this.domObj.appendChild(handleDiv);
        
        var sliderDiv = document.createElement('div');
        sliderDiv.style.position = 'absolute';
        sliderDiv.style.top = '43px';
        sliderDiv.style.left = '0px';
        sliderDiv.style.width = '51px';
        sliderDiv.style.height = '91px';
        this.domObj.appendChild(sliderDiv);

        var sliderHandle = document.createElement('img');
        sliderHandle.src = Fusion.getFusionURL() + widgetTag.location + 'Navigator/slider.png';
        sliderHandle.className = 'png24';
        sliderHandle.width = 31;
        sliderHandle.height = 14;
        sliderHandle.style.position = 'absolute';
        sliderHandle.style.left = '10px';
        sliderHandle.style.top = '49px';
        sliderDiv.appendChild(sliderHandle);
        
        this.domObj.style.position = 'absolute';
        this.domObj.style.zIndex = 1000;
        this.domObj.style.width = '51px';
        this.domObj.style.height = '204px';
        this.domObj.style.cursor = 'pointer';
        
        var checkPosition = this.checkPosition.bind(this);
    
    if (this.domObj.currentStyle) {
      if (this.domObj.currentStyle.left == 'auto') {
        var pDim = Element.getDimensions(this.domObj.parentNode);
        var nRight = parseInt(this.domObj.currentStyle.right);
        var navDim = Element.getDimensions(this.domObj);
        this.domObj.style.left = (pDim.width - nRight - navDim.width) + 'px';
      }
    }
        //set up the navigator as draggable
        new Draggable(this.domObj, {handle: handleDiv, starteffect: false, endeffect: false});
        //this observer pins the navigator to the top right after a drag so
        //that it moves if the window is resized
        var observer = {
            element: this.domObj,
            onStart: function() { },
            onEnd: checkPosition
        };
        //this should position the nav tool by the right rather than the left,
        //but it is broken in IE
        Draggables.addObserver(observer);
        
        var options = {};
        options.axis = 'vertical';
        options.range = $R(1, 91);        
        options.sliderValue = 91;
        options.onChange = this.scaleChanged.bind(this);
        this.slider = new Control.Slider(sliderHandle,sliderDiv, options);
        this.slider.setDisabled();
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.updateSlider.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.updateValue.bind(this));
    },
    
    scaleChanged: function(value) {
        if (!this.bInternalChange) {
            var map = this.getMap();
            var center = map.getCurrentCenter();
            var size = map.getSize();
            var w_deg = size.w * value;
            var h_deg = size.h * value;
            map.setExtents(new OpenLayers.Bounds(center.x - w_deg / 2,
                                               center.y - h_deg / 2,
                                               center.x + w_deg / 2,
                                               center.y + h_deg / 2));
        }
    },
    
    checkPosition: function() {
        var nav = this.domObj;
        var pDim = Element.getDimensions(nav.parentNode);
        var nLeft, nTop;
    nLeft = parseInt(nav.style.left);
    nTop = parseInt(nav.style.top);
    if (nLeft + nav.width > pDim.width) {
            nLeft = pDim.width - nav.width;
            nav.style.left = nLeft + 'px';
        }
        if (nTop + nav.height > pDim.height) {
            nTop = pDim.height - nav.height;
            nav.style.top = nTop + 'px';
        }
        if (nLeft < 0) {
            nav.style.left = '0px';
        }
        if (nTop < 0) {
            nav.style.top = '0px';
        }
    },
    
    updateSlider: function() {
        var olMap = this.getMap().oMapOL;
        if (olMap.baseLayer.singleTile) {
            this.slider.values = [];
            this.slider.range = $R(olMap.baseLayer.minResolution,olMap.baseLayer.maxResolution);
            this.bInternalChange = true;
            this.slider.setValue(olMap.getResolution());
            this.bInternalChange = false;
        } else {
            var n = olMap.resolutions.length;
            var max = olMap.resolutions[0];
            var min = olMap.resolutions[n-1];
            this.slider.values = [];
            this.slider.range = $R(1,91);
            for (var i=0; i<n; i++) {
                var r = olMap.resolutions[i];
                this.slider.values.push(parseInt((r/max)*91));
            }
        }
        this.slider.setEnabled();
    },
    
    updateValue: function() {
        var olMap = this.getMap().oMapOL;
        this.bInternalChange = true;
        this.slider.setValue(olMap.getResolution());
        this.bInternalChange = false;
    },
    
    pan: function(x,y) {
        //console.log('pan by : ' + x + ', ' + y);
        var map = this.getMap();
        var center = map.getCurrentCenter();
        var res = map.oMapOL.getResolution();
        var size = map.oMapOL.getSize();
        map.zoom(center.x + (x * size.w * res), center.y + (y * size.h * res), 1);
    },
    
    zoom: function(factor) {
        //console.log('zoom by factor: ' + factor);
        var map = this.getMap();
        var center = map.getCurrentCenter();
        map.zoom(center.x, center.y, factor);
    }
    
};
