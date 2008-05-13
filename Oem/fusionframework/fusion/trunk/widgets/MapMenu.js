/**
 * Fusion.Widget.MapMenu
 *
 * $Id: MapMenu.js 1396 2008-05-08 15:34:30Z madair $
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
    mapGroupData: null,
    sRootFolder: '',
    aMenus : null,
    initialize : function(widgetTag)
    {
        //console.log('MapMenu.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
        Object.inheritFrom(this, Fusion.Tool.MenuBase.prototype, []);
        this.enable();
        
        var json = widgetTag.extension;
        
        //If no folder is specified for enumeration, build a menu
        //from the mapgroup alone. Folders are only supported with MapGuide.
        //Otherwise, create a hash of mapgroup resourceId to mapGroup data
        //to be used to assign mapgroup extensions to enumerated maps.
        
        var mapGroups = Fusion.applicationDefinition.mapGroups;
        this.mapGroupData = {};
        for (var i=0; i<mapGroups.length; i++) {
            var mapGroup = mapGroups[i];
            if (json.Folder) {
                this.mapGroupData[mapGroup.maps[0].resourceId] = mapGroup; 
            } else {
                var opt = {};
                opt.label = mapGroup.mapId;
                var data = mapGroup;
                var action = new Jx.Action(this.switchMap.bind(this, data));
                var menuItem = new Jx.MenuItem(action,opt);
                this.oMenu.add(menuItem);
            }
        }
        
        //get the mapdefinitions as xml if there  is a folder specified
        //in the widget tag. All subfolders will be enumerated.
        //FIXME: this should be platform agnostic, Library:// isn't!
        //FIXME: use JSON rather than XML        
        this.arch = this.getMap().getAllMaps()[0].arch;
        if (this.arch == 'MapGuide' && json.Folder) {
            this.sRootFolder = json.Folder ? json.Folder[0] : 'Library://';
            var s =       this.arch + '/' + Fusion.getScriptLanguage() +
                          '/MapMenu.' + Fusion.getScriptLanguage();
            var params =  {parameters:'folder='+this.sRootFolder,
                          onComplete: this.processMapMenu.bind(this)};
            Fusion.ajaxRequest(s, params);
        };

    },

    processMapMenu: function(r) {
        if (r.responseXML) {
            this.aMenus = {};
            var node = new DomNode(r.responseXML);
            var mapNode = node.findFirstNode('MapDefinition');
            while (mapNode) {
                
                var sId = mapNode.getNodeText('ResourceId');
                var sPath = sId.replace(this.sRootFolder, '');
                if (sPath.lastIndexOf('/') > -1) {
                    sPath = sPath.slice(0, sPath.lastIndexOf('/'));
                    this.createFolders(sPath);
                } else {
                    sPath = '';
                }
                var opt = {};
                opt.label = mapNode.getNodeText('Name');
                
                // check for mapgroup data and if there is none,
                // create a maptag that will be passed to the map
                // widget constructor 
                var data = null;
                if (this.mapGroupData[mapNode.getNodeText('ResourceId')]) {
                    data = this.mapGroupData[mapNode.getNodeText('ResourceId')];
                } else {
                    data = {maps:[{'resourceId':mapNode.getNodeText('ResourceId'),
                            'singleTile':true,
                            'type': this.arch,
                            'extension':{'ResourceId': [mapNode.getNodeText('ResourceId')]}
                           }]};
                    //set up needed accessor
                    data.getInitialView = function() {
                        return this.initialView;
                    };
                }
                var action = new Jx.Action(this.switchMap.bind(this, data));
                var menuItem = new Jx.MenuItem(action,opt);
                
                if (sPath == '') {
                    this.oMenu.add(menuItem);
                }else {
                    this.aMenus[sPath].add(menuItem);
                }
                
                mapNode = node.findNextNode('MapDefinition');
            }
        }
    },
    
    createFolders: function(sId) {
        var aPath = sId.split('/');
        //loop through folders, creating them if they don't exist
        var sParent = '';
        var sSep = '';
        for (var i=0; i < aPath.length; i++) {
            if (!this.aMenus[sParent + sSep + aPath[i]]){
                var opt = {label:aPath[i]};
                var menu = new Jx.SubMenu(opt);
                if (sParent == '') {
                    this.oMenu.add(menu);
                } else {
                    this.aMenus[sParent].add(menu);
                }
                this.aMenus[sParent + sSep + aPath[i]] = menu;
            }
            sParent = sParent + sSep + aPath[i];
            sSep = '/';
        };
    },
    
    //action to perform when the button is clicked
    activateTool: function() {
        this.oMenu.show();
    },
        
    //change the map, preserving current extents
    switchMap: function(data) {
        var ce = this.getMap().getCurrentExtents();
        data.initialView = {minX:ce.left,
                            minY:ce.bottom,
                            maxX:ce.right,
                            maxY:ce.top
                            };        
        this.getMap().loadMapGroup(data);
    }
};