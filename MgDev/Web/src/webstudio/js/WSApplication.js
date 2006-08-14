/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSApplication.js,v 1.123 2006/06/14 15:09:12 zak Exp $
 * @fileoverview    this file contains classes managing the Web Studio
 *                  application
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
 * The WSApplication class is the main controller for the Web Studio 
 * application.  The trigger to starting the application happens in
 * index.html by just creating a new instance of WSApplication.
 *
 * WSApplication kick-starts the application in its initialize function
 * by reading the config.xml file, the projections.xml file, and creating
 * the user interface.
 *
 * Once the application is running, the main purpose of WSApplication is
 * to dispatch user input from the main menu, toolbar and the site explorer.
 * The rest of the interface is handled by the WSResourceManager (for the 
 * open resources panel) and WSView sub-classes (for resources that are
 * being edited)
 */
var WSApplication = Class.create();

WSApplication.prototype = {
    /** 
     *all resources that the application knows about from enumerating the
     * repository are kept in this list for convenient access
     */
    _oMasterObjectList : [],
    
    /**
     * an instance of WSBroker, used to communicate with the mapagent cgi
     * interface of MapGuide
     */
    _oBroker : null,
    
    /**
     * an instance of WSFactory, used to generate run-time objects
     * representing resource documents.
     */
    _oFactory : null,
    
    /**
     * a JxTree instance used in the site explorer for displaying and
     * navigating the repository.
     *
     * TODO: this breaks the architecture of WSApplication.  It would be
     * better to move this into a separate class that handled the
     * site explorer.
     */
    _oTree : null,
    
    /**
     * this is an anonymous object used to store a reference to the last
     * resource the user cut or copied so that the paste operation can
     * do the right thing.
     */
    _oClipboard : null,
    
    /**
     * an instance of WSResourceManager which manages open resources on
     * behalf of the application.
     */
    _oResourceManager : null,
    
    /**
     * a JxSnapPanel instance that contains the site explorer.
     *
     * TODO:  This is currently managed by WSApplication but should
     * probably be deferred to another class to manage as per the
     * comments with _oTree.  However, WSApplication would still
     * maintain this reference to it since it actually creates it.
     */
    _oTreePane : null,

    /** 
     * a JxSnapPanel instance that contains the open resources list.  
     * The _oResourceManager manages the content.
     */
    _oOpenResourcePanel : null,
    
    /**
     * a reference to an instance of WSIframePage that allows embedding
     * the MapGuide Open Source Map Agent frameset inside a panel.  This
     * is only instantiated when first requested.  The reference is kept
     * to ensure it is only instantiated once.
     */
    _oMapAgent : null,
    
    /**
     * a reference to an instance of WSHtmlPage that allows embedding
     * of the Getting Started page in a panel.  This is instantiated
     * once when the application is started and then a reference is
     * kept so we don't have to create it again.
     */
    _oGettingStarted: null,
    
    /**
     * a reference to an instance of WSIframePage that allows embedding
     * the help system inside a panel.  This is only instantiated once
     * when the help system is first started so we don't have to create
     * it again.
     */
    _oHelp: null,
    
    /**
     * a reference to an instance of WSIframePage that allows embedding
     * the MapGuide Open Source Site Administrator frameset inside a panel.
     * This is only instantiated when first requested.  The reference is
     * kept to ensure it is only instantiated once.
     */
    _oSiteAdministrator: null,
    
    /**
     * when the application connects to a new site, it requests a session
     * so that temporary state can be stored without needing to save
     * into the main library.  This is that session id returned by the
     * server.  When the server is disconnected, this is set to null.
     * Use getSession() to access the session id safely.
     */
    _szSessionId : null,
    
    /**
     * this is an internal list of available projections, loaded from
     * projections.xml.
     */
    _aoProjections: null,
    
    /**
     * a reference to a WSFileDialog.  We only really need one
     * copy of this dialog since it is designed to be reused.
     *
     * TODO: Other objects have their own copies, but if they could reference
     * this one, it would be better.
     */ 
    _oFileDialog: null,
    
    /**
     * construct a new instance of WSApplication.  This initializes run time
     * objects, loads the config and coordinate systems, and creates the
     * user interface.
     */
    initialize : function()
    {
        //jxTrace( 'WSApplication::initialize()' );
        this._oClipboard = {};
        this.loadConfig();
        this.loadCoordinateSystems();
        this._oBroker = new WSBroker()
        this._oFactory = new WSFactory(this);
        this.createDisplay();
        this._oFileDialog = new WSFileDialog({app:this});
        this._oHelp = new WSIframePage({title:'Help Guide', url: 'help/index.html', app: this, onloadCallback: this.helpLoaded.bind(this)});
    },
    /**
     * load the configuration file from the server asynchronously.  When the
     * file has been loaded, it is processed by _configLoaded().
     */
    loadConfig: function() {
      //jxTrace('WSApplication::loadConfig()');
        var ts = 'ts=' + (new Date()).getTime();
        new Ajax.Request( 'config.xml', 
            Object.extend({method:'get', onComplete:this._configLoaded.bind(this), parameters: ts}));
    },
    /**
     * loade the coordinate systems from projections.xml asynchronously.
     * When the file has been loaded, it is processed by 
     * _coordinateSystemsLoaded().
     */
    loadCoordinateSystems: function() {
        //jxTrace('WSApplication::loadCoordinateSystems()');
        var ts = 'ts=' + (new Date()).getTime();
        new Ajax.Request( 'projections.xml', 
            Object.extend({method:'get', onComplete:this._coordinateSystemsLoaded.bind(this), parameters: ts}));
    },
    /**
     * called when the configuration file has been loaded.  This is the queue
     * to launch the open site dialog for the first time.
     *
     * @param r {Object} the XmlHttpRequest object that loaded the config
     *        file.
     */
    _configLoaded: function(r) {
      //jxTrace('WSApplication::configLoaded()');
        this.config = new DomNode(r.responseXML);
        this.defaultSite = this.config.getNodeText('DefaultSite');
        this.defaultUserName = this.config.getNodeText('DefaultUserName');
        this.defaultTechnology = this.config.getNodeText('DefaultTechnology');
        this.dispatchOpenSite();
    },
    /**
     * called when the projections file has been loaded.  It is processed
     * into an internal array for use by other objects.
     *
     * @param r {Object} the XmlHttpRequest object that loaded the projections
     *        file.
     *
     * TODO: create a formal WSCoordinateSystem object instead of using
     * an anonymous object.
     */
    _coordinateSystemsLoaded: function(r) {
        //jxTrace('WSApplication::projectionsLoaded()');
        this._aoCoordinateSystems = [];
        var root = new DomNode(r.responseXML);
        var proj = root.findFirstNode('projection');
        while(proj) {
            var o = {};
            o.description = proj.getNodeText('description');
            o.epsg = proj.getNodeText('epsg');
            o.wkt = proj.getNodeText('wkt');
            this._aoCoordinateSystems.push(o);
            proj = root.findNextNode('projection');
        }
    },
    /**
     * accessor function to retrieve the list of coordinate systems
     * that the application knows about.
     *
     * @return {Array} an array of coordinate system objects.
     */
    getCoordinateSystems: function() {
        return this._aoCoordinateSystems;
    },
    /**
     * callback from connecting to a site.  We check for error codes
     * to see if something went wrong.  If it didn't go wrong, then
     * we set the session id (since that is what we requested) and
     * enumerate the root of the library node to give the user
     * something to interact with.
     *
     * @param r {Object} the XmlHttpRequest object that requested
     *        the session id.
     *
     * TODO: more robust error handling?
     */
    _setSession : function( r ) {
        //jxTrace( 'WSApplication::_setSession("'+r.responseText+'")' );
        if (r.status == '401' || r.status == '559') {
            alert( 'invalid user name or password' );
        } else if (r.status == '404') {
            alert( 'the URL you provided does not exist.  Please check and try again.' );
        } else if (r.status != '200') {
            alert( 'an unknown error occurred with status ' + r.status + ' and message ' + r.responseText);
        } else {
            this._szSessionId = r.responseText;
            this._oTreePanel.setBusy(true);
            var r = new WSEnumerateResources('Library://', '', 0);
            this._oBroker.dispatchRequest(r,this.populateTree.bind(this));
        }
        this.updateActions();
    },
    /**
     * return the session id for the current connection
     *
     * @return {String} the session id
     */
    getSession: function() {
        return this._szSessionId;
    },
    /**
     * called when the user has used the Open Site dialog.  This updates
     * _oBroker with the new connection information and sends of a 
     * request for a session id with the new server.  The result is
     * handled by _setSession().
     *
     * @param szURL {String} url to a mapguide site
     * @param szUser {String} the user name to use to connect to the site
     * @param szPassword {String} the password to use to connect to the site.
     */
    connectToSite : function(szURL, szUser, szPassword) {
        //jxTrace( 'WSApplication::connectToSite()' );
        this._oBroker.setSiteURL(szURL, szUser, szPassword);
        this._oBroker.dispatchRequest(new WSCreateSession(), this._setSession.bind(this));
    },
    /**
     * creates the user interface for the application.  There is a
     * substantial amount of work to do including:
     * - creating JxAction instances for everything in the menu and
     *   main toolbar
     * - creating the main menu and associated items
     * - creating the toolbar and associated items
     * - creating the context menu for the tree and associated items
     * - creating the JxHorizontalSplitter for the main layout
     * - creating the site explorer and open resources snap panels
     * - creating the JxTree for the site explorer
     * - creating the resource manager
     * - launching the Getting Started guide for the first time.
     */
    createDisplay : function()
    {
        //jxTrace( 'WSApplication::createDisplay()' );
        this._oClipboard = {};
        /* create actions to handle menu/toolbar items */
        this._oOpenResourceAction = new JxAction(this.dispatchOpenResource.bind(this));
        this._oNewFolderAction = new JxAction(this.dispatchNewFolder.bind(this));
        this._oNewLayerAction = new JxAction(this.dispatchNewLayer.bind(this));
        this._oNewMapAction = new JxAction(this.dispatchNewMap.bind(this));
        this._oNewFeatureSourceAction = new JxAction(this.dispatchNewFeatureSource.bind(this));
        this._oNewUnmanagedFeatureSourceAction = new JxAction(this.dispatchNewUnmanagedFeatureSource.bind(this));
        this._oOpenAction = new JxAction(this.dispatchOpen.bind(this));
        this._oCloseAction = new JxAction(this.dispatchClose.bind(this));
        this._oCloseAllAction = new JxAction(this.dispatchCloseAll.bind(this));
        this._oSaveAction = new JxAction(this.dispatchSave.bind(this));
        this._oSaveAsAction = new JxAction(this.dispatchSaveAs.bind(this));
        this._oSaveAllAction = new JxAction(this.dispatchSaveAll.bind(this));
        this._oRenameAction = new JxAction(this.dispatchRename.bind(this));
        this._oOpenSiteAction = new JxAction(this.dispatchOpenSite.bind(this));
        this._oCloseSiteAction = new JxAction(this.dispatchCloseSite.bind(this));
        this._oCutAction = new JxAction(this.dispatchCut.bind(this));
        this._oCopyAction = new JxAction(this.dispatchCopy.bind(this));
        this._oPasteAction = new JxAction(this.dispatchPaste.bind(this));
        this._oDeleteAction = new JxAction(this.dispatchDelete.bind(this));
        this._oRefreshSiteAction = new JxAction(this.dispatchRefreshSite.bind(this));
        this._oSiteAdministratorAction = new JxAction(this.dispatchSiteAdministrator.bind(this));
        this._oMapAgentAction = new JxAction(this.dispatchMapAgent.bind(this));
        this._oOptionsAction = new JxAction(this.dispatchOptions.bind(this));
        this._oHelpAboutAction = new JxAction(this.dispatchAbout.bind(this));
        this._oHelpGuideAction = new JxAction(this.dispatchGuide.bind(this));
        this._oHelpGettingStartedAction = new JxAction(this.dispatchGettingStarted.bind(this));
        this._oOpenFromAction = new JxAction(this.dispatchOpenFromMenu.bind(this));
        this._oDummyAction = new JxAction(this.dispatchDummy.bind(this));
        
        /* the main menu */
        this.menu = new JxMenuBar('menuBar');
        var fileMenu = new JxMenu('File');
        var editMenu = new JxMenu('Edit');
        var toolMenu = new JxMenu('Tools');
        this._oWindowMenu = new JxMenu('Window');
        var helpMenu = new JxMenu('Help');
        
        var fileNew = new JxMenu('New ...');
        var newFolder = new JxMenuItem('Folder', this._oNewFolderAction);
        var newLayer = new JxMenuItem('Layer', this._oNewLayerAction);
        var newMap = new JxMenuItem('Map', this._oNewMapAction, true);
        var newFeatureSource = new JxMenuItem('Feature Source', this._oNewFeatureSourceAction);
        var newUnmanagedFeatureSource = new JxMenuItem('Unmanaged Feature Source', this._oNewUnmanagedFeatureSourceAction);
        fileNew.add(newFolder, newLayer, newMap, newFeatureSource, newUnmanagedFeatureSource);
        
        var fileOpenResource = new JxMenuItem('Open Resource', this._oOpenResourceAction);
        var fileClose = new JxMenuItem('Close', this._oCloseAction, true);
        var fileSave = new JxMenuItem('Save', this._oSaveAction);
        var fileSaveAs = new JxMenuItem('Save As', this._oSaveAsAction);
        var fileSaveAll = new JxMenuItem('Save All', this._oSaveAllAction);
        var fileRename = new JxMenuItem('Rename', this._oRenameAction, true);
        var fileOpenSite = new JxMenuItem('Open Site', this._oOpenSiteAction);
        var fileRefreshSite = new JxMenuItem('Refresh Site', this._oRefreshSiteAction);
        var fileCloseSite = new JxMenuItem('Close Site', this._oCloseSiteAction, true);
        var fileRecentResources = new JxMenuItem('Recent Resources', this._oDummyAction);
        var fileRecentSites = new JxMenuItem('Recent Sites', this._oDummyAction);
        
        fileMenu.add(fileNew, fileOpenResource, fileClose, fileSave, fileSaveAs, fileSaveAll, 
                     fileRename, fileOpenSite, fileRefreshSite, fileCloseSite, fileRecentResources, fileRecentSites);
        
        var editCut = new JxMenuItem('Cut', this._oCutAction);
        var editCopy = new JxMenuItem('Copy', this._oCopyAction);
        var editPaste = new JxMenuItem('Paste', this._oPasteAction);
        var editDelete = new JxMenuItem('Delete', this._oDeleteAction, true);
        
        editMenu.add( editCut, editCopy, editPaste, editDelete );
        
        var toolSiteAdministrator = new JxMenuItem('Site Administrator', this._oSiteAdministratorAction);
        var toolMapAgent = new JxMenuItem('MapAgent', this._oMapAgentAction, true);
        var toolOptions = new JxMenuItem('Options', this._oOptionsAction );
        toolMenu.add(toolSiteAdministrator, toolMapAgent, toolOptions);
        
        this._oWindowCloseAll = new JxMenuItem('Close All Resources', this._oCloseAllAction, true );
        this._oWindowMenu.add(this._oWindowCloseAll);
        
        var helpGettingStarted = new JxMenuItem('Getting Started ...', this._oHelpGettingStartedAction);
        var helpStudioGuide = new JxMenuItem('MapGuide Open Source Web Studio Help', this._oHelpGuideAction, true);
        var helpAbout = new JxMenuItem('About MapGuide Open Source Web Studio', this._oHelpAboutAction);
        helpMenu.add(helpGettingStarted, helpStudioGuide, helpAbout);
        
        this.menu.add( fileMenu, editMenu, toolMenu, this._oWindowMenu, helpMenu );
        
        //context menu
        editRename = new JxMenuItem('Rename', this._oRenameAction, true);
        var editOpen = new JxMenuItem('Open', this._oOpenAction, true);
        editCut = new JxMenuItem('Cut', this._oCutAction);
        editCopy = new JxMenuItem('Copy', this._oCopyAction);
        editPaste = new JxMenuItem('Paste', this._oPasteAction, true);
        editDelete = new JxMenuItem('Delete', this._oDeleteAction);
        
        fileNew = new JxMenu('New ...');
        newFolder = new JxMenuItem('Folder', this._oNewFolderAction);
        newLayer = new JxMenuItem('Layer', this._oNewLayerAction);
        newMap = new JxMenuItem('Map', this._oNewMapAction, true);
        newFeatureSource = new JxMenuItem('Feature Source', this._oNewFeatureSourceAction);
        newUnmanagedFeatureSource = new JxMenuItem('Unmanaged Feature Source', this._oNewUnmanagedFeatureSourceAction);
        fileNew.add(newFolder, newLayer, newMap, newFeatureSource, newUnmanagedFeatureSource);
        this.treeContextMenu = new JxContextMenu();
        this.treeContextMenu.add(editRename, fileNew, editOpen, editCut, editCopy, editPaste, editDelete);
        
        var serverItem = new JxToolbarItem('images/icon_sites.png', 'Open Site ...', this._oOpenSiteAction, true);
        //var newItem = new JxToolbarItem('images/icon_new.png', 'New Resource ...', this._oNewResourceAction);
        var openItem = new JxToolbarItem('images/icon_open.png', 'Open Resource ...', this._oOpenResourceAction);
        var saveItem = new JxToolbarItem('images/icon_save.png', 'Save Resource', this._oSaveAction);
        var saveAllItem = new JxToolbarItem('images/icon_saveall.png', 'Save All ', this._oSaveAllAction, true);
        var cutItem = new JxToolbarItem( 'images/icon_cut.png', 'Cut', this._oCutAction );
        var copyItem = new JxToolbarItem( 'images/icon_copy.png', 'Copy', this._oCopyAction );
        var pasteItem = new JxToolbarItem( 'images/icon_paste.png', 'Paste', this._oPasteAction );
        var refreshItem = new JxToolbarItem('images/icon_refresh.png', 'Refresh Site', this._oRefreshSiteAction);
        var helpItem = new JxToolbarItem('images/icon_help.png', 'Help', this._oHelpGuideAction);

        /* the main toolbar */
        this.toolbar = new JxToolbar('toolBar');
        this.toolbar.add(serverItem, openItem, saveItem, saveAllItem, cutItem,
                         copyItem, pasteItem, refreshItem, helpItem); //debugItem);
        
        /* layout main interface split panels and sizing constraints */
        new JxEdgeConstraint( JxPageBottomEdge,new JxBottomEdge('thePage') );
        new JxEdgeConstraint( new JxBottomEdge('toolBar'), 
                              new JxTopEdge('thePage') );
        
        $('thePage').style.width = Position.getPageDimensions().width + 'px';
        
        //create the splitter
        var hSplitter = new JxHorizontalSplitter('thePage', 200);
        hSplitter.elements[1].className = 'jxHorizontalSplitter';
        hSplitter.elements[1].id = 'horizontalSplitter';
        JxSizeConstraint.apply(hSplitter.elements[1], 10, 'auto');

        this._leftPane = hSplitter.elements[0];
        this._leftPane.id = 'leftPane';
        this._rightPane = hSplitter.elements[2];
        this._rightPane.id = 'rightPane';

        this._leftSnaps = new JxSnapManager(this._leftPane);
        
        JxConstraintManager.initialize();
        /* the constraint manager will call windowSizeChanged on this
         * object if the window size changes.  Its up to the application
         * to decide what to do about this.
         */
        JxConstraintManager.addWindowSizeListener(this);

        /* snap panels for the site explorer and open resources */
        this._oTreePanel = new JxSnapPanel({label:'Site Explorer', helpCallback: this.showHelpTopic.bind(this, 'panel_siteexplorer.html')});
        this._oTreePanel.content.id = 'snapTreePanel';
        this._oOpenResourcePanel = new JxSnapPanel({label:'Open Resources', helpCallback: this.showHelpTopic.bind(this, 'panel_openresources.html')});
        this._oOpenResourcePanel.content.id = 'snapOpenResourcePanel';
        this._leftSnaps.add(this._oTreePanel, this._oOpenResourcePanel);

        /* create tree control and trigger initial population */ 
        this._oTree = new JxTree(this._oTreePanel.content);
        JxConstraintManager.apply();
        
        /* initialize the resource manager now that it has a user interface
         * element to work with
         */
        this._oResourceManager = new WSResourceManager(this, this._oOpenResourcePanel.content, this._rightPane);
        this._oResourceManager.addSelectionListener(this);
        this._oResourceManager.addResourceListener(this);

        /* open the getting started guide and update the menu and toolbar actions */
        this.dispatchGettingStarted();
        this.updateActions();
    },
    /**
     * show a help topic in the embedded help system
     * @param topic {String} the url to the topic page to display
     */
    showHelpTopic: function(topic) {
        this._oHelp.topic = topic;
        if (this._oResourceManager.currentResource == this._oHelp) {
            this.helpLoaded();
        } else {
            //ensure the help system is loaded and current
            this.dispatchGuide();
        }
    },
    /**
     * handle the help system being loaded.  This could come from an
     * asynchoronous request when the help system is first loaded, or
     * from a user event if the system is already loaded.  In either
     * case, we reach into the dom to call the setHelpTopic function
     * inside the help system.
     */
    helpLoaded: function() {
        //load the actual help page requested
        this._oHelp.getView().iframe.contentWindow.setHelpTopic(this._oHelp.topic);
    },
    /**
     * this ensures that the actions available to the application
     * are properly enabled or disabled for the current application
     * state.  Menus and toolbar items are managed by their action
     * so disabling the action will update the user interface
     * appropriately.
     *
     * If you add new actions to the application, make sure to put
     * the appropriate code in here.  If you add new functions that
     * change the state of the application, make sure you call this
     * function to ensure everything is processed correctly.
     */
    updateActions : function() {
        
        if (!this._oClipboard.node) {
            this._oPasteAction.setEnabled(false);
        } else {
            this._oPasteAction.setEnabled(true);            
        }
        if (!this._oCurrentNode) {
            this._oRenameAction.setEnabled(false);
            this._oCutAction.setEnabled(false);
            this._oCopyAction.setEnabled(false);
            this._oPasteAction.setEnabled(false);
            this._oDeleteAction.setEnabled(false);
        } else {
            var rm = this.getResourceManager()
            var bIsOpen = rm.isResourceOpen(this._oCurrentNode.data.getResourceID());
            //open resources can't be renamed
            if (bIsOpen) {
                this._oRenameAction.setEnabled(false);
                this._oCutAction.setEnabled(false);
                this._oCopyAction.setEnabled(false);
                this._oDeleteAction.setEnabled(false);
            } else {
                this._oRenameAction.setEnabled(true);
                this._oCutAction.setEnabled(true);
                this._oCopyAction.setEnabled(true);
                this._oDeleteAction.setEnabled(true);
                
            }
            /* don't add paste here.  Its valid for paste to
             * be disabled if something is selected, but
             * its not valid if there is no where to paste it
             */
        }
        
        var rm = this.getResourceManager();
        this._oSaveAction.setEnabled(rm.isCurrentResourceDirty());
        this._oSaveAllAction.setEnabled(rm.isAnyResourceDirty());
        
        this._oRefreshSiteAction.setEnabled(this._szSessionId != null);
        this._oOpenSiteAction.setEnabled(this._szSessionId == null);

        this._oOpenResourceAction.setEnabled(this._szSessionId != null);

        //disable close and close all when no resources are open
        if (rm.currentResource) {
            this._oSaveAsAction.setEnabled(!(rm.currentResource instanceof WSHtmlPage || rm.currentResource instanceof WSIframePage));
            this._oCloseAction.setEnabled(true);
            this._oCloseAllAction.setEnabled(true);
        } else {
            this._oSaveAsAction.setEnabled(false);
            this._oCloseAction.setEnabled(false);
            this._oCloseAllAction.setEnabled(false);
        }
        //enable always-available items
        this._oNewFolderAction.setEnabled(true);
        this._oNewLayerAction.setEnabled(true);
        this._oNewMapAction.setEnabled(true);
        this._oNewFeatureSourceAction.setEnabled(true);
        this._oNewUnmanagedFeatureSourceAction.setEnabled(true);
        
        //disable unimplemented menu items
        this._oOptionsAction.setEnabled(false);
        this._oDummyAction.setEnabled(false);
    },
    /**
     * handle the user resizing the window by giving the various user
     * interface components a chance to resize.
     *
     * This function is called by constraint manager because we are
     * a windowSizeListener.
     */ 
    windowSizeChanged: function() {
        //jxTrace('WSApplication:windowSizeChanged()');
        $('thePage').style.width = Position.getPageDimensions().width + 'px';
        this._leftSnaps.resize();
        this._oResourceManager.resize();
        //we've changed the size of thePage so make the constraint manager
        //fix up the layout
        JxConstraintManager.apply();
    },
    /* TODO: seems to be unused.  I think this is old code. Confirm and delete.
    bindToAction : function( item, action) {
        //jxTrace('WSApplication::bindToAction()');
        item.addActionListener(action);
        action.addPropertyChangeListener(item);
    },
    */
    /**
     * refresh a node in the tree, recursively processing all
     * open folders.  If no node is passed, process the whole
     * tree from the root node.
     *
     * @param oNode {JxTreeFolder} an instance of JxTreeFolder
     *        to refresh from, or null.
     */
    refreshTreeNode: function(oNode) {
        if (!oNode) {
            oNode = this._oTree.findChild(['Library://']);
        }
        if (oNode instanceof JxTreeFolder) {
            if (oNode.isOpen) {
                this.disclosed(oNode);
                for (var i=0; i<oNode.nodes.length; i++) {
                    this.refreshTreeNode(oNode.nodes[i]);
                }
            }
        }
    },
    /**
     * the application is a disclosureListener on the JxTreeFolder
     * instances in the tree.  This gets called when the user
     * expands or collapses a folder in the tree.  If they've opened
     * a folder, we dispatch an asynchronous request to enumerate that
     * folder.  Enumeration results are handled by populateTree.
     *
     * @param oNode {JxTreeFolder} the folder that was disclosed.
     */
    disclosed: function(oNode) {
        //jxTrace('WSApplication::disclosed()');
        if (oNode.isOpen) {
            this._oTreePanel.setBusy(true);
            if (oNode.data) {
                //disclose from branch
                var r = new WSEnumerateResources(oNode.data.getResourceID(), '', 1);
                this._oBroker.dispatchRequest(r,this.populateTree.bind(this));    
            } else {
                //disclose the root node
                var r = new WSEnumerateResources('Library://', '', 1);
                this._oBroker.dispatchRequest(r,this.populateTree.bind(this));    
            }
        }
    },
    /**
     * add a new resource to the application.  This updates the master object
     * list and inserts the resource into the current tree view.
     *
     * @param oResource {WSBaseObj} the object to insert
     *
     * @return {Boolean} true if the object was added, false if not.
     */
    addResource : function(oResource) {
        //jxTrace('WSApplication::addResource() - ' + o.ctype + ' ' + o.rid);
        var aPath = [];
        for (var i=0; i<oResource._path.length; i++) {
            aPath[i] = oResource._path[i];
        }
        var root = this._oTree.findChild(aPath);
        //we can only add it if there is an existing path to add it to.
        if (!root && oResource.getPath() == "Library://")
            root = this._oTree;
        if (root) {
            var item = null;
            if (oResource.ctype == 'folder') {
                //jxTrace( 'WSApplication::addResouce() ctype is folder, path is '+o.path);
                if (oResource.getName() == "Library://") {
                    //jxTrace('WSApplication::addResource() using server icon');
                    item = new JxTreeFolder( { label:oResource.getName(), 
                                               data: oResource, 
                                               imgTreeFolder: 'images/tree_server.png',
                                               imgTreeFolderOpen: 'images/tree_server.png', 
                                               /*contextMenu: this.treeContextMenu,*/
                                               isOpen: true } );
                } else {
                    //jxTrace('WSApplication::addResource() using folder icon');
                    item = new JxTreeFolder( { label:oResource.getName(), 
                                               data: oResource, 
                                               contextMenu: this.treeContextMenu } );
                }
                item.addDisclosureListener(this);
            } else {
                var imgIcon = 'images/tree_page.png';
                if (oResource.ctype == 'LayerDefinition') {
                    imgIcon = 'images/tree_layer.png';
                } else if (oResource.ctype == 'MapDefinition') {
                    imgIcon = 'images/tree_map.png';
                } else if (oResource.ctype == 'FeatureSource') {
                    imgIcon = 'images/tree_data.png';
                } else if (oResource.ctype == 'SymbolLibrary') {
                    imgIcon = 'images/tree_symbol.png';
                } else if (oResource.ctype == 'WebLayout') {
                    imgIcon = 'images/tree_layout.png';
                } else if (oResource.ctype == 'LoadProcedure') {
                    imgIcon = 'images/tree_load.png';
                } else if (oResource.ctype == 'PrintLayout') {
                    imgIcon = 'images/tree_print.png';
                }
                item = new JxTreeItem({label:oResource.getName(),
                                       data: oResource, 
                                       imgIcon: imgIcon, 
                                       contextMenu: this.treeContextMenu });
            }
            item.addSelectionListener(this);
            root.append(item);
            oResource.treeItem = item;
            this._oMasterObjectList.push(oResource);
            return true;
        } else {
            return false;
        }
    },
    /**
     * remove an object from the application.  This updates the master
     * object list and removes the node and any listeners from the tree.
     *
     * @param oResource {WSBaseObj} the object to remove
     *
     * @return {Boolean} true if the item was removed, false if it wasn't.
     */ 
    removeResource : function(oResource) {
        //jxTrace('WSApplication::removeResource()');
        var idx = null;
        for (var i=0; i < this._oMasterObjectList.length; i++) {
            var obj = this._oMasterObjectList[i];
            if (oResource.getResourceID() == obj.getResourceID()) {
                idx = i;
            }
        }
        
        if (idx) {
            this._oMasterObjectList.splice(idx, 1);
            //remove listeners
            if (oResource.treeItem.removeDisclosureListener) 
                oResource.treeItem.removeDisclosureListener(this);
            oResource.treeItem.removeSelectionListener(this);
            //remove the node from the parent
            oResource.treeItem.parent.remove(oResource.treeItem);
            return true;
        } else {
            //error -- not found
        }
        return false;
    },
    /**
     * searches the master object list for resources that are
     * in a particular folder.
     *
     * @param szPath {String} the path to search
     * @result {Array} an array of objects that are in this path
     */
    findResources : function(szPath) {
        //jxTrace('WSApplication::findResource()');
        var a = [];
        for (var i=0; i<this._oMasterObjectList.length; i++) {
            var o = this._oMasterObjectList[i];
            if (o.getResourceID() != szPath && o.szPath == szPath) {
                a.push(o);
            }
        }
        return a;
    },
    /** 
     * populate the master object list with new objects from the repository
     * based on the enumeration of some folder in the repository.
     *
     * We need the enumeration target so we ensure it gets passed back by
     * including it when we bind.
     *
     * The WSFactory is used to generate real objects to represent each
     * ResourceFolder or ResourceDocument in the resulting XML.
     *
     * This function handles synchronizing the master object list and the
     * associated tree view as part of the update process.
     *
     * @param r {Object} the XmlHttpRequest object
     * @param json {Object} a JSON object provided by prototype automatically,
     *        we ignore it.
     * @param o {Object} the original request so we can get the resource
     *        id that we need to update in the tree
     *
     * TODO: provide better error handling if something goes wrong.
     */
    populateTree : function(r, json, o)
    {
        //jxTrace('WSApplication::populateTree()');
        
        //this is the path in the repository that is being enumerated
        var updatePath = o.originalRequest.parameters.resourceid;
        o.originalRequest = null; //allow garbage collection
        //track if we should open the first level
        var bOpenLibrary = false;
        
        // these are all the objects we already know about in that path
        var aExisting = this.findResources(updatePath);
        
        if(!r.responseXML) {
            alert( 'ERROR: ' + r.statusText ) 
        } else{
            //a is an array of new objects created from the XML
            var a = this._oFactory.process(r.responseXML);
        }
        for (var i = 0; i< a.length; i++) {
            var bFound = false;
            for (var j=0; j<aExisting.length;j++) {
                if (a[i].getResourceID() == aExisting[j].getResourceID()) {
                    bFound = true;
                    aExisting.splice(j,1);
                    break;
                }
            }
            if (!bFound) {
                this.addResource(a[i]);
                if (a[i].name == 'Library://') {
                    bOpenLibrary = true;
                }
            }
        }
        //anything left in aExisting needs to be removed
        for (var i=0; i<aExisting.length; i++)
            this.removeResource(aExisting[i]);
        
        this._oTreePanel.setBusy(false);
        
        /* on first load, we should expand the library node.  
         * Its just being nice to the user!
         */
        if (bOpenLibrary) {
            this._oTreePanel.setBusy(true);
            var r = new WSEnumerateResources('Library://', '', 1);
            this._oBroker.dispatchRequest(r,this.populateTree.bind(this));
        }
    },
    /* menu handlers */
    dispatchNewResource : function(o) {
        //jxTrace('WSApplication::dispatchNewResource()');
    },
    dispatchOpenResource : function(o) {
        //jxTrace('WSApplication::dispatchOpenResource()');
        this._oFileDialog.openResource({handler:this.handleOpenResource.bind(this)}); 
    },
    handleOpenResource: function(button, dialog) {
        if (button == 'Open') {
            this._oResourceManager.addResource(dialog.getResource());
        }
        dialog.close();
    },
    /**
     * generate an enumerate resources request so that any changed resources
     * can be processed by the resource manager. This is needed for new resources
     * created from templates.
     */
     openResourceFromRepository: function(rid) {
         var r = new WSEnumerateResources(rid, '', 0);
         this._oBroker.dispatchRequest(r, this.handleOpenResource.bind(this));
     },
     /**
      * pass enumerated resources to the resource manager for addition to the
      * application and view
      */
     handleOpenResource: function(r, json, o) {
         if(!r.responseXML) {
             alert( 'ERROR: ' + r.statusText ) 
         } else{
                 //a is an array of new objects created from the XML
                 var a = this._oFactory.process(r.responseXML);
         }
         for (var i=0; i<a.length; i++) {
             this.addResource(a[i]);
             a[i].addContentListener(this);
             this.getResourceManager().addResource(a[i]);
         }
     },
     
    /**
     * get a string representing the path to the current node
     * or the root if there is no current node
     * 
     * @return {String} szPath the path
     */
    getCurrentPath: function() {
        var szPath = 'Library://';
        if (this._oCurrentNode && (this._oCurrentNode.data.getName() != 'Library://')) {
            szPath = this._oCurrentNode.data.getPath();
            if (this._oCurrentNode.data.getType() == 'folder') {
                szPath += this._oCurrentNode.data.getName() + '/';
            }
        }
        return szPath;
    },
    /**
     * handle the New Folder action by creating a new folder where the
     * current selection is in the site explorer tree.  
     *
     * TODO: make sure the folder name is unique so this doesn't fail.
     */
    dispatchNewFolder : function() {
        /*new folder should be:
         *   sibling of a selected resource
         *   child of a selected folder 
         *   child of library if library or none is selected
         */
        var szRid = "Library://New Folder/";
        var oNode = null;
        var szName = '';
        if (this._oCurrentNode && (this._oCurrentNode.data.getName() != 'Library://')) {
            var currentNode = this._oCurrentNode.data;
            szName = (currentNode.getType() == 'folder')?currentNode.getName() + '/':'';
            szRid = currentNode.getPath() + szName + 'New Folder/';
            oNode = this._oCurrentNode.parent;
            //TODO: disclose folder
            //this.disclosed(_oCurrentNode);
        }
        var r = new WSSetResource(szRid, null, null);
        this._oBroker.dispatchRequest(r,this.refreshTreeNode.bind(this, oNode));
    },
    /**
     * handle the New Layer action by creating a new layer where the
     * current selection is in the site explorer tree.  This launches
     * the file dialog so the user can select a name for the layer.
     *
     * The actual creation of the layer is done in handleNewLayer in
     * case the user cancels.
     *
     * TODO: It would be nice to create unnamed resources but that
     *       requires some changes to WSFactory and WSView.
     */
    dispatchNewLayer : function(o) {
        //jxTrace('WSApplication::dispatchNewLayer()');
        /**
         * set a path to open, appending the name if it is a folder
         * then dispatch a dialog with type 'layer'
         */
        var szPath = 'Library://';
        if (this._oCurrentNode && (this._oCurrentNode.data.getName() != 'Library://')) {
            szPath = this._oCurrentNode.data.getPath();
            if (this._oCurrentNode.data.getType() == 'folder') {
                szPath += this._oCurrentNode.data.getName() + '/';
            }
        }
        var szType = 'LayerDefinition';
        //open save as dialog with appropriate defaults
        this._oFileDialog.openSaveAs({handler:this.handleNewLayer.bind(this),
                                    types:this._oFileDialog.getFilter(szType),
                                    path:szPath});      
        
    },
    /**
     * The user has clicked a button in the dialog.  If they clicked ok,
     * create the new layer.  This triggers an asynchronous load of an
     * xml template of a new layer from the server.  openNewLayer is
     * called when the template is ready.
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {WsFileDialog} the dialog in which the button was 
     *        clicked.
     */
    handleNewLayer: function(szButton, oDialog) {
        if (szButton == 'Save') {
            var ts = 'ts=' + (new Date()).getTime();

            new Ajax.Request( 'templates/LayerDefinition.xml', 
                             {method: 'get',
                              onSuccess : this.openNewLayer.bind(this, oDialog.getResource()),
                              parameters: ts});
        }
        oDialog.close();
    },
    /**
     * A template for a new layer has been loaded from the server,
     * now we can actually put this into the repository using the
     * broker.  When the broker finishes the request, the resource
     * object must be built before it can be added to the resource manager.
     * openResourceFromRepository is called to handle that process.
     *
     * @param rid {String} the resource id where we want to push the template
     * @param r {Object} the XmlHttpRequest object with the layer template.
     */
     openNewLayer: function(rid, r){
         var oLayer = new WSSetResource(rid, r.responseText, null);
         var oRm = this.getResourceManager();
         this._oBroker.dispatchRequest(oLayer, this.openResourceFromRepository.bind(this, rid));
    },
    /**
     * handle the New Map action by creating a new map where the
     * current selection is in the site explorer tree.  This launches
     * the file dialog so the user can select a name for the map.
     *
     * The actual creation of the map is done in handleNewMap in
     * case the user cancels.
     *
     * TODO: It would be nice to create unnamed resources but that
     *       requires some changes to WSFactory and WSView.
     */
    dispatchNewMap : function() {
        //jxTrace('WSApplication::dispatchNewMap()');
        /**
         * set a path to open, appending the name if it is a folder
         * then dispatch a dialog with type 'map'
         */
        var szPath = 'Library://';
        if (this._oCurrentNode && (this._oCurrentNode.data.getName() != 'Library://')) {
            szPath = this._oCurrentNode.data.getPath();
            if (this._oCurrentNode.data.getType() == 'folder') {
                szPath += this._oCurrentNode.data.getName() + '/';
            }
        }
        var szType = 'MapDefinition';
        //open save as dialog with appropriate defaults
        this._oFileDialog.openSaveAs({handler:this.handleNewMap.bind(this),
                                    types:this._oFileDialog.getFilter(szType),
                                    path:szPath});      
        
    },
    /**
     * The user has clicked a button in the dialog.  If they clicked ok,
     * create the new map.  This triggers an asynchronous load of an
     * xml template of a new map from the server.  openNewMap is
     * called when the template is ready.
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {WsFileDialog} the dialog in which the button was 
     *        clicked.
     */
    handleNewMap: function(szButton, oDialog) {
        if (szButton == 'Save') {
            var ts = 'ts=' + (new Date()).getTime();

            new Ajax.Request( 'templates/MapDefinition.xml', 
                             {method: 'get',
                              onSuccess : this.openNewMap.bind(this, oDialog.getResource()),
                              parameters: ts
                              });
        }
        oDialog.close();
    },
    /**
     * A template for a new map has been loaded from the server,
     * now we can actually put this into the repository using the
     * broker.  When the broker finishes the request, openResourceFromRepository
     * is called.
     *
     * @param rid {String} the resource id where we want to push the template
     * @param r {Object} the XmlHttpRequest object with the map template.
     */
    openNewMap: function(rid, r){
        var map = new WSSetResource(rid, r.responseText, null);
        this._oBroker.dispatchRequest(map,this.openResourceFromRepository.bind(this, rid));
    },
    /**
     * handle the New Feature Source action by creating a new feature
     * source where the current selection is in the site explorer tree.  
     * This launches the file dialog so the user can select a name for the 
     * feature source.
     *
     * The actual creation of the feature source is done in handleNewMap in
     * case the user cancels.
     *
     * TODO: It would be nice to create unnamed resources but that
     *       requires some changes to WSFactory and WSView.
     */
    dispatchNewFeatureSource : function() {
        //jxTrace('WSApplication::dispatchNewFeatureSource()');
        /**
         * set a path to open
         * then dispatch a dialog with type 'FeatureSource'
         */
        var szPath = this.getCurrentPath();
        var szType = 'FeatureSource';
        this._oFileDialog.unManagedFeatureSource = false;
        //open save as dialog with appropriate defaults
        this._oFileDialog.openSaveAs({handler:this.handleNewFeatureSource.bind(this),
                                    types:this._oFileDialog.getFilter(szType),
                                    path:szPath});      
    },
    /**
     * The user has clicked a button in the dialog.  If they clicked ok,
     * create the new feature source.  This triggers an asynchronous 
     * load of an xml template of a new feature source from the server.
     * openNewFeatureSource is called when the template is ready.
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {WsFileDialog} the dialog in which the button was 
     *        clicked.
     */
    handleNewFeatureSource: function(szButton, oDialog){
        if (szButton == 'Save') {
            var ts = 'ts=' + (new Date()).getTime();

            new Ajax.Request( 'templates/FeatureSource.xml', {method: 'get',
                onSuccess : this.openNewFeatureSource.bind(this, oDialog.getResource()),
                parameters: ts});
        }
        oDialog.close();
    },
    /**
     * A template for a new feature source has been loaded from the server,
     * now we can actually put this into the repository using the
     * broker.  When the broker finishes the request, openResourceFromRepository
     * is called.
     *
     * @param rid {String} the resource id where we want to push the template
     * @param r {Object} the XmlHttpRequest object with the map template.
     */
    openNewFeatureSource: function(rid, r) {
        if (r.status != 200) { return; }
        var szXML = new String(r.responseText);
        if (this._oFileDialog.unManagedFeatureSource) {   
            //unmanaged features don't need the readonly attribute
            var oFSDom = new DomNode(r.responseXML.childNodes[0]);
            oFSDom.setNodeText('Value','enter path');
            szXML = oFSDom.toXML();
        }
        var oFeatureSource = new WSSetResource(rid, szXML, null);
        var rm = this.getResourceManager();
        this._oFileDialog.unManagedFeatureSource = null;
        this._oBroker.dispatchRequest(oFeatureSource, this.openResourceFromRepository.bind(this, rid));        
    },
    /**
     * handle the New Unmanaged Feature Source action by creating a new 
     * feature source where the current selection is in the site explorer 
     * tree. This launches the file dialog so the user can select a name for 
     * the feature source.
     *
     * The actual creation of the feature source is done in handleNewMap in
     * case the user cancels.
     *
     * TODO: It would be nice to create unnamed resources but that
     *       requires some changes to WSFactory and WSView.
     */
    dispatchNewUnmanagedFeatureSource : function() {
        //jxTrace('WSApplication::dispatchNewUnmanagedFeatureSource()');
            /**
         * set a path to open
         * then dispatch a dialog with type 'FeatureSource'
         */
        var szPath = this.getCurrentPath();
        var szType = 'FeatureSource';
        this._oFileDialog.unManagedFeatureSource = true;
        //open save as dialog with appropriate defaults
        this._oFileDialog.openSaveAs({handler:this.handleNewFeatureSource.bind(this),
                                    types:this._oFileDialog.getFilter(szType),
                                    path:szPath});      
    },
    /**
     * Open the currently selected resource by passing it to the
     * resource manager.
     */
    dispatchOpen : function() {
        //jxTrace('WSApplication::dispatchOpen()');
        if (this._oCurrentNode) {
            this._oResourceManager.addResource(this._oCurrentNode.data);
            this._oCurrentNode.data.addContentListener(this);
        }
    },
    /**
     * Cut the currently selected resource.  This puts it on the
     * clip board and changes its visual appearance in the site
     * explorer.  Nothing actually happens to the resource unless
     * the user pastes it somewhere.
     */
    dispatchCut : function() {
        //jxTrace('WSApplication::dispatchCut()');
        if (this._oCurrentNode) {
            if (this._oCurrentNode.parent) {
                Element.removeClassName(this._oCurrentNode.domObj.childNodes[2], 'jxTreeSelectedNode');
                Element.addClassName(this._oCurrentNode.domObj, 'jxTreeCutNode');
                this._oClipboard.node = this._oCurrentNode;
                this._oCurrentNode = null;
                this._oClipboard.operation = 'cut';
                this.updateActions();
            }
        }
    },
    /**
     * Copy the currently selected resource.  This puts it on the
     * clip board and changes its visual appearance in the site
     * explorer.  Nothing actually happens to the resource unless
     * the user pastes it somewhere.
     */
    dispatchCopy : function() {
        //jxTrace('WSApplication::dispatchCopy()');
        if (this._oCurrentNode) {
            this._oClipboard.node = this._oCurrentNode;
            this._oClipboard.operation = 'copy';
            this.updateActions();
        }
    },
    /**
     * Paste the last resource that was put onto the clipboard.  What
     * happens to the resource depends on whether it was cut or copied.
     *
     * The broker is used to actually move or copy the resource.
     */
    dispatchPaste : function() {
        //jxTrace('WSApplication::dispatchPaste()');
        if (this._oCurrentNode && this._oClipboard.node) {
            var sourceID = this._oClipboard.node.data.getResourceID();
            //jxTrace('WSApplication::dispatchPaste() sourceID is ' + sourceID);
            var name = this._oClipboard.node.data.getName();
            var type = this._oClipboard.node.data.getType();
            var destinationID = '';
            /* if the destination is a folder, we have to calculate the path a little
             * differently by including the name of the folder in the path 
             */
            if (this._oCurrentNode.data.getType() == 'folder') {
                if (this._oCurrentNode.data.getPath() == 'Library://') {
                    destinationID = 'Library://'+name+'.'+type;
                } else {
                    destinationID = this._oCurrentNode.data.getPath() + 
                                    this._oCurrentNode.data.getName() + '/' + name + '.' + type;
                }
            } else {
                destinationID = this._oCurrentNode.data.getPath() + name + '.' + type;
            }
            
            //sanity check - are we pasting on top of the resource we copied/cut from?
            if (destinationID == sourceID) {
                var bFoundNewId = false;
                var newDestinationID = destinationID;
                var idx = destinationID.lastIndexOf('.');
                var start = destinationID.substring(0,idx);
                var end = destinationID.substring(idx);
                var i = 2;
                while(!bFoundNewId) {
                    newDestinationID = start + ' ('+i+')' + end;
                    bFoundNewId = true;
                    for (var j=0; j<this._oCurrentNode.nodes.length; j++) {
                        if (this._oCurrentNode.nodes[j].data.getResourceID() == newDestinationID) {
                            bFoundNewId = false;
                            break;
                        }
                    }
                    i++;
                }
                destinationID = newDestinationID;
            }
            //TODO check permissions on target node path?
            switch (this._oClipboard.operation) {
                case 'copy':
                    //jxTrace('WSApplication::dispatchPaste(): copy resource from ' + sourceID + ' to ' + destinationID);
                    var r = new WSCopyResource(sourceID, destinationID, true);
                    if (this._oCurrentNode.data.getType() == 'folder') {
                        this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oCurrentNode));
                    } else {
                        this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oCurrentNode.parent));
                    }
                    break;
                case 'cut':
                    if (!this.confirmReferenceChangeDialog) {
                        var size = Position.getPageDimensions();
                        var o = {
                            title: 'Confirm Resource Reference Changes',
                            id: 'confirmReferenceChangeDialog',
                            contentURL : 'dialogs/confirmreferencechange.html',
                            width: 500,
                            height: 350,
                            top: (size.height-350)/2,
                            left: (size.width-500)/2,
                            handler: this.handleConfirmReferenceChange.bind(this, sourceID, destinationID)
                        };
                        this.confirmReferenceChangeDialog = new JxDialog( o );
                    }
                    this.confirmReferenceChangeDialog.onOpen = this.confirmReferencesOpen.bind(this, this.confirmReferenceChangeDialog, sourceID);
                    this.confirmReferenceChangeDialog.open();
                    break;
            }
        }
    },
    /**
     * The user is being prompted to confirm an action that might invalidate
     * one or more resources in the repository.  Enumerate the references
     * using the broker.  referencesLoaded updates the dialog with the
     * references.
     *
     * @param oDialog {JxDialog} the dialog to put the resource references
     *        into
     * @param szSourceId {String} the id of the resource to get the references
     *        to from the broker.
     */
    confirmReferencesOpen: function(oDialog, szSourceID) {
        oDialog.registerIds(['references'], oDialog.domObj);
        //enumerate references for the sourceID
        this._oTreePanel.setBusy(true);
        var r = new WSEnumerateResourceReferences(szSourceID);
        var b = this.getBroker();
        b.dispatchRequest(r, this.referencesLoaded.bind(this, oDialog));
    },
    /**
     * the server has enumerated the references to a resource.  Populate
     * the associated dialog box with the references so the user
     * knows what might be affects.
     *
     * @param oDialog {JxDialog} the dialog to put the references in
     * @param r {Object} the XmlHttpRequest object that contains the references
     *
     * TODO: this should all be moved into a WSDialog instance, including
     * the utility functions that are used by this.
     */
    referencesLoaded : function (oDialog, r) {
        if (r.responseXML){
          //add references to the table in the panel
          var root = new DomNode(r.responseXML);
          var references = [];
                var node = root.findFirstNode('ResourceId');
                while(node) {
                    references.push(node.textContent);
                    node = root.findNextNode('ResourceId');
                }
                table = oDialog.getObj('references');
                for (var i=0; i<references.length; i++) {
                    row = this.createResourceReferenceRow(references[i]);
                    table.appendChild(row);
                }            
        }
        this._oTreePanel.setBusy(false);
    },
    /**
     * utility function to parse a resource id into its path, name and type.
     * 
     * @param szResourceId {String} the resource ID to parse.
     * @return {Object} an object 
     */
    parseResourceId : function( szResourceId) {
        var result = {};
        var idx = szResourceId.lastIndexOf('/');
        result.path = szResourceId.slice(0,idx+1);
        var pair = szResourceId.slice(idx+1);
        idx = pair.lastIndexOf('.');
        result.name = pair.slice(0,idx);
        result.type = pair.slice(idx+1);
        return result;
    },
    /**
     * create a table row inside the resource references dialog
     *
     * @param szResourceId {String} the resource to add
     */
    createResourceReferenceRow: function( szResourceId ) {
        ref = this.parseResourceId(szResourceId);
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
    /**
     * handle the user requesting to delete a resource.  The user has
     * to confirm this action, but its different if we are deleting a
     * folder or a document.
     */
    dispatchDelete : function(o) {
        //jxTrace('WSApplication::dispatchDelete()');
        if (this._oCurrentNode instanceof JxTreeFolder) {
            if (!this.confirmDeleteFolderDialog) {
                var size = Position.getPageDimensions();
                var o = {
                    title: 'Confirm Delete Folder',
                    id: 'confirmDeleteDialog',
                    contentURL : 'dialogs/confirmdeletefolder.html',
                    width: 400,
                    height: 100,
                    top: (size.height-400)/2,
                    left: (size.width-100)/2,
                    handler: this.handleConfirmDelete.bind(this),
                    buttons: ['ok','cancel']
                };
                this.confirmDeleteFolderDialog = new JxDialog( o );
            }
            this.confirmDeleteFolderDialog.open();
        } else {
            if (!this.confirmDeleteDialog) {
                var size = Position.getPageDimensions();
                var o = {
                    title: 'Confirm Delete Resource',
                    id: 'confirmDeleteDialog',
                    contentURL : 'dialogs/confirmdelete.html',
                    width: 500,
                    height: 350,
                    top: (size.height-500)/2,
                    left: (size.width-350)/2,
                    handler: this.handleConfirmDelete.bind(this),
                    buttons: ['ok','cancel']
                };
                this.confirmDeleteDialog = new JxDialog( o );
            }
            this.confirmDeleteDialog.onOpen = this.confirmReferencesOpen.bind(this, this.confirmDeleteDialog, this._oCurrentNode.data.getResourceID());
            this.confirmDeleteDialog.open();
        }
    },
    /**
     * handle the user confirming deleting a resource.
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {WsFileDialog} the dialog in which the button was 
     *        clicked.
     */
    handleConfirmDelete : function(szButton, oDialog) {
        oDialog.close();
        var table = oDialog.getObj('references');
        if (table) {
            while(table.childNodes.length > 0) {
                table.removeChild(table.firstChild); 
            }
        }
        if (szButton == 'ok') {
            Element.removeClassName(this._oCurrentNode.domObj.childNodes[2], 'jxTreeSelectedNode');
            Element.addClassName(this._oCurrentNode.domObj.childNodes[2], 'jxTreeCutNode');
            var r = new WSDeleteResource(this._oCurrentNode.data.getResourceID());
            this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oCurrentNode.parent));
        }
    },
    /**
     * handle the user confirming to move a resource
     *
     * @param szSourceID {String} the resource to move
     * @param szDestinationID {String} where to move it
     * @param szButton {String} the button that was clicked
     * @param oDialog {WsFileDialog} the dialog in which the button was 
     *        clicked.
     */
    handleConfirmReferenceChange : function(szSourceID, szDestinationID, szButton, oDialog ) {
        //jxTrace('WSApplication::handleConfirmReferenceChange() ' + source + ' to ' + destination);
        oDialog.close();
        var table = oDialog.getObj('references');
        if (table) {
            while(table.childNodes.length > 0) {
                table.removeChild(table.firstChild); 
            }
        }
        if (szButton == 'ok') {
            Element.removeClassName(this._oClipboard.node.domObj, 'jxTreeCutNode');
            var r = new WSMoveResource(szSourceID, szDestinationID, true);
            /* refresh destination */
            if (this._oCurrentNode.data.getType() == 'folder') {
                this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oCurrentNode));
            } else {
                this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oCurrentNode.parent));
            }
            /* refresh source */
            if (this._oCurrentNode.parent != this._oClipboard.node.parent) {
                this._oBroker.dispatchRequest(r,this.disclosed.bind(this, this._oClipboard.node.parent));
            }
        }
    },
    /**
     * show the about dialog.
     */
    dispatchAbout: function() {
        if (!this.aboutDialog) {
            var size = Position.getPageDimensions();
            
            var o = {
                title: 'About MapGuide Open Source Studio Lite',
                id: 'aboutDialog',
                contentURL : 'dialogs/about.html',
                width: 400,
                height: 400,
                top: (size.height-400)/2,
                left: (size.width-400)/2
            };
            this.aboutDialog = new JxDialog( o );
        }
        this.aboutDialog.show();
    },
    /**
     * close the currently selected resource.  The resource manager actually
     * does this.
     */
    dispatchClose : function() {
        //jxTrace('WSApplication::dispatchClose()');
        this.getResourceManager().closeCurrentResource();
    },
    /**
     * close all open resources.  The resource manager actually does this.
     */
    dispatchCloseAll : function() {
        //jxTrace('WSApplication::dispatchCloseAll()');
        this.getResourceManager().closeAllResources();
    },
    /**
     * save the currently selected resource.  The resource manager actually
     * does this.
     */
    dispatchSave : function() {
        //jxTrace('WSApplication::dispatchSave()');
        this.getResourceManager().saveCurrentResource();      
    },
    /**
     * save the currently selected resource under a new name.  The resource
     * manager actually does this.
     */
    dispatchSaveAs : function() {
        //jxTrace('WSApplication::dispatchSaveAs()');       
        this.getResourceManager().saveCurrentResourceAs();      
    },
    /**
     * save all open resources.  The resource manager actually does this
     */
    dispatchSaveAll : function() {
        //jxTrace('WSApplication::dispatchSaveAll()');       
        this.getResourceManager().saveAllResources();      
    },
    /**
     * rename the currently selected resource. The resource can not be
     * currently open (this is enforced by updateActions).
     */
    dispatchRename: function() {
        //jxTrace('WSApplication::dispatchRename()');
        if (!this._oCurrentNode) {
            return;
        }
        var domObj = this._oCurrentNode.data.treeItem.domObj.childNodes[2];
        var editor = new WSInPlaceEditor(domObj, null,
                            {okButton:false, highlightcolor:null,
                             formId:'editor_field_form',
                             onComplete:this.handleRename.bind(this)});
        domObj.editor = editor;
        editor.enterEditMode('click');
    },
    /**
     * renaming actually happens after the user hits the enter key.  If
     * there is a need to change the name, a broker request is issued
     * and if it succeeds, it is handled in handleRenameResult.
     *
     * @param domObj {Object} the input that the user typed into
     * @param newName {String} what the user put in the input
     */
    handleRename: function(domObj,newName) {
        if (!newName) {
            domObj.editor.dispose();
            return;
        } else {
            var szRid = this._oCurrentNode.data.rid;
            var oRid = new WSResourceId(szRid);
            var szDstRid = '';
            var szSep = (oRid.getPath() == "Library://")?'':'/';
            if (oRid.getType()) {
                //not a folder
                szDstRid = oRid.getPath() + szSep + newName +'.'+ oRid.getType();
            } else {
                szDstRid = oRid.getPath() + szSep + newName + '/';
            }
            domObj.editor.dispose();
            var r = new WSMoveResource(szRid, szDstRid, false);
            this._oTreePanel.setBusy(true);
            var r = new WSMoveResource(szRid, szDstRid, false);
            this._oBroker.dispatchRequest(r,this.handleRenameResult.bind(this, newName));
        }
    },
    /**
     * handle the result of an asynchronous request to change the name of a
     * resource.  If the request succeeded, we need to update the resource in
     * the master object list and the tree.
     */
    handleRenameResult: function(newName,r) {
        this._oTreePanel.setBusy(false);
        //TODO error check for other status
        if (r.status == 500) {
            //show an error dialog - name was rejected by server
            if (!this.renameFailedDialog) { 
                var size = Position.getPageDimensions();
                var o = {
                    title: 'Rename Failed',
                    id: 'renameFailedDialog',
                    contentURL : 'dialogs/renamefailure.html',
                    width: 400,
                    height: 100,
                    top: (size.height-400)/2,
                    left: (size.width-100)/2,
                    handler: this.handleRenameFailed.bind(this),
                    buttons: ['ok']
                };
                this.renameFailedDialog = new JxDialog( o );
            }
            this.renameFailedDialog.open();    
        } else {
            //change the object's rid
            var szRid = this._oCurrentNode.data.rid;
            var oRid = new WSResourceId(szRid);
            var szDstRid = '';
            var szSep = (oRid.getPath() == "Library://")?'':'/';
            if (oRid.getType()) {
                //not a folder
                szDstRid = oRid.getPath() + szSep + newName +'.'+ oRid.getType();
            } else {
                szDstRid = oRid.getPath() + szSep + newName + '/';
            }
            this._oCurrentNode.data.setResourceID(szDstRid);
            //change the label
            this._oCurrentNode.data.treeItem.domObj.childNodes[2].innerHTML = newName;
        }
    },
    /**
     * capture rename failed dialog event
     */
     handleRenameFailed : function(szButton, oDialog ) {
         oDialog.close();
     },
     
    /**
     * launch the open site dialog.  When the dialog is actually ready to
     * be displayed, we need to populate it with default values from
     * the config.  This is handled by setupOpenSiteDefaults.
     */
    dispatchOpenSite : function() {
        //jxTrace('WSApplication::dispatchOpenSite()');
        if (!this.openSiteDialog) {
            var size = Position.getPageDimensions();
            
            var o = {
                title: 'Site Authentication',
                id: 'siteAuthenticationDialog',
                contentURL : 'dialogs/opensite.html',
                helpURL : 'help/dialog_opensite.html',
                width: 400,
                height: 175,
                top: (size.height-155)/2,
                left: (size.width-400)/2,
                handler: this.openSiteHandler.bind(this),
                onOpen: this.setupOpenSiteDefaults.bind(this),
                buttons: ['ok','cancel'],
                resizeable: true
            };
            this.openSiteDialog = new JxDialog( o );
        }
        this.openSiteDialog.open();    
    },
    /**
     * once the open site dialog is opened for the first time, we need 
     * to populate it with defaults.
     *
     * TODO: automatically focus the correct input
     */
    setupOpenSiteDefaults: function() {
        if (this.openSiteDialog.getValue('dialog.opensite.site_url') == '') {
            this.openSiteDialog.setValue('dialog.opensite.site_url', this.defaultSite);
        }
        if (this.openSiteDialog.getValue('dialog.opensite.user_id') == '') {
            this.openSiteDialog.setValue('dialog.opensite.user_id', this.defaultUserName );
        }
        /*
        if (this.defaultSite == '') {
            this.openSiteDialog.values['dialog.opensite.site_url'].focus();
        } else if (this.defaultUserName == '') {
            this.openSiteDialog.values['dialog.opensite.user_id'].focus();
        } else {
            this.openSiteDialog.values['dialog.opensite.password'].focus();
        }*/
    },
    /**
     * handle user input from the open site dialog
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {Object} the open site dialog
     */
    openSiteHandler : function(szButton, oDialog ) {
        if (szButton == 'ok') {
            var siteURL = oDialog.getValue('dialog.opensite.site_url');
            var userName = oDialog.getValue('dialog.opensite.user_id');
            var userPass = oDialog.getValue('dialog.opensite.password');
            this.connectToSite( siteURL, userName, userPass );
        }
        oDialog.close();
    },
    /**
     * close the current site.  This requires a bunch of clean up.  If any
     * open resources have been modified, the user has to be warned and 
     * given a chance to not close.
     */
    dispatchCloseSite : function() {
        //jxTrace('WSApplication::dispatchCloseSite()');
        /* "this should all just work" (TM) */
        if (this._oResourceManager.isAnyResourceDirty()) {
          if (!this.closeSiteCancelledDialog) {
            var size = Position.getPageDimensions();
                  var o = {
                      title: 'Close Site Cancelled',
                      id: 'closeSiteCancelledDialog',
                      contentURL : 'dialogs/closesitecancelled.html',
                      width: 400,
                      height: 100,
                      top: (size.height-400)/2,
                      left: (size.width-100)/2,
                      handler: this.handleCloseSiteCancelled.bind(this)
                  };
                  this.closeSiteCancelledDialog = new JxDialog( o );
          }
          this.closeSiteCancelledDialog.open();
          return;
        }
        this._oMasterObjectList.length = 0;
        this._oCurrentNode = null;
        this._oClipboard = {};
        this._oResourceManager.closeAllResources();
        this._oTree.clear();
        this._oBroker.clearSiteURL();
        this._szSessionId = null;
        this.updateActions();
        this.dispatchOpenSite();
    },
    /**
     * handle user cancelling closing the site.
     *
     * @param szButton {String} the button that was clicked
     * @param oDialog {Object} the open site dialog
     */
    handleCloseSiteCancelled : function(szButton, oDialog) {
        oDialog.close();
    },
    dispatchRefreshSite : function(o) {
        //jxTrace('WSApplication::dispatchRefreshSite()');
        this.refreshTreeNode();
    },
    /**
     * launch the site administration tool in a panel
     */
    dispatchSiteAdministrator : function() {
        //jxTrace('WSApplication::dispatchSiteAdministrator()');
        if (!this._oSiteAdministrator) {
            var url = this._oBroker.mapGuideURL;
            this._oSiteAdministrator = new WSIframePage({title:'Site Administrator', url:url+'mapadmin/login.php', app:this});
        }
        this._oResourceManager.addResource(this._oSiteAdministrator);
    },
    /**
     * launch the map agent tool in a panel
     */
    dispatchMapAgent : function() {
        //jxTrace('WSApplication::dispatchMapAgent()');
        if (!this._oMapAgent) {
            var url = this._oBroker.mapGuideURL;
            this._oMapAgent = new WSIframePage({title:'MapAgent', url:url+'/mapagent/', app:this});
        }
        this._oResourceManager.addResource(this._oMapAgent);
    },
    /**
     * switch to a resource using the window menu
     *
     * @param o {Object} the resource to open.
     */
    dispatchOpenFromMenu : function(o) {
        this._oResourceManager.openResource(o.resource);
    },
    /**
     * launch the options dialog.  Not implemented yet
     */
    dispatchOptions : function() {
        //jxTrace('WSApplication::dispatchOptions()');
    },
    /**
     * open the getting started guide
     */
    dispatchGettingStarted : function() {
        if (!this._oGettingStarted) {
            this._oGettingStarted = new WSHtmlPage('Getting Started', 'help/gettingstarted.html', this);
        }
        this._oResourceManager.addResource(this._oGettingStarted);
    },
    /**
     * open the embedded help system
     */
    dispatchGuide : function() {
        //jxTrace('WSApplication::dispatchHelpGuide()');       
        this._oResourceManager.addResource(this._oHelp);
    },
    /**
     * a default action that does nothing
     */
    dispatchDummy : function() {
        //jxTrace('WSApplication::dispatchDummy()');  
    },
    /**
     * selection changed handler is called from either the tree or the 
     * resource manager.  If its the tree, we have to update the
     * visual style.  If its the resource manager, update the window
     * menu and the window title.
     *
     * @param o {Object} the thing that was selected
     *
     * TODO: tree stuff should be moved to a separate class
     */
    selectionChanged : function(o) {
        //jxTrace('WSApplication::selectionChanged()');
        if (o instanceof JxTreeItem || o instanceof JxTreeFolder) {
            if (this._oCurrentNode) {
                Element.removeClassName(this._oCurrentNode.domObj.childNodes[2], 'jxTreeSelectedNode');
            }
            Element.addClassName(o.domObj.childNodes[2], 'jxTreeSelectedNode');
            this._oCurrentNode = o;

            if (o.lastEvent && o.lastEvent.type == 'dblclick') {
                this._oResourceManager.addResource(this._oCurrentNode.data);
                this._oCurrentNode.data.addContentListener(this);
            } 
        } else {
            document.title = 'MapGuide Open Source Web Studio - ' + o.getName();
            
            for (var i=0; i<this._oWindowMenu.items.length; i++) {
                if (this._oWindowMenu.items[i].resource == o) {
                    Element.addClassName(this._oWindowMenu.items[i].domObj.childNodes[0], 'jxMenuItemSelected');
                } else {
                    Element.removeClassName(this._oWindowMenu.items[i].domObj.childNodes[0], 'jxMenuItemSelected');
                }
            }
        }
        this.updateActions();
    },
    /**
     * the application is a content change listener.  All we really need to
     * do is update the actions.
     */
    contentChanged : function () {
        this.updateActions();
    },
    /**
     * event triggered by the resource manager when a new resource is added
     * to the resource manager.  Add it to the window menu.
     *
     * @param resource {Object} the resource that was added
     */
    resourceAdded : function(resource) {
        var bNeedToAdd = true;
        for (var i=0; i<this._oWindowMenu.items.length; i++) {
            if (this._oWindowMenu.items[i].resource == resource) {
                bNeedToAdd = false;
                break;
            }
        }
        if (bNeedToAdd) {
            var item = new JxMenuItem(resource.getName(), this._oOpenFromAction);
            item.resource = resource;
            item.domObj.title = resource.getPath();
            this._oWindowMenu.add(item);
        }
    },
    /**
     * event triggered by the resource manager when a new resource is removed
     * from the resource manager.  Remove it from the window menu.
     *
     * @param resource {Object} the resource that was removed
     */
    resourceRemoved: function(resource) {
        for (var i=0; i<this._oWindowMenu.items.length; i++) {
            if (this._oWindowMenu.items[i].resource == resource) {
                this._oWindowMenu.remove(this._oWindowMenu.items[i]);
                break;
            }
        }
        this.updateActions();
    },
    /**
     * return a reference to the resource manager.
     * @return {Object} the resource manager
     */
    getResourceManager : function() { return this._oResourceManager;},
    /**
     * set the broker object to be used by the application
     * @param b {WSBroker} the broker to use
     */
    setBroker : function(b) { /*jxTrace('WSApplication::setBroker()'); */this._oBroker = b;},
    /**
     * return a reference to the broker used by the application
     * @return {Object} the resource manager
     */
    getBroker : function() {/*jxTrace('WSApplication::getBroker()');*/ return this._oBroker;}
};
