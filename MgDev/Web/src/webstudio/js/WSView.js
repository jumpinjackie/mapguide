/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSView.js,v 1.134 2006/06/16 20:47:55 zak Exp $
 * @fileoverview    this file contains classes that manage the view of an object
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

/**
 * WSResourceManager manages open resources.  It emits selectionChanged
 * events when the user changes the selection in its list.
 */
var WSResourceManager = Class.create();
Object.extend(WSResourceManager.prototype, JxListenerBase.prototype);
Object.extend(WSResourceManager.prototype, {
    /** the instance of WSApplication, used primarily for broker requests to the server */
    app : null,
    
    /** the DOM element in which views are opened */
    viewPane : null,
    
    /** the DOM element in which the resource manager can list open resources */
    resourcePane : null,
    
    /** a UL element the resource manager uses to list open resource */
    domObj : null,
    
    /** an array of open resources being managed by the resource manager */
    resources : null,
    
    /** 
     * an array of recently accessed resources so when a resource is closed, the last
     * viewed resource can be opened
     */
    history : null,
    
    /** the current index in the history for future history navigation (forward/back) */
    historyIdx : null,
    
    /** the currently selected resource */
    currentResource : null,
    
    /** holds a WebLayout that MapDefinition views can use for previews */
    previewMapTemplate : null,
    
    /** holds a WebLayout that LayerDefinition views can use for previews */
    previewLayoutTemplate : null,
    
    /** used to track loading of WebLayout templates */
    requests : 0,
    
    /** an WSFileDialog instance to be used for SaveAs operations */
    fileDialog: null,

    /** an array of selection listeners (required by JxListenerBase) */
    sl : null,

    /** an array of resource listeners (required by JxListenerBase) */
    rl : null,

    /**
     * constructor for WSResourceManager.  This adds a new ul element 
     * to the resourcePane for the list of open resources and creates the
     * fileDialog instance for later use.
     *
     * @param resourcePane {DOM Object} a DOM Object to put the resource
     *        list in
     * @param viewPane {DOM Object} a DOM Object to put the current
     *        resource's view into
     */
    initialize : function(app,resourcePane,viewPane) {
        this.app = app;
        this.resources = [];
        this.history = [];
        this.historyIdx = -1;
        this.resourcePane = resourcePane;
        this.viewPane = viewPane;
        this.domObj = document.createElement('ul');
        this.domObj.className = 'jxList';
        this.getTemplateContent();
        this.resourcePane.appendChild(this.domObj);
        this.sl = [];
        this.rl = [];
        this.fileDialog = new WSFileDialog({app:this.app});
    },
    /**
     * add a resource to the list of open resources.  This checks to see
     * if the resource is already open and just shows it if it is.  If
     * it is not already open, the resource is added to the list and
     * made the current resource.
     *
     * @param resource {WSBaseObj} the resource to add
     */
    addResource : function (resource) {
        var rid = resource.getResourceID();
        var nRes = this.resources.length;
        var bAdded = null;
        for (var i = 0; i<nRes; i++) {
           if (this.resources[i].getResourceID() == rid) {
                bAdded = 'true';
                resource = this.resources[i];
                break;
           }
        }
        if (!bAdded){
            this.resources.push(resource);
            var li = document.createElement('li');
            var a = document.createElement('a');
            a.href = '#';
            Event.observe(a,'click',this.openResource.bind(this, resource));
            var span = document.createElement('span');
            span.innerHTML = resource.getName();
            a.appendChild(span);
            a.className = resource.getType();
            var img = document.createElement('img');
            img.className = 'resourceCloseImg png24';
            img.src = 'images/tree_close.png';
            img.alt = 'Close Resource';
            img.title = 'Close Resource';
            Event.observe(img,'click',this.removeResource.bind(this, resource));
            a.appendChild(img);
            li.appendChild(a);
            this.domObj.appendChild(li);
            resource.listObj = li;
        }
        resource.addContentListener(this);
        this.processEvent(this.rl, 'resourceAdded', resource);
        this.openResource(resource);
    },
    /**
     * remove an open resource.  If the resource has been changed, display a
     * dialog to confirm closing it.  If not, call an internal function to
     * remove the resource.
     *
     * @param resource {WSBaseObj} the resource to remove.
     */
    removeResource : function(resource) {
        //jxTrace('WSResourceManager::removeResource('+resource.getName()+')')
        /* 1. check to see if document needs saving */
        if (resource.isDirty()) {
            var size = Position.getPageDimensions();
            var o = {
                title: 'Confirm Close : ' + resource.getName() + ' ('+resource.getType()+')',
                id: 'confirmCloseResourceDialog',
                contentURL : 'dialogs/confirmcloseresource.html',
                width: 400,
                height: 100,
                top: (size.height-100)/2,
                left: (size.width-400)/2,
                handler: this.handleCloseResource.bind(this, resource),
                buttons: ['ok', 'cancel']
            };
            var confirmCloseResourceDialog = new JxDialog( o );
            confirmCloseResourceDialog.open();
        } else {
            this._removeResource(resource);
        }
    },
    /**
     * handle the user's selection from the confirm close resource dialog.
     * Only remove the resource if the user clicked 'ok'.
     *
     * @param resource {WSBaseObj} the resource to remove
     * @param button {String} the button that was clicked
     * @param dialog {WSFileDialog} the file dialog instance
     */
    handleCloseResource: function(resource, button, dialog) {
        dialog.close();
        if (button == 'ok') {
            this._removeResource(resource);
        }
    },
    /**
     * internal function to actually remove the resource.  This removes
     * it from the list of open resources, removes it from the history, 
     * and opens the last open resource.
     *
     * @param {WSBaseObj} the resource to remove
     */
    _removeResource: function(resource) {
        this.domObj.removeChild(resource.listObj);
        resource.closeView();
        resource.removeContentListener(this);
        this.removeResourceFromHistory(resource);
        var resourceIdx = this.findResourceIndex(resource);
        this.resources.splice(resourceIdx,1);
        if (this.currentResource == resource) {
            this.currentResource = null;
            if (this.history.length > 0) {
                this.openResource(this.history[this.historyIdx]);
            }
        }
        this.processEvent(this.rl, 'resourceRemoved', resource);
    },
    /**
     * move forwards in the history of open resources.  This
     * changes the history index and calls openResource.
     */
    nextResource : function() {
        if (this.historyIdx != this.history.length - 1) {
            this.historyIdx++;
            this.openResource(this.history[historyIdx]); 
        }
    },
    /**
     * move backwards in the history of open resources.  This
     * changes the history index and calls openResource.
     */
    previousResource : function() {
        if (this.historyIdx != 0) {
            this.historyIdx--;
            this.openResource(this.history[historyIdx]); 
        }
    },
    /**
     * remove a resource from the history array.  This removes
     * all copies and adjusts the history index
     * @param resource {WSBaseObj} the resource to remove
     */
    removeResourceFromHistory : function(resource) {
        //jxTrace('WSResourceManager::removeResourceFromHistory('+resource.getName()+')');
        var i=0;
        while(i<this.history.length) {
            if (this.history[i] == resource) {
                this.history.splice(i,1);
                this.historyIdx--;
            } else {
                i++;
            }
        }        
    },
    /**
     * return the index of a resource in the WSResourceManager's internal
     * list of open resources
     * @param resource {WSBaseObj} the resource to find
     * @return {Integer} the index of the resource or -1 if not found.
     */
    findResourceIndex : function(resource) {
      var nRes = this.resources.length;
        for (var i = 0; i<nRes; i++) {
          if (this.resources[i] == resource) {
              return i;
          }
      }
      return -1;
    },
    /**
     * test for an open resource.
     * @param rid {String} the resource to look for.
     * @return {Boolean} true if resource is open 
     */
    isResourceOpen : function(rid) {
        var nRes = this.resources.length;
        var bFound = false;
        for (var i = 0; i<nRes; i++) {
           if (this.resources[i].getResourceID() == rid) {
                bFound = true;
                break;
           }
        }
        return bFound;
    },
    /**
     * resize the view, giving the view's JxSnapManager a chance to
     * fix itself up
     */
    resize : function() {
        for (var i=0; i<this.resources.length; i++) {
            (this.resources[i].getView()).resize();
        }
    },
    /**
     * open the view for a resource object. This triggers the selectionChanged 
     * event.
     * 
     * @param resource {WSBaseObj} a resource being managed by the resource 
     *        manager.
     */
    openResource : function(resource) {
        //sanity check - don't need to do anything if the view is already open.
        if (this.currentResource == resource) {
            return;
        }
        resource.openView(this.viewPane);
        
        //if another resource is currently open, hide it and deselect it
        if (this.currentResource && this.currentResource != resource) {
            this.currentResource.hideView();
            Element.removeClassName(this.currentResource.listObj, 'selected');
        }
        //add the new resource view to the viewPane and set it as selected
        resource.showView(); 
        this.currentResource = resource;
        Element.addClassName(this.currentResource.listObj, 'selected');
        //make sure the JxSnapManager has a chance to resize its pane
        resource.getView().resize();
        //update the history if necessary
        if (this.historyIdx == this.history.length - 1) {
            this.history.push( this.currentResource );
            this.historyIdx = this.history.length - 1;
       }
       //finally emit a selectionChanged event so listeners can update
       this.processEvent(this.sl, 'selectionChanged', resource);
    },
    /**
     * When a modification occurs, change the appearance of the view
     * in the open resource list so the user knows it needs to be saved. 
     * 
     * @param resource {WSBaseobj} the resource to modify.
     */
    contentChanged : function(resource) {
        //set class of dirty object to show it is modified
        if (resource.isDirty()) {
          Element.addClassName(resource.listObj, 'modified');
        } else {
          Element.removeClassName(resource.listObj, 'modified');
        }
    },
    /**
     * check to see if the current resource needs to be saved
     *
     * @return true if it does, false otherwise
     */
    isCurrentResourceDirty : function() {
        return this.currentResource?this.currentResource.isDirty():false;
    },
    /**
     * check to see if any open resource needs to be saved
     *
     * @return true if any resource needs to be saved, false otherwise
     */
    isAnyResourceDirty : function() {
        var dirty = false;
        for (var i=0; i<this.resources.length; i++) {
            dirty = dirty || this.resources[i].isDirty();
        }
        return dirty;
    },
    /**
     * collect templates from server for previews by issuing AJAX requests
     * and tracking when they are loaded.
     */
    getTemplateContent : function () {
        this.requests = this.requests + 2;
        var ts = 'ts=' + (new Date()).getTime();
        
        new Ajax.Request( 'templates/layermap.templ', 
                         {method: 'get',
                          onSuccess : this.templateReady.bind(this),
                          parameters: ts});
        new Ajax.Request( 'templates/weblayout.templ', 
                         {method: 'get',
                          onSuccess : this.templateReady.bind(this),
                          parameters: ts});
    },

    /**
     * set templates callback function stores the appropriate preview templates
     *
     * @param r {Object} the xmlhttprequest response object
     */
    templateReady : function(r) {
        if (r.responseText) {
            if (r.responseText.indexOf('MapDefinition') != -1) {
                this.previewMapTemplate = r.responseText;
            } else {//if (r.responseText.indexOf('WebLayout') != -1)
                this.previewLayoutTemplate = r.responseText;
            };
        }
    },
    /**
     * save the current resource
     */
    saveCurrentResource : function() {
        if (this.currentResource) {
            this.currentResource.save();
        }
    },
    /**
     * save the current resource under a new name.  This opens the file
     * dialog to choose a location for saving.  This can be cancelled
     * by the user.
     */
    saveCurrentResourceAs : function() {
        var type = this.currentResource.getType();
        var path = this.currentResource.getPath();
        this.fileDialog.openSaveAs({handler:this.handleSaveAs.bind(this), types:this.fileDialog.getFilter(type),path:path});
    },
    /**
     * handle the user's choice from the save as dialog.
     *
     * @param button {String} the button the user clicked
     * @param dialog {WSFileDialog} the file dialog instance
     */
    handleSaveAs: function(button,dialog) {
        if (button == 'Save') {
            this.currentResource.setResourceID(dialog.getResource());
            this.currentResource.save(true);
            this.currentResource.listObj.childNodes[0].childNodes[0].innerHTML = this.currentResource.getName();
            this.app.dispatchRefreshSite();
        }
        dialog.close();
    },
    /**
     * save all open resources that have changed
     */
    saveAllResources : function() {
        for (var i=0; i<this.resources.length; i++) {
            this.resources[i].save();
        }
    },
    /**
     * close the active resource, if any
     */
    closeCurrentResource : function() {
        if (this.currentResource) {
            this.removeResource(this.currentResource);
        }
    },
    /**
     * close all open resources
     */
    closeAllResources: function() {
        for (var i=this.resources.length-1; i>=0; i--) {
            this.removeResource(this.resources[i]);
        }
    },
    /**
     * add a selection listener to the WSResourceManager.
     *
     * @param o {Object} an object that implements a selectionChanged method
     */
    addSelectionListener: function(o){this.addListener(this.sl, o);},
    /**
     * remove a previously registered selection listener.
     *
     * @param o {Object} an object that was previously added using addSelectionListener
     */
    removeSelectionListener: function(o) {this.removeListener(this.sl, o);},
    /**
     * add a resource listener to the WSResourceManager.
     *
     * @param o {Object} an object that implements a resourceChanged method
     */
    addResourceListener: function(o){this.addListener(this.rl, o);},
    /**
     * remove a previously registered resource listener.
     *
     * @param o {Object} an object that was previously added using addResourceListener
     */
    removeResourceListener: function(o) {this.removeListener(this.rl, o);}
} );

/**
 * WSView is the base class for views of objects.  It is not intended that
 * you create an instance of this class directly.  Instead, create an
 * instance of a subclass.  This class provides common utility functions
 * used by all subclasses.
 *
 * Due to the limitations of prototype, overloaded functions make superclass
 * functions of the same name unavailable in subclasses.  To allow
 * for some level of abstraction, a baseInit function is provided for 
 * subclasses to call from within initialize()
 */
var WSView = Class.create();
WSView.prototype = {
    snapManager : null,
    viewManager : null,
    obj : null,
    domObj : null,
    firstResize : null,
    panelStatus : null,
    closeButton : null,
    viewPane : null,
    /**
     * construct a new view object.  This should probably never
     * be called as all the work is done in baseInit
     *
     * @param o {WSBaseObj} the object that this is a view for
     * @param openIn {Object} the DOM element to put the view in
     */
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
    },
    /**
     * initialize the view.  All subclasses should
     * call this from their initialize function before
     * doing any per-class initialization
     */
    baseInit : function (o, openIn) {
        this.obj = o;
        o.addContentListener(this);
        
        this.viewPane = openIn;
        this.viewManager = o.app.getResourceManager();
        this.domObj = document.createElement('div');
        this.domObj.className = 'snapManagerContainer';
        
        this.closeButton = document.createElement('a');
        this.closeButton.className = 'snapClose';
        this.closeButton.href='#';
        Event.observe(this.closeButton, 'click', this.close.bind(this));
        
        var img = document.createElement('img');
        img.src = 'images/icon_close.png';
        img.alt = 'Close Resource';
        img.title = 'Close Resource';
        
        this.closeButton.appendChild(img);
        this.domObj.appendChild(this.closeButton);
        openIn.appendChild(this.domObj);
        
        this.snapManager = new JxSnapManager(this.domObj);
        this.firstResize = true;
        this.panelStatus = {};
    },
    /** 
     * destructing the view requires cleaning up closures in the
     * DOM (inputs with events bound to functions in this object)
     *
     * Subclasses should overload this function and call baseFinalize
     * once all the per-panel cleanup is done.
     */
    finalize: function() { this.baseFinalize(); },
    /**
     * this cleans up after the overall view stuff.  Call after
     * per-panel cleanup is complete
     */
    baseFinalize: function() {
        //jxTrace('WSView::baseFinalize()');
        this.obj = null;
        if (this.domObj.parentNode) {
            //jxTrace('WSView::baseFinalize - removing from DOM');
            this.domObj.parentNode.removeChild(this.domObj);
        }
        this.domObj = null;
        this.closeButton = null;
        this.panelStatus = null;
        this.snapManager.finalize();
        this.snapManager = null;
    },
    /**
     * show the view.  Changes the view's DOM object display to block
     */
    show: function() { this.domObj.style.display = 'block'; },
    /**
     * hide the view.  Changes the view's DOM object display to none
     */
    hide: function() { this.domObj.style.display = 'none'; },
    /**
     * close the view.  Defer to viewManager to remove the resource
     * associated with this view (which will eventually end up back
     * here calling finalize)
     */
    close: function() { this.viewManager.removeResource(this.obj); },
    /**
     * the container of this view has been resized so inform the
     * snap manager so that it can fill the available space
     */
    resize : function () {
        var size = Element.getContentBoxSize(this.viewPane);
        Element.setBorderBoxSize(this.domObj, {height: size.height});
        this.snapManager.resize();
        if (this.firstResize) {
            this.snapManager.distributeSpace();
            this.firstResize = false;
        }
    },
    getPanel: function( idx ) {
        if (idx >= 0 && idx < this.snapManager.panels.length) {
            return this.snapManager.panels[idx];
        }
        return null;
    },
    panelLoaded : function(name) {
        this.panelStatus[name] = true;
    },
    renderProperties : function() {
        this._oPropertiesPanel.getObj('featuresourceid').value = this.obj.getName();
        this._oPropertiesPanel.getObj('type').value = this.obj.getType();
        this._oPropertiesPanel.getObj('location').value = this.obj.getPath();
        this._oPropertiesPanel.getObj('owner').value = this.obj.getDomValue('Owner');
        this._oPropertiesPanel.getObj('created').value = this.obj.getDomValue('CreatedDate');
        this._oPropertiesPanel.getObj('modified').value = this.obj.getDomValue('ModifiedDate');
        //create the tab box
        var tabBar = new JxTabBar(this._oPropertiesPanel.getObj('references'));
        var tabReferencedTo = new JxTab('References', {contentURL: 'dialogs/referencedto.html', onContentReady: this.panelLoaded.bind(this, 'referencedto')});
        var tabReferencedBy = new JxTab('Referenced By', {contentURL: 'dialogs/referencedby.html', onContentReady: this.panelLoaded.bind(this, 'referencedby')});
        tabBar.addTab(tabReferencedTo, tabReferencedBy);
    },
    parseResourceId : function( resid) {
        var result = {};
        var idx = resid.lastIndexOf('/');
        result.path = resid.slice(0,idx+1);
        var pair = resid.slice(idx+1);
        idx = pair.lastIndexOf('.');
        result.name = pair.slice(0,idx);
        result.type = pair.slice(idx+1);
        return result;
    },
    renderResourceReferences: function(referencedTo, referencedBy) {
        var table = this._oPropertiesPanel.getObj('referencedto');
        for (var i=0; i<referencedTo.length; i++) {
            row = this.createResourceReferenceRow(referencedTo[i]);
            table.appendChild(row);
        }
        table = this._oPropertiesPanel.getObj('referencedby');
        for (var i=0; i<referencedBy.length; i++) {
            row = this.createResourceReferenceRow(referencedBy[i]);
            table.appendChild(row);
        }
    },
    createResourceReferenceRow: function( resourceId ) {
        ref = this.parseResourceId(resourceId);
        row = document.createElement('tr');
        cell = document.createElement('td');
        cell.innerHTML = ref.name;
        row.appendChild(cell);
        cell = document.createElement('td');
        cell.innerHTML = ref.type;
        row.appendChild(cell);
        cell = document.createElement('td');
        cell.innerHTML = ref.path;
        row.appendChild(cell);
        return row;
    },
    showHelpTopic: function(topic) {
        this.viewManager.app.showHelpTopic(topic);
    }
};
var WSLayerDefinitionView = Class.create();
Object.extend(WSLayerDefinitionView.prototype, WSView.prototype);
Object.extend(WSLayerDefinitionView.prototype, {
    previewDefinition : null,
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this.bSettingsRendered = false;
        this.bFeatureSchemaRendered = false;
        this.bStyleRendered = false;
        this.bPropertiesRendered = false;
        this.bResourceReferencesRendered = false;
        
        this._oPropertiesPanel = new JxSnapPanel({label:'Properties - ' + o.getName() + ' ('+o.getType()+')', state:'closed', helpCallback: this.showHelpTopic.bind(this, 'help/panel_layerproperties.html')});
        this._oPropertiesPanel.content.id = 'snapPropertiesPanel';
        this._oSettingsPanel = new JxSnapPanel({label:'Settings', helpCallback: this.showHelpTopic.bind(this, 'help/panel_layersettings.html')});
        this._oSettingsPanel.content.id = 'snapSettingsPanel';
        this._oStylePanel = new JxSnapPanel({label:'Style', helpCallback: this.showHelpTopic.bind(this, 'help/panel_layerstyle.html')});
        this._oStylePanel.content.id = 'snapStylePanel';
        this._oPreviewPanel = new JxSnapPanel({label:'Preview', helpCallback: this.showHelpTopic.bind(this, 'help/panel_layerpreview.html')});
        this._oPreviewPanel.content.id = 'snapPreviewPanel';
        this._oPreviewPanel.content.className = 'snapContent noScrolling';
        
        this.snapManager.add(this._oPropertiesPanel,
                         this._oSettingsPanel,
                         this._oStylePanel,
                         this._oPreviewPanel);
                         
        this.expressionDialog = new WSExpressionBuilderDialog({handler:this.handleExpression.bind(this), app:o.app});
        this.loadPanels();
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.obj.removeContentListener(this);
        this.baseFinalize();
    },
    loadPanels: function() {
        //jxTrace('WSLayerDefinitionView::loadPanels()');
        this.panelStatus = {properties:false, settings:false, style:false, referencedto:false, referencedby:false};
        this._oPropertiesPanel.setContentURL('panels/layerproperties.html', this.panelLoaded.bind(this, 'properties'));
        this._oSettingsPanel.setContentURL('panels/layersettings.html', this.panelLoaded.bind(this, 'settings'));
        this._oStylePanel.setContentURL('panels/layerstyle.html', this.panelLoaded.bind(this, 'style'));
    },
    panelLoaded : function(name) {
        //jxTrace('WSLayerDefinitionView::panelLoaded('+name+')');
        this.panelStatus[name] = true;
        switch(name){
            case 'properties':
                ids = ['featuresourceid','type','location','owner','created','modified',
                       'references', 'title', 'abstract', 'keywords', 'metadata',
                       'bounds', 'srs', 'opaque', 'queryable'];
                this._oPropertiesPanel.registerIds(ids, this._oPropertiesPanel.domObj);
                break;
            case 'referencedto':
                this._oPropertiesPanel.registerIds(['referencedto'], this._oPropertiesPanel.domObj);
                break;
            case 'referencedby':
                this._oPropertiesPanel.registerIds(['referencedby'], this._oPropertiesPanel.domObj);
                break;
            case 'settings':
                this._oSettingsPanel.registerIds(['resourceid','chooseresource', 
                                                  'featureclass', 
                                                  'geometry', 'attributeTable', 
                                                  'filter', 'choosefilter',
                                                  'url', 'chooseurl',
                                                  'tooltip', 'choosetooltip'],
                                                   this._oSettingsPanel.domObj);
        }
        this.checkRenderStatus();
    },
    spatialContextsLoaded : function() {
        //jxTrace('WSLayerDefinitionView::spatialContextsLoaded()');
        /*
        * preview
        */
        this.viewManager.requests=0;
        var mapDefinition = 'Session:'+this.obj.app.getSession() + '//' + this.obj.name + '_Preview.MapDefinition';
        this.previewDefinition = 'Session:'+this.obj.app.getSession() +'//' + this.obj.name + '_Preview.WebLayout';

        //replace mapdefinition's layer with this layer and push to session
        var szMapXML = new String(this.viewManager.previewMapTemplate);
        //TODO: do this with an object literal and a common replace fn
        if (szMapXML)
        {
            //debugger;
            this.viewManager.requests++;
            szMapXML = szMapXML.replace(/%layername%/, this.obj.name);
            szMapXML = szMapXML.replace(/%name%/, 'Preview - ' + this.obj.name);
            var szSessionRid = this.obj.getSessionResourceID();
            szMapXML = szMapXML.replace(/%rid%/, szSessionRid);
            var extents = this.obj.getExtents();
            if (extents.crsWkt != '') {
                szMapXML = szMapXML.replace(/%coordinatesystem%/, extents.crsWkt);
            } else {
                //TODO: when MapAgent supports EPSG to WKT we could probably set this
                szMapXML = szMapXML.replace(/%coordinatesystem%/, '');
            }
            szMapXML = szMapXML.replace(/%minx%/, extents.minx);
            szMapXML = szMapXML.replace(/%miny%/, extents.miny);
            szMapXML = szMapXML.replace(/%maxx%/, extents.maxx);
            szMapXML = szMapXML.replace(/%maxy%/, extents.maxy);
            szMapXML = szMapXML.replace(/%selectable%/, 'true');
            //replace the layout's map with the preview map and push to session
            var r = new WSSetResource(mapDefinition, unescape(szMapXML), null);
            //jxTrace('Preview MapDefinition: '+unescape(szMapXML));
            var b = this.obj.app.getBroker();
            b.dispatchRequest(r,this.layoutWasSet.bind(this, 'SetMapDefinition'));
        }
        var szLayoutXML = new String(this.viewManager.previewLayoutTemplate);
        if (szLayoutXML)
        {
            //debugger;
            this.viewManager.requests++;
            //replace the layout's map with the preview map and push to session
            szLayoutXML = szLayoutXML.replace(/%rid%/, mapDefinition);
            //don't show the infopane for a layer preview
            szLayoutXML = szLayoutXML.replace(/%infopane%/, 'false');
            r = new WSSetResource(this.previewDefinition, unescape(szLayoutXML), null);
            b.dispatchRequest(r,this.layoutWasSet.bind(this, 'SetPreviewLayout'));
        }
        
        this.checkRenderStatus();
    },
    /**
     * this function saves a session copy of the resource. As a content
     * listener, the layoutWasSet function will be called upon completion.
     * This allows the preview to be updated when the user changes the layer
     * but has not saved it. 
     */
    updateSessionLayer: function() {
        //debugger;
        this._oPreviewPanel.setBusy(true);
        this.viewManager.requests++;
        this.obj.saveToSession();        
    },
    layoutWasSet : function(why) {
        //jxTrace('WSLayerDefinitionView::layoutWasSet()');
        //var reason = why;
        if (this.viewManager.requests > 1) {
            this.viewManager.requests--;
            return;
        }
        this.viewManager.requests=0;
        var b = this.obj.app.getBroker();
        if (!this.mapFrame) {
            //this._oPreviewPanel.setBusy(true);
            this.mapFrame = document.createElement('iframe');
            this.mapFrame.className = 'previewFrame';
            this.mapFrame.frameBorder = '0';
            this.mapFrame.marginHeight = '0';
            this.mapFrame.marginWidth = '0';
            this.mapFrame.width = '100%';
            this.mapFrame.height = '100%';
            Event.observe(this._oPreviewPanel, 'load',this._oPreviewPanel.setBusy.bind(this, false));
            this._oPreviewPanel.content.appendChild(this.mapFrame);
        } 
        //for preview refresh, we recreate the content each time
        this.mapFrame.src = b.mapGuideURL +
                        'mapviewerphp/ajaxviewer.php?' +
                        'WEBLAYOUT=' +
                        this.previewDefinition +
                        '&USERNAME=' + b.user +
                        '&PASSWORD=' + b.pass;
        
    },
    headerLoaded : function() {
        //jxTrace('WSLayerDefinitionView::headerLoaded()');
        this.bHeaderLoaded = true;
        this.checkRenderStatus();
    },
    contentLoaded : function() {
        //jxTrace('WSLayerDefinitionView::contentLoaded()');
        this.bContentLoaded = true;
        this.checkRenderStatus();
    },
    contentChanged: function() {
        var title = 'Properties - ' + this.obj.getName() + ' ('+this.obj.getType()+')';
        if (this.obj.isDirty()) {
            title = 'Properties - <span class="modified">' + this.obj.getName() + ' ('+this.obj.getType()+')</span>';
        }
        this._oPropertiesPanel.setLabel(title);
        
        var oPreview = this._oPreviewPanel;
        //oPreview.setLabel(oPreview.getLabel() + ' - needs refresh');
        oPreview.loadingObj.img.src = 'images/icon_refresh.png';
        oPreview.loadingObj.img.style.visibility = 'visible';
        Event.observe(oPreview.loadingObj.link, 'click', this.updateSessionLayer.bind(this));
    },
    featureSchemaLoaded : function () {
        //jxTrace('WSLayerDefinitionView::featureSchemaLoaded()');
        this.bFeatureSchemaLoaded = true;
        this.checkRenderStatus();
        /*save a copy of the resource to the session
         *this needs to be dispatched here so that it's ready when layoutWasSet
         *is called to render the preview.
         */
        this.updateSessionLayer();
        
    },
    resourceReferencesLoaded: function() {
        //jxTrace('WSLayerDefinitionView::resourceReferencesLoaded()');
        this.bResourceReferencesLoaded = true;
        this.checkRenderStatus();
    },
    checkRenderStatus : function() {
        //jxTrace('WSLayerDefinitionView::checkRenderStatus()');
        if (this.panelStatus['properties'] && this.panelStatus['referencedto'] && 
            this.panelStatus['referencedby'] && this.bResourceReferencesLoaded && 
            !this.bResourceReferencesRendered && this.bPropertiesRendered) {
            this.bResourceReferencesRendered = true;
            var refTo = this.obj.getContentValue('ResourceId');
            var refBy = this.obj.getResourceReferences();
            this.renderResourceReferences([refTo], refBy);
        }
        if (this.panelStatus['properties'] && this.bHeaderLoaded && !this.bPropertiesRendered) {
            this.bPropertiesRendered = true;
            this.renderProperties();
        }
        if (this.panelStatus['settings'] && this.bContentLoaded && !this.bSettingsRendered) {
            this.bSettingsRendered = true;
            this.renderSettings();
        }
        if (this.panelStatus['style'] && this.bContentLoaded && this.bFeatureSchemaLoaded && !this.bStyleRendered) {
            this.bStyleRendered = true;
            this.renderStyle();
        }
        if (this.panelStatus['settings'] && this.bFeatureSchemaLoaded && !this.bFeatureSchemaRendered) {
            this.bFeatureSchemaRendered = true;
            this.renderFeatureSchema();
        }
    },
    renderSettings : function() {
        //jxTrace('WSLayerDefinitionView::renderSettings()');
        this._oSettingsPanel.getObj('resourceid').value = this.obj.getContentValue('ResourceId');
        this._oSettingsPanel.getObj('filter').value = this.obj.getFilter();
        this._oSettingsPanel.getObj('url').value = this.obj.getUrl();
        this._oSettingsPanel.getObj('tooltip').value = this.obj.getToolTip();
        
        Event.observe(this._oSettingsPanel.getObj('chooseresource'), 'click', this.chooseResource.bind(this));
        Event.observe(this._oSettingsPanel.getObj('choosefilter'), 'click', this.handleChooseFilter.bind(this));
        Event.observe(this._oSettingsPanel.getObj('chooseurl'), 'click', this.handleChooseUrl.bind(this));
        Event.observe(this._oSettingsPanel.getObj('choosetooltip'), 'click', this.handleChooseToolTip.bind(this));
    },
    getProperties: function() {
        var properties = [];
        var featureClass = this.obj.getContentValue('FeatureName');
        for (var i=0; i<this.obj.featureClasses.length; i++) {
            var key = this.obj.featureClasses[i].schema + ':' + this.obj.featureClasses[i].name;
            if (key == featureClass) {
                typeName = this.obj.featureClasses[i].type;
                break;
            }
        }
        var featureType = this.obj.featureTypes[typeName];
        for (var i=0; i<featureType.attributes.length; i++) {
            properties.push(featureType.attributes[i].name);
        }
        return properties;
    },
    handleChooseFilter: function() {
        var filter = this.obj.getFilter();
        var properties = this.getProperties();
        this.expressionDialog.setfn = this.obj.setFilter.bind(this.obj);
        this.expressionDialog.setobj = 'filter';
        this.expressionDialog.open(properties, filter);
    },
    handleChooseUrl: function() {
        var url = this.obj.getUrl();
        var properties = this.getProperties();
        this.expressionDialog.setfn = this.obj.setUrl.bind(this.obj);
        this.expressionDialog.setobj = 'filter';
        this.expressionDialog.open(properties, url);
    },
    handleChooseToolTip: function() {
        var tooltip = this.obj.getToolTip();
        var properties = this.getProperties();
        this.expressionDialog.setfn = this.obj.setToolTip.bind(this.obj);
        this.expressionDialog.setobj = 'filter';
        this.expressionDialog.open(properties, tooltip);
    },
    handleExpression: function(button, dialog) {
        if (button == 'ok') {
            dialog.setfn(dialog.getExpression());
            this._oSettingsPanel.getObj(dialog.setobj).value = dialog.getExpression();
        }
        dialog.close();
    },
    chooseResource: function() {
        var fs = new WSResourceId(this.obj.getContentValue('ResourceId'));
        this.viewManager.fileDialog.chooseResource({handler:this.handleChoice.bind(this),
                            types:this.viewManager.fileDialog.getFilter('FeatureSource'),
                            path:fs.getPath()});
    },
    handleChoice: function(button,dialog) {
        if (button == 'Select' || button == 'Open') {
            this.bContentLoaded = false;
            this.bFeatureSchemaLoaded = false;
            this.bFeatureSchemaRendered = false;
            this.bSettingsRendered = false;
            this.bStyleRendered = false;
            var resource = dialog.getResource();
            this.obj.setFeatureSourceId(resource);
        }
        dialog.close();
    },
    renderStyle: function() {
      //jxTrace('WSLayerDefinitionView::renderStyle()');
        this._oStylePanel.registerIds(['layer.scales.toolbar',
                                       'layer.scales.ranges'], 
                                       this._oStylePanel.domObj);
        
        var toolbar = new JxToolbar(this._oStylePanel.getObj('layer.scales.toolbar'));
        this._oAddScaleAction = new JxAction(this.dispatchAddScale.bind(this));
        this._oAddRuleAction = new JxAction(this.dispatchAddRule.bind(this));
        this._oThemeAction = new JxAction(this.dispatchTheme.bind(this));
        this._oDuplicateAction = new JxAction(this.dispatchDuplicate.bind(this));
        this._oMoveUpAction = new JxAction(this.dispatchMoveUp.bind(this));
        this._oMoveDownAction = new JxAction(this.dispatchMoveDown.bind(this));
        this._oDeleteAction = new JxAction(this.dispatchDelete.bind(this));
        
        var addScaleItem = new JxToolbarItem('images/icon_add_scale.png', 'Add Scale Range', this._oAddScaleAction);
        var addRuleItem = new JxToolbarItem('images/icon_add_rule.png', 'Add Rule', this._oAddRuleAction);
        var themeItem = new JxToolbarItem('images/icon_theme.png', 'Theme', this._oThemeAction);
        var duplicateItem = new JxToolbarItem('images/icon_duplicate.png', 'Duplicate', this._oDuplicateAction);
        var moveUpItem = new JxToolbarItem('images/icon_move_up.png', 'Move Up', this._oMoveUpAction);
        var moveDownItem = new JxToolbarItem('images/icon_move_down.png', 'Move Down', this._oMoveDownAction);
        var deleteItem = new JxToolbarItem('images/icon_delete.png', 'Delete', this._oDeleteAction);
        
        toolbar.add(addScaleItem, addRuleItem, themeItem, duplicateItem,
                    moveUpItem, moveDownItem, deleteItem);
        
        this.selectRuleWatcher = this.selectRule.bindAsEventListener(this);
        this.selectRangeWatcher = this.selectRange.bindAsEventListener(this);
        this.setLegendLabelWatcher = this.setLegendLabel.bindAsEventListener(this);
        this.setMinScaleWatcher = this.setMinScale.bindAsEventListener(this);
        this.setMaxScaleWatcher = this.setMaxScale.bindAsEventListener(this);
        this.editExpressionWatcher = this.editExpression.bindAsEventListener(this);
        this.editAreaStyleWatcher = this.editAreaStyle.bindAsEventListener(this);
        this.edpreviewitLineStyleWatcher = this.editLineStyle.bindAsEventListener(this);
        this.editPointStyleWatcher = this.editPointStyle.bindAsEventListener(this);
        this.editLabelStyleWatcher = this.editLabelStyle.bindAsEventListener(this);
        
        /* render existing scale ranges */
        var vsr = this.obj.vectorScaleRanges;
        for (var i=0; i<vsr.length; i++) {
            this.createScaleRange(vsr[i]);
        }
        
        this.activateActions();
    },
    createScaleRange: function(range) {
      //jxTrace('WSLayerDefinitionView::createScaleRange()');
        var tbody = this._oStylePanel.getObj('layer.scales.ranges');
        /* the row contains the entire range */
        var tr = document.createElement('tr');
        /* the first column is used for selecting the row */
        var th = document.createElement('th');
        Element.addClassName(th, 'dataTableMarker');
        th.range = range;
        Event.observe(th,'click',this.selectRangeWatcher);
        th.innerHTML = '&nbsp;';
        tr.appendChild(th);
        
        /* the second column contains the min scale */
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.type = 'text';
        input.className = 'wideText';
        
        Event.observe(input, 'focus', this.selectRangeWatcher);
        Event.observe(input, 'change', this.setMinScaleWatcher);
        
        input.value = range.get('MinScale') != ''?range.get('MinScale') : '0';
        input.range = range;
        
        td.appendChild(input);
        tr.appendChild(td);
        
        /* the third column contains the max scale */
        td = document.createElement('td');
        input = document.createElement('input');
        input.type = 'text';
        input.className = 'wideText';
        
        Event.observe(input, 'focus', this.selectRangeWatcher);
        Event.observe(input, 'change', this.setMaxScaleWatcher);

        input.value = range.get('MaxScale') != ''?range.get('MaxScale') : 'infinity';
        input.range = range;
        
        td.appendChild(input);
        tr.appendChild(td);
        
        /* the fourth column contains an embedded table for the rules */
        td = document.createElement('td');
        Element.addClassName(td, 'layerRulesTd');
        td.colSpan = 5;

        var table = document.createElement('table');
        var tableBody = document.createElement('tbody');
        range.ruleTableBody = tableBody;
        
        table.appendChild(tableBody);
        Element.addClassName(table, 'dataTable');
        Element.addClassName(table, 'layerRulesTable');
        var ruleKey = '';
        var typeStyle = range.get('AreaTypeStyle');
        if (typeStyle == '') {
            typeStyle = range.get('LineTypeStyle');
            if (typeStyle == '') {
                typeStyle = range.get('PointTypeStyle');
                if (typeStyle != '') {
                    ruleKey = 'PointRule';
                }
            } else {
                ruleKey = 'LineRule';
            }
        } else {
            ruleKey = 'AreaRule';
        }
        for (var j=0; j<typeStyle.length; j++) {
            var rules = typeStyle[j].get(ruleKey);
            for (var i=0; i<rules.length; i++) {
                var row = this.createRuleRow(rules[i]);
                tableBody.appendChild(row);
            }
        }
        td.appendChild(table);
        tr.appendChild(td);
        tbody.appendChild(tr);
        
        tr.range = range;
        range.tr = tr;
        range.th = th;
        
    },
    createRuleRow: function(rule) {
        var tr = document.createElement('tr');
        /* first column is the row selector */
        var th = document.createElement('th');
        Element.addClassName(th, 'dataTableMarker');
        th.rule = rule;
        Event.observe(th,'click',this.selectRuleWatcher);
        tr.appendChild(th);
        
        /* second column is the filter */
        var td = document.createElement('td');
        var a = document.createElement('a');
        a.href = '#';
        a.appendChild(this.createExpressionPreview(rule));
        a.rule = rule;
        rule.expressionPreview = a;
        Event.observe(a, 'click', this.editExpressionWatcher);
        td.appendChild(a);
        tr.appendChild(td);
        
        /* third column is the style */
        td = document.createElement('td');
        
        a = document.createElement('a');
        a.href = '#';
        
        var geomType = this.obj.getGeometricType();
        switch(geomType) {
            case 'surface':
                a.appendChild(this.createAreaPreview(rule));
                Event.observe(a, 'click', this.editAreaStyleWatcher);
                break;
            case 'curve':
                a.appendChild(this.createLinePreview(rule));
                Event.observe(a, 'click', this.editLineStyleWatcher);
                break;
            case 'point':
                a.appendChild(this.createPointPreview(rule));
                Event.observe(a, 'click', this.editPointStyleWatcher);
                break;
        }
        rule.stylePreview = a;
        a.rule = rule;

        td.appendChild(a);
        tr.appendChild(td);

        /* fourth column is the feature label */
        td = document.createElement('td');
        a = document.createElement('a');
        a.href = '#';
        a.appendChild(this.createLabelPreview(rule));
        rule.labelPreview = a;
        a.rule = rule;
        Event.observe(a,'click',this.editLabelStyleWatcher);
        td.appendChild(a);
        tr.appendChild(td);
        
        /* fifth column is legend label */
        td = document.createElement('td');
        var input = document.createElement('input');
        input.type = 'text';
        input.className = 'wideText';
        
        Event.observe(input, 'focus', this.selectRuleWatcher);
        Event.observe(input, 'change', this.setLegendLabelWatcher);
        
        input.value =  rule.get('LegendLabel');
        input.rule = rule;
        
        td.appendChild(input);
        tr.appendChild(td);
        
        rule.tr = tr;
        rule.th = th;
        tr.rule = rule;
        
        return tr;
    },
    createExpressionPreview: function(rule) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        
        preview.innerHTML = rule.get('Filter');
        return preview;
    },
    createAreaPreview: function(rule) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        
        var div = document.createElement('div');
        div.style.position = 'relative';
        div.style.width = '100px';
        div.style.height = '17px';
        
        var as = rule.get('AreaSymbolization2D');
        var fill = as.get('Fill');
        var stroke = as.get('Stroke');
        
        if (fill != '') {
            var color = fill.get('ForegroundColor');
            color = color.substr(2);
            div.style.backgroundColor = '#'+color;
        }
        if (stroke != '') {
            var unit = stroke.get('Unit');
            var conv = this.getConversionForUnit(unit);
            var width = 100 * conv.unitsPerPixel;
            var height = 17 * conv.unitsPerPixel;
            
            var t = parseFloat(stroke.get('Thickness'));
            if (t == 0) {
                t = 1 * conv.unitsPerPixel;
            }
            
            div.style.width = (width-2*t)+conv.cssSize;
            div.style.height = (height-2*t)+conv.cssSize;
            
            var color = stroke.get('Color');
            color = color.substr(2);
            div.style.border = t+conv.cssSize + ' solid #'+color;
        }
        if (fill == '' && stroke == '') {
            preview.innerHTML = 'None';
        }
        preview.appendChild(div);
        return preview;
    },
    createLinePreview: function(rule) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        
        var lineSymbolizations = rule.get('LineSymbolization2D');
        for (var i=0; i<lineSymbolizations.length; i++) {
            var t = parseFloat(lineSymbolizations[i].get('Thickness'));
            var c = lineSymbolizations[i].get('Color');
            c = c.substr(2);
            var line = document.createElement('div');
            
            var unit = lineSymbolizations[i].get('Unit');
            var conv = this.getConversionForUnit(unit);
            var width = 100 * conv.unitsPerPixel;
            var height = 17 * conv.unitsPerPixel;            
            
            if (t == 0) {
                t = 1 * conv.unitsPerPixel;
            }
            line.style.position = 'absolute';
            line.style.overflow = 'hidden';
            line.style.left = '0px';
            line.style.top = ((height-t)/2) + conv.cssSize;
            line.style.width = '100px';
            line.style.height = t + conv.cssSize;
            line.style.backgroundColor = '#' + c;
            
            preview.appendChild(line);
        }
        
        if (lineSymbolizations.length == 0) {
            preview.innerHTML = 'None';
        }
        
        return preview;
    },
    createPointPreview: function(rule) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        var ps = rule.get('PointSymbolization2D');
        if (ps == '') {
            preview.innerHTML = 'None';
        } else {
            /* five possibilities - Mark, Image, Font, W2D, or Block */
            var symbol = ps.get('Mark');
            if (symbol != '') {
                var sizeContext = symbol.get('SizeContext');
                if (sizeContext == 'MappingUnits') {
                    preview.appendChild(this.previewNotAvailable());
                } else {
                    var shape = symbol.get('Shape');
                    var color = '000000';
                    var fill = symbol.get('Fill');
                    if (fill) {
                        color = fill.get('ForegroundColor');
                        color = color.substr(2);
                    }
                    //var edge = symbol.get('Edge');
                    var sizeX = parseFloat(symbol.get('SizeX'));
                    var sizeY = parseFloat(symbol.get('SizeY'));
                    var unit = symbol.get('Unit');
                    var conv = this.getConversionForUnit(unit);
                    var width = 100 * conv.unitsPerPixel;
                    var height = 17 * conv.unitsPerPixel;
                    var img = document.createElement('img');
                    img.style.position = 'absolute';
                    img.style.left = (width - sizeX)/2+conv.cssSize;
                    img.style.top = (height - sizeY)/2+conv.cssSize;
                    img.src = 'images/points/'+shape.toLowerCase()+'.png';
                    img.style.backgroundColor = '#'+color;
                    img.style.width = sizeX + conv.cssSize;
                    img.style.height = sizeY + conv.cssSize;
                    preview.appendChild(img);
                }
            }
            symbol = ps.get('Image');
            if (symbol != '') {
                preview.appendChild(this.previewNotAvailable());
            }
            symbol = ps.get('Font');
            if (symbol != '') {
                if (symbol.get('Bold') != '') {
                    preview.style.fontWeight = 'bold';
                }
                if (symbol.get('Underlined') != '') {
                    preview.style.textDecoration = 'underline';
                }
                if (symbol.get('Italic') != '') {
                    preview.style.fontStyle = 'italic';
                }
                if (symbol.get('SizeY') != '') {
                    preview.style.fontSize = symbol.get('SizeY') + 'pt';
                }
                var color = symbol.get('ForegroundColor');
                if (color != '') {
                    color = color.substr(2);
                    preview.style.color= '#' + color;
                }
                color = symbol.get('BackgroundColor');
                if (color != '') {
                    color = color.substr(2);
                    preview.style.backgroundColor = '#'+ color;
                }
                if (symbol.get('FontName')) {
                    preview.style.fontFamily = symbol.get('FontName');
                }
                preview.innerHTML = symbol.get('Character');
                preview.style.textAlign = 'center';
                preview.style.lineHeight = '17px';
            }
            symbol = ps.get('W2D');
            if (symbol != '') {
                preview.appendChild(this.previewNotAvailable());
            }
            symbol = ps.get('Block');
            if (symbol != '') {
                preview.appendChild(this.previewNotAvailable());
            }
        }
        return preview;
    },
    createLabelPreview: function(rule) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        
        var label = document.createElement('span');
        label.style.padding = '2px';
        label.style.lineHeight = '17px';
        var ruleLabel = rule.get('Label');
        if (ruleLabel != '') {
            if (ruleLabel.get('Bold') != '') {
                label.style.fontWeight = 'bold';
            }
            if (ruleLabel.get('Underlined') != '') {
                label.style.textDecoration = 'underline';
            }
            if (ruleLabel.get('Italic') != '') {
                label.style.fontStyle = 'italic';
            }
            if (ruleLabel.get('SizeY') != '') {
                label.style.fontSize = ruleLabel.get('SizeY') + 'pt';
            }
            var color = ruleLabel.get('ForegroundColor');
            if (color != '') {
                color = color.substr(2);
                label.style.color= '#' + color;
            }
            color = ruleLabel.get('BackgroundColor');
            if (color != '') {
                color = color.substr(2);
                label.style.backgroundColor = '#'+ color;
            }
            if (ruleLabel.get('FontName')) {
                label.style.fontFamily = ruleLabel.get('FontName');
                label.innerHTML = ruleLabel.get('FontName');
            }
        } else {
            label.innerHTML = '&nbsp;';
        }
        preview.appendChild(label);
        return preview;
    },
    previewNotAvailable: function() {
        var div = document.createElement('div');
        div.style.width = '100px';
        div.style.height = '17px';
        div.style.textAlign = 'center';
        div.style.lineHeight = '17px';
        div.style.fontFamily = 'Arial';
        div.style.fontSize = '10px';
        div.innerHTML = 'preview not available';
        return div;
    },
    getConversionForUnit: function(unit) {
        var sizeUnit;
        var factor = 1;
        var dpi = 96;
        switch(unit) {
            case 'Millimeters':
                factor = 25.4/dpi;
                sizeUnit = 'mm';
                break;
            case 'Centimeters':
                factor = 2.54/dpi;
                sizeUnit = 'cm';
                break;
            case 'Inches':
                factor = 1/dpi;
                sizeUnit = 'in';
                break;
            case 'Points':
                factor = 72/dpi;
                sizeUnit = 'pt';
                break;
            default:
                sizeUnits = 'px';
        }
        return {cssSize: sizeUnit, unitsPerPixel: factor};
    },
    selectRange: function(e) {
        var target = Event.element(e);
        this.setSelectedRange(target.range);
    },
    setSelectedRange: function(range) {
        if (this.currentRange) {
            Element.removeClassName(this.currentRange.tr, 'activeRow');
            Element.removeClassName(this.currentRange.tr, 'selectedRow');
            Element.removeClassName(this.currentRange.th, 'activeMarker');
            Element.removeClassName(this.currentRange.th, 'selectedMarker');
        }
        this.currentRange = range;
        if (this.currentRule) {
            if (this.currentRule.parent.parent != this.currentRange) {
                Element.removeClassName(this.currentRule.tr, 'activeRow');
                Element.removeClassName(this.currentRule.tr, 'selectedRow');
                Element.removeClassName(this.currentRule.th, 'activeMarker');
                Element.removeClassName(this.currentRule.th, 'selectedMarker');
                this.currentRule = null;
            } else {
                Element.removeClassName(this.currentRule.tr, 'activeRow');
                Element.removeClassName(this.currentRule.th, 'activeMarker');
            }
        }
        if (this.currentRange) {
            Element.addClassName(this.currentRange.tr, 'selectedRow');
            Element.addClassName(this.currentRange.tr, 'activeRow');
            Element.addClassName(this.currentRange.th, 'selectedMarker');
            Element.addClassName(this.currentRange.th, 'activeMarker');
            this.lastSelected = 'range';
        } else {
            this.lastSelected = '';
        }
        this.activateActions();
    },
    selectRule: function(e) {
        var target = Event.element(e);
        this.setSelectedRule(target.rule);
    },
    setSelectedRule: function(rule) {
        var range;
        if (rule && rule.parent && rule.parent.parent) {
            range = rule.parent.parent;
        }
        
        if (this.currentRule && this.currentRule != rule) {
            Element.removeClassName(this.currentRule.tr, 'activeRow');
            Element.removeClassName(this.currentRule.tr, 'selectedRow');
            Element.removeClassName(this.currentRule.th, 'activeMarker');
            Element.removeClassName(this.currentRule.th, 'selectedMarker');
        }
        if (this.currentRange) {
            if (this.currentRange != range) {
                Element.removeClassName(this.currentRange.tr, 'activeRow');
                Element.removeClassName(this.currentRange.tr, 'selectedRow');
                Element.removeClassName(this.currentRange.th, 'activeMarker');
                Element.removeClassName(this.currentRange.th, 'selectedMarker');
            } else {
                Element.removeClassName(this.currentRange.tr, 'activeRow');
                Element.removeClassName(this.currentRange.th, 'activeMarker');
            }
        }
        this.currentRange = range;
        if (this.currentRange) {
            Element.addClassName(this.currentRange.tr, 'selectedRow');
            Element.addClassName(this.currentRange.th, 'selectedMarker');
        }
        if (this.currentRule != rule) {
            this.currentRule = rule;
        }
        if (this.currentRule) {
            Element.addClassName(this.currentRule.tr, 'selectedRow');
            Element.addClassName(this.currentRule.th, 'selectedMarker');
            Element.addClassName(this.currentRule.tr, 'activeRow');
            Element.addClassName(this.currentRule.th, 'activeMarker');
            this.lastSelected = 'rule';
        } else {
            this.lastSelected = '';
        }
        this.activateActions();
    },
    setLegendLabel: function(e) {
        var target = Event.element(e);
        if (this.currentRule) {
            this.currentRule.set('LegendLabel', target.value);
        }
    },
    setMinScale: function(e) {
        var target = Event.element(e);
        if (this.currentRange) {
            this.currentRange.set('MinScale', target.value);
        }
    },
    setMaxScale: function(e) {
        var target = Event.element(e);
        if (this.currentRange) {
            this.currentRange.set('MaxScale', target.value);
        }
    },
    editExpression: function(e) {
        var target = Event.findElement(e, 'A');
        this.setSelectedRule(target.rule);
        if (!this.editExpressionDialog) {
            var o = {app: this.viewManager.app,handler: this.handleEditExpression.bind(this)};
            this.editExpressionDialog = new WSExpressionBuilderDialog( o );
        }
        var properties = [];
        var featureClass = this.obj.getContentValue('FeatureName');
        for (var i=0; i<this.obj.featureClasses.length; i++) {
            var key = this.obj.featureClasses[i].schema + ':' + this.obj.featureClasses[i].name;
            if (key == featureClass) {
                typeName = this.obj.featureClasses[i].type;
                break;
            }
        }
        var featureType = this.obj.featureTypes[typeName];
        for (var i=0; i<featureType.attributes.length; i++) {
            properties.push(featureType.attributes[i].name);
        }
        this.editExpressionDialog.open(properties, target.rule.get('Filter'));
        this.editExpressionDialog.rule = target.rule;
    },
    handleEditExpression: function(button, dialog) {
        if (button == 'ok') {
            var expression = dialog.getExpression();
            dialog.rule.set('Filter', expression);
            var preview = this.createExpressionPreview(dialog.rule);
            dialog.rule.expressionPreview.replaceChild(preview,
                 dialog.rule.expressionPreview.childNodes[0]);
        }
        dialog.close();
    },
    editAreaStyle: function(e) {
        var target = Event.findElement(e, 'A');
        this.setSelectedRule(target.rule);
        if (!this.editLineDialog) {
            var o = {app: this.viewManager.app,handler: this.handleAreaLine.bind(this)};
            this.editAreaDialog = new WSStyleAreaDialog( o );
        }
        this.editAreaDialog.open(target.rule);
    },
    handleAreaLine: function(button, dialog) {
        if (button == 'ok') {
            dialog.updateRule();
            var preview = this.createAreaPreview(dialog.rule);
            dialog.rule.stylePreview.replaceChild(preview,
                 dialog.rule.stylePreview.childNodes[0]);
        }
        dialog.close();
    },
    editLineStyle: function(e) {
        var target = Event.findElement(e, 'A');
        this.setSelectedRule(target.rule);
        if (!this.editLineDialog) {
            var o = {app: this.viewManager.app,handler: this.handleEditLine.bind(this)};
            this.editLineDialog = new WSStyleLineDialog( o );
        }
        this.editLineDialog.open(target.rule);
    },
    handleEditLine: function(button, dialog) {
        if (button == 'ok') {
            dialog.updateRule();
            var preview = this.createLinePreview(dialog.rule);
            dialog.rule.stylePreview.replaceChild(preview,
                 dialog.rule.stylePreview.childNodes[0]);        
        }
        dialog.close();
    },
    editPointStyle: function(e) {
        var target = Event.findElement(e, 'A');
        this.setSelectedRule(target.rule);
        if (!this.editPointDialog) {
            var o = {app: this.viewManager.app,handler: this.handleEditPoint.bind(this)};
            this.editPointDialog = new WSStylePointDialog( o );
        }
        var properties = [];
        var featureClass = this.obj.getContentValue('FeatureName');
        for (var i=0; i<this.obj.featureClasses.length; i++) {
            var key = this.obj.featureClasses[i].schema + ':' + this.obj.featureClasses[i].name;
            if (key == featureClass) {
                typeName = this.obj.featureClasses[i].type;
                break;
            }
        }
        var featureType = this.obj.featureTypes[typeName];
        for (var i=0; i<featureType.attributes.length; i++) {
            properties.push(featureType.attributes[i].name);
        }
        this.editPointDialog.open(target.rule, properties);
    },
    handleEditPoint: function(button, dialog) {
        if (button == 'ok') {
            dialog.updateRule();
            var preview = this.createPointPreview(dialog.rule);
            dialog.rule.stylePreview.replaceChild(preview,
                 dialog.rule.stylePreview.childNodes[0]);
        }
        dialog.close();
    },
    editLabelStyle: function(e) {
        var target = Event.findElement(e, 'A');
        this.setSelectedRule(target.rule);
        if (!this.editLabelDialog) {
            var o = {app: this.viewManager.app,handler: this.handleEditLabel.bind(this)};
            this.editLabelDialog = new WSStyleLabelDialog( o );
        }
        var properties = [];
        var featureClass = this.obj.getContentValue('FeatureName');
        for (var i=0; i<this.obj.featureClasses.length; i++) {
            var key = this.obj.featureClasses[i].schema + ':' + this.obj.featureClasses[i].name;
            if (key == featureClass) {
                typeName = this.obj.featureClasses[i].type;
                break;
            }
        }
        var featureType = this.obj.featureTypes[typeName];
        for (var i=0; i<featureType.attributes.length; i++) {
            properties.push(featureType.attributes[i].name);
        }
        this.editLabelDialog.open(target.rule, properties);
    },
    handleEditLabel: function(button, dialog) {
        if (button == 'ok') {
            dialog.updateRule();
            var preview = this.createLabelPreview(dialog.rule);
            dialog.rule.labelPreview.replaceChild(preview,
                dialog.rule.labelPreview.childNodes[0]);
        }
        dialog.close();
    },
    activateActions: function() {
        this._oAddScaleAction.setEnabled(true);
        this._oAddRuleAction.setEnabled(this.currentRange);
        this._oThemeAction.setEnabled(false);
        this._oDuplicateAction.setEnabled(this.currentRange);
        var bDeleteEnabled = false;
        var idx = -1; 
        var nItems = 0;
        if (this.lastSelected == 'range') {
            nItems = this.obj.vectorScaleRanges.length;
            for (var i=0; i<nItems; i++) {
                if (this.obj.vectorScaleRanges[i] == this.currentRange) {
                    idx = i;
                    break;
                }
            }
            bDeleteEnabled = true;
        } else if (this.lastSelected == 'rule') {
            var parent = this.currentRule.parent;
            var rules = parent.get('AreaRule');
            if (rules == '') {
                rules = parent.get('LineRule');
                if (rules == '') {
                    rules = parent.get('PointRule');
                }
            }
            if (rules != '') {
                nItems = rules.length;
                for (var i=0; i<nItems; i++) {
                    if (rules[i] == this.currentRule) {
                        idx = i;
                        break;
                    }
                }
            }
            bDeleteEnabled = (nItems > 1);
        }
        this._oDeleteAction.setEnabled(bDeleteEnabled);
        this._oMoveUpAction.setEnabled(idx > 0);
        this._oMoveDownAction.setEnabled(idx >= 0 && idx < nItems - 1);
    },
    dispatchAddScale: function() {
      //jxTrace('dispatchAddScale');
        var vsr = new WSVectorScaleRangeType();
        var geomType = this.obj.getGeometricType();
        switch(geomType) {
            case 'surface':
                var typeStyle = new WSAreaTypeStyleType();
                var rule = new WSAreaRuleType();
                var as = new WSAreaSymbolizationFillType();
                rule.set('AreaSymbolization2D', as);
                typeStyle.set('AreaRule', [rule]);
                rule.set('LegendLabel', '');
                vsr.set('AreaTypeStyle', [typeStyle]);
                break;
            case 'curve':
                var typeStyle = new WSLineTypeStyleType();
                var rule = new WSLineRuleType();
                var ls = new WSStrokeType();
                ls.set('LineStyle', 'Solid');
                ls.set('Thickness', '0');
                ls.set('Color', 'FF000000');
                ls.set('Unit', 'Points');
                ls.set('SizeContext', 'DeviceUnits');
                rule.set('LineSymbolization2D', [ls]);
                rule.set('LegendLabel', '');
                typeStyle.set('LineRule', [rule]);
                vsr.set('LineTypeStyle', [typeStyle]);
                break;
            case 'point':
                var typeStyle = new WSPointTypeStyleType();
                var rule = new WSPointRuleType();
                var ps = new WSPointSymbolization2DType();
                var mark = new WSMarkSymbolType();
                mark.set('Shape', 'Circle');
                mark.set('Unit', 'Points');
                mark.set('SizeContext', 'DeviceUnits');
                mark.set('SizeX', '6');
                mark.set('SizeY', '6');
                ps.set('Mark', mark);
                rule.set('PointSymbolization2D', ps);
                rule.set('LegendLabel', '');
                typeStyle.set('DisplayAsText', 'false');
                typeStyle.set('AllowOverpost', 'false');
                typeStyle.set('PointRule', [rule]);
                vsr.set('PointTypeStyle', [typeStyle]);
                break;
        }
        //for setDirty support
        vsr.parent = this.obj;
        this.obj.vectorScaleRanges.push(vsr);
        this.obj.setDirty();
        this.createScaleRange(vsr);
    },
    dispatchAddRule: function() {
      //jxTrace('dispatchAddRule');
        var vsr = this.currentRange;
        var geomType = this.obj.getGeometricType();
        switch(geomType) {
            case 'surface':
                var typeStyle = vsr.get('AreaTypeStyle');
                var rules = typeStyle[0].get('AreaRule');
                var rule = new WSAreaRuleType();
                var as = new WSAreaSymbolizationFillType();
                rule.set('AreaSymbolization2D', as);
                rule.set('LegendLabel', '');
                rules.push(rule);
                typeStyle[0].set('AreaRule', rules);
                break;
            case 'curve':
            var typeStyle = vsr.get('LineTypeStyle');
                var rules = typeStyle[0].get('LineRule');
                var rule = new WSLineRuleType();
                var ls = new WSStrokeType();
                ls.set('LineStyle', 'Solid');
                ls.set('Thickness', '0');
                ls.set('Color', 'FF000000');
                ls.set('Unit', 'Points');
                ls.set('SizeContext', 'DeviceUnits');
                rule.set('LineSymbolization2D', [ls]);
                rule.set('LegendLabel', '');
                rules.push(rule);
                typeStyle[0].set('LineRule', rules);
                break;
            case 'point':
                var typeStyle = vsr.get('PointTypeStyle');
                var rules = typeStyle[0].get('PointRule');
                var rule = new WSPointRuleType();
                var ps = new WSPointSymbolization2DType();
                var mark = new WSMarkSymbolType();
                mark.set('Shape', 'Circle');
                mark.set('Unit', 'Points');
                mark.set('SizeContext', 'DeviceUnits');
                mark.set('SizeX', '6');
                mark.set('SizeY', '6');
                ps.set('Mark', mark);
                rule.set('PointSymbolization2D', ps);
                rule.set('LegendLabel', '');
                rules.push(rule);
                typeStyle[0].set('PointRule', rules);
                break;
        }
        var tr = this.createRuleRow(rule);
        this.currentRange.ruleTableBody.appendChild(tr);
    },
    dispatchTheme: function() {
        //jxTrace('dispatchTheme');
    },
    dispatchDuplicate: function() {
        //jxTrace('dispatchDuplicate');
        if (this.lastSelected == 'range') {
            var vsr = this.currentRange.clone();
            vsr.parent = this.obj;
            this.obj.vectorScaleRanges.push(vsr);
            this.obj.setDirty();
            this.createScaleRange(vsr);
        } else if (this.lastSelected == 'rule') {
            var idx = this.getRuleIndex(this.currentRule);
            if (idx.index >= 0) {
                var rule = this.currentRule.clone();
                rule.parent = this.currentRule.parent;
                idx.rules.push(rule);
                this.currentRule.parent.set(idx.ruleKey, idx.rules);
                var tr = this.createRuleRow(rule);
                this.currentRange.ruleTableBody.appendChild(tr);
            }
        }
        this.activateActions();
    },
    dispatchMoveUp: function() {
      //jxTrace('dispatchMoveUp');
        var idx = -1; 
        var nItems = 0;
        if (this.lastSelected == 'range') {
            nItems = this.obj.vectorScaleRanges.length;
            for (var i=0; i<nItems; i++) {
                if (this.obj.vectorScaleRanges[i] == this.currentRange) {
                    idx = i;
                    break;
                }
            }
            if (idx > 0) {
                var row = this.currentRange.tr;
                var tbody = row.parentNode;
                tbody.insertBefore(row, tbody.childNodes[idx-1]);
                this.obj.vectorScaleRanges.splice(idx,1);
                this.obj.vectorScaleRanges.splice(idx-1, 0, this.currentRange);
                this.obj.setDirty(true);
            }
            
        } else if (this.lastSelected == 'rule') {
            var idx = this.getRuleIndex(this.currentRule);
            if (idx.index > 0) {
                var row = this.currentRule.tr;
                var tbody = row.parentNode;
                tbody.insertBefore(row, tbody.childNodes[idx.index-1]);
                idx.rules.splice(idx.index, 1);
                idx.rules.splice(idx.index-1,0,this.currentRule);
                this.currentRule.parent.set(idx.ruleKey, idx.rules);
            }
        }
        this.activateActions();
    },
    dispatchMoveDown: function() {
      //jxTrace('dispatchMoveDown');
        var idx = -1; 
        var nItems = 0;
        if (this.lastSelected == 'range') {
            nItems = this.obj.vectorScaleRanges.length;
            for (var i=0; i<nItems; i++) {
                if (this.obj.vectorScaleRanges[i] == this.currentRange) {
                    idx = i;
                    break;
                }
            }
            if (idx >= 0 && idx < nItems - 1) {
                var row = this.currentRange.tr;
                var tbody = row.parentNode;
                tbody.insertBefore(tbody.childNodes[idx+1], row);
                this.obj.vectorScaleRanges.splice(idx,1);
                this.obj.vectorScaleRanges.splice(idx+1, 0, this.currentRange);
                this.obj.setDirty(true);
            }
            
        } else if (this.lastSelected == 'rule') {
            var idx = this.getRuleIndex(this.currentRule);
            if (idx.index >= 0 && idx.index < idx.nItems - 1) {
                var row = this.currentRule.tr;
                var tbody = row.parentNode;
                tbody.insertBefore(tbody.childNodes[idx.index+1], row);
                idx.rules.splice(idx.index, 1);
                idx.rules.splice(idx.index+1,0,this.currentRule);
                this.currentRule.parent.set(idx.key, idx.rules);
            }
        }
        this.activateActions();
    },
    dispatchDelete: function() {
      //jxTrace('dispatchDelete');
        var idx = -1; 
        var nItems = 0;
        if (this.lastSelected == 'range') {
            nItems = this.obj.vectorScaleRanges.length;
            for (var i=0; i<nItems; i++) {
                if (this.obj.vectorScaleRanges[i] == this.currentRange) {
                    idx = i;
                    break;
                }
            }
            if (idx >= 0) {
                var row = this.currentRange.tr;
                var tbody = row.parentNode;
                tbody.removeChild(row);
                this.obj.vectorScaleRanges.splice(idx.index,1);
                this.obj.setDirty(true);
                this.setSelectedRange(null);
            }
            
        } else if (this.lastSelected == 'rule') {
            var idx = this.getRuleIndex(this.currentRule);
            if (idx.index >= 0 && idx.index < idx.nItems) {
                var row = this.currentRule.tr;
                var tbody = row.parentNode;
                tbody.removeChild(row);
                idx.rules.splice(idx.index, 1);
                this.currentRule.parent.set(idx.key, idx.rules);
                this.setSelectedRule(null);
            }
        }
    },
    getRuleIndex: function( rule ) {
        var idx = -1;
        var nItems = 0;
        var parent = rule.parent;
        var rules = parent.get('AreaRule');
        var ruleKey = 'AreaRule';
        if (rules == '') {
            rules = parent.get('LineRule');
            ruleKey = 'LineRule';
            if (rules == '') {
                rules = parent.get('PointRule');
                ruleKey = 'PointRule';
            }
        }
        if (rules != '') {
            nItems = rules.length;
            for (var i=0; i<nItems; i++) {
                if (rules[i] == this.currentRule) {
                    idx = i;
                    break;
                }
            }
        }
        return {index: idx, key: ruleKey, nItems: nItems, rules: rules};
    },
    renderFeatureSchema: function() {
        //jxTrace('WSLayerDefinitionView::renderFeatureSchema()');
        var featureName = this.obj.getContentValue('FeatureName');
        var featureClass = this.obj.getContentValue('FeatureName');
        var typeName = ''; //this is the lookup key for featureTypes.
        var s = this._oSettingsPanel.getObj('featureclass');
        while (s.options.length > 0) {
            s.options[0] = null;
        }
        for (var i=0; i<this.obj.featureClasses.length; i++) {
            var key = this.obj.featureClasses[i].schema + ':' + this.obj.featureClasses[i].name;
            var opt = new Option( key, key, key == featureClass, key == featureClass );
            if (key == featureClass) {
                typeName = this.obj.featureClasses[i].type;
            }
            s.options[s.options.length] = opt;
        }
        var geometry = this.obj.getContentValue('Geometry');
        var featureType = this.obj.featureTypes[typeName];
        s = this._oSettingsPanel.getObj('geometry');
        while (s.options.length > 0) {
            s.options[0] = null;
        }
        var opt = new Option(geometry, geometry, true, true);
        s.options[s.options.length] = opt;
        /*
        TODO: it is not clear to me where the values of geometry come from.
        s.options.length = 0;
        for (var i=0; i<featureType.length; i++) {
            var opt = new Option( featureType.type, 
                                  featureType.type,
                                  featureType.type == featureClass,
                                  featureType.type == featureClass );
            if (featureType.type == featureClass) {
                typeName = featureType.complexType;
            }
            s.options[s.options.length] = opt;
        }
        */
        
        var table = this._oSettingsPanel.getObj('attributeTable');
        while(table.childNodes.length > 0) {
            table.removeChild(table.childNodes[0]);
        }
        for (var i=0; i<featureType.attributes.length; i++) {
            var ftype = featureType.attributes[i];
            var mapping = this.obj.getPropertyMapping(ftype.name);
            var s = '';
            var row = document.createElement('tr');
            var cell = document.createElement('th');
            var check = document.createElement('input');
            check.type = 'checkbox';
            check.name = ftype.name;
            check.checked = mapping?true:false;
            cell.appendChild(check);
            row.appendChild(cell);
            cell = document.createElement('td');
            var input = document.createElement('input');
            input.type = 'text';
            input.name = '';
            input.value = mapping?mapping.value:ftype.name;
            input.disabled = mapping?false:true;
            cell.appendChild(input);
            row.appendChild(cell);
            cell = document.createElement('td');
            cell.innerHTML = ftype.name;
            row.appendChild(cell);
            table.appendChild(row);
            Event.observe(check, 'click', this.changePropertyMapping.bind(this, check, input));
            Event.observe(input, 'keyup', this.changePropertyMapping.bind(this, check, input));
            Event.observe(input, 'blur', this.changePropertyMapping.bind(this, check, input));
        }
    },
    changePropertyMapping: function(check, input) {
        //jxTrace('WSLayerDefinitionView::changePropertyMapping()');
        if (check.checked) {
            this.obj.setPropertyMapping(check.name, input.value);
            input.disabled = false;
        } else {
            this.obj.removePropertyMapping(check.name);
            input.disabled = true;
        }
    },
    addScaleRange: function() {
        //jxTrace('WSLayerDefinitionView::addScaleRange()');
    }
});

var WSMapDefinitionView = Class.create();
Object.extend(WSMapDefinitionView.prototype, WSView.prototype);
Object.extend(WSMapDefinitionView.prototype, {
    previewDefinition : null,
    uidBase: [0],
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this.bSettingsRendered = false;
        this.bLayersRendered = false;
        this.bPropertiesRendered = false;
        this.bResourceReferencesRendered = false;
        this.bLayersByGroupRendered = false;
        this.bLayerDrawingOrderRendered = false;
        this.bBaseLayersRendered = false;
        
        this._oPropertiesPanel = new JxSnapPanel({label:'Properties - ' + o.getName() + ' ('+o.getType()+')', state:'closed', helpCallback: this.showHelpTopic.bind(this, 'help/panel_mapproperties.html')});
        this._oPropertiesPanel.content.id = 'snapPropertiesPanel';
        this._oSettingsPanel = new JxSnapPanel({label:'Settings', helpCallback: this.showHelpTopic.bind(this, 'help/panel_mapsettings.html')});
        this._oSettingsPanel.content.id = 'snapSettingsPanel';
        this._oLayersPanel = new JxSnapPanel({label:'Layers', helpCallback: this.showHelpTopic.bind(this, 'help/panel_maplayers.html')});
        this._oLayersPanel.content.id = 'snapLayersPanel';
        this._oPreviewPanel = new JxSnapPanel({label:'Preview', helpCallback: this.showHelpTopic.bind(this, 'help/panel_mappreview.html')});
        this._oPreviewPanel.content.id = 'snapPreviewPanel';
        this._oPreviewPanel.content.className = 'snapContent noScrolling';
        
        this.snapManager.add(this._oPropertiesPanel,
                         this._oSettingsPanel,
                         this._oLayersPanel,
                         this._oPreviewPanel);
         this.loadPanels();
         this.uniqueId = this.uidBase[0];
         this.uidBase[0] ++;
     },
     finalize: function() {
         //TODO: clean up all the panels then call baseFinalize
         this.obj.removeContentListener(this);
         this.baseFinalize();
     },
     loadPanels: function() {
         this.panelStatus = {properties:false, settings:false, layers:false, referencedto:false, referencedby:false};
         this._oPropertiesPanel.setContentURL('panels/mapproperties.html', this.panelLoaded.bind(this, 'properties'));
         this._oSettingsPanel.setContentURL('panels/mapsettings.html', this.panelLoaded.bind(this, 'settings'));
         this._oLayersPanel.setContentURL('panels/mapstyle.html', this.panelLoaded.bind(this, 'layers'));
     },
     panelLoaded : function(name) {
         this.panelStatus[name] = true;
         switch(name){
             case 'properties':
                 var ids = ['featuresourceid','type','location','owner','created','modified', 'references'];
                 this._oPropertiesPanel.registerIds(ids, this._oPropertiesPanel.domObj);
                 break;
             case 'referencedto':
                 this._oPropertiesPanel.registerIds(['referencedto'], this._oPropertiesPanel.domObj);
                 break;
             case 'referencedby':
                 this._oPropertiesPanel.registerIds(['referencedby'], this._oPropertiesPanel.domObj);
                 break;
             case 'settings':
                 this._oSettingsPanel.registerIds(['description','coordinatesystem', 
                                                   'choosecoordinatesystem',
                                                  'backgroundcolor', 'backgroundcolor.swatch', 'minx', 'miny', 
                                                  'maxx', 'maxy'], this._oSettingsPanel.domObj);
                 break;
            case 'layers':
                this._oLayersPanel.registerIds(['layers'], this._oLayersPanel.domObj);
                break;
         }
         this.checkRenderStatus();
     },
     contentLoaded : function() {
         this.bContentLoaded = true;
         this.checkRenderStatus();
        /*
         * preview
         */
        this.viewManager.requests=0;
        var mapDefinition = this.obj.rid;
        var previewMapDefinition = this.obj.getSessionResourceID();
        this.previewDefinition = 'Session:'+this.obj.app.getSession() +'//' + this.obj.name + '_MapPreview.WebLayout';

        var szLayoutXML = new String(this.viewManager.previewLayoutTemplate);
        if (szLayoutXML) {
            this.viewManager.requests++;
            //replace the layout's map with the preview map and push to session
            szLayoutXML = szLayoutXML.replace(/%rid%/, previewMapDefinition);//mapDefinition);
            szLayoutXML = szLayoutXML.replace(/%infopane%/, 'true');
            var r = new WSSetResource(this.previewDefinition, szLayoutXML, null);
            var b = this.obj.app.getBroker();
            b.dispatchRequest(r,this.layoutWasSet.bind(this));

            /*save a copy of the resource to the session
             *this needs to be dispatched here so that it's ready when layoutWasSet
             *is called to render the preview.
             */
            this.updateSessionMap();
        };
        //set the content of the preview panel to the preview server URL
    },
    /**
     * this function saves a session copy of the resource. As a content
     * listener, the layoutWasSet function will be called upon completion.
     * This allows the preview to be updated when the user changes the map
     * but has not saved it. 
     */
    updateSessionMap: function() {
        this._oPreviewPanel.setBusy(true);
        this.viewManager.requests++;
        this.obj.saveToSession();       
    },
    /**
     * listener for preview setup
     *
     * this is called by the underlying model object to notify the view that
     * the required elements have been pushed to the server. It returns each time
     * it is called until the reference count is 0, indicating that the
     * previewDefinition (WebLayout) and session MapDefinition are available so
     * the web tier's viewer can render the preview.
     */
    layoutWasSet : function() {
        if (this.viewManager.requests > 1) {
            this.viewManager.requests--;
            return;
        }
        this.viewManager.requests = 0;
        
        var b = this.obj.app.getBroker();
        if (this._oPreviewPanel.content.childNodes.length == 0) {
            this.mapFrame = document.createElement('iframe');
            this._oPreviewPanel.content.appendChild(this.mapFrame);
            this.mapFrame.className = 'previewFrame';
            this.mapFrame.frameBorder = '0';
            this.mapFrame.marginHeight = '0';
            this.mapFrame.marginWidth = '0';
        }
        this.mapFrame.src = b.mapGuideURL +
                        'mapviewerphp/ajaxviewer.php?' +
                        'WEBLAYOUT=' + this.previewDefinition +
                        '&USERNAME=' + b.user +
                        '&PASSWORD=' + b.pass;
    },
    contentChanged: function() {
        var title = 'Properties - ' + this.obj.getName() + ' ('+this.obj.getType()+')';
        if (this.obj.isDirty()) {
            title = 'Properties - <span class="modified">' + this.obj.getName() + ' ('+this.obj.getType()+')</span>';
        }
        this._oPropertiesPanel.setLabel(title);
        var oPreview = this._oPreviewPanel;
        //oPreview.setLabel(oPreview.getLabel() + ' - needs refresh');
        oPreview.loadingObj.img.src = 'images/icon_refresh.png';
        oPreview.loadingObj.img.style.visibility = 'visible';
        Event.observe(oPreview.loadingObj.link, 'click', this.updateSessionMap.bind(this));
    },
    headerLoaded : function() {
        this.bHeaderLoaded = true;
        this.checkRenderStatus();
    },
    checkRenderStatus : function() {
        if (this.panelStatus['properties'] && this.panelStatus['referencedto'] && 
            this.panelStatus['referencedby'] && this.bResourceReferencesLoaded && 
            !this.bResourceReferencesRendered && this.bPropertiesRendered) {
            var refTo = this.obj.getResourceToReferences();
            var refBy = this.obj.getResourceReferences();
            this.renderResourceReferences(refTo, refBy);
            this.bResourceReferencesRendered = true;
        }
        if (this.panelStatus['properties'] && this.bHeaderLoaded && !this.bPropertiesRendered) {
            this.renderProperties();
            this.bPropertiesRendered = true;
        }
        if (this.panelStatus['settings'] && this.bContentLoaded && !this.bSettingsRendered) {
            this.renderSettings();
            this.bSettingsRendered = true;
        }
        if (this.panelStatus['layers'] && this.bContentLoaded && !this.bLayersRendered) {
            this.renderLayers();
            this.bLayersRendered = true;
        }
        if (this.panelStatus['layerGroups'] && this.bContentLoaded && !this.bLayersByGroupRendered) {
            this.renderLayersByGroup();
            this.bLayersByGroupRendered = true;
        }
        if (this.panelStatus['layerOrder'] && this.bContentLoaded && !this.bLayerDrawingOrderRendered) {
            this.renderLayersDrawingOrder();
            this.bLayerDrawingOrderRendered = true;
        }
        if (this.panelStatus['baseLayers'] && this.bContentLoaded && !this.bBaseLayersRendered) {
            this.renderBaseLayers();
            this.bBaseLayersRendered = true;
        }
    },
    resourceReferencesLoaded: function() {
        this.bResourceReferencesLoaded = true;
        this.checkRenderStatus();
    },
    renderSettings : function() {
        this._oSettingsPanel.getObj('description').value = this.obj.getContentValue('Name');
        this._oSettingsPanel.getObj('coordinatesystem').value = this.obj.getContentValue('CoordinateSystem');
        Event.observe(this._oSettingsPanel.getObj('choosecoordinatesystem'), 'click', this.chooseCoordinateSystem.bind(this));
        var bgcolor = this.obj.getContentValue('BackgroundColor');
        this._oSettingsPanel.getObj('backgroundcolor').value = bgcolor;

        this._oSettingsPanel.getObj('backgroundcolor.swatch').style.backgroundColor = '#' + bgcolor.substring(2);
        
        this._oSettingsPanel.getObj('minx').value = this.obj.getContentValue('MinX');
        this._oSettingsPanel.getObj('miny').value = this.obj.getContentValue('MinY');
        this._oSettingsPanel.getObj('maxx').value = this.obj.getContentValue('MaxX');
        this._oSettingsPanel.getObj('maxy').value = this.obj.getContentValue('MaxY');
        
        //attach events to modifiable elements
        Event.observe(this._oSettingsPanel.getObj('description'), 'keyup', this.changeSetting.bind(this,'description','Name'));
        Event.observe(this._oSettingsPanel.getObj('description'), 'blur', this.changeSetting.bind(this, 'description', 'Name'));
        Event.observe(this._oSettingsPanel.getObj('backgroundcolor'), 'keyup', this.changeSetting.bind(this,'backgroundcolor','BackgroundColor'));
        Event.observe(this._oSettingsPanel.getObj('backgroundcolor'), 'blur', this.changeSetting.bind(this, 'backgroundcolor', 'BackgroundColor'));
        Event.observe(this._oSettingsPanel.getObj('minx'), 'keyup', this.changeSetting.bind(this,'minx', 'MinX'));
        Event.observe(this._oSettingsPanel.getObj('minx'), 'blur', this.changeSetting.bind(this,   'minx', 'MinX'));
        Event.observe(this._oSettingsPanel.getObj('miny'), 'keyup', this.changeSetting.bind(this,'miny', 'MinY'));
        Event.observe(this._oSettingsPanel.getObj('miny'), 'blur', this.changeSetting.bind(this,   'miny', 'MinY'));
        Event.observe(this._oSettingsPanel.getObj('maxx'), 'keyup', this.changeSetting.bind(this,'maxx', 'MaxX'));
        Event.observe(this._oSettingsPanel.getObj('maxx'), 'blur', this.changeSetting.bind(this,   'maxx', 'MaxX'));
        Event.observe(this._oSettingsPanel.getObj('maxy'), 'keyup', this.changeSetting.bind(this,'maxy', 'MaxY'));
        Event.observe(this._oSettingsPanel.getObj('maxy'), 'blur', this.changeSetting.bind(this,   'maxy', 'MaxY'));
    },
    chooseCoordinateSystem: function() {
        if (!this._oCoordinateSystemDialog) {
            this._oCoordinateSystemDialog = new WSCoordinateSystemDialog(
                {app:this.viewManager.app,
                 handler:this.handleChooseCoordinateSystem.bind(this)});
        }
        this._oCoordinateSystemDialog.open(this._oSettingsPanel.getObj('coordinatesystem').value);
    },
    handleChooseCoordinateSystem: function(button, dialog) {
        if (button == 'ok') {
            this._oSettingsPanel.getObj('coordinatesystem').value = dialog.getWKT();
            this.obj.setContentValue('CoordinateSystem', dialog.getWKT());
        }
        dialog.close();
    },
    setDefaultCrsAndExtents: function(rid) {
        var r = new WSGetResourceContent(rid);
        var b = this.viewManager.app.getBroker();
        b.dispatchRequest(r, this.layerResourceLoaded.bind(this));
    },
    layerResourceLoaded: function(r) {
        if (r.status == 200 && r.responseXML) {
            var node = new DomNode(r.responseXML);
            var resourceId = node.getNodeText('ResourceId');
            var r = new WSGetSpatialContexts(resourceId);
            var b = this.viewManager.app.getBroker();
            b.dispatchRequest(r, this.spatialContextsLoaded.bind(this));
        }
    },
    spatialContextsLoaded: function(r) {
        if (r.status == 200 && r.responseXML) {
            var node = new DomNode(r.responseXML);
            var wkt = node.getNodeText('CoordinateSystemWkt');
            var ll = node.findFirstNode('LowerLeftCoordinate');
            var minx = ll.getNodeText('X');
            var miny = ll.getNodeText('Y');
            var ur = node.findFirstNode('UpperRightCoordinate');
            var maxx = ur.getNodeText('X');
            var maxy = ur.getNodeText('Y');

            this._oSettingsPanel.getObj('coordinatesystem').value = wkt;
            this.obj.setContentValue('CoordinateSystem', wkt);
            
            this._oSettingsPanel.getObj('minx').value = minx;
            this._oSettingsPanel.getObj('miny').value = miny;
            this._oSettingsPanel.getObj('maxx').value = maxx;
            this._oSettingsPanel.getObj('maxy').value = maxy;
            
            this.obj.setContentValue('MinX', minx);
            this.obj.setContentValue('MinY', miny);
            this.obj.setContentValue('MaxX', maxx);
            this.obj.setContentValue('MaxY', maxy);
        }
    },
    changeSetting: function(name, xmlName) {
        var domObj = this._oSettingsPanel.getObj(name);
        this.obj.changeSetting(xmlName, domObj.value);
        
        var bgcolor = this._oSettingsPanel.getObj('backgroundcolor').value;
        if (bgcolor.length == 8) {
            this._oSettingsPanel.getObj('backgroundcolor.swatch').style.backgroundColor = '#' + bgcolor.substring(2);
        }
    },
    renderLayers: function() {
        this._oLayersPanel.registerIds(['map.style.layers','map.style.settings',
                                        'map.style.title', 'map.style.toolbar',
                                        'map.style.views', 'map.style.fieldset',
                                        'map.style.nonesettings',
                                        'map.style.maplayersettings',
                                        'map.style.mapgroupsettings',
                                        'map.style.baselayersettings',
                                        'map.style.basegroupsettings',
                                        'map.layer.name',
                                        'map.layer.resourceid',
                                        'map.layer.chooseresource',
                                        'map.layer.group',
                                        'map.layer.showinlegend',
                                        'map.layer.selectable',
                                        'map.layer.visible',
                                        'map.layer.legendlabel',
                                        'map.layer.expandinlegend',
                                        'map.group.name',
                                        'map.group.group',
                                        'map.group.showinlegend',
                                        'map.group.visible',
                                        'map.group.legendlabel',
                                        'map.group.expandinlegend',
                                        'base.layer.name',
                                        'base.layer.resourceid',
                                        'base.layer.chooseresource',
                                        'base.layer.group',
                                        'base.layer.selectable',
                                        'base.layer.showinlegend',
                                        'base.layer.legendlabel',
                                        'base.layer.expandinlegend',
                                        'base.group.name',
                                        'base.group.showinlegend',
                                        'base.group.visible',
                                        'base.group.legendlabel',
                                        'base.group.expandinlegend',
                                        'map.style.finitescales',
                                        'map.style.scale',
                                        'map.style.addscale'
                                        ], this._oLayersPanel.domObj);
        
        var toolbar = new JxToolbar(this._oLayersPanel.getObj('map.style.toolbar'));
        this._oNewFolderAction = new JxAction(this.dispatchNewFolder.bind(this));
        this._oAddLayerAction = new JxAction(this.dispatchAddLayer.bind(this));
        this._oEditAction = new JxAction(this.dispatchEdit.bind(this));
        this._oCutAction = new JxAction(this.dispatchCut.bind(this));
        this._oCopyAction = new JxAction(this.dispatchCopy.bind(this));
        this._oPasteAction = new JxAction(this.dispatchPaste.bind(this));
        this._oMoveUpAction = new JxAction(this.dispatchMoveUp.bind(this));
        this._oMoveDownAction = new JxAction(this.dispatchMoveDown.bind(this));
        this._oDeleteAction = new JxAction(this.dispatchDelete.bind(this));
        
        var newFolderItem = new JxToolbarItem('images/icon_new_folder.png', 'Open Folder', this._oNewFolderAction);
        var addLayerItem = new JxToolbarItem('images/icon_add_layer.png', 'Add Layer', this._oAddLayerAction);
        var editItem = new JxToolbarItem('images/icon_edit.png', 'Edit', this._oEditAction);
        var cutItem = new JxToolbarItem('images/icon_cut.png', 'Cut', this._oCutAction);
        var copyItem = new JxToolbarItem('images/icon_copy.png','Copy', this._oCopyAction);
        var pasteItem = new JxToolbarItem('images/icon_paste.png', 'Paste', this._oPasteAction);
        var moveUpItem = new JxToolbarItem('images/icon_move_up.png', 'Move Up', this._oMoveUpAction);
        var moveDownItem = new JxToolbarItem('images/icon_move_down.png', 'Move Down', this._oMoveDownAction);
        var deleteItem = new JxToolbarItem('images/icon_delete.png', 'Delete', this._oDeleteAction);
        toolbar.add(newFolderItem, addLayerItem, editItem, cutItem, copyItem, 
                    pasteItem, moveUpItem, moveDownItem, deleteItem);
        
        //create the main tabs for groups, drawing order and base layers
        var tabBar = new JxTabBar(this._oLayersPanel.getObj('map.style.layers'), this._oLayersPanel.getObj('map.style.views'));
        tabBar.addSelectionListener(this);
        
        this.groupTab = new JxTab('Groups');
        this.groupTabContent = this.groupTab.content;
        Element.addClassName(this.groupTabContent, 'mapLayersBox');

        this.drawingTab = new JxTab('Drawing Order');
        this.drawingTabContent = this.drawingTab.content;
        Element.addClassName(this.drawingTabContent,'mapLayersBox');
        
        this.baseTab = new JxTab('Base Layers');
        this.baseTabContent = this.baseTab.content;
        Element.addClassName(this.baseTabContent,'mapLayersBox');
        
        tabBar.addTab(this.groupTab, this.drawingTab, this.baseTab);
        
        //render the content of the groups tab
        this.groupTree = new JxTree(this.groupTabContent);
        var options = {imgTreeFolder: 'images/tree_map.png', imgTreeFolderOpen: 'images/tree_map.png', label: 'Map', isOpen: true};
        this.groupRoot = new JxTreeFolder(options);
        this.groupRoot.data = this.obj.groupRoot;
        this.obj.groupRoot.gTreeItem = this.groupRoot;
        this.obj.groupRoot.doListObj = null;
        this.groupRoot.addSelectionListener(this);
        this.groupTree.append(this.groupRoot);
        this.renderGroupInTree(this.obj.groupRoot, this.groupRoot);
        
        //render the content of the drawing order tab
        this.drawingOrderList = document.createElement('ul');
        this.drawingOrderList.id = 'dolist_'+this.uniqueId;
        this.drawingOrderList.className = 'jxList';
        this.drawingTabContent.appendChild(this.drawingOrderList);
        for (var i=0; i<this.obj.mapLayers.length;i++) {
            var li = document.createElement('li');
            var a = document.createElement('a');
            a.href = '#';
            a.className = 'LayerDefinition';
            a.innerHTML = this.obj.mapLayers[i].name;
            Event.observe(a,'click',this.selectionChanged.bind(this, li));
            this.obj.mapLayers[i].doListObj = li;
            li.data = this.obj.mapLayers[i];
            li.appendChild(a);
            this.drawingOrderList.appendChild(li);
        }
        //render the content of the base layers tab
        this.baseTree = new JxTree(this.baseTabContent);
        var options = {imgTreeFolder: 'images/tree_map.png', imgTreeFolderOpen: 'images/tree_map.png', label: 'Base Layers', isOpen: true};
        this.baseRoot = new JxTreeFolder(options);
        this.baseRoot.data = this.obj.baseMapGroups;
        this.obj.baseMapGroups.gTreeItem = this.baseRoot;
        this.obj.baseMapGroups.doListObj = null;
        this.baseRoot.addSelectionListener(this);
        this.baseTree.append(this.baseRoot);
        this.renderGroupInTree(this.obj.baseMapGroups, this.baseRoot);
        
        
        this.activeTab = this.groupTab;
        this.activateActions();
        
        /* set up fixed function references so we can use 
         * Event.observe and Event.stopObserving 
         */
        this.setDirtyWatcher = this.obj.setDirty.bind(this);
        this.chooseLayerResourceWatcher = this.chooseLayerResource.bind(this);
        this.handleMapLayerChangeWatcher = this.handleMapLayerChange.bind(this);
        this.handleMapGroupChangeWatcher = this.handleMapGroupChange.bind(this);
        this.handleBaseLayerChangeWatcher = this.handleBaseLayerChange.bind(this);
        this.handleBaseGroupChangeWatcher = this.handleBaseGroupChange.bind(this);
        this.handleAddScaleWatcher = this.handleAddScale.bind(this);
        this.finiteScaleWatcher = this.finiteScaleChanged.bind(this);
  
        //need finiteScaleWatcher before we can call updateFiniteScales
        this.updateFiniteScales();
        
        Event.observe(this._oLayersPanel.getObj('map.layer.chooseresource'), 'click', this.chooseLayerResourceWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.chooseresource'),'click', this.chooseLayerResourceWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.name'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.name'),'change', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.showinlegend'),'keypress', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.selectable'),'click', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.visible'),'click', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.legendlabel'),'keypress', this.setDirtyWatcher);        Event.observe(this._oLayersPanel.getObj('map.layer.legendlabel'),'change', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.layer.expandinlegend'),'click', this.handleMapLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.name'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.name'),'change', this.handleMapGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.showinlegend'),'click', this.handleMapGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.visible'),'click', this.handleMapGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.legendlabel'),'keypress', this.setDirtyWatcher);        Event.observe(this._oLayersPanel.getObj('map.group.legendlabel'),'change', this.handleMapGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.group.expandinlegend'),'click', this.handleMapGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.name'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.name'),'change', this.handleBaseLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.selectable'),'click', this.handleBaseLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.showinlegend'),'click', this.handleBaseLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.legendlabel'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.legendlabel'),'change', this.handleBaseLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.layer.expandinlegend'),'click', this.handleBaseLayerChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.name'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.name'),'change', this.handleBaseGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.showinlegend'),'click', this.handleBaseGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.visible'),'click', this.handleBaseGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.legendlabel'),'keypress', this.setDirtyWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.legendlabel'),'change', this.handleBaseGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('base.group.expandinlegend'),'click', this.handleBaseGroupChangeWatcher);
        Event.observe(this._oLayersPanel.getObj('map.style.addscale'), 'click', this.handleAddScaleWatcher);
    },
    renderGroupInTree: function( group, parent ) {
        for (var i=0; i<group.layers.length; i++) {
            var nodeOptions = {};
            nodeOptions.imgIcon = 'images/tree_layer.png';
            nodeOptions.label = group.layers[i].name;
            nodeOptions.data = group.layers[i];
            var node = new JxTreeItem(nodeOptions);
            node.addSelectionListener(this);
            parent.append(node);
            group.layers[i].gTreeItem = node;
        }
        for (var i=0; i<group.groups.length; i++) {
            var groupOptions = {label: group.groups[i].name, isOpen: true, data: group.groups[i]};
            var folder = new JxTreeFolder(groupOptions);
            folder.data = group.groups[i];
            folder.addSelectionListener(this);
            parent.append(folder);
            group.groups[i].gTreeItem = folder;
            this.renderGroupInTree(group.groups[i],folder);
        }
    },
    removeGroup: function(item) {
        for (var i=0; i<item.layers.length; i++) {
            if (item.layers[i].doListObj) {
                this.drawingOrderList.removeChild(item.layers[i].doListObj);
            }
            if (item.layers[i].gTreeItem) {
                item.layers[i].gTreeItem.parent.remove(item.layers[i].gTreeItem);
                item.layers[i].gTreeItem.finalize();
                item.layers[i].gTreeItem = null;
            }
        }
        for (var i=0; i<item.groups.length; i++) {
            this.removeGroup(item.groups[i]);
        }
    },
    activateActions: function() {
        var selection = this.activeTab.currentSelection;
        var obj = this.activeTab.currentSelection ? this.activeTab.currentSelection.data : null;
        var parent = obj ? obj.parent : null;
        
        switch(this.activeTab) {
            case this.groupTab:
                //new folder is always available on this tab
                this._oNewFolderAction.setEnabled(selection);
                //add layer is always available on this tab
                this._oAddLayerAction.setEnabled(selection);
                //edit is only available if the current selection is a layer
                this._oEditAction.setEnabled(selection &&
                                             typeof(obj.groups) == 'undefined' &&
                                             selection != this.groupRoot);
                //cut is only available if the current selection is a layer or a group
                this._oCutAction.setEnabled(selection && selection != this.groupRoot);
                //copy is only available if the current selection is a layer or a group
                this._oCopyAction.setEnabled(selection && selection != this.groupRoot);
                //paste is only available if there is something selected and something on the clipboard
                this._oPasteAction.setEnabled(selection && this.activeTab.clipboard);
                //move up and down are only available if the layer can be moved in its group
                //and if there is a place to move it to.  It can apply to either groups or layers
                if (selection && selection != this.groupRoot) {
                    //handle groups
                    if (obj.groups) {
                        var idx = parent.groups.indexOf(obj);
                        this._oMoveUpAction.setEnabled(idx > 0);
                        this._oMoveDownAction.setEnabled(idx < parent.groups.length - 1);
                    } else {
                        this._oMoveUpAction.setEnabled(false);
                        this._oMoveDownAction.setEnabled(false);
                    }
                } else {
                    this._oMoveUpAction.setEnabled(false);
                    this._oMoveDownAction.setEnabled(false);
                }
                this._oDeleteAction.setEnabled(selection && selection != this.groupRoot);
                break;
            case this.drawingTab:
                //new folder is never available on the drawing tab
                this._oNewFolderAction.setEnabled(false);
                //add layer is always available on the drawing tab
                this._oAddLayerAction.setEnabled(true);
                //edit is available if there is a current selection
                this._oEditAction.setEnabled(selection);
                this._oCutAction.setEnabled(true);
                this._oCopyAction.setEnabled(true);
                this._oPasteAction.setEnabled(this.activeTab.clipboard?true:false);
                var b = selection?true:false;
                if (b) {
                    var nLayers = this.drawingOrderList.childNodes.length;
                    var idx = -1;
                    for (var i=0; i<nLayers;i++) {
                        if (this.drawingOrderList.childNodes[i] == this.activeTab.currentSelection) {
                            idx = i;
                            break;
                        }
                    }
                    this._oMoveUpAction.setEnabled(idx>0);
                    this._oMoveDownAction.setEnabled(idx<nLayers-1);
                } else {
                    this._oMoveUpAction.setEnabled(false);
                    this._oMoveUpAction.setEnabled(false);
                }
                this._oDeleteAction.setEnabled(true);
                break;
            case this.baseTab:
                this._oNewFolderAction.setEnabled(selection == this.baseRoot);
                this._oAddLayerAction.setEnabled(selection && selection != this.baseRoot);
                this._oEditAction.setEnabled(selection &&
                                             selection != this.baseRoot &&
                                             typeof(obj.groups) == 'undefined');
                this._oCutAction.setEnabled(selection && selection != this.baseRoot);
                this._oCopyAction.setEnabled(selection && selection != this.baseRoot);
                if (this.activeTab.clipboard) {
                    if (this.activeTab.clipboard.data.groups && selection == this.baseRoot) {
                        this._oPasteAction.setEnabled(true);
                    } else if (!this.activeTab.clipboard.data.groups && selection != this.baseRoot) {
                        this._oPasteAction.setEnabled(true);
                    } else {
                        this._oPasteAction.setEnabled(false);
                    }
                } else {
                    this._oPasteAction.setEnabled(false);
                }
                
                //move up and down are only available if the layer can be moved in its group
                //and if there is a place to move it to.  It can apply to either groups or layers
                if (selection && selection != this.baseRoot) {
                    //handle groups
                    if (obj.groups) {
                        var idx = parent.groups.indexOf(obj);
                        this._oMoveUpAction.setEnabled(idx > 0);
                        this._oMoveDownAction.setEnabled(idx < parent.groups.length - 1);
                    } else {
                        //handle layers
                        var idx = parent.layers.indexOf(obj);
                        this._oMoveUpAction.setEnabled(idx > 0);
                        this._oMoveDownAction.setEnabled(idx < parent.layers.length - 1);
                    }
                } else {
                    this._oMoveUpAction.setEnabled(false);
                    this._oMoveDownAction.setEnabled(false);
                }
                this._oDeleteAction.setEnabled(selection && selection != this.baseRoot);
                break;
        }
    },
    chooseLayerResource: function() {
        var rid = '';
        if (this.activeTab == this.groupTab) {
            rid = this._oLayersPanel.getObj('map.layer.resourceid').value;
        } else {
            rid = this._oLayersPanel.getObj('base.layer.resourceid').value;
        }
        
        var fs = new WSResourceId(rid);
this.viewManager.fileDialog.chooseResource({handler:this.handleChooseResource.bind(this),types:this.viewManager.fileDialog.getFilter('LayerDefinition'),path:fs.getPath()});
    },
    handleChooseResource: function(button,dialog) {
        if (button == 'Select' || button == 'Open') {
            var rid = dialog.getResource();
            if (this.activeTab == this.groupTab) {
                this._oLayersPanel.getObj('map.layer.resourceid').value = rid;
                this.handleMapLayerChange();
            } else {
                this._oLayersPanel.getObj('base.layer.resourceid').value = rid;
                this.handleBaseLayerChange();
            }
        }
        dialog.close();
    },
    selectionChanged : function(o) {
        this._oLayersPanel.getObj('map.style.fieldset').style.display = 'block';
        if (o instanceof JxTab) {
            this.activeTab = o;
        } else {
            if (this.activeTab.currentSelection) {
                if (this.activeTab.currentSelection.data) {
                    if (this.activeTab.currentSelection.data.doListObj) {
                        Element.removeClassName(this.activeTab.currentSelection.data.doListObj, 'selected');
                    }
                    if (this.activeTab.currentSelection.data.gTreeItem) {
                        Element.removeClassName(this.activeTab.currentSelection.data.gTreeItem.domObj.childNodes[2], 'jxTreeSelectedNode');
                    }
                }
            }
            this.activeTab.currentSelection = o;
            if (this.activeTab.currentSelection && this.activeTab.currentSelection.data) {
                if (this.activeTab.currentSelection.data.gTreeItem) {
                    if (this.activeTab != this.groupTab && this.groupTab.currentSelection && this.groupTab.currentSelection.data && this.groupTab.currentSelection.data.gTreeItem) {
                        Element.removeClassName(this.groupTab.currentSelection.data.gTreeItem.domObj.childNodes[2], 'jxTreeSelectedNode');
                    }
                    Element.addClassName(this.activeTab.currentSelection.data.gTreeItem.domObj.childNodes[2], 'jxTreeSelectedNode');
                    if (this.activeTab != this.baseTab) {
                        this.groupTab.currentSelection = o.data.gTreeItem;
                    }
                    
                }
                if (this.activeTab.currentSelection.data.doListObj) {
                    Element.addClassName(this.activeTab.currentSelection.data.doListObj, 'selected');
                    if (this.activeTab != this.baseTab) {
                        if (o.data.groups) {
                            this.drawingTab.currentSelection = null;
                        } else {
                            this.drawingTab.currentSelection = o.data.doListObj;
                        }
                    }
                } else {
                    this.drawingTab.currentSelection = null;
                }
            }
            
        }
        switch(this.activeTab) {
            case this.groupTab:
                if (!this.activeTab.currentSelection || this.activeTab.currentSelection == this.groupRoot) {
                    this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'block';
                    this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.mapgroupsettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.title').innerHTML = 'Settings';
                } else {
                    if (this.activeTab.currentSelection.data.groups) {
                        this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.mapgroupsettings').style.display = 'block';
                        this.populateMapGroupSettings();
                    } else {
                        this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'block';
                        this._oLayersPanel.getObj('map.style.mapgroupsettings').style.display = 'none';
                        this.populateMapLayerSettings();
                    }
                }
                this._oLayersPanel.getObj('map.style.baselayersettings').style.display = 'none';
                this._oLayersPanel.getObj('map.style.basegroupsettings').style.display = 'none';
                break;
            case this.drawingTab:
                if (this.activeTab.currentSelection) {
                    this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'block';
                    this.populateMapLayerSettings();
                } else {
                    this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'block';
                    this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.title').innerHTML = 'Settings';
                }
                this._oLayersPanel.getObj('map.style.mapgroupsettings').style.display = 'none';
                this._oLayersPanel.getObj('map.style.baselayersettings').style.display = 'none';
                this._oLayersPanel.getObj('map.style.basegroupsettings').style.display = 'none';
                break;
            case this.baseTab:
                if (!this.activeTab.currentSelection || this.activeTab.currentSelection == this.baseRoot) {
                    this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'block';
                    this._oLayersPanel.getObj('map.style.baselayersettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.basegroupsettings').style.display = 'none';
                    this._oLayersPanel.getObj('map.style.title').innerHTML = 'Settings';
                } else {
                    if (this.activeTab.currentSelection.data.groups) {
                        this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.baselayersettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.basegroupsettings').style.display = 'block';
                        this.populateBaseGroupSettings();
                    } else {
                        this._oLayersPanel.getObj('map.style.nonesettings').style.display = 'none';
                        this._oLayersPanel.getObj('map.style.baselayersettings').style.display = 'block';
                        this._oLayersPanel.getObj('map.style.basegroupsettings').style.display = 'none';
                        this.populateBaseLayerSettings();
                    }
                }
                this._oLayersPanel.getObj('map.style.maplayersettings').style.display = 'none';
                this._oLayersPanel.getObj('map.style.mapgroupsettings').style.display = 'none';
                break;
        }
        this.activateActions();
    },
    populateMapLayerSettings: function() {
        var obj = this.activeTab.currentSelection.data;
        this._oLayersPanel.getObj('map.style.title').innerHTML = '"'+obj.name+'" Layer Settings';
        
        this._oLayersPanel.getObj('map.layer.name').value = obj.name;
        this._oLayersPanel.getObj('map.layer.resourceid').value = obj.resourceId;
        this._oLayersPanel.getObj('map.layer.group').value = obj.parent.name?obj.parent.name:'None';
        this._oLayersPanel.getObj('map.layer.visible').checked = obj.visible == 'true' ? true : false;
        this._oLayersPanel.getObj('map.layer.selectable').checked = obj.selectable == 'true' ? true : false;
        this._oLayersPanel.getObj('map.layer.showinlegend').checked = obj.showInLegend == 'true' ? true : false;
        this._oLayersPanel.getObj('map.layer.legendlabel').value = obj.legendLabel;
        this._oLayersPanel.getObj('map.layer.expandinlegend').checked = obj.expandInLegend == 'true' ? true : false;
    },
    populateMapGroupSettings: function() {
        var obj = this.activeTab.currentSelection.data;
        this._oLayersPanel.getObj('map.style.title').innerHTML = '"'+obj.name+'" Group Settings';
        this._oLayersPanel.getObj('map.group.name').value = obj.name;
        this._oLayersPanel.getObj('map.group.group').value = obj.parent.name?obj.parent.name:'None';
        this._oLayersPanel.getObj('map.group.visible').checked = obj.visible == 'true' ? true : false;
        this._oLayersPanel.getObj('map.group.showinlegend').checked = obj.showInLegend == 'true' ? true : false;
        this._oLayersPanel.getObj('map.group.legendlabel').value = obj.legendLabel;
        this._oLayersPanel.getObj('map.group.expandinlegend').checked = obj.expandInLegend == 'true' ? true : false;
    },
    populateBaseLayerSettings: function() {
        var obj = this.activeTab.currentSelection.data;
        this._oLayersPanel.getObj('map.style.title').innerHTML = '"'+obj.name+'" Layer Settings';
        this._oLayersPanel.getObj('base.layer.name').value = obj.name;
        this._oLayersPanel.getObj('base.layer.resourceid').value = obj.resourceId;
        this._oLayersPanel.getObj('base.layer.group').value = obj.parent.name?obj.parent.name:'None';
        this._oLayersPanel.getObj('base.layer.selectable').checked = obj.selectable == 'true' ? true : false;
        this._oLayersPanel.getObj('base.layer.showinlegend').checked = obj.showInLegend == 'true' ? true : false;
        this._oLayersPanel.getObj('base.layer.legendlabel').value = obj.legendLabel;
        this._oLayersPanel.getObj('base.layer.expandinlegend').checked = obj.expandInLegend == 'true' ? true : false;
    },
    populateBaseGroupSettings: function() {
        var obj = this.activeTab.currentSelection.data;
        this._oLayersPanel.getObj('map.style.title').innerHTML = '"'+obj.name+'" Group Settings';
        this._oLayersPanel.getObj('base.group.name').value = obj.name;
        this._oLayersPanel.getObj('base.group.visible').checked = obj.visible == 'true' ? true : false;
        this._oLayersPanel.getObj('base.group.showinlegend').checked = obj.showInLegend == 'true' ? true : false;
        this._oLayersPanel.getObj('base.group.legendlabel').value = obj.legendLabel;
        this._oLayersPanel.getObj('base.group.expandinlegend').checked = obj.expandInLegend == 'true' ? true : false;
    },
    drawingOrderChanged: function(list) {
        var aLayers = [];
        for (var i=0; i<list.childNodes.length; i++) {
            var li = list.childNodes[i];
            aLayers.push( li.data );
        }
        this.obj.setDrawingOrder(aLayers);
        this.activateActions();
    },
    handleMapLayerChange: function() {
        //jxTrace('handleMapLayerChange');
        var obj = this.activeTab.currentSelection.data;
        if (!this.obj.isLayerNameUnique(obj, this._oLayersPanel.getObj('map.layer.name').value)) {
            this._oLayersPanel.getObj('map.layer.name').value = obj.name;
            alert('Layer names must be unique.');
        } else {
            obj.name = this._oLayersPanel.getObj('map.layer.name').value;
            obj.resourceId = this._oLayersPanel.getObj('map.layer.resourceid').value;
            obj.visible = this._oLayersPanel.getObj('map.layer.visible').checked ? 'true' : 'false';
            obj.selectable = this._oLayersPanel.getObj('map.layer.selectable').checked ? 'true' : 'false';
            obj.showInLegend = this._oLayersPanel.getObj('map.layer.showinlegend').checked ? 'true' : 'false';
            obj.legendLabel = this._oLayersPanel.getObj('map.layer.legendlabel').value;
            obj.expandInLegend = this._oLayersPanel.getObj('map.layer.expandinlegend').checked ? 'true' : 'false';
            this.obj.setDirty();
            obj.gTreeItem.domObj.childNodes[2].innerHTML = obj.name;
            obj.doListObj.childNodes[0].innerHTML = obj.name;
        }
    },
    handleMapGroupChange: function() {
        //jxTrace('handleMapGroupChange');
        var obj = this.activeTab.currentSelection.data;
        if (!this.obj.isGroupNameUnique(obj, this._oLayersPanel.getObj('map.group.name').value)) {
            this._oLayersPanel.getObj('map.group.name').value = obj.name;
            alert('Group names must be unique.');
        } else {
            obj.name = this._oLayersPanel.getObj('map.group.name').value;
            obj.visible = this._oLayersPanel.getObj('map.group.visible').checked ? 'true' : 'false';
            obj.showInLegend = this._oLayersPanel.getObj('map.group.showinlegend').checked ? 'true' : 'false';
            obj.legendLabel = this._oLayersPanel.getObj('map.group.legendlabel').value;
            obj.expandInLegend = this._oLayersPanel.getObj('map.group.expandinlegend').checked ? 'true' : 'false';
            this.obj.setDirty();
            obj.gTreeItem.domObj.childNodes[2].innerHTML = obj.name;
        }
    },
    handleBaseLayerChange: function() {
        //jxTrace('handleBaseLayerChange');
        var obj = this.activeTab.currentSelection.data;
        if (!this.obj.isLayerNameUnique(obj, this._oLayersPanel.getObj('base.layer.name').value)) {
            this._oLayersPanel.getObj('base.layer.name').value = obj.name;
            alert('Layer names must be unique.');
        } else {
            obj.name = this._oLayersPanel.getObj('base.layer.name').value;
            obj.resourceId = this._oLayersPanel.getObj('base.layer.resourceid').value;
            obj.selectable = this._oLayersPanel.getObj('base.layer.selectable').checked ? 'true' : 'false';
            obj.showInLegend = this._oLayersPanel.getObj('base.layer.showinlegend').checked ? 'true' : 'false';
            obj.legendLabel = this._oLayersPanel.getObj('base.layer.legendlabel').value;
            obj.expandInLegend = this._oLayersPanel.getObj('base.layer.expandinlegend').checked ? 'true' : 'false';
            this.obj.setDirty();
            obj.gTreeItem.domObj.childNodes[2].innerHTML = obj.name;
        }
    },
    handleBaseGroupChange: function() {
        //jxTrace('handleBaseGroupChange');
        var obj = this.activeTab.currentSelection.data;
        if (!this.obj.isGroupNameUnique(obj, this._oLayersPanel.getObj('base.group.name').value)) {
            this._oLayersPanel.getObj('base.group.name').value = obj.name;
            alert('Group names must be unique.');
        } else {
            obj.name = this._oLayersPanel.getObj('base.group.name').value;
            obj.visible = this._oLayersPanel.getObj('base.group.visible').checked ? 'true' : 'false';
            obj.showInLegend = this._oLayersPanel.getObj('base.group.showinlegend').checked ? 'true' : 'false';
            obj.legendLabel = this._oLayersPanel.getObj('base.group.legendlabel').value;
            obj.expandInLegend = this._oLayersPanel.getObj('base.group.expandinlegend').checked ? 'true' : 'false';
            this.obj.setDirty();
            obj.gTreeItem.domObj.childNodes[2].innerHTML = obj.name;
        }
    },
    handleAddScale: function() {
        this.obj.addFiniteScale(this._oLayersPanel.getObj('map.style.scale').value);
        this.updateFiniteScales();
    },
    finiteScaleChanged: function() {
        var fs = this.obj.getFiniteScales();
        var ol = this._oLayersPanel.getObj('map.style.finitescales');
        for (var i=0; i<ol.childNodes.length; i++) {
            if (ol.childNodes[i].childNodes[0].value == '') {
                fs[i] = null;
            } else {
                fs[i] = ol.childNodes[i].childNodes[0].value;
            }
        }
        fs = fs.compact();
        this.obj.setFiniteScales(fs);
        this.updateFiniteScales();
    },
    updateFiniteScales: function() {
        var fs = this.obj.getFiniteScales();
        var ol = this._oLayersPanel.getObj('map.style.finitescales');
        
        while(ol.childNodes.length > fs.length) {
            var idx = ol.childNodes.length-1;
            
            if (ol.childNodes[idx].nodeType == 'input' && ol.childNodes[idx].type == 'text') {
                Event.stopObserving(ol.childNodes[idx].childNodes[0], 'blur', this.finiteScaleWatcher);
            }
            ol.removeChild(ol.childNodes[idx]);
        }
        while(ol.childNodes.length < fs.length) {
            var li = document.createElement('li');
            var text = document.createElement('input');
            text.type = "text";
            text.className = "wideText";
            Event.observe(text, 'blur', this.finiteScaleWatcher);
            li.appendChild(text);
            ol.appendChild(li);
        }
        for (var i=0; i<fs.length; i++) {
            ol.childNodes[i].childNodes[0].value = fs[i];
        }
    },
    dispatchNewFolder: function() {
        switch(this.activeTab) {
            case this.groupTab:
                /* two things to do:
                 * 1. create a new group in the underlying object
                 * 2. create a new element in the interface
                 */
                var parent = this.activeTab.currentSelection.data;
                if (!parent.groups) {
                    parent = parent.parent;
                }
                var group = this.obj.createMapLayerGroup(parent);
                
                var groupOptions = {label: group.name, isOpen: true, data: group};
                var folder = new JxTreeFolder(groupOptions);
                folder.addSelectionListener(this);
                parent.gTreeItem.append(folder);
                group.gTreeItem = folder;
                break;
            case this.baseTab:
                var parent = this.baseRoot.data;
                var group = this.obj.createBaseMapGroup();
                var groupOptions = {label: group.name, isOpen: true, data: group};
                var folder = new JxTreeFolder(groupOptions);
                folder.addSelectionListener(this);
                parent.gTreeItem.append(folder);
                group.gTreeItem = folder;
                break;
        }
    },
    dispatchAddLayer: function() {
    this.viewManager.fileDialog.chooseResource({handler:this.handleAddLayer.bind(this),types:this.viewManager.fileDialog.getFilter('LayerDefinition')});
    },
    handleAddLayer: function(button,dialog) {
        if (button == 'Select' || button == 'Open') {
            var rid = dialog.getResource();
            
            switch(this.activeTab) {
                case this.drawingTab:
                case this.groupTab:
                    var parent = (this.activeTab == this.groupTab) ? this.activeTab.currentSelection.data :
                                                                     this.groupRoot.data;
                    if (!parent.groups) { parent = parent.parent; }
                    if (this.obj.mapLayers.length == 0) {
                        //need to set the coordinate system and extents from this layer
                        this.setDefaultCrsAndExtents(rid);
                    }
                    
                    /* three things to do:
                     * 1. create a new group in the underlying object
                     * 2. create a new tree element in the interface
                     * 3. create a new list element in the interface
                     */
                     var layer = this.obj.createMapLayer(parent, rid);

                     var nodeOptions = {};
                     nodeOptions.imgIcon = 'images/tree_layer.png';
                     nodeOptions.label = layer.name;
                     nodeOptions.data = layer;
                     var node = new JxTreeItem(nodeOptions);
                     node.addSelectionListener(this);
                     parent.gTreeItem.append(node);
                     layer.gTreeItem = node;
                     
                     var li = document.createElement('li');
                     //TODO: this can easily break when we remove things from the list
                     li.id = 'do_'+this.uniqueId+'_'+this.drawingOrderList.length;
                     var a = document.createElement('a');
                     a.href='#';
                     a.className = 'LayerDefinition';
                     a.innerHTML = layer.name;
                     Event.observe(a, 'click', this.selectionChanged.bind(this, li));
                     layer.doListObj = li;
                     li.data = layer;
                     li.appendChild(a);
                     this.drawingOrderList.appendChild(li);
                     
                     break;
                case this.baseTab:
                    if (this.obj.baseMapLayers.length == 0) {
                        //need to set the coordinate system and extents from this layer
                        this.setDefaultCrsAndExtents(rid);
                    }
                
                    var parent = this.activeTab.currentSelection.data;
                    if (!parent.groups) { parent = parent.parent; }
                    var layer = this.obj.createBaseMapLayer(parent, rid);

                    var nodeOptions = {};
                    nodeOptions.imgIcon = 'images/tree_layer.png';
                    nodeOptions.label = layer.name;
                    nodeOptions.data = layer;
                    var node = new JxTreeItem(nodeOptions);
                    node.addSelectionListener(this);
                    parent.gTreeItem.append(node);
                    layer.gTreeItem = node;
                    break;
            }
        }
        dialog.close();
    },
    dispatchEdit: function() {
        /* this ends up being a two stage process because we need the enumeration xml */
        var r = new WSEnumerateResources(this.activeTab.currentSelection.data.resourceId, '', 0);
        this.viewManager.app.getBroker().dispatchRequest(r,this.resourceLoaded.bind(this));
    },
    resourceLoaded: function(r) {
        var resources = this.viewManager.app._oFactory.process(r.responseXML);
        this.viewManager.addResource(resources[0]);
    },
    clearClipboard: function() {
        if (this.activeTab.clipboard  && this.activeTab.clipboard.operation == 'cut') {
            switch(this.activeTab) {
                case this.groupTab:
                    Element.removeClassName(this.activeTab.operation.data.gTreeItem.domObj, 'jxTreeCutNode');
                    break;
                case this.drawingTab:
                    Element.removeClassName(this.activeTab.operation.data.doListObj.domObj, 'jxTreeCutNode');
                    break;
                case this.baseTab:
            }
        }
        this.activeTab.clipboard = null;
    },
    dispatchCut: function() {
        this.clearClipboard();
        if (this.activeTab.currentSelection) {
            this.activeTab.clipboard = {data:this.activeTab.currentSelection.data, operation: 'cut', group:this.activeTab.currentSelection.data.parent};
        
            if (this.activeTab.currentSelection.data.gTreeItem) {
                Element.addClassName(this.activeTab.currentSelection.data.gTreeItem.domObj, 'jxTreeCutNode');
            }
            if (this.activeTab.currentSelection.data.doListObj) {
                Element.addClassName(this.activeTab.currentSelection.data.doListObj, 'jxTreeCutNode');
            }
        }
        this.activateActions();
    },
    dispatchCopy: function() {
        this.clearClipboard();
        if (this.activeTab.currentSelection) {
            this.activeTab.clipboard = {data:this.activeTab.currentSelection.data, operation: 'copy', group:this.activeTab.currentSelection.data.parent};
        }
        this.activateActions();
    },
    dispatchPaste: function() {
        if (this.activeTab.clipboard) {
            var obj = this.activeTab.clipboard.data;
            /* special case on cut operation is to remove it from the tree/list first */
            if (this.activeTab.clipboard.operation == 'cut') {
                switch(this.activeTab) {
                    case this.drawingTab:
                    case this.groupTab:
                        if (obj.doListObj) {
                            Element.removeClassName(obj.doListObj, 'jxTreeCutNode');
                            this.drawingOrderList.removeChild(obj.doListObj);
                            obj.doListObj = null;
                        }
                        if (obj.gTreeItem) {
                            Element.removeClassName(obj.gTreeItem.domObj, 'jxTreeCutNode');
                            obj.gTreeItem.parent.remove(obj.gTreeItem);
                            obj.gTreeItem.finalize();
                            obj.gTreeItem = null;
                        }
                        if (obj.groups) {
                            this.obj.detachMapLayerGroup(obj);
                        } else {
                            this.obj.detachMapLayer(obj);
                        }
                        break;
                    case this.baseTab:
                        Element.removeClassName(obj.gTreeItem.domObj, 'jxTreeCutNode');
                        obj.gTreeItem.parent.remove(obj.gTreeItem);
                        obj.gTreeItem.finalize();
                        obj.gTreeItem = null;
                        if (obj.groups) {
                            this.obj.detachBaseMapGroup(obj);
                        } else {
                            this.obj.detachBaseMapLayer(obj);
                        }
                        break;
                }
            }
            //cut turns into copy once the item is removed from the tree/list
            this.activeTab.clipboard.operation = 'copy';
            /* clone object for insert operation */
            var clone = this.obj.cloneObject(obj);
            clone.parent = null;
            switch(this.activeTab) {
                case this.groupTab:
                    /* model: clone goes into parent */
                    var parent = null;
                    if (this.activeTab.currentSelection.data.groups) {
                        parent = this.activeTab.currentSelection.data;
                    } else {
                        parent = this.activeTab.currentSelection.data.parent;
                    }
                    if (clone.groups) {
                        this.obj.attachMapLayerGroup(clone, parent);
                    } else {
                        this.obj.attachMapLayer(clone, parent);
                    }
                    
                    /* view: clone goes into tree */
                    if (clone.groups) {
                        var groupOptions = {label: clone.name, isOpen: true, data: clone};
                        var folder = new JxTreeFolder(groupOptions);
                        folder.addSelectionListener(this);
                        parent.gTreeItem.append(folder);
                        clone.gTreeItem = folder;
                        this.renderGroupInTree(clone, folder);
                    } else {
                        var nodeOptions = {};
                        nodeOptions.imgIcon = 'images/tree_layer.png';
                        nodeOptions.label = clone.name;
                        nodeOptions.data = clone;
                        var node = new JxTreeItem(nodeOptions);
                        node.addSelectionListener(this);
                        parent.gTreeItem.append(node);
                        clone.gTreeItem = node;
                    }
                    
                    /* view: cloned layers also go into the drawing list */
                    var li = document.createElement('li');
                    var a = document.createElement('a');
                    a.href= '#';
                    a.className = 'LayerDefinition';
                    a.innerHTML = clone.name;
                    Event.observe(a,'click',this.selectionChanged.bind(this, li));
                    clone.doListObj = li;
                    li.data = clone;
                    li.appendChild(a);
                    this.drawingOrderList.appendChild(li);
                    
                    break;
                    
                case this.drawingTab:
                    /* model: clone moves in mapLayers but parent doesn't change */
                    this.obj.attachMapLayer(clone, this.activeTab.currentSelection.data);
                    clone.parent = this.activeTab.clipboard.group;
                    
                    var cloneIdx = this.obj.mapLayers.indexOf(clone);
                    var insertIdx = this.activeTab.clipboard.group.layers.length;
                    for (var i=0; i<this.activeTab.clipboard.group.layers.length; i++) {
                        if (cloneIdx < this.obj.mapLayers.indexOf(this.activeTab.clipboard.group.layers[i])) {
                            insertIdx = i;
                            break;
                        }
                    }
                    clone.parent.layers.splice(insertIdx,0,clone);
                    
                    /* view: insert into list before selected item */
                    var li = document.createElement('li');
                    var a = document.createElement('a');
                    a.href = '#';
                    a.className = 'LayerDefinition';
                    a.innerHTML = clone.name;
                    Event.observe(a,'click',this.selectionChanged.bind(this, li));
                    clone.doListObj = li;
                    li.data = clone;
                    li.appendChild(a);
                    this.drawingOrderList.insertBefore(li, this.activeTab.currentSelection.data.doListObj);
                    
                    /* create tree item for clone within group in the tree */
                    var nodeOptions = {};
                    nodeOptions.imgIcon = 'images/tree_layer.png';
                    nodeOptions.label = clone.name;
                    nodeOptions.data = clone;
                    var node = new JxTreeItem(nodeOptions);
                    node.addSelectionListener(this);
                    clone.gTreeItem = node;
                    
                    if (insertIdx == 0){
                        clone.parent.gTreeItem.insert(node);
                    } else {
                        clone.parent.gTreeItem.insert(node, clone.parent.gTreeItem.nodes[insertIdx-1]);
                    }
                    
                    break;
                case this.baseTab:
                    /* model: clone goes into parent */
                    var parent = null;
                    if (this.activeTab.currentSelection.data.groups) {
                        parent = this.activeTab.currentSelection.data;
                    } else {
                        parent = this.activeTab.currentSelection.data.parent;
                    }
                    if (clone.groups) {
                        this.obj.attachBaseMapGroup(clone);
                    } else {
                        this.obj.attachBaseMapLayer(clone, parent);
                    }
                
                    /* view: clone goes into tree */
                    if (clone.groups) {
                        var groupOptions = {label: clone.name, isOpen: true, data: clone};
                        var folder = new JxTreeFolder(groupOptions);
                        folder.addSelectionListener(this);
                        parent.gTreeItem.append(folder);
                        clone.gTreeItem = folder;
                        this.renderGroupInTree(clone, folder);
                    } else {
                        var nodeOptions = {};
                        nodeOptions.imgIcon = 'images/tree_layer.png';
                        nodeOptions.label = clone.name;
                        nodeOptions.data = clone;
                        var node = new JxTreeItem(nodeOptions);
                        node.addSelectionListener(this);
                        parent.gTreeItem.append(node);
                        clone.gTreeItem = node;
                    }
                    break;
            }
        }
    },
    dispatchMoveUp: function() {
        if (!this.activeTab.currentSelection || !this.activeTab.currentSelection.data) {
            return;
        }
        var obj = this.activeTab.currentSelection.data;
        var parent = obj.parent;
        var bMoved = false;
        switch(this.activeTab) {
            case this.groupTab:
                if (obj.groups) {
                    var idx = parent.groups.indexOf(obj);
                    if (idx > 0) {
                        parent.groups.splice(idx,1);
                        idx --;
                        parent.groups.splice(idx,0,obj);
                        bMoved = true;
                    }
                } else {
                    var idx = parent.layers.indexOf(obj);
                    if (idx > 0) {
                        parent.layers.splice(idx,1);
                        idx --;
                        parent.layers.splice(idx,0,obj);
                        bMoved = true;
                    }
                }
                if (bMoved) {
                    var insertIdx = parent.gTreeItem.nodes.indexOf(obj.gTreeItem) - 2;
                    parent.gTreeItem.remove(obj.gTreeItem);
                    if (insertIdx <= 0) {
                        parent.gTreeItem.insert(obj.gTreeItem);
                    } else {
                        parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[insertIdx]);
                    }
                }
                break;
            case this.drawingTab:
                var idx = this.obj.mapLayers.indexOf(obj);
                if (idx > 0) {
                    this.obj.mapLayers.splice(idx,1);
                    idx --;
                    this.obj.mapLayers.splice(idx,0,obj);
                    bMoved = true;
                    
                    this.drawingOrderList.removeChild(obj.doListObj);
                    this.drawingOrderList.insertBefore(obj.doListObj,this.drawingOrderList.childNodes[idx]);
                    
                    /* the position of the item in the tree may not change so
                     * this is pure lazy programming :o)
                     */
                    parent.gTreeItem.remove(obj.gTreeItem);
                    for (var i=0; i<parent.layers.length; i++) {
                        if (idx < this.obj.mapLayers.indexOf(parent.layers[i])) {
                            if (i==0) {
                                parent.gTreeItem.insert(obj.gTreeItem);
                            } else {
                                parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[idx-1]);
                            }
                            break;
                        }
                    }
                }
                break;
            case this.baseTab:
                if (obj.groups) {
                    var idx = parent.groups.indexOf(obj);
                    if (idx > 0) {
                        parent.groups.splice(idx,1);
                        idx --;
                        parent.groups.splice(idx,0,obj);
                        bMoved = true;
                    }
                } else {
                    var idx = parent.layers.indexOf(obj);
                    if (idx > 0) {
                        parent.layers.splice(idx,1);
                        idx --;
                        parent.layers.splice(idx,0,obj);
                        bMoved = true;
                    }
                }
                if (bMoved) {
                    var insertIdx = parent.gTreeItem.nodes.indexOf(obj.gTreeItem) - 2;
                    parent.gTreeItem.remove(obj.gTreeItem);
                    if (insertIdx <= 0) {
                        parent.gTreeItem.insert(obj.gTreeItem);
                    } else {
                        parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[insertIdx]);
                    }
                }               
                break;
        }
        if (bMoved) {
            this.obj.setDirty();
            this.activateActions();
        }
    },
    dispatchMoveDown: function() {
        if (!this.activeTab.currentSelection || !this.activeTab.currentSelection.data) {
            return;
        }
        var obj = this.activeTab.currentSelection.data;
        var parent = obj.parent;
        var bMoved = false;
        switch(this.activeTab) {
            case this.groupTab:
                if (obj.groups) {
                    var idx = parent.groups.indexOf(obj);
                    if (idx < parent.groups.length - 1) {
                        parent.groups.splice(idx,1);
                        idx ++;
                        parent.groups.splice(idx,0,obj);
                        bMoved = true;
                    }
                } else {
                    var idx = parent.layers.indexOf(obj);
                    if (idx < parent.layers.length - 1) {
                        parent.layers.splice(idx,1);
                        idx ++;
                        parent.layers.splice(idx,0,obj);
                        bMoved = true;
                    }
                }
                if (bMoved) {
                    var insertIdx = parent.gTreeItem.nodes.indexOf(obj.gTreeItem);
                    parent.gTreeItem.remove(obj.gTreeItem);
                    if (insertIdx < 0) {
                        parent.gTreeItem.insert(obj.gTreeItem);
                    } else {
                        parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[insertIdx]);
                    }
                }
                break;
            case this.drawingTab:
                var idx = this.obj.mapLayers.indexOf(obj);
                if (idx < this.obj.mapLayers.length - 1) {
                    this.obj.mapLayers.splice(idx,1);
                    idx ++;
                    this.obj.mapLayers.splice(idx,0,obj);
                    bMoved = true;
                    
                    this.drawingOrderList.removeChild(obj.doListObj);
                    this.drawingOrderList.insertBefore(obj.doListObj,this.drawingOrderList.childNodes[idx]);
                    
                    /* the position of the item in the tree may not change so
                     * this is pure lazy programming :o)
                     */
                    parent.gTreeItem.remove(obj.gTreeItem);
                    for (var i=0; i<parent.layers.length; i++) {
                        if (idx < this.obj.mapLayers.indexOf(parent.layers[i])) {
                            if (i==0) {
                                parent.gTreeItem.insert(obj.gTreeItem);
                            } else {
                                parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[idx-1]);
                            }
                            break;
                        }
                    }
                }
                break;
            case this.baseTab:
                if (obj.groups) {
                    var idx = parent.groups.indexOf(obj);
                    if (idx < parent.groups.length - 1) {
                        parent.groups.splice(idx,1);
                        idx ++;
                        parent.groups.splice(idx,0,obj);
                        bMoved = true;
                    }
                } else {
                    var idx = parent.layers.indexOf(obj);
                    if (idx < parent.groups.length - 1) {
                        parent.layers.splice(idx,1);
                        idx ++;
                        parent.layers.splice(idx,0,obj);
                        bMoved = true;
                    }
                }
                if (bMoved) {
                    var insertIdx = parent.gTreeItem.nodes.indexOf(obj.gTreeItem);
                    parent.gTreeItem.remove(obj.gTreeItem);
                    if (insertIdx < 0) {
                        parent.gTreeItem.insert(obj.gTreeItem);
                    } else {
                        parent.gTreeItem.insert(obj.gTreeItem, parent.gTreeItem.nodes[insertIdx]);
                    }
                }               
                break;
        }
        if (bMoved) {
            this.obj.setDirty();
            this.activateActions();
        }
    },       
    dispatchDelete: function() {
        if (!this.activeTab.currentSelection) {
            return;
        }
        var obj = this.activeTab.currentSelection.data;
        this.selectionChanged(null);
        
        if (obj.groups) {
            this.removeGroup(obj);
        }
        if (obj.gTreeItem) {
            obj.gTreeItem.parent.remove(obj.gTreeItem);
            obj.gTreeItem.finalize();
            obj.gTreeItem = null;
        }
        if (obj.doListObj) {
            this.drawingOrderList.removeChild(obj.doListObj);
            obj.doListObj = null;
        }
        switch(this.activeTab) {
            case this.groupTab:
            case this.drawingTab:
                if (obj.groups) {
                    this.obj.removeMapLayerGroup(obj);
                } else {
                    this.obj.removeMapLayer(obj);
                }
                break;
            case this.baseTab:
                if (obj.groups) {
                    this.obj.removeBaseMapGroup(obj);
                } else {
                    this.obj.removeBaseMapLayer(obj);
                }
                break;
        }
    }
});

var WSFeatureSourceView = Class.create();
Object.extend(WSFeatureSourceView.prototype, WSView.prototype);
Object.extend(WSFeatureSourceView.prototype, {
    oSelect: null,
    oUploadPane: null,
    bManaged: false,
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this.bSettingsRendered = false;
        this.bPropertiesRendered = false;
        this.bResourceReferencesRendered = false;
        
        this._oPropertiesPanel = new JxSnapPanel({label:'Properties - ' + o.getName() + ' ('+o.getType()+')', state:'closed', helpCallback: this.showHelpTopic.bind(this, 'help/panel_featuresourceproperties.html')});
        this._oPropertiesPanel.content.id = 'snapPropertiesPanel';
        this._oSettingsPanel = new JxSnapPanel({label:'Settings', helpCallback: this.showHelpTopic.bind(this, 'help/panel_featuresourcesettings.html')});
        this._oSettingsPanel.content.id = 'snapSettingsPanel';
        
        this.snapManager.add(this._oPropertiesPanel,
                         this._oSettingsPanel);
        this.loadPanels();
    },
    headerLoaded : function() {
        this.bHeaderLoaded = true;
        this.checkRenderStatus();
    },
    contentLoaded : function() {
        this.bContentLoaded = true;
        this.checkRenderStatus();
        //handle managed/unmanaged
        //this._oSettingsPanel.setContent(this._oSettingsPanel.getObj('data'));
    },
    contentChanged: function() {
        var title = 'Properties - ' + this.obj.getName() + ' ('+this.obj.getType()+')';
        if (this.obj.isDirty()) {
            title = 'Properties - <span class="modified">' + this.obj.getName() + ' ('+this.obj.getType()+')</span>';
        }
        this._oPropertiesPanel.setLabel(title);
    },
    resourceReferencesLoaded: function() {
        this.bResourceReferencesLoaded = true;
        this.checkRenderStatus();
    },
    loadPanels: function() {
        this.panelStatus = {properties:false,
                            settings:false,
                            referencedto:false,
                            referencedby:false};
        this._oPropertiesPanel.setContentURL('panels/featureproperties.html',
                                    this.panelLoaded.bind(this, 'properties'));
        this._oSettingsPanel.setContentURL(  'panels/featuresettings.html',
                                    this.panelLoaded.bind(this, 'settings'));
    },
    panelLoaded : function(name) {
        this.panelStatus[name] = true;
        switch(name){
            case 'properties':
                var ids = ['featuresourceid','type','location','owner','created','modified',
                       'references'];
                this._oPropertiesPanel.registerIds(ids, this._oPropertiesPanel.domObj);
                break;
            case 'referencedto':
                this._oPropertiesPanel.registerIds(['referencedto'], this._oPropertiesPanel.domObj);
                break;
            case 'referencedby':
                this._oPropertiesPanel.registerIds(['referencedby'], this._oPropertiesPanel.domObj);
                break;
            case 'settings':
                this._oSettingsPanel.registerIds(['data', 'providerType', 'dataFileList', 'uploadPane'], this._oSettingsPanel.domObj);
        }
        this.checkRenderStatus();
    },
    /**
     * checkRenderStatus
     * Feature sources have no preview and no references to sub-elements so
     * rendering of references is only for elements referenced by this object.
     */
    checkRenderStatus : function() {
        if (this.panelStatus['properties'] && this.panelStatus['referencedto'] && 
            this.panelStatus['referencedby'] && this.bResourceReferencesLoaded && 
            !this.bResourceReferencesRendered && this.bPropertiesRendered) {
            var refBy = this.obj.getResourceReferences();
            this.renderResourceReferences([],refBy);
            this.bResourceReferencesRendered = true;
        }
        if (this.panelStatus['properties'] && this.bHeaderLoaded && !this.bPropertiesRendered) {
            this.renderProperties();
            this.bPropertiesRendered = true;
        }
        if (this.panelStatus['settings'] && this.bContentLoaded && !this.bSettingsRendered) {
            this.renderSettings();
            this.bSettingsRendered = true;
        }
    },
    /**
     * settings html has loaded so fill in values and set up event handlers
     * unmanaged resources don't allow client upload so those controls are hidden
     * based on path type detection
     */
    renderSettings : function() {
        //TODO: use full provider name instead of short type throughout this class
        this.oSelect = this._oSettingsPanel.getObj('providerType');
        this._oSettingsPanel.getObj('data').value = this.obj.getContentValue('Value');
        this.bManaged = (this.obj.getContentValue('Value').indexOf('%') != 0)?false:true;
        this.setSelect(this.obj.getContentValue('Provider'));
        Event.observe(this._oSettingsPanel.getObj('providerType'), 'change', this._updateDataType.bind(this));
        if (this.bManaged) {
            //managed FeatureSource
            this._oSettingsPanel.getObj('data').disabled = true;
            //define onload that will set up the form for submissions
            Event.observe(this._oSettingsPanel.getObj('uploadPane'), "load", this._uploadPaneLoaded.bind(this));
            //set the src of the form subpanel to have it load
            this._oSettingsPanel.getObj('uploadPane').src = 'featureupload.html';
            this._enumerateData();
        } else {
            //unmanaged FeatureSource
            //hide the file upload controls
            this._oSettingsPanel.getObj('uploadPane').style.display = 'none';
            Event.observe(this._oSettingsPanel.getObj('data'), "keyup", this._updateLocation.bind(this));
            this._getFeatureSchema();
        }
    },
    /**
     * sets form variables for iframe subpanel to allow uploading data files
     */
    _uploadPaneLoaded: function() {
        //jxTrace('upload panel loaded');
        //TODO: handle Firefox js 'unreponsive script' timeout by chunking upload.
        this.oUploadPane = (this._oSettingsPanel.getObj('uploadPane').contentDocument)?
            this._oSettingsPanel.getObj('uploadPane').contentDocument:
            this._oSettingsPanel.getObj('uploadPane').document.frames['uploadPane'].document;
        //set the location to the form
        if (this.oUploadPane.location.href.indexOf("featureupload.html") == -1) {
            //refresh list of data files
            this.oUploadPane.onload = this._uploadPaneLoaded;
            this.oUploadPane.location = "featureupload.html";
            return;
        }
        this._enumerateData();
        //set form variables
        this.oUploadPane.getElementById('uploadForm').action = this.obj.app.getBroker().mapAgentURL;
        this.oUploadPane.getElementById('resource').value = this.obj.rid;
        var szSelectValue = this.oSelect.options[this.oSelect.selectedIndex].value;
        
        //DefaultFileLocation isn't handled by MgApplicationRepositoryManager::SetResourceData
        //in the server so leave this as file, even for shapefile uploads.
        //this.oUploadPane.getElementById('datatype').value = (szSelectValue == 'SHP')?'DefaultFileLocation':'File';
        this.oUploadPane.getElementById('datatype').value = 'File';
        
        //the form needs user and pass for authentication
        var oBroker = this.viewManager.app.getBroker();
        this.oUploadPane.getElementById('username').value = oBroker.user;
        this.oUploadPane.getElementById('password').value = oBroker.pass;
    },
    /**
     * retrieves server list of datafiles referenced by a managed FeatureSource
     */
    _enumerateData: function() {
        var r = new WSEnumerateResourceData(this.obj.getResourceID());
        var b = this.obj.app.getBroker();
        b.dispatchRequest(r,this._dataEnumerated.bind(this));
        
    },
    /**
     * parses resources returned by ENUMERATERESOURCEDATA call
     * and updates displayed list
     */
    _dataEnumerated: function(r) {
        if (r.status != 200) {
          //jxTrace('failed to EnumerateResourceData');
            return;
        }
        var oNode = new DomNode(r.responseXML);
        var oRd = oNode.findFirstNode('ResourceData');
        var oList = this._oSettingsPanel.getObj('dataFileList');
        while (oList.childNodes.length > 0) {
            oList.removeChild(oList.childNodes[0]);
        }
        
        var nFiles = 0;
        while (oRd) {
            var szName = oRd.getNodeText('Name');
            oRd = oNode.findNextNode('ResourceData');
            var li = document.createElement('li');
            var a = document.createElement('a');
            a.innerHTML = szName;
            li.appendChild(a);
            oList.appendChild(li);
            nFiles++;
        }
        
        //disable the data type selector if there are any data files in the resource - it
        //isn't practical to change type once files are in the repository
        if (nFiles) {
            this._oSettingsPanel.getObj('providerType').disabled = true;
        }
        //if this is an SDF file, append it to the data location and update the xml
        //setFilePath will mark the document as dirty.
        //TODO: handle upload of a replacement file
        if (nFiles == 1 &&
            this.oSelect.options[this.oSelect.selectedIndex].value == 'SDF' &&
            this._oSettingsPanel.getObj('data').value.indexOf(szName) == -1){
            this._oSettingsPanel.getObj('data').value += szName;
            this.obj.setFilePath(this._oSettingsPanel.getObj('data').value);
        }
        
    },
    /**
     * performs DESCRIBESCHEMA on unmanaged resources because those
     * can't be enumerated by _enumerateData
     */
    _getFeatureSchema: function() {
        var r = new WSDescribeSchema(this.obj.getResourceID());
        var b = this.obj.app.getBroker();
        b.dispatchRequest(r,this._schemaReturned.bind(this));
        
    },
    /**
     * parses xml of source's schema, picking out data file names
     */
    _schemaReturned: function(r) {
        if (r.status != 200) {
          //jxTrace('failed to EnumerateResourceData');
            return;
        }
        var oNode = new DomNode(r.responseXML);
        var oRd = oNode.findFirstNode('xs:element');
        var oList = this._oSettingsPanel.getObj('dataFileList');
        while (oList.childNodes.length > 0) {
            oList.removeChild(oList.childNodes[0]);
        }
        
        while (oRd) {
            var szName = oRd.getAttribute('name');
            //TODO: fix this hack which is needed to avoid grabbing internal
            //      elements from individual schemas
            if (oRd.getAttribute('substitutionGroup')) {
                var li = document.createElement('li');
                var a = document.createElement('a');
                a.innerHTML = szName;
                li.appendChild(a);
                oList.appendChild(li);
            }            
            oRd = oNode.findNextNode('xs:element');
        }
    },
    /**
     * change the path an unmanaged featureSource refers to
     * 
     * updateLocation is only valid if the featureSource is unmanaged
     * (not in the repository). There is no validation to ensure the path
     * points to valid data or even an available directory
     *
     */
    _updateLocation: function() {
        this.obj.setContentValue('Value', this._oSettingsPanel.getObj('data').value);
        this.obj.setDirty();
        //TODO: save the featuresource in session to allow ENUMERATEDATA
        //or warn user to save and reload to see the change
        //this.obj.setProvider();
    },
    _updateDataType: function() {
        var szSelectValue = this.oSelect.options[this.oSelect.selectedIndex].value;
        this.obj.setProvider(szSelectValue);
        this.obj.setContentValue('Provider', 'OSGeo.' + szSelectValue + '.3.0');
        //update form variables if this is a managed source
        if (this.oUploadPane) {
            this.oUploadPane.getElementById('datatype').value = (szSelectValue == 'SHP')?'DefaultFileLocation':'File';
            this._enumerateData();
        }
    },
    setSelect: function(value) {
        var select = this.oSelect;
        for (var i=0; i<select.options.length; i++) {
            if (value.indexOf(select.options[i].value) != -1) {
                select.selectedIndex = i;
                break;
            }
        }
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.obj.removeContentListener(this);
        
        this.baseFinalize();
    }
});

var WSWebLayoutView = Class.create();
Object.extend(WSWebLayoutView.prototype, WSView.prototype);
Object.extend(WSWebLayoutView.prototype, {
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this._oPropertiesPanel = new JxSnapPanel({label:'Properties - ' + o.getName() + ' ('+o.getType()+')'});
        this._oPropertiesPanel.content.id = 'snapPropertiesPanel';
        this._oPreviewPanel = new JxSnapPanel({label:'Preview'});
        this._oPreviewPanel.content.className = 'snapContent noScrolling';
        this.snapManager.add(this._oPropertiesPanel,
                             this._oPreviewPanel);
        this.loadPanels();
        this.preview();
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.obj.removeContentListener(this);
        this.baseFinalize();
    },
    loadPanels: function() {
        this.panelStatus = {properties:false, referencedto:false, referencedby:false};
        this._oPropertiesPanel.setContentURL('panels/featureproperties.html', this.panelLoaded.bind(this, 'properties'));
    },
    resourceReferencesLoaded: function() {
        //jxTrace('WSWebLayoutView::resourceReferencesLoaded()');
        this.bResourceReferencesLoaded = true;
        this.checkRenderStatus();
    },
    headerLoaded : function() {
        //jxTrace('WSWebLayoutView::headerLoaded()');
        this.bHeaderLoaded = true;
        this.checkRenderStatus();
    },
    contentChanged: function() {
        var title = 'Properties - ' + this.obj.getName() + ' ('+this.obj.getType()+')';
        if (this.obj.isDirty()) {
            title = 'Properties - <span class="modified">' + this.obj.getName() + ' ('+this.obj.getType()+')</span>';
        }
        this._oPropertiesPanel.setLabel(title);
    },
    panelLoaded : function(name) {
        //jxTrace('WSWebLayoutView::panelLoaded('+name+')');
        this.panelStatus[name] = true;
        switch(name){
            case 'properties':
                var ids = ['featuresourceid','type','location','owner','created','modified', 'references'];
                this._oPropertiesPanel.registerIds(ids, this._oPropertiesPanel.domObj);
                break;
            case 'referencedto':
                this._oPropertiesPanel.registerIds(['referencedto'], this._oPropertiesPanel.domObj);
                break;
            case 'referencedby':
                this._oPropertiesPanel.registerIds(['referencedby'], this._oPropertiesPanel.domObj);
                break;
        }
        this.checkRenderStatus();
    },
    checkRenderStatus : function() {
        if (this.panelStatus['properties'] && this.panelStatus['referencedto'] && 
            this.panelStatus['referencedby'] && this.bResourceReferencesLoaded && 
            !this.bResourceReferencesRendered && this.bPropertiesRendered) {
            var refTo = this.obj.getContentValue('ResourceId');
            var refBy = this.obj.getResourceReferences();
            this.renderResourceReferences([refTo],refBy);
            this.bResourceReferencesRendered = true;
        }
        if (this.panelStatus['properties'] && this.bHeaderLoaded && !this.bPropertiesRendered) {
            this.renderProperties();
            this.bPropertiesRendered = true;
        }
    },
    preview: function() {
        var b = this.obj.app.getBroker();
        if (this._oPreviewPanel.content.childNodes.length == 0) {
            //this._oPreviewPanel.setBusy(true);
            this.mapFrame = document.createElement('iframe');
            this._oPreviewPanel.content.appendChild(this.mapFrame);
            this.mapFrame.className = 'previewFrame';
            this.mapFrame.frameBorder = '0';
            this.mapFrame.marginHeight = '0';
            this.mapFrame.marginWidth = '0';
            Event.observe(this._oPreviewPanel, 'load', this._oPreviewPanel.setBusy.bind(this, false));
            this._oPreviewPanel.content.appendChild(this.mapFrame);
        }
        this.mapFrame.src = b.mapGuideURL +
                        'mapviewerphp/ajaxviewer.php?' +
                        'WEBLAYOUT=' + this.obj.getResourceID() +
                        '&USERNAME=' + b.user +
                        '&PASSWORD=' + b.pass;
    }
});

var WSHtmlView = Class.create();
Object.extend(WSHtmlView.prototype, WSView.prototype);
Object.extend(WSHtmlView.prototype, {
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this._oPropertiesPanel = new JxSnapPanel({label:o.getName()});
        this._oPropertiesPanel.content.id = 'snapPropertiesPanel';
        this.snapManager.add(this._oPropertiesPanel);
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.baseFinalize();
    }
});

var WSIframeView = Class.create();
Object.extend(WSIframeView.prototype, WSView.prototype);
Object.extend(WSIframeView.prototype, {
    initialize : function(o, openIn, onload) {
        this.baseInit(o, openIn);
        this._oPropertiesPanel = new JxSnapPanel({label:o.getName()});
        this._oPropertiesPanel.content.className = 'snapContent noScrolling';
        this.snapManager.add(this._oPropertiesPanel);
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'previewFrame';
        this.iframe.frameBorder = '0';
        this.iframe.marginHeight = '0';
        this.iframe.marginWidth = '0';
        Event.observe(this.iframe, 'load', onload);
        this._oPropertiesPanel.content.appendChild(this.iframe);
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.baseFinalize();
    },
    setIframeURL: function(url) {
        this.iframe.src = url;
    }
});

/**
 * @class WSUnsupportedView
 * comment This is a placeholder view for objects that are not editable by
 * studio lite. Properties are displayed.
 * 
 */
var WSUnsupportedView = Class.create();
Object.extend(WSUnsupportedView.prototype, WSView.prototype);
Object.extend(WSUnsupportedView.prototype, {
    initialize : function(o, openIn) {
        this.baseInit(o, openIn);
        this._oPropertiesPanel = new JxSnapPanel({label:'Properties - ' + o.getName() + ' ('+o.getType()+')'});
        this._oUnsupportedPanel = new JxSnapPanel({label:'Unsupported Resource'});
        this._oUnsupportedPanel.setContentURL('panels/unsupported.html', this.panelLoaded.bind(this, 'unsupported'));
        this.snapManager.add(this._oPropertiesPanel, this._oUnsupportedPanel);
        this.loadPanels();
    },
    loadPanels: function() {
        this.panelStatus = {properties:false, referencedto:false, referencedby:false};
        this._oPropertiesPanel.setContentURL('panels/featureproperties.html', this.panelLoaded.bind(this, 'properties'));
    },
    resourceReferencesLoaded: function() {
        //jxTrace('WSUnsupportedView::resourceReferencesLoaded()');
        this.bResourceReferencesLoaded = true;
        this.checkRenderStatus();
    },
    headerLoaded : function() {
        //jxTrace('WSUnsupportedView::headerLoaded()');
        this.bHeaderLoaded = true;
        this.checkRenderStatus();
    },
    contentChanged: function() {
        var title = 'Properties - ' + this.obj.getName() + ' ('+this.obj.getType()+')';
        if (this.obj.isDirty()) {
            title = 'Properties - <span class="modified">' + this.obj.getName() + ' ('+this.obj.getType()+')</span>';
        }
        this._oPropertiesPanel.setLabel(title);
    },
    panelLoaded : function(name) {
        //jxTrace('WSUnsupportedView::panelLoaded('+name+')');
        this.panelStatus[name] = true;
        switch(name){
            case 'properties':
                var ids = ['featuresourceid','type','location','owner','created','modified', 'references'];
                this._oPropertiesPanel.registerIds(ids, this._oPropertiesPanel.domObj);
                break;
            case 'referencedto':
                this._oPropertiesPanel.registerIds(['referencedto'], this._oPropertiesPanel.domObj);
                break;
            case 'referencedby':
                this._oPropertiesPanel.registerIds(['referencedby'], this._oPropertiesPanel.domObj);
                break;
        }
        this.checkRenderStatus();
    },
    finalize: function() {
        //TODO: clean up all the panels then call baseFinalize
        this.obj.removeContentListener(this);
        
        this.baseFinalize();
    },
    /**
     * checkRenderStatus
     * Unsupported elements have no preview and no references to sub-elements so
     * rendering of references is only for elements referenced by this object.
     */
    checkRenderStatus : function() {
        if (this.panelStatus['properties'] && this.panelStatus['referencedto'] && 
            this.panelStatus['referencedby'] && this.bResourceReferencesLoaded && 
            !this.bResourceReferencesRendered && this.bPropertiesRendered) {
            var refBy = this.obj.getResourceReferences();
            this.renderResourceReferences([],refBy);
            this.bResourceReferencesRendered = true;
        }
        if (this.panelStatus['properties'] && this.bHeaderLoaded && !this.bPropertiesRendered) {
            this.renderProperties();
            this.bPropertiesRendered = true;
        }
    }
});
