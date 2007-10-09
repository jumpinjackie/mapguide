/********************************************************************** * 
 * @project Fusion
 * @revision $Id: PanOnClick.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose pan the map a fixed amount in a particular direction
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
 * pan the map a fixed amount in a particular direction
 * 
 * **********************************************************************/

Fusion.Widget.PanOnClick = Class.create();
Fusion.Widget.PanOnClick.prototype = 
{
    fPercent: null,
    nDeltaX: null,
    nDeltaY: null,
    initialize : function(widgetTag)
    {
        //console.log('FitToWindow.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        
        var json = widgetTag.extension;
        
        var percent = json.Percentage ? json.Percentage[0] : 75;
        this.fPercent = parseFloat(percent)/100;
        
        var direction = json.Direction ? json.Direction[0] : '';
        switch (direction) {
            case 'north':
                this.nDeltaX = 0;
                this.nDeltaY = 1;
                break;
            case 'south':
                this.nDeltaX = 0;
                this.nDeltaY = -1;
                break;
            case 'east':
                this.nDeltaX = 1;
                this.nDeltaY = 0;
                break;
            case 'west':
                this.nDeltaX = -1;
                this.nDeltaY = 0;
                break;
            default:
                this.nDeltaX = 0;
                this.nDeltaY = 0;
        }
        
    },

    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     */
    execute : function()
    {
        var extents = this.getMap().getCurrentExtents();
        var center = this.getMap().getCurrentCenter();
        var fX, fY;
        fX = center.x + this.nDeltaX * (extents[2] - extents[0]) * this.fPercent;
        fY = center.y + this.nDeltaY * (extents[3] - extents[1]) * this.fPercent;
        this.getMap().zoom(fX, fY, 1);
    }
};