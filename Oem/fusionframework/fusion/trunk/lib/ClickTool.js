/**
 * Fusion.Tool.Click
 *
 * $Id: ClickTool.js 970 2007-10-16 20:09:08Z madair $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

 /****************************************************************************
 * Class: Fusion.Tool.Click
 *
 * Utility class to manage a click on the map (mouse up event).
 *
 * All classes using this should redefine the execute function
 * **********************************************************************/

Fusion.Tool.Click = Class.create();
Fusion.Tool.Click.prototype =
{
    /**
     * constructor
     * @param oMap {Object} a map widget
     */
    initialize : function()
    {
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
    },

    execute : function(x,y)
    {
    },

    activateClickTool : function()
    {
        //console.log('Fusion.Tool.Click.activateClickTool');
        if (this.oMap) {
            this.oMap.observeEvent('mouseup', this.mouseUpCB);
        }
    },

    deactivateClickTool : function()
    {
        //console.log('Fusion.Tool.Click.deactivateClickTool');
        if (this.oMap) {
            this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        }
    },


    mouseUp : function(e)
    {
        //console.log('Fusion.Tool.Click.mouseUp');
        if (OpenLayers.Event.isLeftClick(e)) {
            var sPixPoint = this.oMap.getEventPosition(e);
            this.execute(sPixPoint.x, sPixPoint.y);
        }
    }
};
