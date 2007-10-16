/********************************************************************** * 
 * $Id: $
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
    defaultTemplate: '{features} features selected on {layers} layers',
    domSpan: null,
    
    initialize : function(widgetTag) {
        //console.log('SelectionInfo.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
                
        this.emptyText = this.domObj.innerHTML;
        
        var json = widgetTag.extension;
        
        this.template = json.Template ? json.Template[0] : this.defaultTemplate;
        
        this.domSpan = document.createElement('span');
        this.domSpan.className = 'spanSelectionInfo';
        this.domSpan.innerHTML = this.emptyText;
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
            this.domSpan.innerHTML = this.template.replace('{layers}',nLayers).replace('{features}',nFeatures);
        } else {
            this.domSpan.innerHTML = this.emptyText;
        }
    }
};
