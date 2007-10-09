/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ScaleEntry.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Manually enter a scale or choose from some previous scales
 * @author pspencer@dmsolutions.ca
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 ********************************************************************
 *
 * The user can manually type in a new scale or can select a previous
 * scale from a drop-down list.
 *
 * **********************************************************************/

Fusion.Widget.ScaleEntry = Class.create();
Fusion.Widget.ScaleEntry.prototype = {
    precision: 4,
    historyLength: 10,
    history: null,
    
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        var json = widgetTag.extension;
        
        var c = json['Class'] ? json['Class'][0] : '';
        
        var d = document.createElement('div');
        if (c != '') {
            Element.addClassName(d, c);
        }
        
        this.picker = new Jx.Picker(d, true);
        this.picker.addSelectionListener(this);
                
        this.precision = json.Precision ? parseInt(json.Precision[0]) : this.precision;
        
        this.historyLength = json.HistoryLength ? parseInt(json.HistoryLength[0]): this.historyLength;
        
        this.history = [];
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.scaleChanged.bind(this));
        
    },
    
    scaleChanged: function() {
        this.picker.setValue(this.scaleToString(this.getMap().oMapOL.getScale()));
    },
    
    selectionChanged: function(obj) {
        var v = obj.getValue();
        bInHistory = false;
        for (var i=0; i<this.history.length; i++) {
            if (this.history[i] == v) {
                bInHistory = true;
                break;
            }
        }
        var rx = /[0-9]+(\.[0-9]*)?/;
        if (rx.test(v)) {
            v = parseFloat(this.scaleToString(v));
            if (this.getMap().getScale() != v) {
                this.getMap().zoomScale(v);
                if (!bInHistory) {
                    this.addToHistory(v);
                }
            }
        }
    },
    
    addToHistory: function(scale) {
        for (var i=0; i<this.history.length; i++) {
            if (scale > this.history[i]) {
                break;
            }
        }
        this.history.splice(i, 0, scale);
        this.picker.add(this.scaleToString(scale), i);
        if (this.historyLength && this.history.length > this.historyLength) {
            this.history.pop();
            this.picker.remove(this.historyLength);
        }
    },
    
    scaleToString: function(scale) {
        scale = Math.abs(parseFloat(scale));
        return "" + Math.round(scale * Math.pow(10,this.precision))/Math.pow(10,this.precision);
    }
};