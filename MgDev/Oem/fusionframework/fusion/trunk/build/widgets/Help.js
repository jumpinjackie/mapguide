/**
 * Fusion.Widget.Help
 *
 * $Id: $
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
 * Class: Fusion.Widget.Help
 *
 * Display a user help page.
 * 
 * **********************************************************************/

Fusion.Widget.Help = Class.create();
Fusion.Widget.Help.prototype = {
    /* popup window initialization parameters */
    sFeatures : 'menubar=no,location=no,resizable=no,status=no',

    /* the frame or window name to target.  If set to the Name of a
     * task pane widget, then it will appear in the task pane
     */
    target: 'HelpWindow',
    
    /* the url to open.  If specified, it is relative to the
     * application, not fusion
     */
    baseUrl: null,
    
    initialize : function(widgetTag) {
        //console.log('Help.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        var json = widgetTag.extension;
        this.target = json.Target ? json.Target[0] : "HelpWindow";
        this.baseUrl = json.Url ? json.Url[0] : Fusion.getFusionURL() + widgetTag.location + '/Help/Help.html';

        /* this widget is always enabled unless it was explicitly disabled
         * in the widget tag
         */
        if (!widgetTag.Disabled || widgetTag.Disabled[0].toLowerCase() != 'true') {
            this.enable();                   
        }
    },
    
    execute : function() {
        var url = this.baseUrl;
        /* check to see if this is going into a task pane */
        var taskPaneTarget = Fusion.getWidgetById(this.target);
        if ( taskPaneTarget ) {
            taskPaneTarget.setContent(url);
        } else {
            /* check to see if it is going into a frame in the page */
            var pageElement = $(this.target);
            if ( pageElement ) {
                pageElement.src = url;
            } else {
                /* open in a window */
                window.open(url, this.target, this.sWinFeatures);
            }
        }
    }
};
