/*****************************************************************************
 *
 * $Id: ConfigMgr.js 724 2007-08-24 00:33:05Z pspencer $
 *
 * Purpose: Fusion.Lib.ConfigMgr handles communication with the server
 *          during initialization, and triggers loading of the WebLayout
 *
 * Project: Fusion
 *
 * Author: DM Solutions Group Inc 
 *
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 *****************************************************************************/
/**
 * Fusion.Lib.ConfigMgr
 *
 * Configuration class for fusion using a web layout.
 */


Fusion.Lib.ConfigMgr = Class.create();
Object.extend(Fusion.Lib.ConfigMgr.prototype, {
    oApp: null,
    oBroker: null,
    //array of map widgets. For now only the map from the weblayout is used
    //TODO : use the mapconfig to pass other maps.
    aoMapWidget : null,

    oWebLayout: null,
    
    oWebLayout : null,

    aWidgetNames : [],
    
    aWidgets: null,

    /**
     * construct a new configuration manager. 
     *
     * @param app {Object} the application object
     */
    initialize : function(app, sessionid) {
        // console.log('configuration manager initializing');
        this.oApp = app;
        this.aoMapWidget = [];
        this.aWidgets = [];
        
        this.scriptLang = app.getScriptLanguage();
        this.redirectScript = app.getRedirectScript();
        
		this.sessionID = sessionid || null;
        this.createWebLayout();
    },

    /**
     * create an object to manage the web layout.  When it has
     * finished loading and parsing the web layout, it will
     * emit an event.  We load the map definitions when this
     * event is triggered.
     */
    createWebLayout : function()
    {
        // console.log('ConfigMgr::parseWebLayout');
        this.oWebLayout = new Fusion.Lib.WebLayout(this.oApp);
        this.oWebLayout.registerForEvent(Fusion.Event.WEBLAYOUT_PARSED, this.loadWidgets.bind(this));
        this.oWebLayout.parse();
    },

    getListofWidgets : function() {
        var oCommand = null;
        var sTmp;
        var aCommands = [];
        for (var i=0; i<this.oWebLayout.commandObj.length; i++) {
            oCommand = this.oWebLayout.commandObj[i];
            
            if ($(oCommand.getName())) {
                aCommands.push(oCommand);
            }
        }
        return aCommands;
    },
    
    getListofToolbarWidgets: function() {
        var aCommands = [];
        var toolbars = this.oWebLayout.aToolbars;
        for (var i=0; i<toolbars.length; i++) {
            /* test to see if the container for the toolbar is in the page */
            if($(toolbars[i].container)) {
                for (var j=0; j<toolbars[i].buttons.length; j++) {
                    var button = toolbars[i].buttons[j];
                    if (button.func == 'Command') {
                        var oCommand = this.oWebLayout.getCommandByName(toolbars[i].buttons[j].obj.name);
                        if (oCommand) {
                            aCommands.push(oCommand);
                        }
                    } else if (button.func == 'Flyout') {
                        aCommands = aCommands.concat(this.getListofMenuWidgets(button.obj.subItems));
                    }
                }
            }
        }

        return aCommands;
    },
    
    getListofTaskPaneWidgets: function() {
        var oCommand = null;
        var sTmp;
        var aTaskPanes = [];
        var aCommands = [];

        //first find all the TaskPanes in the page
        for (var i=0; i<this.oWebLayout.commandObj.length; i++) {
            oCommand = this.oWebLayout.commandObj[i];
            if (oCommand.getAction() == "TaskPane") {
                aTaskPanes.push(oCommand);
            }
        }

        //now find all widgets that target a TaskPane
        for (var i=0; i<this.oWebLayout.commandObj.length; i++) {
            oCommand = this.oWebLayout.commandObj[i];
            if (oCommand.jsonNode.Target) {
              var target = oCommand.jsonNode.Target[0];
              for (j=0; j<aTaskPanes.length; ++j) {
                if (target == aTaskPanes[j].sName) {
                  aCommands.push(oCommand);
                }
              }
            }
        }

        return aCommands;
    },
    
    getListofContextMenuWidgets: function() {
        var aCommands = [];
        var menus = this.oWebLayout.aMenus;
        for (var i=0; i<menus.length; i++) {
            /* test to see if the container for the toolbar is in the page */
            if(menus[i].mapName == '' || this.getMapByName(menus[i].mapName)) {
                aCommands = aCommands.concat(this.getListofMenuWidgets(menus[i].items));
            }
        }
        return aCommands;
    },
    
    getListofMenuWidgets: function(menuItems) {
        var aCommands = [];
        for (var i=0; i<menuItems.length; i++) {
            var item = menuItems[i];
            switch (item.func) {
                case 'Separator':
                    break;
                case 'Command':
                    var oCommand = this.oWebLayout.getCommandByName(item.obj.name);
                    if (oCommand) {
                        aCommands.push(oCommand);
                    }
                    break;
                case 'Flyout':
                    aCommands = aCommands.concat(this.getListofMenuWidgets(item.obj.subItems));
                    break;
                default:
                
            }
        }
        return aCommands;
    },


    loadWidgets : function() {
        var aCommands = this.getListofWidgets();
        aCommands = aCommands.concat(this.getListofToolbarWidgets());
        aCommands = aCommands.concat(this.getListofContextMenuWidgets());
        aCommands = aCommands.concat(this.getListofTaskPaneWidgets());
        
        for (var i=0; i<aCommands.length; i++) {
			var c = aCommands[i];
			var js = c.sLocation + '/' + c.getAction() + '.js';
            Fusion.require(js);
        }
        
        this.oApp.setLoadState(this.oApp.LOAD_WIDGETS);
    },

    createWidgets :  function() {
        this.createMapWidget();
        this.createAllWidgets();
        this.createToolbars();
        this.createContextMenus();        
    },

    /**convension for the widgets :
     * basic command widgets will be names [Action] with file name being [Action].js
     *  -ex ZoomIn = Zoom()
     *  basic command widgets will be located under base_url/widgets/
     */
    createMapWidget : function() {
        var aCommands = this.oWebLayout.getCommandByType('MapCommandType');
        if (aCommands.length > 0)
        {
            var oCommand = null;
            for (var i=0; i<aCommands.length; i++)
            {
                oCommand = aCommands[i];
                var oElement =  $(oCommand.getName());
                if (oElement != null)
                {
                    var widget;
                    var sID = this.sessionID ? ',"'+this.sessionID+'"': '';
                    var sTmp = 'widget = new Fusion.Widget.' + oCommand.getAction() + '(oCommand'+sID+')';
                    eval(sTmp);
                    if ($(oCommand.getName())) {
                        $(oCommand.getName()).widget = widget;
                    }
                    this.aoMapWidget.push(widget);
                }
            }
        }
    },
    
    getMapInfo: function(resourceId) {
        for (var i=0; i<this.oWebLayout.aMaps.length; i++) {
            if (this.oWebLayout.aMaps[i].resourceId == resourceId) {
                return this.oWebLayout.aMaps[i];
            }
        }
        return null;
    },

    getMapByName : function(sName)
    {
        var nMaps = this.aoMapWidget.length;
        var oMap = null;
        for (var i=0; i<nMaps; i++)
        {
            if (this.aoMapWidget[i].getMapName() == sName){
                oMap = this.aoMapWidget[i];
                break;
            }
        }
        return oMap;
    },

    /**
      get the map object using the html element id used by the map
    */
    getMapById : function(sId)
    {
        var nMaps = this.aoMapWidget.length;
        var oMap;
        for (var i=0; i<nMaps; i++)
        {
            oMap = this.aoMapWidget[i];
            if (oMap.getDomId() == sId)
            {
                return oMap;
            }
        }
        return null;
    },

    getMapByIndice : function(nIndice)
    {
        if (nIndice < this.aoMapWidget.length)
        {
            var oMap = this.aoMapWidget[nIndice];
            return oMap;
        }

        return null;
    },

  
    createAllWidgets : function()
    {
        var aCommands = this.getListofTaskPaneWidgets();
        aCommands = aCommands.concat(this.getListofWidgets());
        for (var i=0; i<aCommands.length; i++) {
            var oCommand = aCommands[i];
            if (oCommand.getAction() != '' && 
                oCommand.getAction() != 'MGMap' && /* TODO Remove the need to test these here */
                oCommand.getAction() != 'MSMap') {
                var widget;
                var sTmp = 'widget = new Fusion.Widget.' + oCommand.getAction() + '(oCommand)';
                eval(sTmp);
                this.aWidgets.push(widget);
                if ($(oCommand.getName())) {
                    $(oCommand.getName()).widget = widget;
                }
            }
        }
    },
    
    createToolbars: function() {
        //console.log('searching for toolbars to create');
        var dummyAction = new Jx.Action(null);
        var toolbars = this.oWebLayout.aToolbars;
        if (toolbars.length > 0) {
            for (var i=0; i<toolbars.length; i++) {
                var toolbarObj = $(toolbars[i].container);
                if (toolbarObj) {
                    //console.log('creating toolbar');
                    var toolbar = toolbars[i];
                    var tb = new Jx.Toolbar(toolbarObj);
                    tb.domObj.id = toolbar.name;
                    for (var j=0; j<toolbar.buttons.length; j++) {
                        var id = 'toolbar'+i+'command'+j;
                        var button = toolbar.buttons[j];
                        var tbItem = null;
                        switch (button.func) {
                            case 'Separator':
                                tb.add(new Jx.ToolbarSeparator());
                                break;
                            case 'Command':
                                var oCommand = this.oWebLayout.getCommandByName(button.obj.name);
                                
                                tbItem = new Jx.ToolbarItem();
                                tbItem.domObj.id = id;
                                tb.add(tbItem);

                                var oldName = oCommand.getName();
                                oCommand.setName(id);
                                var widget;
                                var sTmp = 'widget = new Fusion.Widget.' + oCommand.getAction() + '(oCommand)';
                                eval(sTmp);
                                this.aWidgets.push(widget);
                                oCommand.setName(oldName);                                
                                break;
                            case 'Flyout':
                                var options = {};
                                options.label = button.obj.label;
                                options.tooltip = button.obj.tooltip;
                                if (button.obj.imageUrl != '') {
                                    options.imgPath = button.obj.imageUrl;
                                }                                
                                var menu = new Jx.Menu(options);
                                
                                this.processMenuUiItems(button.obj.subItems, menu, null);
                                
                                tb.add(menu);
                                break;
                            default:
                                /* TODO: this could be an exception? */
                                tb.add(new Jx.ToolbarSeparator());
                        }
                    }
                }
                //else { console.log ('toolbar ' + toolbars[i].name + ' not found'); }
            }

        }
    },
    
    createContextMenus: function() {
        var menus = this.oWebLayout.aMenus;
        if (menus.length > 0) {
            for (var i=0; i<menus.length; i++) {
                var oMap = null;
                if (menus[i].mapName != '') {
                    oMap = this.getMapByName(menus[i].mapName);
                } else {
                    oMap = this.getMapByIndice(0);
                }
                var menu = new Jx.ContextMenu();
                this.processMenuUiItems(menus[i].items, menu, oMap);
                oMap.setContextMenu(menu);
            }
        }
    },
    
    processMenuUiItems: function(menuItems, menu, oMap) {
        for (var i=0; i<menuItems.length; i++) {
            var item = menuItems[i];
            switch (item.func) {
                case 'Separator':
                    break;
                case 'Command':
                    var widget = this.createWidgetFromCommandName(item.obj.name, '');
                    if (widget) {
                        if (widget.isMenuWidget) {
                            menu.add(widget.getMenu());
                        } else {
                            var action;
                            if (oMap) {
                                action = new Jx.Action(oMap.executeFromContextMenu.bind(oMap, widget));
                                widget.registerForEvent(Fusion.Event.WIDGET_STATE_CHANGED, 
                                    function(eventID, aWidget) {
                                        this.setEnabled(aWidget.isEnabled());
                                    }.bind(action));                        
                            } else {
                                action = new Jx.Action(widget.activateTool.bind(widget));
                            
                            }

                            var options = {};
                            options.label = widget._oCommand.sLabel;
                            options.image = widget._oCommand.sImageurl;
                            menu.add(new Jx.MenuItem(action, options));
                        }
                    }
                    break;
                case 'Flyout':
                    var options = {};
                    options.label = item.obj.label;
                    if (item.obj.imageUrl != '') {
                        options.image = item.obj.imageUrl;
                    }
                    var subMenu = new Jx.SubMenu(options);
                    this.processMenuUiItems(item.obj.subItems, subMenu, oMap);
                    menu.add(subMenu);
                    break;
                default:
            
            }
        }
    },
    
    createWidgetFromCommandName: function(name, id) {
        var widget = null;
        var oCommand = this.oWebLayout.getCommandByName(name);
        if (oCommand) {
            var oldName = oCommand.getName();
            oCommand.setName('');
            var widget;
            var sTmp = 'widget = new Fusion.Widget.' + oCommand.getAction() + '(oCommand)';
            eval(sTmp);
            this.aWidgets.push(widget);
            oCommand.setName(oldName);
        }
        return widget;
    },
    
    getWidgetById: function(id) {
        var widget = null;
        var element = $(id);
        if (element) widget = element.widget
        return widget;
    },
    
    getWidgetsByType: function(type) {
        var a = [];
        for (var i=0; i<this.aWidgets.length; i++) {
            if (this.aWidgets[i].sName == type) {
                a.push(this.aWidgets[i]);
            }
        }
        return a;
    }
});