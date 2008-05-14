/**
 * Fusion.Widget.SaveMap
 *
 * $Id: SaveMap.js 1396 2008-05-08 15:34:30Z madair $
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
    imageWidth : null,
    imageHeight : null,
    initialize : function(widgetTag) {
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, false]);

        var json = widgetTag.extension;
        this.format = (json.Format && json.Format[0] != '')?
                       json.Format[0] : 'png';
        
        //for DWF, parse printLayouts and build menu
        if (this.format == 'DWF' && json.PrintLayout.length) {
            Object.inheritFrom(this, Fusion.Tool.MenuBase.prototype, []);
            
            var layouts = json.PrintLayout;
            for (var i = 0; i < layouts.length; i++) {
                var layout = layouts[i];
                var opt = {};
                opt.label = layout.Name[0];
                var data = {rid:layout.ResourceId[0]};
                if (layout.PageHeight) {
                    data.pageHeight = layout.PageHeight[0];
                };
                if (layout.PageWidth) {
                    data.pageWidth = layout.PageWidth[0];
                };
                if (layout.Margins) {
                    data.margins = [layout.Margins[0].Top[0],
                                    layout.Margins[0].Left[0],
                                    layout.Margins[0].Right[0],
                                    layout.Margins[0].Bottom[0]];
                };
                var menuItem = null;
                if (layout.Scale) {
                    //create entries for weblayout specified scales
                    menuItem = new Jx.SubMenu(opt);
                    for (var j=0; j < layout.Scale.length; j++) {
                        data.scale = layout.Scale[j];
                        var scaleAction = new Jx.Action(this.setLayout.bind(this, data));
                        var subMenuItem = new Jx.MenuItem(scaleAction,{label:data.scale});
                        menuItem.add(subMenuItem);
                    }
                    //add an entry for current scale
                    var currentScaleAction = new Jx.Action(this.setLayout.bind(this, data));
                    var currentScaleItem = new Jx.MenuItem(currentScaleAction,
                                                         {label:'Current Scale'});
                    menuItem.add(currentScaleItem);
                } else {
                    //if there are no scales, the layout is used with current scale
                    var action = new Jx.Action(this.setLayout.bind(this, data));
                    menuItem = new Jx.MenuItem(action,opt);
                };
                this.oMenu.add(menuItem);
            }
        } else {
            Object.inheritFrom(this, Fusion.Tool.ButtonBase.prototype, []);
            if (json.Width && json.Width[0] != '') {
                this.imageWidth = json.Width[0];
            }
            if (json.Height && json.Height[0] != '') {
                this.imageHeight = json.Height[0];
            }
        }

        this.enable = Fusion.Widget.SaveMap.prototype.enable;
    },
    
    enable: function() {
        Fusion.Tool.ButtonBase.prototype.enable.apply(this, []);
    },
    
    setLayout: function(data) {
        this.printScale = data.scale;
        this.printLayout = data.rid;
        this.pageHeight = data.pageHeight;
        this.pageWidth = data.pageWidth;
        this.pageMargins = data.margins;

        this.activateTool();
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
        var szPageHeight = '';
        var szPageWidth = '';
        var szPageMargins = '';
        if (this.format === 'DWF') {
            if (this.printLayout) {
                szLayout = '&layout=' + this.printLayout;                
            } else {
                alert('DWF Save is not properly configured.');
                return;
            }
            if (this.printScale) {
                szScale = '&scale=' + this.printScale;
            }            
            if (this.pageHeight) {
                szPageHeight = '&pageheight=' + this.pageHeight;
            }
            if (this.pageWidth) {
                szPageWidth = '&pagewidth=' + this.pageWidth;
            }
            if (this.pageMargins) {
                szPageMargins = '&margins=' + this.pageMargins.join(',');
            }
        }
        var szHeight = '';
        if (this.imageHeight) {
            szHeight = '&height=' + this.imageHeight;
        }
        var szWidth = '';
        if (this.imageWidth) {
            szWidth = '&width=' + this.imageWidth;
        }
        var m = this.getMap().aMaps[0];
        if(navigator.appVersion.match(/\bMSIE\b/)) {
            var url = Fusion.fusionURL + '/' + m.arch + '/' + Fusion.getScriptLanguage() + "/SaveMapFrame." + Fusion.getScriptLanguage() + '?session='+m.getSessionID() + '&mapname=' + m.getMapName() + '&format=' + this.format + szLayout + szWidth + szHeight + szPageHeight + szPageWidth + szPageMargins;
            w = open(url, "Save", 'menubar=no,height=200,width=300');
        } else {
            var s = Fusion.fusionURL + '/' + m.arch + '/' + Fusion.getScriptLanguage() + "/SaveMap." + Fusion.getScriptLanguage() + '?session='+m.getSessionID() + '&mapname=' + m.getMapName() + '&format=' + this.format + szLayout + szWidth + szHeight + szPageHeight + szPageWidth + szPageMargins;
            
            this.iframe.src = s;
        }
    }
};
