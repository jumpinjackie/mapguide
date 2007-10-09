/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ZoomSlider.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Continuous zoom between two scales using a graphical interface
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
 * Continuous zoom between two scales using a graphical interface
 *
 * This uses the Scriptaculous Control.Slider control.  You need two <div>
 * elements in the page, one that is the 'track' and the other that is the
 * 'handle' of the slider.
 * 
 * **********************************************************************/

Fusion.Widget.ZoomSlider = Class.create();
Fusion.Widget.ZoomSlider.prototype = 
{
    bUseMapScale: false,

    initialize : function(widgetTag)
    {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        
        var json = widgetTag.extension;
        
        this._oTrack = json.Track ? json.Track[0] : '';
        this._oHandle = json.Handle ? json.Handle[0] : '';
        var direction = json.Direction ? json.Direction[0] : 'horizontal';
        
        this.fMinScale = json.MinScale ? parseFloat(json.MinScale[0]) : 0;
        this.fMaxScale = json.MaxScale ? parseFloat(json.MaxScale[0]) : 1;
        if (this.fMaxScale == 1) {
            this.bUseMapScale = true;
        }
        var options = {};
        options.axis = direction;
        options.range = $R(this.fMinScale, this.fMaxScale);
        options.sliderValue = this.clipScale(this.getMap().getScale());
        options.onChange = this.scaleChanged.bind(this);
        this._oSlider = new Control.Slider(this._oHandle, this._oTrack, options);
        
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.mapExtentsChanged.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoaded.bind(this));
        
    },
    
    mapLoaded: function() {
        /* rough scale approximation -  */
        var scale = this.getMap()._fScale;
        //TODO I tried to expand the range by 20% but it causes problems
        if (this.bUseMapScale) {
            this.fMaxScale = scale;
            this._oSlider.range = $R(this.fMinScale, this.fMaxScale);
        }
        this._oSlider.setValue(scale);
    },
    
    clipScale: function(scale) {
        return Math.min(this.fMaxScale, Math.max(this.fMinScale, scale));
    },
    
    scaleChanged: function(value) {
        if (this.getMap().getScale() != value) {
            this.getMap().zoomScale(value);
        }
    },
    
    mapExtentsChanged: function() {
        var scale = this.clipScale(this.getMap().getScale());
        if (scale != this._oSlider.value) {
            this._oSlider.setValue(scale);
        }
    }
};
