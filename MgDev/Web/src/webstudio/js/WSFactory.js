/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSFactory.js,v 1.84 2006/06/16 20:48:18 zak Exp $
 * @fileoverview    this file contains classes for creating objects
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @author          Zak James (zjames@dmsolutions.ca)
 * @author          Fred Warnock (fwarnock@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser
 * General Public License as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
var WSFactory = Class.create();
WSFactory.prototype = {
    app : '',
    aRid : '',
    aValidTypes : null,
    initialize : function(app) {
        //jxTrace('WSFactory::initialize()');
        this.app = app;
        this.aValidTypes = {
            MapDefinition : WSMapDefinition,
            WebLayout: WSWebLayout,
            LayerDefinition : WSLayerDefinition,
            FeatureSource : WSFeatureSource
        };
    },
    process: function(xml) {
        //jxTrace('WSFactory::process()');
        var d = new DomNode(xml);
        var a = [];
        
        var resourceList = d.findFirstNode('ResourceList');
        var l = resourceList.childNodes.length;
        for (var i=0; i<l; i++) {
            var child = resourceList.childNodes[0];
            resourceList.removeChild(child);
            if (child.nodeName == 'ResourceFolder') {
                o = new WSFolderObj(child, this.app);
            } else if (child.nodeName == 'ResourceDocument') {
                var rid = child.findFirstNode('ResourceId');
                if (rid) {
                    var szType = rid.textContent.split('.')[1];
                    if (this.aValidTypes[szType]) {
                        o = new this.aValidTypes[szType] (child, this.app);
                    } else {
                        o = new WSUnsupported(child, this.app);
                    }
                } else {
                    //error
                }
            }
            if (o) {
                a.push(o);
            }
            d.findNextNode('ResourceDocument');
        }
        return a;
    },
    
    createObj : function( xml ) {
    }
};

var WSBaseObj = Class.create();

WSBaseObj.prototype = {
    app : '',
    ctype : 'base',
    depth : 0,
    modified: '',
    rid : null,
    dom : null,
    _path : null,
    name : '',
    szPath : '',
    content : null,
    header : null,
    dom : null,
    cl : null,
    dirty : null,
    initialize : function(xml, app) {
        //jxTrace('WSBaseObj::initialize()');
        this.baseInitialize(xml, app);
    },
    baseInitialize : function( xml, app ) {
        //jxTrace('WSBaseObj::baseInitialize()');
        this.app = app;
        this.dom = xml;
        this.cl = [];
        this.content = null;
        this.header = null;
        this.parseResourceID();
        this.dirty = false;
    },
    openView: function() { },
    closeView: function() {
        //jxTrace('WSBaseObj::closeView()')
        this.view.finalize();
        this.view = null;
    },
    showView: function() {
        //jxTrace('WSBaseObj::showView');
        if (this.view) {
            this.view.show();
        }
    },
    hideView: function() {
        //jxTrace('WSBaseObj::hideView()');
        if (this.view) {
            this.view.hide();
        }
    },
    setClean: function() {
        //jxTrace('WSBaseObj::setClean()');
        if (this.dirty) {
            this.dirty = false;
            this.processEvent(this.cl, 'contentChanged', this);
        }
    },
    setDirty: function() {
        //jxTrace('WSBaseObj::setDirty()');
        if (!this.dirty) {
            this.dirty = true;
            this.processEvent(this.cl, 'contentChanged', this);
        }
    },
    isDirty : function() {
        //jxTrace('WSBaseObj::isDirty()');
        return this.dirty;
    },
    requestContent : function() {
        //jxTrace('WSBaseObj::requestContent()');
        var r = new WSGetResourceContent(this.getResourceID());
        var b = this.app.getBroker();
        b.dispatchRequest(r, this.contentLoaded.bind(this));
    },
    requestHeader : function() {
        //jxTrace('WSBaseObj::requestHeader()');
        var r = new WSGetResourceHeader(this.getResourceID());
        var b = this.app.getBroker();
        b.dispatchRequest(r, this.headerLoaded.bind(this));
    },
    contentLoaded : function(r) {
        //jxTrace('WSBaseObj::contentLoaded()');
        if (r.responseXML) {
            //use childNodes[0] to avoid extra <#document> tag
            this.content = this.xhrToDomNode(r.responseXML);
            this.processEvent(this.cl, 'contentLoaded', this);
        }
    },
    headerLoaded : function(r) {
        //jxTrace('WSBaseObj::headerLoaded()');
        if (r.responseXML) {
            this.header = this.xhrToDomNode(r.responseXML);
            this.processEvent(this.cl, 'headerLoaded', this);
        }
    },
    setContent : function(xml, f) {
        //jxTrace('WSBaseObj::setContent()');
        var r = new WSSetResource(this.getResourceID(), xml);
        var b = this.app.getBroker();
        b.dispatchRequest(r, f);
    },
    save : function() {
        //jxTrace('WSBaseObj::save()');
        if (!this.dirty && !arguments[0]) {
            return true;
        }
        var xml = this.contentToXML();
        //jxTrace(xml);
        this.setContent(xml, this.setClean.bind(this));
    },
    /**
     * save the resource in the session repository.
     * 
     * The callback is used to notify the view which is a listener
     */
    saveToSession: function() {
        //jxTrace('WSBaseObj::saveToSession()');
        var szRid = this.getSessionResourceID();
        var xml = this.contentToXML();
        var r = new WSSetResource(szRid, xml);
        var b = this.app.getBroker();
        b.dispatchRequest(r,  this.sessionCopySaved.bind(this));
    },
    /**
     * notify view that the session copy is ready
     */
    sessionCopySaved: function(r) {
        if (r.status == 200) {
           //jxTrace(this.getSessionResourceID() + ' saved with status: ' + r.status);
            this.processEvent(this.cl, 'layoutWasSet', this);
        } else {
           //jxTrace(this.getSessionResourceID() + "<b>FAILED</b> with " + r.statusText);
        }
    },
    contentToXML : function() {
        //jxTrace('WSBaseObj::contentToXML()');
        return this.content.toXML();
    },
    getPath : function() {
        //jxTrace('WSBaseObj::getPath()');
        var s = this._path[0] +  '';
        for (var i=1; i<this._path.length;i++) {
            s = s + this._path[i] + '/';
        }
        return s;
    },
    getDomValue : function(tagName) {
        //jxTrace('WSBaseObj::getDomValue('+tagName+')');
        var t = this.dom.findFirstNode(tagName);
        return t?t.textContent:'';
    },
    setDomValue : function(tagName, value) {
        //jxTrace('WSBaseObj::setDomValue('+tagName+','+value+')');
        var t = this.dom.findFirstNode(tagName);
        if (t) { 
            t.textContent = value; 
        }
    },
    getContentValue : function(tagName) {
        //jxTrace('WSBaseObj::getContentValue('+tagName+')');
        var t = this.content.findFirstNode(tagName);
        return t.textContent;
    },
    setContentValue : function(tagName, value) {
        //jxTrace('WSBaseObj::setContentValue('+tagName+','+value+')');
        var t = this.content.findFirstNode(tagName);
        if (t.textContent != value) {
            this.setDirty();
            t.textContent = value;
        }
    },
    getHeaderValue : function(tagName) {
        //jxTrace('WSBaseObj::getHeaderValue('+tagName+')');
        var t = this.header.findFirstNode(tagName);
        return t.textContent;
    },
    setHeaderValue : function(tagName, value) {
        //jxTrace('WSBaseObj::setHeaderValue('+tagName+','+value+')');
        var t = this.header.findFirstNode(tagName);
        t.textContent = value;
    },
    setResourceID: function(rid) {
        this.rid = rid;
        this.parseResourceID();
        this.setDirty();
    },
    getResourceID : function() { 
        //jxTrace('WSBaseObj::getResourceID()');
        if (this.rid) {
            return this.rid;
        } else {
            return this.getDomValue('ResourceId');
        }
    },
    /**
     * this uses the WSResourceId utility class to build the session
     * version of the object's resource id.
     *
     */
    getSessionResourceID: function() {
        //TODO: convert other functions in this class to use the object
        var oOriginalId = new WSResourceId(this.getResourceID());
        return oOriginalId.getSessionResourceId(this.app.getSession());
    },
    getDepth : function() { 
        //jxTrace('WSBaseObj::getDepth()'); 
        return this.getDomValue('Depth') 
    },
    getName : function() { 
        //jxTrace('WSBaseObj::getName() - ' + this.name);
        return this.name; 
    },
    getType : function() { 
        //jxTrace('WSBaseObj::getType() - ' + this.ctype);
        return this.ctype; 
    },
    parseResourceID : function() {
        //jxTrace('WSBaseObj::parseResourceID()');
        this._path = ['Library://'];
        this.rid = this.getResourceID();
        //strip Library:// from the beginning
        //split on '/'
        //if the last element of the array is empty, its a folder
        
        if (this.rid == 'Library://') {
            this.name = 'Library://';
            this.ctype = 'folder';
        } else {
            var rid = this.rid.substring(10);
            var path = rid.split('/');
            for (var i=0; i<path.length; i++) {
                if (path[i] != '') {
                    this._path.push(path[i]);
                }
            }
            this.name = this._path.pop();
            var idx = this.name.lastIndexOf('.');
            if (idx >= 0) {
                this.ctype = this.name.substring(idx+1);
                this.name = this.name.substring(0,idx);
            } else {
                this.ctype = 'folder';
            }
        }
        this.szPath = this.getPath();
    },
    getView : function() {
        //jxTrace('WSBaseObj::getView()');
        return this.view;
    },
    addContentListener : function( o ) {
        //jxTrace('WSBaseObj::addContentListener()');
        this.addListener( this.cl, o );
    },
    removeContentListener : function( o ) {
        //jxTrace('WSBaseObj::removeContentListener');
        this.removeListener(this.cl, o);
    },
    requestResourceReferences: function() {
        //jxTrace('WSBaseObj::requestResourceReferences()');
        if (!this.view) {
            return;
        }
        this.view._oPropertiesPanel.setBusy(true);
        var r = new WSEnumerateResourceReferences(this.getResourceID());
        var b = this.app.getBroker();
        b.dispatchRequest(r, this.resourceReferencesLoaded.bind(this));
    },
    resourceReferencesLoaded: function(r) {
        //jxTrace('WSBaseObj::resourceReferencesLoaded()');
        if (!this.view) {
            return;
        }
        this.view._oPropertiesPanel.setBusy(false);
        if (r.responseXML) {
            this.resourceReferences = this.xhrToDomNode(r.responseXML);
            this.processEvent(this.cl, 'resourceReferencesLoaded', this);
        }
    },
    getResourceReferences : function() {
        //jxTrace('WSBaseObj::getResourceReferences()');
        var references = [];
        var node = this.resourceReferences.findFirstNode('ResourceId');
        while(node) {
            references.push(node.textContent);
            node = this.resourceReferences.findNextNode('ResourceId');
        }
        return references;
    },
    /**
     * convert an XmlHttpResponse responseXML to a DomNode
     */
    xhrToDomNode: function( xmlNode ) {
        var result = null;
        if (xmlNode.nodeName == '#document') {
            for (var i=0; i<xmlNode.childNodes.length; i++) {
                if (xmlNode.childNodes[i].nodeName != 'xml') {
                    result = new DomNode(xmlNode.childNodes[i]);
                    break;
                }
            }
        } else {
            alert( 'no #document tag in response xml' );
        }
        
        return result;
    },
    createDomNode: function(name, value) {
        var node = new DomNode();
        node.nodeName = name;
        node.textContent = value || '';
        return node;
    }
};
Object.extend(WSBaseObj.prototype, JxListenerBase.prototype);

var WSFolderObj = Class.create();
Object.extend(WSFolderObj.prototype, WSBaseObj.prototype);
Object.extend(  WSFolderObj.prototype, {
    numberOfFolders: 0,
    numberOfDocuments: 0,
    initialize : function( xml, app ) {
        this.baseInitialize( xml,app );
        this.ctype = 'folder';
        //TODO: get the enclosed item count from xml
        this.numberOfDocuments = 0;
        this.view = null;
    },
    openView : function(domObj) {
        if (!this.view) {
            this.setClean();
            this.view = new WSUnsupportedView(this, domObj);
            this.requestHeader();
        }
    }
});

var WSMapDefinition = Class.create();
Object.extend(WSMapDefinition.prototype, WSBaseObj.prototype);
Object.extend(  WSMapDefinition.prototype, {
    numberOfFolders: 0,
    numberOfDocuments: 0,
    initialize : function( xml, app ) {
        this.baseInitialize( xml,app );
        this.ctype = 'MapDefinition';
    },
    openView : function(domObj) {
        if (!this.view) {
            this.setClean();
            this.view = new WSMapDefinitionView(this, domObj);
            this.requestContent();
            this.requestHeader();
            this.requestResourceReferences();
        }
    },
    /*
     * Find resources that this definition refers TO
     * They are internal to the content XML
     */
    getResourceToReferences : function() {
        //jxTrace('WSMapDefinition::getResourceToReferences()');
        var references = [];
        var node = this.content.findFirstNode('ResourceId');
        while(node) {
            references.push(node.textContent);
            node = this.content.findNextNode('ResourceId');
        }
        return references;
    },
    /**
     * overload base class function to update XML if the object
     * is dirty.  This serves both save and saveToSession
     */
    contentToXML : function() {
        if (this.isDirty()) {
            this.updateContent();
        }
        return this.content.toXML();
    },

    //TODO: replace this with setContentValue from base class
    changeSetting: function(name, value) {
        var node = this.content.findFirstNode(name);
        if (!(node.getNodeText(name)==value)) {
            node.setNodeText(name, value);
            this.setDirty();
        }
    },
    contentLoaded : function(r) {
        //jxTrace('WSLayerDefinition::contentLoaded()');
        if (r.responseXML) {
            this.content = this.xhrToDomNode(r.responseXML);
            this.parseMapLayers();
            this.processEvent(this.cl, 'contentLoaded', this);    
        }
    },
    getGroup: function(groupName) {
        for (var i=0; i<this.mapGroups.length; i++) {
            if (this.mapGroups[i].name == groupName) {
                return this.mapGroups[i];
            }
        }
        return null;
    },
    parseMapLayers: function() {
        this.mapLayers = [];
        this.groupRoot = {name:'',groups:[],layers:[]};
        this.mapGroups = [];
        //process groups
        var group = this.content.findFirstNode('MapLayerGroup');
        while(group) {
            var o = {};
            o.name = group.getNodeText('Name');
            //jxTrace('processing group ' + o.name);
            o.visible = group.getNodeText('Visible');
            o.showInLegend = group.getNodeText('ShowInLegend');
            o.expandInLegend = group.getNodeText('ExpandInLegend');
            o.legendLabel = group.getNodeText('LegendLabel');
            o.group = group.getNodeText('Group');
            o.groups = []; //groups contained by this group
            o.layers = []; //layers contained by this group
            
            if (o.group != '') {
                var parent = this.getGroup(o.group);
                if (parent) {
                    //jxTrace('adding ' + o.name + ' to parent group ' + parent.name);
                    parent.groups.push(o);
                    o.parent = parent;
                } else {
                    //jxTrace('error adding ' + o.name + ' to group ' + o.group);
                }
            } else {
                this.groupRoot.groups.push(o);
                o.parent = this.groupRoot;
            }
            this.mapGroups.push(o);
            group = this.content.findNextNode('MapLayerGroup');
        }
        //process layers in the order in which they appear (its important!)
        var layer = this.content.findFirstNode('MapLayer');
        while(layer) {
            var o = {};
            o.name = layer.getNodeText('Name');
            //jxTrace('processing layer ' + o.name );
            o.resourceId = layer.getNodeText('ResourceId');
            o.selectable = layer.getNodeText('Selectable');
            o.showInLegend = layer.getNodeText('ShowInLegend');
            o.legendLabel = layer.getNodeText('LegendLabel');
            o.expandInLegend = layer.getNodeText('ExpandInLegend');
            o.visible = layer.getNodeText('Visible');
            o.group = layer.getNodeText('Group');
            if (o.group != '') {
                var parent = this.getGroup(o.group);
                if (parent) {
                    parent.layers.push(o);
                    o.parent = parent;
                } else {
                    //jxTrace(o.name + ' failed to be added to group ' + o.group);
                    this.group = '';
                    o.parent = null;
                }
            } else {
                //jxTrace(o.name + ' has no group');
                
                this.groupRoot.layers.push(o);
                o.parent = this.groupRoot;
            }
            this.mapLayers.push(o);
            layer = this.content.findNextNode('MapLayer');
        }
        var baseMapDefinition = this.content.findFirstNode('BaseMapDefinition');
        this.finiteDisplayScales = [];
        /* capture the baseMapGroups in the same kind of structure as the
         * groups/layers to make the ui code reusable.  This isn't strictly
         * necessary since the structure of the base map groups and layers
         * is restricted
         */
        this.baseMapGroups = {groups:[],layers:[]};
        this.baseMapLayers = [];
        if (baseMapDefinition) {
            var fds = baseMapDefinition.findFirstNode('FiniteDisplayScale');
            while(fds) {
                this.finiteDisplayScales.push(parseFloat(fds.textContent));
                fds = baseMapDefinition.findNextNode('FiniteDisplayScale');
            }
            this.finiteDisplayScales = this.finiteDisplayScales.sortBy(function(value,index){return value});
            var bmlg = baseMapDefinition.findFirstNode('BaseMapLayerGroup');
            while(bmlg) {
                var group = {}
                group.name = bmlg.getNodeText('Name');
                group.visible = bmlg.getNodeText('Visible');
                group.showInLegend = bmlg.getNodeText('ShowInLegend');
                group.expandInLegend = bmlg.getNodeText('ExpandInLegend');
                group.legendLabel = bmlg.getNodeText('LegendLabel');
                group.groups = [];
                group.layers = [];
                var bml = bmlg.findFirstNode('BaseMapLayer');
                while(bml) {
                    var layer = {};
                    layer.name = bml.getNodeText('Name');
                    layer.resourceId = bml.getNodeText('ResourceId');
                    layer.selectable = bml.getNodeText('Selectable');
                    layer.showInLegend = bml.getNodeText('ShowInLegend');
                    layer.legendLabel = bml.getNodeText('LegendLabel');
                    layer.expandInLegend = bml.getNodeText('ExpandInLegend');
                    group.layers.push(layer);
                    this.baseMapLayers.push(layer);
                    layer.parent = group;
                    bml = bmlg.findNextNode('BaseMapLayer');
                }
                this.baseMapGroups.groups.push(group);
                group.parent = this.baseMapGroups;
                bmlg = baseMapDefinition.findNextNode('BaseMapLayerGroup');
            }
        }
    },
    createMapLayerGroup: function(parent) {
        var group = {};
        group.name = this.uniqueName('Layer Group', this.mapGroups.concat(this.baseMapGroups));
        group.visible = 'true';
        group.showInLegend = 'true';
        group.expandInLegend = 'true';
        group.legendLabel = group.name
        group.parent = parent;
        group.groups = []; //groups contained by this group
        group.layers = []; //layers contained by this group
        parent.groups.push(group);
        this.mapGroups.push(group);
        this.setDirty();
        return group;
    },
    createMapLayer: function(parent, resourceId) {
        var ld = new WSResourceId(resourceId);
        var layer = {};
        layer.name = this.uniqueName(ld.getName(), this.mapLayers.concat(this.baseMapLayers));
        layer.resourceId = resourceId;
        layer.selectable = 'true';
        layer.showInLegend = 'true';
        layer.legendLabel = layer.name;
        layer.expandInLegend = 'true';
        layer.visible = 'true';
        layer.group = parent.name;
        layer.parent = parent;
        parent.layers.push(layer);
        this.mapLayers.push(layer);
        this.setDirty();
        return layer;
    },
    createBaseMapGroup: function() {
        var group = {}
        group.name = this.uniqueName('Layer Group', this.mapGroups.concat(this.baseMapGroups.groups));
        group.visible = 'true';
        group.showInLegend = 'true';
        group.expandInLegend = 'true';
        group.legendLabel = group.name;
        group.groups = [];
        group.layers = [];
        this.baseMapGroups.groups.push(group);
        this.setDirty();
        return group;
    },
    createBaseMapLayer: function(parent,resourceId) {
        var ld = new WSResourceId(resourceId);
        var layer = {};
        layer.name = this.uniqueName(ld.getName(), this.mapLayers.concat(this.baseMapLayers));
        layer.resourceId = resourceId;
        layer.selectable = 'true';
        layer.showInLegend = 'true';
        layer.legendLabel = layer.name;
        layer.expandInLegend = 'true';
        parent.layers.push(layer);
        layer.parent = parent;
        this.baseMapLayers.push(layer);
        this.setDirty();
        return layer;
    },
    /**
     * create a deep copy of an object
     */
    cloneObject: function(obj) {
        var clone = Object.extend({}, obj);
        if (clone.groups) {
            clone.name = this.uniqueName(clone.name, this.mapGroups.concat(this.baseMapGroups));
            for (var i=0; i<clone.groups.length; i++) {
                clone.groups[i] = this.cloneObject(clone.groups[i]);
            }
        } else {
            clone.name = this.uniqueName(clone.name, this.mapLayers.concat(this.baseMapLayers));
        }
        return clone;
    },
    removeMapLayer: function(layer) {
        var idx = layer.parent.groups.indexOf(layer);
        layer.parent.groups.splice(idx,1);
        idx = this.mapLayers.indexOf(layer);
        this.mapLayers.splice(idx,1);
        this.setDirty();
    },
    removeMapLayerGroup: function(group) {
        for (var i=0; i<group.groups.length; i++) {
            this.removeMapLayerGroup(group.groups[i]);
        }
        for (var i=0; i<group.layers.length; i++) {
            this.removeMapLayer(group.layers[i]);
        }
        var idx = group.parent.groups.indexOf(group);
        group.parent.groups.splice(idx,1);
        idx = this.mapGroups.indexOf(group);
        this.mapGroups.splice(idx,1);
        this.setDirty();
    },
    removeBaseMapLayer: function(layer) {
        var idx = layer.parent.groups.indexOf(layer);
        layer.parent.groups.splice(idx,1);
        idx = this.baseMapLayers.indexOf(layer);
        this.baseMapLayers.splice(idx,1);
        this.setDirty();
    },
    removeBaseMapGroup: function(group) {
        for (var i=0; i<group.groups.length; i++) {
            this.removeBaseMapGroup(group.groups[i]);
        }
        for (var i=0; i<group.layers.length; i++) {
            this.removeBaseMapLayer(group.layers[i]);
        }
        var idx = group.parent.groups.indexOf(group);
        group.parent.groups.splice(idx,1);
        idx = this.baseMapGroups.indexOf(group);
        this.baseMapGroups.splice(idx,1);
        this.setDirty();
    },
    /**
     * attach a map layer to the object model.  It must be inserted relative
     * to some other object in the model.  If the relative is a group, then the
     * layer is appended to that group.  Otherwise, it is appended to the root
     * group.
     * Appending to a group puts the layer last in the drawing order.  Appending
     * relative to a layer inserts the layer into the drawing order.
     */
    attachMapLayer: function(layer, relativeTo) {
        if (relativeTo.groups) {
            if (!layer.parent) {
                relativeTo.layers.push(layer);
                layer.parent = relativeTo;
            }
            this.mapLayers.push(layer);
        } else {
            var idx = this.mapLayers.indexOf(relativeTo);
            this.mapLayers.splice(idx,0,layer);
            layer.parent = this.groupRoot;
        }
        this.setDirty();
    },
    /** 
     * attach a group (optionally with sub groups and layers) to the
     * object model.  The group must be inserted relative to some other
     * object in the model.  The relative must be a group.  The group is
     * made a sub-group of the relative.  All layers are appended to the
     * end of the drawing order
     */
    attachMapLayerGroup: function(group, relativeTo) {
        this.mapGroups.push(group);

        if (!group.parent) {
            relativeTo.groups.push(group);
            group.parent = relativeTo;
        }
        
        for (var i=0; i<group.groups.length; i++) {
            this.attachMapLayerGroup(group.groups[i], group);
        }
        for (var i=0; i<group.layers.length; i++) {
            this.attachMapLayer(group.layers[i], group);
        }
        this.setDirty();
    },
    /**
     * attach a base map layer to the object model.  It must be inserted 
     * relative to a group.
     */
    attachBaseMapLayer: function(layer, relativeTo) {
        this.baseMapLayers.push(layer);
        if (!layer.parent) {
            relativeTo.layers.push(layer);
            layer.parent = relativeTo;
        }
        this.setDirty();
    },
    /**
     * attach a base map group to the object model.  It is not inserted
     * relative to anything in the base map definition because it
     * is always appended to the root group
     */
    attachBaseMapGroup: function(group) {
        this.baseMapGroups.groups.push(group);
        for (var i=0; i<group.layers; i++) {
            this.attachBaseMapLayer(group.layers[i], group);
        }
        this.setDirty();
    },
    /**
     * detach a map layer from the object model, optionally detaching
     * it from its parent.  This allows for cut/paste
     */
    detachMapLayer: function(obj) {
        var idx = this.mapLayers.indexOf(obj);
        this.mapLayers.splice(idx,1);
        if (!arguments[1]) {
            idx = obj.parent.layers.indexOf(obj);
            obj.parent.layers.splice(idx,1);
            obj.parent = null;
        }
        this.setDirty();
    },
    /**
    * detach a group from the object model, optionally detaching from
    * its parent.  This allows for cut/paste
    */
    detachMapLayerGroup: function(obj) {
        for (var i=0; i<obj.layers.length; i++) {
            this.detachMapLayer(obj.layers[i], true);
        }
        for (var i=0; i<obj.groups.length; i++) {
            this.detachMapLayerGroup(obj.groups[i], true);
        }
        var idx = this.mapGroups.indexOf(obj);
        this.mapGroups.splice(idx,1);
        /* detach from parent as well, but only the top level */
        if (!arguments[1]) {
            idx = obj.parent.groups.indexOf(obj);
            obj.parent.groups.splice(idx,1);
            obj.parent = null;
        }
        this.setDirty();
    },
    /**
    * detach a base map layer from the object model, optionally detaching from
    * its parent.  This allows for cut/paste
    */
    detachBaseMapLayer: function(obj) {
        var idx = this.baseMapLayers.indexOf(obj);
        this.baseMapLayers.splice(idx,1);
        if (!arguments[1]) {
            idx = obj.parent.layers.indexOf(obj);
            obj.parent.layers.splice(idx,1);
            obj.parent = null;
        }
        this.setDirty();
    },
    /**
    * detach a base map group from the object model, optionally detaching from
    * its parent.  This allows for cut/paste
    */
    detachBaseMapGroup: function(obj) {
        for (var i=0; i<obj.layers.length; i++) {
            this.detachBaseMapLayer(obj.layers[i], true);
        }
        for (var i=0; i<obj.groups.length; i++) {
            this.detachBaseMapGroup(obj.groups[i], true);
        }
        var idx = this.baseMapGroups.groups.indexOf(obj);
        this.baseMapGroups.groups.splice(idx,1);
        if (!arguments[1]) {
            idx = obj.parent.groups.indexOf(obj);
            obj.parent.groups.splice(idx,1);
            obj.parent = null;
        }
        this.setDirty();
    },
    isGroupNameUnique: function(group, name) {
        var a = this.mapGroups.concat(this.baseMapGroups.groups);
        for (var i=0; i<a.length;i++) {
            if (a[i] != group && a[i].name == name) {
                return false;
            }
        }
        return true;
    },
    isLayerNameUnique: function(layer, name) {
        var a = this.mapLayers.concat(this.baseMapLayers);
        for (var i=0; i<a.length;i++) {
            if (a[i] != layer && a[i].name == name) {
                return false;
            }
        }
        return true;
    },
    uniqueName: function(originalName, from) {
        var name = originalName;
        var i = 0;
        var j = 2;
        while(i<from.length) {
            if (from[i].name == name) {
                name = originalName + ' ('+j+')';
                j++;
                i=0;
            } else {
                i++;
            }
        }
        return name;
    },
    setDrawingOrder: function( aLayers ) {
        this.setDirty();
        this.mapLayers = aLayers;
    },
    /**
     * reverse the process of parseMapLayers by turning the arrays back into 
     * a dom tree
     */
    updateContent: function() {
        //rebuild the dom representation from the objects
        var mapDefinition = this.content.findFirstNode('MapDefinition');
        //remove MapLayer
        var obj = mapDefinition.findFirstNode('MapLayer');
        while(obj) {
            mapDefinition.removeChild(obj);
            obj = mapDefinition.findFirstNode('MapLayer');
        }
        //remove MapLayerGroup
        obj = mapDefinition.findFirstNode('MapLayerGroup');
        while(obj) {
            mapDefinition.removeChild(obj);
            obj = mapDefinition.findFirstNode('MapLayerGroup');
        }
        //remove BaseMapDefinition
        obj = mapDefinition.findFirstNode('BaseMapDefinition');
        while(obj) {
            mapDefinition.removeChild(obj);
            obj = mapDefinition.findFirstNode('BaseMapDefinition');
        }
        
        //build map layers
        for (var i=0; i<this.mapLayers.length; i++) {
            var o = this.mapLayers[i];
            obj = DomNodeFactory.create('MapLayer');
            obj.appendChild(DomNodeFactory.create('Name', o.name));
            obj.appendChild(DomNodeFactory.create('ResourceId', o.resourceId));
            obj.appendChild(DomNodeFactory.create('Selectable', o.selectable));
            obj.appendChild(DomNodeFactory.create('ShowInLegend', o.showInLegend));
            obj.appendChild(DomNodeFactory.create('LegendLabel', o.legendLabel));
            obj.appendChild(DomNodeFactory.create('ExpandInLegend', o.expandInLegend));
            obj.appendChild(DomNodeFactory.create('Visible', o.visible));
            obj.appendChild(DomNodeFactory.create('Group', o.parent.name));
            mapDefinition.appendChild(obj);
        }

        //build map groups
        for (var i=0; i<this.mapGroups.length; i++) {
            var o = this.mapGroups[i];
            obj = DomNodeFactory.create('MapLayerGroup');
            obj.appendChild(DomNodeFactory.create('Name', o.name));
            obj.appendChild(DomNodeFactory.create('Visible', o.visible));
            obj.appendChild(DomNodeFactory.create('ShowInLegend', o.showInLegend));
            obj.appendChild(DomNodeFactory.create('ExpandInLegend', o.expandInLegend));
            obj.appendChild(DomNodeFactory.create('LegendLabel', o.legendLabel));
            obj.appendChild(DomNodeFactory.create('Group', o.parent.name));
            mapDefinition.appendChild(obj);
        }

        //build map base layers.  This is only valid if there is at least
        //one finite display scale.  Oddly it doesn't matter if there
        //are no groups or layers ... it just needs a finite display scale
        if (this.finiteDisplayScales.length > 0) {
            var baseMapDefinition = DomNodeFactory.create('BaseMapDefinition');
            for (var i=0; i<this.finiteDisplayScales.length;i++) {
                baseMapDefinition.appendChild(DomNodeFactory.create('FiniteDisplayScale', this.finiteDisplayScales[i]));
            }
            for (var i=0; i<this.baseMapGroups.groups.length; i++) {
                var o = this.baseMapGroups.groups[i];
                obj = DomNodeFactory.create('BaseMapLayerGroup');
                obj.appendChild(DomNodeFactory.create('Name', o.name));
                obj.appendChild(DomNodeFactory.create('Visible', o.visible));
                obj.appendChild(DomNodeFactory.create('ShowInLegend', o.showInLegend));
                obj.appendChild(DomNodeFactory.create('ExpandInLegend', o.expandInLegend));
                obj.appendChild(DomNodeFactory.create('LegendLabel', o.legendLabel));
                for (var j=0; j<o.layers.length;j++) {
                    var layer = o.layers[j];
                    var baseMapLayer = DomNodeFactory.create('BaseMapLayer');
                    baseMapLayer.appendChild(DomNodeFactory.create('Name', layer.name));
                    baseMapLayer.appendChild(DomNodeFactory.create('ResourceId', layer.resourceId));
                    baseMapLayer.appendChild(DomNodeFactory.create('Selectable', layer.selectable));
                    baseMapLayer.appendChild(DomNodeFactory.create('ShowInLegend', layer.showInLegend));
                    baseMapLayer.appendChild(DomNodeFactory.create('LegendLabel', layer.legendLabel));
                    baseMapLayer.appendChild(DomNodeFactory.create('ExpandInLegend', layer.expandInLegend));
                    obj.appendChild(baseMapLayer);
                }
                baseMapDefinition.appendChild(obj);
            }
            mapDefinition.appendChild(baseMapDefinition);
        }
    },
    numberOfLayers: function() {
        return this.mapLayers.length;
    },
    addFiniteScale: function(scale) {
        this.finiteDisplayScales.push(parseFloat(scale));
        this.finiteDisplayScales = this.finiteDisplayScales.sortBy(function(value,index){return value});
        this.setDirty();
    },
    setFiniteScales: function(fs) {
        this.finiteDisplayScales = fs;
        this.finiteDisplayScales = this.finiteDisplayScales.sortBy(function(value,index){return value});
        this.setDirty();
    },
    getFiniteScales: function() {
        return this.finiteDisplayScales;
    }
});

var WSWebLayout = Class.create();
Object.extend(WSWebLayout.prototype, WSBaseObj.prototype);
Object.extend(  WSWebLayout.prototype, {
    initialize : function( xml, app ) {
        this.baseInitialize( xml,app );
        this.ctype = 'WebLayout';
    },
    openView : function(domObj) {
        if (!this.view) {
            this.setClean();
            this.view = new WSWebLayoutView(this, domObj);
            this.requestContent();
            this.requestHeader();
            this.requestResourceReferences();
        }
    }
});
var WSFeatureSource = Class.create();
Object.extend(WSFeatureSource.prototype, WSBaseObj.prototype);
Object.extend(  WSFeatureSource.prototype, {
    initialize : function( xml, app ) {
        this.baseInitialize( xml,app );
        this.ctype = 'FeatureSource';
    },
    openView : function(domObj) {
        if (!this.view) {
            this.setClean();
            this.view = new WSFeatureSourceView(this, domObj);
            this.requestContent();
            this.requestHeader();
            this.requestResourceReferences();
        }
    },
    setProvider: function(szProvider) {
        //update object to reflect new data type and set other fields
        //appropriately
        //if there is no change of type, exit
        if (this.content.getNodeText('Provider').indexOf(szProvider) != -1) {
            return;
        }
        this.setDirty();
        //modify content based on provider
        var oParam = this.content.findFirstNode('Parameter');
        switch (szProvider){
            case 'SHP':
                //change file parameter
                oParam.setNodeText('Name', 'DefaultFileLocation');
                //remove ReadOnly parameter
                while (oParam) {
                    if (oParam.getNodeText('Name') == 'ReadOnly') {
                        this.content.removeChild(oParam);
                        break;
                    }
                    oParam = this.content.findNextNode('Parameter');
                }
                break;
            default://assume SDF
                oParam.setNodeText('Name', 'File');
                //test for readonly node and create it if needed
                var bReadOnly = false;
                while (oParam) {
                    if (oParam.getNodeText('Name') == 'ReadOnly') {
                        bReadOnly = true;
                        break;
                    }
                    oParam = this.content.findNextNode('Parameter');
                }
                if (!bReadOnly) {
                    var oNode = DomNodeFactory.create('Parameter');
                    var oChild = DomNodeFactory.create('Name', 'ReadOnly');
                    oNode.appendChild(oChild);
                    oChild = DomNodeFactory.create('Value', 'True');
                    oNode.appendChild(oChild);
                    this.content.appendChild(oNode);
                }                
        }
    },
    setFilePath: function(szFilePath) {

        var oParam = this.content.findFirstNode('Parameter');
        if (oParam.getNodeText('Name') == 'File') {
            var szPath = oParam.getNodeText('Value');
            if (szPath == szFilePath) {
                return;
            } else {
                //modify content
                oParam.setNodeText('Value', szFilePath);
                this.setDirty();
            }
        }
    }
    
});
var WSLayerDefinition = Class.create();
Object.extend(WSLayerDefinition.prototype, WSBaseObj.prototype);
Object.extend(  WSLayerDefinition.prototype, {
    _extents : {},
    spatialContexts : null,
    featureSchema : null,
    featureClasses : null,
    featureTypes : null,
    initialize : function( xml, app ) {
        //jxTrace('WSLayerDefinition::initialize()');
        this.baseInitialize( xml,app );
        this.ctype = 'LayerDefinition';
        this.featureClasses = [];
        this.featureTypes = [];
    },
    /**
     * overload base class function to update XML if the object
     * is dirty.  This serves both save and saveToSession
     */
    contentToXML : function() {
        if (this.isDirty()) {
            this.updateContent();
        }
        xml = this.content.toXML();
      //jxTrace(xml);
        return xml;
    },
    updateContent: function() {
        var vld = this.content.findFirstNode('VectorLayerDefinition');
        var vsr = vld.findFirstNode('VectorScaleRange');
        while(vsr) {
            vld.removeChild(vsr);
            vsr = vld.findFirstNode('VectorScaleRange');
        }
        for (var i=0; i<this.vectorScaleRanges.length; i++) {
            var newVSR = DomNodeFactory.create('VectorScaleRange');
            var vsr = this.vectorScaleRanges[i].toDomNode();
            for (var j=0; j<vsr.length; j++) {
                newVSR.appendChild(vsr[j]);
            }
            vld.appendChild(newVSR);
        }
    },
    openView : function(domObj) {
        //jxTrace('WSLayerDefinition::openView()');
        if (!this.view) {
            this.setClean();
            this.view = new WSLayerDefinitionView(this, domObj);
            this.requestContent();
            this.requestHeader();
            this.requestResourceReferences();
        }
    },
    requestSpatialContext : function() {
        //jxTrace('WSLayerDefinition::requestSpatialContext()');
        var r = new WSGetSpatialContexts(this.getContentValue('ResourceId'));
        var b = this.app.getBroker();
        b.dispatchRequest(r, this.spatialContextLoaded.bind(this));
    },
    requestFeatureSchema : function() {
        //jxTrace('WSLayerDefinition::requestFeatureSchema()');
        if (!this.view) {
            return;
        }
        this.view._oSettingsPanel.setBusy(true);
        var r = new WSDescribeSchema(this.getContentValue('ResourceId'));
        var b = this.app.getBroker();
        b.dispatchRequest(r, this.featureSchemaLoaded.bind(this));
    },
    contentLoaded : function(r) {
        //jxTrace('WSLayerDefinition::contentLoaded()');
        if (r.responseXML) {
            this.content = this.xhrToDomNode(r.responseXML);
            this.parsePropertyMappings();
            this.processEvent(this.cl, 'contentLoaded', this);    
            this.requestSpatialContext();
            this.requestFeatureSchema();
        }
    },
    spatialContextLoaded : function(r) {
        //jxTrace('WSLayerDefinition::spatialContextLoaded()');
        if (r.responseXML) {
            this.spatialContexts = this.xhrToDomNode(r.responseXML);
            this.processEvent(this.cl, 'spatialContextsLoaded', this);    
        }
    },
    featureSchemaLoaded : function(r) {
        //jxTrace('WSLayerDefinition::featureSchemaLoaded()');
        if (!this.view) {
            return;
        }
        this.view._oSettingsPanel.setBusy(false);
        if (r.responseXML) {
            this.featureSchema = this.xhrToDomNode(r.responseXML);
            this.parseFeatureSchema();
            this.processEvent(this.cl, 'featureSchemaLoaded', this);    
        }
    },
    getFilter: function() {
        var value = '';
        var node = this.content.findFirstNode('Filter');
        if (node) {
            value = node.textContent;
        }
        return value;
    },
    setFilter: function(filter) {
        var node = this.content.findFirstNode('Filter');
        if (filter == '') {
            if (node) {
                node.parentNode.removeChild(node);
            }
        } else {
            if (node) {
                node.textContent = filter;
            } else {
                node = DomNodeFactory.create('Filter', filter);
                var refNode = this.content.findFirstNode('PropertyMapping');
                if (!refNode) {
                    refNode = this.content.findFirstNode('Geometry');
                }
                refNode.parentNode.insertBefore(node, refNode);
            }
        }
        this.setDirty();
    },
    getUrl: function() {
        var value = '';
        var node = this.content.findFirstNode('Url');
        if (node) {
            value = node.textContent;
        }
        return value;
    },
    setUrl: function(url) {
        var node = this.content.findFirstNode('Url');
        if (url == '') {
            if (node) {
                node.parentNode.removeChild(node);
            }
        } else {
            if (node) {
                node.textContent = url;
            } else {
                node = DomNodeFactory.create('Url', url);
                var refNode = this.content.findFirstNode('Tooltip');
                if (!refNode) {
                    refNode = this.content.findFirstNode('VectorScaleRange');
                }
                refNode.parentNode.insertBefore(node, refNode);
            }
        }
        this.setDirty();
    },
    getToolTip: function() {
        var value = '';
        var node = this.content.findFirstNode('ToolTip');
        if (node) {
            value = node.textContent;
        }
        return value;
    },
    setToolTip: function(tooltip) {
        var node = this.content.findFirstNode('ToolTip');
        if (tooltip == '') {
            if (node) {
                node.parentNode.removeChild(node);
            }
        } else {
            if (node) {
                node.textContent = tooltip;
            } else {
                node = DomNodeFactory.create('ToolTip', tooltip);
                var refNode = this.content.findFirstNode('VectorScaleRange');
                refNode.parentNode.insertBefore(node, refNode);
            }
        }
        this.setDirty();
    },
    getGeometricType: function() {
        var geometricType = '';
        
        var featureClass = this.getContentValue('FeatureName');
        var typeName = '';
        for (var i=0; i<this.featureClasses.length; i++) {
            var key = this.featureClasses[i].schema + ':' + this.featureClasses[i].name;
            if (key == featureClass) {
                typeName = this.featureClasses[i].type;
                break;
            }
        }
        var featureType = this.featureTypes[typeName];
        var geomType = featureType.geometryNode.attributes['fdo:geometricTypes'];
        if (geomType == null) {
            geomType = featureType.geometryNode.attributes['geometrictypes'];
        }
        if (geomType.indexOf('surface') != -1) {
            geometricType = 'surface';
        } else if (geomType.indexOf('curve') != -1) {
            geometricType = 'curve';
        } else if (geomType.indexOf('point') != -1) {
            geometricType = 'point';
        }
        return geometricType;
    },
    parsePropertyMappings : function() {
        //jxTrace('WSLayerDefinition::parsePropertyMappings()');
        this.propertyMappings = [];
        var mapping = this.content.findFirstNode('PropertyMapping');
        while(mapping) {
            var o = {};
            o.name = mapping.getNodeText('Name');
            o.value = mapping.getNodeText('Value');
            this.propertyMappings.push(o);
            mapping = this.content.findNextNode('PropertyMapping');
        }
    },
    setPropertyMapping: function(name, value) {
        //jxTrace('WSLayerDefinition::setPropertyMapping()');
        var bFound = false;
        for (var i=0; i<this.propertyMappings.length; i++) {
            if (this.propertyMappings[i].name == name) {
                this.propertyMappings[i].value = value;
                var mapping = this.content.findFirstNode('PropertyMapping');
                while(mapping) {
                    var mappingName = mapping.getNodeText('Name');
                    if (mappingName == name) {
                        mapping.setNodeText('Value', value);
                        break;
                    }
                    mapping = this.content.findNextNode('PropertyMapping');
                }
                bFound = true;
            }
        }
        if (!bFound) {
            this.propertyMappings.push({name:name, value:value});
            var newMapping = new DomNode();
            newMapping.nodeName = 'PropertyMapping';
            var mappingName = new DomNode();
            mappingName.nodeName = 'Name';
            mappingName.textContent = name;
            var mappingValue = new DomNode();
            mappingValue.nodeName = 'Value';
            mappingValue.textContent = value;
            newMapping.appendChild(mappingName);
            newMapping.appendChild(mappingValue);
            /* the schema requires mappings to appear before Geometry */
            var node = this.content.findFirstNode('Geometry');
            node.parentNode.insertBefore(newMapping, node);
        }
        this.setDirty();
    },
    removePropertyMapping: function(name) {
        //jxTrace('WSLayerDefinition::removePropertyMapping()');
        for (var i=0; i<this.propertyMappings.length; i++) {
            if (this.propertyMappings[i].name == name) {
                this.propertyMappings.splice(i,1);
                this.setDirty();
                var mapping = this.content.findFirstNode('PropertyMapping');
                while(mapping) {
                    var mappingName = mapping.getNodeText('Name');
                    if (mappingName == name) {
                        mapping.parentNode.removeChild(mapping);
                        break;
                    }
                    mapping = this.content.findNextNode('PropertyMapping');
                }
                
                break;
            }
        }
    },
    
    getPropertyMapping: function(name) {
        //jxTrace('WSLayerDefinition::getPropertyMapping()');
        for (var i=0; i<this.propertyMappings.length; i++) {
            if (this.propertyMappings[i].name == name) {
                return this.propertyMappings[i];
            }
        }
        return false;
    },
    parseFeatureSchema: function() {
        //jxTrace('WSLayerDefinition::parseFeatureSchema()');
        //TODO: this only looks at the first schema
        var root = this.featureSchema.findFirstNode('xs:schema');
        //loop through child nodes of the schema looking for
        //elements and complex types
        this.featureClasses = [];
        this.featureTypes = [];
        for (var i=0; i<root.childNodes.length; i++) {
            var node = root.childNodes[i];
            if (node.nodeName == 'xs:element') {
                this.addFeatureClass(node);
            } else if (node.nodeName == 'xs:complexType'){
                this.addFeatureType(node);
            }
        }
        /* if no feature name is set, it is because this is a new layer so we need to set it */
        if (this.getContentValue('FeatureName') == '' && this.featureClasses.length > 0) {
            var featureName = this.featureClasses[0].schema + ':' + this.featureClasses[0].name
            this.setContentValue('FeatureName', featureName);

            var geomName = this.featureTypes[this.featureClasses[0].type].geometryName;
            this.setContentValue('Geometry', geomName);

            var vectorScaleRange = new DomNode();
            vectorScaleRange.nodeName = 'VectorScaleRange';
            /* now set up a default scale range for the layer to make it valid xml */
            var geomType = this.featureTypes[this.featureClasses[0].type].geometryNode.attributes['fdo:geometricTypes'];

            /* create a default representation depending on type */
            if (geomType.indexOf('surface') != -1) {
                var areaTypeStyle = DomNodeFactory.create('AreaTypeStyle');
                var areaRule = DomNodeFactory.create('AreaRule');
                areaRule.appendChild(DomNodeFactory.create('LegendLabel'));
                var areaSymbolization2d = DomNodeFactory.create('AreaSymbolization2D');
                areaSymbolization2d.appendChild(this.createDefaultFill());
                areaSymbolization2d.appendChild(this.createDefaultStroke());
                areaRule.appendChild(areaSymbolization2d);
                areaTypeStyle.appendChild(areaRule);
                vectorScaleRange.appendChild(areaTypeStyle);
            } else if (geomType.indexOf('curve') != -1) {
                var lineTypeStyle = DomNodeFactory.create('LineTypeStyle');
                var lineRule = DomNodeFactory.create('LineRule');
                lineRule.appendChild(DomNodeFactory.create('LegendLabel'));
                var lineSymbolization2d = this.createDefaultStroke();
                lineSymbolization2d.nodeName = 'LineSymbolization2D';
                lineRule.appendChild(lineSymbolization2d);
                lineTypeStyle.appendChild(lineRule);
                vectorScaleRange.appendChild(lineTypeStyle);
            } else if (geomType.indexOf('point') != -1) {
                var pointTypeStyle = DomNodeFactory.create('PointTypeStyle');
                pointTypeStyle.appendChild(DomNodeFactory.create('DisplayAsText','false'));
                pointTypeStyle.appendChild(DomNodeFactory.create('AllowOverpost','false'));
                var pointRule = DomNodeFactory.create('PointRule');
                pointRule.appendChild(DomNodeFactory.create('LegendLabel'));
                var pointSymbolization2d =DomNodeFactory.create('PointSymbolization2D');
                pointSymbolization2d.appendChild(this.createDefaultMark());
                pointRule.appendChild(pointSymbolization2d);
                pointTypeStyle.appendChild(pointRule);
                vectorScaleRange.appendChild(pointTypeStyle);
            } else {
                alert('yikes! Unsupported geometry type - ' + geomType);
            }
            
            var node = this.content.findFirstNode('VectorLayerDefinition');
            node.appendChild(vectorScaleRange);
        }
        this.parseVectorScaleRanges();
    },
    createDefaultFill: function() {
        var fill = DomNodeFactory.create('Fill');
        fill.appendChild(DomNodeFactory.create('FillPattern','Solid'));
        fill.appendChild(DomNodeFactory.create('ForegroundColor','FFFFFFFF'));
        fill.appendChild(DomNodeFactory.create('BackgroundColor','FF000000'));
        return fill;
    },
    createDefaultStroke: function() {
        var stroke = DomNodeFactory.create('Stroke');
        stroke.appendChild(DomNodeFactory.create('LineStyle','Solid'));
        stroke.appendChild(DomNodeFactory.create('Thickness','0'));
        stroke.appendChild(DomNodeFactory.create('Color','FF000000'));
        stroke.appendChild(DomNodeFactory.create('Unit','Inches'));
        stroke.appendChild(DomNodeFactory.create('SizeContext','DeviceUnits'));
        return stroke;
    },
    createDefaultMark: function() {
        var mark = DomNodeFactory.create('Mark');
        mark.appendChild(DomNodeFactory.create('Unit','Inches'));
        mark.appendChild(DomNodeFactory.create('SizeContext','DeviceUnits'));
        mark.appendChild(DomNodeFactory.create('SizeX','0.06'));
        mark.appendChild(DomNodeFactory.create('SizeY','0.06'));
        mark.appendChild(DomNodeFactory.create('Shape','Circle'));
        mark.appendChild(this.createDefaultFill());
        var edge = this.createDefaultStroke();
        edge.nodeName = 'Edge';
        mark.appendChild(edge);
        return mark;
    },
    addFeatureClass: function(node) {
        //jxTrace('WSLayerDefinition::addFeatureClass('+node.getAttribute('name')+')');
        var o = {};
        o.name = node.getAttribute('name');
        var type = node.getAttribute('type');
        o.schema = type.split(':')[0];
        o.type = type.split(':')[1];
        var fieldNode = node.findFirstNode('xs:field');
        o.field = fieldNode.getAttribute('xpath');
        this.featureClasses.push(o);
    },
    addFeatureType: function(node) {
        //jxTrace('WSLayerDefinition::addFeatureType('+node.getAttribute('name')+')');
        var o = {};
        o.name = node.getAttribute('name');
      //jxTrace('WSLayerDefinition::addFeatureType() - name is ' + o.name);
        
        var geomName = node.getAttribute('fdo:geometryName');
        if (geomName == null) {
            geomName = node.getAttribute('geometryname');
        }
        o.geometryName = geomName;

      //jxTrace('WSLayerDefinition::addFeatureType('+o.name+') geometryName is ' + o.geometryName);
        
        o.attributes = [];
        var seq = node.findFirstNode('xs:sequence');
        if (seq) {
            for (var i=0; i<seq.childNodes.length; i++) {
                var name = seq.childNodes[i].getAttribute('name');
                if (name != o.geometryName) {
                    o.attributes.push({name:name});
                    //jxTrace('added attribute ' + name );
                } else {
                    o.geometryNode = seq.childNodes[i];
                }
            }
        }
        this.featureTypes[o.name] = o;
    },
    getFeatureSchemaElements : function() {
        //jxTrace('WSLayerDefinition::getFeatureSchemaElements()');
        var result = [];
        var node = this.featureSchema.findFirstNode('xs:element');
        while(node) {
            //we only want xs:element that contain an xs:field
            var fieldNode = node.findFirstNode('xs:field');
            if (fieldNode) {
                var name = node.getAttribute('name');
                var type = node.getAttribute('type');
                var field = fieldNode.getAttribute('xpath');
                result.push({name:name, type:type, field:field});
                node = this.featureSchema.findNextNode('xs:element');
            } else {
                node = null; //stop processing
            }
        }
        return result;
    },
    getFeatureSchemaValue: function( name /*, attribute */) {
        //jxTrace('WSLayerDefinition::getFeatureSchemaValue('+name+')');
        var node = this.featureSchema.findFirstNode(name);
        if (arguments[1]) {
            return node.getAttribute( arguments[1] );
        } else {
            return node.textContent;
        }
    },
    getExtents: function() {
        //jxTrace('WSLayerDefinition::getExtents()');
        if (this.spatialContexts != null) {
            //TODO: handle multiple spatial contexts
            d = this.spatialContexts;
            
            var crsName = d.getNodeText('CoordinateSystemName');
            var crsWkt = d.getNodeText('CoordinateSystemWkt');
            var ll = d.findFirstNode('LowerLeftCoordinate');
            var minx = parseFloat(ll.findFirstNode('X').textContent);
            var miny = parseFloat(ll.findFirstNode('Y').textContent);
            var ur = d.findFirstNode('UpperRightCoordinate');
            var maxx = parseFloat(ur.findFirstNode('X').textContent);
            var maxy = parseFloat(ur.findFirstNode('Y').textContent);
            return {'crsName' : crsName,
                    'crsWkt' : crsWkt,
                    'minx' : minx,
                    'miny' : miny,
                    'maxx' : maxx,
                    'maxy' : maxy};
        }
    },
    setFeatureSourceId: function( rid ) {
        this.setDirty();
        
        //unset feature name and geometry
        this.setContentValue('FeatureName', '');
        this.setContentValue('Geometry', '');
        this.featureClasses = [];
        this.featureTypes = [];
        
        //remove property mappings
        var mapping = this.content.findFirstNode('PropertyMapping');
        while(mapping) {
            mapping.parentNode.removeChild(mapping);
            mapping = this.content.findFirstNode('PropertyMapping');
        }
        //remove vector scale ranges
        var range = this.content.findFirstNode('VectorScaleRange');
        while(range) {
            range.parentNode.removeChild(range);
            range = this.content.findFirstNode('VectorScaleRange');
        }
        this.vectorScaleRanges = [];
        
        this.setContentValue('ResourceId', rid);
        //update the view
        this.processEvent(this.cl, 'contentLoaded', this);    
        this.requestSpatialContext();
        this.requestFeatureSchema();
    },
    parseVectorScaleRanges: function() {
        this.vectorScaleRanges = [];
        var vsr = this.content.findFirstNode('VectorScaleRange');
        while(vsr) {
            var newVSR = new WSVectorScaleRangeType(vsr);
            newVSR.parent = this;
            this.vectorScaleRanges.push(newVSR);
            vsr = this.content.findNextNode('VectorScaleRange');
        }
    }
});
      
var WSBaseType = Class.create();
WSBaseType.prototype = {
    keys: null,
    values: null,        
    initialize: function(node) {
        this.initializeBase();
        if (node) {
            this.parse(node);
        }
    },
    initializeBase: function() {
        //jxTrace( 'WSBaseType::initializeBase()' );
        this.keys = [];
        this.values = {};
    },
    clone: function() {
        var newType = new WSBaseType();
        newType.keys = [];
        newType.values = {};
        for (var i=0; i<this.keys.length; i++) {
            newType.keys[i] = Object.extend({}, this.keys[i]);
            var key = newType.keys[i].name;
            var value;
            if (newType.keys[i].multiple) {
                value = [];
                for (var j=0; j<this.values[key].length; j++) {
                    if (newType.keys[i].creator && this.values[key][j] != '') {
                        value.push(this.values[key][j].clone());
                    } else {
                        value.push(this.values[key][j]);
                    }
                }
            } else {
                if (newType.keys[i].creator && this.values[key] && this.values[key] != '') {
                    value = this.values[key].clone();
                } else {
                    value = this.values[key];
                }
            }
            newType.set(key, value);
        }
        return newType;
    },
    parse: function(node) {
        //jxTrace('WSBaseType::parse()');
        if(!node) {
            return;
        }
        for (var i=0; i<this.keys.length; i++) {
            var name = this.keys[i].name;
            var creator = this.keys[i].creator;
            if (this.keys[i].multiple) {
                var children = [];
                var child = node.findFirstNode(name);
                while(child) {
                    if (creator) {
                        children.push(new creator(child));
                    } else {
                        children.push(child.textContent);
                    }
                    child = node.findNextNode(name);
                }
                this.set(name, children);
            } else {
                var child = node.findFirstNode(name);
                if (child && creator) {
                    this.set(name, new creator(child));
                } else {
                    this.set(name, child ? child.textContent : '');
                }
            }
        }
    },
    toDomNode: function() {
        var nodes = [];
        for (var i=0; i<this.keys.length; i++) {
            if (typeof(this.values[this.keys[i].name]) != 'undefined') {
                var key = this.keys[i].name;
                var value = this.values[key];
                if (value.toDomNode) {
                    var newNode = DomNodeFactory.create(key);
                    var children = value.toDomNode();
                    for (var j=0; j<children.length; j++) {
                        newNode.appendChild(children[j]);
                    }
                    nodes.push(newNode);
                } else if (typeof(value) == 'object') {
                    for (var j=0; j<value.length; j++) {
                        var newNode = DomNodeFactory.create(key);
                        var children = value[j].toDomNode();
                        for (var k=0; k<children.length; k++) {
                            newNode.appendChild(children[k]);
                        }
                        nodes.push(newNode);
                    }
                } else if (this.keys[i].mandatory) {
                    nodes.push(DomNodeFactory.create(key, value));
                } else if (value != ''){
                    nodes.push(DomNodeFactory.create(key, value));
                }
            } else if (this.keys[i].mandatory) {
                nodes.push(DomNodeFactory.create(key, ''));
            }
        }
        return nodes;
    },
    set: function(key, value) {
        //jxTrace('WSBaseType::set(' + key + ',' + value+')');
        var keyIsValid = false;
        for (var i=0; i<this.keys.length; i++) {
            if (this.keys[i].name == key) {
                keyIsValid = true;
                break;
            }
        }
        if (keyIsValid) {
            this.values[key] = value;
            if (typeof(value) == 'object') {
                if (value.toDomNode) {
                    value.parent = this;
                } else {
                    for (var i=0; i<value.length; i++) {
                        value[i].parent = this;
                    }
                }
            }
            this.setDirty();
        }
    },
    get: function(key) {
        //jxTrace( 'WSBaseType::get('+key+')');
        return this.values[key] ? this.values[key] : '';
    },
    setDirty: function() {
        if (this.parent && this.parent.setDirty) {
            this.parent.setDirty();
        }
    }
};

var WSVectorScaleRangeType = Class.create();
Object.extend(WSVectorScaleRangeType.prototype, WSBaseType.prototype);
Object.extend(WSVectorScaleRangeType.prototype, {
    initialize: function(node) {
        this.initializeVectorScaleRange();
        this.parse(node);
    },
    initializeVectorScaleRange: function() {
        //jxTrace('WSVectorScaleRangeType::initializeVectorScaleRange()');
        this.initializeBase();
        this.keys = this.keys.concat([{name: 'MinScale',mandatory: false, multiple: false},
                                      {name: 'MaxScale',mandatory: false, multiple: false},
                                      {name: 'AreaTypeStyle',mandatory: false, multiple: true, creator: WSAreaTypeStyleType},
                                      {name: 'LineTypeStyle',mandatory: false, multiple: true, creator: WSLineTypeStyleType},
                                      {name: 'PointTypeStyle',mandatory: false, multiple: true, creator: WSPointTypeStyleType}
                                      ]);
    }
});

var WSAreaTypeStyleType = Class.create();
Object.extend(WSAreaTypeStyleType.prototype, WSBaseType.prototype);
Object.extend(WSAreaTypeStyleType.prototype, {
    initialize: function(node) {
        this.initializeAreaTypeStyle();
        this.parse(node);
    },
    initializeAreaTypeStyle: function() {
        //jxTrace('WSAreaTypeStyleType::initializeAreaTypeStyle()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'AreaRule', mandatory: true, multiple: true, creator: WSAreaRuleType}]);
    }
});

var WSAreaRuleType = Class.create();
Object.extend(WSAreaRuleType.prototype, WSBaseType.prototype);
Object.extend(WSAreaRuleType.prototype, {
    initialize: function(node) {
        this.initializeAreaRuleType();
        this.parse(node);
    },
    initializeAreaRuleType: function() {
        //jxTrace('WSAreaRuleType::initializeAreaRuleType()');
        this.initializeBase();
        this.keys = this.keys.concat([{name: 'LegendLabel', mandatory: true, multiple: false},
                                      {name: 'Filter', mandatory: false, multiple: false},
                                      {name: 'Label', mandatory: false, multiple: false, creator: WSTextSymbolType},
                                      {name: 'AreaSymbolization2D', mandatory: true, multiple: false, creator: WSAreaSymbolizationFillType}
                                      ]);
    }
});
                                             
/* this is here because the schema for LayerDefintion has it */
var WSAreaSymbolizationType = Class.create();
Object.extend(WSAreaSymbolizationType.prototype, WSBaseType.prototype);
Object.extend(WSAreaSymbolizationType.prototype, {
    initialize: function(node) {
        this.initializeAreaSymbolization();
        this.parse(node);
    },
    initializeAreaSymbolization: function() {
        //jxTrace('WSAreaSymbolizationType::initializeAreaSymbolization()');
        this.initializeBase();
    }
});   

var WSAreaSymbolizationFillType = Class.create();
Object.extend(WSAreaSymbolizationFillType.prototype, WSAreaSymbolizationType.prototype);
Object.extend(WSAreaSymbolizationFillType.prototype, {
    initialize: function(node) {
        this.initializeAreaSymbolizationFill();
        this.parse(node);
    },
    initializeAreaSymbolizationFill: function() {
        //jxTrace('WSAreaSymbolizationFillType::initializeAreaSymbolizationFill()');
        this.initializeAreaSymbolization();
        this.keys = this.keys.concat([{name: 'Fill', mandatory: false, multiple: false, creator: WSFillType},
                                      {name: 'Stroke', mandatory: false, multiple: false, creator: WSStrokeType}]);
    }
});

var WSLineTypeStyleType = Class.create();
Object.extend(WSLineTypeStyleType.prototype, WSBaseType.prototype);
Object.extend(WSLineTypeStyleType.prototype, {
    initialize: function(node) {
        this.initializeLineTypeStyle();
        this.parse(node);
    },
    initializeLineTypeStyle: function() {
        //jxTrace('WSLineTypeStyleType::initializeLineTypeStyle()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'LineRule', mandatory: true, multiple: true, creator: WSLineRuleType}]);
    }
});

var WSLineRuleType = Class.create();
Object.extend(WSLineRuleType.prototype, WSBaseType.prototype);
Object.extend(WSLineRuleType.prototype, {
    initialize: function(node) {
        this.initializeLineRule();
        this.parse(node);
    },
    initializeLineRule: function() {
        //jxTrace('WSLineRuleType::initializeRule()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'LegendLabel', mandatory: true, multiple: false},
                                      {name:'Filter',  mandatory: false, multiple: false},
                                      {name:'Label',  mandatory: false, multiple: false, creator: WSTextSymbolType},
                                      {name:'LineSymbolization2D', mandatory: true, multiple: true, creator: WSStrokeType}
                                      ]);
    }
});
                                             
var WSPointTypeStyleType = Class.create();
Object.extend(WSPointTypeStyleType.prototype, WSBaseType.prototype);
Object.extend(WSPointTypeStyleType.prototype, {
    initialize: function(node) {
        this.initializePointTypeStyle();
        this.parse(node);
    },
    initializePointTypeStyle: function() {
        //jxTrace('WSPointTypeStyleType::initializePointTypeStyle()');
        this.initializeBase();
        this.keys = this.keys.concat([{name: 'DisplayAsText', mandatory: true, multiple: false},
                                      {name: 'AllowOverpost', mandatory: true, multiple: false},
                                      {name: 'PointRule',  mandatory: true, multiple: true, creator: WSPointRuleType}
                                      ]);
    }
});   

var WSPointRuleType = Class.create();
Object.extend(WSPointRuleType.prototype, WSBaseType.prototype);
Object.extend(WSPointRuleType.prototype, {
    initialize: function(node) {
        this.initializePointRule();
        this.parse(node);
    },
    initializePointRule: function() {
        //jxTrace('WSPointRuleType::initializePointRule()');
        this.initializeBase();
        this.keys = this.keys.concat([{name: 'LegendLabel', mandatory: true, multiple: false},
                                      {name: 'Filter',  mandatory: false, multiple: false},
                                      {name: 'Label',  mandatory: false, multiple: false, creator: WSTextSymbolType},
                                      {name: 'PointSymbolization2D', mandatory: false, multiple: false, creator: WSPointSymbolization2DType}
                                      ]);
    }
});

var WSPointSymbolization2DType = Class.create();
Object.extend(WSPointSymbolization2DType.prototype, WSBaseType.prototype);
Object.extend(WSPointSymbolization2DType.prototype, {
    initialize: function(node) {
        this.initializePointSymbolization2D();
        this.parse(node);
    },
    initializePointSymbolization2D: function() {
        //jxTrace('WSPointSymbolization2DType::initializePointSymbolization2D()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'Mark', mandatory: false, multiple: false, creator: WSMarkSymbolType},
                                      {name:'Image', mandatory: false, multiple: false, creator: WSImageSymbolType},
                                      {name:'Font', mandatory: false, multiple: false, creator: WSFontSymbolType},
                                      {name:'W2D', mandatory: false, multiple: false, creator: WSW2DSymbolType},
                                      {name:'Block',mandatory: false, multiple: false, creator: WSBlockSymbolType}
                                      ]);
    }
});

var WSSymbolType = Class.create();    
Object.extend(WSSymbolType.prototype, WSBaseType.prototype);
Object.extend(WSSymbolType.prototype, {
    initialize: function(node) {
        this.initializeSymbol();
        this.parse(node);
    },
    initializeSymbol: function() {
        //jxTrace('WSSymbolType::initializeSymbol()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'Unit', mandatory: true, multiple: false},
                                      {name:'SizeContext', mandatory: true, multiple: false},
                                      {name:'SizeX', mandatory: true, multiple: false},
                                      {name:'SizeY', mandatory: true, multiple: false},
                                      {name:'Rotation', mandatory: false, multiple: false},
                                      {name:'MaintainAspect', mandatory: false, multiple: false},
                                      {name:'InsertionPointX', mandatory: false, multiple: false},
                                      {name:'InsertionPointY', mandatory: false, multiple: false}
                                      ]);
    }
});     

var WSMarkSymbolType = Class.create();
Object.extend(WSMarkSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSMarkSymbolType.prototype, {
    initialize: function(node) {
        this.initializeMarkSymbol();
        this.parse(node);
    },
    initializeMarkSymbol: function() {
        //jxTrace('WSMarkSymbolType::initializeMarkSymbol()');
        this.initializeSymbol();
        this.keys = this.keys.concat([{name:'Shape', mandatory: true, multiple: false},
                                      {name:'Fill', mandatory: false, multiple: false, creator: WSFillType},
                                      {name:'Edge', mandatory: false, multiple: false, creator: WSStrokeType}
                                      ]);
    }
});

var WSImageSymbolType = Class.create();
Object.extend(WSImageSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSImageSymbolType.prototype, {
    initialize: function(node) {
        this.initializeImageSymbol();
        this.parse(node);
    },
    initializeImageSymbol: function() {
        //jxTrace('WSImageSymbolType::initializeImageSymbol()');
        this.initializeSymbol();
        this.keys = this.keys.concat([{name:'Image', mandatory: false, multiple: false, creator: WSImage},
                                      {name:'Content', mandatory: false, multiple: false}
                                      ]);
    }
});

var WSImage = Class.create();
Object.extend(WSImage.prototype, WSBaseType.prototype);
Object.extend(WSImage.prototype, {
    initialize: function(node) {
        this.initializeImage();
        this.parse(node);
    },
    initializeImage: function() {
        //jxTrace('WSImage::initializeImage()');
        this.initializeBase();
        this.keys = this.keys.concat([{name:'ResourceId', mandatory: true, multiple: false},
                                      {name:'LibraryItemName', mandatory: true, multiple: false}
                                      ]);
    }
});

var WSFontSymbolType = Class.create();
Object.extend(WSFontSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSFontSymbolType.prototype, {
    initialize: function(node) {
        this.initializeFontSymbol();
        this.parse(node);
    },
    initializeFontSymbol: function() {
        //jxTrace('WSFontSymbolType::initializeFontSymbol()');
        this.initializeSymbol();      
        this.keys = this.keys.concat([{name:'FontName', mandatory: true, multiple: false},
                                      {name:'Character', mandatory: false, multiple: false},
                                      {name:'Bold', mandatory: false, multiple: false},
                                      {name:'Italic', mandatory: false, multiple: false},
                                      {name:'Underlined', mandatory: false, multiple: false},
                                      {name:'ForegroundColor', mandatory: false, multiple: false}
                                      ]);
    }
});

var WSW2DSymbolType = Class.create();
Object.extend(WSW2DSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSW2DSymbolType.prototype, {
    initialize: function(node) {
        this.initializeW2DSymbol();
        this.parse(node);
    },
    initializeW2DSymbol: function() {
        //jxTrace('WSW2DSymbolType::initializeW2DSymbol()');
        this.initializeSymbol();
        this.keys = this.keys.concat([{name:'W2DSymbol', mandatory: true, multiple: false, creator: WSW2D},
                                      {name:'Bold', mandatory: false, multiple: false},
                                      {name:'Italic', mandatory: false, multiple: false},
                                      {name:'Underlined', mandatory: false, multiple: false},
                                      {name:'ForegroundColor', mandatory: false, multiple: false}
                                      ]);
    }
});

var WSW2D = Class.create();
Object.extend(WSW2D.prototype, WSBaseType.prototype);
Object.extend(WSW2D.prototype, {
    initialize: function(node) {
        this.initializeW2D();
        this.parse(node);
    },
    initializeW2D: function() {
        this.initializeBase();
        this.keys = this.keys.concat([{name:'ResourceId', mandatory: true, multiple: false},
                                      {name:'LibraryItemName', mandatory: true, multiple: false}
                                      ]);
    }
});

var WSBlockSymbolType = Class.create();
Object.extend(WSBlockSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSBlockSymbolType.prototype, {
    initialize: function(node) {
        this.initializeBlockSymbol();
        this.parse(node);
    },
    initializeBlockSymbol: function() {
        this.initializeSymbol();
        this.keys = this.keys.concat([{name:'DrawingName', mandatory: true, multiple: false},
                                      {name:'BlockName', mandatory: true, multiple: false},
                                      {name:'BlockColor', mandatory: false, multiple: false},
                                      {name:'LayerColor', mandatory: false, multiple: false}
                                      ]);
    }
});

var WSTextSymbolType = Class.create();
Object.extend(WSTextSymbolType.prototype, WSSymbolType.prototype);
Object.extend(WSTextSymbolType.prototype, {
    initialize: function(node) {
        this.initializeTextSymbol();
        this.parse(node);
    },
    initializeTextSymbol: function() {
        this.initializeSymbol(); 
        this.keys = this.keys.concat([{name: 'Text', mandatory: true, multiple: false},
                                      {name: 'FontName', mandatory: true, multiple: false},
                                      {name: 'ForegroundColor', mandatory: true, multiple: false},
                                      {name: 'BackgroundColor', mandatory: true, multiple: false},
                                      {name: 'BackgroundStyle', mandatory: true, multiple: false},
                                      {name: 'HorizontalAlignment', mandatory: false, multiple: false},
                                      {name: 'VerticalAlignment', mandatory: false, multiple: false},
                                      {name: 'Bold', mandatory: false, multiple: false},
                                      {name: 'Italic', mandatory: false, multiple: false},
                                      {name: 'Underlined', mandatory: false, multiple: false},
                                      {name: 'AdvancedPlacement', mandatory: false, multiple: false},
                                      {name: 'ScaleLimit', mandatory: false, multiple: false}
                                      ]);
    }
});

var WSFillType = Class.create();
Object.extend(WSFillType.prototype, WSBaseType.prototype);
Object.extend(WSFillType.prototype, {
    initialize: function(node) {
        this.initializeFill();
        this.parse(node);
    },
    initializeFill: function() {
        this.initializeBase();
        this.keys = this.keys.concat([{name:'FillPattern', mandatory: true, multiple: false},
                                      {name:'ForegroundColor', mandatory: true, multiple: false},
                                      {name:'BackgroundColor', mandatory: true, multiple: false}
                                      ]);
    }
});

var WSStrokeType = Class.create();
Object.extend(WSStrokeType.prototype, WSBaseType.prototype);
Object.extend(WSStrokeType.prototype, {
    initialize: function(node) {
        this.initializeStroke();
        this.parse(node);
    },
    initializeStroke: function() {
        this.initializeBase();
        this.keys = this.keys.concat([{name:'LineStyle', mandatory: true, multiple: false},
                                      {name:'Thickness', mandatory: true, multiple: false},
                                      {name:'Color', mandatory: true, multiple: false},
                                      {name:'Unit', mandatory: true, multiple: false},
                                      {name:'SizeContext', mandatory: true, multiple: false}
                                      ]);
    }
});

var WSHtmlPage = Class.create();
Object.extend(WSHtmlPage.prototype, WSBaseObj.prototype);
Object.extend(  WSHtmlPage.prototype, {
    initialize : function( title, url, app ) {
        this.cl = [];
        this.app = app;
        this.url = url;
        this.rid = url;
        this._path = [url];
        this.ctype = 'HtmlPage';
        this.name = title;
    },
    parseResourceID: function() {},
    openView: function(domObj) {
        if (!this.view) {
            this.view = new WSHtmlView(this, domObj);
            var ts = 'ts=' + (new Date()).getTime();
            var a = new Ajax.Request( this.url, 
                Object.extend({method:'get',onComplete:this.contentLoaded.bind(this),
                parameters: ts}));     
        }
    },
    contentLoaded : function(r) { 
        if (!this.view) {
            return;
        }
        
        (this.view.getPanel(0)).setContent(r.responseText); 
    }
});

var WSIframePage = Class.create();
Object.extend(WSIframePage.prototype, WSBaseObj.prototype);
Object.extend(  WSIframePage.prototype, {
    onloadCallback : null,
    initialize : function( o ) {
        this.cl = [];
        this.app = o.app;
        this.ctype = 'IframePage';
        this.name = o.title;
        this.url = o.url;
        this.rid = o.url;
        this._path = [o.url];
        if (o.onloadCallback) {
            this.onloadCallback = o.onloadCallback;
        }
    },
    
    openView: function(domObj) {
        if (!this.view) {
            this.view = new WSIframeView(this, domObj, this.viewOpened.bind(this));
            this.view.setIframeURL(this.url);
        } else {
            this.viewOpened();
        }
    },
    viewOpened: function() {
        if (this.onloadCallback) {
            this.onloadCallback();
        }
    }
});

var WSUnsupported = Class.create();
Object.extend(WSUnsupported.prototype, WSBaseObj.prototype);
Object.extend(  WSUnsupported.prototype, {
    initialize : function( xml, app ) {
        this.baseInitialize(xml,app);
        this.cl = [];
        this.app = app;
    },
    openView: function(domObj) {
        if (!this.view) {
            this.setClean();
            this.view = new WSUnsupportedView(this, domObj);
            this.requestHeader();
            this.requestResourceReferences();
        }
    }
});
