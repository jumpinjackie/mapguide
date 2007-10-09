/*****************************************************************************
 * $Id: $
 *
 * Controls visibility of a DOM element based on the busy state of the map
 *
 * Copyright DM Solutions Group 2007 All Rights Reserved
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
 *****************************************************************************
 *
 * AcitivityIndicator is a widget that shows or hides its DOM element
 * based on whether the map widget is busy or not.  The map widget
 * becomes busy when loading maps, reloading maps or redrawing
 * layers.
 *
 * **************************************************************************/

Fusion.Widget.ActivityIndicator = Class.create();
Fusion.Widget.ActivityIndicator.prototype =  {
    element: null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        this.element = this.domObj;
        var json = widgetTag.extension;
        if (json.ElementId) {
            var elm = $(json.ElementId[0]);
            if (elm && elm != this.domObj) {
                this.domObj.appendChild(elm);
                this.element = elm;
            }
        }
        this.element.style.visibility = 'hidden';
        this.getMap().registerForEvent(Fusion.Event.MAP_BUSY_CHANGED, this.busyChanged.bind(this));
    },
    busyChanged: function() {
        this.element.style.visibility = this.getMap().isBusy() ? 'visible' : 'hidden';
    }
};
