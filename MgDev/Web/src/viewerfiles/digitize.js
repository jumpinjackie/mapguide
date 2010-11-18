function Point(x, y)
{
    this.X = x;
    this.Y = y;
    
    this.rotate = function(angle, origin)
    {
        angle     *= Math.PI / 180;
        var radius = this.distanceTo(origin);
        var theta  = angle + Math.atan2(this.Y - origin.Y, this.X - origin.X);
        this.X     = origin.X + radius * Math.cos(theta);
        this.Y     = origin.Y + radius * Math.sin(theta);
        
        return this;
    };

    this.move = function(x, y)
    {
        this.X += x;
        this.Y += y;
        
        return this;
    };

    this.resize = function(scale, origin, ratio)
    {
        ratio = (ratio == undefined) ? 1 : ratio;
        this.X = origin.X + (scale * ratio * (this.X - origin.X));
        this.Y = origin.Y + (scale * (this.Y - origin.Y));
        
        return this;
    };

    this.distanceTo = function(point)
    {
        return Math.sqrt(Math.pow(this.X - point.X, 2) + Math.pow(this.Y - point.Y, 2));
    };

    this.clone = function()
    {
        return new Point(this.X, this.Y);
    };
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
    
    this.rotate = function (angle, origin)
    {
        for (var i = 0; i < this.points.length; ++i)
        {
            this.points[i].rotate(angle, origin);
        }
        
        return this;
    };
    
    this.move = function (x, y)
    {
        for (var i = 0; i < this.points.length; ++i)
        {
            this.points[i].move(x, y);
        }
        
        return this;
    };

    
    this.resize = function(scale, origin, ratio)
    {
        for (var i = 0; i < this.points.length; ++i)
        {
            this.points[i].resize(scale, origin, ratio);
        }
        
        return this;
    };
    
    this.getBounds = function()
    {
        var xs = [];
        var ys = [];
        for (var i = 0; i < this.points.length; ++i)
        {
            xs.push(this.points[i].X);
            ys.push(this.points[i].Y);
        }
        
        var left   = xs.min();
        var right  = xs.max();
        var top    = ys.min();
        var bottom = ys.max();
        var size   = {width: right - left, height: bottom - top};
        
        return {left: left, right: right, top: top, bottom: bottom, size: size};
    };
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

function Rectangle()
{
    this.Point1 = null;
    this.Point2 = null;
}

function Polygon()
{
    this.LineStringInfo = LineString;
    this.LineStringInfo();

    this.clone = function()
    {
        var c = new Polygon();
        
        for (var i = 0; i < this.points.length; ++i)
        {
            c.AddPoint(this.points[i].clone());
        }
        
        return c;
    };

    this.getCentroid = function()
    {
        var sumX = 0.0;
        var sumY = 0.0;
        
        for (var i = 0; i < this.points.length - 1; ++i)
        {
            var b = this.points[i];
            var c = this.points[i + 1];
            
            sumX += (b.X + c.X) * (b.X * c.Y - c.X * b.Y);
            sumY += (b.Y + c.Y) * (b.X * c.Y - c.X * b.Y);
        }
        
        var area = -1 * this.getArea();
        var x = sumX / (6 * area);
        var y = sumY / (6 * area);
        
        return new Point(x, y);
    };

    this.getArea = function()
    {
        var sum  = 0.0;
        
        for (var i = 0; i < this.points.length - 1; ++i)
        {
            var b = this.points[i];
            var c = this.points[i + 1];
            sum  += (b.X + c.X) * (c.Y - b.Y);
        }
        
        return -sum / 2.0;
    };
}

function Digitizer(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH, tiphandler, tiptext)
{
    this.handler = handler;
    this.cnvfunc = cnvfunc;
    this.fbShape = fbShape;
    this.fbDiv = fbDiv;
    this.fbColor = fbColor;
    this.fbPos = fbPos;
    this.fbW = fbW;
    this.fbH = fbH;
    this.tiphandler = tiphandler;
    this.tiptext = tiptext;
    this.cancelTgt = cancelTgt;

    this.InitFeedback = function()
    {
        var rd = document.getElementById(this.fbDiv).style;
        rd.position = "absolute";
        rd.left = this.fbPos + "px";
        rd.top = "0px";
        rd.width = this.fbW + "px";
        rd.height = this.fbW + "px";
        rd.visibility = "visible";
        rd.clip = "rect(0px " + this.fbW + "px " + this.fbH + "px " + "0px)";
    }
    this.ClearFeedback = function()
    {
        if(this.fbDiv)
            document.getElementById(this.fbDiv).style.visibility = "hidden";
        if(this.fbShape)
            this.fbShape.clear();
    }
    this.DisplayTipFeedback = function(x, y, show)
    {
        if(this.tiphandler != null)
            this.tiphandler(x + this.fbPos + 16, y + 16, this.tiptext, show);
    }
    this.Cancel = function()
    {
        this.ClearFeedback();
        this.DisplayTipFeedback(0, 0, false);
    }
    this.EnableCancel = function()
    {
        if(this.cancelTgt)
            this.cancelTgt.focus();
    }
}

function PointDigitizer(handler, cnvfunc)
{
    this.DigitizerInfo = Digitizer;
    this.DigitizerInfo(handler, cnvfunc, null, null, null, null, null, null);

    this.MouseDown = function(e, x, y)
    {
    }
    this.MouseUp = function(e, x, y)
    {
        this.handler(cnvfunc(x, y));
    }
    this.MouseMove = function(e, x, y)
    {
    }
}

function LineDigitizer(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH)
{
    this.DigitizerInfo = Digitizer;
    this.DigitizerInfo(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH);
    this.line = new LineString();
    this.xstart = 0;
    this.ystart = 0;

    this.MouseDown = function(e, x, y)
    {
        this.xstart = x;
        this.ystart = y;
        this.line.AddPoint(cnvfunc(x, y));
        this.InitFeedback();
    }
    this.MouseUp = function(e, x, y)
    {
        this.line.AddPoint(cnvfunc(x, y));
        this.ClearFeedback();
        this.handler(this.line);
    }
    this.MouseMove = function(e, x, y)
    {
        if(this.line.Count > 0)
            this.Feedback(x, y);
    }
    this.Feedback = function(x, y)
    {
        this.fbShape.clear();
        this.fbShape.setColor(this.fbColor);
        this.fbShape.drawLine(this.xstart, this.ystart, x, y);
        this.fbShape.paint();
    }
}

function CircleDigitizer(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH)
{
    this.DigitizerInfo = Digitizer;
    this.DigitizerInfo(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH);
    this.circle = new Circle();
    this.xstart = -1;
    this.ystart = -1;

    this.MouseDown = function(e, x, y)
    {
        this.xstart = x;
        this.ystart = y;
        this.circle.Center = cnvfunc(x, y);
        this.InitFeedback();
    }
    this.MouseUp = function(e, x, y)
    {
        this.ClearFeedback();
        this.circle.SetRadius(cnvfunc(x, y));
        this.handler(this.circle);
    }
    this.MouseMove = function(e, x, y)
    {
        if(this.xstart != -1)
            this.Feedback(x, y);
    }
    this.GetRadius = function(x, y)
    {
        dx = x - this.xstart;
        dy = y - this.ystart;
        return Math.sqrt(dx*dx + dy*dy);
    }
    this.Feedback = function(x, y)
    {
        r = this.GetRadius(x, y);
        x1 = this.xstart - r;
        w = 2 * r;
        y1 = this.ystart - r;
        h = w;
        this.fbShape.clear();
        this.fbShape.setColor(this.fbColor);
        this.fbShape.drawEllipse(x1, y1, w, h);
        this.fbShape.paint();
    }
}

function RectangleDigitizer(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH)
{
    this.DigitizerInfo = Digitizer;
    this.DigitizerInfo(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH);
    this.rect = new Rectangle();
    this.xstart = -1;
    this.ystart = -1;

    this.MouseDown = function(e, x, y)
    {
        this.xstart = x;
        this.ystart = y;
        this.rect.Point1 = cnvfunc(x, y);
        this.InitFeedback();
    }
    this.MouseMove = function(e, x, y)
    {
        if(this.xstart != -1)
            this.Feedback(x, y);
    }
    this.MouseUp = function(e, x, y)
    {
        this.ClearFeedback();
        this.rect.Point2 = cnvfunc(x, y);
        if(this.xstart > x)
        {
            x = this.rect.Point1.X;
            this.rect.Point1.X = this.rect.Point2.X;
            this.rect.Point2.X = x;
        }
        if(this.ystart > y)
        {
            y = this.rect.Point1.Y;
            this.rect.Point1.Y = this.rect.Point2.Y;
            this.rect.Point2.Y = y;
        }
        this.handler(this.rect);
    }
    this.Feedback = function(x, y)
    {
        x1 = this.xstart < x? this.xstart: x;
        y1 = this.ystart < y? this.ystart: y;
        this.fbShape.clear();
        this.fbShape.setColor(this.fbColor);
        this.fbShape.drawRect(x1, y1, Math.abs(this.xstart - x), Math.abs(this.ystart - y));
        this.fbShape.paint();
    }
}

function LineStringDigitizer(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH, tiphandler, tiptext, closed)
{
    this.DigitizerInfo = Digitizer;
    this.DigitizerInfo(handler, cnvfunc, cancelTgt, fbShape, fbDiv, fbColor, fbPos, fbW, fbH, tiphandler, tiptext);
    this.closed = closed;
    this.pline = closed? (new Polygon()) : (new LineString());
    this.xs = new Array();
    this.ys = new Array();

    this.MouseDown = function(e, x, y)
    {
    }
    this.MouseUp = function(e, x, y)
    {
        if(this.xs.length == 0)
            this.InitFeedback();
        this.pline.AddPoint(cnvfunc(x, y));
        var macOS = navigator.appVersion.indexOf("Mac")!=-1;
        var endLine = macOS ? e.shiftKey : e.ctrlKey;
        if(!endLine)
        {
            if(this.xs.length == 0)
            {
                this.xs.push(x); this.xs.push(x);
                this.ys.push(y); this.ys.push(y);
            }
            else
            {
                this.xs[this.xs.length - 1] = x;
                this.ys[this.ys.length - 1] = y;
                this.xs.push(x); this.ys.push(y);
            }
            this.EnableCancel();
        }
        else
        {
            this.ClearFeedback();
            this.DisplayTipFeedback(0, 0, false);
            if(closed)
                this.pline.AddPoint(this.pline.Point(0));
            this.handler(this.pline);
        }
    }
    this.MouseMove = function(e, x, y)
    {
        if(this.xs.length > 0)
        {
            this.Feedback(x, y);
            this.DisplayTipFeedback(x, y, true);
        }
    }
    this.Feedback = function(x, y)
    {
        this.xs[this.xs.length - 1] = x;
        this.ys[this.ys.length - 1] = y;
        this.fbShape.clear();
        this.fbShape.setColor(this.fbColor);
        if(this.xs.length == 2)
            this.fbShape.drawLine(this.xs[0], this.ys[0], this.xs[1], this.ys[1]);
        else
        {
            if(closed)
                this.fbShape.drawPolygon(this.xs, this.ys);
            else
                this.fbShape.drawPolyline(this.xs, this.ys);
        }
        this.fbShape.paint();
    }
    this.ScaleFeedback = function(scale, origin)
    {
        for (var i = 0; i < this.xs.length; ++i)
        {
            this.xs[i] = origin.X + (scale * (this.xs[i] - origin.X));
            this.ys[i] = origin.Y + (scale * (this.ys[i] - origin.Y));
        }

        this.Feedback(origin.X, origin.Y);
    }
}
