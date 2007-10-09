/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ZoomOnClick.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Zoom the map by a fixed amount when a button is clicked
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
 * Zoom the map by a fixed amount when a button is clicked
 * 
 * **********************************************************************/

Fusion.Widget.ZoomOnClick = Class.create();
Fusion.Widget.ZoomOnClick.prototype = 
{
    nFactor: 2,
    initialize : function(widgetTag)
    {
        //console.log('ZoomOnClick.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
                
        var json = widgetTag.extension;
        this.nFactor = parseFloat(json.Factor ? json.Factor[0] : this.nFactor);
    },

    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     */
    execute : function()
    {
        //console.log('ZoomOnClick.activateTool');
        var center = this.getMap().getCurrentCenter();
        this.getMap().zoom(center.x, center.y, this.nFactor);
    },

    setParameter : function(param, value)
    {
        if (param == "Factor" && value > 0)
        {
            this.nFactor = value;
        }
    }
};
