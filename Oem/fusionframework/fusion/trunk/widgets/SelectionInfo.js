/**
 * Fusion.Widget.SelectionInfo
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
 * Class: Fusion.Widget.SelectionInfo
 *
 * Displays the number of features and number of layers in the current
 * selection.
 *
 * Template (string, optional) 
 *
 * The format of the output string.  Use {layers} and {features} as 
 * placeholders for the number of layers and features in the current
 * selection.
 *
 * You can embed HTML in the template, but you must escape any characters
 * that result in illegal HTML.  This would include:
 *
 * < is &lt;
 * > is &gt;
 * & is &amp;
 * **********************************************************************/

Fusion.Widget.SelectionInfo = Class.create();
Fusion.Widget.SelectionInfo.prototype = {
    defaultTemplate: 'selectionInfo',
    domSpan: null,
    
    initialize : function(widgetTag) {
        //console.log('SelectionInfo.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
                
        
        var json = widgetTag.extension;
        
        this.emptyText = json.EmptyText ? json.EmptyText[0] : this.domObj.innerHTML;
        this.template = json.Template ? json.Template[0] : this.defaultTemplate;
        
        this.domSpan = document.createElement('span');
        this.domSpan.className = 'spanSelectionInfo';
        this.domSpan.innerHTML = OpenLayers.String.translate(this.emptyText);
        this.domObj.innerHTML = '';
        this.domObj.appendChild(this.domSpan);

        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.update.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.update.bind(this));
    },
    
    update: function() {
        var olMap = this.getMap();
        var aMaps = olMap.getAllMaps();
        var map = aMaps[0];
        if (map.hasSelection()) {
            var layers = map.getSelectedLayers();
            var nLayers = layers.length;
            var nFeatures = map.getSelectedFeatureCount();
            this.domSpan.innerHTML = OpenLayers.String.translate(this.template,nFeatures,nLayers);
        } else {
            this.domSpan.innerHTML = OpenLayers.String.translate(this.emptyText);
        }
    }
};
