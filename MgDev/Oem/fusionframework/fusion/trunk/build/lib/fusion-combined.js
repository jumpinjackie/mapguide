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
});/*****************************************************************************
 * $Id: utils.js 593 2007-05-09 20:49:46Z pspencer $
 * Purpose: abstract handling of DOM node structures
 * Project: MapGuide Open Source 
 * Author: DM Solutions Group Inc
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
 *****************************************************************************/

var DomNode = Class.create();

DomNode.prototype = {
    initialize: function( xmlNode /*, parent */ ) {
        this.textContent = '';
        this.nodeName = xmlNode?xmlNode.nodeName:'';
        this.parentNode = arguments[1]?arguments[1]:null;
        this.childNodes  = [];
        this.attributes = [];
        this.attributeNames = [];
        if (xmlNode) {
            if (xmlNode.attributes) {
                for (var i=0; i<xmlNode.attributes.length; i++) {
                    this.attributeNames.push(xmlNode.attributes[i].name);
                    this.attributes[xmlNode.attributes[i].name] = xmlNode.attributes[i].nodeValue;
                }
            }
            for (var i=0; i<xmlNode.childNodes.length; i++) {
              if (xmlNode.childNodes[i].nodeType != 3) {
                this.childNodes.push(new DomNode(xmlNode.childNodes[i], this));
              } else {
                this.textContent = this.textContent + xmlNode.childNodes[i].nodeValue;
              }
            }
        }
        this._currentNode = 0;
    },
    getAttribute: function(name) {
        return typeof(this.attributes[name]) != 'undefined' ? this.attributes[name] : null;
    },
    removeChild: function(child) {
        var result = null;
        for(var i=0; i<this.childNodes.length; i++) {
            if (this.childNodes[i] == child) {
                this._currentNode = 0;
                this.childNodes.splice(i,1);
                child.parentNode = null;
                result = child;
                break;
            }
        }
        return result;
    },
    appendChild: function(child) {
        if (child.parentNode) {
            child.parentNode.removeChild(child);
        }
        child.parentNode = this;
        this.childNodes.push(child);
    },
    insertBefore: function(newChild,refChild) {
        var bInserted = false;
        if (refChild) {
            for (var i=0; i<this.childNodes.length; i++) {
                if (this.childNodes[i] == refChild) {
                    if (newChild.parentNode) {
                        newChild.parentNode.removeChild(child);
                    }
                    newChild.parentNode = this;
                    this.childNodes.splice(i,0,newChild);
                    bInserted = true;
                    break;
                }
            }
        }
        if (!bInserted) {
            this.appendChild(newChild);
        }
    },
    toString: function(depth) {
        var s = '';
        var spacer = '';
        for (i=0; i<depth; i++) {
            spacer = spacer + '';
        }
        s = spacer + '&lt;' + this.nodeName;
        if (this.attributes.length > 0) {
            for (var name in this.attributes) {
                if (typeof(this.attributes[name]) == 'String' ) {
                    s = s + ' ' + name + '="' + this.attributes[name] + '"';
                }
            }
        }
        s = s + '&gt;'
        if (this.childNodes.length == 0) {
            s = s + this.textContent;
            spacer = '';
        } else {  
            s = s + '\n';
        }
        for (var i=0; i<this.childNodes.length; i++) {
            s = s + this.childNodes[i].toString( depth + 1 );
        }
        s = s + spacer + '&lt;/'+this.nodeName+'&gt;';
        return s;
    },
    toXML: function() {
        var s = this.parentNode?'':'<?xml version="1.0" encoding="UTF-8"?>\n';
        s = s+ '<' + this.nodeName;
        if (this.attributeNames.length > 0) {
            for (var i=0; i<this.attributeNames.length; i++) {
                var name = this.attributeNames[i];
                s = s + ' ' + name + '="' + this.attributes[name] + '"';
            }
        }
        s = s + '>'
        if (this.childNodes.length == 0) {
            var content = this.textContent + ''; //force string value if textContent was automatically made to a number
            content = content.replace('&','&amp;');
            content = content.replace(/</g, encodeURIComponent('&lt;'));
            content = content.replace(/>/g, encodeURIComponent('&gt;'));
            s = s + content;
            
        }
        for (var i=0; i<this.childNodes.length; i++) {
            s = s + this.childNodes[i].toXML();
        }
        s = s + '</'+this.nodeName+'>\n';
        return s;
    },
    getNodeText: function(name) {
        var s = '';
        var n = this.findFirstNode(name);
        if (n) {
            s = n.textContent;
        }
        return s;
    },
    setNodeText: function(name, value) {
        var n = this.findFirstNode(name);
        if (n) {
            n.setTextContent(value);
        }
    },
    setTextContent: function(value) {
        this.textContent = value;
    },
    setAttribute: function(name, value) {
        if (typeof this.attributes[name] == 'undefined') {
            this.attributeNames.push(name);
        }
        this.attributes[name] = value;
    },
    findFirstNode: function( name ) {
        this._currentNode = 0;
        if (this.nodeName == name) {
            return this;
        } else {
            for (var i=0; i<this.childNodes.length; i++) {
                var node = this.childNodes[i].findFirstNode(name);
                if (node) {
                    if (node.parentNode == this) {
                        this._currentNode = i + 1;
                    } else {
                        this._currentNode = i;          
                    }
                    return node;
                }
            }
            return false;
        }
    },
    findNextNode: function( name ) {
        if (this.nodeName == name) {
            return this;
        } else {
            for (var i=this._currentNode; i<this.childNodes.length; i++) {
                var node = this.childNodes[i].findNextNode(name);
                if (node) {
                    if (node.parentNode == this) {
                        this._currentNode = i + 1;
                    } else {
                        this._currentNode = i;          
                    }
                    return node;
                }
            }
            return false;
        } 
    }
};

var DomNodeFactory = {
    create: function( name, value ) {
        var node = new DomNode();
        node.nodeName = name;
        node.textContent = value || '';
        return node;
    }
};/*****************************************************************************
 * $Id: Error.js 737 2007-09-04 14:55:50Z pspencer $
 * Purpose: Fusion.Error - general error class for managing error information
 * Project: Fusion
 * Author: DM Solutions Group Inc 
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
 *****************************************************************************/
/**
 * Class: Fusion.Error
 * General purpose Error object
 */
Fusion.Error = Class.create();

Fusion.Error.FATAL = 0;
Fusion.Error.WARNING = 1;
Fusion.Error.NOTICE = 2;

Fusion.Error.prototype = {
    type: null,
    message: null,
    initialize: function(type, message) {
        this.type = type;
        this.message = message;
    },
    
    alert: function() {
        var type = this.typeToString(this.type);
        alert('Fusion Error: ' + type + '\n' + this.message);
    },
    
    toString: function() {
        var type = this.typeToString(this.type);
        return type + ": " + this.message;
    },
    
    typeToString: function(type) {
        switch (type) {
            case Fusion.Error.FATAL:
                return 'FATAL';
            case Fusion.Error.WARNING:
                return 'WARNING';
            case Fusion.Error.NOTICE:
                return 'NOTICE';
            default:
                return 'UNKNOWN ('+type+')';
        }
    }
};/*****************************************************************************
 * $Id: $
 * Purpose: ApplicationDefinition Parser
 * Project: Fusion
 * Author: DM Solutions Group Inc
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
 *****************************************************************************/

/**
 * ApplicationDefinition
 *
 * Utility class to parse an application definition
 *
 */
Fusion.Lib.ApplicationDefinition = Class.create();
Fusion.Lib.ApplicationDefinition.prototype = {
    /**
     * Property: mapGroups
     *
     * array of map groups, parsed from ApplicationDefinition.  A MapGroup
     * consists of one or more Maps that can be combined into the same
     * OpenLayers Map object
     */
    mapGroups: null,

    /**
     * Property: widgetSets
     *
     * array of widget sets (each one corresponding to a map) parsed
     * from the ApplicationDefinition.
     */
    widgetSets: null,

    /**
     * Property: {Object} oBroker
     *
     * A Broker object that can communicate with a MapGuide instance
     * in the case we are running against a MapGuide server
     */
    oBroker: null,

    /**
     * Property: {Array} searchDefinitions
     *
     * An array of search definitions
     */
    searchDefinitions: null,

    /**
     * Property: {Array} searchCategories
     *
     * An array of search categories
     */
    searchCategories: null,

    /**
     * Constructor: ApplicationDefinition
     *
     * construct a new instance of the ApplicationDefinition object.  While
     * not enforced, this is intended to be a singleton.
     *
     * Parameter: sessionId
     *
     * an optional session id to initialize the application with, passed to
     * the map widgets when they are created.
     */

    initialize: function(sessionId) {
        //console.log('ApplicationDefinition initialize');
        Fusion.Lib.EventMgr.initialize.apply(this, []);
        this.sessionId = sessionId;
        this.oBroker = Fusion.getBroker();
        this.applicationDefinition =  Fusion.getApplicationDefinitionURL();

        this.widgetSets = [];
        this.mapGroups = [];
        this.searchDefinitions = [];
        this.searchCategories = [];
        this.parse();
    },

    /**
     * Function: parse
     *
     * start parsing the ApplicationDefinition file.  This happens
     * asynchronously since the ApplicationDefinition has to be
     * retrieved from the server or the MapGuide repository.  When
     * parsing is complete, an APPLICATIONDEFINITION_PARSED event
     * will be emitted.  This function returns true if parsing
     * will start, false if it will not (due to a missing
     * application definition for instance).
     */
    parse : function() {
        if (this.applicationDefinition == '') {
            //TODO: emit an error
            return null;
        }
        /* if the application definition is not in the mapguide server,
           just load the xml*/

        if ( (this.applicationDefinition.match('Library://') == null) &&
             (this.applicationDefinition.match('Session:') == null) ) {
            var options = {};
            options.method = 'get';
            options.onSuccess = this.convertXML.bind(this);
            new Ajax.Request( this.applicationDefinition, options);
        } else {
            //TODO: request as JSON format
            var r = new Fusion.Lib.MGRequest.MGGetResourceContent(this.applicationDefinition);
            this.oBroker.dispatchRequest(r, this.convertXML.bind(this));
        }
        return true;
    },

    /**
     * Function: convertXML
     *
     * Optionally convert XML to JSON using a server-side script
     * if the application definition wasn't available in JSON.
     *
     * Parameter: {XmlHttpRequest} r
     *
     * the XmlHttpRequest object
     *
     * Parameter: json
     *
     * boolean indicator if the content is JSON or not.
     */
    convertXML: function(r, json) {
        if (json) {
            this.parseJSON(r, json);
        } else {
            var options = {};
            options.onSuccess = this.parseJSON.bind(this);
            options.parameters = 'xml='+encodeURIComponent(r.responseText.replace(/\\/g, '\\\\\\\\'))+'&ts='+((new Date()).getTime());
            var sl = Fusion.getScriptLanguage();
            Fusion.ajaxRequest('common/'+sl+'/Xml2JSON.'+sl, options);
        }
    },

    /**
     * Function: parseJSON
     *
     * parse a JSON string into an ApplicationDefinition
     *
     * Parameter: {XmlHttpRequest} r
     *
     * the XmlHttpRequest object
     *
     * Parameter: json
     *
     * boolean indicator if the content is JSON or not.
     */
     parseJSON: function(r, json) {
        if (json) {
            var mainNode;
            eval("mainNode="+r.responseText);

            var appDef = mainNode.ApplicationDefinition;
            /* process Map nodes */
            if (appDef.MapSet) {
                var mapSet = appDef.MapSet[0];
                if (mapSet.MapGroup instanceof Array) {
                    for (var i=0; i<mapSet.MapGroup.length; i++) {
                        var mapGroup = new Fusion.Lib.ApplicationDefinition.MapGroup(mapSet.MapGroup[i]);
                        this.mapGroups.push(mapGroup);
                    }
                }
            }

            /* process widget sets */
            if (appDef.WidgetSet) {
                for (var i=0; i<appDef.WidgetSet.length; i++) {
                    var widgetSet = new Fusion.Lib.ApplicationDefinition.WidgetSet(appDef.WidgetSet[i]);
                    this.widgetSets.push(widgetSet);
                }
            } else {
                //TODO: would this be an error?
            }

            /* process extensions */
            if (appDef.Extension) {
                var extension = appDef.Extension[0];
                /* process search definitions */
                if (extension.SearchDefinitions instanceof Array) {
                    var categories = extension.SearchDefinitions[0];
                    if (categories.SearchCategory instanceof Array) {
                        for (var i=0; i<categories.SearchCategory.length; i++) {
                            var oCategory = {};
                            var category = categories.SearchCategory[i];
                            oCategory.id = category['@id'];
                            oCategory.name = category['@name'];
                            oCategory.layer = category.Layer ? category.Layer[0] : '';
                            oCategory.searchDefinitions = [];
                            this.searchCategories[oCategory.id] = oCategory;
                            var defns = category.SearchDefinition;
                            for (var k=0; k<defns.length; k++) {
                                var defn = new Fusion.Lib.ApplicationDefinition.SearchDefinition(defns[k]);
                                defn.category = oCategory;
                                oCategory.searchDefinitions[defn.id] = defn;
                                this.searchDefinitions[defn.id] = defn;
                            }
                        }
                    }
                }

            }
        }
        Fusion.setLoadState(Fusion.LOAD_WIDGETS);
    },

    /**
     * Function: create
     *
     * Create the application definition.  This actually triggers initializing
     * every widget and container.
     */
    create: function() {
        for (var i=0; i<this.widgetSets.length; i++) {
            this.widgetSets[i].create(this);
        }
    },

    /**
     * Function: getMapByName
     *
     * return a map widget with the given name
     *
     * Parameter: {String} name
     *
     * The map name to return
     *
     * Returns: {Object} a map object or null if not found.
     */
    getMapByName : function(name) {
        var map = null;
        for (var i=0; i<this.widgetSets.length; i++) {
            map = this.widgetSets[i].getMapByName(name);
            if (map) {
                break;
            }
        }
        return map;
    },

    /**
     * Function: getMapById
     *
     * return a map widget with the given id
     *
     * Parameter: {String} id
     *
     * The map id to return.  ID is distinct from map.name in that id is the
     * id of the HTML tag where the map widget is inserted.
     *
     * Returns: {Object} a map object or null if not found.
     */
    getMapById : function(id) {
        var map = null;
        for (var i=0; i<this.widgetSets.length; i++) {
            map = this.widgetSets[i].mapWidget;
            if (map.mapId == id) {
                break;
            }
        }
        return map;
    },

    /**
     * Function: getMapByIndice
     *
     * return the map widget at the given index
     *
     * Parameter: {String} indice
     *
     * The map indice to return
     *
     * Returns: {Object} a map object or null if not found.
     */
     getMapByIndice : function(indice) {
         var map = null;
         if (this.widgetSets.length < indice) {
             map = this.widgetSets[indice].getMapWidget();
         }
         return map;
     },

    /**
     * Function: getMapGroup
     *
     * return the specified map group from the application definition
     *
     * Parameter: {String} mapgroup
     *
     * The id of the MapGroup to return
     *
     * Returns: {Object} a MapGroup appdef or null if not found.
     */
     getMapGroup : function(mapGroupId) {
         var mapGroup = null;
         for (var i=0; i<this.mapGroups.length; ++i) {
           if (this.mapGroups[i].mapId == mapGroupId) {
             mapGroup = this.mapGroups[i];
             break;
           }
         }
         return mapGroup;
     },

     /**
      * Function getWidgetsByType
      *
      * returns an array of widgets by type.
      *
      * Parameter: {String} type
      *
      * the type of widget to get references to
      *
      * Returns: {Array} an array of widgets, which may be empty
      */
     getWidgetsByType: function(type) {
         var widgets = [];
         for (var i=0; i<this.widgetSets.length; i++) {
             widgets = widgets.concat(this.widgetSets[i].getWidgetsByType(type));
         }
         return widgets;
     }
};

Fusion.Lib.ApplicationDefinition.MapGroup = Class.create();
Fusion.Lib.ApplicationDefinition.MapGroup.prototype = {
    initialView: null,
    maps: null,

    initialize: function(jsonNode) {
        this.mapId = jsonNode['@id'][0];
        this.maps = [];
        /* parse InitialView */
        if (jsonNode.InitialView) {
            var iv = jsonNode.InitialView[0];
            if (iv.CenterX && iv.CenterY && iv.Scale) {
                this.setInitialView(parseFloat(iv.CenterX[0]),
                                    parseFloat(iv.CenterY[0]),
                                    parseFloat(iv.Scale[0]));
            } else {
                //TODO: emit warning that the initial view was incomplete
            }
        }
        /* parse maps */
        if (jsonNode.Map instanceof Array) {
            for (var i=0; i<jsonNode.Map.length; i++) {
                var map = new Fusion.Lib.ApplicationDefinition.Map(jsonNode.Map[i]);
                this.maps.push(map);
            }
        } else {
            //TODO: do we need a warning that there are no layers in this map?
        }
        this.links = {groups:[], layers:[]};
        this.layerEvents = {};
        if (jsonNode.Extension) {
            var extension = jsonNode.Extension[0];
            if (extension.Links) {
                /* process Groups */
                if (extension.Links[0].Group instanceof Array) {
                    for (var j=0; j<extension.Links[0].Group.length; j++) {
                        var group = extension.Links[0].Group[j];
                        this.links.groups.push({name:group.Name,url:group.Url});
                    }
                }
                if (extension.Links[0].Layer instanceof Array) {
                    for (var j=0; j<extension.Links[0].Layer.length; j++) {
                        var layer = extension.Links[0].Layer[j];
                        this.links.layers.push({name:layer.Name,url:layer.Url});
                    }
                }
            }
            /* process layer events */
            //TODO: Should this be called MapEvents?
            if (extension.LayerEvents) {
                if (extension.LayerEvents[0].Layer instanceof Array) {
                    for (var j=0; j<extension.LayerEvents[0].Layer.length; j++) {
                        var layer = extension.LayerEvents[0].Layer[j];
                        var layerObj = {};
                        layerObj.name = layer.Name[0];
                        layerObj.onEnable = [];
                        layerObj.onDisable = [];

                        if (layer.OnEnable instanceof Array) {
                            for (var k=0; k<layer.OnEnable[0].Layer.length; k++) {
                                var kLayer = layer.OnEnable[0].Layer[k];
                                layerObj.onEnable.push({name:kLayer.Name[0], enable: kLayer.Enable[0] == 'true' ? true : false});
                            }
                        }
                        if (layer.OnDisable instanceof Array) {
                            for (var k=0; k<layer.OnDisable[0].Layer.length; k++) {
                                var kLayer = layer.OnDisable[0].Layer[k];
                                layerObj.onDisable.push({name:kLayer.Name[0], enable: kLayer.Enable[0] == 'true' ? true : false});
                            }
                        }
                        this.layerEvents[layerObj.name] = layerObj;
                    }
                }
            }
        } else {
            this.extension = {};
        }

    },

    getInitialView: function() {
        return this.initialView;
    },

    setInitialView: function(x,y,scale) {
        this.initialView = {x:x, y:y, scale:scale};
    }
};

Fusion.Lib.ApplicationDefinition.Map = Class.create();
Fusion.Lib.ApplicationDefinition.Map.prototype = {
    type: null,
    singleTile: false,
    extension: null,
    initialize: function(jsonNode) {
        /* TODO: type can be any supported OpenLayers type */
        this.type = jsonNode.Type[0];
        if (jsonNode.SingleTile) {
            var b = jsonNode.SingleTile[0].toLowerCase();
            this.singleTile = (b == "true") ? true : false;
        }
        if (jsonNode.Extension) {
            this.extension = jsonNode.Extension[0];
        } else {
            this.extension = {};
        }
        this.resourceId = this.extension.ResourceId ? this.extension.ResourceId[0] : '';
        Fusion.require(this.type + '/' + this.type + '.js');
    }
};

Fusion.Lib.ApplicationDefinition.WidgetSet = Class.create();
Fusion.Lib.ApplicationDefinition.WidgetSet.prototype = {
    containers: null,
    widgetTags: null,
    widgetTagsByName: null,
    widgetInstances: null,
    mapWidget: null,
    mapId: null,
    initialize: function(jsonNode) {
        this.containers = [];
        this.widgetTags = [];
        this.widgetInstances = [];
        this.widgetTagsByName = {};
        /* process map widgets */
        if (jsonNode.MapWidget) {
            for (var i=0; i<jsonNode.MapWidget.length; i++) {
                var widget = new Fusion.Lib.ApplicationDefinition.Widget(jsonNode.MapWidget[i]);
                widget.widgetSet = this;
                this.mapWidgetTag = widget;
                this.mapId = jsonNode.MapWidget[i].MapId[0];
            }
        }
        /* process widgets */
        if (jsonNode.Widget) {
            for (var i=0; i<jsonNode.Widget.length; i++) {
                var widget = new Fusion.Lib.ApplicationDefinition.Widget(jsonNode.Widget[i]);
                widget.widgetSet = this;
                this.widgetTags.push(widget);
                this.widgetTagsByName[widget.name] = widget;
            }
        }
        /* process containers */
        if (jsonNode.Container) {
            for (var i=0; i<jsonNode.Container.length; i++) {
                var container = new Fusion.Lib.ApplicationDefinition.Container(jsonNode.Container[i]);
                this.containers.push(container);
            }
        }

    },

    /**
     * Function: addWidgetInstance
     *
     * keep track of live widgets created in this widgetSet
     *
     * Parameter: {<Fusion.Widget>} widget
     *
     * the widget to add
     */
    addWidgetInstance: function(widget) {
        this.widgetInstances.push(widget);
    },

    /**
     * Function: getMapWidget
     *
     * return the map widget for this widget set
     *
     * Returns: {<Fusion.Lib.Map>} a map widget or null
     */
    getMapWidget: function() {
        return this.mapWidget;
    },

    /**
     * Function: create
     *
     * create all the things required by this widgetSet, including
     * containers and widgets.
     *
     * Parameter: {<Fusion.Lib.ApplicationDefinition>}
     *
     * the application definition that this widgetSet is part of
     */
    create: function(appDef) {

        //find the map group for the map requested
        var mapGroup = null;
        if (this.mapId) {
            for (var i=0; i<appDef.mapGroups.length; ++i) {
              if (this.mapId == appDef.mapGroups[i].mapId) {
                mapGroup = appDef.mapGroups[i];
                break;
              }
            }
        }

        //create the Map widget for this WidgetSet
        this.mapWidget = new Fusion.Widget.Map(this.mapWidgetTag.name,mapGroup);
        $(this.mapWidgetTag.name).widget = this.mapWidget;

        //create all the other widgets for the widget set
        for (var i=0; i<this.widgetTags.length; i++) {
            this.widgetTags[i].create(this);
        }

        //create all the containers for the widget set
        for (var i=0; i<this.containers.length; i++) {
            this.containers[i].create(this);
        }
    },
    /**
     * Function: getMapByName
     *
     * return the map widget from this widget set if the map's name
     * matches the requested name, or null.
     *
     * Parameter: {String} name
     *
     * The map name to check
     *
     * Returns: {Object} a map object or null.
     */
    getMapByName : function(name) {
        var map = null;
        if (this.mapWidget.getMapName() == name) {
            map = this.mapWidget;
        }
        return map;
    },

    /**
     * Function getWidgetsByType
     *
     * returns an array of widgets by type.
     *
     * Parameter: {String} type
     *
     * the type of widget to get references to
     *
     * Returns: {Array} an array of widgets, which may be empty
     */
    getWidgetsByType: function(type) {
        var widgets = [];
        for (var i=0; i<this.widgetInstances.length; i++) {
            if (this.widgetInstances[i].sName == type) {
                widgets.push(this.widgetInstances[i]);
            }
        }
        return widgets;
    },

    getWidgetByName: function(name) {
        return this.widgetTagsByName[name];
    }
};

Fusion.Lib.ApplicationDefinition.Container = Class.create();
Fusion.Lib.ApplicationDefinition.Container.prototype = {
    name: null,
    type: null,
    validPositions: ['top', 'left', 'bottom', 'right'],
    position: 'top',
    items: null,
    initialize: function(jsonNode) {
        this.type = jsonNode.Type[0];
        this.name = jsonNode.Name[0];
        var position = jsonNode.Position ? jsonNode.Position[0].toLowerCase() : this.position;
        for (var i=0; i<this.validPositions.length; i++) {
            if (this.validPositions[i] == position) {
                this.position = position;
                break;
            }
        }
        this.items = [];
        if (jsonNode.Item) {
            for (var i=0; i<jsonNode.Item.length; i++) {
                var item = new Fusion.Lib.ApplicationDefinition.Item(jsonNode.Item[i]);
                this.items.push(item);
            }
        } else {
            //TODO: is this a problem if there are no items?
        }
    },

    create: function(widgetSet) {
        var container;
        if (this.type == 'Toolbar' || this.type == 'Statusbar') {
            if ($(this.name)) {
                container = new Jx.Toolbar(this.name, this.position);
                $(this.name).container = container;
            }
        } else if (this.type == 'ContextMenu') {
            container = new Jx.ContextMenu();
            widgetSet.getMapWidget().setContextMenu(container);
        }
        for (var i=0; i<this.items.length; i++) {
            this.items[i].create(widgetSet, container);
        }
    }

};

Fusion.Lib.ApplicationDefinition.Widget = Class.create();
Fusion.Lib.ApplicationDefinition.Widget.prototype = {
    name: null,
    type: null,
    statusText: null,
    location: null,
    imageUrl: null,
    imageClass: null,
    tooltip: null,
    label: null,
    disabled: null,
    extension: null,
    initialize: function(jsonNode) {
        if (jsonNode) {
            this.type = jsonNode.Type[0];
            this.name = jsonNode.Name ? jsonNode.Name[0] : '';
            this.statusText = jsonNode.StatusText ? jsonNode.StatusText[0] : '';
            //TODO: this may be an extension
            this.location = jsonNode.Location ? jsonNode.Location[0] : 'widgets/';
            if (this.location.slice(-1) != '/') {
                this.location += '/';
            }
            this.imageUrl = jsonNode.ImageUrl ? jsonNode.ImageUrl[0] : '';
            this.imageClass = jsonNode.ImageClass ? jsonNode.ImageClass[0] : '';
            this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
            this.label = jsonNode.Label ? jsonNode.Label[0] : '';
            this.disabled = jsonNode.Disabled ? (jsonNode.Disabled[0].toLowerCase() == 'true' ? true : false) : false;

            //console.log('Widget: ' + this.type + ', ' + this.name + ', ' + this.description);

            if (jsonNode.Extension) {
                this.extension = jsonNode.Extension[0];
            } else {
                this.extension = {};
            }
            //require the widget code
            //TODO: remove the need for this check
            if (this.type != 'Map') {
              Fusion.require(this.location + this.type + '.js');
            }
        }
    },

    getMapWidget: function() {
        if (this.widgetSet) {
            return this.widgetSet.getMapWidget();
        } else {
            return null;
        }
    },

    /**
     * Function: create
     *
     * creates a new instance of the widget, optionally using a
     * different name during instantiation to accomodate
     * containers
     *
     * Parameter: name
     *
     * An optional name to use for the widget, overrides the
     * original name temporarily if passed.
     *
     * Returns: an instance of the widget represented by this
     * object.
     */
    create: function(widgetSet, name) {
        var widget = null;
        this.widgetSet = widgetSet;
        var oldName = this.name;
        if (typeof name == 'undefined') {
            name = this.name;
        }
        /* create the widget if the name is not null and the name
         * is either an empty string (in the case of buttons in menus)
         * or something exists in the dom with the right id
         */
        if (name != null && (name == '' || $(name) != null)) {
            this.name = name;
            widget = eval("new Fusion.Widget."+this.type+"(this)");
            widgetSet.addWidgetInstance(widget);
            if ($(this.name)) {
                widget.id = this.name;
                $(this.name).widget = widget;
            }
            this.name = oldName;
        }
        return widget;
    }
};

Fusion.Lib.ApplicationDefinition.Item = Class.create();
Fusion.Lib.ApplicationDefinition.Item.prototype = {
    uniqueId: [0],
    type: null,
    initialize: function(jsonNode) {
        this.type = jsonNode.Function[0];
        switch(this.type) {
            case 'Widget':
                this.widgetName = jsonNode.Widget[0];
                break;
            case 'Flyout':
                this.flyout = new Fusion.Lib.ApplicationDefinition.Flyout(jsonNode);
                break;
            case 'Separator':
                break;
        }
    },

    create: function(widgetSet, container) {
        switch(this.type) {
            case 'Widget':
                var widgetTag = widgetSet.getWidgetByName(this.widgetName);
                if (widgetTag) {
                    var name = 'FusionItem'+this.uniqueId[0];
                    this.uniqueId[0]++;
                    if (container instanceof Jx.Toolbar) {
                        /* create a button for the widget */
                        var tbItem = new Jx.ToolbarItem();
                        tbItem.domObj.id = name;
                        container.add(tbItem);
                        widgetTag.create(widgetSet, name);
                    } else if (container instanceof Jx.Menu ||
                               container instanceof Jx.ContextMenu ||
                               container instanceof Jx.SubMenu) {
                        var widget = widgetTag.create(widgetSet, '');
                        var action = new Jx.Action(widget.activateTool.bind(widget));
                        var opt = {};
                        opt.label = widgetTag.label;
                        opt.image = widgetTag.imageUrl;
                        var menuItem = new Jx.MenuItem(action, opt);
                        container.add(menuItem);
                    }
                }
                break;
            case 'Flyout':
                /* create a menu */
                var menu;
                var opt = {};
                opt.label = this.flyout.label;
                opt.tooltip = this.flyout.tooltip;
                opt.image = this.flyout.imageUrl;
                opt.imageClass = this.flyout.imageClass;
                if (container instanceof Jx.Toolbar) {
                    menu = new Jx.Menu(opt);
                } else if (container instanceof Jx.Menu ||
                           container instanceof Jx.ContextMenu ||
                           container instanceof Jx.SubMenu) {
                    menu = new Jx.SubMenu(opt);
                }
                container.add(menu);
                this.flyout.create(widgetSet, menu);

                break;
            case 'Separator':
                if (container instanceof Jx.Toolbar) {
                    container.add(new Jx.ToolbarSeparator());
                } else if (container instanceof( Jx.Menu) ||
                           container instanceof(Jx.SubMenu) ||
                           container instanceof(Jx.ContextMenu)) {
                    container.add(new Jx.MenuSeparator());
                }
                break;
        }
    }
};

Fusion.Lib.ApplicationDefinition.Flyout = Class.create();
Fusion.Lib.ApplicationDefinition.Flyout.prototype = {
    label: null,
    tooltip: null,
    description: null,
    imageUrl: null,
    items: null,

    initialize: function(jsonNode) {
        this.label = jsonNode.Label ? jsonNode.Label[0] : '';
        this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
        this.description = jsonNode.Description ? jsonNode.Description[0] : '';
        this.imageUrl = jsonNode.ImageUrl ? jsonNode.ImageUrl[0] : '';
        this.items = [];
        if (jsonNode.Item instanceof Array) {
            for (var i=0; i<jsonNode.Item.length; i++) {
                this.items.push(new Fusion.Lib.ApplicationDefinition.Item(jsonNode.Item[i]));
            }
        }
    },

    create: function(widgetSet, menu) {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].create(widgetSet, menu);
        }
    }

};

Fusion.Lib.ApplicationDefinition.SearchDefinition = Class.create();
Fusion.Lib.ApplicationDefinition.SearchDefinition.prototype = {
    id: null,
    name: null,
    category: null,
    parameters: null,
    join: null,
    rule: null,

    initialize: function(json) {
        this.id = json['@id'];
        this.name = json['@name'];
        if (json.Join instanceof Array) {
            this.join = new Fusion.Lib.ApplicationDefinition.SearchJoin(json.Join[0]);
        }
        this.parameters = [];
        if (json.Parameter instanceof Array) {
            for (var i=0; i<json.Parameter.length; i++) {
                this.parameters.push(json.Parameter[i]['@name']);
            }
        }
        var rule;
        if (json.SearchAnd instanceof Array) {
            this.rule = new Fusion.Lib.ApplicationDefinition.SearchRule('AND');
            rule = json.SearchAnd[0];
        } else if (json.SearchOr instanceof Array) {
            this.rule = new Fusion.Lib.ApplicationDefinition.SearchRule('OR');
            rule = json.SearchOr[0];
        }
        if (rule && rule.SearchCondition instanceof Array) {
            for (var i=0; i<rule.SearchCondition.length; i++) {
                this.rule.add(new Fusion.Lib.ApplicationDefinition.SearchCondition(rule.SearchCondition[i]));
            }
        }
    },

    getJoinUrl: function(params) {
        if (this.join) {
            return '&joinlayer='+this.join.layer+'&joinpk='+this.join.primaryKey+'&joinfk='+this.join.foreignKey;
        } else {
            return '';
        }
    },

    getFilterUrl: function(params) {
        return '&filter='+encodeURIComponent(this.rule.toString(params));
    }
};

Fusion.Lib.ApplicationDefinition.SearchJoin = Class.create();
Fusion.Lib.ApplicationDefinition.SearchJoin.prototype = {
    layer: null,
    primaryKey: null,
    foreignKey: null,
    initialize: function(json) {
        this.layer = json.Layer ? json.Layer[0] : '';
        this.primaryKey = json.PrimaryKey ? json.PrimaryKey[0] : '';
        this.foreignKey = json.ForeignKey ? json.ForeignKey[0] : '';
    }
};

Fusion.Lib.ApplicationDefinition.SearchRule = Class.create();
Fusion.Lib.ApplicationDefinition.SearchRule.prototype = {
    type: null,
    conditions: null,
    initialize: function(type) {
        this.type = type;
        this.conditions = [];
    },

    add: function(condition) {
        this.conditions.push(condition);
    },

    remove: function(condition) {
        for (var i=0; i<this.conditions.length; i++) {
            if (this.conditions[i] == condition) {
                this.conditions.splice(i, 1);
                break;
            }
        }
    },

    toString: function(params) {
        var conditions = [];
        for (var i=0; i<this.conditions.length; i++) {
            this.conditions[i].setParams(params);
            var c = this.conditions[i].toString();
            if (c != '') {
                conditions.push(c);
            }
        }
        return '(' + conditions.join(') ' + this.type + ' (') + ')';
    }
};

Fusion.Lib.ApplicationDefinition.SearchCondition = Class.create();
Fusion.Lib.ApplicationDefinition.SearchCondition.prototype = {
    column: null,
    operator: null,
    parameter: null,
    quote: null,
    value: null,
    operators: {eq:'=', like:'like', lt:'<', lte:'<=', gt:'>', gte:'>=', neq:'<>'},
    includeIfEmpty: false,

    initialize: function(json) {
        this.column = json.Column[0];
        this.operator = this.operators[json.Operator[0].toLowerCase()];
        this.parameter = json.Parameter[0];
        this.quote = json['@quote'] ? json['@quote'] : '';
        this.wildcard = json['@wildcard'] ? json['@wildcard'] : 'both';
    },

    setParams: function(p) {
        if (p[this.parameter]) {
            this.value = p[this.parameter];
        } else {
            this.value = '';
        }
    },

    toString: function() {
        var value = this.value ? this.value : '';
        if (value == '' && !this.includeIfEmpty) {
            return '';
        }
        var prewildcard = '';
        var prewildcard = '';
        var postwildcard = '';
        if (this.operator == 'like') {
            if (this.wildcard == 'before' || this.wildcard == 'both') {
                prewildcard = '*';
            }
            if (this.wildcard == 'after' || this.wildcard == 'both') {
                postwildcard = '*';
            }
        }
        var wildcard = this.operator == 'like' ? '*' : '';
        return this.column + ' ' + this.operator + ' ' + this.quote + prewildcard + value + postwildcard + this.quote;
    }
};
/**
 * @project         Fusion
 * @revision        $Id: MGBroker.js 863 2007-10-06 17:48:57Z madair $
 * @fileoverview    this file contains classes for communicating
 *                  with a MapGuide MapAgent
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
 * MGBroker is used to broker requests to the MapGuide Open Source
 * mapagent interface.  It is a very simple class that is configured
 * with a URL and credentials via the setSiteURL method and can
 * send requests to the server via the dispatchRequest method.
 */
Fusion.Lib.MGBroker = Class.create();
Fusion.Lib.MGBroker.prototype = {
    /**
     * the URL to a MapGuide Open Source installation.  Set this using
     * setSiteURL
     * @type String
     */
    mapGuideURL : '',
    /**
     * the agent URL for the MapGuide Open Source installation.  Set from
     * setSiteURL
     * @type String
     */
    mapAgentURL : '',
    
    /**
     * default method for communicating with the server, if set
     * this overrides the individual request methods
     */
    method: null,
    
    /**
     * @class
     * MGBroker constructor
     *
     * @constructor
     * create a new MGBroker instance
     */
    initialize : function() { 
    },
    /**
     * send a request to the MapGuide Open Source server using
     * XMLHttpRequest and return the result to the specified
     * function.
     * @param r {Object} an MGRequest-subclass instance that
     *        defines the operation to request.
     * @param f {Function} a function object to invoke when the
     *        XMLHttpRequest call completes
     */
    dispatchRequest : function( r, f ) {
        var s = r.encode() + '&ts='+(new Date()).getTime();
        if (this.method) {
            r.options.method = this.method;
        }
        var a = new Ajax.Request( this.mapAgentURL, 
                 Object.extend({ parameters:r.parameters, onComplete:f }, r.options ) );
        a.originalRequest = r;
    },
    /**
     * set up a connection to a MapGuide Open Source site.  This function
     * expects that url is in the form http(s)://<address>/path-to-mapguide.
     * Path-to-mapguide is should be the base URL to a MapGuide Open
     * Source install.  It is expected that the mapagent is
     * in the expected place (mapagent/mapagent.fcgi) under that URL.  If
     * (for some strange reason) its not, then you can include the full
     * path to mapagent.fcgi in the URL and this function won't try to
     * guess its location.
     * The user name and password are passed on using basic HTML
     * authentication (http://<user>:<pass>@<server>/path-to-mapguide).
     * @param url {String} a properly formatted universal reverse locator
     *        to a MapGuide Open Source installation.
     * @param user {String} a valid user name
     * @param pass {String} the password for the given user.
     */
    setSiteURL : function(url, user, pass) {
        //url = url.replace('://', '://'+user+':'+pass+'@');
        this.user = user;
        this.pass = pass;
        if (url.indexOf('mapagent.fcgi') == -1) {
            if (url.charAt(url.length - 1) != '/') {
                url = url + '/';
            }
            this.mapGuideURL = url;            
            url = url + 'mapagent/mapagent.fcgi';
        }
        this.mapAgentURL = url;
    },
    /**
     * remove all authentication information from the broker
     */
    clearSiteURL: function() {
        this.user = '';
        this.pass = '';
        this.mapGuideURL = '';
        this.mapAgentURL = '';
    }
};

/**
 * MGRequest is the base class for all broker-compatible requests.  A request
 * is a wrapper around an operation that is supported by the mapagent.
 */
Fusion.Lib.MGRequest = Class.create();
Fusion.Lib.MGRequest.prototype = {
    /**
     * core options shared by all requests
     */
    options : null,
    
    /**
     * core parameters shared by all requests
     */
    parameters : null,
    
    /**
     * @constructor
     * initialize a new instance of MGRequest
     */
    initializeRequest : function() {
        this.options = { method:'post' };
        this.parameters = { version : '1.0.0', locale : 'en' };
    },
    
    /**
     * set the parameters associated with this request.  Parameters are
     * dependent on the specific MGRequest subclass except for two
     * mandatory parameters, version and locale, that are provided by
     * this base class.
     *
     * @param o {Object} an object that contains named key : value pairs 
     * representing parameters to a request
     */
    setParams : function( o ){ Object.extend( this.parameters, (o || {}) ); },

    /**
     * set the options associated with this request
     * @param o {Object} an object that contains named key : value pairs 
     * representing for a request
     */
    setOptions : function( o ){ Object.extend( this.options, (o || {}) ); },
    
    /**
     * returns a string containing all the request parameters in URL form suitable
     * for appending to a URL.
     * @return {String} the parameters in URL form.
     */
    encode : function() {
        var s = sep = '';
        for (var p in this.parameters) {
            if (this.parameters[p]) {
                s = s + sep + p + '=' + encodeURI(this.parameters[p]);
            }
            sep = '&';
        }
        return s;
    }
};

Fusion.Lib.MGRequest.MGEnumerateResources = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResources.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 * @class MGEnumerateResources
 * encapsulate a request to the server to enumerate resources in the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResources.prototype, {
    /**
     * @constructor
     * initialize a new instance of MGEnumerateResources
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @param type {String} optional parameter indicating the type of resources
     * to enumerate.  If not set, it will default to an empty string which
     * indicates all types will be returned.
     *
     * @param depth {Integer} optional parameter that controls the depth of the
     * resource tree to enumerate.  If not set, it will default to -1 which
     * means the tree will be fully enumerated.
     *
     * @return {Object} an instance of MGEnumerateResources
     */
    initialize : function( resourceID, type, depth ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCES',
            resourceid : (resourceID || "Library://"),
            type : (type || ""),
            depth : (typeof depth == 'undefined' ? -1 : depth) } );
    }
});

Fusion.Lib.MGRequest.MGGetResourceContent = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetResourceContent.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to get resource contents from the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetResourceContent.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceContent
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetResourceContent
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCECONTENT',
            resourceid : (resourceID || "Library://")
        } );
    }
});

Fusion.Lib.MGRequest.MGGetResourceHeader = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetResourceHeader.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to get resource header from the library.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetResourceHeader.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetResourceHeader
     *
     * @param resourceID {String} optional parameter indicating the resource
     * to enumerate.  If not set or null, it defaults to "Library://" which
     * is the root of the library.
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetResourceHeader
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETRESOURCEHEADER',
            resourceid : (resourceID || "Library://")
        } );
    }
});

Fusion.Lib.MGRequest.MGCreateSession = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGCreateSession.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to create a new session on the server.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGCreateSession.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGCreateSession
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGCreateSession
     */
    initialize : function( ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'CREATESESSION'
        } );
    }
});

Fusion.Lib.MGRequest.MGCopyResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGCopyResource.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to copy a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGCopyResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGCopyResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGCopyResource
     */
    initialize : function( sourceID, destinationID, overwrite ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'COPYRESOURCE',
            source : sourceID,
            destination: destinationID,
            overwrite : overwrite
        } );
    }
});

Fusion.Lib.MGRequest.MGDeleteResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGDeleteResource.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to delete a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGDeleteResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGDeleteResource
     *
     * @param resourceID {String} the id of the resource to delete
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGDeleteResource
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DELETERESOURCE',
            resourceid : resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGMoveResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGMoveResource.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to move a resource in the repository.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGMoveResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
     *
     * @param sourceID {String} the Resource ID of the source
     * @param destinationID {String} the Resource ID of the destination
     * @param overwrite {Boolean} overwrite the destination if it exists
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGMoveResource
     */
    initialize : function( sourceID, destinationID, overwrite ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'MOVERESOURCE',
            source : sourceID,
            destination : destinationID,
            overwrite : overwrite
        } );
    }
});

Fusion.Lib.MGRequest.MGMoveResource = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGMoveResource.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to set the content XML of a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGMoveResource.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGMoveResource
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGMoveResource
     */
    initialize : function( resourceID, content, header ) {
        this.initializeRequest();
        this.setParams( {
            method: 'post', /* SetContent requires post method */
            operation : 'SETRESOURCE',
            resourceid : resourceID,
            content : content,
            header : header
        } );
    }
});

Fusion.Lib.MGRequest.MGDescribeSchema = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGDescribeSchema.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to describe the schema of a FeatureSource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGDescribeSchema.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGDescribeSchema
     *
     * @param resourceID {String} the id of the resource to describe the schema for
     * @param schema {String} what does this do?
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGDescribeSchema
     */
    initialize : function( resourceID, schema ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'DESCRIBEFEATURESCHEMA',
            resourceid : resourceID,
            schema : schema
        } );
    }
});

Fusion.Lib.MGRequest.MGGetSpatialContexts = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetSpatialContexts.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to retrieve the spatial context of a resource.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetSpatialContexts.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetSpatialContexts
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     * @param activeonly {Boolean} what does this do?
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetSpatialContexts
     */
    initialize : function(resourceID, activeonly) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETSPATIALCONTEXTS',
            resourceid : resourceID,
            activeonly : activeonly?'1':'0'
        } );
    }
});

Fusion.Lib.MGRequest.MGEnumerateResourceReferences = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResourceReferences.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to enumerate the references to a resource id.
 *
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResourceReferences.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     *
     * @param resourceID {String} the id of the resource to retrieve the spatial context for
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGEnumerateResourceReferences
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEREFERENCES',
            resourceid: resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGEnumerateResourceData = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGEnumerateResourceData.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      Fusion.Lib.MGRequest.MGDescribeSchema should be used for those sources.
 */
Object.extend(  Fusion.Lib.MGRequest.MGEnumerateResourceData.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGEnumerateResourceData
     *
     * @param resourceID {String} the id of the FeatureSource to retrieve data for
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGEnumerateResourceData
     */
    initialize : function( resourceID ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'ENUMERATERESOURCEDATA',
            resourceid: resourceID
        } );
    }
});

Fusion.Lib.MGRequest.MGGetVisibleMapExtent = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetVisibleMapExtent.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to enumerate the data associated with
 * a FeatureSource
 * N.B. This does not enumerate resource data for 'unmanaged' FeatureSources
 *      (those referencing files or directories outside the respository)
 *      Fusion.Lib.MGRequest.MGDescribeSchema should be used for those sources.
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetVisibleMapExtent.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the name of the map
     * @param viewCenterX {String} the horizontal center of the view
     * @param viewCenterY {String} the vertical center of the view
     * @param viewScale {String} the scale of the map
     * @param dataExtent {String} the extent of the data 
     * @param displayDpi {String} the DPI of the display
     * @param displayWidth {String} the width of the map
     * @param displayHeight {String} the height of the map
     * @param showLayers {String} a list of layer names to show
     * @param hideLayers {String} a list of layer names to hide
     * @param showGroups {String} a list of group names to show
     * @param hideGroups {String} a list of groupnames to hide
     * @param refreshLayers {String} a list of layers that need to be refreshed
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetVisibleMapExtent
     */
    initialize : function( sessionId, mapName, viewCenterX, viewCenterY,
                           viewScale, dataExtent, displayDpi, displayWidth, 
                           displayHeight, showLayers, hideLayers, 
                           showGroups, hideGroups, refreshLayers ) {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETVISIBLEMAPEXTENT',
            session: sessionId,
            mapname: mapName,
            setviewcenterx: viewCenterX,
            setviewcentery: viewCenterY,
            setviewscale: viewScale,
            setdataextent: dataExtent,
            setdisplaydpi: displayDpi,
            setdisplaywidth: displayWidth,
            setdisplayheight: displayHeight,
            showlayers: showLayers,
            hidelayers: hideLayers,
            showgroups: showGroups,
            hidegroups: hideGroups,
            refreshlayers: refreshLayers
        } );
    }
});


Fusion.Lib.MGRequest.MGQueryMapFeatures = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGQueryMapFeatures.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 */
Object.extend(  Fusion.Lib.MGRequest.MGQueryMapFeatures.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGQueryMapFeatures
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the id of the session to restore
     * @param geometry (sting wkt} gemetry to use for selection.  Example : POLYGON(x1 y1, x2,y2)
     * @param maxFeatures {integer} number of maximum results (-1 to indicate no maximum)
     * @param selectionPersist {boolean} save the selection (valid values are 0 and 1) 
     * @param selectionVariant {String} indicates the spatial operation. Valid values are 'INTERSECTS', ...
     *@param layerNames {String} comma separated list of layer names to include in the query
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGQueryMapFeatures
     */
    initialize : function( sessionId, mapName, geometry, maxFeatures, persist, selectionVariant, layerNames ) 
    {
        this.initializeRequest();
        this.setParams( {
            operation : 'QUERYMAPFEATURES',
            session: sessionId,
            mapname: mapName,
            geometry: geometry,
            maxFeatures: maxFeatures,
            persist: persist,
            selectionVariant: selectionVariant,
            layerNames: layerNames
        } );
    }
});

Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope = Class.create();
Object.extend(Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope.prototype, Fusion.Lib.MGRequest.prototype);
/**
 * @extends MGRequest
 *
 * encapsulate a request to the server to query map features on 
 * selectable layers
 */
Object.extend(  Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope.prototype, {
    /**
     * @constructor
     * initialize a new instance of Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     *
     * @param sessionId {String} the id of the session to restore
     * @param mapName {String} the id of the session to restore
     * @param features (String XML} a feature set selection XML
     *
     * @return {Object} an instance of Fusion.Lib.MGRequest.MGGetFeatureSetEnvelope
     */
    initialize : function( sessionId, mapName, features ) 
    {
        this.initializeRequest();
        this.setParams( {
            operation : 'GETFEATURESETENVELOPE',
            session: sessionId,
            mapname: mapName,
            featureSet: features
        } );
    }
});/*****************************************************************************
 * @project Fusion
 * @revision $Id: Widget.js 818 2007-09-14 20:57:20Z madair $
 * @purpose Base Class for all widgets
 * @author pspencer@dmsolutions.ca
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
 * ***************************************************************************
 *
 * This is the base class for all widgets.  It provides some basic
 * functionality that all widgets should need.
 *
 * ***************************************************************************/
 
Fusion.Event.WIDGET_STATE_CHANGED = Fusion.Event.lastEventId++;
 
Fusion.Widget = Class.create();
Fusion.Widget.prototype = {
    bIsMutuallyExclusive: null,
    sName: null,
    oMap: null,
    bEnabled: false,
    mapLoadedWatcher: null,
    paramRegister: null,
    groups: [],
    group: null,
    domObj: null,
    
    /**
     * initialize the widget
     * @param sName {string} the name of the widget
     */
    initialize: function(widgetTag, bMutEx) {
        this.bIsMutuallyExclusive = bMutEx;
        this.sName = widgetTag.type;
        this.widgetTag = widgetTag;
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.registerEventID(Fusion.Event.WIDGET_STATE_CHANGED);
        
        var group = widgetTag.extension.Group ? widgetTag.extension.Group[0] : '';
        if (group != '') {
            if (!this.groups[group]) {
                this.groups[group] = [];
            }
            this.groups[group].push(this);
            this.group = group;
        }
        this.setMap(widgetTag.getMapWidget());
        
        if (widgetTag.name) {
            this.domObj = $(widgetTag.name);
        }

        this.paramRegister = [];
    },
    /**
     * set the map object that this widget is associated with
     * @param oMap {Object} the map
     */
    setMap: function(oMap) {
        if (this.mapLoadedWatcher) {
            this.oMap.deregisterForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
            this.mapLoadedWatcher = null;
        }
        
        this.oMap = oMap;
        if (oMap) {
            this.mapLoadedWatcher = this._mapLoaded.bind(this);
            oMap.registerForEvent(Fusion.Event.MAP_LOADED, this.mapLoadedWatcher);
        }
        
        if (oMap && oMap.isLoaded()) {
            this.enable();
        } else {
            this.disable();
        }
    },
    /**
     * accessor to get the Map object that this widget is associated with
     * @return {object} the map
     */
    getMap: function() {
        return this.oMap;
    },
    
    /**
     */
    _mapLoaded: function() {
        if (this.oMap && this.oMap.isLoaded()) {
            //console.log('enable');
            this.enable();
        } else {
            //console.log('disable');
            this.disable();
        }
    },
    
    /** 
     * set whether this widget is mutually exclusive on its map
     * @param bIsMutEx {boolean} is the widget mutually exclusive?
     */
    setMutEx: function(bIsMutEx) {
        this.bIsMutuallyExclusive = bIsMutEx;
    },
    
    /**
     * accessor to determine if the widget should be activated mutually
     * exclusively from other widgets on the map.
     * @return {boolean} true if the widget is mutually exclusive
     */
    isMutEx: function() {
        return this.bIsMutuallyExclusive;
    },
    
    /**
     * accessor to return the name of the widget.  Mostly for debugging
     * @return {string} the name of the widget
     */
    getName: function() {
        return this.sName;
    },
    
    isEnabled: function() { return this.bEnabled; },
    
    enable: function() { this.bEnabled = true; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    disable: function() { this.bEnabled = false; this.triggerEvent(Fusion.Event.WIDGET_STATE_CHANGED, this);},

    setParameter : function(param, value){},

    registerParameter : function(param) {
      this.paramRegister.push(param);
    }
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ButtonBase.js 747 2007-09-05 00:19:18Z pspencer $
 * @purpose Base widget for button type widgets
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * Utility base class for a button type widget.
 *
 * Configurations for the buttons come from :
 * - html area : corresponds to the web layout command name
 * - width,height : default are 20,20. Classes should redifine teh
 *   getButtonWidth and getButtonHeight to provide othe values
 * - imageurl : image that will be used when the button is active.
 *   Default value is read from the web layout command imageurl parameter.
 *   Classes can redifine getImageURL.
 * - imageurl : image that will be used when the button is active.
 *   Default value is read from the web layout command disabledimageurl 
 *   parameter.  Classes can redifine getDisabledImageURL.
 * 
 * Clases inheriting should redefine the function activateTool which is
 * called when the button is clicked
 * **********************************************************************/

 
Fusion.Tool.ButtonBase = Class.create();
Fusion.Tool.ButtonBase.prototype = {
    /**
     * constructor
     */
    initialize : function() {
        /* overload enable/disable.  Normal inheritance should
         * work but because we use inheritFrom, it doesn't overload
         * Widget's enable/disable functions.  We do it manually
         * here.
         */
        this.enable = Fusion.Tool.ButtonBase.prototype.enable;
        this.disable = Fusion.Tool.ButtonBase.prototype.disable;

        //console.log('Fusion.Tool.ButtonBase.initialize');

        this._oButton = new Fusion.Tool.Button(this.domObj, this.widgetTag);
        if (!this.isEnabled()) {
            this._oButton.disableTool();
        }
        this.clickWatcher = this.clickCB.bind(this);
        this._oButton.observeEvent('click', this.clickWatcher);
    },
    
    clickCB : function(e) {
        //console.log('Fusion.Tool.ButtonBase.clickCB');
        if (this.isEnabled()) {
            this.activateTool();
        }
        /* I put this in to prevent the context menu from activating tools twice but it doesn't seem to be needed now */
        /* Event.stop(e); */
        //remove the focus on the button to prevent a problem in IE with some
        //buttons retaining their background colour when they shouldn't
        this._oButton._oButton.domObj.blur();
    },

    activateTool :  function() {
        //console.log('Fusion.Tool.ButtonBase.activateTool');
        if (this.execute) {
            this.execute();
        }
        if (this.group) {
            this._oButton.activateTool();
            for (var i=0; i<this.groups[this.group].length; i++) {
                if (this.groups[this.group][i] != this) {
                    this.groups[this.group][i]._oButton.deactivateTool();
                }
            }
        }
    },
    
    enable: function() {
        //console.log('button base enable');
        Fusion.Widget.prototype.enable.apply(this,[]);
        this._oButton.enableTool();
    },
    
    disable: function() {
        //console.log('button base disable');
        Fusion.Widget.prototype.disable.apply(this,[]);
        this._oButton.disableTool();
    }
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: $
 * @purpose Base widget for menu type widgets
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * **********************************************************************/

 
Fusion.Tool.MenuBase = Class.create();
Fusion.Tool.MenuBase.prototype = {
    /**
     * constructor
     */
    initialize : function() {
        /* overload enable/disable.  Normal inheritance should
         * work but because we use inheritFrom, it doesn't overload
         * Widget's enable/disable functions.  We do it manually
         * here.
         */
        this.enable = Fusion.Tool.MenuBase.prototype.enable;
        this.disable = Fusion.Tool.MenuBase.prototype.disable;

        //console.log('Fusion.Tool.MenuBase.initialize');
            var options = {};
            options.imgPath = this.widgetTag.imageUrl;
            options.imgClass = this.widgetTag.imageClass;
            options.tooltip = this.widgetTag.tooltip;
            options.label = this.widgetTag.label;

        this.oMenu = new Jx.Menu(options);
$(this.widgetTag.name).appendChild(this.oMenu.domObj);
    },
    
    enable: function() {
        //TODO: figure out how to enable and disable menu widgets
        Fusion.Widget.prototype.enable.apply(this,[]);
    },
    
    disable: function() {
        Fusion.Widget.prototype.disable.apply(this,[]);
    }
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ButtonTool.js 758 2007-09-06 15:29:42Z pspencer $
 * @purpose Utility class used by button type widgets
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * extended description
 *
 ***********************************************************************/
 
Fusion.Tool.Button = Class.create();
Fusion.Tool.Button.prototype = {
    bActive: null,
    sActiveClass: 'jxButtonActive',
    sDisabledClass: 'jxButtonDisabled',
  
    initialize : function(domObj, widgetTag)  {
        //console.log('Fusion.Tool.Button.initialize');
        var json = widgetTag.extension;
        
        this.buttonAction = new Jx.Action(this.buttonClicked.bind(this));
        if (domObj) {
            var options = {};
            options.imgPath = widgetTag.imageUrl;
            options.imgClass = widgetTag.imageClass;
            options.tooltip = widgetTag.tooltip;
            options.label = widgetTag.label;
            this._oButton = new Jx.Button(this.buttonAction, options);
            domObj.appendChild(this._oButton.domObj);
        }
        this.bActive = false;
        this.bEnabled = true;
        
        if (widgetTag.disabled) {
            this.disableTool();
        }
    },
    
    buttonClicked: function() {
        
    },

    observeEvent : function(sEvent, fnCB) {
        if (this._oButton) {
             Event.observe(this._oButton.domObj, sEvent, fnCB);
        }
    },
    
    stopObserveEvent : function(sEvent, fnCB) {
        if (this._oButton) {
             Event.stopObserving(this._oButton.domObj, sEvent, fnCB);
        }
    },
    
    enableTool : function() {
        this.buttonAction.setEnabled(true);
    },

    disableTool : function() {
        this.buttonAction.setEnabled(false);
    },

    activateTool : function() {
        //console.log('Fusion.Tool.Button.activateTool');
        this.bActive = true;
        if (!this._oButton) {
            return;
        }
        if (this._sImageURL != null && this._sImageURL.length > 0) {
            this._oButton.setImage(this._sImageURL);
        }
        if (this.sActiveClass != '') {
            Element.addClassName(this._oButton.domA, this.sActiveClass);
        }
    },

    deactivateTool :  function() {
        //console.log('Fusion.Tool.Button.deactivateTool');
        this.bActive = false;
        if (!this._oButton) {
            return;
        }
        if (this.sActiveClass != '') {
            Element.removeClassName(this._oButton.domA, this.sActiveClass);
        }
    },

    clickCB : function(e) { this.activateTool(); },
    isActive: function() {return this.bActive;}
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: CanvasTool.js 800 2007-09-09 23:08:16Z pspencer $
 * @purpose Base class for widgets that want to use canvas functions
 * @author pspencer@dmsolutions.ca
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
 * ********************************************************************
 *
 * 
 * **********************************************************************/

Fusion.Tool.Canvas = Class.create();
Fusion.Tool.Canvas.prototype = 
{
    context: null,
    canvas: null,
    width: null,
    height: null,
    
    initialize : function()
    {
        this.context = null;
        this.canvas = null;
        this.width = null;
        this.height = null;
        
        this.mouseMoveCB = this.mouseMove.bindAsEventListener(this);
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
        this.mouseDownCB = this.mouseDown.bindAsEventListener(this);
        this.dblClickCB = this.dblClick.bindAsEventListener(this);
        
        this.resizeCanvasFn = this.resizeCanvas.bind(this);
    },
    
    /**
     * (public) clearContext()
     *
     * wipe the slate clean
     */
    clearContext: function() {
        //console.log('Fusion.Tool.Canvas.clearContext');
        if (this.context) {
            this.context.clearRect(0,0,this.width,this.height);
        }
    },

    activateCanvas: function() {
        var map = this.getMap();
        map.registerForEvent(Fusion.Event.MAP_RESIZED, this.resizeCanvasFn);
        var domObj = map.getDomObj();
        
        var size = Element.getDimensions(domObj);
        this.width = size.width;
        this.height = size.height;
        
        /* create dynamic canvas */
        if (!this.canvas) {
            this.canvas = document.createElement('canvas');
            
            // we need to init this for IE 
            if (typeof G_vmlCanvasManager != "undefined") { 
                document.getElementsByTagName('BODY')[0].appendChild(this.canvas);
                G_vmlCanvasManager.initElement(this.canvas); 
                this.canvas = document.getElementsByTagName('BODY')[0].lastChild;
            } 
            
            this.canvas.id = 'featureDigitizer';
            this.canvas.style.position = 'absolute';
            this.canvas.style.top = '0px';
            this.canvas.style.left = '0px';
            this.canvas.style.width = this.width+'px';
            this.canvas.style.height = this.height+'px';
            this.canvas.width = this.width;
            this.canvas.height = this.height;
            this.canvas.style.zIndex = 99;
            
        }
    
        domObj.appendChild(this.canvas);
        if (!this.context) {
            this.context = this.canvas.getContext('2d');
        }
        this.canvas.style.width = this.width+'px';
        this.canvas.style.height = this.height+'px';
        this.canvas.width = this.width;
        this.canvas.height = this.height;
        
        map.observeEvent('mousemove', this.mouseMoveCB);
        map.observeEvent('mouseup', this.mouseUpCB);
        map.observeEvent('mousedown', this.mouseDownCB);
        map.observeEvent('dblclick', this.dblClickCB);
    },
    
    resizeCanvas: function() {
        var map = this.getMap();
        var domObj = map.getDomObj();
        var size = Element.getDimensions(domObj);
        this.width = size.width;
        this.height = size.height;
        this.canvas.style.width = this.width+'px';
        this.canvas.style.height = this.height+'px';
        this.canvas.width = this.width;
        this.canvas.height = this.height;        
    },
    
    /**
     * (public) deactivate()
     *
     * deactivate the line digitizing tool
     */
    deactivateCanvas: function() {
        //console.log('Fusion.Tool.Canvas.deactivate');
        var map = this.getMap();
        map.deregisterForEvent(Fusion.Event.MAP_RESIZED, this.resizeCanvasFn);
        map.getDomObj().removeChild(this.canvas);
        this.context.clearRect(0,0,this.width,this.height);
        map.stopObserveEvent('mousemove', this.mouseMoveCB);
        map.stopObserveEvent('mouseup', this.mouseUpCB);
        map.stopObserveEvent('mousedown', this.mouseDownCB);
        map.stopObserveEvent('dblclick', this.dblClickCB);
    },
    
    /**
     * (public) mouseDown(e)
     *
     * handle the mouse down event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseDown: function(e) { },

    /**
     * (public) mouseUp(e)
     *
     * handle the mouse up event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseUp: function(e) { },

    /**
     * (public) mouseMove(e)
     *
     * handle the mouse move event
     *
     * @param e Event the event that happened on the mapObj
     */
    mouseMove: function(e) { },

    /**
     * (public) dblClick(e)
     *
     * handle the mouse dblclick event
     *
     * @param e Event the event that happened on the mapObj
     */
    dblClick: function(e) { }
};

Fusion.Tool.Canvas.Point = Class.create();
Fusion.Tool.Canvas.Point.prototype = {
    center: null,
    radius: null,
    lineStyle: null,
    fillStyle: null,
    
    initialize: function(map) {
        this.center = new Fusion.Tool.Canvas.Node(0,0, map);
        this.radius = 5;
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
        this.segments = [];
        
    },
    
    setPoint: function(x,y) {
        this.center.set(x,y);
    },
    
    getPoint: function() {
        this.center.updateGeo();
        return {x:this.center.x, y:this.center.y};
    },
    
    draw: function( context ) {
        var x = this.center.px;
        var y = this.center.py;
        var radius = this.radius;
        
        this.fillStyle.apply(context);
        this.lineStyle.apply(context);
        
        context.beginPath();
        context.arc(x,y,radius, 0, 2*Math.PI, 1);
        context.closePath();
        context.fill(); 
        context.stroke();
        
    },

    getNodes: function() {
        return [this.center];
    },

    clean: function() {},
    
    updateGeo: function() {
        this.center.updateGeo();
    },
    
    updatePx: function() {
        this.center.updatePx();
    }
};

Fusion.Tool.Canvas.Circle = Class.create();
Fusion.Tool.Canvas.Circle.prototype = {
    map: null,
    center: null,
    radius: null,
    radiusPx: null,
    start: null,
    end: null,
    lineStyle: null,
    fillStyle: null,
    
    initialize: function(map) {
        this.map = map;
        this.center = new Fusion.Tool.Canvas.Node(0,0, map);
        this.radius = 1;
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
        this.segments = [];
    },
    
    setCenter: function(x,y) {
        this.center.set(x,y);
    },
    
    setRadius: function(r) {
        if (r > 1 || r < -1) {
            this.radius = Math.abs(r);
            this.radiusPx = this.map.geoToPixMeasure(this.radius);
        } else {
            this.radius = 1;
            this.radiusPx = 1;
        }
    },
    
    draw: function( context ) {
        var x = this.center.px;
        var y = this.center.py;
        var radius = this.radiusPx;
        this.fillStyle.apply(context);
        this.lineStyle.apply(context);
        
        context.beginPath();
        if (this.start && this.end) {
            context.moveTo(x,y);
            var s = this.start;
            var e = this.end;
            if (s < e) {
                var t = s;
                s = e;
                e = t;
            }
            var sx = x + Math.sin(s) * radius;
            var sy = y - Math.cos(s) * radius;
            context.lineTo(sx,sy);
            context.arc(x,y,radius, s - Math.PI/2, e - Math.PI/2, 1);
            context.lineTo(x, y);
        } else {
            context.arc(x,y,radius, 0, 2*Math.PI, 1);
        }
        context.closePath();
        context.fill(); 
        context.stroke();        
    },
    
    getNodes: function() {
        return [this.center];
    },
    
    clean: function() {},
    
    updateGeo: function() {
        this.center.updateGeo();
        this.radius = this.map.pixToGeoMeasure(this.radiusPx);
    },
    
    updatePx: function() {
        this.center.updatePx();
        this.radiusPx = this.map.geoToPixMeasure(this.radius);
    }
};

Fusion.Tool.Canvas.Polygon = Class.create();
Fusion.Tool.Canvas.Polygon.prototype = {
    segments: null,
    lineStyle: null,
    fillStyle: null,
    map: null,
    
    initialize: function(map) {
        this.map = map;
        this.segments = [];
        this.lineStyle = new Fusion.Tool.Canvas.Style({lineWidth:2,strokeStyle:'rgba(0,0,0,1.0)'});
        this.fillStyle = new Fusion.Tool.Canvas.Style({fillStyle:'rgba(0,0,255, 0.5)'});
    },

    clean: function() {
        var nodes = this.getNodes();
        this.segments = [];
        var n1 = nodes[0];
        //console.log('n1: '+ n1);
        var n2 = nodes[1];
        for (var i=1; i<nodes.length;i++) {
            if (n1.x != n2.x || n1.y != n2.y) {
                this.addSegment(new Fusion.Tool.Canvas.Segment(n1,n2));
                //console.log('n2: '+ n2);
                n1 = n2;
            }
            n2 = nodes[i];
        }
        
        this.addSegment(new Fusion.Tool.Canvas.Segment(n1, nodes[0]));
        //console.log(this);
    },

    getNodes: function() {
        var nodes = [];
        nodes.push(this.segments[0].from);
        for (var i=0; i<this.segments.length; i++) {
            nodes.push(this.segments[i].to);
        }
        return nodes;
    },

    /*
     * reverse the nodes in the feature
     * and adjust segments
     */
    reverseNodes: function() {
        var nSegments = this.segments.length;
        if (!nSegments) {
            return;
        }
        //flip nodes on each segment
        for (var i=0; i < nSegments; i++) {
            var seg = this.segments[i];
            var tmp = seg.from;
            seg.from = seg.to;
            seg.to = tmp;
        };
        //reverse segment order
        this.segments.reverse();
    },
    
    /*
     * remove node from the nodes in this feature
     * and adjust segments
     */
    removeNode: function(node) {
        //end cases
        if (node == this.segments[0].from) {
            this.segments[0].from = null;
            this.segments.shift();
            this.segments[0].from = this.segments[this.segments.length - 1].to;
            return;
        }
        if (node == this.segments[this.segments.length -1].from) {
            this.segments[this.segments.length -1].from = null;
            this.segments.pop();
            this.segments[0].from = this.segments[this.segments.length - 1].to;
            return;
        }
        //general case
        for (var i=1; i < this.segments.length; i++) {
            if (node == this.segments[i].from){
                this.segments[i-1].to = this.segments[i].to;
                this.segments[i].from = null;
                this.segments.splice(i, 1);
                return;
            }
        };
        
    },
    
    draw: function( context ) {
        var x = this.segments[0].from.px;
        var y = this.segments[0].from.py;
        if (this.segments.length > 2) {
            /* draw closing line and fill */
        
            this.fillStyle.apply(context);
            context.beginPath();
            context.moveTo(x,y);
            for (var i=0; i<this.segments.length; i++) {
                var s = this.segments[i];
                context.lineTo(s.to.px, s.to.py);         
            }
            context.lineTo(x,y); //closing line
            context.closePath();
            context.fill(); 
        }
        /* draw outline */
        this.lineStyle.apply(context);
        for (var i=0; i<this.segments.length; i++) {
            this.segments[i].draw(context);
        }
    
        var last = this.lastSegment();
        context.beginPath();
        context.moveTo(last.to.px,last.to.py);
        context.lineTo(x,y);
        context.stroke();
    },

    addSegment: function( s ) {
        s.normalStyle = this.lineStyle;
        this.segments[this.segments.length] = s;
        //console.log('add segment ' + s);
    },

    lastSegment: function() {
        return this.segments[this.segments.length-1];
    },

    /* find the segment with the given node as its end
     * @param Object node - the node at the end
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if nothing is found.
     */
     segmentTo: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasTo(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* find the segment with the given node as its start
     * @param Object node - the node at the start
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if there is none.
     */
     segmentFrom: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasFrom(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* extend an existing line by creating a new segment attached
     * to the last segment
     * @return the new segment
     */
    extendLine: function() {
        var last = this.lastSegment();
        var newNode = new Fusion.Tool.Canvas.Node(last.to.x, last.to.y, this.map);
        var newSegment = new Fusion.Tool.Canvas.Segment( last.to, newNode );
        this.addSegment(newSegment);
        return newSegment;  
    },

    /* determine if the passed pixel coordinate is within this feature
     * @param point Object - {px,py} representation of point
     * @return true if the point is contained
     *
     * uses crossing test (Jordan Curve Theorem) algorithm discussed at
     * http://www.acm.org/tog/editors/erich/ptinpoly/
     */
    contains: function(node) {
        return true;  
    },
    
    
    toString: function() {
        var szFeature = this.segments[0].from.toString();
        for (var i=0; i < this.segments.length; i++) {
            szFeature += ',' + this.segments[i].to.toString();
        }
        return 'POLYGON(' + szFeature + ')';
    },
    
    updateGeo: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updateGeo();
        }
    },
    
    updatePx: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updatePx();
        }
    }
};

Fusion.Tool.Canvas.Line = Class.create();
Fusion.Tool.Canvas.Line.prototype = {
    segments: null,
    lineStyle: null,
    map: null,
    
    initialize: function(map) {
        this.map = map;
        this.segments = [];
        this.lineStyle = new Fusion.Tool.Canvas.Style({strokeStyle:'rgba(0,0,0,1.0)'});
    },

    clean: function() {
        var nodes = this.getNodes();
        this.segments = [];
        var n1 = nodes[0];
        var n2 = nodes[1];
        for (var i=1; i<nodes.length;i++) {
            //console.log('n1: '+ n1);
            //console.log('n2: '+ n2);
            n2 = nodes[i];
            if (n1.x != n2.x || n1.y != n2.y) {
                this.addSegment(new Fusion.Tool.Canvas.Segment(n1,n2));
                n1 = n2;
            }
        }
        //console.log(this);
    },

    getNodes: function() {
        var nodes = [];
        nodes.push(this.segments[0].from);
        for (var i=0; i<this.segments.length; i++) {
            nodes.push(this.segments[i].to);
        }
        return nodes;
    },

    /*
     * reverse the nodes in the feature
     * and adjust segments
     */
    reverseNodes: function() {
        var nSegments = this.segments.length;
        if (!nSegments) {
            return;
        }
        //flip nodes on each segment
        for (var i=0; i < nSegments; i++) {
            var seg = this.segments[i];
            var tmp = seg.from;
            seg.from = seg.to;
            seg.to = tmp;
        };
        //reverse segment order
        this.segments.reverse();
    },
    
    /*
     * remove node from the nodes in this feature
     * and adjust segments
     */
    removeNode: function(node) {
        //end cases
        if (node == this.segments[0].from) {
            this.segments[0].from = null;
            this.segments.shift();
            return;
        }
        if (node == this.segments[this.segments.length -1].from) {
            this.segments[this.segments.length -1].from = null;
            this.segments.pop();
            return;
        }
        //general case
        for (var i=1; i < this.segments.length; i++) {
            if (node == this.segments[i].from){
                this.segments[i-1].to = this.segments[i].to;
                this.segments[i].from = null;
                this.segments.splice(i, 1);
                return;
            }
        };
        
    },

    draw: function( context ) {
        for (var i=0; i<this.segments.length; i++) {
            this.segments[i].draw(context);
        }
    },

    addSegment: function( s ) {
        s.normalStyle = this.lineStyle;
        this.segments[this.segments.length] = s;
    },

    lastSegment: function() {
        return this.segments[this.segments.length-1];
    },

    /* find the segment with the given node as its end
     * @param Object node - the node at the end
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if nothing is found.
     */
     segmentTo: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasTo(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* find the segment with the given node as its start
     * @param Object node - the node at the start
     * @param Int tolerance - an optional tolerance in pixels
     * @return the segment or null if there is none.
     */
     segmentFrom: function(node) {
         var margin = arguments.length > 1?arguments[1]:3;
         for (var i=0; i<this.segments.length; i++) {
             if (this.segments[i].hasFrom(node, margin)) {
                 return this.segments[i];
             }
         }
         return null;        
     },

    /* extend an existing line by creating a new segment attached
     * to the last segment
     * @return the new segment
     */
    extendLine: function() {
        var last = this.lastSegment();
        var newNode = new Fusion.Tool.Canvas.Node(last.to.x, last.to.y, this.map);
        var newSegment = new Fusion.Tool.Canvas.Segment( last.to, newNode );
        this.addSegment(newSegment);
        return newSegment;  
    },
    
    updateGeo: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updateGeo();
        }
    },
    
    updatePx: function() {
        for (var i=0; i < this.segments.length; i++) {
            this.segments[i].updatePx();
        }
    }
};

Fusion.Tool.Canvas.Segment = Class.create();
Fusion.Tool.Canvas.Segment.prototype = {
    from: null,
    to: null,
    
    initialize: function(from, to) {
        this.from = from;
        this.to = to;
        this.isEditing = false;
        this.normalStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(0,0,0,1.0)'});
        this.editStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(255,0,0,1.0)'});
    },

    /* returns true if the node is at the end of this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    hasTo: function(node, margin) {
        return this.to.near({x:node.px, y:node.py}, margin);
    },

    /* returns true if the node is at the start of this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    hasFrom: function(node, margin) {
        return this.from.near({x:node.px, y:node.py}, margin);
    },
    
    /* returns true if the given point falls along this segment
     * within the given margin
     * @return Bool true if found within margin, false otherwise
     */
    intersectsPoint: function(point, margin){
        //check bbox
        var minX = Math.min(this.to.px, this.from.px);
        var maxX = Math.max(this.to.px, this.from.px);
        if (point.x > maxX || point.x < minX){return false;};
        var maxY = Math.max(this.to.py, this.from.py);
        var minY = Math.min(this.to.py, this.from.py);
        if (point.y < minY || point.y > maxY){return false;};
        
        //determine slope
        var slope = parseFloat((maxY-minY))/(maxX-minX);
        var segY = slope * (point.x - minX) + minY;
        return (segY - margin < point.y && segY + margin > point.y);

    },
    
    setNormalStyle: function( style ) {
        this.normalStyle = style;
    },

    setEditStyle: function( style ) {
        this.editStyle = style;
    },

    draw: function( context ) {
        /* set up correct style */
        if (this.isEditing) {
            this.editStyle.apply(context);
        } else {
            this.normalStyle.apply(context);
        }
    
        /* draw segment */
        context.beginPath();
        context.moveTo(this.from.px, this.from.py);
        context.lineTo(this.to.px, this.to.py);
        context.closePath();
        context.stroke();
    
        /* draw nodes if editing */
        if (this.isEditing) {
            this.from.draw( context );
            this.to.draw( context );
        }
    },

    /* changes rendering style */
    setEditing: function(bEditing) {
        this.isEditing = bEditing;
    },
    
    toString: function() {
        return this.from.toString() + ', '+ this.to.toString();
    },
    
    updateGeo: function() {
        this.from.updateGeo();
        this.to.updateGeo();
    },
    
    updatePx: function() {
        this.from.updatePx();
        this.to.updatePx();
    }
};

Fusion.Tool.Canvas.Node = Class.create();
Fusion.Tool.Canvas.Node.prototype = {
    x: null,
    y: null,
    px: null,
    py: null,
    uid: null,
    map: null,
    counter: [0],
    isSelected: false,
    
    initialize: function(x,y, map) {
        this.map = map;
        this.set(x,y);
        var p = map.geoToPix(x, y);
        this.setPx(p.x, p.y);
        this.radius = 3;
        this.uid = this.counter[0];
        this.counter[0]++;
        this.normalStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, strokeStyle:'rgba(0,0,0,1.0)'});
        this.selectedStyle = new Fusion.Tool.Canvas.Style({lineWidth:1, fillStyle:'rgba(255,0,0,1.0)',
                                                strokeStyle:'rgba(255,0,0,1.0)'});
    },

    set: function(x,y) {
        this.x = x;
        this.y = y;
        //update px position
        var p = this.map.geoToPix(x, y);
        this.setPx(p.x, p.y);
    },
    
    setPx: function(px, py) {
        this.px = px;
        this.py = py;
    },
    
    updateGeo: function() {
        if (!this.px || !this.py) {return;};
        var g = this.map.pixToGeo(this.px, this.py);
        this.set(g.x, g.y);
    },
    
    updatePx: function() {
        if (!this.x || !this.y) {return;};
        var p = this.map.geoToPix(this.x, this.y);
        this.setPx(p.x, p.y);
    },
    
    /* returns true if the supplied pixel position is
     * within the given tolerance
     * @return Bool true if found within margin, false otherwise
     */
     /*TODO: uses a square envelope for speed but could use radius
      *TODO: should support geographic tolerance
      */
    near: function(point, tolerance) {
        var minX = point.x - tolerance;
        var maxX = point.x + tolerance;
        var maxY = point.y + tolerance;
        var minY = point.y - tolerance;
        return ((this.px > minX && this.px < maxX) && (this.py > minY && this.py < maxY))?true:false;
    },

    /* returns true if this node is
     * within the given bbox
     * @param Array bbox - array of pixel coordinates to search within
     * @return Bool true if found within, false otherwise
     */
    within: function(bbox) {
        //TODO: handle > 2 coord pairs
        var minX = Math.min(bbox[0], bbox[2]);
        var maxX = Math.max(bbox[0], bbox[2]);
        var minY = Math.min(bbox[1], bbox[3]);
        var maxY = Math.max(bbox[1], bbox[3]);
        return ((this.px > minX && this.px < maxX) && (this.py > minY && this.py < maxY))?true:false;
    },

    /* draw a node on a canvas. */
    draw: function( context ) {
        /* set up correct style */
        if (this.isSelected) {
            this.selectedStyle.apply(context);
        } else {
            this.normalStyle.apply(context);
        }

        context.beginPath();
        context.arc(this.px, this.py, this.radius, 0, 2*Math.PI,1);
        context.closePath();
        context.stroke();
        if(this.isSelected){
            context.fill();
        };
    },
    
    /* changes rendering style */
    setSelected: function(bSelected) {
        this.isSelected = bSelected;
    },

    toString: function() {
        return '('+this.uid+') '+ this.x + ' ['+this.px+'px] '+ this.y+ ' ['+this.py+'px] ';
    }
};

/* encapsulate a context style */
Fusion.Tool.Canvas.Style = Class.create();
Fusion.Tool.Canvas.Style.prototype = {
    properties: ['fillStyle',
                 'globalAlpha',
                 'globalCompositeOperation',
                 'lineCap',
                 'lineJoin',
                 'lineWidth',
                 'miterLimit',
                 'shadowBlur',
                 'shadowColor',
                 'shadowOffsetX',
                 'shadowOffsetY',
                 'strokeStyle'],
    
    initialize: function( o ) { 
        for (var i=0; i<this.properties.length; i++) {
            var p = this.properties[i];
            this[p] = o[p] ? o[p]:null;
        }
    },

    set: function( p, v ) {
        this[p] = v;
    },

    apply: function(context) {
        for (var i=0; i<this.properties.length; i++) {
            var p = this.properties[i];
            if (this[p]) {
                context[p] = this[p];
            }
        }
    }
};/********************************************************************** * 
 * @project Fusion
 * @revision $Id: ClickTool.js 718 2007-08-21 17:22:25Z madair $
 * @purpose Utility class to do a click on the map
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * Utility class to manage a click on the map (mouse up event).
 *
 * All classes using this should redefine the execute function
 * **********************************************************************/
Fusion.Tool.Click = Class.create();
Fusion.Tool.Click.prototype =
{
    /**
     * constructor
     * @param oMap {Object} a map widget
     */
    initialize : function()
    {
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
    },

    execute : function(x,y)
    {
    },

    activateClickTool : function()
    {
        //console.log('Fusion.Tool.Click.activateClickTool');
        if (this.oMap) {
            this.oMap.observeEvent('mouseup', this.mouseUpCB);
        }
    },

    deactivateClickTool : function()
    {
        //console.log('Fusion.Tool.Click.deactivateClickTool');
        if (this.oMap) {
            this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        }
    },


    mouseUp : function(e)
    {
        //console.log('Fusion.Tool.Click.mouseUp');
        if (OpenLayers.Event.isLeftClick(e)) {
            var sPixPoint = this.oMap.getEventPosition(e);
            this.execute(sPixPoint.x, sPixPoint.y);
        }
    }
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: RectTool.js 829 2007-09-19 14:45:50Z pspencer $
 * @purpose Utility class to do a rect on the map
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * Utility class to draw a rectangle on the map and return the coordinates
 * through the doAction function.
 * All classes should redifine the doAction function
 * **********************************************************************/
Fusion.Tool.Rectangle = Class.create();
Fusion.Tool.Rectangle.prototype = {
    oRectDiv : null,
    _sStartPos : null,
    _bIsActive : null,

    /**
     * constructor
     * @param oMap {Object} a map widget
     */
    initialize : function() {
        //console.log('Fusion.Tool.Rectangle.initialize');
        this.oRectDiv = document.createElement('div');
        this.oRectDiv.style.position = 'absolute';
        this.oRectDiv.style.border = '1px solid red';
        this.oRectDiv.style.top = '0px';
        this.oRectDiv.style.left = '0px';
        this.oRectDiv.style.width = '1px';
        this.oRectDiv.style.height = '1px';
        this.oRectDiv.style.visibility = 'hidden';
        this.oRectDiv.style.lineHeight = '1px'; //for IE
        this.oRectDiv.style.zIndex = 99;

        this._bIsActive = false;

        this.mouseMoveCB = this.mouseMove.bindAsEventListener(this);
        this.mouseUpCB = this.mouseUp.bindAsEventListener(this);
        this.mouseDownCB = this.mouseDown.bindAsEventListener(this);
        this.mouseOutCB = this.mouseOut.bindAsEventListener(this);
    },

    execute : function(l,b,r,t) {},

    activateRectTool : function() {
        //console.log('Fusion.Tool.Rectangle.activateRectTool');
        if (this._bIsActive) {
            //console.log('tool is already active');
            return;
        }
        if (this.oMap) {
            this.oRectDiv.style.left = '0px';
            this.oRectDiv.style.top = '0px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'hidden';
            this._bIsActive = true;
            var oDomElem =  this.oMap.getDomObj();
            oDomElem.appendChild(this.oRectDiv);
            this.oMap.observeEvent('mousemove', this.mouseMoveCB);
            this.oMap.observeEvent('mouseup', this.mouseUpCB);
            this.oMap.observeEvent('mousedown', this.mouseDownCB);
        }
    },

    deactivateRectTool : function() {
        this._sStartPos = null;
        //console.log('Fusion.Tool.Rectangle.deactivateRectTool');
        if (!this._bIsActive) {
            //console.log('tool is not active');
            return;
        }
        this._bIsActive = false;
        var oDomElem =  this.oMap.getDomObj();
        oDomElem.removeChild(this.oRectDiv);
        this.oMap.stopObserveEvent('mousemove', this.mouseMoveCB);
        this.oMap.stopObserveEvent('mouseup', this.mouseUpCB);
        this.oMap.stopObserveEvent('mousedown', this.mouseDownCB);
    },

    mouseDown : function (e) {
        if (OpenLayers.Event.isLeftClick(e)) {
            var p = this.oMap.getEventPosition(e);
            this._sStartPos = p;

            this.oRectDiv.style.left = p.x + 'px';
            this.oRectDiv.style.top = p.y + 'px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'visible';
            OpenLayers.Event.observe(document, 'mouseout', this.mouseOutCB);
        }
        OpenLayers.Event.stop(e);
    },

    mouseUp : function(e) {
        if (this._sStartPos != null) {
            var t = parseInt(this.oRectDiv.style.top);
            var l = parseInt(this.oRectDiv.style.left);
            var r = l + parseInt(this.oRectDiv.style.width);
            var b = t + parseInt(this.oRectDiv.style.height);
            this.event = e;
            this.execute(l,b,r,t);

            this.oRectDiv.style.left = '0px';
            this.oRectDiv.style.top = '0px';
            this.oRectDiv.style.width = '1px';
            this.oRectDiv.style.height = '1px';
            this.oRectDiv.style.visibility = 'hidden';

            this._sStartPos = null;
            OpenLayers.Event.stop(e);
        }
        OpenLayers.Event.stopObserving(document, 'mouseout', this.mouseOutCB);
    },

    mouseMove : function(e) {
        if (this._sStartPos != null)
        {
            var p = this.oMap.getEventPosition(e);
            var l = this._sStartPos.x;
            var t = this._sStartPos.y;
            var r = p.x;
            var b = p.y;

            this.oRectDiv.style.left = Math.min(l,r) + 'px';
            this.oRectDiv.style.top = Math.min(t,b) + 'px';
            this.oRectDiv.style.width = Math.abs(r-l) + 'px';
            this.oRectDiv.style.height = Math.abs(t-b) + 'px';
            OpenLayers.Event.stop(e);
        }
    },
    
    mouseOut: function(e) {
        var target = e.target || e.srcElement;
        if (target.tagName.toLowerCase() == 'html') {
            this.mouseUp(e);
        }
    }
        
};
/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Search.js 718 2007-08-21 17:22:25Z madair $
 * @purpose core search widget architecture
 * @author pspencer@dmsolutions.ca
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
 ********************************************************************
 *
 * The Fusion search mechanism defines a common capability for searches that
 * link individual searches to a common search set maintained for all
 * search-based widgets.  The common search set can be statically and/or
 * dynamically created.  Static definition is done in the WebLayout.  Dynamic
 * creation is done by individual widgets and creating instances of
 * Fusion.Tool.SearchDefinition(s) - or rather a MGSearchDefinition.
 *
 * Widgets that want to take advantage of this behaviour can sub-class this
 * widget.  Just make sure to initialize properly!
 * 
 * **********************************************************************/

Fusion.Tool.Search = Class.create();

Fusion.Tool.Search.prototype = {
    lastSearch: null,
    lastResult: null,
    resultOffset: 0,
    initialize : function() {
        //console.log('Fusion.Tool.Search.initialize');
    },
    getProperties: function() {
        var properties = null;
        if (this.lastResult && this.lastResult.properties) {
            properties = this.lastResult.properties;
        }
        return properties;
    },
    getNumberOfProperties: function() {
        var n = 0;
        if (this.lastResult && this.lastResult.properties) {
            n = this.lastResult.properties.length;
        }
        return n;
    },
    getProperty: function(n) {
        var property = '';
        if (this.lastResult && this.lastResult.properties) {
            property = this.lastResult.properties[n];
        }
        return property;
    },
    getNumberOfResults: function() {
        result = 0;
        if (this.lastResult && this.lastResult.values) {
            result = this.lastResult.values.length;
        }
        return result;
    },
    getFirstResult: function() {
        this.resultOffset = 0;
        return this.getResult(this.resultOffset);
    },
    getNextResult: function() {
        this.resultOffset++;
        return this.getResult(this.resultOffset);
    },
    getResult: function(idx) {
        result = null;
        if (this.lastResult && this.lastResult.values) {
            result = this.lastResult.values[idx];
        }
        return result;
    },
    zoomToResult: function(filter) {
        //console.log('zoomTo ' + filter);
        var filter = '&filter='+filter;
        
        var s = this.getMap().arch + '/' + Fusion.getScriptLanguage() + "/Query." + Fusion.getScriptLanguage() ;
        var params = {};
        params.parameters = 'session='+this.getMap().getSessionID()+'&mapname='+ this.getMap().getMapName()+
                         '&layers='+this.layerName+filter, 
        params.onComplete = this.selectComplete.bind(this);
        Fusion.ajaxRequest(s, params);
    },
    selectComplete: function(r) {
        var node = new DomNode(r.responseXML);
        var success = node.getNodeText('Selection');
        if (success == 'true') {
            this.getMap().newSelection();
            this.getMap().getSelection(this.zoomToSelection.bind(this));
        }    
    },
    /**
     * set the extents of the map based on the pixel coordinates
     * passed
     * 
     * @param selection the active selection, or null if there is none
     */
    zoomToSelection : function(selection) {
        var ll = selection.getLowerLeftCoord();
        var ur = selection.getUpperRightCoord();
        //buffer extents (zoom out by factor of two)
        var dX = ur.x - ll.x;
        var dY = ur.y - ll.y;
        ll.x = ll.x - dX;
        ur.x = ur.x + dX;
        ll.y = ll.y - dY;
        ur.y = ur.y + dY;
        this.getMap().setExtents(new OpenLayers.Bounds(ll.x,ll.y,ur.x,ur.y));
    }
};

/********************************************************************** * 
 * @project Fusion
 * @revision $Id: Map.js 870 2007-10-09 20:13:24Z madair $
 * @purpose Generic Map widget
 * @author yassefa@dmsolutions.ca
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
 ********************************************************************
 *
 * Fusion.Widget.Map : generic class for map widgets. Provides common utility classes.
 * **********************************************************************/

Fusion.Event.MAP_EXTENTS_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_BUSY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_GENERIC_EVENT = Fusion.Event.lastEventId++;
Fusion.Event.MAP_RESIZED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SELECTION_ON = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SELECTION_OFF = Fusion.Event.lastEventId++;
Fusion.Event.MAP_ACTIVE_LAYER_CHANGED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_LOADED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_LOADING = Fusion.Event.lastEventId++;
Fusion.Event.MAP_RELOADED = Fusion.Event.lastEventId++;
Fusion.Event.MAP_SESSION_CREATED = Fusion.Event.lastEventId++;


Fusion.Constant.LAYER_POINT_TYPE = 0;
Fusion.Constant.LAYER_LINE_TYPE = 1;
Fusion.Constant.LAYER_POLYGON_TYPE = 2;
Fusion.Constant.LAYER_SOLID_TYPE = 3;
Fusion.Constant.LAYER_RASTER_TYPE = 4;

Fusion.Widget.Map = Class.create();
Fusion.Widget.Map.prototype =
{

    _oDomObj : null,
    _sDomObj : '',
    _sMapname : '',  
    _nWidth : -1,
    _nHeight : -1,  
    _fMetersperunit : -1,
    _fScale : -1,
    _nDpi : 96,
    _oCurrentExtents: null,
    _oInitialExtents: null,
    _nWorkers: 0,
    oContextMenu: null,
    bSupressContextMenu: false,
    
    aMaps: null,
    layerRoot: null,
    
    /**
     * construct a new view Fusion.Widget.Map class.  
     */
    initialize : function(name, mapGroup) {    
        //console.log('Fusion.Widget.Map.initialize');

        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);

        this._nCellSize = -1;
        
        this._sDomObj = name;
        this._oDomObj = $(this._sDomObj);
        
        this.layerRoot = new Fusion.Widget.Map.Group();
        
        if (this._oDomObj.jxLayout) {
            this._oDomObj.jxLayout.addSizeChangeListener(this);
        }
        
        OpenLayers.DOTS_PER_INCH = this._nDpi;
        if (!this.oMapOL) this.oMapOL = new OpenLayers.Map(this._sDomObj, {controls: []} );
        this.oMapOL.viewPortDiv.style.position = 'absolute';  //not the top level container so set it to absolute

        //create the 'Map' layer widgets defined in the MapGroup
        this.aMaps = [];
        this.mapGroup = mapGroup;
        for (var i=0; i<mapGroup.maps.length; ++i) {
          var mapTag = mapGroup.maps[i];
          if (Fusion.Maps[mapTag.type]) {
              this.aMaps[i] = eval("new Fusion.Maps."+mapTag.type+"(this,mapTag)");
              this.layerRoot.addGroup(this.aMaps[i].layerRoot);
              
          } else {
              //TODO: we can add more OpenLayers layers ...
          }
        }
        $(name).widget = this;

        this.registerEventID(Fusion.Event.MAP_EXTENTS_CHANGED);
        this.registerEventID(Fusion.Event.MAP_BUSY_CHANGED);
        this.registerEventID(Fusion.Event.MAP_GENERIC_EVENT);
        this.registerEventID(Fusion.Event.MAP_RESIZED);
        this.registerEventID(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED);
        this.registerEventID(Fusion.Event.MAP_LOADED);
        this.registerEventID(Fusion.Event.MAP_LOADING);
        this.registerEventID(Fusion.Event.MAP_RELOADED);
        this.registerEventID(Fusion.Event.MAP_SELECTION_ON);
        this.registerEventID(Fusion.Event.MAP_SELECTION_OFF);
        
        this._oDomObj.oncontextmenu = function() {return false;};
        OpenLayers.Event.observe(this._oDomObj, 'contextmenu', this.onContextMenu.bind(this));
        
        this.aSelectionCallbacks = [];
        this.bFetchingSelection = false;
        
    },
    
    onMouseWheel: function(e) {   /* not neede in OL version? */
        var wheelDelta = e.wheelDelta ? e.wheelDelta : e.detail*-1;
        var wheelSet = null;
        
        var factor = this.nFactor;
        var size = Element.getDimensions(this._oImg);

        /* if the mouse hasn't moved yet, zoom on center */
        if (!this.lastMousePos) {
            this.lastMousePos = {x:size.width/2,y:size.height/2};
        }
        
        /* always work from the current image top/left in case the user has
         * more than one zoom before the new image arrives.
         */
        var top = parseInt(this._oImg.style.top);
        var left = parseInt(this._oImg.style.left);
        
        /* image location and dimensions for a temporarily resized version of the
         * current image while we wait for the new image to arrive
         */
        var newLeft, newTop, newWidth, newHeight;
        var newCenterX, newCenterTop;
        
        /* the direction we are zooming - 1 for in, -1 for out */
        var direction = 1;
        
        /* calculate the new image dimensions and zoom factor */
        if (wheelDelta > 0) {
            /* mouse position relative to top left of img */
            var x = this.lastMousePos.x - left;
            var y = this.lastMousePos.y - top;

            /* center the image on the mouse position */
            newLeft = left - x;
            newTop = top - y;
            
            /* increase size of image */
            newWidth = size.width * factor;
            newHeight = size.height * factor;
            
        } else {
            /* reduce size of image */
            newWidth = size.width / factor;
            newHeight = size.height / factor;

            /* mouse position relative to top left of img */
            var x = (this.lastMousePos.x - left)/factor;
            var y = (this.lastMousePos.y - top)/factor;
            
            /* center the image on the mouse position */
            newLeft = left + x;
            newTop = top + y;
            
            /* reverse factor for geographic zoom */
            direction = -1;
        }

        /* move/size the image */
        this._oImg.style.width = newWidth + "px";
        this._oImg.style.height = newHeight + "px";
        this._oImg.style.top = newTop + 'px';
        this._oImg.style.left = newLeft + 'px';
        
        /* figure out what geographic point will be at the new center.
         * Essentially, the geographic location of the mouse has to stay
         * in the same pixel location relative to the top/left.
         */
        var geoPoint = {};
        if (this.lastMousePos) {
            /* multiplier as a ratio of the current width to new width */
            var geoFactor = (size.width / newWidth);
            
            /* current geographic size */
            var curGW = this._afCurrentExtents[2] - this._afCurrentExtents[0];
            var curGH = this._afCurrentExtents[3] - this._afCurrentExtents[1];
            
            /* new geographic size is just a factor of the current one */
            var newGW = curGW * geoFactor;
            var newGH = curGH * geoFactor;
            
            /* geographic location of the mouse */
            var mouseLoc = this.pixToGeo(this.lastMousePos.x, this.lastMousePos.y);
            
            /* new geographic left/top is calculated from current mouse location and
             * taking the geographic distance to the top left in the current view and
             * using the geoFactor to figure out how far (geographically) it will be
             * in the new view
             */
            var newGL = mouseLoc.x - (mouseLoc.x - this._afCurrentExtents[0])*geoFactor;
            var newGT = mouseLoc.y + (this._afCurrentExtents[3] - mouseLoc.y)*geoFactor;

            /* now find the center so we can zoom */
            geoPoint.x = (newGL + newGW/2);
            geoPoint.y = (newGT - newGH/2);
        } else {
            geoPoint = this.getCurrentCenter();
        }
        
        /* finally we can zoom */
        this.zoom(geoPoint.x, geoPoint.y, direction*factor);
    },
    
    /**
     * returns the dom element 
     */
    getDomObj : function() {
        return this._oDomObj;
    },


    getMapName : function() {  
        //TODO: what is the mapname in the case of multiple map layer objects?
        //just return baselayer mapname for now
        //return this._sMapname;
        return this.aMaps[0].getMapName();
    },

    getDomId : function() {  
        return this._sDomObj;
    },

    setMapOptions: function(options) {
        this.oMapOL.setOptions(options);
    },

    addMap: function(map) {
        if (!map.singleTile) {
            this.singleTile = false;
        }
        this.oMapOL.addLayer(map.oLayerOL);
        map.registerForEvent(Fusion.Event.MAP_SELECTION_OFF, this.selectionHandler.bind(this));
        map.registerForEvent(Fusion.Event.MAP_SELECTION_ON, this.selectionHandler.bind(this));
    },

    getAllMaps: function() {
        return this.aMaps;
    },
    
    /**
     * Function: query
     *
     * dispatch query requests to maps
     */
    query: function(options) {
        for (var i=0; i<this.aMaps.length; i++ ) {
            if (this.aMaps[i].query(options)) {
            }
        }
    },
    
    /**
     * Function: selectionHandler
     *
     * handle selection events from maps and republish for
     * widgets as appropriate
     */
    selectionHandler: function() {
        if (this.hasSelection()) {
            this.triggerEvent(Fusion.Event.MAP_SELECTION_ON);
        } else {
            this.triggerEvent(Fusion.Event.MAP_SELECTION_OFF);
        }
    },
    
    /**
     * Function: hasSelection
     *
     * returns true if any map has a selection
     */
     hasSelection: function() {
         for (var i=0; i<this.aMaps.length; i++ ) {
             if (this.aMaps[i].hasSelection()) {
                 return true;
             }
         }
         return false;
     },
     
     /**
      * Function: clearSelection
      *
      * clear the selection on all maps
      */
     clearSelection: function() {
         this.oSelection = null;
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].clearSelection();
         }
     },
     
     /**
      * Function: getSelection
      *
      * returns the current selection asynchronously in case we
      * need to retrieve the details from the server
      */
     getSelection: function(callback) {
         this.aSelectionCallbacks.push(callback);
         if (this.bFetchingSelection) {
             return;
         }
         this.bFetchingSelection = true;
         this.oSelection = {};
         this.nSelectionMaps = 0;
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.nSelectionMaps++;
             this.aMaps[i].getSelection(this.accumulateSelection.bind(this, this.aMaps[i]));
         }
     },

     /**
      * Function: setSelectionXML
      *
      * sets a Selection XML back to the server
      */
      setSelection: function(selText, requery, zoomTo) {
         for (var i=0; i<this.aMaps.length; i++ ) {
             this.aMaps[i].setSelection(selText, requery, zoomTo);
         }
      },

     /**
      * Function: accumulateSelection
      *
      * accumulate the selection results from each map and when all have
      * reported in, pass the results to the callback function
      */
     accumulateSelection: function(map, oSelection) {
         this.oSelection[map._sMapname] = oSelection;
         
         if (!--this.nSelectionMaps) {
             this.bFetchingSelection = false;
             for (var i=0; i<this.aSelectionCallbacks.length; i++) {
                 this.aSelectionCallbacks[i](this.oSelection);
             }
             this.aSelectionCallbacks = [];
         }
     },

     /**
      * Function: setActiveLayer
      *
      * sets the active layer for selection/manipulation
      */
    setActiveLayer: function( oLayer ) {
        this.oActiveLayer = oLayer;
        this.oActiveMap = oLayer.map;
        this.triggerEvent(Fusion.Event.MAP_ACTIVE_LAYER_CHANGED, oLayer);
    },

     /**
      * Function: getActiveLayer
      *
      * returns the active layer for selection/manipulation
      */
    getActiveLayer: function() {
        return this.oActiveLayer;
    },

    /**
     * indicate that a new asynchronous process has started and make sure the
     * visual indicator is visible for the user.  This is intended to be used
     * internally by gMap but could be used by external tools if appropriate.
     */
    _addWorker : function() {
        this._nWorkers += 1;
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
    },

    /**
     * indicate that an asynchronous process has completed and hide the
     * visual indicator if no remaining processes are active.  This is 
     * intended to be used internally by gMap but could be used by 
     * external tools if appropriate.  Only call this function if
     * addWorker was previously called
     */
    _removeWorker : function() {
        if (this._nWorkers > 0) {
            this._nWorkers -= 1;
        }
        this.triggerEvent(Fusion.Event.MAP_BUSY_CHANGED, this);
    },
    
    isBusy: function() {
        return this._nWorkers > 0;
    },

    sizeChanged: function() {
        this.resize();
    },
    
    resize : function() {
      //console.log('Fusion.Widget.Map.resize');
        this.oMapOL.updateSize();
        var d = Element.getDimensions(this.getDomObj());
        this._nWidth = d.width;
        this._nHeight = d.height;
        if (this._oCurrentExtents) {
          this.setExtents(this._oCurrentExtents);
        } else if (this._oInitialExtents) {
          this.setExtents(this._oInitialExtents);
        }
        this.triggerEvent(Fusion.Event.MAP_RESIZED, this);
    },
    
    redraw: function() {
      for (var i=0; i<this.aMaps.length; i++ ) {
        this.aMaps[i].oLayerOL.mergeNewParams({ts : (new Date()).getTime()});
      }
      this.oMapOL.setCenter(this.oMapOL.getCenter(), this.oMapOL.getZoom(), false, true);
    },
    
    setExtents : function(oExtents) {
        if (!oExtents) {
            Fusion.reportError(new Fusion.Error(Fusion.Error.WARNING, 'Map.setExtents called with null extents'));
        }
        if (oExtents instanceof Array && oExtents.length == 4) {
            oExtents = new OpenLayers.Bounds(oExtents[0], oExtents[1], oExtents[2], oExtents[3]);
        }
        if (this.aMaps[0].bSingleTile) {
            var viewSize = this.oMapOL.getSize();
            var idealResolution = Math.max( oExtents.getWidth()  / viewSize.w,
                                            oExtents.getHeight() / viewSize.h );
            
            this.oMapOL.baseLayer.resolutions = [idealResolution];
            this.oMapOL.zoom = 1;
        }
        this.oMapOL.zoomToExtent(oExtents);
        this._oCurrentExtents = this.oMapOL.getExtent();
        this.triggerEvent(Fusion.Event.MAP_EXTENTS_CHANGED);
    },

    fullExtents : function() {
      //determine the initialExtents
      if (!this._oInitialExtents) {
        var bbox = Fusion.getQueryParam("bbox");   //set as min x,y, max x,y
        if (bbox) {
          this._oInitialExtents = new OpenLayers.Bounds.fromArray(bbox.split(","));
        } else if (this.mapGroup.initialView) {
          this._oInitialExtents = this.getExtentFromPoint(this.mapGroup.initialView.x, this.mapGroup.initialView.y, this.mapGroup.initialView.scale);
        } else {
          this._oInitialExtents = this.oMapOL.getMaxExtent();
        }
      }
      this.setExtents(this._oInitialExtents); 
    },

    isMapLoaded: function() {
        return (this._oCurrentExtents) ? true : false;
    },

    zoom : function(fX, fY, nFactor) {
        //do this differntly with OL code??
        var extent = this.oMapOL.getExtent();
        var fDeltaX = extent.right - extent.left;
        var fDeltaY = extent.top - extent.bottom;
        var fMinX,fMaxX,fMinY,fMaxy;

        if (nFactor == 1 || nFactor == 0) {
            /*recenter*/
            fMinX = fX - (fDeltaX/2);
            fMaxX = fX + (fDeltaX/2);
            fMinY = fY - (fDeltaY/2);
            fMaxY = fY + (fDeltaY/2);
        } else if (nFactor > 0) {
            /*zoomin*/
            fMinX = fX - (fDeltaX/2 / nFactor);
            fMaxX = fX + (fDeltaX/2 / nFactor);
            fMinY = fY - (fDeltaY/2 / nFactor);
            fMaxY = fY + (fDeltaY/2 / nFactor);
        } else if (nFactor < 0) {
            /*zoomout*/
            fMinX = fX - ((fDeltaX/2) * Math.abs(nFactor));
            fMaxX = fX + ((fDeltaX/2) * Math.abs(nFactor));
            fMinY = fY - ((fDeltaY/2) * Math.abs(nFactor));
            fMaxY = fY + ((fDeltaY/2) * Math.abs(nFactor));
        }
        this.setExtents(new OpenLayers.Bounds(fMinX, fMinY, fMaxX, fMaxY));
    },
    
    zoomToScale: function(fScale) {
        var center = this.getCurrentCenter();
        var extent = this.getExtentFromPoint(center.x, center.y, fScale);
        this.setExtents(extent);
    },
    
    queryRect : function(fMinX, fMinY, fMaxX, fMaxY) { },
    
    queryPoint : function(fX, fY) { },
    
    /**
     *
     * convert pixel coordinates into geographic coordinates.
     *
     * @paran pX int the x coordinate in pixel units
     * @param pY int the y coordinate in pixel units
     *
     * @return an object with geographic coordinates in x and y properties of the 
     *         object.
     */
    pixToGeo : function( pX, pY ) {
        var lonLat = this.oMapOL.getLonLatFromPixel( new OpenLayers.Pixel(pX,pY) );
        return {x:lonLat.lon, y:lonLat.lat};
    },

    /**
     *
     * convert geographic coordinates into pixel coordinates.
     *
     * @paran gX int the x coordinate in geographic units
     * @param gY int the y coordinate in geographic units
     *
     * @return an object with pixel coordinates in x and y properties of the 
     *         object.
     */
    geoToPix : function( gX, gY ) {
        if (!(this._oCurrentExtents)) {
            return null;
        }
        var px = this.oMapOL.getPixelFromLonLat(new OpenLayers.LonLat(gX,gY));  //use getViewPortPxFromLonLat instead?
        return {x:Math.floor(px.x), y:Math.floor(px.y)};
    },

    /**
     *
     * convert pixel into geographic : used to measure.
     *
     * @param nPixels int measures in pixel
     *
     * @return geographic measure
     */
    pixToGeoMeasure : function(nPixels) {
        var resolution = this.oMapOL.getResolution();
        return (nPixels*resolution);
    },
    
    /**
     *
     * convert geographic into pixels.
     *
     * @param fGeo float distance in geographic units
     *
     * @return pixels
     */
    geoToPixMeasure : function(fGeo) {
        return parseInt(fGeo/this.oMapOL.getResolution());
    },
    
    /**
     * Function: getCurrentCenter
     *
     * returns the current center of the map view
     *
     * Return: {Object} an object with the following attributes
     * x - the x coordinate of the center
     * y - the y coordinate of the center
     */
    getCurrentCenter : function() {
        var c = this._oCurrentExtents.getCenterLonLat();
        return {x:c.lon, y:c.lat};
    },

    /**
     *
     * returns the current extents
     */
    getCurrentExtents : function() {
        return this.oMapOL.getExtent();
    },

    /**
     *
     * returns initial extents
    */
    getInitialExtents : function(){
        return this._oInitialExtents;
    },

    /**
     * Function: getExtentFromPoint
     *
     * returns the Extent of the map given a center point and a scale (optional)
     *
     * Return: {OpenLayers.Bounds} the bounds for the map centered on a point
     */
    getExtentFromPoint: function(fX,fY,fScale) {
        if (!fScale) fScale = this.getScale();
        var res = OpenLayers.Util.getResolutionFromScale(fScale, this.oMapOL.baseLayer.units);
        var size = this.getSize();
        var w_deg = size.w * res;
        var h_deg = size.h * res;
        return new OpenLayers.Bounds(fX - w_deg / 2,
                                           fY - h_deg / 2,
                                           fX + w_deg / 2,
                                           fY + h_deg / 2);
    },
    
    getScale : function() {
        return this.oMapOL.getScale();
    },
    
    getUnits : function() {
        return this.oMapOL.baseLayer.units;
    },
    
    getSize: function() {
        return this.oMapOL.getSize();
    },

    getEventPosition : function(e) {
        return this.oMapOL.events.getMousePosition(e);
    },

    setCursor : function(cursor) {
        if (cursor && cursor.length && typeof cursor == 'object') {
            for (var i = 0; i < cursor.length; i++) {
                this._oDomObj.style.cursor = cursor[i];
                if (this._oDomObj.style.cursor == cursor[i]) {
                    break;
                }
            }
        } else if (typeof cursor == 'string') {
            this._oDomObj.style.cursor = cursor;
        } else {
            this._oDomObj.style.cursor = 'auto';  
        }
    },
    /**
     *
     * Observe specified event on the event div of the map
     *
     * @param sEventName string event name (eg : mousemove')
     * @param fnCB function Call back function name
     *
     */
     observeEvent  : function(sEventName, fnCB)
     {
         OpenLayers.Event.observe(this._oDomObj, sEventName, fnCB, false);
     },

     /**
     *
     * Stop observing specified event on the event div of the map
     *
     * @param sEventName string event name (eg : mousemove')
     * @param fnCB function Call back function name
     *
     */
     stopObserveEvent : function(sEventName, fnCB)
     {
         OpenLayers.Event.stopObserving(this._oDomObj, sEventName, fnCB, false);
     },

     /**
     *
     * call the Activate method on the widget
     * if widgets is set to be mutually exclusive,
     * all other widgets are deactivated
     *
     * @param nId integer widget id
     */
     activateWidget : function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.activateWidget ' + oWidget.getName());*/
         if (oWidget.isMutEx()) {
             if (this.oActiveWidget) {
                 this.deactivateWidget(this.oActiveWidget);
             }
             oWidget.activate();
             this.oActiveWidget = oWidget;
         } else {
             oWidget.activate();
         }
     },

     /**
     *
     * call the Activate method on the widget
     * if widgets is set to be mutually exclusive,
     * all other widgets are deactivated
     *
     * @param oWidget the widget to deactivate
     */
     deactivateWidget : function(oWidget)
     {
         /*console.log('Fusion.Widget.Map.deactivateWidget ' + oWidget.getName());*/
         oWidget.deactivate();
         this.oActiveWidget = null;
     },
     
     /**
      */
     isLoaded: function() {
         return (this.oMapOL.getExtent() != null);
     },
     
     supressContextMenu: function( bSupress ) {
         this.bSupressContextMenu = bSupress;
     },
     
     setContextMenu: function(menu) {
         //console.log('setcontextmenu');
         this.oContextMenu = menu;
     },
     
     onContextMenu: function(e) {
         //console.log('oncontextmenu');
         if (this.oContextMenu && !this.bSupressContextMenu && this.isLoaded()) {
             this.oContextMenu.show(e);
             this.contextMenuPosition = this.getEventPosition(e);
             OpenLayers.Event.stop(e);
         }
     },
     
     executeFromContextMenu: function(widget) {
         //console.log('executefromcontextmenu');
         widget.execute(this.contextMenuPosition.x, this.contextMenuPosition.y);
     }
};

Fusion.Event.LAYER_PROPERTY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Widget.Map.Layer = Class.create();
Fusion.Widget.Map.Layer.prototype = {
    name: null,
    initialize: function(name) {
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.name = name;
        this.registerEventID(Fusion.Event.LAYER_PROPERTY_CHANGED);
    },
    clear: function() {},
    set: function(property, value) {
        this[property] = value;
        this.triggerEvent(Fusion.Event.LAYER_PROPERTY_CHANGED, this);
    }
};

Fusion.Event.GROUP_PROPERTY_CHANGED = Fusion.Event.lastEventId++;
Fusion.Widget.Map.Group = Class.create();
Fusion.Widget.Map.Group.prototype = {
    name: null,
    groups: null,
    layers: null,
    initialize: function(name) {
        Object.inheritFrom(this, Fusion.Lib.EventMgr, []);
        this.name = name;
        this.groups = [];
        this.layers = [];
        this.registerEventID(Fusion.Event.GROUP_PROPERTY_CHANGED);
    },
    clear: function() {
        for (var i=0; i<this.groups.length; i++) {
            this.groups[i].clear();
        }
        for (var i=0; i<this.layers.length; i++) {
            this.layers[i].clear();
        }
        this.groups = [];
        this.layers = [];
    },
    set: function(property, value) {
        this[property] = value;
        this.triggerEvent(Fusion.Event.GROUP_PROPERTY_CHANGED, this);
    },
    addGroup: function(group) {
        group.parentGroup = this;
        this.groups.push(group);
        
    },
    addLayer: function(layer) {
        layer.parentGroup = this;
        this.layers.push(layer);
    },
    findGroup: function(name) {
        return this.findGroupByAttribute('name', name);
    },
    findGroupByAttribute: function(attribute, value) {
        if (this[attribute] == value) {
            return this;
        }
        for (var i=0; i<this.groups.length; i++) {
            var group = this.groups[i].findGroupByAttribute(attribute, value);
            if (group) {
                return group;
            }
        }
        return null;
    },
    findLayer: function(name) {
        return this.findLayerByAttribute('name', name);
    },
    findLayerByAttribute: function(attribute, value) {
        for (var i=0; i<this.layers.length; i++) {
            if (this.layers[i][attribute] == value) {
                return this.layers[i];
            }
        }
        for (var i=0; i<this.groups.length; i++) {
            var layer = this.groups[i].findLayerByAttribute(attribute,value);
            if (layer) {
                return layer;
            }
        }
        return null;
    }
};


/**
 * SelectionObject
 *
 * Utility class to hold slection information
 *
 */
var GxSelectionObject = Class.create();
GxSelectionObject.prototype = 
{
    aLayers : null,

    initialize: function(oXML) 
    {
        this.aLayers = [];
        this.nTotalElements =0;

        var root = new DomNode(oXML);
        
        var node  = root.getNodeText('minx');
        if (node)
        {
            this.fMinX =  parseFloat(root.getNodeText('minx'));
            this.fMinY =  parseFloat(root.getNodeText('miny'));
            this.fMaxX =  parseFloat(root.getNodeText('maxx'));
            this.fMaxY =  parseFloat(root.getNodeText('maxy'));
        }
    
        //this is only available when the property mapping is set
        //on the layer. TODO : review
        var oTmpNode = root.findFirstNode('TotalElementsSelected');
        if (oTmpNode)
        {
            this.nTotalElements = parseInt(oTmpNode.textContent);
            if (this.nTotalElements > 0)
            {
                this.nLayers =  root.getNodeText('NumberOfLayers');
                var layerNode = root.findFirstNode('Layer');
                var iLayer=0;             
                while(layerNode) 
                {
                    this.aLayers[iLayer++] = new GxSelectionObjectLayer(layerNode);
                
                    layerNode =  root.findNextNode('Layer');
                }
            }
        }
    },

    getNumElements : function()
    {
        return this.nTotalElements;
    },

    getLowerLeftCoord : function()
    {
        return {x:this.fMinX, y:this.fMinY};
    },

    getUpperRightCoord : function()
    {
        return {x:this.fMaxX, y:this.fMaxY};
    },

    getNumLayers : function()
    {
        return this.nLayers;
    },
    
    getLayerByName : function(name)
    {
        var oLayer = null;
        for (var i=0; i<this.nLayers; i++)
        {
            if (this.aLayers[i].getName() == name)
            {
                oLayer = this.aLayers[i];
                break;
            }
        }
        return oLayer;
    },

    getLayer : function(iIndice)
    {
        if (iIndice >=0 && iIndice < this.nLayers)
        {
            return this.aLayers[iIndice];
        }
        else
        {
            return null;
        }
            
    }
};


var GxSelectionObjectLayer = Class.create();
GxSelectionObjectLayer.prototype = {

    sName: null,
    nElements: null,
    aElements: null,
    nProperties: null,
    aPropertiesName: null,
    aPropertiesTypes: null,

    type: null,
    area: null,
    distance: null,
    bbox: null,
    center: null,
    
    initialize: function(oNode) 
    {
        this.sName =  oNode.getNodeText('Name');
        this.nElements =  oNode.getNodeText('ElementsSelected');

        this.aElements = [];

        this.nProperties = oNode.getNodeText('PropertiesNumber');

        this.aPropertiesName = [];
        var oTmp = oNode.getNodeText('PropertiesNames');
        this.aPropertiesName = oTmp.split(",");

        this.aPropertiesTypes = [];
        oTmp = oNode.getNodeText('PropertiesTypes');
        this.aPropertiesTypes = oTmp.split(",");
        
        var oValueCollection = oNode.findNextNode('ValueCollection');
        
        this.area = 0;
        this.distance = 0;
        
        var iElement=0;
        while(oValueCollection) 
        {
            this.aElements[iElement] = [];
            for (var i=0; i<oValueCollection.childNodes.length; i++)
            {
                oTmp = oValueCollection.childNodes[i].findFirstNode('v');
                this.aElements[iElement][i] = oTmp.textContent;
                
            }
            var type = oValueCollection.attributes['type'];
            var area = oValueCollection.attributes['area'];
            var distance = oValueCollection.attributes['distance'];
            var bbox = oValueCollection.attributes['bbox'];
            var center = oValueCollection.attributes['center'];
            
            this.aElements[iElement]['attributes'] = {};
            this.aElements[iElement]['attributes'].type = type;
            this.aElements[iElement]['attributes'].bbox = bbox;
            this.aElements[iElement]['attributes'].center = bbox;
            //console.log('type is ' + type);
            if (type > 1) {
                this.area += parseFloat(area);
                this.aElements[iElement]['attributes'].area = area;
            }
            if (type > 0) {
                this.aElements[iElement]['attributes'].distance = distance;
                this.distance += parseFloat(distance);
            }
            oValueCollection = oNode.findNextNode('ValueCollection');
            iElement++;
        }
        //console.log( 'final area is ' + this.area);
        //console.log( 'final distance is ' + this.distance);
        
    },

    getName : function()
    {
        return this.sName;
    },

    getNumElements : function()
    {
        return this.nElements;
    },

    getNumProperties : function()
    {
        return this.nProperties;
    },

    getPropertyNames : function()
    {
        return this.aPropertiesName;
    },

    getPropertyTypes : function()
    {
        return this.aPropertiesTypes;
    },

    getElementValue : function(iElement, iProperty)
    {
        if (iElement >=0 && iElement < this.nElements &&
            iProperty >=0 && iProperty < this.nProperties)
        {
            return this.aElements[iElement][iProperty];
        }
        else
        {
            return null;
        }
    }
};
