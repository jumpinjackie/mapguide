/**
 * Fusion.Widget.SaveMap
 *
 * $Id: SaveMap.js 1169 2008-01-10 16:21:12Z pspencer $
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
 * Class: Fusion.Widget.SaveMap
 *
 * Save the current map image on the client's computer
 *
 * usage:
 * DWF format support requires a structure like this in the application
 * definition's widget tag extension:
 *    <Extension>
 *      <Format></Format>
 *      <ResourceId></ResourceId>
 *      <Scale></Scale>
 *    </Extension>
 * **********************************************************************/

Fusion.Widget.SaveMap = Class.create();
Fusion.Widget.SaveMap.prototype = {
    iframe : null,
    printLayout : null,
    printScale : null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);
        Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);

        var json = widgetTag.extension;
        this.format = (json.Format && json.Format[0] != '')?
                       json.Format[0] : 'png';
        
        //for DWF, parse printLayouts and build menu
        if (this.format == 'DWF') {
            if (json.ResourceId) {
                this.printLayout = json.ResourceId[0];
                if (json.Scale) {
                    this.printScale =  json.Scale[0];
                }
            } else {
                //TODO: Warning that the widget is improperly configured
                //because we need  print layout for this to work.
                //TODO: deactivate the widget?
            }
        }

        this.enable = Fusion.Widget.SaveMap.prototype.enable;
    },
    
    enable: function() {
        Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
    },
    
    /**
     * called when the button is clicked by the Fusion.Tool.ButtonBase widget
     * prompts user to save the map.
     */
    activateTool : function() {
        if (!this.iframe) {
            this.iframe = document.createElement('iframe');
            this.iframe.id = 'w';
            this.iframe.style.visibility = 'hidden';
            document.body.appendChild(this.iframe);
        }
        var szLayout = '';
        var szScale = '';
        if (this.format === 'DWF') {
            if (this.printLayout) {
                szLayout = '&layout=' + this.printLayout;                
            } else {
                //TODO: issue an error?
                return;
            }
            if (this.printScale) {
                szScale = '&scale=' + this.printScale;
            }
        }
        //TODO: revisit Fusion.getWebAgentURL
		var m = this.getMap().aMaps[0];
        if(navigator.appVersion.match(/\bMSIE\b/)) {
            //var url = Fusion.getWebAgentURL() + "OPERATION=GETDYNAMICMAPOVERLAYIMAGE&FORMAT=PNG&VERSION=1.0.0&SESSION=" + this.getMap().getSessionID() + "&MAPNAME=" + this.getMap().getMapName() + "&SEQ=" + Math.random();
            
            var url = Fusion.fusionURL + '/' + m.arch + '/' + Fusion.getScriptLanguage() + "/SaveMapFrame." + Fusion.getScriptLanguage() + '?session='+m.getSessionID() + '&mapname=' + m.getMapName() + '&format=' + this.format + szLayout;
            //this.iframe.src = url;
            w = open(url, "Save", 'menubar=no,height=200,width=300');
        } else {
            var s = Fusion.fusionURL + '/' + m.arch + '/' + Fusion.getScriptLanguage() + "/SaveMap." + Fusion.getScriptLanguage() + '?session='+m.getSessionID() + '&mapname=' + m.getMapName() + '&format=' + this.format + szLayout;
            //console.log(s);
            
            this.iframe.src = s;
        }
    }
};
