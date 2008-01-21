/**
 * Fusion.Widget.ExtentHistory
 *
 * $Id: ExtentHistory.js 970 2007-10-16 20:09:08Z madair $
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

 /********************************************************************
 * Class: Fusion.Widget.ExtentHistory
 *
 * Maintain and navigate through a history of extents
 * 
 * **********************************************************************/

Fusion.Event.HISTORY_CHANGED = Fusion.Event.lastEventId++;

Fusion.Widget.ExtentHistory = Class.create();
Fusion.Widget.ExtentHistory.prototype = {
    events: [],
    aHistory: [],
    sDirection: null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
        
        var json = widgetTag.extension;
        var sDirection = json.Direction ? json.Direction[0].toLowerCase() : 'previous';
        if (sDirection != 'previous' && sDirection != 'next') {
            this.sDirection = 'previous';
        } else {
            this.sDirection = sDirection;
        }
        
        if (!this.aHistory['history']) {
            this.aHistory['history'] = [];
            this.aHistory['index'] = -1;
            this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.extentsChanged.bind(this));
            this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.reset.bind(this));
            
        }
        this.enable = Fusion.Widget.ExtentHistory.prototype.historyChanged;
        
        this.disable = Fusion.Widget.ExtentHistory.prototype.historyChanged;
        
        this.registerEventID(Fusion.Event.HISTORY_CHANGED);
        
        this.registerForEvent(Fusion.Event.HISTORY_CHANGED, this.historyChanged.bind(this));
        //console.log(this.events[Fusion.Event.HISTORY_CHANGED].length);
        this.disable();
    },
    
    reset: function() {
        if (this.getMap().isMapLoaded()) {
            this.aHistory['history'] = [this.getMap().getCurrentExtents()];
            this.aHistory['index'] = 0;
        } else {
            this.aHistory['history'] = [];
            this.aHistory['index'] = -1;
        }
        this.historyChanged();
    },
    
    extentsChanged: function() {
        var extents = this.getMap().getCurrentExtents();
        if (this.aHistory['history'].length == 0) {
            this.aHistory['history'].push(extents);
            this.aHistory['index'] = 0;
        } else {
            var aExtents = this.aHistory['history'][this.aHistory['index']];
            if (aExtents.top == extents.top &&
                aExtents.bottom == extents.bottom &&
                aExtents.left == extents.left &&
                aExtents.right == extents.right) {
                return;
            }
            //clear forward history if we have gone backwards at some point
            if (this.aHistory['index'] != (this.aHistory['history'].length - 1)) {
                this.aHistory['history'] = this.aHistory['history'].slice(0, this.aHistory['index'] + 1);
            }
            this.aHistory['history'].push(extents);
            this.aHistory['index'] = this.aHistory['history'].length - 1;
        }
        this.triggerEvent(Fusion.Event.HISTORY_CHANGED);
    },
    
    historyChanged: function() {
        if (this.sDirection == 'previous') {
            if (this.aHistory['index'] > 0) {
                Fusion.Tool.ButtonBase.prototype.enable.apply(this,[]);
            } else {
                Fusion.Tool.ButtonBase.prototype.disable.apply(this,[]);
            }
        } else {
            if (this.aHistory['index'] < (this.aHistory['history'].length - 1)) {
                Fusion.Tool.ButtonBase.prototype.enable.apply(this,[]);
            } else {
                Fusion.Tool.ButtonBase.prototype.disable.apply(this,[]);
            }
        }
    },

    execute: function() {
        if (this.sDirection == 'previous') {
            if (this.aHistory['index'] > 0) {
                this.aHistory['index'] --;
                this.getMap().setExtents(this.aHistory['history'][this.aHistory['index']]);
                this.triggerEvent(Fusion.Event.HISTORY_CHANGED);
            }
        } else {
            if (this.aHistory['index'] < (this.aHistory['history'].length - 1)) {
                this.aHistory['index'] ++;
                this.getMap().setExtents(this.aHistory['history'][this.aHistory['index']]);
                this.triggerEvent(Fusion.Event.HISTORY_CHANGED);
            }
        }
    }
};