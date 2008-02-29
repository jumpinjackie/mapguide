/**
 * Fusion.Widget.MapMenu
 *
 * $Id: MapMenu.js 970 2007-10-16 20:09:08Z madair $
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
 * Class: Fusion.Widget.MapMenu
 *
 * A widget that displays a selection of maps that can be loaded into the 
 * application.  The list of maps is configured in the ApplicationDefinition.
 * **********************************************************************/


Fusion.Widget.MapMenu = Class.create();
Fusion.Widget.MapMenu.prototype = 
{
    domObj: null,
    oMenu: null,
    sRootFolder: '',
    aMenus : null,
    initialize : function(widgetTag)
    {
        //console.log('MapMenu.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.MenuBase.prototype, []);
        this.enable();
        
        var json = widgetTag.extension;
        var mapGroups = Fusion.applicationDefinition.mapGroups;
        for (var i=0; i<mapGroups.length; i++) {
            var mapGroup = mapGroups[i];
            var opt = {};
            opt.label = mapGroup.mapId;
            var data = mapGroup;
            var action = new Jx.Action(this.switchMap.bind(this, data));
            var menuItem = new Jx.MenuItem(action,opt);
            this.oMenu.add(menuItem);
        }
    },
    
    switchMap: function(data) {
        this.getMap().loadMapGroup(data);
    }
};