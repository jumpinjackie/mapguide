/*****************************************************************************
 * $Id: WebLayout.js 669 2007-06-29 19:56:30Z pspencer $
 * Purpose: Fusion
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
 * WebLayout
 *
 * Utility class to parse a web layout
 *
 */
Fusion.Event.WEBLAYOUT_PARSED = Fusion.Event.lastEventId++;

Fusion.Lib.WebLayout = Class.create();
Object.extend(Fusion.Lib.WebLayout.prototype, Fusion.Lib.EventMgr);
Object.extend(Fusion.Lib.WebLayout.prototype, {   
    mapId : '',
    aMaps: null,
    commandObj : null,
    aToolbars: null,
    aMenus: null,
    webLayout : null,
    oBroker: null,
    oConfigManager: null,

    initialize: function(app) {   
        //console.log('WebLayout initialize');
        Fusion.Lib.EventMgr.initialize.apply(this, []);
        this.app = app;
        this.oBroker = app.getBroker();
        this.webLayout =  app.getWebLayout();
        
        this.commandObj = [];
        this.aMaps = [];
        this.registerEventID(Fusion.Event.WEBLAYOUT_PARSED);
    },

    parse : function() {
        /* if the web layout is not in the mapguide server, 
           just load the xml*/
        
        if (this.webLayout.match('Library://') == null) {
            var options = {};
            options.method = 'get';
            options.onSuccess = this.convertXML.bind(this);
            new Ajax.Request( this.webLayout, options);
        } else {
            var r = new Fusion.Lib.MGRequest.MGGetResourceContent(this.webLayout);
            this.oBroker.dispatchRequest(r, this.parseXML.bind(this));
        }
    },
    
    convertXML: function(r, json) {
        var options = {};
        options.onSuccess = this.parseXML.bind(this);
        options.parameters = 'xml='+encodeURIComponent(r.responseText.replace(/\\/g, '\\\\\\\\'))+'&ts='+((new Date()).getTime());
        var sl = Fusion.getScriptLanguage();
        Fusion.ajaxRequest('common/'+sl+'/Xml2JSON.'+sl, options);
    },
    
    parseXML: function(r, json) {
        if (json) {
            var mainNode;
            eval("mainNode="+r.responseText);
            
            var webLayout = mainNode.WebLayout;
            /* process Map nodes */
            
            if (webLayout.Map instanceof Array) {
                var maps = webLayout.Map;
                for (var i=0; i<maps.length; i++) {
                    var map = maps[i];
                    var mapObj = {};
                    mapObj.resourceId = map.ResourceId[0];
                    mapObj.referenceImageUrl = map.ReferenceImageUrl[0];
                    mapObj.links = {layers:[], groups: []};
                    mapObj.layerEvents = {};
                    if (map.Links) {
                        /* process Groups */
                        if (map.Links[0].Group instanceof Array) {
                            for (var j=0; j<map.Links[0].Group.length; j++) {
                                var group = map.Links[0].Group[j];
                                mapObj.links.groups.push({name:group.Name,url:group.Url});
                            }
                        }
                        if (map.Links[0].Layer instanceof Array) {
                            for (var j=0; j<map.Links[0].Layer.length; j++) {
                                var layer = map.Links[0].Layer[j];
                                mapObj.links.layers.push({name:layer.Name,url:layer.Url});
                            }
                        }
                    }
                    /* process layer events */
                    if (map.LayerEvents) {
                        if (map.LayerEvents[0].Layer instanceof Array) {
                            for (var j=0; j<map.LayerEvents[0].Layer.length; j++) {
                                var layer = map.LayerEvents[0].Layer[j];
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
                                mapObj.layerEvents[layerObj.name] = layerObj;
                            }
                        }
                    }
                    this.aMaps.push(mapObj);
                }
            }
            
            var commandSet = webLayout.CommandSet;
            if (commandSet instanceof Array) {
                var commands = commandSet[0].Command;
                if (commands instanceof Array) {
                    for (var i=0; i<commands.length; i++) {
                        this.commandObj.push(new Fusion.Lib.WebCommand(commands[i]));
                    }
                }
            }
            
            /* process toolbars */
            this.aToolbars = [];

            if (webLayout.ToolBar instanceof Array) {
                var toolbars = webLayout.ToolBar;
                for (var i=0; i<toolbars.length; i++) {
                    this.aToolbars.push( new Fusion.Lib.WebLayout.Toolbar(toolbars[i]) );
                }
            }
            
            /* process context menus */
            this.aMenus = [];

            if (webLayout.ContextMenu instanceof Array) {
                var menus = webLayout.ContextMenu;
                for (var i=0; i<menus.length; i++) {
                    this.aMenus.push( new Fusion.Lib.WebLayout.ConextMenu(menus[i]) );
                }
            }
            
            /* process search definitions */
            this.aSearchCategories = {};
            this.aSearchDefinitions = {};
            if (webLayout.SearchDefinitions instanceof Array) {
                var categories = webLayout.SearchDefinitions[0];
                if (categories.SearchCategory instanceof Array) {
                    for (var i=0; i<categories.SearchCategory.length; i++) {
                        var oCategory = {};
                        var category = categories.SearchCategory[i];
                        oCategory.id = category['@id'];
                        oCategory.name = category['@name'];
                        oCategory.layer = category.Layer ? category.Layer[0] : '';
                        oCategory.searchDefinitions = [];
                        var defns = category.SearchDefinition;
                        for (var k=0; k<defns.length; k++) {
                            var defn = new Fusion.Lib.WebLayout.SearchDefinition(defns[k]);
                            defn.category = oCategory;
                            oCategory.searchDefinitions[defn.id] = defn;
                            this.aSearchDefinitions[defn.id] = defn;
                        }
                    }
                }
            }
            this.triggerEvent(Fusion.Event.WEBLAYOUT_PARSED);
        }
    },
    
    getMapResourceId : function() {
        return this.mapId;
    },

    getCommandByName : function(sName) {
        var oCommand;
        for (var i=0; i<this.commandObj.length; i++) {
            oCommand = this.commandObj[i];

            if (oCommand.getName() == sName) {
                return this.commandObj[i];
            }
        }
    },

    getCommandByType : function(sType) {
        var oCommand;
        var aReturn = [];

        for (var i=0; i<this.commandObj.length; i++) {
            oCommand = this.commandObj[i];

            if (oCommand.getType() == sType) {
                aReturn.push(oCommand);
            }
        }

        return aReturn;
    }
});

Fusion.Lib.WebLayout.Toolbar = Class.create();
Fusion.Lib.WebLayout.Toolbar.prototype = {
    buttons: null,
    name: null,
    container: null,
    initialize: function(jsonNode) {
        this.name = jsonNode.Name ? jsonNode.Name[0] : '';
        this.container = jsonNode.Container ? jsonNode.Container[0] : '';
        this.buttons = [];
        if (jsonNode.Button instanceof Array) {
            for (var i=0; i<jsonNode.Button.length; i++) {
                this.buttons.push(new Fusion.Lib.WebLayout.UiItem(jsonNode.Button[i]));
            }
        }
    }
};

Fusion.Lib.WebLayout.ConextMenu = Class.create();
Fusion.Lib.WebLayout.ConextMenu.prototype = {
    items: null,
    mapName: null,
    initialize: function(jsonNode) {
        this.mapName = jsonNode.Map ? jsonNode.Map[0] : '';
        this.items = [];
        if (jsonNode.MenuItem instanceof Array) {
            for (var i=0; i<jsonNode.MenuItem.length; i++) {
                this.items.push(new Fusion.Lib.WebLayout.UiItem(jsonNode.MenuItem[i]));
            }
        }
    }
};

Fusion.Lib.WebLayout.UiItem = Class.create();
Fusion.Lib.WebLayout.UiItem.prototype = {
    func: null,
    obj: null,
    initialize: function(jsonNode) {
        this.func = jsonNode.Function[0];
        switch(this.func) {
            case 'Separator':
                break;
            case 'Command':
                this.obj = new Fusion.Lib.WebLayout.Command(jsonNode);
                break;
            case 'Flyout':
                this.obj = new Fusion.Lib.WebLayout.Flyout(jsonNode);
                break;
            default:
                /* TODO: this could be an exception? */
        }
    }
};

Fusion.Lib.WebLayout.Command = Class.create();
Fusion.Lib.WebLayout.Command.prototype = {
    name: null,
    initialize: function(jsonNode) {
        this.name = jsonNode.Command[0];
    }
};

Fusion.Lib.WebLayout.Flyout = Class.create();
Fusion.Lib.WebLayout.Flyout.prototype = {
    label: null,
    tooltip: null,
    description: null,
    imageUrl: null,
    disabledImageUrl: null,
    /* TODO add active class, disabled class */
    subItems: null,
    
    initialize: function(jsonNode) {
        this.label = jsonNode.Label ? jsonNode.Label[0] : '';
        this.tooltip = jsonNode.Tooltip ? jsonNode.Tooltip[0] : '';
        this.description = jsonNode.Description ? jsonNode.Description[0] : '';
        this.imageUrl = jsonNode.ImageURL ? jsonNode.ImageURL[0] : '';
        this.disabledImageUrl = jsonNode.DisabledImageURL ? jsonNode.DisabledImageURL[0] : '';
        this.subItems = [];
        if (jsonNode.SubItem instanceof Array) {
            for (var i=0; i<jsonNode.SubItem.length; i++) {
                this.subItems.push(new Fusion.Lib.WebLayout.UiItem(jsonNode.SubItem[i]));
            }
        }
    }
};

Fusion.Lib.WebLayout.SearchDefinition = Class.create();
Fusion.Lib.WebLayout.SearchDefinition.prototype = {
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
            this.join = new Fusion.Lib.WebLayout.SearchJoin(json.Join[0]);
        }
        this.parameters = [];
        if (json.Parameter instanceof Array) {
            for (var i=0; i<json.Parameter.length; i++) {
                this.parameters.push(json.Parameter[i]['@name']);
            }
        }
        var rule;
        if (json.SearchAnd instanceof Array) {
            this.rule = new Fusion.Lib.WebLayout.SearchRule('AND');
            rule = json.SearchAnd[0];
        } else if (json.SearchOr instanceof Array) {
            this.rule = new Fusion.Lib.WebLayout.SearchRule('OR');
            rule = json.SearchOr[0];
        }
        if (rule && rule.SearchCondition instanceof Array) {
            for (var i=0; i<rule.SearchCondition.length; i++) {
                this.rule.add(new Fusion.Lib.WebLayout.SearchCondition(rule.SearchCondition[i]));
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

Fusion.Lib.WebLayout.SearchJoin = Class.create();
Fusion.Lib.WebLayout.SearchJoin.prototype = {
    layer: null,
    primaryKey: null,
    foreignKey: null,
    initialize: function(json) {
        this.layer = json.Layer ? json.Layer[0] : '';
        this.primaryKey = json.PrimaryKey ? json.PrimaryKey[0] : '';
        this.foreignKey = json.ForeignKey ? json.ForeignKey[0] : '';
    }
};

Fusion.Lib.WebLayout.SearchRule = Class.create();
Fusion.Lib.WebLayout.SearchRule.prototype = {
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

Fusion.Lib.WebLayout.SearchCondition = Class.create();
Fusion.Lib.WebLayout.SearchCondition.prototype = {
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