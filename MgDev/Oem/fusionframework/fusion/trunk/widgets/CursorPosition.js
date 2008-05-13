/**
 * Fusion.Widget.CursorPosition
 *
 * $Id: CursorPosition.js 1396 2008-05-08 15:34:30Z madair $
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
 * Class: Fusion.Widget.CursorPosition
 *
 * Displays the geographic position of the cursor when it is over the map.
 *
 * Precision (integer, optional)
 *
 * The number of digits to round the output to.  The geographic location
 * is calculated with arbitrary precision and is often not necessary. A
 * value of less than 0 means no rounding (the default if parameter is
 * missing).
 *
 * Template (string, optional) 
 *
 * The format of the output string.  Use {x} and {y} as placeholders for
 * the x and y location.  The default template is:
 *
 * x: {x}, y: {y}
 *
 * You can embed HTML in the template, but you must escape any characters
 * that result in illegal HTML.  This would include:
 *
 * < is &lt;
 * > is &gt;
 * & is &amp;
 *
 * So a two-line display would be:
 *
 * x: {x}&lt;br/&gt;y: {y}
 * **********************************************************************/

Fusion.Widget.CursorPosition = Class.create();
Fusion.Widget.CursorPosition.prototype = {
    defaultTemplate: 'x: {x}, y: {y}',
    domSpan: null,
    
    /* the units to display distances in */
    units: Fusion.UNKNOWN,

    initialize : function(widgetTag) {
        //console.log('CursorPosition.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
                
        
        var json = widgetTag.extension;
        
        this.emptyText = json.EmptyText ? json.EmptyText[0] : this.domObj.innerHTML;
        this.template = json.Template ? json.Template[0] : this.defaultTemplate;
        this.precision = json.Precision ? parseInt(json.Precision[0]) : -1;
        this.units = json.Units ? Fusion.unitFromName(json.Units[0]) : Fusion.UNKOWN;

        this.domSpan = document.createElement('span');
        this.domSpan.className = 'spanCursorPosition';
        this.domSpan.innerHTML = this.emptyText;
        this.domObj.innerHTML = '';
        this.domObj.appendChild(this.domSpan);

        this.enable = Fusion.Widget.CursorPosition.prototype.enable;
        this.disable = Fusion.Widget.CursorPosition.prototype.enable;
        
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.setUnits.bind(this));
        this.registerParameter('Units');
    },
    
    enable: function() {
        this.mouseMoveWatcher = this.mouseMove.bind(this);
        this.mouseOutWatcher = this.mouseOut.bind(this);

        this.getMap().observeEvent('mousemove', this.mouseMoveWatcher);
        this.getMap().observeEvent('mouseout', this.mouseOutWatcher);
    },
    
    disable: function() {
        this.getMap().stopObserveEvent('mousemove', this.mouseMoveWatcher);
        this.getMap().stopObserveEvent('mouseout', this.mouseOutWatcher);
    },
    
    mouseOut: function(e) {
        this.domSpan.innerHTML = this.emptyText;
    },
    
    mouseMove: function(e) {
        var map = this.getMap();
        var p = map.getEventPosition(e);
        if (this.units != Fusion.PIXELS) {
            p = map.pixToGeo(p.x, p.y);
            if (p) {
                if (this.units != Fusion.UNKNOWN) {
                    var convFactor = map.getMetersPerUnit();
                    p.x = Fusion.fromMeter(this.units, p.x * convFactor);
                    p.y = Fusion.fromMeter(this.units, p.y * convFactor);
                }
                if (this.precision >= 0) {
                    var factor = Math.pow(10,this.precision);
                    p.x = Math.round(p.x * factor)/factor;
                    p.y = Math.round(p.y * factor)/factor;
                }
            }
        }
        if (p) {
            var unitAbbr = Fusion.unitAbbr(this.units);
            this.domSpan.innerHTML = this.template.replace('{x}',p.x).replace('{y}',p.y).replace('{units}', unitAbbr).replace('{units}', unitAbbr);
        }
    },

    setUnits: function() {
      if (this.units == Fusion.UNKNOWN) {
        this.setParameter('Units',this.getMap().getUnits());
      }
    },

    setParameter: function(param, value) {
        if (param == 'Units') {
            this.units = Fusion.unitFromName(value);
        }
    }
};
