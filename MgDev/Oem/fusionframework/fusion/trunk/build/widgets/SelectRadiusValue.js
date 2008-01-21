/**
 * Fusion.Widget.SelectRadiusValue
 *
 * $Id: SelectRadiusValue.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.SelectRadiusValue
 *
 * A widget to allow the user to specify the radius to use for a 
 * SelectRadius widget.
 *
 * **********************************************************************/

Fusion.Widget.SelectRadiusValue = Class.create();
Fusion.Widget.SelectRadiusValue.prototype =  {
    radiusWidgetName: null,
    label: '',
    className: '',
    domLabel: null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        /* parse widget properties */
        var json = widgetTag.extension;
        
        this.radiusWidgetName = json.RadiusName ? json.RadiusName[0] : null;
        this.label = json.Label ? json.Label[0] : '';
        this.className = json.ClassName ? json.ClassName[0] : '';
        
        /* a container for the widget */
        //this.domObj = document.createElement('div');
        //this.domObj.className = this.className;
        
        /* put in the label */
        this.domLabel = document.createElement('label');
        this.domLabel.className = this.className;
        this.domLabel.innerHTML = this.label;
        //this.domObj.appendChild(label);
        
        /* put in the input */
        this.input = document.createElement('input');
        this.input.type = 'text';
        this.domLabel.appendChild(this.input);
        
        /* put into page */
        this.domObj.appendChild(this.domLabel);
        Event.observe(this.input, 'blur', this.onBlur.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoaded.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.mapExtentsChanged.bind(this));
        
    },
    
    mapLoaded: function() {
        var widgets = Fusion.getWidgetsByType('SelectRadius');
        for (var i=0; i<widgets.length; i++) {
            if (widgets[i].sName == this.radiusWidgetName) {
                this.widget = widgets[i];
            }
        }
        this.updateFromWidgetValue();
    },
    
    mapExtentsChanged: function() {
        this.updateWidgetValue();
    },
    
    onBlur: function() {
        this.updateWidgetValue();
    },
    
    updateWidgetValue: function() {
        if (this.widget) {
            var radius = this.getMap().geoToPixMeasure(this.input.getValue());
            this.widget.setRadius(radius);
        }
    },
    
    updateFromWidgetValue: function() {
        if (this.widget) {
            this.input.value = this.getMap().pixToGeoMeasure(this.widget.getRadius());
        }
    }
};