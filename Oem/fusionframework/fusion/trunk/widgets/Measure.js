/**
 * Fusion.Widget.Measure
 *
 * $Id: Measure.js 1396 2008-05-08 15:34:30Z madair $
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

 /* ********************************************************************
 * Class: Fusion.Widget.Measure
 *
 * The Measure widget allows the user to measure distances or areas on the map 
 * in one or more segments. Area is positive if measured clockwise.
 * 
 * **********************************************************************/

Fusion.Constant.MEASURE_TYPE_DISTANCE = 0;
Fusion.Constant.MEASURE_TYPE_AREA = 1;
Fusion.Constant.MEASURE_TYPE_BOTH = 2;

Fusion.Event.MEASURE_SEGMENT_UPDATE = Fusion.Event.lastEventId++;
Fusion.Event.MEASURE_CLEAR = Fusion.Event.lastEventId++;
Fusion.Event.MEASURE_COMPLETE = Fusion.Event.lastEventId++;

Fusion.Widget.Measure = Class.create();
Fusion.Widget.Measure.prototype = {
    isDigitizing: false,
    //distance of each segment
    distances: null,
    distanceMarkers: null,
    
    //Array of points used to compute area
    aAreaFirstPoint: null,
    //cumulativeDistance
    cumulativeDistance: 0,
    lastDistance: 0,
    //for areas
    //cumulativeArea
    cumulativeArea: 0,
    lastArea: 0,
    
    /* the units to display distances in */
    units: Fusion.UNKNOWN,

    /* Type of measure: values = disance, area or both, default: both*/
    mType: null,

    /* Precision of the distance displayed */
    distPrecision: 4,
    
    /* Precision of the area displayed */
    areaPrecision: 4,
    
    /* Style for the distance line used for distance draw */   
    distanceNormalStyle: null,

    /* Style for the polygon used for area draw */   
    fillStyle: null,

    /* Style for the polygon line used for area draw */    
    areaStyle: null,
    
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.Canvas.prototype, []);
        this.asCursor = ['crosshair'];
        var json = widgetTag.extension;
        this.units = (json.Units && (json.Units[0] != '')) ?  Fusion.unitFromName(json.Units[0]): this.units;
        this.distPrecision = json.DistancePrecision ? parseInt(json.DistancePrecision[0]) : 4;
        this.areaPrecision = json.AreaPrecision ? parseInt(json.AreaPrecision[0]) : 4;  
        
        this.sTarget = json.Target ? json.Target[0] : "";
        this.sBaseUrl = Fusion.getFusionURL() + 'widgets/Measure/Measure.php';
        
              
        //init measure type
        this.measureType = Fusion.Constant.MEASURE_TYPE_BOTH;
        if (json.Type) {
            switch(json.Type[0].toLowerCase()) {
                case 'distance':
                    this.measureType = Fusion.Constant.MEASURE_TYPE_DISTANCE;
                    break;
                case 'area':
                    this.measureType = Fusion.Constant.MEASURE_TYPE_AREA;
                    break;
            }
        }
        //we don't support area yet.
        this.measureType = Fusion.Constant.MEASURE_TYPE_DISTANCE;
        
        //Here are the canvas style definition
        var fillStyle = json.FillStyle ? json.FillStyle[0] : 'rgba(0,0,255, 0.3)';
        var lineStyleWidth = json.LineStyleWidth ? json.LineStyleWidth[0] : 2;
        var lineStyleColor = json.LineStyleColor ? json.LineStyleColor[0] : 'rgba(0,0,255,0.3)';     
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:fillStyle});
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:lineStyleWidth,strokeStyle:lineStyleColor});  	
        this.distanceMarkers = [];
        this.distances = [];
        
        this.registerEventID(Fusion.Event.MEASURE_SEGMENT_UPDATE);
        this.registerEventID(Fusion.Event.MEASURE_CLEAR);
        this.registerEventID(Fusion.Event.MEASURE_COMPLETE);
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.resetCanvas.bind(this));
        this.keyHandler = this.onKeyPress.bind(this);
        Fusion.addWidgetStyleSheet(widgetTag.location + 'Measure/Measure.css');

        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.setUnits.bind(this, this.units));
        this.registerParameter('Units');
    },
    
    onKeyPress: function(e) {
        //console.log('Rule::onKeyPress');
        var charCode = (e.charCode ) ? e.charCode : ((e.keyCode) ? e.keyCode : e.which);
        //console.log(charCode);
        if (charCode == Event.KEY_ESC) {
            this.resetCanvas();
        } 
    },
    
    /**
     * (public) activate()
     *
     * activate the measure tool
     */
    activateTool: function() {
        this.getMap().activateWidget(this);
        this._oButton.activateTool();
    },

    /**
     * (public) initVars()
     *
     * reset area and/or distance vars
     */    
    initVars: function() {
        this.cumulativeDistance = 0;
        this.lastDistance = 0;
        this.cumulativeArea = 0;
        this.lastArea = 0;
        this.aAreaFirstPoint = null;
    },
    
    activate: function() {
        this.activateCanvas();
        this.initVars();
        this.triggerEvent(Fusion.Event.MEASURE_CLEAR, this);
        Event.observe(document,"keypress",this.keyHandler);
        this.loadDisplayPanel();
    },
    
    loadDisplayPanel : function() {
        if (this.sTarget) {
            var url = this.sBaseUrl;
            var queryStr = 'locale='+Fusion.locale;
            if (url.indexOf('?') < 0) {
                url += '?';
            } else if (url.slice(-1) != '&') {
                url += '&';
            }
            url += queryStr;
            
            var taskPaneTarget = Fusion.getWidgetById(this.sTarget);
            var outputWin = window;
            if ( taskPaneTarget ) {
                taskPaneTarget.setContent(url);
                outputWin = taskPaneTarget.iframe.contentWindow;
            } else {
                outputWin = window.open(url, this.sTarget, this.sWinFeatures);
            }
            this.registerForEvent(Fusion.Event.MEASURE_CLEAR, this.clearDisplay.bind(this, outputWin));  
            this.registerForEvent(Fusion.Event.MEASURE_SEGMENT_UPDATE, this.updateDisplay.bind(this, outputWin));
            this.registerForEvent(Fusion.Event.MEASURE_COMPLETE, this.updateDisplay.bind(this, outputWin));
        } else {
            this.totalDistanceMarker = new Fusion.Widget.Measure.DistanceMarker(this.units, this.distPrecision, 'Total:');
            var oDomElem =  this.getMap().getDomObj();
            if (!this.totalDistanceMarker.domObj.parentNode || 
                this.totalDistanceMarker.domObj.parentNode != oDomElem) {
                oDomElem.appendChild(this.totalDistanceMarker.domObj);
            }
            Element.addClassName(this.totalDistanceMarker.domObj, 'divMeasureTotal');
            this.totalDistanceMarker.domObj.style.display = 'none';
            this.registerForEvent(Fusion.Event.MEASURE_CLEAR, this.clearTotalDistance.bind(this));  
            this.registerForEvent(Fusion.Event.MEASURE_SEGMENT_UPDATE, this.updateTotalDistance.bind(this));
            this.registerForEvent(Fusion.Event.MEASURE_COMPLETE, this.updateTotalDistance.bind(this));
        }
    },    
    
    /**
     * (public) deactivate()
     *
     * deactivate the ruler tool
     */
    deactivate: function() {
        //console.log('Ruler.deactivate');
        Event.stopObserving(document, 'keypress', this.keyHandler);           
        this._oButton.deactivateTool();
        this.deactivateCanvas();
        this.resetCanvas();
    },
    
    resetCanvas: function() {
        if (this.isDigitizing) {
            this.isDigitizing = false;
        }
        this.clearContext();
        this.initVars();
        for (var i=0; i<this.distanceMarkers.length; i++)  {
            this.distanceMarkers[i].destroy();
        }
        this.distanceMarkers = [];
        this.triggerEvent(Fusion.Event.MEASURE_CLEAR, this);
    },
      
    /**
     * (public) mouseDown(e)
     *
     * handle the mouse down event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseDown: function(e) {  	
        if (Event.isLeftClick(e)) {
            var map = this.getMap();
            var p = map.getEventPosition(e);
            var gp = map.pixToGeo(p.x, p.y);
            
            if (!this.isDigitizing) {
                this.resetCanvas();
                var from = new Fusion.Tool.Canvas.Node(gp.x,gp.y, map);
                var to = new Fusion.Tool.Canvas.Node(gp.x,gp.y, map);
                var lastSegment = new Fusion.Tool.Canvas.Segment(from,to);
                if (this.measureType == Fusion.Constant.MEASURE_TYPE_DISTANCE) {
                    this.feature = new Fusion.Tool.Canvas.Line(map);
                    this.feature.lineStyle = this.lineStyle;
                } else {
                    this.feature = new Fusion.Tool.Canvas.Polygon(map);
                    this.feature.fillStyle = this.fillStyle;
                    this.feature.lineStyle = this.lineStyle;
                }
                this.feature.addSegment(lastSegment);
                this.aAreaFirstPoint = new Fusion.Tool.Canvas.Node(gp.x,gp.y, map);
                this.isDigitizing = true;                                  
            } else {
                //if digitizing
                var lastSegment = this.feature.lastSegment();
                lastSegment.to.set(gp.x,gp.y);
                if (lastSegment.from.x == lastSegment.to.x && 
                    lastSegment.from.y == lastSegment.to.y) {
                    this.dblClick(e);
                    return;
                }
                this.feature.extendLine();
                this.updateMarker(this.lastMarker, lastSegment, e);
            }
            //create a new marker
            this.lastMarker = new Fusion.Widget.Measure.DistanceMarker(this.units, this.distPrecision);
            this.distanceMarkers.push(this.lastMarker);
            this.clearContext();
            this.feature.draw(this.context);
        }
    },

    /**
     * (public) mouseMove(e)
     *
     * handle the mouse move event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseMove: function(e) {
        if (!this.isDigitizing) {
            return;
        }
        var offset = {x:0,y:0};
        var oElement = this.getMap().getDomObj();
        //var target = e.target || e.srcElement;
        if (this.delayUpdateTimer) {
            window.clearTimeout(this.delayUpdateTimer);
        }
        var map = this.getMap();
        var p = map.getEventPosition(e);
        var gp = map.pixToGeo(p.x, p.y);
        
        var lastSegment = this.feature.lastSegment();
        lastSegment.to.set(gp.x,gp.y);
        this.clearContext();
        this.feature.draw(this.context);
        this.lastMarker.setCalculating();
        this.delayUpdateTimer = window.setTimeout(this.delayUpdate.bind(this, lastSegment, e), 100);
        
        this.positionMarker(this.lastMarker, lastSegment);
        if (this.totalDistanceMarker) {
          var size = this.totalDistanceMarker.getSize();
          this.totalDistanceMarker.domObj.style.top = (p.y - size.height) + 'px';
          this.totalDistanceMarker.domObj.style.left = p.x + 'px';
        }
    },
    
    delayUpdate: function(lastSegment, e) {
        this.delayUpdateTimer = null;
        this.updateMarker(this.lastMarker, lastSegment, e);
    },
   
    /**
     * (public) dblClick(e)
     *
     * handle the mouse dblclick event
     *
     * @param e Event the event that happened on the mapObj
     */
    dblClick: function(e) {
        //console.log('Digitizer.dblClick');
        if (!this.isDigitizing) {
            return;
        }
        var p = this.getMap().getEventPosition(e);
        var gp = this.getMap().pixToGeo(p.x, p.y);   
        var seg = this.feature.lastSegment();
        seg.to.set(gp.x,gp.y);
        this.clearContext();
        this.feature.draw(this.context);
        
        if (this.measureType == Fusion.Constant.MEASURE_TYPE_AREA || this.measureType == Fusion.Constant.MEASURE_TYPE_BOTH) {
            
        }
        if (this.measureType == Fusion.Constant.MEASURE_TYPE_DISTANCE || this.measureType == Fusion.Constant.MEASURE_TYPE_BOTH) {
        }  

        this.isDigitizing = false;
        
        //mousedown creates a new segment before dblClick so remove the last one
        var lastMarker = this.distanceMarkers.pop();
        lastMarker.destroy();
        this.triggerEvent(Fusion.Event.MEASURE_COMPLETE);                    
    },
    
    positionMarker: function(marker, segment) {
        var oDomElem =  this.getMap().getDomObj();
        if (!marker.domObj.parentNode || 
            marker.domObj.parentNode != oDomElem) {
            oDomElem.appendChild(marker.domObj);
        }
        var size = marker.getSize();
        var t = (segment.from.py + segment.to.py - size.height)/2 ;
        var l = (segment.from.px + segment.to.px - size.width)/2;
        marker.domObj.style.top = t + 'px';
        marker.domObj.style.left = l + 'px';
    },
    
    updateMarker: function(marker, segment, e) {
        if (!marker) {
            return;
        }
        this.measureSegment(segment, marker);
        this.positionMarker(marker, segment);
    },
    
    measureSegment: function(segment, marker) {
        var points = '&x1='+segment.from.x+'&y1='+segment.from.y+
                     '&x2='+segment.to.x+'&y2='+segment.to.y;
        var map = this.getMap();
        var aMaps = map.getAllMaps();
        var s = aMaps[0].arch + '/' + Fusion.getScriptLanguage() + "/Measure." + Fusion.getScriptLanguage() ;
        var sessionId = aMaps[0].getSessionID();
        var params = {};
        params.parameters = 'session='+sessionId+'&locale='+Fusion.locale+'&mapname='+ this.getMap().getMapName()+points;
        params.onComplete = this.measureCompleted.bind(this, segment, marker);
        Fusion.ajaxRequest(s, params);
    },
    
    measureCompleted: function(segment, marker, r, json) {
        if (json && r.status == 200) {
            var o;
            eval('o='+r.responseText);
            if (o.distance) {
              /* distance returned is always in meters*/
              //var mapUnits = Fusion.unitFromName(this.getMap().getUnits());
              //if (mapUnits == Fusion.DEGREES || Fusion.DECIMALDEGREES)
              mapUnits = Fusion.METERS;

              if (mapUnits != this.units) {
                o.distance = Fusion.convert(mapUnits, this.units, o.distance);
              }
              
              marker.setDistance(o.distance);
              this.positionMarker(marker, segment);
              this.triggerEvent(Fusion.Event.MEASURE_SEGMENT_UPDATE);                    
            }
        }
    },
    
  /*
      * updates the summary display if it is loaded in a window somewhere
      */
    updateDisplay: function(outputWin) {
        var outputDoc = outputWin.document;
        var tbody = outputDoc.getElementById('segmentTBody');
        var value;
        if (tbody) {
            this.clearDisplay(outputWin);
            var totalDistance = 0;
            var units = Fusion.unitAbbr(this.units);
            for (var i=0; i<this.distanceMarkers.length; i++) {
                var distance = this.distanceMarkers[i].getDistance();
                totalDistance += distance;
            
                var tr = outputDoc.createElement('tr');
                var td = outputDoc.createElement('td');
                td.innerHTML = OpenLayers.String.translate('segment',i+1);
                tr.appendChild(td);
                td = outputDoc.createElement('td');
                if (this.distPrecision == 0) {
                  value = Math.floor(distance);
                }
                else {
                  value = distance.toPrecision(this.distPrecision);
                }
                td.innerHTML = value + ' ' + units;
                tr.appendChild(td);
                tbody.appendChild(tr);
            }
            var tDist = outputDoc.getElementById('totalDistance');
            if (this.distPrecision == 0) {
                  value = Math.floor(totalDistance);
            }
            else {
              value = totalDistance.toPrecision(this.distPrecision);
            }
            tDist.innerHTML = value + ' ' + units;
        }
    },
    
  /*
      * updates the summary display if it is loaded in a window somewhere
      */
    updateTotalDistance: function() {
      if (this.distanceMarkers.length > 1) {
        var totalDistance = 0;
        var units = Fusion.unitAbbr(this.units);
        for (var i=0; i<this.distanceMarkers.length; i++) {
            var distance = this.distanceMarkers[i].getDistance();
            totalDistance += distance;
        }
        this.totalDistanceMarker.domObj.style.display = 'block';
        this.totalDistanceMarker.setDistance(totalDistance);
      }
    },
    
  /*
      *clears the summary display if it is loaded in a window somewhere
      */
    clearDisplay: function(outputWin) {
        var outputDoc = outputWin.document;
        var tbody = outputDoc.getElementById('segmentTBody');
        if (tbody) {
          while(tbody.firstChild) {
              tbody.firstChild.marker = null;
              tbody.removeChild(tbody.firstChild);
          }
          var tDist = outputDoc.getElementById('totalDistance');
          tDist.innerHTML = '';
        }
    },
    
  /*
      *clears the summary display if it is loaded in a window somewhere
      */
    clearTotalDistance: function() {
      this.totalDistanceMarker.domObj.style.display = 'none';
    },
    
  /*
     * Callback method for the MAP_LOADED event
     * Set the units to whatever is specified in the AppDef, or the mapUnits if not specified
     * Subsequent calls from a ViewOptions widget would override the value specified.
     */
    setUnits: function(units) {
      units = (units == Fusion.UNKNOWN)?Fusion.unitFromName(this.getMap().getUnits()):units;
      this.setParameter('Units', Fusion.unitName(units));
    },

    setParameter: function(param, value) {
      //console.log('setParameter: ' + param + ' = ' + value);
        if (param == 'Units') {
            this.units = Fusion.unitFromName(value);
            for (var i=0; i<this.distanceMarkers.length; i++) {
                this.distanceMarkers[i].setUnits(this.units);
            }
            if (this.totalDistanceMarker) {
              this.totalDistanceMarker.setUnits(this.units);
            }
        }
    }
};

/*
* A class for handling the 'tooltip' for the distance measurement.  Markers also hold the distance
values and all markers are held in an array in the Measure widget for access.
*/
Fusion.Widget.Measure.DistanceMarker = Class.create();
Fusion.Widget.Measure.DistanceMarker.prototype = {
    calculatingImg: null,
    distance: 0,
    initialize: function(units, precision, label) {
        this.precision = precision;
        this.label = label ? label:'';
        this.domObj = document.createElement('div');
        this.domObj.className = 'divMeasureMarker';
        this.calculatingImg = document.createElement('img');
        this.calculatingImg.src = Fusion.getFusionURL() + 'widgets/Measure/MeasurePending.gif';
        this.calculatingImg.width = 19;
        this.calculatingImg.height = 4;
        this.setUnits(units);
        this.setCalculating();
    },
    
    destroy: function() {
      if (this.domObj.parentNode) {
          this.domObj.parentNode.removeChild(this.domObj);
      }
    },
    
    setUnits: function(units) {
        this.unit = units;
        this.unitAbbr = Fusion.unitAbbr(units);
    },
    
    getDistance: function() {
        return this.distance;
    },
    
    getDistanceLabel: function() {
      var value;
      if (this.precision == 0) {
        value = Math.floor(this.distance);
      }
      else {
          value = this.distance.toPrecision(this.precision);
      }

      return this.label + ' ' + value + ' ' + this.unitAbbr;  
    },
    
    setDistance: function(distance) {
        if (this.calculatingImg.parentNode) {
            this.calculatingImg.parentNode.removeChild(this.calculatingImg);
        }
        this.distance = distance;
        this.domObj.innerHTML = this.getDistanceLabel();
    },
    
    setCalculating: function() {
        if (!this.calculatingImg.parentNode) {
            this.domObj.innerHTML = '';
            this.domObj.appendChild(this.calculatingImg);
        }
    },
    
    getSize: function() {
        var size =  Element.getDimensions(this.domObj);
        var imgSize = {width:19, height:4};
        if (size.width < imgSize.width) {
            size.width += imgSize.width;
        }
        if (size.height < imgSize.height) {
            size.height += imgSize.height;
        }
        return size;
    }
};
