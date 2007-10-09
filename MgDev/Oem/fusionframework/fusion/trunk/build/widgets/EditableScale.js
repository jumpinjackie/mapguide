/********************************************************************** * 
 * $Id: $
 * 
 * Shows current scale, and allows the user to enter a new scale
 *
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
 * The user can manually type in a new scale
 *
 * **********************************************************************/

Fusion.Widget.EditableScale = Class.create();
Fusion.Widget.EditableScale.prototype = {
    precision: 4,
    
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        var json = widgetTag.extension;
                
        this.domScale = document.createElement('input');
        this.domScale.className = 'inputEditableScale';
        this.domObj.appendChild(this.domScale);
        Event.observe(this.domScale, 'keypress', this.keyPressHandler.bindAsEventListener(this));
        this.precision = json.Precision ? parseInt(json.Precision[0]) : this.precision;
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.scaleChanged.bind(this));
        
        Fusion.addWidgetStyleSheet(widgetTag.location + '/EditableScale/EditableScale.css');
        
        
    },
    
    scaleChanged: function() {
        this.domScale.value = this.scaleToString(this.getMap().oMapOL.getScale());
    },
    
    scaleToString: function(scale) {
        scale = Math.abs(parseFloat(scale));
        return "" + Math.round(scale * Math.pow(10,this.precision))/Math.pow(10,this.precision);
    },
    
    keyPressHandler: function(e) {
        if (e.keyCode == Event.KEY_RETURN) {
            this.zoomToScale();
        }
    },
    
    zoomToScale: function(e) {
        var scale = parseFloat(this.domScale.value);
        if (scale) {
            this.getMap().zoomToScale(scale);
        }
    }
};