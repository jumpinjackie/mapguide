/**
 * Fusion.Widget.Navigator
 *
 * $Id: $
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
 * Class: Fusion.Widget.Navigator
 *
 * A widget that immplements an in-map navigation control with zoom and pan.
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
        a.alt = OpenLayers.String.translate('panEast');
        a.title = OpenLayers.String.translate('panEast');
        a.coords = '27,176, 27,177, 40,190, 44,182, 44,159';
        Event.observe(a, 'mouseup', this.pan.bindAsEventListener(this, this.panAmount/100, 0) );
        m.appendChild(a);

        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = OpenLayers.String.translate('panWest');
        a.title = OpenLayers.String.translate('panWest');
        a.coords = '24,177, 24,176, 7,159, 7,182, 11,190';
        Event.observe(a, 'mouseup', this.pan.bindAsEventListener(this, -this.panAmount/100, 0) );
        m.appendChild(a);

        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = OpenLayers.String.translate('panSouth');
        a.title = OpenLayers.String.translate('panSouth');
        a.coords = '25,178, 12,191, 21,197, 30,197, 39,191, 26,178';
        Event.observe(a, 'mouseup', this.pan.bindAsEventListener(this, 0, -this.panAmount/100) );
        m.appendChild(a);

        var a = document.createElement('area');
        a.shape = 'poly';
        a.alt = OpenLayers.String.translate('panNorth');
        a.title = OpenLayers.String.translate('panNorth');
        a.coords = '26,175, 43,158, 8,158, 25,175';
        Event.observe(a, 'mouseup', this.pan.bindAsEventListener(this, 0, this.panAmount/100) );
        m.appendChild(a);

        var a = document.createElement('area');
        a.shape = 'circle';
        a.alt = OpenLayers.String.translate('zoomOut');
        a.title = OpenLayers.String.translate('zoomOut');
        a.coords = '25,142,8';
        Event.observe(a, 'mouseup', this.zoom.bindAsEventListener(this, 1/this.zoomFactor) );
        m.appendChild(a);

        var a = document.createElement('area');
        a.shape = 'circle';
        a.alt = OpenLayers.String.translate('zoomIn');
        a.title = OpenLayers.String.translate('zoomIn');
        a.coords = '25,34,8';
        Event.observe(a, 'mouseup', this.zoom.bindAsEventListener(this, this.zoomFactor) );
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
        sliderDiv.style.top = '44px';
        sliderDiv.style.left = '0px';
        sliderDiv.style.width = '51px';
        sliderDiv.style.height = '88px';
        this.domObj.appendChild(sliderDiv);

        var sliderHandle = document.createElement('img');
        sliderHandle.src = Fusion.getFusionURL() + widgetTag.location + 'Navigator/slider.png';
        sliderHandle.className = 'png24';
        sliderHandle.width = 29;
        sliderHandle.height = 12;
        sliderHandle.style.position = 'absolute';
        sliderHandle.style.left = '11px';
        sliderHandle.style.top = '49px';
        sliderDiv.appendChild(sliderHandle);
        
        this.activityIndicator = document.createElement('img');
        this.activityIndicator.src = Fusion.getFusionURL() + widgetTag.location + 'Navigator/spinner.gif';
        this.activityIndicator.width = 18;
        this.activityIndicator.height = 6;
        this.activityIndicator.style.position = 'absolute';
        this.activityIndicator.style.top = '3px';
        this.activityIndicator.style.right = '4px';
        handleDiv.appendChild(this.activityIndicator);

        this.domObj.style.position = 'absolute';
        this.domObj.style.zIndex = 1000;
        this.domObj.style.width = '51px';
        this.domObj.style.height = '204px';
        this.domObj.style.cursor = 'pointer';

        var checkPosition = this.checkPosition.bind(this);

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
        options.onChange = this.scaleChanged.bindAsEventListener(this);
        this.slider = new Control.Slider(sliderHandle,sliderDiv, options);
        this.slider.setDisabled();
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.updateSlider.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_RESIZED, this.checkPosition.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.updateValue.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_BUSY_CHANGED, this.busyChanged.bind(this));
    },

    scaleChanged: function(e, value) {
        var map = this.getMap();
        var activeWidget = null;
        if (map.oActiveWidget) {
          activeWidget = map.oActiveWidget;
          map.deactivateWidget(map.oActiveWidget);
        }
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
        Event.stop(e);
        if (activeWidget) {
          map.activateWidget(activeWidget);
        }
        return false;
    },

    checkPosition: function() {
        var nav = this.domObj;
        var pDim = Element.getDimensions(nav.parentNode);
        var nLeft, nTop;
        nLeft = parseInt(nav.style.left);
        nTop = parseInt(nav.style.top);
        if (nLeft + nav.getWidth() > pDim.width) {
            nLeft = pDim.width - nav.getWidth();
            nav.style.left = nLeft + 'px';
        }
        if (nTop + nav.getHeight() > pDim.height) {
            nTop = pDim.height - nav.getHeight();
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
            var res = olMap.baseLayer.resolutions;
            var n = res.length;
            var max = res[0];
            var min = res[n-1];
            this.slider.values = [];
            this.slider.range = $R(1,91);
            for (var i=0; i<n; i++) {
                var r = res[i];
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

    pan: function(e,x,y) {
        //console.log('pan by : ' + x + ', ' + y);
        var map = this.getMap();
        var activeWidget = null;
        if (map.oActiveWidget) {
          activeWidget = map.oActiveWidget;
          map.deactivateWidget(map.oActiveWidget);
        }
        var center = map.getCurrentCenter();
        var res = map.oMapOL.getResolution();
        var size = map.oMapOL.getSize();
        map.zoom(center.x + (x * size.w * res), center.y + (y * size.h * res), 1);
        Event.stop(e);
        if (activeWidget) {
          map.activateWidget(activeWidget);
        }
        
        return false;
    },

    zoom: function(e, factor) {
        //console.log('zoom by factor: ' + factor);
        var map = this.getMap();
        var activeWidget = null;
        if (map.oActiveWidget) {
          activeWidget = map.oActiveWidget;
          map.deactivateWidget(map.oActiveWidget);
        }
        var center = map.getCurrentCenter();
        map.zoom(center.x, center.y, factor);
        Event.stop(e);
        if (activeWidget) {
          map.activateWidget(activeWidget);
        }
        return false;
    },
    
    busyChanged: function() {
        this.activityIndicator.style.visibility = this.getMap().isBusy() ? 'visible' : 'hidden';
    }
    

};
