/**
 * Fusion.Widget.LayerManager
 *
 * $Id: LayerManager.js 978 2007-10-17 18:24:46Z pspencer $
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
 * Class: Fusion.Widget.LayerManager
 * 
 * Displays a LayerManager of all the layers in the map as a collapsable tree.
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

Fusion.Widget.LayerManager = Class.create();
Fusion.Widget.LayerManager.prototype = {
    currentNode: null,
    bIsDrawn: false,
    initialize : function(widgetTag) {
        //console.log('LayerManager.initialize');
        Object.inheritFrom(this, Fusion.Widget.prototype, [widgetTag, true]);

        var json = widgetTag.extension;
        this.delIconSrc = json.DeleteIcon ? json.DeleteIcon[0] : 'images/icons/select-delete.png';
    
        Fusion.addWidgetStyleSheet(widgetTag.location + 'LayerManager/LayerManager.css');
        this.cursorNormal = ["url('images/grab.cur'),move", 'grab', '-moz-grab', 'move'];
        this.cursorDrag = ["url('images/grabbing.cur'),move", 'grabbing', '-moz-grabbing', 'move'];
        
        this.getMap().registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoaded.bind(this));
        this.getMap().registerForEvent(Fusion.Event.MAP_RELOADED, this.mapLoaded.bind(this));
    },
    
   
    mapLoaded: function() {
        this.draw();
    },
    
   
   /**
     * remove the dom objects representing the legend layers and groups
     */
    clear: function(node) {
        while (node.childNodes.length > 0) {
          this.clear(node.childNodes[0]);
            node.remove(node.childNodes[0]);
        }
    },
  
    /**
     * Draws the layer manager
     *
     * @param r Object the reponse xhr object
     */
    draw: function(r) {
    if (this.mapList) {
      this.mapList.remove();
//      this.clear(this.mapList);
      this.mapList = null;
    }
       
    //create the master UL element to hold the list of layers
    this.mapList = document.createElement('ul');
    Element.addClassName(this.mapList, 'jxLman');
    this.domObj.appendChild(this.mapList);
      
    //this processes the OL layers
    var map = this.getMap();
    for (var i=0; i<map.aMaps.length; ++i) {
      var mapBlock = document.createElement('li');
      Element.addClassName(this.mapBlock, 'jxLmanMap');
      mapBlock.id = 'mapBlock_'+i;
      
      //add a handle so the map blocks can be re-arranged
      var handle = document.createElement('a');
      handle.innerHTML = map.aMaps[i].sTitle;
      Element.addClassName(handle, 'jxLmanHandle');
      mapBlock.appendChild(handle);
      
      this.mapList.appendChild(mapBlock);
      this.processMapBlock(mapBlock, map.aMaps[i]);
    }
    
    if (map.aMaps.length >1) {
      var options = [];
      options.onUpdate = this.updateMapBlock.bind(this, map);
      options.handle = 'jxLmanHandle';
      Sortable.create(this.mapList.id);
    }
    },

    processMapBlock: function(blockDom, map) {
    var mapBlockList = document.createElement('ul');
    Element.addClassName(mapBlockList, 'jxLmanSet');
    mapBlockList.id = 'fusionLayerManager_'+map.getMapName();
    blockDom.appendChild(mapBlockList);
    map.layerPrefix = 'layer_';   //TODO make this unique for each block
    
    //this process all layers within an OL layer
    var processArray = map.aLayers;
    if (map.bLayersReversed) {
      processArray.reverse();
    }
    for (var i=0; i<processArray.length; ++i) {
      var blockItem = document.createElement('li');
      Element.addClassName(blockItem, 'jxLmanLayer');
      blockItem.id = map.layerPrefix+i;
      mapBlockList.appendChild(blockItem);
      this.createItemHtml(blockItem, processArray[i]);
      blockItem.layer = processArray[i];
    }
    
    var options = [];
    options.onUpdate = this.updateLayer.bind(this, map);
    Sortable.create(mapBlockList.id, options);
    },
   
  createItemHtml: function(parent, layer) {
    var delIcon = document.createElement('img');
    delIcon.src = this.delIconSrc;
    Event.observe(delIcon, 'click', this.deleteLayer.bind(this, layer));
    delIcon.style.visibility = 'hidden';
    parent.appendChild(delIcon);
    
    var visSelect = document.createElement('input');
    visSelect.type = 'checkbox';
    Event.observe(visSelect, 'click', this.visChanged.bind(this, layer));
    parent.appendChild(visSelect);
    if (layer.visible) {
      visSelect.checked = true;
    } else {
      visSelect.checked = false;
    }
    
    var label = document.createElement('a');
    label.innerHTML = layer.legendLabel;
    Event.observe(label, 'mouseover', this.setGrabCursor.bind(this));
    Event.observe(label, 'mousedown', this.setDragCursor.bind(this));
    Event.observe(label, 'mouseout', this.setNormalCursor.bind(this));
    parent.appendChild(label);
    
    Event.observe(parent, 'mouseover', this.setHandleVis.bind(this, delIcon));
    Event.observe(parent, 'mouseout', this.setHandleHide.bind(this, delIcon));
  },
  
  setHandleVis: function(delIcon) {
    delIcon.style.visibility = 'visible';
  },
  
  setHandleHide: function(delIcon) {
    delIcon.style.visibility = 'hidden';
  },
  
  setGrabCursor: function(ev) {
    this.setCursor(this.cursorNormal, Event.element(ev) );
  },
  
  setDragCursor: function(ev) {
    this.setCursor(this.cursorDrag, Event.element(ev) );
  },
  
  setNormalCursor: function(ev) {
    this.setCursor('auto', Event.element(ev) );
  },
  
    setCursor : function(cursor, domObj) {
        this.cursor = cursor;
        if (cursor && cursor.length && typeof cursor == 'object') {
            for (var i = 0; i < cursor.length; i++) {
                domObj.style.cursor = cursor[i];
                if (domObj.style.cursor == cursor[i]) {
                    break;
                }
            }
        } else if (typeof cursor == 'string') {
            domObj.style.cursor = cursor;
        } else {
            domObj.style.cursor = 'auto';  
        }
    },
  
    updateLayer: function(map, ul) {
    //reorder the layers in the client as well as the session
    var aLayerIndex = [];
    var aIds = [];
    var nLayers = ul.childNodes.length;
    for (var i=0; i<nLayers; ++i) {
      aIds[i] = ul.childNodes[i].id.split('_');
      var index = parseInt(aIds[i].pop());
      if (map.bLayersReversed) {
        index = nLayers - (index+1);
      }
      aLayerIndex.push(index);
      ul.childNodes[i].id = '';
    }
    
    //reset the ID's on the LI elements to be in order
    for (var i=0; i<ul.childNodes.length; ++i) {
      var node = ul.childNodes[i];
      aIds[i].push(i);
      node.id = aIds[i].join('_');
      node.childNodes[1].checked = node.layer.isVisible()
    }
    if (map.bLayersReversed) {
      aLayerIndex.reverse();
    }
    map.reorderLayers(aLayerIndex);
    },
   
    updateMapBlock: function(map, ul) {
    //reorder the OL layers
  },
  
  deleteLayer: function(layer, ev) {
    var targetLI = Event.element(ev).parentNode;
    var ul = targetLI.parentNode;
    Element.remove(targetLI.id);
    this.updateLayer(layer.oMap, ul);
  },
  
  visChanged: function(layer2, ev) {
    var target = Event.element(ev);
    var layer = target.parentNode.layer;
    if (target.checked) {
      layer.show();
    } else {
      layer.hide();
    }
  }

};
