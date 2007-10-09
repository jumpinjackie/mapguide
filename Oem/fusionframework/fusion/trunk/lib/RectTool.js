/********************************************************************** * 
 * @project Fusion
 * @revision $Id: RectTool.js 829 2007-09-19 14:45:50Z pspencer $
 * @purpose Utility class to do a rect on the map
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
 * Utility class to draw a rectangle on the map and return the coordinates
 * through the doAction function.
 * All classes should redifine the doAction function
 * **********************************************************************/
Fusion.Tool.Rectangle = Class.create();
Fusion.Tool.Rectangle.prototype = {
    oRectDiv : null,
    _sStartPos : null,
    _bIsActive : null,

    /**
     * constructor
     * @param oMap {Object} a map widget
     */
    initialize : function() {
        //console.log('Fusion.Tool.Rectangle.initialize');
        this.oRectDiv = document.createElement('div');
        this.oRectDiv.style.position = 'absolute';
        this.oRectDiv.style.border = '1px solid red';
        this.oRectDiv.style.top = '0px';
        this.oRectDiv.style.left = '0px';
        this.oRectDiv.style.width = '1px';
        this.oRectDiv.style.height = '1px';
        this.oRectDiv.style.visibility = 'hidden';
        this.oRectDiv.style.lineHeight = '1px'; //for IE
        this.oRectDiv.style.zIndex = 99;

        this._bIsActive = false;

        this.mouseMoveCB = this.mouseMove.bindAsEventListener(this);
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
        this.mouseDownCB = this.mouseDown.bindAsEventListener(this);
        this.mouseOutCB = this.mouseOut.bindAsEventListener(this);
    },

    execute : function(l,b,r,t) {},

    activateRectTool : function() {
        //console.log('Fusion.Tool.Rectangle.activateRectTool');
        if (this._bIsActive) {
            //console.log('tool is already active');
            return;
        }
        if (this.oMap) {
            this.oRectDiv.style.left = '0px';
            this.oRectDiv.style.top = '0px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'hidden';
            this._bIsActive = true;
            var oDomElem =  this.oMap.getDomObj();
            oDomElem.appendChild(this.oRectDiv);
            this.oMap.observeEvent('mousemove', this.mouseMoveCB);
            this.oMap.observeEvent('mouseup', this.mouseUpCB);
            this.oMap.observeEvent('mousedown', this.mouseDownCB);
        }
    },

    deactivateRectTool : function() {
        this._sStartPos = null;
        //console.log('Fusion.Tool.Rectangle.deactivateRectTool');
        if (!this._bIsActive) {
            //console.log('tool is not active');
            return;
        }
        this._bIsActive = false;
        var oDomElem =  this.oMap.getDomObj();
        oDomElem.removeChild(this.oRectDiv);
        this.oMap.stopObserveEvent('mousemove', this.mouseMoveCB);
        this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        this.oMap.stopObserveEvent('mousedown', this.mouseDownCB);
    },

    mouseDown : function (e) {
        if (OpenLayers.Event.isLeftClick(e)) {
            var p = this.oMap.getEventPosition(e);
            this._sStartPos = p;

            this.oRectDiv.style.left = p.x + 'px';
            this.oRectDiv.style.top = p.y + 'px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'visible';
            OpenLayers.Event.observe(document, 'mouseout', this.mouseOutCB);
        }
        OpenLayers.Event.stop(e);
    },

    mouseUp : function(e) {
        if (this._sStartPos != null) {
            var t = parseInt(this.oRectDiv.style.top);
            var l = parseInt(this.oRectDiv.style.left);
            var r = l + parseInt(this.oRectDiv.style.width);
            var b = t + parseInt(this.oRectDiv.style.height);
            this.event = e;
            this.execute(l,b,r,t);

            this.oRectDiv.style.left = '0px';
            this.oRectDiv.style.top = '0px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'hidden';

            this._sStartPos = null;
            OpenLayers.Event.stop(e);
        }
        OpenLayers.Event.stopObserving(document, 'mouseout', this.mouseOutCB);
    },

    mouseMove : function(e) {
        if (this._sStartPos != null)
        {
            var p = this.oMap.getEventPosition(e);
            var l = this._sStartPos.x;
            var t = this._sStartPos.y;
            var r = p.x;
            var b = p.y;

            this.oRectDiv.style.left = Math.min(l,r) + 'px';
            this.oRectDiv.style.top = Math.min(t,b) + 'px';
            this.oRectDiv.style.width = Math.abs(r-l) + 'px';
            this.oRectDiv.style.height = Math.abs(t-b) + 'px';
            OpenLayers.Event.stop(e);
        }
    },
    
    mouseOut: function(e) {
        var target = e.target || e.srcElement;
        if (target.tagName.toLowerCase() == 'html') {
            this.mouseUp(e);
        }
    }
        
};
