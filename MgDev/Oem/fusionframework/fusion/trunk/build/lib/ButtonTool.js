/**
 * Fusion.Tool.Button
 *
 * $Id: ButtonTool.js 970 2007-10-16 20:09:08Z madair $
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
 
/********************************************
 * Class: Fusion.Tool.Button
 *
 * Utility base class for the actual buttons used by widgets.
 * **********************************************************************/

Fusion.Tool.Button = Class.create();
Fusion.Tool.Button.prototype = {
    bActive: null,
    sActiveClass: 'jxButtonActive',
    sDisabledClass: 'jxButtonDisabled',
  
    initialize : function(domObj, widgetTag)  {
        //console.log('Fusion.Tool.Button.initialize');
        var json = widgetTag.extension;
        
        this.buttonAction = new Jx.Action(this.buttonClicked.bind(this));
        if (domObj) {
            var options = {};
            options.imgPath = widgetTag.imageUrl;
            options.imgClass = widgetTag.imageClass;
            options.tooltip = widgetTag.tooltip;
            options.label = widgetTag.label;
            this._oButton = new Jx.Button(this.buttonAction, options);
            domObj.appendChild(this._oButton.domObj);
        }
        this.bActive = false;
        this.bEnabled = true;
        
        if (widgetTag.disabled) {
            this.disableTool();
        }
    },
    
    buttonClicked: function() {
        
    },

    observeEvent : function(sEvent, fnCB) {
        if (this._oButton) {
             Event.observe(this._oButton.domObj, sEvent, fnCB);
        }
    },
    
    stopObserveEvent : function(sEvent, fnCB) {
        if (this._oButton) {
             Event.stopObserving(this._oButton.domObj, sEvent, fnCB);
        }
    },
    
    enableTool : function() {
        this.buttonAction.setEnabled(true);
    },

    disableTool : function() {
        this.buttonAction.setEnabled(false);
    },

    activateTool : function() {
        //console.log('Fusion.Tool.Button.activateTool');
        this.bActive = true;
        if (!this._oButton) {
            return;
        }
        if (this._sImageURL != null && this._sImageURL.length > 0) {
            this._oButton.setImage(this._sImageURL);
        }
        if (this.sActiveClass != '') {
            Element.addClassName(this._oButton.domA, this.sActiveClass);
        }
    },

    deactivateTool :  function() {
        //console.log('Fusion.Tool.Button.deactivateTool');
        this.bActive = false;
        if (!this._oButton) {
            return;
        }
        if (this.sActiveClass != '') {
            Element.removeClassName(this._oButton.domA, this.sActiveClass);
        }
    },

    clickCB : function(e) { this.activateTool(); },
    isActive: function() {return this.bActive;}
};
