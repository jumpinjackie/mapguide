/**
 * Fusion.Tool.Canvas
 *
 * $Id: CanvasTool.js 970 2007-10-16 20:09:08Z madair $
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

/********************************************
 * Class: Fusion.Tool.Canvas
 *
 * Utility base class for drawing features on the map.
 * **********************************************************************/

Fusion.Tool.Canvas = Class.create();
Fusion.Tool.Canvas.prototype = 
{
    context: null,
    canvas: null,
    width: null,
    height: null,
    
    initialize : function()
    {
        this.context = null;
        this.canvas = null;
        this.width = null;
        this.height = null;
        
        this.mouseMoveCB = this.mouseMove.bindAsEventListener(this);
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
        this.mouseDownCB = this.mouseDown.bindAsEventListener(this);
        this.dblClickCB = this.dblClick.bindAsEventListener(this);
        
        this.resizeCanvasFn = this.resizeCanvas.bind(this);
    },
    
    /**
     * (public) clearContext()
     *
     * wipe the slate clean
     */
    clearContext: function() {
        //console.log('Fusion.Tool.Canvas.clearContext');
        if (this.context) {
            this.context.clearRect(0,0,this.width,this.height);
        }
    },

    activateCanvas: function() {
        var map = this.getMap();
        map.registerForEvent(Fusion.Event.MAP_RESIZED, this.resizeCanvasFn);
        var domObj = map.getDomObj();
        
        var size = Element.getDimensions(domObj);
        this.width = size.width;
        this.height = size.height;
        
        /* create dynamic canvas */
        if (!this.canvas) {
            this.canvas = document.createElement('canvas');
            
            // we need to init this for IE 
            if (typeof G_vmlCanvasManager != "undefined") { 
                document.getElementsByTagName('BODY')[0].appendChild(this.canvas);
                G_vmlCanvasManager.initElement(this.canvas); 
                this.canvas = document.getElementsByTagName('BODY')[0].lastChild;
            } 
            
            this.canvas.id = 'featureDigitizer';
            this.canvas.style.position = 'absolute';
            this.canvas.style.top = '0px';
            this.canvas.style.left = '0px';
            this.canvas.style.width = this.width+'px';
            this.canvas.style.height = this.height+'px';
            this.canvas.width = this.width;
            this.canvas.height = this.height;
            this.canvas.style.zIndex = 99;
            
        }
    
        domObj.appendChild(this.canvas);
        if (!this.context) {
            this.context = this.canvas.getContext('2d');
        }
        this.canvas.style.width = this.width+'px';
        this.canvas.style.height = this.height+'px';
        this.canvas.width = this.width;
        this.canvas.height = this.height;
        
        map.observeEvent('mousemove', this.mouseMoveCB);
        map.observeEvent('mouseup', this.mouseUpCB);
        map.observeEvent('mousedown', this.mouseDownCB);
        map.observeEvent('dblclick', this.dblClickCB);
    },
    
    resizeCanvas: function() {
        var map = this.getMap();
        var domObj = map.getDomObj();
        var size = Element.getDimensions(domObj);
        this.width = size.width;
        this.height = size.height;
        this.canvas.style.width = this.width+'px';
        this.canvas.style.height = this.height+'px';
        this.canvas.width = this.width;
        this.canvas.height = this.height;        
    },
    
    /**
     * (public) deactivate()
     *
     * deactivate the line digitizing tool
     */
    deactivateCanvas: function() {
        //console.log('Fusion.Tool.Canvas.deactivate');
        var map = this.getMap();
        map.deregisterForEvent(Fusion.Event.MAP_RESIZED, this.resizeCanvasFn);
        map.getDomObj().removeChild(this.canvas);
        this.context.clearRect(0,0,this.width,this.height);
        map.stopObserveEvent('mousemove', this.mouseMoveCB);
        map.stopObserveEvent('mouseup', this.mouseUpCB);
        map.stopObserveEvent('mousedown', this.mouseDownCB);
        map.stopObserveEvent('dblclick', this.dblClickCB);
    },
    
    /**
     * (public) mouseDown(e)
     *
     * handle the mouse down event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseDown: function(e) { },

    /**
     * (public) mouseUp(e)
     *
     * handle the mouse up event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseUp: function(e) { },

    /**
     * (public) mouseMove(e)
     *
     * handle the mouse move event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseMove: function(e) { },

    /**
     * (public) dblClick(e)
     *
     * handle the mouse dblclick event
     *
     * @param e Event the event that happened on the mapObj
     */
    dblClick: function(e) { }
};

/********************************************
 * Class: Fusion.Tool.Canvas.Point
 *
 * Utility base class for drawing point features on the map.
 * **********************************************************************/

Fusion.Tool.Canvas.Point = Class.create();
Fusion.Tool.Canvas.Point.prototype = {
    center: null,
    radius: null,
    lineStyle: null,
    fillStyle: null,
    
    initialize: function(map) {
        this.center = new Fusion.Tool.Canvas.Node(0,0, map);
        this.radius = 5;
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
        this.segments = [];
        
    },
    
    setPoint: function(x,y) {
        this.center.set(x,y);
    },
    
    getPoint: function() {
        this.center.updateGeo();
        return {x:this.center.x, y:this.center.y};
    },
    
    draw: function( context ) {
        var x = this.center.px;
        var y = this.center.py;
        var radius = this.radius;
        
        this.fillStyle.apply(context);
        this.lineStyle.apply(context);
        
        context.beginPath();
        context.arc(x,y,radius, 0, 2*Math.PI, 1);
        context.closePath();
        context.fill(); 
        context.stroke();
        
    },

    getNodes: function() {
        return [this.center];
    },

    clean: function() {},
    
    updateGeo: function() {
        this.center.updateGeo();
    },
    
    updatePx: function() {
        this.center.updatePx();
    }
};

/********************************************
 * Class: Fusion.Tool.Canvas.Circle
 *
 * Utility base class for drawing circle features on the map.
 * **********************************************************************/

Fusion.Tool.Canvas.Circle = Class.create();
Fusion.Tool.Canvas.Circle.prototype = {
    map: null,
    center: null,
    radius: null,
    radiusPx: null,
    start: null,
    end: null,
    lineStyle: null,
    fillStyle: null,
    
    initialize: function(map) {
        this.map = map;
        this.center = new Fusion.Tool.Canvas.Node(0,0, map);
        this.radius = 1;
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
        this.segments = [];
    },
    
    setCenter: function(x,y) {
        this.center.set(x,y);
    },
    
    setRadius: function(r) {
        if (r > 1 || r < -1) {
            this.radius = Math.abs(r);
            this.radiusPx = this.map.geoToPixMeasure(this.radius);
        } else {
            this.radius = 1;
            this.radiusPx = 1;
        }
    },
    
    draw: function( context ) {
        var x = this.center.px;
        var y = this.center.py;
        var radius = this.radiusPx;
        this.fillStyle.apply(context);
        this.lineStyle.apply(context);
        
        context.beginPath();
        if (this.start && this.end) {
            context.moveTo(x,y);
            var s = this.start;
            var e = this.end;
            if (s < e) {
                var t = s;
                s = e;
                e = t;
            }
            var sx = x + Math.sin(s) * radius;
            var sy = y - Math.cos(s) * radius;
            context.lineTo(sx,sy);
            context.arc(x,y,radius, s - Math.PI/2, e - Math.PI/2, 1);
            context.lineTo(x, y);
        } else {
            context.arc(x,y,radius, 0, 2*Math.PI, 1);
        }
        context.closePath();
        context.fill(); 
        context.stroke();        
    },
    
    getNodes: function() {
        return [this.center];
    },
    
    clean: function() {},
    
    updateGeo: function() {
        this.center.updateGeo();
        this.radius = this.map.pixToGeoMeasure(this.radiusPx);
    },
    
    updatePx: function() {
        this.center.updatePx();
        this.radiusPx = this.map.geoToPixMeasure(this.radius);
    }
};

/********************************************
 * Class: Fusion.Tool.Canvas.Polygon
 *
 * Utility base class for drawing polygon features on the map.
 * **********************************************************************/

Fusion.Tool.Canvas.Polygon = Class.create();
Fusion.Tool.Canvas.Polygon.prototype = {
    segments: null,
    lineStyle: null,
    fillStyle: null,
    map: null,
    
    initialize: function(map) {
        this.map = map;
        this.segments = [];
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
    },

    clean: function() {
        var nodes = this.getNodes();
        this.segments = [];
        var n1 = nodes[0];
        //console.log('n1: '+ n1);
        var n2 = nodes[1];
        for (var i=1; i<nodes.length;i++) {
            if (n1.x != n2.x || n1.y != n2.y) {
                this.addSegment(new Fusion.Tool.Canvas.Segment(n1,n2));
                //console.log('n2: '+ n2);
                n1 = n2;
            }
            n2 = nodes[i];
        }
        
        this.addSegment(new Fusion.Tool.Canvas.Segment(n1, nodes[0]));
        //console.log(this);
    },

    getNodes: function() {
        var nodes = [];
        nodes.push(this.segments[0].from);
        for (var i=0; i<this.segments.length; i++) {
            nodes.push(this.segments[i].to);
        }
        return nodes;
    },

    /*
     * reverse the nodes in the feature
     * and adjust segments
     */
    reverseNodes: function() {
        var nSegments = this.segments.length;
        if (!nSegments) {
            return;
        }
        //flip nodes on each segment
        for (var i=0; i < nSegments; i++) {
            var seg = this.segments[i];
            var tmp = seg.from;
            seg.from = seg.to;
            seg.to = tmp;
        };
        //reverse segment order
        this.segments.reverse();
    },
    
    /*
     * remove node from the nodes in this feature
     * and adjust segments
     */
    removeNode: function(node) {
        //end cases
        if (node == this.segments[0].from) {
            this.segments[0].from = null;
            this.segments.shift();
            this.segments[0].from = this.segments[this.segments.length - 1].to;
            return;
        }
        if (node == this.segments[this.segments.length -1].from) {
            this.segments[this.segments.length -1].from = null;
            this.segments.pop();
            this.segments[0].from = this.segments[this.segments.length - 1].to;
            return;
        }
        //general case
        for (var i=1; i < this.segments.length; i++) {
            if (node == this.segments[i].from){
                this.segments[i-1].to = this.segments[i].to;
                this.segments[i].from = null;
                this.segments.splice(i, 1);
                return;
            }
        };
        
    },
    
    draw: function( context ) {
        var x = this.segments[0].from.px;
        var y = this.segments[0].from.py;
        if (this.segments.length > 2) {
            /* draw closing line and fill */
        
            this.fillStyle.apply(context);
            context.beginPath();
            context.moveTo(x,y);
            for (var i=0; i<this.segments.length; i++) {
                var s = this.segments[i];
                context.lineTo(s.to.px, s.to.py);         
            }
            context.lineTo(x,y); //closing line
            context.closePath();
            context.fill(); 
        }
        /* draw outline */
        this.lineStyle.apply(context);
        for (var i=0; i<this.segments.length; i++) {
            this.segments[i].draw(context);
        }
    
        var last = this.lastSegment();
        context.beginPath();
        context.moveTo(last.to.px,last.to.py);
        context.lineTo(x,y);
        context.stroke();
    },

    addSegment: function( s ) {
        s.normalStyle = this.lineStyle;
        this.segments[this.segments.length] = s;
        //console.log('add segment ' + s);
    },

    lastSegment: function() {
        return this.segments[this.segments.length-1];
    },

    /* find the segment with the given node as its end
     * @param Object node - the node at the end
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if nothing is found.
     */
     segmentTo: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasTo(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* find the segment with the given node as its start
     * @param Object node - the node at the start
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if there is none.
     */
     segmentFrom: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasFrom(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* extend an existing line by creating a new segment attached
     * to the last segment
     * @return the new segment
     */
    extendLine: function() {
        var last = this.lastSegment();
        var newNode = new Fusion.Tool.Canvas.Node(last.to.x, last.to.y, this.map);
        var newSegment = new Fusion.Tool.Canvas.Segment( last.to, newNode );
        this.addSegment(newSegment);
        return newSegment;  
    },

    /* determine if the passed pixel coordinate is within this feature
     * @param point Object - {px,py} representation of point
     * @return true if the point is contained
     *
     * uses crossing test (Jordan Curve Theorem) algorithm discussed at
     * http://www.acm.org/tog/editors/erich/ptinpoly/
     */
    contains: function(node) {
        return true;  
    },
    
    
    toString: function() {
        var szFeature = this.segments[0].from.toString();
        for (var i=0; i < this.segments.length; i++) {
            szFeature += ',' + this.segments[i].to.toString();
        }
        return 'POLYGON(' + szFeature + ')';
    },
    
    updateGeo: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updateGeo();
        }
    },
    
    updatePx: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updatePx();
        }
    }
};

/********************************************
 * Class: Fusion.Tool.Canvas.line
 *
 * Utility base class for drawing line features on the map.
 * **********************************************************************/

Fusion.Tool.Canvas.Line = Class.create();
Fusion.Tool.Canvas.Line.prototype = {
    segments: null,
    lineStyle: null,
    map: null,
    
    initialize: function(map) {
        this.map = map;
        this.segments = [];
        this.lineStyle = new Fusion.Tool.Canvas.Style({strokeStyle:'rgba(0,0,0,1.0)'});
    },

    clean: function() {
        var nodes = this.getNodes();
        this.segments = [];
        var n1 = nodes[0];
        var n2 = nodes[1];
        for (var i=1; i<nodes.length;i++) {
            //console.log('n1: '+ n1);
            //console.log('n2: '+ n2);
            n2 = nodes[i];
            if (n1.x != n2.x || n1.y != n2.y) {
                this.addSegment(new Fusion.Tool.Canvas.Segment(n1,n2));
                n1 = n2;
            }
        }
        //console.log(this);
    },

    getNodes: function() {
        var nodes = [];
        nodes.push(this.segments[0].from);
        for (var i=0; i<this.segments.length; i++) {
            nodes.push(this.segments[i].to);
        }
        return nodes;
    },

    /*
     * reverse the nodes in the feature
     * and adjust segments
     */
    reverseNodes: function() {
        var nSegments = this.segments.length;
        if (!nSegments) {
            return;
        }
        //flip nodes on each segment
        for (var i=0; i < nSegments; i++) {
            var seg = this.segments[i];
            var tmp = seg.from;
            seg.from = seg.to;
            seg.to = tmp;
        };
        //reverse segment order
        this.segments.reverse();
    },
    
    /*
     * remove node from the nodes in this feature
     * and adjust segments
     */
    removeNode: function(node) {
        //end cases
        if (node == this.segments[0].from) {
            this.segments[0].from = null;
            this.segments.shift();
            return;
        }
        if (node == this.segments[this.segments.length -1].from) {
            this.segments[this.segments.length -1].from = null;
            this.segments.pop();
            return;
        }
        //general case
        for (var i=1; i < this.segments.length; i++) {
            if (node == this.segments[i].from){
                this.segments[i-1].to = this.segments[i].to;
                this.segments[i].from = null;
                this.segments.splice(i, 1);
                return;
            }
        };
        
    },

    draw: function( context ) {
        for (var i=0; i<this.segments.length; i++) {
            this.segments[i].draw(context);
        }
    },

    addSegment: function( s ) {
        s.normalStyle = this.lineStyle;
        this.segments[this.segments.length] = s;
    },

    lastSegment: function() {
        return this.segments[this.segments.length-1];
    },

    /* find the segment with the given node as its end
     * @param Object node - the node at the end
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if nothing is found.
     */
     segmentTo: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasTo(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* find the segment with the given node as its start
     * @param Object node - the node at the start
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if there is none.
     */
     segmentFrom: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasFrom(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* extend an existing line by creating a new segment attached
     * to the last segment
     * @return the new segment
     */
    extendLine: function() {
        var last = this.lastSegment();
        var newNode = new Fusion.Tool.Canvas.Node(last.to.x, last.to.y, this.map);
        var newSegment = new Fusion.Tool.Canvas.Segment( last.to, newNode );
        this.addSegment(newSegment);
        return newSegment;  
    },
    
    updateGeo: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updateGeo();
        }
    },
    
    updatePx: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updatePx();
        }
    }
};

/********************************************
 * Class: Fusion.Tool.Canvas.Segment
 *
 * Utility base class for drawing line segments on the map.
 * **********************************************************************/

Fusion.Tool.Canvas.Segment = Class.create();
Fusion.Tool.Canvas.Segment.prototype = {
    from: null,
    to: null,
    
    initialize: function(from, to) {
        this.from = from;
        this.to = to;
        this.isEditing = false;
        this.normalStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(0,0,0,1.0)'});
        this.editStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(255,0,0,1.0)'});
    },

    /* returns true if the node is at the end of this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    hasTo: function(node, margin) {
        return this.to.near({x:node.px, y:node.py}, margin);
    },

    /* returns true if the node is at the start of this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    hasFrom: function(node, margin) {
        return this.from.near({x:node.px, y:node.py}, margin);
    },
    
    /* returns true if the given point falls along this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    intersectsPoint: function(point, margin){
        //check bbox
        var minX = Math.min(this.to.px, this.from.px);
        var maxX = Math.max(this.to.px, this.from.px);
        if (point.x > maxX || point.x < minX){return false;};
        var maxY = Math.max(this.to.py, this.from.py);
        var minY = Math.min(this.to.py, this.from.py);
        if (point.y < minY || point.y > maxY){return false;};
        
        //determine slope
        var slope = parseFloat((maxY-minY))/(maxX-minX);
        var segY = slope * (point.x - minX) + minY;
        return (segY - margin < point.y && segY + margin > point.y);

    },
    
    setNormalStyle: function( style ) {
        this.normalStyle = style;
    },

    setEditStyle: function( style ) {
        this.editStyle = style;
    },

    draw: function( context ) {
        /* set up correct style */
        if (this.isEditing) {
            this.editStyle.apply(context);
        } else {
            this.normalStyle.apply(context);
        }
    
        /* draw segment */
        context.beginPath();
        context.moveTo(this.from.px, this.from.py);
        context.lineTo(this.to.px, this.to.py);
        context.closePath();
        context.stroke();
    
        /* draw nodes if editing */
        if (this.isEditing) {
            this.from.draw( context );
            this.to.draw( context );
        }
    },

    /* changes rendering style */
    setEditing: function(bEditing) {
        this.isEditing = bEditing;
    },
    
    toString: function() {
        return this.from.toString() + ', '+ this.to.toString();
    },
    
    updateGeo: function() {
        this.from.updateGeo();
        this.to.updateGeo();
    },
    
    updatePx: function() {
        this.from.updatePx();
        this.to.updatePx();
    }
};

/********************************************
 * Class: Fusion.Tool.Canvas.Node
 *
 * Utility base class to hold nodes that make up otherr features
 * **********************************************************************/

Fusion.Tool.Canvas.Node = Class.create();
Fusion.Tool.Canvas.Node.prototype = {
    x: null,
    y: null,
    px: null,
    py: null,
    uid: null,
    map: null,
    counter: [0],
    isSelected: false,
    
    initialize: function(x,y, map) {
        this.map = map;
        this.set(x,y);
        var p = map.geoToPix(x, y);
        this.setPx(p.x, p.y);
        this.radius = 3;
        this.uid = this.counter[0];
        this.counter[0]++;
        this.normalStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(0,0,0,1.0)'});
        this.selectedStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, fillStyle:'rgba(255,0,0,1.0)',
                                                strokeStyle:'rgba(255,0,0,1.0)'});
    },

    set: function(x,y) {
        this.x = x;
        this.y = y;
        //update px position
        var p = this.map.geoToPix(x, y);
        this.setPx(p.x, p.y);
    },
    
    setPx: function(px, py) {
        this.px = px;
        this.py = py;
    },
    
    updateGeo: function() {
        if (!this.px || !this.py) {return;};
        var g = this.map.pixToGeo(this.px, this.py);
        this.set(g.x, g.y);
    },
    
    updatePx: function() {
        if (!this.x || !this.y) {return;};
        var p = this.map.geoToPix(this.x, this.y);
        this.setPx(p.x, p.y);
    },
    
    /* returns true if the supplied pixel position is
     * within the given tolerance
     * @return Bool true if found within margin, false otherwise
     */
     /*TODO: uses a square envelope for speed but could use radius
      *TODO: should support geographic tolerance
      */
    near: function(point, tolerance) {
        var minX = point.x - tolerance;
        var maxX = point.x + tolerance;
        var maxY = point.y + tolerance;
        var minY = point.y - tolerance;
        return ((this.px > minX && this.px < maxX) && (this.py > minY && this.py < maxY))?true:false;
    },

    /* returns true if this node is
     * within the given bbox
     * @param Array bbox - array of pixel coordinates to search within
     * @return Bool true if found within, false otherwise
     */
    within: function(bbox) {
        //TODO: handle > 2 coord pairs
        var minX = Math.min(bbox[0], bbox[2]);
        var maxX = Math.max(bbox[0], bbox[2]);
        var minY = Math.min(bbox[1], bbox[3]);
        var maxY = Math.max(bbox[1], bbox[3]);
        return ((this.px > minX && this.px < maxX) && (this.py > minY && this.py < maxY))?true:false;
    },

    /* draw a node on a canvas. */
    draw: function( context ) {
        /* set up correct style */
        if (this.isSelected) {
            this.selectedStyle.apply(context);
        } else {
            this.normalStyle.apply(context);
        }

        context.beginPath();
        context.arc(this.px, this.py, this.radius, 0, 2*Math.PI,1);
        context.closePath();
        context.stroke();
        if(this.isSelected){
            context.fill();
        };
    },
    
    /* changes rendering style */
    setSelected: function(bSelected) {
        this.isSelected = bSelected;
    },

    toString: function() {
        return '('+this.uid+') '+ this.x + ' ['+this.px+'px] '+ this.y+ ' ['+this.py+'px] ';
    }
};

/* encapsulate a context style */
/********************************************
 * Class: Fusion.Tool.Canvas.Style
 *
 * Utility base class to encapsulate a context style.
 * **********************************************************************/

Fusion.Tool.Canvas.Style = Class.create();
Fusion.Tool.Canvas.Style.prototype = {
    properties: ['fillStyle',
                 'globalAlpha',
                 'globalCompositeOperation',
                 'lineCap',
                 'lineJoin',
                 'lineWidth',
                 'miterLimit',
                 'shadowBlur',
                 'shadowColor',
                 'shadowOffsetX',
                 'shadowOffsetY',
                 'strokeStyle'],
    
    initialize: function( o ) { 
        for (var i=0; i<this.properties.length; i++) {
            var p = this.properties[i];
            this[p] = o[p] ? o[p]:null;
        }
    },

    set: function( p, v ) {
        this[p] = v;
    },

    apply: function(context) {
        for (var i=0; i<this.properties.length; i++) {
            var p = this.properties[i];
            if (this[p]) {
                context[p] = this[p];
            }
        }
    }
};