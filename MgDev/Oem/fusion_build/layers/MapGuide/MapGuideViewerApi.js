/**
 * Fusion API AjaxViewer API layer
 *
 * $Id: MapGuideViewerApi.js 2583 2012-09-07 12:03:03Z jng $
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

/***************************************************************************
* This is a simple API layer to mimick the MapGuide ajaxviewer API
*/
var mgApiMapWidgetId = 'Map';
var mgApiActiveWidget = null;
var mgApiActiveControl = null;
var mgApiDrawControls;
var mgApiDigitizingLayer = null;
var mgApiInitialized = false;
var MainFusionWindow = GetFusionWindow();
var OpenLayers = MainFusionWindow.OpenLayers;
var Fusion = MainFusionWindow.Fusion;
var Class = MainFusionWindow.Class;
var Object = MainFusionWindow.Object;

function Refresh() {
    //var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mgApiMapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        mapWidget.redraw();
    }
}

function SetSelectionXML(selectionXml) {
    //var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mgApiMapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        mapWidget.setSelection(selectionXml, true);
    }
}

function ZoomToView(x, y, scale, refresh) {
    //var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mgApiMapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        var extent = mapWidget.getExtentFromPoint(x, y, scale);
        mapWidget.setExtents(extent);
    }
}

function DigitizePoint(handler) {
    mgApiStartDigitizing('point', handler)
}

function DigitizeLine(handler) {
    mgApiStartDigitizing('line', handler)
}

function DigitizeLineString(handler) {
    mgApiStartDigitizing('linestr', handler)
}

function DigitizeRectangle(handler) {
    mgApiStartDigitizing('rectangle', handler)
}

function DigitizePolygon(handler) {
    mgApiStartDigitizing('polygon', handler)
}

function ClearDigitization(bCancelHandler) {
    if (mgApiDigitizingLayer) {
        mgApiDigitizingLayer.removeFeatures(mgApiDigitizingLayer.features);
    }
    if (bCancelHandler)
        mgApiDeactivate();
}

//Theses are the Geometry classes used in the MapGuide Viewer API
function Point(x, y) {
    this.X = x;
    this.Y = y;
}

function LineString()
{
    this.points = new Array();
    this.Count = 0;

    this.AddPoint = function(pt)
    {
        this.points.push(pt);
        this.Count ++;
    }

    this.Point = function(i)
    {
        if(i < 0 || i >= this.points.length)
            return null;
        return this.points[i];
    }
}

function Circle()
{
    this.Center = null;
    this.Radius = 0;

    this.SetRadius = function(pt)
    {
        dx = pt.X - this.Center.X;
        dy = pt.Y - this.Center.Y;
        this.Radius = Math.sqrt(dx*dx + dy*dy);
    }
}

function Rectangle(p1, p2)
{
    this.Point1 = p1;
    this.Point2 = p2;
}

function Polygon()
{
    this.LineStringInfo = LineString;
    this.LineStringInfo();
}

//The following methods are private and not intended for use by applications
//
function mgApiStartDigitizing(type, handler) {
    mgApiInit();
    if (handler) {
      //var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mgApiMapWidgetId);
      mgApiActiveWidget = mapWidget.oActiveWidget;
      if (mgApiActiveWidget) {
        mapWidget.deactivateWidget(mgApiActiveWidget);
      }
      
      var control = mgApiDrawControls[type];
      control.userHandler = handler;
      mgApiActiveControl = control;
      control.activate();
    }
}

function mgApiCallHandler(evt) {
  var geom = evt.feature.geometry;
  var apiGeom = null;
  if (this.handler.CLASS_NAME == 'OpenLayers.Handler.RegularPolygon') {
      var v = geom.getVertices();
      apiGeom = new Rectangle(new Point(v[0].x, v[0].y), new Point(v[2].x, v[2].y));
  } else {
      switch (geom.CLASS_NAME) {
        case 'OpenLayers.Geometry.Point':
          apiGeom = new Point(geom.x, geom.y);
          break;
        case 'OpenLayers.Geometry.LineString':
          apiGeom = new LineString();
          var v = geom.getVertices();
          for (var i=0; i<v.length; ++i) {
            apiGeom.AddPoint(new Point(v[i].x, v[i].y));
          }
          break;
        case 'OpenLayers.Geometry.Polygon':
          apiGeom = new LineString();
          var v = geom.getVertices();
          for (var i=0; i<v.length; ++i) {
            apiGeom.AddPoint(new Point(v[i].x, v[i].y));
          }
          break;
      }      
  }
  this.userHandler(apiGeom);
  
  //deactivate the control in a separate thread so that the chain of event
  //handlers has a chance to finish before the deactivation occurs
  window.setTimeout(mgApiDeactivate, 100);
  
  return false;
}

function mgApiDeactivate() {
    if (mgApiActiveControl) {
        mgApiActiveControl.deactivate();
        mgApiActiveControl = null;
    }
  if (mgApiActiveWidget) {
    //var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mgApiMapWidgetId);
    mapWidget.activateWidget(mgApiActiveWidget);
    mgApiActiveWidget = null;
  }
}

// set the Stylemap for the MGAPI
var mgApiStyle = new OpenLayers.Style({
            pointRadius: 4,
            graphicName: "square",
            fillColor: "white",
            fillOpacity: 0.4,
            strokeWidth: 2,
            strokeOpacity: 1,
            strokeColor: "#666666"
        });
var mgApiStyleMap = new OpenLayers.StyleMap(mgApiStyle);
    
Fusion.registerForEvent(Fusion.Event.FUSION_INITIALIZED, mgApiInit);

//set up of digitizing tools once everything is initialized
function mgApiInit() {
    if (mgApiInitialized) {
        return;
    }
    mgApiInitialized = true;
  var map = Fusion.getWidgetById(mgApiMapWidgetId).oMapOL;
  
  mgApiDigitizingLayer = new OpenLayers.Layer.Vector("Digitizing Layer", {styleMap: mgApiStyleMap});
  map.addLayers([mgApiDigitizingLayer]);

  mgApiDrawControls = {
      point: new OpenLayers.Control.DrawFeature(mgApiDigitizingLayer,
              OpenLayers.Handler.Point, {
                handlerOptions: {
                  layerOptions: {
                    styleMap: mgApiStyleMap
                  }
                }
              }),
      line: new OpenLayers.Control.DrawFeature(mgApiDigitizingLayer,
                  OpenLayers.Handler.Path, {
                    handlerOptions: {
                      freehandToggle: null, 
                      freehand: false, 
                      persist: true,
                      style: "default", // this forces default render intent
                      layerOptions: {
                        styleMap: mgApiStyleMap
                      }
                    },
                    callbacks: {
                      'point': mgApiCheckLine
                    }
                  }),
      linestr: new OpenLayers.Control.DrawFeature(mgApiDigitizingLayer,
                  OpenLayers.Handler.Path, {
                    handlerOptions: {
                      freehand: false, 
                      persist: true, 
                      style: "default", // this forces default render intent
                      layerOptions: {
                        styleMap: mgApiStyleMap
                      }
                    }
                  }),
      rectangle: new OpenLayers.Control.DrawFeature(mgApiDigitizingLayer,
                  OpenLayers.Handler.RegularPolygon, {
                    handlerOptions: {
                      persist: true, 
                      sides: 4, 
                      irregular: true,
                      style: "default", // this forces default render intent
                      layerOptions: {
                        styleMap: mgApiStyleMap
                      }
                    }
                  }),
      polygon: new OpenLayers.Control.DrawFeature(mgApiDigitizingLayer,
                  OpenLayers.Handler.Polygon, {
                    handlerOptions: {
                      freehand: false, 
                      persist: true, 
                      style: "default", // this forces default render intent
                      layerOptions: {
                        styleMap: mgApiStyleMap
                      }
                    }
                  })
  };

  for(var key in mgApiDrawControls) {
      if (mgApiDrawControls[key].events) {
          mgApiDrawControls[key].events.register('featureadded', null, mgApiCallHandler);
          map.addControl(mgApiDrawControls[key]);          
      }
  }

}

//this callback method for the 'line' control to limit the number of points 
//in the linestring to 2 - there is an extra point in the feature that gets
//removed when the feature is finalized
function mgApiCheckLine(point, geom) {
  if (geom.components.length == 3) {
    this.handler.dblclick();
    //this.handler.finalize();
  }
}

/* locate the Fusion window */
function GetFusionWindow() {
    var curWindow = window;
    while (!curWindow.Fusion) {
        if (curWindow.parent && curWindow != curWindow.parent) {
            curWindow = curWindow.parent;
        } else if(curWindow.opener) {
            curWindow = curWindow.opener;
        } else {
            alert('Could not find Fusion instance');
            break;
        }
    }
    return curWindow;
}

function GetFusionMapWidget() {
    return Fusion.getWidgetById(mgApiMapWidgetId);
}
