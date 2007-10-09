/********************************************************************** * 
 * @project Fusion
 * @revision $Id: FeatureCanvas.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose FeatureCanvas widget
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
 * 
 * 
 * **********************************************************************/



Fusion.Widget.FeatureCanvas = Class.create();
Fusion.Widget.FeatureCanvas.prototype = {
    features: null,
    initialize : function(widgetTag) {
        //console.log('FeatureCanvas.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.Canvas.prototype, []);
        var json = widgetTag.extension;
        this.activateCanvas();
        this.features = [];
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.update.bind(this));
    },
    
    newPoint: function(x,y) {
        var p = new Fusion.Tool.Canvas.Point(this.getMap());
        p.setPoint(x,y);
        this.features.push(p);
        this.draw();
        return p;
    },
    
    newLine: function(points) {
        
    },
    
    newPolygon: function(points) {
        
    },
    
    newCircle: function(x, y, r) {
        var p = new Fusion.Tool.Canvas.Circle(this.getMap());
        p.setCenter(x,y);
        p.setRadius(r);
        this.features.push(p);
        this.draw();
        return p;
    },
    
    update: function() {
        this.clearContext();
        for (var i=0; i<this.features.length; i++) {
            this.features[i].updatePx();
            this.features[i].draw(this.context);
        }
    },
    
    draw: function() {
        this.clearContext();
        for (var i=0; i<this.features.length; i++) {
            this.features[i].draw(this.context);
        }
    },
    
    resizeCanvas: function() {
        Fusion.Tool.Canvas.prototype.resizeCanvas.apply(this, []);
        this.update();
    }
};
