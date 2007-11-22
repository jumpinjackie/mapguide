/**
 * Fusion.Widget.Legend
 *
 * $Id: Legend.js 1004 2007-10-30 14:48:40Z madair $
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

 /***************************************************************************
 * Class: Fusion.Widget.Legend
 * 
 * Displays a legend of all the layers in the map as a collapsable tree.
 *
 * ShowRootFolder (boolean, optional)
 *
 * This controls whether the tree will have a single root node that
 * contains the name of the map as its label.  By default, the root
 * node does not appear.  Set to "true" or "1" to make the root node
 * appear.
 *
 * RootFolderIcon: (string, optional)
 *
 * The url to an image to use for the root folder.  This only has an
 * affect if ShowRootFolder is set to show the root folder.
 *
 * LayerThemeIcon: (string, optional)
 *
 * The url to an image to use for layers that are currently themed.
 *
 * DisabledLayerIcon: (string, optional)
 *
 * The url to an image to use for layers that are out of scale.
 *
 * **********************************************************************/

Fusion.Widget.Legend = Class.create();
Fusion.Widget.Legend.prototype = {
    currentNode: null,
    bIsDrawn: false,
    initialize : function(widgetTag) {
        this.defLayerRasterIcon = 'images/icons/legend-raster.png';
        this.defLayerThemeIcon = 'images/icons/legend-theme.png';
        this.defDisabledLayerIcon = 'images/icons/legend-layer.png';
        this.defRootFolderIcon = 'images/icons/legend-map.png';
        this.defLayerInfoIcon = 'images/icons/tree_layer_info.png';
        this.defGroupInfoIcon = 'images/icons/tree_group_info.png';
       
        //console.log('Legend.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);
       
        var json = widgetTag.extension;
       
        this.imgLayerRasterIcon = json.LayerRasterIcon ? json.LayerRasterIcon[0] : this.defLayerRasterIcon;
       
        this.imgLayerThemeIcon = json.LayerThemeIcon ? json.LayerThemeIcon[0] : this.defLayerThemeIcon;

        this.imgDisabledLayerIcon = json.DisabledLayerIcon ? json.DisabledLayerIcon[0] : this.defDisabledLayerIcon;       
       
        this.imgLayerInfoIcon = json.LayerInfoIcon ? json.LayerInfoIcon[0] : this.defLayerInfoIcon;

        this.imgGroupInfoIcon = json.GroupInfoIcon ? json.GroupInfoIcon[0] : this.defGroupInfoIcon;
       
        //not used?
        //this.layerInfoURL = json.LayerInfoURL ? json.LayerInfoURL[0] : '';
        this.selectedLayer = null;
       
        this.oTree = new Jx.Tree(this.domObj);
       
        this.hideInvisibleLayers = (json.HideInvisibleLayers && json.HideInvisibleLayers[0]) == 'true' ? true : false;
        
        this.refreshAction = new Jx.Action(this.update.bind(this));
        this.refreshItem = new Jx.MenuItem(this.refreshAction, {label: 'Refresh'});
        this.expandAction = new Jx.Action(this.expand.bind(this));
        this.expandItem = new Jx.MenuItem(this.expandAction, {label: 'Expand'});
        this.collapseAllAction = new Jx.Action(this.collapseAll.bind(this));
        this.collapseAllItem = new Jx.MenuItem(this.collapseAllAction, {label: 'Collapse All'});
        
        this.contextMenu = new Jx.ContextMenu();
        this.contextMenu.add(this.refreshItem, this.expandItem, this.collapseAllItem);
        this.showMapFolder = (json.ShowRootFolder && json.ShowRootFolder[0] == 'true') ? true : false;
        if (this.showMapFolder) {
            var opt = {};
            opt.label = 'Map';
            opt.data = null;
            opt.imgTreeFolder = json.RootFolderIcon ? json.RootFolderIcon[0] : this.defRootFolderIcon;
            opt.imgTreeFolderOpen = opt.imgTreeFolder;
            opt.isOpen = true;
            opt.contextMenu = opt.contextMenu = this.contextMenu;
            this.oRoot = new Jx.TreeFolder(opt);
            this.oTree.append(this.oRoot);
        } else {
            this.oRoot = this.oTree;
        }
        this.extentsChangedWatcher = this.update.bind(this);
        
       
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoaded.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_RELOADED, this.draw.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADING, this.mapLoading.bind(this));
       
        //this.getLayers();
    },
    
    expand: function() {
        this.recurseTree('expand', this.oRoot);
    },
    
    collapseAll: function() {
        this.recurseTree('collapse', this.oRoot);
    },
    
    recurseTree: function(op, folder) {
        for (var i=0; i<folder.nodes.length; i++) {
            var item = folder.nodes[i];
            if (item instanceof Jx.TreeFolder) {
                this.recurseTree(op, item);
                item[op]();
            }
        }
    },
   
    mapLoading: function() {
        this.getMap().deregisterForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.extentsChangedWatcher);
        this.clear();
    },
   
    mapLoaded: function() {
        this.getMap().registerForEvent(Fusion.Event.MAP_EXTENTS_CHANGED, this.extentsChangedWatcher);
        this.draw();
    },
    
    /**
     * the map state has become invalid in some way (layer added, removed,
     * ect).  For now, we just re-request the map state from the server
     * which calls draw which recreates the entire legend from scratch
     *
     * TODO: more fine grained updating of the legend would be nice
     */
    invalidate: function() {
        this.draw();
    },
   
    /**
     * Callback for legend XML response. Creates a list of layers and sets up event
     * handling. Create groups if applicable.
     * TODO: error handling
     *
     * @param r Object the reponse xhr object
     */
    draw: function(r) {
        this.bIsDrawn = false;
        this.clear();
        var map = this.getMap();
        if (this.showMapFolder) {
            this.oRoot.setName(map.getMapName());
        }
        this.oMapInfo = map.oMapInfo;
        if (!map.layerRoot.legend) {
            map.layerRoot.legend = {};
            map.layerRoot.legend.treeItem = this.oRoot;
        }
        for (var i=0; i<map.layerRoot.groups.length; i++) {
            map.layerRoot.groups[i].visible = true;
            this.processMapGroup(map.layerRoot.groups[i], this.oRoot);
        }
        for (var i=0; i<map.layerRoot.layers.length; i++) {
            this.processMapLayer(map.layerRoot.layers[i], this.oRoot);
        }
        this.bIsDrawn = true;
        this.update();
    },
   
    processMapGroup: function(group, folder) {
        if (group.displayInLegend) {
            /* make a 'namespace' on the group object to store legend-related info */
            group.legend = {};
            var opt = {};
            opt.label = group.legendLabel;
            opt.data = group;
            opt.contextMenu = opt.contextMenu = this.contextMenu;
            opt.isOpen = group.expandInLegend;
            group.legend.treeItem = new Jx.TreeFolder(opt);
            folder.append(group.legend.treeItem);
            group.legend.checkBox = document.createElement('input');
            group.legend.checkBox.type = 'checkbox';
            group.legend.treeItem.domObj.insertBefore(group.legend.checkBox, group.legend.treeItem.domObj.childNodes[1]);
            group.legend.checkBox.checked = group.visible?true:false;
            Event.observe(group.legend.checkBox, 'click', this.stateChanged.bind(this, group));
            var groupInfo = this.getGroupInfoUrl(group.groupName);
            if (groupInfo) {
                var a = document.createElement('a');
                a.href = groupInfo;
                a.target = '_blank';
                var img = document.createElement('img');
                Jx.addToImgQueue({domElement:img, src: this.imgGroupInfoIcon});
                img.border = 0;
                a.appendChild(img);
                group.legend.treeItem.domObj.insertBefore(a, group.legend.treeItem.domObj.childNodes[4]);
            }
            if (this.oSelectionListener) {
                group.legend.treeItem.addSelectionListener(this);
            }
            for (var i=0; i<group.groups.length; i++) {
                this.processMapGroup(group.groups[i], group.legend.treeItem);
            }
            for (var i=0; i<group.layers.length; i++) {
                this.processMapLayer(group.layers[i], group.legend.treeItem);
            }
        }
    },
   
    processMapLayer: function(layer, folder) {
        /* make a 'namespace' on the layer object to store legend-related info */
        layer.legend = {};
        layer.legend.parentItem = folder;
        layer.legend.checkBox = document.createElement('input');
        layer.legend.checkBox.type = 'checkbox';
        Event.observe(layer.legend.checkBox, 'click', this.stateChanged.bind(this, layer));
        layer.legend.currentRange = null;
        layer.registerForEvent(Fusion.Event.LAYER_PROPERTY_CHANGED, this.layerPropertyChanged.bind(this));
    },
   
    layerPropertyChanged: function(eventID, layer) {
        layer.legend.checkBox.checked = layer.isVisible();
    },

    update: function() {
        if (this.bIsDrawn) {
            window.setTimeout(this._update.bind(this), 1);
        }
    },
   
    /**
     * update the tree when the map scale changes
     */
    _update: function() {
        var map = this.getMap();
        var currentScale = map.getScale();
        for (var i=0; i<map.layerRoot.groups.length; i++) {
            this.updateGroupLayers(map.layerRoot.groups[i], currentScale);
        }
        for (var i=0; i<map.layerRoot.layers.length; i++) {
            this.updateLayer(map.layerRoot.layers[i], currentScale);
        }
    },
   
    /**
     * remove the dom objects representing the legend layers and groups
     */
    clear: function() {
        while (this.oRoot.nodes.length > 0) {
            this.oRoot.remove(this.oRoot.nodes[0]);
        }
    },
    selectionChanged: function(o) {
        if (this.currentNode) {
            Element.removeClassName(this.currentNode.domObj.childNodes[3], 'jxTreeSelectedNode');
        }
        this.currentNode = o;
        Element.addClassName(this.currentNode.domObj.childNodes[3], 'jxTreeSelectedNode');
       
        if (o.data instanceof Fusion.Widget.Map.Group) {
            this.getMap().setActiveLayer(null);
        } else {
            this.getMap().setActiveLayer(o.data);
        }
    },
    updateGroupLayers: function(group, fScale) {
        for (var i=0; i<group.groups.length; i++) {
            this.updateGroupLayers(group.groups[i], fScale);
        }
        for (var i=0; i<group.layers.length; i++) {
            this.updateLayer(group.layers[i], fScale);
        }   
    },
    updateLayer: function(layer, fScale) {
        //var bFirstDisplay = false;  //removed for bug#5170
        if (!layer.displayInLegend) {
            return;
        }
        var range = layer.getScaleRange(fScale);
        if (range == layer.legend.currentRange && layer.legend.treeItem) {
            return;
        }
       
        layer.legend.currentRange = range;
        if (range != null) {
            layer.legend.checkBox.disabled = false;
            if (range.styles.length > 1) {
                //tree item needs to be a folder
                if (!layer.legend.treeItem) {
                    //bFirstDisplay = true;
                    layer.legend.treeItem = this.createFolderItem(layer);
                    layer.parentGroup.legend.treeItem.append(layer.legend.treeItem);
                } else if (layer.legend.treeItem instanceof Jx.TreeItem) {
                    this.clearTreeItem(layer);
                    layer.legend.treeItem = this.createFolderItem(layer);
                    layer.parentGroup.legend.treeItem.append(layer.legend.treeItem);
                } else {
                    while(layer.legend.treeItem.nodes.length > 0) {
                        layer.legend.treeItem.remove(layer.legend.treeItem.nodes[0]);
                    }
                }
                for (var i=0; i<range.styles.length; i++) {
                    var item = this.createTreeItem(layer,
                                               range.styles[i], fScale, false);
                    layer.legend.treeItem.append(item);
                }
            } else {
               
                //tree item is really a tree item
                if (!layer.legend.treeItem) {
                    //bFirstDisplay = true;
                    var style;
                    if (layer.supportsType(4)) {
                        style = true;
                    } else {
                        style = range.styles[0];
                    }
                    layer.legend.treeItem = this.createTreeItem(layer, style, fScale, true);
                    layer.parentGroup.legend.treeItem.append(layer.legend.treeItem);                   
                } else if (layer.legend.treeItem instanceof Jx.TreeFolder) {
                    this.clearTreeItem(layer);
                    var style;
                    if (layer.supportsType(4)) {
                        style = true;
                    } else {
                        style = range.styles[0];
                    }
                    layer.legend.treeItem = this.createTreeItem(layer, style, fScale, true);
                    layer.parentGroup.legend.treeItem.append(layer.legend.treeItem);
                } else {
                    if (range.styles.length > 0) {
                        Jx.addToImgQueue({
                            domElement:layer.legend.treeItem.domObj.childNodes[2], 
                            src: range.styles[0].getLegendImageURL(fScale, layer, this.getMap())
                        });
                    }
                    Element.removeClassName(layer.legend.treeItem.domObj, 'jxDisabled');
                }
            }
           
        } else {
            if (this.hideInvisibleLayers) {
                if (layer.legend.treeItem) {
                    layer.parentGroup.legend.treeItem.remove(layer.legend.treeItem);
                    layer.legend.treeItem = null;
                }
            } else {
                layer.legend.checkBox.disabled = true;
                this.clearTreeItem(layer);
                var newTreeItem = this.createTreeItem(layer, null, null, true);
                if (layer.legend.treeItem) {
                    layer.parentGroup.legend.treeItem.replace(newTreeItem, layer.legend.treeItem);
                    layer.legend.treeItem.finalize();
                } else {
                    layer.parentGroup.legend.treeItem.append(newTreeItem);
                }
                layer.legend.treeItem = newTreeItem;
            }
        }
        //if (bFirstDisplay) {
            layer.legend.checkBox.checked = layer.visible?true:false;
        //}
    },
    createFolderItem: function(layer) {
        var opt = {};
        opt.label = layer.legendLabel == '' ? '&nbsp;' : layer.legendLabel;
        opt.data = layer;
        opt.isOpen = layer.expandInLegend;
        opt.contextMenu = opt.contextMenu = this.contextMenu;
        opt.imgTreeFolderOpen = layer.themeIcon;
        opt.imgTreeFolder = layer.themeIcon;
        var folder = new Jx.TreeFolder(opt);
        folder.domObj.insertBefore(layer.legend.checkBox, folder.domObj.childNodes[1]);
        var layerInfo = this.getLayerInfoUrl(layer.layerName);
        if (layerInfo) {
            var a = document.createElement('a');
            a.href = layerInfo;
            a.target = '_blank';
            var img = document.createElement('img');
            Jx.addToImgQueue({domElement:img, src:this.imgLayerInfoIcon});
            img.border = 0;
            a.appendChild(img);
            folder.domObj.insertBefore(a, folder.domObj.childNodes[4]);
        }
        folder.addSelectionListener(this);
       
        return folder;
    },
    createTreeItem: function(layer, style, scale, bCheckBox) {
        var opt = {};
        if (bCheckBox) {
            opt.label = layer.legendLabel == '' ? '&nbsp;' : layer.legendLabel;
        } else {
            opt.label = style.legendLabel == '' ? '&nbsp;' : style.legendLabel;
        }
        opt.data = layer;
        opt.contextMenu = opt.contextMenu = this.contextMenu;
        if (!style) {
            opt.imgIcon = this.imgDisabledLayerIcon;
            opt.enabled = false;
        } else if (layer.supportsType(4) && !style.getLegendImageURL) {
            opt.imgIcon = this.imgLayerRasterIcon;
        } else {
            opt.imgIcon = style.getLegendImageURL(scale, layer);
        }
       
        var item = new Jx.TreeItem(opt);
        if (bCheckBox) {
            item.domObj.insertBefore(layer.legend.checkBox, item.domObj.childNodes[1]);
            /* only need to add layer info if it has a check box too */
            var layerInfo = this.getLayerInfoUrl(layer.layerName);
            if (layerInfo) {
                var a = document.createElement('a');
                a.href = layerInfo;
                a.target = '_blank';
                var img = document.createElement('img');
                Jx.addToImgQueue({domElement:img, src: this.imgLayerInfoIcon});
                img.border = 0;
                a.appendChild(img);
                item.domObj.insertBefore(a, item.domObj.childNodes[4]);
            }
        }

        item.addSelectionListener(this);
       
        return item;
    },
    clearTreeItem: function(layer) {
        if (layer.legend.treeItem && layer.legend.treeItem.parent) {
            layer.legend.treeItem.parent.remove(layer.legend.treeItem);
            layer.legend.treeItem.finalize();
            layer.legend.treeItem = null;
        }
    },
    stateChanged: function(obj) {
        if (obj.legend && obj.legend.checkBox) {
            if (obj.legend.checkBox.checked) {
                obj.show();
            } else {
                obj.hide();
            }
        }
    },
    getGroupInfoUrl: function(groupName) {
        if (this.oMapInfo) {
            var groups = this.oMapInfo.links.groups;
            for (var i=0; i<groups.length; i++) {
                if (groups[i].name == groupName) {
                    return groups[i].url;
                }
            }
        }
        return null;
    },
    getLayerInfoUrl: function(layerName) {
        if (this.oMapInfo) {
            var layers = this.oMapInfo.links.layers;
            for (var i=0; i<layers.length; i++) {
                if (layers[i].name == layerName) {
                    return layers[i].url;
                }
            }
        }
        return null;
    }
};
