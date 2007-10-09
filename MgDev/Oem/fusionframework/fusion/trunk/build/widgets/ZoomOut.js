/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ZoomOut.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose ZoomOut
 * @author yassefa@dmsolutions.ca
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
 * extended description
 * **********************************************************************/

Fusion.Widget.ZoomOut = Class.create();
Fusion.Widget.ZoomOut.prototype = 
{
    nFactor: -2,
    initialize : function(widgetTag)
    {
        //console.log('ZoomOut.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.Click.prototype, []);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        this.asCursor = ["url('images/zoomout.cur'),auto",'-moz-zoom-out', 'auto'];
    },

    activateTool : function()
    {
        //console.log('ZoomOut.activateTool');
        this.getMap().activateWidget(this);
    },

    activate : function()
    {
        //console.log('ZoomOut.activate');
        this.activateClickTool();
        this.getMap().setCursor(this.asCursor);
        /*button*/
        this._oButton.activateTool()
    },

    deactivate : function()
    {
        //console.log('ZoomOut.deactivate');
        
         this.deactivateClickTool();
         this.getMap().setCursor('auto');

          /*icon button*/
         this._oButton.deactivateTool();
    },

    execute : function(nX, nY)
    {
        var sGeoPoint = this.getMap().pixToGeo(nX,nY);
        this.getMap().zoom(sGeoPoint.x, sGeoPoint.y, this.nFactor);
    },

    setParameter : function(param, value)
    {
        if (param == "Factor" && value < 0)
        {
            this.nFactor = value;
        }
    }
};
