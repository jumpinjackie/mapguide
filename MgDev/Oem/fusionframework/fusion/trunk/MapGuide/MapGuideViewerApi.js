/**
 * Fusion API AjaxViewer API layer
 *
 * $Id: MapGuide.js 1204 2008-02-01 19:41:53Z madair $
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
var mapWidgetId = 'Map';

function Refresh() {
    var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        mapWidget.redraw();
    }
}

function SetSelectionXML(selectionXml) {
    var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        mapWidget.setSelection(selectionXml, true, true);
    }
}

function ZoomToView(x, y, scale, refresh) {
    var Fusion = window.top.Fusion;
    var mapWidget = Fusion.getWidgetById(mapWidgetId);
    if (mapWidget && mapWidget.isMapLoaded()) {
        var extent = mapWidget.getExtentFromPoint(x, y, scale);
        mapWidget.setExtents(extent);
    }
}

function DigitizePoint(handler) {
    if (handler) {
      var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      var digitizer = new Fusion.Tool.Canvas.Point(mapWidget);
      digitizer.mouseUp = PointHandlers.prototype.mouseUp;
      Object.inheritFrom(digitizer, Fusion.Tool.Canvas.prototype, []);
      digitizer.handler = handler;
      digitizer.activateCanvas();
      
      //add a listener to update the position of the features
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, 
        function(){
          digitizer.updatePx();
          digitizer.clearContext();
          digitizer.draw(digitizer.context);
        }
      );
    }
}

function DigitizeLine(handler) {
    if (handler) {
      var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      var digitizer = new Fusion.Tool.Canvas.Line(mapWidget);
      digitizer.mouseDown = LineHandlers.prototype.mouseDown;
      digitizer.mouseMove = LineHandlers.prototype.mouseMove;
      Object.inheritFrom(digitizer, Fusion.Tool.Canvas.prototype, []);
      digitizer.handler = handler;
      digitizer.activateCanvas();
      
      //add a listener to update the position of the features
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, 
        function(){
          digitizer.updatePx();
          digitizer.clearContext();
          digitizer.draw(digitizer.context);
        }
      );
    }
}

function DigitizeLineString(handler) {
    if (handler) {
      var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      var digitizer = new Fusion.Tool.Canvas.Line(mapWidget);
      digitizer.mouseDown = MultiPointHandlers.prototype.mouseDown;
      digitizer.mouseMove = MultiPointHandlers.prototype.mouseMove;
      digitizer.dblClick = MultiPointHandlers.prototype.dblClick;
      Object.inheritFrom(digitizer, Fusion.Tool.Canvas.prototype, []);
      digitizer.handler = handler;
      digitizer.activateCanvas();
      
      //add a listener to update the position of the features
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, 
        function(){
          digitizer.updatePx();
          digitizer.clearContext();
          digitizer.draw(digitizer.context);
        }
      );
    }
}

function DigitizeRectangle(handler) {
    if (handler) {
      var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      var digitizer = new Fusion.Tool.Canvas.Polygon(mapWidget);
      digitizer.mouseDown = RectangleHandlers.prototype.mouseDown;
      digitizer.mouseMove = RectangleHandlers.prototype.mouseMove;
      Object.inheritFrom(digitizer, Fusion.Tool.Canvas.prototype, []);
      digitizer.handler = handler;
      digitizer.activateCanvas();
      
      //add a listener to update the position of the features
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, 
        function(){
          digitizer.updatePx();
          digitizer.clearContext();
          digitizer.draw(digitizer.context);
        }
      );
    }
}

function DigitizePolygon(handler) {
    if (handler) {
      var Fusion = window.top.Fusion;
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      var digitizer = new Fusion.Tool.Canvas.Polygon(mapWidget);
      digitizer.mouseDown = MultiPointHandlers.prototype.mouseDown;
      digitizer.mouseMove = MultiPointHandlers.prototype.mouseMove;
      digitizer.dblClick = MultiPointHandlers.prototype.dblClick;
      Object.inheritFrom(digitizer, Fusion.Tool.Canvas.prototype, []);
      digitizer.handler = handler;
      digitizer.activateCanvas();
      
      //add a listener to update the position of the features
      var mapWidget = Fusion.getWidgetById(mapWidgetId);
      mapWidget.registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, 
        function(){
          digitizer.updatePx();
          digitizer.clearContext();
          digitizer.draw(digitizer.context);
        }
      );
    }
}

Fusion.Tool.Canvas.prototype.getMap = function() {
  return Fusion.getWidgetById(mapWidgetId);
}

Fusion.Tool.Canvas.prototype.deactivateCanvas = function() {
    var map = this.getMap();
    map.deregisterForEvent(Fusion.Event.MAP_RESIZED, this.resizeCanvasFn);
    map.stopObserveEvent('mousemove', this.mouseMoveCB);
    map.stopObserveEvent('mouseup', this.mouseUpCB);
    map.stopObserveEvent('mousedown', this.mouseDownCB);
    map.stopObserveEvent('dblclick', this.dblClickCB);
}


PointHandlers = Class.create();
PointHandlers.prototype = {
    mouseUp: function(e) {
        var p = this.getMap().getEventPosition(e);
        var point = this.getMap().pixToGeo(p.x, p.y);
        this.setPoint(point.x, point.y);
        this.clearContext();
        this.draw(this.context);
        this.isDigitizing = false;
        this.deactivateCanvas();
        this.handler(new Point(point.x, point.y));
    }
}
    
LineHandlers = Class.create();
LineHandlers.prototype = {
    mouseDown: function(e) {
        if (Event.isLeftClick(e)) {
            var p = this.getMap().getEventPosition(e);

            if (!this.isDigitizing) {
                this.segments = [];
                var point = this.getMap().pixToGeo(p.x, p.y);
                var from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                var to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                var seg = new Fusion.Tool.Canvas.Segment(from,to);
                seg.setEditing(true);
                this.addSegment(seg);
                this.clearContext();
                this.draw(this.context);     

                this.isDigitizing = true;
            } else {
                this.isDigitizing = false;
                var seg = this.lastSegment();
                seg.setEditing(false);
                //seg = this.extendLine();
                this.clearContext();
                this.draw(this.context);
                this.deactivateCanvas();
                
                this.clean();
                var ls = new LineString();
                var nodes = this.getNodes();
                for (var i=0; i<nodes.length; ++i) {
                  var node = nodes[i];
                  ls.AddPoint(new Point(node.x, node.y));
                }
                this.handler(ls);
                
            }
        }
    },

    mouseMove: function(e) {
        //console.log('SelectRadius.mouseMove');
        if (!this.isDigitizing) {
            return;
        }
    
        var p = this.getMap().getEventPosition(e);
        var seg = this.lastSegment();
        seg.to.setPx(p.x,p.y);
        seg.to.updateGeo();
        this.clearContext();
        this.draw(this.context);
    },
}
    
RectangleHandlers = Class.create();
RectangleHandlers.prototype = {
    mouseDown: function(e) {
        if (Event.isLeftClick(e)) {
            var p = this.getMap().getEventPosition(e);

            if (!this.isDigitizing) {
                this.segments = [];
                var point = this.getMap().pixToGeo(p.x, p.y);
                var from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                var to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                this.seg1 = new Fusion.Tool.Canvas.Segment(from,to);
                this.addSegment(this.seg1);
                from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                this.seg2 = new Fusion.Tool.Canvas.Segment(from,to);
                this.addSegment(this.seg2);
                from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                this.seg3 = new Fusion.Tool.Canvas.Segment(from,to);
                this.addSegment(this.seg3);
                from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                this.seg4 = new Fusion.Tool.Canvas.Segment(from,to);
                this.addSegment(this.seg4);
                this.clearContext();
                this.draw(this.context);     

                this.isDigitizing = true;
            } else {
                this.clearContext();
                this.draw(this.context);
                this.deactivateCanvas();
                
                this.clean();
                var p1 = new Point(this.seg1.from.x,this.seg1.from.y);
                var p2 = new Point(this.seg3.from.x,this.seg3.from.y);
                var rect = new Rectangle(p1, p2);
                this.handler(rect);
                
            }
        }
    },

    mouseMove: function(e) {
        //console.log('SelectRadius.mouseMove');
        if (!this.isDigitizing) {
            return;
        }
    
        var p = this.getMap().getEventPosition(e);
        this.seg1.to.setPx(p.x, this.seg1.from.py);
        this.seg1.to.updateGeo();
        this.seg2.from.setPx(p.x, this.seg1.from.py);
        this.seg2.from.updateGeo();
        this.seg2.to.setPx(p.x, p.y);
        this.seg2.to.updateGeo();
        this.seg3.from.setPx(p.x, p.y);
        this.seg3.from.updateGeo();
        this.seg3.to.setPx(this.seg1.from.px, p.y);
        this.seg3.to.updateGeo();
        this.seg4.from.setPx(this.seg1.from.px, p.y);
        this.seg4.from.updateGeo();
        this.clearContext();
        this.draw(this.context);
    },
}
    
MultiPointHandlers = Class.create();
MultiPointHandlers.prototype = {
    mouseDown: function(e) {
        if (Event.isLeftClick(e)) {
            var p = this.getMap().getEventPosition(e);

            if (!this.isDigitizing) {
                this.segments = [];
                var point = this.getMap().pixToGeo(p.x, p.y);
                var from = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                var to = new Fusion.Tool.Canvas.Node(point.x,point.y, this.getMap());
                var seg = new Fusion.Tool.Canvas.Segment(from,to);
                seg.setEditing(true);
                this.addSegment(seg);
                this.clearContext();
                this.draw(this.context);     

                this.isDigitizing = true;
            } else {
                var seg = this.lastSegment();
                seg.setEditing(false);
                seg = this.extendLine();
                seg.setEditing(true);
                this.clearContext();
                this.draw(this.context);
            }
        }
    },

    mouseMove: function(e) {
        //console.log('SelectRadius.mouseMove');
        if (!this.isDigitizing) {
            return;
        }
    
        var p = this.getMap().getEventPosition(e);
        var seg = this.lastSegment();
        seg.to.setPx(p.x,p.y);
        seg.to.updateGeo();
        this.clearContext();
        this.draw(this.context);
    },
    
    dblClick: function(e) {
        //console.log('Digitizer.dblClick');
        if (!this.isDigitizing) {
            return;
        }
        this.event = e;
        var p = this.getMap().getEventPosition(e);
        var point = this.getMap().pixToGeo(p.x, p.y);
        var seg = this.lastSegment();
        seg.setEditing(false);
        seg.to.set(point.x,point.y);
        this.clearContext();
        this.draw(this.context);
        this.isDigitizing = false;
        this.deactivateCanvas();
        
        this.clean();
        var ls = new LineString();
        var nodes = this.getNodes();
        for (var i=0; i<nodes.length; ++i) {
          var node = nodes[i];
          ls.AddPoint(new Point(node.x, node.y));
        }
        this.handler(ls);
    }
}
    
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

