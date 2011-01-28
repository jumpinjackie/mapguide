MapCapturer = function() 
{ 
    this.initialize();
};

MapCapturer.prototype =
{
    active : false,
    inCommand : false,
    moving : false,
    rotating : false,
    isRotating : false,
    isMoving : false,
    lastPixel : null,
    
    rotation : 0.0,
    boxCanvas : null,
    boxCanvasID : "QuickPlotBoxCanvasDiv",
    handleCanvas : null,
    handleCanvasID : "QuickPlotHandleCanvasDiv",
    gripCanvas : null,
    gripCanvasID : "QuickPlotGripCanvasDiv",
    bgCanvas : null,
    bgCanvasID : "QuickPlotBgCanvasDiv",
    boxGraphics : null,
    handleGraphics : null,
    gripGraphics : null,
    bgGraphics : null,

    mapWindow : null,
    mapDoc : null,
    
    captureBox : null,
    rotateHandleStart : null,
    rotateHandleEnd : null,
    
    fillColor : "#476387",
    strokeColor : "#39506F",
    rotateHandleLength : 30,
    rotateGripDiameter : 8,
    rotateSnappingStep : 45,
    rotateSnappingTolerance : 2,
    
    resolutionWarning : "",
    paperSize : {width : 0, height : 0},
    scaleDenominator : 1,
    
    eventSuppressed : false,
    
    initialize : function()
    {
        this.mapWindow = window;
        
        this.mapDoc    = this.mapWindow.document;
        
        this.boxCanvas      = this.createCanvas(this.boxCanvasID, 902);
        this.handleCanvas   = this.createCanvas(this.handleCanvasID, 901);
        this.gripCanvas     = this.createCanvas(this.gripCanvasID, 903);
        this.bgCanvas       = this.createCanvas(this.bgCanvasID, 900);
        this.refreshCanvasSize();
        
        this.boxGraphics    = new jsGraphics(this.boxCanvasID);
        this.handleGraphics = new jsGraphics(this.handleCanvasID);
        this.gripGraphics   = new jsGraphics(this.gripCanvasID);
        this.bgGraphics     = new jsGraphics(this.bgCanvasID);
        
        // Add event listener
        this.mapWindow.OnMouseMove = this.onMouseMove.bindWithEvent(this, this.mapWindow.OnMouseMove);
        this.mapWindow.OnMouseDown = this.onMouseDown.bindWithEvent(this, this.mapWindow.OnMouseDown);
        this.mapWindow.OnMouseUp   = this.onMouseUp.bindWithEvent(this, this.mapWindow.OnMouseUp);
        this.mapWindow.OnMouseOut  = this.onMouseOut.bindWithEvent(this, this.mapWindow.OnMouseOut);
        this.mapWindow.PanFeedback = this.panFeedback.bindWithEvent(this, this.mapWindow.PanFeedback);
        // Function.bindWithEvent / Function.bind cannot handle more than arguments, so workaround...
        this.mouseWheelHandlerBackup = this.mapWindow.HandleMouseWheelEvent;
        this.mapWindow.HandleMouseWheelEvent = this.handleMouseWheelEvent.bind(this);
        
        // Overload MAP_LOADED event handler
        this.mapWindow.OnMapLoaded = this.onMapLoaded.bind(this, this.mapWindow.OnMapLoaded);
    },

    createCanvas : function (id, zIndex)
    {
        var div           = this.mapDoc.createElement("div");
        div.style.cssText = "position:absolute; top:0px; left:0px; z-index:" + zIndex + "; width:1px; height:1px; visibility:hidden";
        div.id            = id;
        this.mapWindow.$("img1").appendChild(div);
        return $(div);
    },
    
    refreshCanvasSize : function()
    {
        var width   = this.mapWindow.mapDevW + "px";
        var height  = this.mapWindow.mapDevH + "px";
        var opacity = 0.8;
        
        this.boxCanvas.style.width     = width;
        this.boxCanvas.style.height    = height;
        this.boxCanvas.setOpacity(opacity);
        this.handleCanvas.style.width  = width;
        this.handleCanvas.style.height = height;
        this.handleCanvas.setOpacity(opacity);
        // Don't set the grip canvas' size
        this.bgCanvas.style.width      = width;
        this.bgCanvas.style.height     = height;
        this.bgCanvas.setOpacity(opacity);
    },
    
    activate : function()
    {
        this.active                        = true;
        this.boxCanvas.style.visibility    = "visible";
        this.handleCanvas.style.visibility = "visible";
        this.gripCanvas.style.visibility   = "visible";
        this.bgCanvas.style.visibility     = "visible";
    },
    
    deactivate : function()
    {
        this.active                        = false;
        this.boxCanvas.style.visibility    = "hidden";
        this.handleCanvas.style.visibility = "hidden";
        this.gripCanvas.style.visibility   = "hidden";
        this.bgCanvas.style.visibility     = "hidden";
        this.captureBox                    = null;
        this.rotation                      = 0.0;
        this.inCommand                     = false;
        this.moving                        = false;
        this.rotating                      = false;
        this.mapWindow.ClearMapMessage();
        this.clear();
        this.clearSnappingHint();
    },

    setSize : function(paperSize, scale)
    {
        this.paperSize        = paperSize;
        this.scaleDenominator = scale;
        
        var res    = this.mapWindow.GetMetersPerUnits();

        // Create the capture box
        var size   = {width : paperSize.width * scale / 1000 / res , height : paperSize.height * scale / 1000 / res};
        var center = this.mapWindow.GetCenter();
        if (this.captureBox != null)
        {
            center = this.captureBox.getCentroid();
        }
        
        var coordinates = 
            [
                {x : center.X - size.width / 2, y : center.Y - size.height / 2},
                {x : center.X + size.width / 2, y : center.Y - size.height / 2},
                {x : center.X + size.width / 2, y : center.Y + size.height / 2},
                {x : center.X - size.width / 2, y : center.Y + size.height / 2},
                {x : center.X - size.width / 2, y : center.Y - size.height / 2}
            ];
            
        this.captureBox = this.createPolygon(coordinates);
        
        if (this.rotation != 0)
        {
            this.captureBox.rotate(this.rotation, center);
        }
        
        if (this.active)
        {
            if (this.validateResolution())
            {
                this.mapWindow.ClearMapMessage();
                this.draw();
            }
            else
            {
                this.mapWindow.ShowMapMessage(this.resolutionWarning, "warning");
                this.clear();
            }
        }
    },
    
    // The coordinates is a collection of a coordinate which is in following format:
    // {x: coordinate, y: coordinate}
    createPolygon : function (coordinates)
    {
        var polygon = new Polygon();
        
        var coordinate = null;
        for (var i = 0; i < coordinates.length; ++i)
        {
            coordinate = coordinates[i];
            polygon.AddPoint(new Point(coordinate.x, coordinate.y));
        }
        
        return polygon;
    },
    
    refreshRotateHandle : function(captureBox)
    {
        var box    = captureBox.clone();
        var center = captureBox.getCentroid();
        
        if (this.rotation != 0)
        {
            box.rotate(-this.rotation, center);
        }
        
        var size = box.getBounds().size;
        this.rotateHandleStart = new Point(center.X, center.Y);
        var length = size.height / 2 + this.rotateHandleLength / this.getPixelsPerMapUnits();
        point      = new Point(center.X, center.Y + length);
        this.rotateHandleEnd   = new Point(point.X, point.Y);
        
        if (this.rotation != 0)
        {
            this.rotateHandleStart.rotate(this.rotation, center);
            this.rotateHandleEnd.rotate(this.rotation, center);
        }
    },

    drawPolygon : function(polygon)
    {
        this.boxGraphics.setColor(this.fillColor);
        this.boxGraphics.setStroke(1);
        
        var xs     = new Array();
        var ys     = new Array();
        var point  = null;
        for (var i = 0; i < polygon.points.length; ++i)
        {
            point  = this.mapUnitsToScreen(polygon.Point(i));
            xs.push(point.X);
            ys.push(point.Y);
        }
        
        // Draw the polygon
        this.boxGraphics.fillPolygon(xs, ys);
        
        // Draw the border
        this.boxGraphics.setColor(this.strokeColor);
        this.boxGraphics.drawPolygon(xs, ys);
    },
    
    draw : function()
    {
        this.clear();
        // Draw the captureBox first
        this.drawPolygon(this.captureBox);
        // Refresh the rotate handle
        this.refreshRotateHandle(this.captureBox);
        
        // Draw the rotate handle
        var start = this.mapUnitsToScreen(this.rotateHandleStart);
        var end   = this.mapUnitsToScreen(this.rotateHandleEnd);
        this.handleGraphics.setColor(this.strokeColor);
        this.handleGraphics.setStroke(2);
        this.handleGraphics.drawLine(start.X, start.Y, end.X, end.Y);
        
        // Draw the rotate start point
        this.boxGraphics.fillEllipse(start.X - this.rotateGripDiameter / 2, start.Y - this.rotateGripDiameter / 2, this.rotateGripDiameter, this.rotateGripDiameter);
        // Draw the end point 
        this.gripGraphics.setColor(this.strokeColor);
        this.gripGraphics.fillEllipse(end.X - this.rotateGripDiameter / 2, end.Y - this.rotateGripDiameter / 2, this.rotateGripDiameter, this.rotateGripDiameter);
        
        this.boxGraphics.paint();
        this.handleGraphics.paint();
        this.gripGraphics.paint();
        
        this.setCursor();
    },
    
    setCursor : function()
    {
        var moveCursor   = this.eventSuppressed ? "default" : "move";
        var rotateCursor = this.eventSuppressed ? "default" : "url(../stdicons/rotate.cur), default";

        var nodes = this.boxCanvas.childNodes;
        for (var i = 0; i < nodes.length; ++i)
        {
            nodes[i].style.cursor = moveCursor;
        }
        
        nodes = this.gripCanvas.childNodes;
        for (var i = 0; i < nodes.length; ++i)
        {
            nodes[i].style.cursor = rotateCursor;
        }
    },
    
    // Suppress all event handlers, like move, rotate, show warning message when the capture box "overflows" the current viewport.
    // It's kind of "back-door" for some functions which are using the quick plot map capturer to draw a rectangle on the screen. Those
    // functions need only to draw a rectangle on the screen but don't need any events
    suppressEvent : function(suppress)
    {
        this.eventSuppressed = suppress;
        this.setCursor();
    },
    
    validateResolution : function()
    {
        if (this.captureBox)
        {
            var viewerSize = {width: this.mapWindow.extX2 - this.mapWindow.extX1, height: this.mapWindow.extY1 - this.mapWindow.extY2};
            var boxSize    = this.captureBox.getBounds().size;
            
            return boxSize.width < viewerSize.width || boxSize.height < viewerSize.height; 
        }
        
        return true;
    },
    
    clear : function()
    {
        this.boxGraphics.clear();
        this.handleGraphics.clear();
        this.gripGraphics.clear();
    },

    onMouseDown : function(evt, originalHandler)
    {
        var result = false;
        
        this.inCommand = this.rotating || this.moving && this.validateResolution();
        
        if ((!this.active || !this.inCommand) && originalHandler != null)
        {
            result = originalHandler.apply(this.mapWindow, Array.prototype.slice.call(arguments, 0, arguments.length - 1));
        }
        
        this.lastPixel = this.getMousePosition(evt);
        
        return result;
    },
    
    onMouseUp : function(evt, originalHandler)
    {
        var result = false;
        
        this.inCommand = this.rotating || this.moving && this.validateResolution();
        if ((!this.active || !this.inCommand) && originalHandler != null)
        {
            result = originalHandler.apply(this.mapWindow, Array.prototype.slice.call(arguments, 0, arguments.length - 1));
        }
        
        this.rotating  = false;
        this.moving    = false;
        this.inCommand = false;
        this.clearSnappingHint();

        return result;
    },
    
    onMouseMove : function(evt, originalHandler)
    {
        var result = false;
        
        if (this.active && !this.eventSuppressed)
        {
            var point  = this.getMousePosition(evt);

            if (!this.inCommand)
            {
                if (originalHandler != null)
                {
                    originalHandler.apply(this.mapWindow, Array.prototype.slice.call(arguments, 0, arguments.length - 1));
                }
                
                this.checkCommand(point);
            }
            else
            {
                if (this.rotating)
                {
                    this.rotate(evt);
                }
                else if (this.moving)
                {
                    this.move(evt);
                }
            }
        }
        else
        {
            if (originalHandler != null)
            {
                result = originalHandler.apply(this.mapWindow, Array.prototype.slice.call(arguments, 0, arguments.length - 1));
            }
        }

        return result;
    },
    
    panFeedback : function(evt, originalHandler)
    {
        if (originalHandler != null)
        {
            originalHandler(evt);
        }
        
        if (this.active && this.validateResolution())
        {
            var res = this.getPixelsPerMapUnits();
            var feedback = this.captureBox.clone();
            
            feedback.move(offx / res, -offy / res);
            this.draw(feedback);
        }
    },
    
    handleMouseWheelEvent : function(delta, evt)
    {
        originalHandler = this.mouseWheelHandlerBackup;
        if (originalHandler != null)
        {
            originalHandler(delta, evt);
        }
        
        if (this.active && this.validateResolution() && (mapInit && !mapLoading))
        {
            // Following functions are defined in ajaxmappane.templ
            var currentScale = CalculateScale();
            var newScale     = GetNewScale(currentScale, wheelZoomDelta);
            var zoomChange   = currentScale / newScale;
            
            //Determine the center of the new, zoomed map, in current screen device coords
            var x = wheelZoomCursor.X;
            var y = wheelZoomCursor.Y;
            var center = this.mapWindow.ScreenToMapUnits(x, y);
            
            var feedback = this.captureBox.clone();
            feedback.resize(zoomChange, center);
            this.draw(feedback);
        }
    },
    
    checkCommand : function (point)
    {
        if (this.validateResolution())
        {
            this.moving   = false;
            this.rotating = false;
            
            if (this.inRotateRange(point))
            {
                this.rotating = true;
            }
            else if (this.inMoveRange(point))
            {
                this.moving   = true;
            }
        }
    },
    
    inRotateRange : function(point)
    {
        var p1 = this.mapWindow.ScreenToMapUnits(0, 0);
        var p2 = this.mapWindow.ScreenToMapUnits(this.rotateGripDiameter, 0);
        var radius = p1.distanceTo(p2) / 2;
        var distance = this.rotateHandleEnd.distanceTo(point);
        
        return distance <= radius;
    },
    
    inMoveRange : function(point)
    {
        var box    = this.captureBox.clone();
        var p      = point.clone();
        
        if (this.rotation != 0)
        {
            var origin = box.getCentroid();
            box.rotate(-this.rotation, origin);
            p.rotate(-this.rotation, origin);
        }
        
        var x1 = Math.min(box.Point(0).X, box.Point(1).X, box.Point(2).X, box.Point(3).X);   
        var x2 = Math.max(box.Point(0).X, box.Point(1).X, box.Point(2).X, box.Point(3).X);
        var y1 = Math.min(box.Point(0).Y, box.Point(1).Y, box.Point(2).Y, box.Point(3).Y);
        var y2 = Math.max(box.Point(0).Y, box.Point(1).Y, box.Point(2).Y, box.Point(3).Y);

        return (p.X <= x2 && p.X >= x1) && (p.Y <= y2 && p.Y >= y1);
    },
    
    onMouseOut : function(evt, originalHandler)
    {
        console.info("mouse out");
    },
    
    onMapLoaded : function(originalHandler, args)
    {
        if (originalHandler != null)
        {
            if (!args)
            {
                args = [];
            }
            
            originalHandler.apply(this.mapWindow, args);
        }
        
        this.refreshCanvasSize();
        
        if (this.active)
        {
            if (this.validateResolution() || this.eventSuppressed)
            {
                this.mapWindow.ClearMapMessage();
                this.draw(this.captureBox);
            }
            else
            {
                this.mapWindow.ShowMapMessage(this.resolutionWarning, "warning");
                this.clear();
            }
        }
    },
    
    getMousePosition : function(evt)
    {
        var offset = this.getScreenOffset();
        var pixel  = new Point(evt.clientX - offset.left, evt.clientY - offset.top);
        
        return this.mapWindow.ScreenToMapUnits(pixel.X, pixel.Y);
    },
    
    move : function(evt)
    {
        var pixel      = this.getMousePosition(evt);
        var x          = pixel.X - this.lastPixel.X;
        var y          = pixel.Y - this.lastPixel.Y;
        this.captureBox.move(x, y);
        this.rotateHandleStart.move(x, y);
        this.rotateHandleEnd.move(x, y);
        this.lastPixel = pixel;
        
        this.draw(this.captureBox);
    },
    
    rotate : function(evt)
    {
        var pixel  = this.getMousePosition(evt);
        
        // Calculate the rotate angle, in degrees / counterclockwise
        var origin = this.captureBox.getCentroid();
        var angle1 = Math.atan2(this.lastPixel.Y - origin.Y, this.lastPixel.X - origin.X);  
        var angle2 = Math.atan2(pixel.Y - origin.Y, pixel.X - origin.X);
        var angle  = (angle2 - angle1) * 180 / Math.PI;

        var rotation = this.rotation + angle;
        if (evt.shiftKey)
        {
            var a = rotation % this.rotateSnappingStep;
            var b = Math.round(rotation / this.rotateSnappingStep);
            var targetRotation = rotation;
            
            if (Math.abs(a) <= this.rotateSnappingTolerance || Math.abs(a)>= this.rotateSnappingStep - this.rotateSnappingTolerance)
            {
                targetRotation = b * this.rotateSnappingStep;
                
                // Draw the snapping guide
                this.drawSnappingHint(targetRotation);
            }
            else
            {
                this.clearSnappingHint();
            }
            
            angle = targetRotation - this.rotation;
        }
        else
        {
            this.clearSnappingHint();
        }
        
        this.captureBox.rotate(angle, origin);
        this.rotateHandleStart.rotate(angle, origin);
        this.rotateHandleEnd.rotate(angle, origin);
        this.lastPixel = pixel;
        this.rotation += angle;
        
        this.draw(this.captureBox);
    },
    
    drawSnappingHint : function(angle)
    {
        this.clearSnappingHint();
        
        this.bgGraphics.setColor("black");
        var length = Math.sqrt(Math.pow(this.mapWindow.mapDevW, 2), Math.pow(this.mapWindow.mapDevH, 2));
        var origin = this.mapUnitsToScreen(this.captureBox.getCentroid());
        var line   = new LineString();
        line.AddPoint(new Point(origin.X, origin.Y - length));
        line.AddPoint(new Point(origin.X, origin.Y + length));
        
        if (angle != 0)
        {
            line.rotate(-angle, origin);
            
        }
        
        var start = line.Point(0);
        var end   = line.Point(1);
        this.bgGraphics.drawLine(start.X, start.Y, end.X, end.Y);
        
        this.bgGraphics.paint();
    },
    
    clearSnappingHint : function()
    {
        this.bgGraphics.clear();
    },

    getCaptureBox : function()
    {
        return this.captureBox;
    },
    
    getNormalizedCapture : function()
    {
        var capture = this.captureBox.clone();
        
        if (this.rotation != 0)
        {
            capture.rotate(-this.rotation, capture.getCentroid());
        }
        
        return capture;
    },
    
    getPixelsPerMapUnits : function()
    {
        var p1 = this.mapWindow.ScreenToMapUnits(0, 0);
        var p2 = this.mapWindow.ScreenToMapUnits(1, 0);
        return 1 / (p2.X - p1.X);
    },
    
    mapUnitsToScreen : function(point)
    {
        var x = (point.X - this.mapWindow.extX1) * this.mapWindow.mapDevW / (this.mapWindow.extX2 - this.mapWindow.extX1);
        var y = (point.Y - this.mapWindow.extY1) * this.mapWindow.mapDevH / (this.mapWindow.extY2 - this.mapWindow.extY1);
        return new Point(x, y);
    },
    
    getScreenOffset : function()
    {
        return this.getContentPosition(this.mapWindow.$("mapImage1"));
    },
    
    getContentPosition: function(element)
    {
        var offsetLeft = 0;
        var offsetTop  = 0;
        var border     = 0;
        
        while (element && element.tagName.toLowerCase() != "body" && element.tagName.toLowerCase() != "html")
        {
            offsetLeft += element.offsetLeft;
            offsetTop  += element.offsetTop;
            
            border      = parseInt(element.style.borderLeftWidth);
            if (!isNaN(border))
            {
                offsetLeft += border;
            }
            
            border      = parseInt(element.style.borderTopWidth);
            if (!isNaN(border))
            {
                offsetTop += border;
            }
            
            element     = element.offsetParent;
        }
        
        return {left: offsetLeft, top: offsetTop};
    }
};

/////////////////////////////

var PreviewDialog = function()
{
};

PreviewDialog.prototype = 
{
    jxDialog : null,
    previewFrame : null,
    innerDoc : null,
    indicator : null,
    previewPicture : null,
    printPicture : null,
    topLeftXYLabel : null,
    bottomRightXYLabel : null,
    previewContainer : null,
    pictureContainer : null,
    printButton : null,
    cancelButton : null,
    loadingErrorMessage : null,
    printStyle: null,
    previewStyle: null,
    rendererName: null,
    // The offset from the border of the paper, in px 
    csLabelOffset : 5,
    // The print margin, in inches
    printMargin: 0,
    mapInfo : {sessionID: "", name: ""},
    captureInfo : {topLeftCs : {x : 0, y : 0}, bottomRightCs : {x : 0, y : 0}, paperSize:{w : 0, h : 0}, scaleDenominator : 0, rotation : 0, params1 : "", params2 : ""},
    params : null,
    
    // The callback will be called right after the JxDialog content has been loaded.
    // The callback accepts 1 parameter: windowName (string)
    // Here it is used to submit the quick plot form to the preview frame.
    contentLoadedCallback : null,
    
    initialize : function(options)
    {
        this.mapInfo      = options.mapInfo;
        this.captureInfo  = options.captureInfo;
        this.rendererName = options.rendererName;
        this.params       = options.params;
        
        this.jxDialog = new Jx.Dialog(
        {
            modal: true,
            width:400,
            height: 400,
            content: '<table border="0" cellspacing="0" cellpadding="0" id="PreviewContainer">' + 
                     '	<tr>' + 
                     '		<td>' + 
                     '			<iframe id="PreviewFrame" scrolling="no" frameborder="0" style="border: 0px; width: 400px; height: 300px;" src="about:blank"></iframe>' +
                     '		</td>' +
                     '  </tr>' +
                     '</table>'
        });
        
        this.jxDialog.addEvent("open", this.jxDialogOpened.bind(this, true));
        this.jxDialog.addEvent("contentLoaded", this.jxDialogContentLoaded.bind(this));
        // Listen to the fade complete event to close the dialog
        var o = this.jxDialog.domObj.get("tween").addEvent("complete", this.closeJxDialog.bind(this));
    },
    
    open : function(contentLoadedCallback, openedCallback, closedCallback)
    {
        this.contentLoadedCallback = contentLoadedCallback;
        this.openedCallback        = openedCallback;
        this.closedCallback        = closedCallback;
        this.jxDialog.loadContent(this.jxDialog.content);
        this.jxDialog.open();
    },
    
    print : function()
    {
        //
        this.previewFrame.contentWindow.doPrint();
    },
    
    cancel : function()
    {
        this.indicator.style.display     = "inline";
        this.indicator.style.visibility  = "visible";
        this.indicator.setOpacity(1);
        
        this.loadingErrorMessage.fade(0);
        this.loadingErrorMessage.style.display = "none";
        // Hide the picture but don't set the 'display' style property to avoid messing the print layout up
        this.printPicture.style.height   = "0px";
        
        if (this.topLeftXYLabel && this.bottomRightXYLabel)
        {
            this.topLeftXYLabel.setOpacity(0);
            this.bottomRightXYLabel.setOpacity(0);
            this.printLabel.setOpacity(0);
            // Remove the labels
            this.topLeftXYLabel.parentNode.removeChild(this.topLeftXYLabel);
            this.topLeftXYLabel     = null;
            this.bottomRightXYLabel.parentNode.removeChild(this.bottomRightXYLabel);
            this.bottomRightXYLabel = null;
            this.printLabel.parentNode.removeChild(this.printLabel);
            this.printLabel         = null;
        }
        
        this.isClosing = true;
        this.jxDialog.blanket.fade(0);
        this.jxDialog.domObj.fade(0);
        
        // Inform the listener that the dialog is closed
        if (this.closedCallback)
        {
            this.closedCallback();
        }
    },
    dispose : function()
    {
        this.jxDialog.blanket.parentNode.removeChild(this.jxDialog.blanket);
        this.jxDialog.domObj.parentNode.removeChild(this.jxDialog.domObj);
        // Inform the listener that the dialog is closed
        if (this.closedCallback)
        {
            this.closedCallback();
        }
    },
    
    jxDialogContentLoaded : function()
    {
        // Set the window name for the preview frame
        this.previewFrame = $("PreviewFrame");
        this.previewFrame.contentWindow.name = "QuickPlotPreviewFrame";
        
        // Inform the listener that the content is ready, then the Quick Plot panel can submit its data to the preview frame
        if (this.contentLoadedCallback)
        {
            this.contentLoadedCallback(this.previewFrame.contentWindow.name);
        }
    },
    
    jxDialogOpened : function(setFrameUrl)
    {
        if (this.previewInnerIsLoaded)
        {
            // Resize the preview frame according to the monitor resolution
            this.innerDoc         = this.previewFrame.contentWindow.document;
            var box       = $(document.body).getDimensions();
            // Width of preview dialog = screen width * factor
            var factor    = 0.5;
            this.previewContainer = $(this.innerDoc.getElementById("PreviewContainer"));
            this.previewContainer.style.width = box.width * factor + "px";
            this.pictureContainer = $(this.innerDoc.getElementById("PictureContainer"));
            var pcBox = this.pictureContainer.getContentBoxSize();
            
            this.indicator        = $(this.innerDoc.getElementById("ProgressIndicator"));
            
            var paperSize  = this.captureInfo.paperSize;
            this.paperSize = paperSize;
            
            var ratio = paperSize.w / paperSize.h;
            
            // Resize the indicator
            this.indicator.style.width  = pcBox.width + "px";
            this.indicator.style.height = pcBox.width / ratio + "px";
            // Set a explicit size to the container
            this.pictureContainer.style.width  = this.indicator.style.width;
            this.pictureContainer.style.height = this.indicator.style.height;
            // Get the styles for the print picture
            var rules = this.innerDoc.styleSheets[1].cssRules || this.innerDoc.styleSheets[1].rules;
            this.previewStyle = rules[0];
            rules     = this.innerDoc.styleSheets[2].cssRules || this.innerDoc.styleSheets[2].rules;
            this.printStyle   = rules[0];
            
            // Reset the background
            this.indicator.style.background = "url(../stdicons/progress_indicator.gif) no-repeat cneter center";
            
            this.loadingErrorMessage = $(this.innerDoc.getElementById("PictureLoadingErrorMessage"));
            this.loadingErrorMessage.setOpacity(0);
            
            // Set the picture url
            var src = this.rendererName + "?session_id=" + this.mapInfo.sessionID +
                  "&map_name=" + this.mapInfo.name + 
                  "&print_dpi=" + this.params.printDpi + 
                  "&paper_size=" + this.captureInfo.paperSize.w + "," + this.captureInfo.paperSize.h + 
                  "&box=" + this.captureInfo.params1 +
                  "&normalized_box=" + this.captureInfo.params2 + 
                  "&scale_denominator=" + this.captureInfo.scaleDenominator + 
                  "&rotation=" + this.captureInfo.rotation; 
                
            this.printPicture       = $(this.innerDoc.getElementById("PrintPicture"));
            this.printPicture.src   = src;
            
            // Listen to print picture onload vent
            this.printPicture.addEvent("load", this.printPictureLoaded.bind(this));
            this.printPicture.addEvent("error", this.printPictureLoadError.bind(this));
            
            var innerBox  = this.previewContainer.getMarginBoxSize();
            // Resize the frame according to the inner container's 
            this.previewFrame.style.width  = innerBox.width + "px";
            this.previewFrame.style.height = innerBox.height + "px";
            
            // Hide the title bar
            this.jxDialog.title.style.display  = "none";
            // Hide the chrome
            this.jxDialog.chrome.style.display = "none";
            
            // Disable the print button until the image is ready
            this.printButton           = this.innerDoc.getElementById("PrintButton");
            this.cancelButton          = this.innerDoc.getElementById("CancelButton");
            this.printButton.disabled  = true;
            this.cancelButton.disabled = true;
            
            var delta     = {x: 22, y: 43};
            var container = $("PreviewContainer");
            var size      = container.getMarginBoxSize();
            this.jxDialog.resize(size.width + delta.x, size.height + delta.y, true);

            // Inform the listeners that the dialog is opened
            if (this.openedCallback)
            {
                this.openedCallback();
            }
            this.jxDialog.blanket.fade(0.2);
            this.jxDialog.domObj.fade(1);
            this.resizeIsPending = false;
            
            this.previewInnerIsLoaded = false;
        }
        else
        {
            this.jxDialog.blanket.setOpacity(0);
            this.jxDialog.domObj.setOpacity(0);
            this.resizeIsPending = true;
        }
    },
    
    previewInnerLoaded: function()
    {
        this.previewInnerIsLoaded = true;
        if (this.resizeIsPending)
        {
            this.jxDialogOpened(false);
        }
    },
    
    closeJxDialog : function()
    {
        if (this.isClosing)
        {
            this.jxDialog.close();
            this.isClosing = false;
        }
    },
    
    printPictureLoaded : function()
    {
        var size = {width: parseInt(this.indicator.style.width), height: parseInt(this.indicator.style.height)};
        
        this.indicator.fade(0);
        this.indicator.style.display = "none";
        
        // Set the preview size
        this.printPicture.setOpacity(0);
        // Clear the inline style
        this.printPicture.style.width   = "";
        this.previewStyle.style.width   = size.width + "px";
        this.previewStyle.style.height  = size.height + "px";
        this.printPicture.fade(1);
        
        // Set the print size
        // NOTE: It works only with a 96 dpi device dpi
        var deviceDpi  = 96;
        var idealSize  = {width:(this.paperSize.w / 25.4 - 2 * this.printMargin) * deviceDpi, height:(this.paperSize.h / 25.4 - 2 * this.printMargin) * deviceDpi};
        // Get the size of the print frame
        var docSize    = $(this.innerDoc.body).getContentBoxSize();
        var realHeight = idealSize.height - (docSize.height - size.height);
        var realWidth  = realHeight * this.paperSize.w / this.paperSize.h;
        if (realWidth > idealSize.width)
        {
            realWidth = idealSize.width;
            realHeight = realWidth / (this.paperSize.w / this.paperSize.h);
        }
        
        this.printStyle.style.width  = realWidth + "px";
        this.printStyle.style.height = realHeight + "px";
        
        // Create the coordinates labels
        if (!this.topLeftXYLabel)
        {
            this.topLeftXYLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.topLeftCs.x, this.captureInfo.topLeftCs.y, "TopLeftXYLabel");
            this.topLeftXYLabel.setOpacity(0);
        }
        
        if (!this.bottomRightXYLabel)
        {
            this.bottomRightXYLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.bottomRightCs.x, this.captureInfo.bottomRightCs.y, "BottomRightXYLabel");
            this.bottomRightXYLabel.setOpacity(0);
        }
        
        if (!this.printLabel)
        {
            this.printLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.bottomRightCs.x, this.captureInfo.bottomRightCs.y, "PrintLabel");
        }
        
        // Set the correct positions for the labels
        var pos    = this.getContentPosition(this.pictureContainer);
        var picDim = this.pictureContainer.getContentBoxSize();
        this.topLeftXYLabel.style.left     = pos.left + this.csLabelOffset + "px";
        this.topLeftXYLabel.style.top      = pos.top + this.csLabelOffset + "px"; 
        var labelDim = this.bottomRightXYLabel.getMarginBoxSize();
        this.bottomRightXYLabel.className  = "ScreenOnly";
        this.bottomRightXYLabel.style.left = pos.left + picDim.width - this.csLabelOffset - labelDim.width + "px";
        this.bottomRightXYLabel.style.top  = pos.top + picDim.height - this.csLabelOffset - labelDim.height + "px";
        labelDim = this.printLabel.getMarginBoxSize();
        this.printLabel.className          = "PrintOnly";
        this.printLabel.style.left         = pos.left + realWidth - this.csLabelOffset - labelDim.width + "px";
        this.printLabel.style.top          = pos.top + realHeight - this.csLabelOffset - labelDim.height + "px";
        
        this.topLeftXYLabel.fade(1);
        this.bottomRightXYLabel.fade(1);
        // Enable the print button
        this.printButton.disabled  = false;
        this.cancelButton.disabled = false;
    },
    
    printPictureLoadError : function()
    {
        this.indicator.fade(0);
        this.indicator.style.display = "none";
        this.loadingErrorMessage.style.display = "inline";
        this.loadingErrorMessage.setOpacity(0);
        this.loadingErrorMessage.fade(1);
        this.cancelButton.disabled = false;
    },
    
    createCoordinateLabel: function(container, cX, cY, id)
    {
        cX = cX + "";
        cY = cY + "";
        var digits = 6;
        
        var index = cX.indexOf(".");
        if (index > -1)
        {
            cX = cX.substr(0, index + digits + 1);
        }
        
        index = cY.indexOf(".");
        if (index > -1)
        {
            cY = cY.substr(0, index + digits + 1);
        }
        
        var label = this.innerDoc.createElement("div");
        container.appendChild(label);
        label.id  = id;
        label.style.cssText = "border:solid 1px black; padding:1px; background:#F2F2F2; color:black; font-size:8pt; z-index:1000; " +
                              "position:absolute; white-space:nowrap";
        label.innerHTML     = "X: " + cX + " Y: " + cY;
        
        return $(label);
    },
    
    getContentPosition: function(element)
    {
        var offsetLeft = 0;
        var offsetTop  = 0;
        var border     = 0;
        
        while (element && element.tagName.toLowerCase() != "body" && element.tagName.toLowerCase() != "html")
        {
            offsetLeft += element.offsetLeft;
            offsetTop  += element.offsetTop;
            
            border      = parseInt(element.style.borderLeftWidth);
            if (!isNaN(border))
            {
                offsetLeft += border;
            }
            
            border      = parseInt(element.style.borderTopWidth);
            if (!isNaN(border))
            {
                offsetTop += border;
            }
            
            element     = element.offsetParent;
        }
        
        return {left: offsetLeft, top: offsetTop};
    }
};

/////////////////////////

function previewQuickPlot(dialogConentLoadedCallback, dialogOpenedCallback, dialogClosedCallback, printDpi, rendererName)
{
    var mapInfo     = {sessionID : GetSessionId(), name : GetMapName()};
    
    var capture     = mapCapturer.getCaptureBox();
    var vertices    = capture.points;
    
    var params1     = "";
    for (var i = 0; i < vertices.length; ++i)
    {
        var vertice = vertices[i];
        params1    += vertice.X + "," + vertice.Y + ",";
    }
    params1         = params1.substr(0, params1.length - 1);
    
    var vertices2   = mapCapturer.getNormalizedCapture().points;
    var params2     = "";
    for (var i = 0; i < vertices2.length; ++i)
    {
        var vertice = vertices2[i];
        params2    += vertice.X + "," + vertice.Y + ",";
    }
    params2         = params2.substr(0, params2.length - 1);
    
    var captureInfo = {topLeftCs : {x : vertices[3].X, y : vertices[3].Y},
                       bottomRightCs : {x : vertices[1].X, y : vertices[1].Y},
                       paperSize : {w : mapCapturer.paperSize.width, h : mapCapturer.paperSize.height},
                       scaleDenominator : mapCapturer.scaleDenominator,
                       rotation : mapCapturer.rotation,
                       params1 : params1,
                       params2 : params2}; 
    // Change the parameters here to control the print dpi and if
    // handle the label font unit type automatically
    var params      = {printDpi : printDpi};
    
    if (!quickPlotPreviewDialog)
    {
        quickPlotPreviewDialog = new PreviewDialog();
        quickPlotPreviewDialog.initialize({mapInfo : mapInfo, captureInfo : captureInfo, rendererName : rendererName, params : params});
    }
    else
    {
        quickPlotPreviewDialog.mapInfo      = mapInfo;
        quickPlotPreviewDialog.captureInfo  = captureInfo;
        quickPlotPreviewDialog.rendererName = rendererName;
        quickPlotPreviewDialog.params       = params;
    }

    quickPlotPreviewDialog.open(dialogConentLoadedCallback, dialogOpenedCallback, dialogClosedCallback);
}

function endPlotPreview()
{
    if (quickPlotPreviewDialog != null)
    {
        quickPlotPreviewDialog.dispose();
        quickPlotPreviewDialog = null;
    }
}

var mapCapturer = new MapCapturer();
var quickPlotPreviewDialog = null;
var message     = null;

