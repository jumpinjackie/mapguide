/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: WSDialog.js,v 1.37 2006/06/14 17:30:25 pspencer Exp $
 * @fileoverview    this file contains dialog classes for Web Studio
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
 * WSFileDialog is a general purpose repository browsing dialog intended to
 * be re-used for opening, saving and choosing resources from the repository.
 *
 * To launch a dialog, call one of:
 *
 * openResource
 * chooseResource
 * openSaveAs
 */ 
var WSFileDialog = Class.create();

Object.extend(WSFileDialog.prototype, {
    dialog: null,
    currentFolder: null,
    handler: null,
    app: null,
    currentResource: null,
    allTypes: [{value:"", label: 'All resources'},
               {value:"WebLayout", label: 'Web layout'},
               {value:"MapDefinition", label: 'Map'},
               {value:"LayerDefinition", label: 'Layer'},
               {value:"FeatureSource", label: 'Feature source'},
               {value:"DrawingSource", label: 'Drawing source'},
               {value:"SymbolLibrary", label: 'Symbol library'},
               {value:"PrintLayout", label: 'Print layout'},
               {value:"LoadProcedure", label: 'Load procedure'}],
    /**
     * initialize a new instance of the file dialog
     * 
     * @param options {Object} an anonymous object which can contain
     *        the following attributes:
     *        app - a WSApplication instance (required);
     *        onContentLoaded - a function to invoke when the content
     *              of the dialog is first loaded.
     */
    initialize: function( options ) {
        this.app = options.app;
        this.onContentLoadedCallback = options.onContentLoaded || null;
        var size = Position.getPageDimensions();
        var o = {
            title: 'File Dialog',
            id: 'filedialog',
            contentURL : 'dialogs/browseresource.html',
            helpURL : 'help/dialog_browseresource.html',
            width: 350,
            height: 400,
            top: (size.height-450)/2,
            left: (size.width-400)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.dialog = new JxDialog( o ); 
    },
    /**
     * handle the content of the interface being loaded by registering
     * various ids we need for populating the interface with stuff
     * from the repository
     */
    onContentLoaded: function() {
        this.dialog.registerIds(['dialog.file.folder',
                                 'dialog.file.up',
                                 'dialog.file.list',
                                 'dialog.file.name',
                                 'dialog.file.type'], this.dialog.domObj);
        var up = this.dialog.getObj('dialog.file.up');
        up.onclick = this.browseUp.bind(this);
        var folder = this.dialog.getObj('dialog.file.folder');
        folder.onchange = this.browseToFolder.bind(this);
        var rtype = this.dialog.getObj('dialog.file.type');
        rtype.onchange = this.redrawPath.bind(this, null);
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
    },
    /**
     * return a filter from the valid filters based on a resource type
     *
     * @param type {String} a resource type
     * @return {Object} a valid filter object for the requested type.
     */
    getFilter: function(type) {
        return this.allTypes.findAll(function(type,value,index){ return value.value == type;}.bind(this,type));
    },
    /**
     * open the file dialog for selecting an existing resource.  The
     * user can change the resource type filter, but they cannot enter
     * a resource name directly.
     *
     * @param options {Object} an anonymous object that contains various
     *        attributes as follows:
     *        path - the starting location for the dialog
     *        types - an array of resource types that are valid to choose
     *                from
     *        resourcename - an initially selected resource name
     *        handler - a function to call when the user clicks an action
     *                  button
     */
    openResource: function(options) {
        options = Object.extend({}, options);
        this.currentFolder = new WSResourceId(options.path ? options.path : 'Library://');
        this.handler = options.handler || null;
        this.setResourceTypes(options.types || this.allTypes);
        this.dialog.setTitle('Open Resource');
        this.dialog.getObj('dialog.file.name').value = options.resourcename || '';
        this.dialog.getObj('dialog.file.name').disabled = true;
        this.dialog.getObj('dialog.file.type').disabled = false;
        this.dialog.setButtons(['Open','Cancel']);
        this.dialog.open();
    },
    /**
     * open the file dialog for choosing a resource of a specific type.
     *
     * @param options {Object} an anonymous object that contains various
     *        attributes as follows:
     *        path - the starting location for the dialog
     *        types - an array of resource types that are valid to choose
     *                from
     *        resourcename - an initially selected resource name
     *        handler - a function to call when the user clicks an action
     *                  button
     */
    chooseResource: function(options) {
        options = Object.extend({}, options);
        this.currentFolder = new WSResourceId(options.path ? options.path : 'Library://');
        this.handler = options.handler || null;
        this.setResourceTypes(options.types || this.allTypes);
        this.dialog.setTitle('Select Resource');
        this.dialog.getObj('dialog.file.name').value = options.resourcename || '';
        this.dialog.getObj('dialog.file.name').disabled = true;
        this.dialog.getObj('dialog.file.type').disabled = true;
        this.dialog.setButtons(['Select','Cancel']);
        this.dialog.open();
    },
    /**
     * open the file dialog for saving a resource under a new name.  The
     * user can change the resource type filter, and enter a new name for
     * the resource.
     *
     * @param options {Object} an anonymous object that contains various
     *        attributes as follows:
     *        path - the starting location for the dialog
     *        types - an array of resource types that are valid to choose
     *                from
     *        resourcename - an initially selected resource name
     *        handler - a function to call when the user clicks an action
     *                  button
     */
    openSaveAs: function(options) {
        options = Object.extend({}, options);
        this.currentFolder = new WSResourceId(options.path ? options.path : 'Library://');
        this.handler = options.handler || null;
        this.setResourceTypes(options.types || this.allTypes);
        this.dialog.setTitle('Save Resource As');
        this.dialog.getObj('dialog.file.name').value = options.resourcename || '';
        this.dialog.getObj('dialog.file.name').disabled = false;
        this.dialog.getObj('dialog.file.type').disabled = true;
        this.dialog.setButtons(['Save','Cancel']);
        this.dialog.open();
    },
    /**
     * change the list of resources by which the repository is filtered
     *
     * @param a {Array} an array of filter objects
     */
    setResourceTypes: function(a) {
        var s = this.dialog.getObj('dialog.file.type');
        while(s.options.length > 0) { s.options[0] = null; }
        for (var i=0; i<a.length; i++) {
            s.options[i] = new Option(a[i].label, a[i].value);
        }
    },
    /**
     * return the resource that the user selected.  This returns a 
     * string if it is the save-as dialog, or an object if it is
     * an existing resource.
     *
     * @return the selected resource
     */
    getResource: function() {
        if (this.dialog.getObj('dialog.file.type').disabled) {
            var path = this.currentFolder.getResourceId();
            var name = this.dialog.getObj('dialog.file.name').value;
            var select = this.dialog.getObj('dialog.file.type');
            var type = select.options[select.selectedIndex].value;
            return path + name + '.' + type;
            
        } else {
            return this.currentResource;
        }
    },
    /**
     * close the file dialog
     */
    close: function() {
        this.dialog.close();
        this.handler = null;
    },
    /**
     * internal handler for handling actions, optionally dispatching to
     * the handler supplied by the caller
     */
    handleDialog: function(button, dialog) {
        if (this.handler) {
            this.handler(button,this);
        }
    },
    /**
     * when the dialog opens, populate the initial path
     */
    onOpen: function() {
        this.redrawPath();
    },
    /**
     * update the interface for a particular path in the repository.  This
     * results in a call to the broker and actually listing the resources
     * is then done in populateArea.
     *
     * @path {String} the path to update to.
     */
    redrawPath: function( path ) {
        //jxTrace( 'WSFileDialog.redrawPath( '+path+')');
        this.currentFolder = path ? new WSResourceId(path) : this.currentFolder;
        var aPath = this.currentFolder.getPathAsArray();
        var select = this.dialog.getObj('dialog.file.folder');
        var partialPath = '';
        var sep = '';
        var depth = '';
        for (var i=0; i<aPath.length; i++) {
            if (select.options.length > 0 && select.options[i] != aPath[i]) {
                //clip select options from here down
                while (select.options.length > i) {
                    select.options[i] = null;
                }
            }
            partialPath = partialPath + aPath[i] + sep;
            sep = '/';
            if (select.options.length == i) {
                var opt = new Option( depth + aPath[i], partialPath);
                depth = depth + ' . ';
                select.options[i] = opt;
            }   
        }
        var name = this.currentFolder.getName();
        if (name != '') {
            var opt = new Option(depth+name,partialPath+sep+name,true,true);
            select.options[select.options.length] = opt;
        }
        //now remove everything from the list area
        var l = this.dialog.getObj('dialog.file.list');
        while(l.childNodes.length > 0) {
            l.removeChild(l.childNodes[0]);
        }
        var r = new WSEnumerateResources(this.currentFolder.getResourceId(), '', 1);
        this.app.getBroker().dispatchRequest(r,this.populateArea.bind(this));        
    },
    /**
     * populate the file area of the dialog with the contents of this folder
     *
     * @param r {Object} the XmlHttpRequest object from redrawPath
     */
    populateArea: function( r ) {
        var resources = this.app._oFactory.process(r.responseXML);
        
        select = this.dialog.getObj('dialog.file.type');
        var filter = select.options[select.selectedIndex].value;

        //this filters out folders and ones that match the current filter
        if (filter != '') {
            resources = resources.findAll(function(filter, resource,index){
                return resource.getType() == 'folder' || resource.getType() == filter;}.bind(this, filter));
        }

        //this sorts the resources by name but also puts folders first by using UPPER case
        //for folder names and lower case for item names.  The names aren't changed.
        resources = resources.sortBy(function(resource,index) {return resource.getType() == 'folder' ? resource.getName().toUpperCase() : resource.getName().toLowerCase()});
        
        var list = this.dialog.getObj('dialog.file.list');
        for(var i=0; i<resources.length; i++) {
            var resource = resources[i];
            var li = document.createElement('li');
            var a = document.createElement('a');
            var type = resource.getType();
            a.href='#';
            switch(type) {
                case 'folder':
                    a.className = 'ResourceFolder';
                    Event.observe(a, 'dblclick', this.redrawPath.bind(this, resource.getResourceID()));
                    break;
                default:
                    a.className = type;
                    Event.observe( a, 'click', this.selectResource.bindAsEventListener(this));
                    Event.observe(a, 'dblclick', this.selectResource.bindAsEventListener(this));
                    a.resource = resource;
            }
            var span = document.createElement('span');
            span.innerHTML = resource.getName();
            a.appendChild(span);
            li.appendChild(a);
            list.appendChild(li);
        }
    },
    /**
     * handle the user requesting to go up a directory in the repository
     */
    browseUp: function() {
        var select = this.dialog.getObj('dialog.file.folder');
        if (select.options.length>1) {
            var path = select.options[select.options.length-2].value;
            this.redrawPath(path);
        }
    },
    /**
     * handle the user descending into a sub-folder
     */
    browseToFolder: function() {
        var select = this.dialog.getObj('dialog.file.folder');
        var path = select.options[select.selectedIndex].value;
        this.redrawPath(path);
    },
    /**
     * handle the user clicking on a resource.  If this was a double
     * click, trigger the open action
     */
    selectResource: function( e ) {
        var target = Event.findElement(e, 'A');
        var resource = target.resource;
        if (resource) {
            var input = this.dialog.getObj('dialog.file.name');
            input.value = resource.getName();
            this.currentResource = resource;
            
            if (e.type == 'dblclick') {
                this.handleDialog('Open', this.dialog);
            }
        }
    }
});



var WSStylePointDialog = Class.create();
Object.extend(WSStylePointDialog.prototype, {
    dialog: null,
    handler: null,
    app: null,
    currentRule: null,
    initialize: function( options ) {
        this.handler = options.handler;
        this.onContentLoadedCallback = options.onContentLoaded || null;
        this.app = options.app;
        this.fileDialog = new WSFileDialog({app:this.app});
        var size = Position.getPageDimensions();
        var o = {
            title: 'Style Point',
            id: 'stylepointdialog',
            contentURL : 'dialogs/stylepoint.html',
            helpURL : 'help/dialog_stylepoint.html',
            width: 500,
            height: 450,
            top: (size.height-450)/2,
            left: (size.width-500)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.dialog = new JxDialog( o );
        this.bContentLoaded = false;
        this.bIsOpen = false;
        this.expressionDialog = new WSExpressionBuilderDialog({app:this.app, handler:this.handleExpression.bind(this)});
    },
    onContentLoaded: function() {
        this.dialog.registerIds(['dialog.stylepoint.symbol',
                                 'dialog.stylepoint.w2dtable',
                                 'dialog.stylepoint.marktable',
                                 'dialog.stylepoint.fonttable',
                                 'dialog.stylepoint.width',
                                 'dialog.stylepoint.choosewidth',
                                 'dialog.stylepoint.height',
                                 'dialog.stylepoint.resourceid',
                                 'dialog.stylepoint.chooseresourceid',
                                 'dialog.stylepoint.name',
                                 'dialog.stylepoint.chooseheight',
                                 'dialog.stylepoint.font',
                                 'dialog.stylepoint.character',
                                 'dialog.stylepoint.size',
                                 'dialog.stylepoint.choosesize',
                                 'dialog.stylepoint.bold',
                                 'dialog.stylepoint.italic',
                                 'dialog.stylepoint.underline',
                                 'dialog.stylepoint.fillcolor',
                                 'dialog.stylepoint.fillcolorpreview',
                                 'dialog.stylepoint.choosefillcolor', /* disabled until we get a color picker */
                                 'dialog.stylepoint.bordercolor',
                                 'dialog.stylepoint.bordercolorpreview',
                                 'dialog.stylepoint.choosebordercolor', /* disabled until we get a color picker */
                                 'dialog.stylepoint.x',
                                 'dialog.stylepoint.y',
                                 'dialog.stylepoint.choosexy',
                                 'dialog.stylepoint.rotation',
                                 'dialog.stylepoint.chooserotation',
                                 'dialog.stylepoint.context',
                                 'dialog.stylepoint.units'], this.dialog.domObj);
                                 
        Event.observe(this.dialog.getObj('dialog.stylepoint.symbol'), 'change', this.symbolChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.fillcolor'), 'change', this.fillColorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.bordercolor'), 'change', this.borderColorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.context'), 'change', this.contextChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.chooseresourceid'), 'click', this.chooseResourceId.bind(this));
        
        Event.observe(this.dialog.getObj('dialog.stylepoint.choosesize'), 'click', this.handleChooseSize.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.choosewidth'), 'click', this.handleChooseWidth.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.chooseheight'), 'click', this.handleChooseHeight.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylepoint.chooserotation'), 'click', this.handleChooseRotation.bind(this));
        
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
        this.bContentLoaded = true;
        if (this.bIsOpen) {
            this.draw();
        }
    },
    open: function(rule, properties) {
        this._rule = rule.clone();
        this.rule = rule;
        this.properties = properties;
        this.dialog.open();
    },
    close: function() {
        this.bIsOpen = false;
        this.dialog.close();
    },
    handleDialog: function(button, dialog) {
        /* all other buttons are handled elsewhere */
        if (button == 'ok' || button == 'cancel') {
            if (this.handler) {
                this.handler(button,this);
            }
        }
    },
    onOpen: function() {
        this.bIsOpen = true;
        if (this.bContentLoaded) {
            this.draw();
        }
    },
    handleChooseSize: function() {
        this.expressionDialog.target = 'dialog.stylepoint.size';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylepoint.size').value);        
    },
    handleChooseWidth: function() {
        this.expressionDialog.target = 'dialog.stylepoint.width';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylepoint.width').value);        
    },
    handleChooseHeight: function() {
        this.expressionDialog.target = 'dialog.stylepoint.height';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylepoint.height').value);        
    },
    handleChooseRotation: function() {
        this.expressionDialog.target = 'dialog.stylepoint.rotation';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylepoint.rotation').value);        
    },
    handleExpression: function(button, dialog) {
        if (button == 'ok') {
            this.dialog.getObj(dialog.target).value = dialog.getExpression();
        }
        dialog.close();
    },
    symbolChanged: function() {
        var select = this.dialog.getObj('dialog.stylepoint.symbol');
        this.updateForSymbol(select.options[select.selectedIndex].value);
    },
    fillColorChanged: function() {
        var color = this.dialog.getObj('dialog.stylepoint.fillcolor').value;
        if (color != '') {
            color = '#' + color.substr(2);
        }
        this.dialog.getObj('dialog.stylepoint.fillcolorpreview').style.backgroundColor = color;
    },
    borderColorChanged: function() {
        var color = this.dialog.getObj('dialog.stylepoint.bordercolor').value;
        if (color != '') {
            color = '#' + color.substr(2);
        }
        this.dialog.getObj('dialog.stylepoint.bordercolorpreview').style.backgroundColor = color;
    },
    contextChanged: function() {
        var select = this.dialog.getObj('dialog.stylepoint.context');
        this.updateForContext(select.options[select.selectedIndex].value);
    },
    chooseResourceId: function() {
        this.fileDialog.chooseResource({handler:this.handleChooseResourceId.bind(this), types:this.fileDialog.getFilter('SymbolLibrary')});
    },
    handleChooseResourceId: function(button,dialog) {
        if (button == 'Select') {
            this.dialog.getObj('dialog.stylepoint.resourceid').value = dialog.getResource();
            var r = new WSGetResourceContent(dialog.getResource());
            this.app.getBroker().dispatchRequest(r, this.updateSymbolNames.bind(this));
        }
        dialog.close();
    },
    updateSymbolNames: function(r) {
        var select = this.dialog.getObj('dialog.stylepoint.name');
        while (select.options.length > 0) {
            select.options[0] = null;
        }
        var library = new DomNode(r.responseXML);
        var symbol = library.findFirstNode('Symbol');
        while(symbol) {
            var name = symbol.getNodeText('Name');
            var opt = new Option(name, name);
            select.options[select.options.length] = opt;
            symbol = library.findNextNode('Symbol');
        }
    },
    updateForContext: function(context) {
        var select = this.dialog.getObj('dialog.stylepoint.units');
        while (select.options.length > 0) {
            select.options[0] = null;
        }
        if (context == 'MappingUnits') {
            select.options[0] = new Option('Millimeters','Millimeters');
            select.options[1] = new Option('Centimeters','Centimeters');
            select.options[2] = new Option('Meters','Meters');
            select.options[3] = new Option('Kilometers','Kilometers');
            select.options[4] = new Option('Inches','Inches');
            select.options[5] = new Option('Feet','Feet');
            select.options[6] = new Option('Yards','Yards');
            select.options[7] = new Option('Miles', 'Miles');
        } else {
            select.options[0] = new Option('Millimeters','Millimeters');
            select.options[1] = new Option('Centimeters','Centimeters');
            select.options[2] = new Option('Inches','Inches');
            select.options[3] = new Option('Points','Points');
        }
    },
    updateForSymbol: function(symbol) {
        var ids = [  'dialog.stylepoint.width',
                     'dialog.stylepoint.choosewidth',
                     'dialog.stylepoint.height',
                     'dialog.stylepoint.resourceid',
                     'dialog.stylepoint.chooseresourceid',
                     'dialog.stylepoint.name',
                     'dialog.stylepoint.chooseheight',
                     'dialog.stylepoint.font',
                     'dialog.stylepoint.character',
                     'dialog.stylepoint.size',
                     'dialog.stylepoint.choosesize',
                     'dialog.stylepoint.bold',
                     'dialog.stylepoint.italic',
                     'dialog.stylepoint.underline',
                     'dialog.stylepoint.fillcolor',
                     'dialog.stylepoint.fillcolorpreview',
                     /*'dialog.stylepoint.choosefillcolor',  disabled until we get a color picker */
                     'dialog.stylepoint.bordercolor',
                     'dialog.stylepoint.bordercolorpreview',
                     /*'dialog.stylepoint.choosebordercolor',  disabled until we get a color picker */
                     'dialog.stylepoint.x',
                     'dialog.stylepoint.y',
                     /*'dialog.stylepoint.choosexy',*/
                     'dialog.stylepoint.rotation',
                     'dialog.stylepoint.chooserotation',
                     /*'dialog.stylepoint.context',*/
                     'dialog.stylepoint.units'];
        for (var i=0; i<ids.length;i++) {
            this.dialog.getObj(ids[i]).disabled = (symbol == '');
        }
        switch(symbol) {
            case '':
            case 'Square':
            case 'Circle':
            case 'Triangle':
            case 'Star':
            case 'Cross':
            case 'X':
                this.dialog.getObj('dialog.stylepoint.w2dtable').style.display = 'none';
                this.dialog.getObj('dialog.stylepoint.marktable').style.display = 'table';
                this.dialog.getObj('dialog.stylepoint.fonttable').style.display = 'none';
                break;
            case 'Font':
                this.dialog.getObj('dialog.stylepoint.w2dtable').style.display = 'none';
                this.dialog.getObj('dialog.stylepoint.marktable').style.display = 'none';
                this.dialog.getObj('dialog.stylepoint.fonttable').style.display = 'table';
                break;
            case 'W2D':
                this.dialog.getObj('dialog.stylepoint.w2dtable').style.display = 'table';
                this.dialog.getObj('dialog.stylepoint.marktable').style.display = 'table';
                this.dialog.getObj('dialog.stylepoint.fonttable').style.display = 'none';
        }
    },
    draw: function() {
        var ps = this._rule.get('PointSymbolization2D');
        if (ps == '') {
            this.updateForSymbol('');
        } else {
            /* five possibilities - Mark, Image, Font, W2D, or Block */
            var symbol = ps.get('Mark');
            if (symbol != '') {
                var shape = symbol.get('Shape');
                this.updateForSymbol(shape);
                this.setSelect('dialog.stylepoint.symbol', shape);
            
                this.dialog.getObj('dialog.stylepoint.width').value = symbol.get('SizeX');
                this.dialog.getObj('dialog.stylepoint.height').value = symbol.get('SizeY');
            
                var fill = symbol.get('Fill');
                if (fill != '') {
                    var color = fill.get('ForegroundColor');
                    this.dialog.getObj('dialog.stylepoint.fillcolor').value = color;
                    this.dialog.getObj('dialog.stylepoint.fillcolorpreview').style.backgroundColor = '#' + color.substr(2);
                } else {
                    this.dialog.getObj('dialog.stylepoint.fillcolorpreview').style.backgroundColor = '';
                }

                var edge = symbol.get('Edge');
                if (edge != '') {
                    color = edge.get('Color');
                    this.dialog.getObj('dialog.stylepoint.bordercolor').value = color;
                    this.dialog.getObj('dialog.stylepoint.bordercolorpreview').style.backgroundColor = '#' + color.substr(2);
                } else {
                    this.dialog.getObj('dialog.stylepoint.bordercolorpreview').style.backgroundColor = '';
                }
            
            } else {
                symbol = ps.get('Font');
                if (symbol != '') {
                    this.setSelect('dialog.stylepoint.symbol', 'Font');
                    this.updateForSymbol('Font');
                
                    this.dialog.getObj('dialog.stylepoint.font').value = symbol.get('FontName');
                    this.dialog.getObj('dialog.stylepoint.character').value = symbol.get('Character');
                    this.dialog.getObj('dialog.stylepoint.size').value = symbol.get('SizeY');
                    this.dialog.getObj('dialog.stylepoint.bold').checked = (symbol.get('Bold').toLowerCase() == 'true');
                    this.dialog.getObj('dialog.stylepoint.italic').checked = (symbol.get('Italic').toLowerCase() == 'true');
                    this.dialog.getObj('dialog.stylepoint.underline').checked = (symbol.get('Underlined').toLowerCase() == 'true');
                } else {
                    symbol = ps.get('W2D');
                    if (symbol != '') {
                        this.setSelect('dialog.stylepoint.symbol', 'W2D');
                        this.updateForSymbol('W2D');
                        var w2dSymbol = symbol.get('W2DSymbol');
                        this.dialog.getObj('dialog.stylepoint.resourceid').value = w2dSymbol.get('ResourceId');
                        this.dialog.getObj('dialog.stylepoint.name').value = w2dSymbol.get('LibraryItemName');
                    }
                }
            }
        
            this.dialog.getObj('dialog.stylepoint.x').value = symbol.get('InsertionPointX');
            this.dialog.getObj('dialog.stylepoint.y').value = symbol.get('InsertionPointY');
            this.dialog.getObj('dialog.stylepoint.rotation').value = symbol.get('Rotation');
        
            var deviceContext = symbol.get('DeviceContext')
            this.setSelect('dialog.stylepoint.context', deviceContext);
        
            this.updateForContext(deviceContext);
            this.setSelect('dialog.stylepoint.units', symbol.get('Unit'));
        }
    },
    updateRule: function() {
        var shape = this.getSelect('dialog.stylepoint.symbol');
        var symbolization = new WSPointSymbolization2DType();
        switch(shape) {
            case 'Square':
            case 'Circle':
            case 'Triangle':
            case 'Star':
            case 'Cross':
            case 'X':
                var symbol = new WSMarkSymbolType();
                symbol.set('Unit', this.getSelect('dialog.stylepoint.units'));
                symbol.set('SizeContext', this.getSelect('dialog.stylepoint.context'));
                symbol.set('SizeX',  this.dialog.getObj('dialog.stylepoint.width').value);
                symbol.set('SizeY', this.dialog.getObj('dialog.stylepoint.height').value);
                symbol.set('Rotation', this.dialog.getObj('dialog.stylepoint.rotation').value);
                //symbol.set('MaintainAspect', );
                symbol.set('InsertionPointX', this.dialog.getObj('dialog.stylepoint.x').value);
                symbol.set('InsertionPointY', this.dialog.getObj('dialog.stylepoint.y').value);
                symbol.set('Shape', shape);
                var color = this.dialog.getObj('dialog.stylepoint.fillcolor').value;
                if (color != '') {
                    var fill = new WSFillType();
                    fill.set('FillPattern', 'Solid');
                    fill.set('ForegroundColor', color);
                    fill.set('BackgroundColor', 'FF000000');
                    symbol.set('Fill', fill);
                }
                color = this.dialog.getObj('dialog.stylepoint.bordercolor').value;
                if (color != '') {
                    var edge = new WSStrokeType();
                    edge.set('LineStyle', 'Solid');
                    edge.set('Thickness', '0');
                    edge.set('Color', color);
                    edge.set('Unit', this.getSelect('dialog.stylepoint.units'));
                    symbol.set('Edge', edge);
                }  
                symbolization.set('Mark', symbol);
                break;
            case 'Font':
                var symbol = new WSFontSymbolType();
                symbol.set('Unit', this.getSelect('dialog.stylepoint.units'));
                symbol.set('SizeContext', this.getSelect('dialog.stylepoint.context'));
                symbol.set('SizeX',  this.dialog.getObj('dialog.stylepoint.width').value);
                symbol.set('SizeY', this.dialog.getObj('dialog.stylepoint.height').value);
                symbol.set('Rotation', this.dialog.getObj('dialog.stylepoint.rotation').value);
                //symbol.set('MaintainAspect', );
                symbol.set('InsertionPointX', this.dialog.getObj('dialog.stylepoint.x').value);
                symbol.set('InsertionPointY', this.dialog.getObj('dialog.stylepoint.y').value);
                symbol.set('FontName', this.dialog.getObj('dialog.stylepoint.font').value);
                symbol.set('Character' ,this.dialog.getObj('dialog.stylepoint.character').value);
                if (this.dialog.getObj('dialog.stylepoint.bold').checked) {
                    symbol.set('Bold','true');
                }
                if (this.dialog.getObj('dialog.stylepoint.italic').checked) {
                    symbol.set('Italic','true');
                }
                if (this.dialog.getObj('dialog.stylepoint.underline').checked) {
                    symbol.set('Underlined','true');
                }
                symbol.set('ForegroundColor', this.dialog.getObj('dialog.stylepoint.fillcolor').value);
                symbolization.set('Font', symbol);
                break;
            case 'W2D':
                var symbol = new WSW2DSymbolType();
                symbol.set('Unit', this.getSelect('dialog.stylepoint.units'));
                symbol.set('SizeContext', this.getSelect('dialog.stylepoint.context'));
                symbol.set('SizeX',  this.dialog.getObj('dialog.stylepoint.width').value);
                symbol.set('SizeY', this.dialog.getObj('dialog.stylepoint.height').value);
                symbol.set('Rotation', this.dialog.getObj('dialog.stylepoint.rotation').value);
                //symbol.set('MaintainAspect', );
                symbol.set('InsertionPointX', this.dialog.getObj('dialog.stylepoint.x').value);
                symbol.set('InsertionPointY', this.dialog.getObj('dialog.stylepoint.y').value);
                
                var W2D = new W2DSymbol();
                W2D.set('ResourceId', this.dialog.getObj('dialog.stylepoint.resourceid').value);
                W2D.set('LibraryItemName', this.dialog.getObj('dialog.stylepoint.name').value);
                symbol.set('W2DSymbol', W2D);
                symbolization.set('W2D', symbol);
                break;
            default:
                symbolization = '';
        }
        this.rule.set('PointSymbolization2D', symbolization);
    },
    getSelect: function(name) {
        var select = this.dialog.getObj(name);
        return select.options[select.selectedIndex].value;
    },
    setSelect: function(name, value) {
        var select = this.dialog.getObj(name);
        for (var i=0; i<select.options.length; i++) {
            if (select.options[i].value == value) {
                select.selectedIndex = i;
                break;
            }
        }
    }
});

var WSStyleLineDialog = Class.create();
Object.extend(WSStyleLineDialog.prototype, {
    dialog: null,
    handler: null,
    app: null,
    currentRule: null,
    initialize: function( options ) {
      //jxTrace('WSStyleLineDialog::initialize()');
        this.handler = options.handler;
        this.app = options.app;
        var size = Position.getPageDimensions();
        var o = {
            title: 'Style Line',
            id: 'stylelinedialog',
            contentURL : 'dialogs/styleline.html',
            helpURL : 'help/dialog_styleline.html',
            width: 500,
            height: 450,
            top: (size.height-450)/2,
            left: (size.width-500)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.bContentLoaded = false;
        this.bIsOpen = false;
        this.dialog = new JxDialog( o );
    },
    onContentLoaded: function() {
      //jxTrace('WSStyleLineDialog::onContentLoaded()');
        var step = 1;
        this.dialog.registerIds(['dialog.styleline.toolbar',
                                 'dialog.styleline.lines',
                                 'dialog.styleline.pattern',
                                 'dialog.styleline.patternpreview',
                                 'dialog.styleline.choosepattern',
                                 'dialog.styleline.thickness',
                                 'dialog.styleline.color',
                                 'dialog.styleline.choosecolor',
                                 'dialog.styleline.colorpreview',
                                 /* 'dialog.styleline.context', */
                                 'dialog.styleline.units'], this.dialog.domObj);
                                 
        Event.observe(this.dialog.getObj('dialog.styleline.choosepattern'), 'click', this.choosePattern.bind(this));
        Event.observe(this.dialog.getObj('dialog.styleline.color'), 'change', this.colorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.styleline.thickness'), 'change', this.thicknessChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.styleline.units'), 'change', this.unitsChanged.bind(this));
        
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
        if (!this.patternPicker) {
            this.patternPicker = new JxPicker(this.dialog.getObj('dialog.styleline.pattern'));
            this.patternPicker.addSelectionListener(this);
            var patterns = ['ACAD_ISO02W100','ACAD_ISO03W100','ACAD_ISO04W100',
            'ACAD_ISO05W100','ACAD_ISO06W100','ACAD_ISO07W100','ACAD_ISO08W100',
            'ACAD_ISO09W100','ACAD_ISO10W100','ACAD_ISO11W100','ACAD_ISO12W100',
            'ACAD_ISO13W100','ACAD_ISO14W100','ACAD_ISO15W100','BORDER','BORDER2',
            'BORDERX2','CENTER','CENTER2','CENTERX2','DASHDOT2','DASHDOT3','DASHDOTX2',
            'DASHED','DASHED2','DASHEDX2','DIVIDE','DIVIDE2','DIVIDEX2','DOT2','DOT3',
            'DOTX2','Dash','DashDot','DashDotDot','Dot','FENCELINE1','FENCELINE2','HIDDEN',
            'HIDDEN2','HIDDENX2','LINE03','LINE04','LINE05','LINE06','LINE07','LINE08',
            'LINE09','LINE10','LINE11','LINE12','LINE13','LINE14','LINE15','LINE16',
            'LINE17','LINE18','LINE19','LINE20','LINE21','LINE22','LINE23','LINE24',
            'LINE25','PHANTOM','PHANTOM2','PHANTOMX2','Rail','Solid','TRACKS'];
            for (var i=0; i<patterns.length; i++) {
                this.patternPicker.add(patterns[i], patterns[i], 'url(images/lines/'+patterns[i]+'.png)', false);
            }
        }
        
        var toolbar = new JxToolbar(this.dialog.getObj('dialog.styleline.toolbar'));
        this.addAction = new JxAction(this.dispatchAdd.bind(this));
        this.duplicateAction = new JxAction(this.dispatchDuplicate.bind(this));
        this.deleteAction = new JxAction(this.dispatchDelete.bind(this));
        this.moveUpAction = new JxAction(this.dispatchMoveUp.bind(this));
        this.moveDownAction = new JxAction(this.dispatchMoveDown.bind(this));
        
        var addItem = new JxToolbarItem('images/icon_add.png', 'Add line style', this.addAction);
        var duplicateItem = new JxToolbarItem('images/icon_duplicate.png', 'Duplicate selected style', this.duplicateAction);
        var deleteItem = new JxToolbarItem('images/icon_delete.png', 'Delete selected line style', this.deleteAction);
        var moveUpItem = new JxToolbarItem('images/icon_move_up.png', 'Move selected line style up', this.moveUpAction);
        var moveDownItem = new JxToolbarItem('images/icon_move_down.png', 'Move selected line style down', this.moveDownAction);
        
        toolbar.add(addItem, duplicateItem, deleteItem, moveUpItem, moveDownItem);
        
        this.selectLineWatcher = this.lineClicked.bindAsEventListener(this);
        this.bContentLoaded = true;
        
        if (this.bIsOpen) {
            this.draw();
        }
    },
    open: function(rule) {
      //jxTrace('WSStyleLineDialog::open()');
        this.rule = rule;
        this._rule = rule.clone();
        this.dialog.open();
    },
    close: function() {
      //jxTrace('WSStyleLineDialog::close()');
        this.bIsOpen = false;
        this.dialog.close();
    },
    handleDialog: function(button, dialog) {
      //jxTrace('WSStyleLineDialog::handleDialog()');
        /* all other buttons are handled elsewhere */
        if (button == 'ok' || button == 'cancel') {
            if (this.handler) {
                this.handler(button,this);
            }
        }
    },
    onOpen: function() {
      //jxTrace('WSStyleLineDialog::onOpen()');
        this.bIsOpen = true;
        if (this.bContentLoaded) {
            this.draw();
        }
    },
    setPattern: function(pattern) {
        this.dialog.getObj('dialog.styleline.patternpreview').innerHTML = pattern;
        this.dialog.getObj('dialog.styleline.patternpreview').style.backgroundImage = 'url(images/lines/'+pattern+'.png)';
    },
    choosePattern: function() {
        this.patternPicker.open();
    },
    selectionChanged: function(picker) {
        var pattern = picker.getSelection();
        if (pattern) {
            this.setPattern(pattern);
            if (this.currentSelection >= 0) {
                var ls = this._rule.get('LineSymbolization2D')[this.currentSelection];
                ls.set('LineStyle', pattern);
                this.updatePreview();
            }
        }
        picker.close();
    },
    colorChanged: function() {
        var color = this.dialog.getObj('dialog.styleline.color').value;
        this.dialog.getObj('dialog.styleline.colorpreview').style.backgroundColor = '#'+color.substr(2);
        if (this.currentSelection >= 0) {
            var ls = this._rule.get('LineSymbolization2D')[this.currentSelection];
            ls.set('Color', color);
            this.updatePreview();
        }        
    },
    thicknessChanged: function() {
        if (this.currentSelection >= 0) {
            var ls = this._rule.get('LineSymbolization2D')[this.currentSelection];
            ls.set('Thickness', this.dialog.getObj('dialog.styleline.thickness').value);
            this.updatePreview();
        }
    },
    unitsChanged: function() {
        if (this.currentSelection >= 0) {
            var ls = this._rule.get('LineSymbolization2D')[this.currentSelection];
            var currentUnit = ls.get('Unit');
            var from = this.getConversionForUnit(currentUnit);
            var newUnit = this.getSelect('dialog.styleline.units');
            var to = this.getConversionForUnit(newUnit);
            var t = this.dialog.getObj('dialog.styleline.thickness').value;
            var newValue = parseFloat(t) * to.unitsPerPixel / from.unitsPerPixel;
            this.dialog.getObj('dialog.styleline.thickness').value = newValue;
            ls.set('Thickness', newValue);
            ls.set('Unit', newUnit);
            this.updatePreview();
        }
    },
    updatePreview: function() {
        if (this.currentSelection >= 0) {
            var ls = this._rule.get('LineSymbolization2D')[this.currentSelection];
            var preview = this.createLinePreview(ls);
            
            var ul = this.dialog.getObj('dialog.styleline.lines');
            var li = ul.childNodes[this.currentSelection];
            li.childNodes[0].replaceChild(preview, li.childNodes[0].childNodes[0]);
        }
    },
    draw: function() {
      //jxTrace('WSStyleLineDialog::draw()');
        var step = 1;
        var ul = this.dialog.getObj('dialog.styleline.lines');
      //jxTrace('WSStyleLineDialog::draw() step ' + (step++));
        
        while (ul.childNodes.length > 0) {
            Event.stopObserving(ul.childNodes[0].childNodes[0], 'click', this.selectLineWatcher);
            ul.removeChild(ul.childNodes[0]);
        }
      //jxTrace('WSStyleLineDialog::draw() step ' + (step++));
        
        var lineSymbolizations = this._rule.get('LineSymbolization2D');
        for (var i=0; i<lineSymbolizations.length; i++) {
            this.addLineSymbolization(lineSymbolizations[i]);
        }
      //jxTrace('WSStyleLineDialog::draw() step ' + (step++));
        this.currentSelection = -1;
        if (lineSymbolizations.length > 0) {
            this.selectLine(0);
        }
      //jxTrace('WSStyleLineDialog::draw() step ' + (step++));
        this.activateActions();
      //jxTrace('WSStyleLineDialog::draw() step ' + (step++));
    },
    addLineSymbolization: function(symbolizer) {
      //jxTrace('WSStyleLineDialog::addLineSymbolization()');
        var preview = this.createLinePreview(symbolizer);
        var li = document.createElement('li');
        var a = document.createElement('a');
        a.href = '#';
        li.appendChild(a)
        a.appendChild(preview);
        this.dialog.getObj('dialog.styleline.lines').appendChild(li);
        Event.observe(a, 'click', this.selectLineWatcher);
    },
    activateActions: function() {
        this.addAction.setEnabled(true);
        this.duplicateAction.setEnabled(this.currentSelection >= 0);
        this.deleteAction.setEnabled(this.currentSelection >= 0);
        this.moveUpAction.setEnabled(this.currentSelection > 0);
        this.moveDownAction.setEnabled(this.currentSelection < this._rule.get('LineSymbolization2D').length - 1);
    },
    lineClicked: function(e) {
        var a = Event.findElement(e, 'A');
        idx = this.getIndexForElement(a);
        this.selectLine(idx);
    },
    selectLine: function(idx) {
        if (idx >=0 ) {
            if (this.currentSelection >= 0) {
                Element.removeClassName(this.getElementForIndex(this.currentSelection), 'selected');
            }
            this.currentSelection = idx;
            Element.addClassName(this.getElementForIndex(this.currentSelection), 'selected');
            var ls = this._rule.get('LineSymbolization2D')[idx];
            
            this.setPattern(ls.get('LineStyle'));
            this.dialog.getObj('dialog.styleline.thickness').value = ls.get('Thickness');

            color = ls.get('Color');
            this.dialog.getObj('dialog.styleline.color').value = color;
            this.dialog.getObj('dialog.styleline.colorpreview').style.backgroundColor = '#' + color.substr(2);

            this.setSelect('dialog.styleline.units', ls.get('Unit'));
            this.dialog.getObj('dialog.styleline.units').disabled = false;
            this.dialog.getObj('dialog.styleline.color').disabled = false;
            this.dialog.getObj('dialog.styleline.thickness').disabled = false;
            this.dialog.getObj('dialog.styleline.choosepattern').disabled = false;
        } else {
            this.currentSelection = idx;
            this.dialog.getObj('dialog.styleline.units').disabled = true;
            this.dialog.getObj('dialog.styleline.color').disabled = true;
            this.dialog.getObj('dialog.styleline.thickness').disabled = true;
            this.dialog.getObj('dialog.styleline.choosepattern').disabled = true;
        }
        this.activateActions();
    },
    getIndexForElement: function(a) {
        var li = a.parentNode;
        var ul = li.parentNode;
        var idx = -1;
        for (var i=0; ul.childNodes.length; i++){
            if (ul.childNodes[i] == li) {
                idx = i;
                break;
            }
        }
        return idx;
    },
    getElementForIndex: function(idx) {
        var ul = this.dialog.getObj('dialog.styleline.lines');
        return ul.childNodes[idx];
    },
    updateRule: function() {
        this.rule.set('LineSymbolization2D', this._rule.get('LineSymbolization2D'));
    },
    getSelect: function(name) {
        var select = this.dialog.getObj(name);
        return select.options[select.selectedIndex].value;
    },
    setSelect: function(name, value) {
        var select = this.dialog.getObj(name);
        for (var i=0; i<select.options.length; i++) {
            if (select.options[i].value == value) {
                select.selectedIndex = i;
                break;
            }
        }
    },
    dispatchAdd: function() {
        var ls = DomNodeFactory.create('LineSymbolization2D');
        var symbolizer = new WSStrokeType(ls);
        var symbolizers = this._rule.get('LineSymbolization2D');
        symbolizers.push(symbolizer);
        symbolizer.set('LineStyle', 'Solid');
        symbolizer.set('Color', 'FF000000');
        symbolizer.set('Thickness', '1');
        symbolizer.set('Unit', 'Points');
        symbolizer.set('SizeContext', 'DeviceUnits');
        this._rule.set('LineSymbolization2D', symbolizers);
        this.addLineSymbolization(symbolizer);
        this.selectLine(symbolizers.length - 1);
    },
    dispatchDuplicate: function() {
        var symbolizers = this._rule.get('LineSymbolization2D');
        var symbolizer = symbolizers[this.currentSelection].clone();
        symbolizers.push(symbolizer);
        this._rule.set('LineSymbolization2D', symbolizers);
        this.addLineSymbolization(symbolizer);
        this.selectLine(symbolizers.length - 1);
    },
    dispatchDelete: function() {
        var symbolizers = this._rule.get('LineSymbolization2D');
        symbolizers.splice(this.currentSelection, 1);
        this._rule.set('LineSymbolization2D', symbolizers);
        var ul = this.dialog.getObj('dialog.styleline.lines');
        var li = ul.childNodes[this.currentSelection];
        Event.stopObserving(li.childNodes[0], 'click', this.selectLineWatcher);
        ul.removeChild(li);
        if (symbolizers.length > 0) {
            var newSelection = this.currentSelection;
            this.currentSelection = -1;
            if (newSelection > 0) {
                this.selectLine(newSelection - 1);
            } else {
                this.selectLine(0);
            }
        } else {
            this.selectLine(-1);
            this.activateActions();
        }
    },
    dispatchMoveUp: function() {
        if (this.currentSelection > 0) {
            var ul = this.dialog.getObj('dialog.styleline.lines');
            var li = ul.childNodes[this.currentSelection];
            ul.insertBefore(li, ul.childNodes[this.currentSelection-1]);
            
            var symbolizers = this._rule.get('LineSymbolization2D');
            var symbolizer = symbolizers[this.currentSelection];
            symbolizers.splice(this.currentSelection, 1);
            this.currentSelection --;
            symbolizers.splice(this.currentSelection, 0, symbolizer);
            this._rule.set('LineSymbolization2D', symbolizers);
            this.activateActions();
        }
    },
    dispatchMoveDown: function() {
        var symbolizers = this._rule.get('LineSymbolization2D');
        if (this.currentSelection < symbolizers.length - 1) {
            var ul = this.dialog.getObj('dialog.styleline.lines');
            var li = ul.childNodes[this.currentSelection];
            ul.insertBefore(ul.childNodes[this.currentSelection+1], li);
            
            var symbolizers = this._rule.get('LineSymbolization2D');
            var symbolizer = symbolizers[this.currentSelection];
            symbolizers.splice(this.currentSelection, 1);
            this.currentSelection ++;
            symbolizers.splice(this.currentSelection, 0, symbolizer);
            this._rule.set('LineSymbolization2D', symbolizers);
            this.activateActions();
        }
    },
    createLinePreview: function(lineSymbolization) {
        var preview = document.createElement('div');
        preview.className = "previewArea";
        
        var t = parseFloat(lineSymbolization.get('Thickness'));
        var c = lineSymbolization.get('Color');
        c = c.substr(2);
        var line = document.createElement('div');
        
        var unit = lineSymbolization.get('Unit');
        var conv = this.getConversionForUnit(unit);
        var width = 100 * conv.unitsPerPixel;
        var height = 17 * conv.unitsPerPixel;            
        
        if (t == 0) {
            t = 1 * conv.unitsPerPixel;
        }
        line.style.position = 'absolute';
        line.style.left = '0px';
        line.style.top = ((height-t)/2) + conv.cssSize;
        line.style.width = '50%';
        line.style.height = t + conv.cssSize;
        line.style.backgroundColor = '#' + c;
        line.style.overflow = 'hidden';
        preview.style.backgroundImage = 'url(images/lines/'+lineSymbolization.get('LineStyle')+'.png)';
        
        preview.appendChild(line);
        return preview;
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
    }
});



var WSStyleAreaDialog = Class.create();
Object.extend(WSStyleAreaDialog.prototype, {
    dialog: null,
    handler: null,
    app: null,
    currentRule: null,
    initialize: function( options ) {
        this.handler = options.handler;
        this.onContentLoadedCallback = options.onContentLoaded || null;
        this.app = options.app;
        var size = Position.getPageDimensions();
        var o = {
            title: 'Style Area',
            id: 'styleareadialog',
            contentURL : 'dialogs/stylearea.html',
            helpURL : 'help/dialog_stylearea.html',
            width: 500,
            height: 450,
            top: (size.height-450)/2,
            left: (size.width-500)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.dialog = new JxDialog( o );
        this.bContentLoaded = false;
        this.bIsOpen = false;
    },
    onContentLoaded: function() {
        this.dialog.registerIds(['dialog.stylearea.fillpattern',
                                 'dialog.stylearea.fillpatternpreview',
                                 'dialog.stylearea.choosefillpattern',
                                 'dialog.stylearea.fgcolor',
                                 'dialog.stylearea.fgchoosecolor',
                                 'dialog.stylearea.fgcolorpreview',
                                 'dialog.stylearea.bgcolor',
                                 'dialog.stylearea.bgchoosecolor',
                                 'dialog.stylearea.bgcolorpreview',
                                 'dialog.stylearea.edgepattern',
                                 'dialog.stylearea.edgepatternpreview',
                                 'dialog.stylearea.chooseedgepattern',
                                 'dialog.stylearea.thickness',
                                 'dialog.stylearea.color',
                                 'dialog.stylearea.colorpreview',
                                 'dialog.stylearea.choosecolor',
                                 'dialog.stylearea.units'], this.dialog.domObj);
                                 
        Event.observe(this.dialog.getObj('dialog.stylearea.choosefillpattern'), 'click', this.chooseFillPattern.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.chooseedgepattern'), 'click', this.chooseEdgePattern.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.fgcolor'), 'change', this.fgColorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.bgcolor'), 'change', this.bgColorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.color'), 'change', this.colorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.thickness'), 'change', this.thicknessChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylearea.units'), 'change', this.unitsChanged.bind(this));
        
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
        this.bContentLoaded = true;
        if (this.bIsOpen) {
            this.draw();
        }
    },
    open: function(rule) {
        this.rule = rule;
        this._rule = rule.clone();
        this.dialog.open();
    },
    close: function() {
        this.bIsOpen = false;
        this.dialog.close();
    },
    handleDialog: function(button, dialog) {
        /* all other buttons are handled elsewhere */
        if (button == 'ok' || button == 'cancel') {
            if (this.handler) {
                this.handler(button,this);
            }
        }
    },
    onOpen: function() {
        this.bIsOpen = true;
        if (this.bContentLoaded) {
            this.draw();
        }
    },
    setFillPattern: function(pattern) {
        if (!pattern || pattern == 'None') {
            this.dialog.getObj('dialog.stylearea.fillpatternpreview').innerHTML = 'No Fill';
            this.dialog.getObj('dialog.stylearea.fillpatternpreview').style.backgroundImage = '';
            this._rule.get('AreaSymbolization2D').set('Fill', '');
            this.dialog.getObj('dialog.stylearea.fgcolor').disabled = true;
            this.dialog.getObj('dialog.stylearea.bgcolor').disabled = true;
            this.dialog.getObj('dialog.stylearea.fgcolor').value = '';
            this.dialog.getObj('dialog.stylearea.bgcolor').value = '';
            this.fgColorChanged();
            this.bgColorChanged();
        } else {
            this.dialog.getObj('dialog.stylearea.fillpatternpreview').innerHTML = pattern;
            this.dialog.getObj('dialog.stylearea.fillpatternpreview').style.backgroundImage = 'url(images/areas/'+pattern+'.png)';
            var fill = this._rule.get('AreaSymbolization2D').get('Fill');
            if (fill == '') {
                fill = new WSFillType();
                this._rule.get('AreaSymbolization2D').set('Fill', fill);
                this.dialog.getObj('dialog.stylearea.fgcolor').disabled = false;
                this.dialog.getObj('dialog.stylearea.bgcolor').disabled = false;
                this.dialog.getObj('dialog.stylearea.fgcolor').value = 'FF000000';
                this.dialog.getObj('dialog.stylearea.bgcolor').value = 'FF000000';
                this.fgColorChanged();
                this.bgColorChanged();
            }
            fill.set('FillPattern', pattern);
        }
    },
    chooseFillPattern: function() {
        this.fillPatternPicker.open();
    },
    setEdgePattern: function(pattern) {
        if (!pattern || pattern == 'None') {
            this.dialog.getObj('dialog.stylearea.edgepatternpreview').innerHTML = 'No Border';
            this.dialog.getObj('dialog.stylearea.edgepatternpreview').style.backgroundImage = '';
            this._rule.get('AreaSymbolization2D').set('Stroke', '');
            this.dialog.getObj('dialog.stylearea.thickness').disabled = true;
            this.dialog.getObj('dialog.stylearea.color').disabled = true;
            this.dialog.getObj('dialog.stylearea.units').disabled = true;
            this.dialog.getObj('dialog.stylearea.thickness').value = '0';
            this.dialog.getObj('dialog.stylearea.color').value = '';
            this.setSelect('dialog.stylearea.units', 'Points');
            this.colorChanged();
        } else {
            this.dialog.getObj('dialog.stylearea.edgepatternpreview').innerHTML = pattern;
            this.dialog.getObj('dialog.stylearea.edgepatternpreview').style.backgroundImage = 'url(images/lines/'+pattern+'.png)';
            var edge = this._rule.get('AreaSymbolization2D').get('Stroke');
            if (edge == '') {
                edge = new WSStrokeType();
                this._rule.get('AreaSymbolization2D').set('Stroke', edge);
                this.dialog.getObj('dialog.stylearea.thickness').disabled = false;
                this.dialog.getObj('dialog.stylearea.color').disabled = false;
                this.dialog.getObj('dialog.stylearea.units').disabled = false;
                this.dialog.getObj('dialog.stylearea.thickness').value = '0';
                this.dialog.getObj('dialog.stylearea.color').value = 'FF000000';
                this.setSelect('dialog.stylearea.units', 'Points');
                this.thicknessChanged();
                this.colorChanged();
                this.unitsChanged();
            }
            edge.set('LineStyle', pattern);
        }
    },
    chooseEdgePattern: function() {
        this.edgePatternPicker.open();
    },
    selectionChanged: function(picker) {
        var pattern = picker.getSelection();
        if (pattern) {
            if (picker == this.fillPatternPicker) {
                this.setFillPattern(pattern);
            } else {
                this.setEdgePattern(pattern);
            }
        }
        picker.close();
    },
    colorChanged: function() {
        var color = this.dialog.getObj('dialog.stylearea.color').value;
        if (color != '') {
            this._rule.get('AreaSymbolization2D').get('Stroke').set('Color', color);
            color = '#' + color.substr(2);
        }
        this.dialog.getObj('dialog.stylearea.colorpreview').style.backgroundColor = color;
    },
    fgColorChanged: function() {
        var color = this.dialog.getObj('dialog.stylearea.fgcolor').value;
        if (color != '') {
            this._rule.get('AreaSymbolization2D').get('Fill').set('ForegroundColor', color);
            color = '#' + color.substr(2);
        }
        this.dialog.getObj('dialog.stylearea.fgcolorpreview').style.backgroundColor = color;
    },
    bgColorChanged: function() {
        var color = this.dialog.getObj('dialog.stylearea.bgcolor').value;
        if (color != '') {
            this._rule.get('AreaSymbolization2D').get('Fill').set('BackgroundColor', color);
            color = '#' + color.substr(2);
        }
        this.dialog.getObj('dialog.stylearea.bgcolorpreview').style.backgroundColor = color;
    },
    thicknessChanged: function() {
        this._rule.get('AreaSymbolization2D').get('Stroke').set('Thickness', this.dialog.getObj('dialog.stylearea.thickness').value);
    },
    unitsChanged: function() {
        var edge = this._rule.get('AreaSymbolization2D').get('Stroke')
        var currentUnit = edge.get('Unit');
        var from = this.getConversionForUnit(currentUnit);
        var newUnit = this.getSelect('dialog.stylearea.units');
        var to = this.getConversionForUnit(newUnit);
        var t = this.dialog.getObj('dialog.stylearea.thickness').value;
        var newValue = parseFloat(t) * to.unitsPerPixel / from.unitsPerPixel;
        this.dialog.getObj('dialog.stylearea.thickness').value = newValue;
        edge.set('Thickness', newValue);
        edge.set('Unit', newUnit);
    },
    draw: function() {
        if (!this.fillPatternPicker) {
            this.fillPatternPicker = new JxPicker(this.dialog.getObj('dialog.stylearea.fillpattern'));
            this.fillPatternPicker.addSelectionListener(this);
            this.fillPatternPicker.add('No Fill', 'None', '', false);
            var patterns = ['Acad_iso02w100','Acad_iso03w100','Acad_iso04w100','Acad_iso05w100',
            'Acad_iso06w100','Acad_iso07w100','Acad_iso08w100','Acad_iso09w100',
            'Acad_iso10w100','Acad_iso11w100','Acad_iso12w100','Acad_iso13w100',
            'Acad_iso14w100','Acad_iso15w100','Angle','Ansi31','Ansi32','Ansi33',
            'Ansi34','Ansi35','Ansi36','Ansi37','Ansi38','Ar_b816','Ar_b816c',
            'Ar_b88','Ar_brelm','Ar_brstd','Ar_conc','Ar_hbone','Ar_parq1','Ar_rroof',
            'Ar_rroof_90','Ar_rshke','Ar_rshke_180','Ar_rshke_270','Ar_rshke_90',
            'Ar_sand','Box','Brass','Brick','Brstone','Clay','Cork','Cross','Dash',
            'Dolmit','Dots','Earth','Escher','Flex','Grass','Grate','Hex',
            'Honey','Hound','Insul','Line','Line_135','Line_45','Line_90',
            'Mudst','Net','Net3','Net_45','Plast','Plasti','Sacncr','Shapefill12',
            'Shapefill13','Shapefill14','Shapefill15','Shapefill16','Shapefill17',
            'Shapefill18','Shapefill19','Shapefill20','Shapefill21','Shapefill22',
            'Shapefill23','Shapefill24','Shapefill25','Shapefill26','Shapefill27',
            'Shapefill28','Shapefill29','Shapefill30','Shapefill31','Shapefill32',
            'Shapefill33','Shapefill34','Shapefill35','Shapefill36','Shapefill37',
            'Shapefill38','Shapefill39','Shapefill40','Shapefill41','Shapefill42',
            'Shapefill43','Shapefill44','Shapefill45','Shapefill46','Shapefill47',
            'Shapefill48','Shapefill49','Shapefill50','Shapefill51','Shapefill52',
            'Shapefill53','Shapefill54','Shapefill55','Shapefill56','Shapefill57',
            'Shapefill58','Shapefill59','Shapefill60','Shapefill61','Shapefill62',
            'Shapefill63','Shapefill64','Shapefill65','Shapefill66','Shapefill67',
            'Shapefill68','Shapefill69','Shapefill70','Shapefill71','Solid',
            'Square','Stars','Steel','Swamp','Trans','Triang','Zigzag'];
            for (var i=0; i<patterns.length; i++) {
                this.fillPatternPicker.add(patterns[i], patterns[i], 'url(images/areas/'+patterns[i]+'.png)', false);
            }
        }
        
        if (!this.edgePatternPicker) {
            this.edgePatternPicker = new JxPicker(this.dialog.getObj('dialog.stylearea.edgepattern'));
            this.edgePatternPicker.addSelectionListener(this);
            this.edgePatternPicker.add('No Border', 'None', '', false);
            var patterns = ['ACAD_ISO02W100','ACAD_ISO03W100','ACAD_ISO04W100',
            'ACAD_ISO05W100','ACAD_ISO06W100','ACAD_ISO07W100','ACAD_ISO08W100',
            'ACAD_ISO09W100','ACAD_ISO10W100','ACAD_ISO11W100','ACAD_ISO12W100',
            'ACAD_ISO13W100','ACAD_ISO14W100','ACAD_ISO15W100','BORDER','BORDER2',
            'BORDERX2','CENTER','CENTER2','CENTERX2','DASHDOT2','DASHDOT3','DASHDOTX2',
            'DASHED','DASHED2','DASHEDX2','DIVIDE','DIVIDE2','DIVIDEX2','DOT2','DOT3',
            'DOTX2','Dash','DashDot','DashDotDot','Dot','FENCELINE1','FENCELINE2','HIDDEN',
            'HIDDEN2','HIDDENX2','LINE03','LINE04','LINE05','LINE06','LINE07','LINE08',
            'LINE09','LINE10','LINE11','LINE12','LINE13','LINE14','LINE15','LINE16',
            'LINE17','LINE18','LINE19','LINE20','LINE21','LINE22','LINE23','LINE24',
            'LINE25','PHANTOM','PHANTOM2','PHANTOMX2','Rail','Solid','TRACKS'];
            for (var i=0; i<patterns.length; i++) {
                this.edgePatternPicker.add(patterns[i], patterns[i], 'url(images/lines/'+patterns[i]+'.png)', false);
            }
        }
        var as = this._rule.get('AreaSymbolization2D');
        var fill = as.get('Fill');
        if (fill != '') {
            this.setFillPattern(fill.get('FillPattern'));
            var color = fill.get('ForegroundColor');
            this.dialog.getObj('dialog.stylearea.fgcolor').value = color;
            this.dialog.getObj('dialog.stylearea.fgcolorpreview').style.backgroundColor = '#' + color.substr(2);
            color = fill.get('BackgroundColor');
            this.dialog.getObj('dialog.stylearea.bgcolor').value = color;
            this.dialog.getObj('dialog.stylearea.bgcolorpreview').style.backgroundColor = '#' + color.substr(2);
        } else {
            this.setFillPattern('None');
        }
        
        var edge = as.get('Stroke');
        if (edge != '') {
            this.setEdgePattern(edge.get('LineStyle'));
            this.dialog.getObj('dialog.stylearea.thickness').value = edge.get('Thickness');
            color = edge.get('Color');
            this.dialog.getObj('dialog.stylearea.color').value = color;
            this.dialog.getObj('dialog.stylearea.colorpreview').style.backgroundColor = '#' + color.substr(2);
            this.setSelect('dialog.stylearea.units', edge.get('Unit'));
        } else {
            this.setEdgePattern('None');
        }

    },
    updateRule: function() {
       this.rule.set('AreaSymbolization2D', this._rule.get('AreaSymbolization2D'));
    },
    getSelect: function(name) {
        var select = this.dialog.getObj(name);
        return select.options[select.selectedIndex].value;
    },
    setSelect: function(name, value) {
        var select = this.dialog.getObj(name);
        for (var i=0; i<select.options.length; i++) {
            if (select.options[i].value == value) {
                select.selectedIndex = i;
                break;
            }
        }
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
    }
});

var WSStyleLabelDialog = Class.create();
Object.extend(WSStyleLabelDialog.prototype, {
    dialog: null,
    handler: null,
    app: null,
    currentRule: null,
    initialize: function( options ) {
        this.handler = options.handler;
        this.onContentLoadedCallback = options.onContentLoaded || null;
        this.app = options.app;
        var size = Position.getPageDimensions();
        var o = {
            title: 'Style Label',
            id: 'stylelabeldialog',
            contentURL : 'dialogs/stylelabel.html',
            helpURL : 'help/dialog_stylelabel.html',
            width: 500,
            height: 525,
            top: (size.height-525)/2,
            left: (size.width-500)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.dialog = new JxDialog( o );
        this.bContentLoaded = false;
        this.bIsOpen = false;
        
        this.expressionDialog = new WSExpressionBuilderDialog({app:this.app, handler:this.handleExpression.bind(this)});
    },
    onContentLoaded: function() {
        this.dialog.registerIds(['dialog.stylelabel.property',
                                 'dialog.stylelabel.chooseproperty',
                                 'dialog.stylelabel.font',
                                 'dialog.stylelabel.size',
                                 'dialog.stylelabel.choosesize',
                                 'dialog.stylelabel.choosesize',
                                 'dialog.stylelabel.bold',
                                 'dialog.stylelabel.italic',
                                 'dialog.stylelabel.underline',
                                 'dialog.stylelabel.color',
                                 'dialog.stylelabel.colorpreview',
                                 'dialog.stylelabel.choosecolor', /* disabled until we get a color picker */
                                 'dialog.stylelabel.bgstyle',
                                 'dialog.stylelabel.bgcolor',
                                 'dialog.stylelabel.bgcolorpreview',
                                 'dialog.stylelabel.choosebgcolor', /* disabled until we get a color picker */
                                 'dialog.stylelabel.fixedlabels',
                                 'dialog.stylelabel.allowobscure',
                                 'dialog.stylelabel.horizontal',
                                 'dialog.stylelabel.choosehorizontal',
                                 'dialog.stylelabel.vertical',
                                 'dialog.stylelabel.choosevertical',
                                 'dialog.stylelabel.rotation',
                                 'dialog.stylelabel.chooserotation',
                                 'dialog.stylelabel.context', /* disabled */
                                 'dialog.stylelabel.units'], this.dialog.domObj);
                                 
        Event.observe(this.dialog.getObj('dialog.stylelabel.property'), 'change', this.propertyChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.chooseproperty'), 'click', this.chooseProperty.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.font'), 'change', this.fontChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.size'), 'change', this.sizeChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.choosesize'), 'click', this.chooseSize.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.bold'), 'change', this.boldChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.italic'), 'change', this.italicChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.underline'), 'change', this.underlineChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.color'), 'change', this.colorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.bgstyle'), 'change', this.bgStyleChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.bgcolor'), 'change', this.bgColorChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.fixedlabels'), 'change', this.fixedLabelsChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.allowobscure'), 'change', this.allowObscureChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.horizontal'), 'change', this.horizontalChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.vertical'), 'change', this.verticalChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.rotation'), 'change', this.rotationChanged.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.choosevertical'), 'click', this.chooseVertical.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.choosehorizontal'), 'click', this.chooseHorizontal.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.chooserotation'), 'click', this.chooseRotation.bind(this));
        Event.observe(this.dialog.getObj('dialog.stylelabel.units'), 'change', this.unitsChanged.bind(this));
        
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
        this.bContentLoaded = true;
        if (this.bIsOpen) {
            this.draw();
        }
    },
    open: function(rule, properties) {
        this.rule = rule;
        this._rule = rule.clone();
        this._rule._parent = rule.parent;
        this.properties = properties;
        this.dialog.open();
    },
    close: function() {
        this.bIsOpen = false;
        var property = this.dialog.getObj('dialog.stylelabel.property');
        while(property.options.length > 0) {
            property.options[0] = null;
        }
        
        this.dialog.close();
    },
    handleDialog: function(button, dialog) {
        /* all other buttons are handled elsewhere */
        if (button == 'ok' || button == 'cancel') {
            if (this.handler) {
                this.handler(button,this);
            }
        }
    },
    onOpen: function() {
        this.bIsOpen = true;
        if (this.bContentLoaded) {
            this.draw();
        }
    },
    propertyChanged: function() {
        var label = this._rule.get('Label');
        var property = this.getSelect('dialog.stylelabel.property');
        if (property == '') {
            this._rule.set('Label', '');
            this.draw();
        } else {
            if (label == '') {
                label = new WSTextSymbolType();
                label.set('Text', property);
                label.set('FontName', 'Arial');
                label.set('ForegroundColor', 'FF000000');
                label.set('BackgroundColor', 'FFFFFFFF');
                label.set('BackgroundStyle', 'Ghosted');
                label.set('SizeContext', 'DeviceUnits');
                label.set('Unit', 'Points');
                label.set('SizeX', '0.3');
                label.set('SizeY', '10');
                this._rule.set('Label', label);
                this.draw();
            } else {
                this._rule.get('Label').set('Text', property);
            }
        }
    },
    chooseProperty: function() {
        this.expressionDialog.target = 'dialog.stylelabel.property';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylelabel.property').value);
    },
    fontChanged: function() {
        this._rule.get('Label').set('FontName',
                    this.dialog.getObj('dialog.stylelabel.font').value);
    },
    sizeChanged: function() {
        this._rule.get('Label').set('SizeY',
                    this.dialog.getObj('dialog.stylelabel.size').value);
    },
    chooseSize: function() {
        this.expressionDialog.target = 'dialog.stylelabel.size';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylelabel.size').value);
    },
    chooseRotation: function() {
        this.expressionDialog.target = 'dialog.stylelabel.rotation';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylelabel.rotation').value);
    },
    chooseVertical: function() {
        this.expressionDialog.target = 'dialog.stylelabel.vertical';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylelabel.vertical').value);
    },
    chooseHorizontal: function() {
        this.expressionDialog.target = 'dialog.stylelabel.horizontal';
        this.expressionDialog.open(this.properties,
             this.dialog.getObj('dialog.stylelabel.horizontal').value);
    },
    handleExpression: function(button, dialog) {
        if (button == 'ok') {
            this.dialog.getObj(dialog.target).value = dialog.getExpression();
            if (dialog.target == 'dialog.stylelabel.size') {
                this.dialog.getObj(dialog.target).value = dialog.getExpression();
                this.sizeChanged();
            } else if (dialog.target == 'dialog.stylelabel.rotation') {
                this.dialog.getObj(dialog.target).value = dialog.getExpression();
                this.rotationChanged();
            } else if (dialog.target == 'dialog.stylelabel.property') {
                var sel = this.dialog.getObj(dialog.target);
                var exp = dialog.getExpression();
                sel.options[sel.options.length] = new Option(exp, exp);
                this.setSelect(dialog.target, exp);
                this.propertyChanged();
            } else if (dialog.target == 'dialog.stylelabel.vertical') {
                var sel = this.dialog.getObj(dialog.target);
                var exp = dialog.getExpression();
                sel.options[sel.options.length] = new Option(exp, exp);
                this.setSelect(dialog.target, exp);
                this.verticalChanged();
            } else if (dialog.target == 'dialog.stylelabel.horizontal') {
                var sel = this.dialog.getObj(dialog.target);
                var exp = dialog.getExpression();
                sel.options[sel.options.length] = new Option(exp, exp);
                this.setSelect(dialog.target, exp);
                this.horizontalChanged();
            }
        }
        dialog.close();
    },
    boldChanged: function() {
        this._rule.get('Label').set('Bold', 
                    this.dialog.getObj('dialog.stylelabel.bold').checked ? "true" : '');
    },                                               
    italicChanged: function() {                        
        this._rule.get('Label').set('Italic',
                    this.dialog.getObj('dialog.stylelabel.italic').checked ? "true" : '');
    },                                               
    underlineChanged: function() {                        
        this._rule.get('Label').set('Underlined',
                    this.dialog.getObj('dialog.stylelabel.underline').checked ? "true" : '');
    },                                               
    colorChanged: function() {
        var color = this.dialog.getObj('dialog.stylelabel.color').value;
        this.dialog.getObj('dialog.stylelabel.colorpreview').style.backgroundColor = '#'+color.substr(2);
        this._rule.get('Label').set('ForegroundColor', color);
    },
    bgStyleChanged: function() {                        
        this._rule.get('Label').set('BackgroundStyle',
                                this.getSelect('dialog.stylelabel.bgstyle'));
    },                                               
    bgColorChanged: function() {
        var color = this.dialog.getObj('dialog.stylelabel.bgcolor').value;
        this.dialog.getObj('dialog.stylelabel.bgcolorpreview').style.backgroundColor = '#'+color.substr(2);
        this._rule.get('Label').set('BackgroundColor', color);
    },
    fixedLabelsChanged: function() {
        var b = this.dialog.getObj('dialog.stylelabel.fixedlabels').checked;
        this.dialog.getObj('dialog.stylelabel.horizontal').disabled = !b;
        this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = !b;            
        this.dialog.getObj('dialog.stylelabel.vertical').disabled = !b;
        this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = !b;            
    },
    allowObscureChanged: function() {
        var b = this.dialog.getObj('dialog.stylelabel.allowobscure').checked;
    },
    horizontalChanged: function() {                        
        this._rule.get('Label').set('HorizontalAlignment', this.dialog.getObj('dialog.stylelabel.horizontal'));
    },                                               
    verticalChanged: function() {                        
        this._rule.get('Label').set('VerticalAlignment', this.dialog.getObj('dialog.stylelabel.vertical'));
    },                                               
    rotationChanged: function() {                        
        this._rule.get('Label').set('Rotation', this.dialog.getObj('dialog.stylelabel.rotation'));
    },                                               
    unitsChanged: function() {
        var label = this._rule.get('Label');
        var currentUnit = label.get('Unit');
        var from = this.getConversionForUnit(currentUnit);
        var newUnit = this.getSelect('dialog.stylelabel.units');
        var to = this.getConversionForUnit(newUnit);
        var t = this.dialog.getObj('dialog.stylelabel.size').value;
        var newValue = parseFloat(t) * to.unitsPerPixel / from.unitsPerPixel;
        this.dialog.getObj('dialog.stylelabel.size').value = newValue;
        label.set('Size', newValue);
        label.set('Unit', newUnit);
    },
    draw: function() {
        var property = this.dialog.getObj('dialog.stylelabel.property');
        if (property.options.length == 0) {
            property.options[0] = new Option('None', '');
            for (var i=0; i<this.properties.length; i++) {
                var prop = this.properties[i];
                property.options[property.options.length] = new Option(prop, prop);
            }
        }
        
        var label = this._rule.get('Label');
        var ids = ['dialog.stylelabel.font',
                   'dialog.stylelabel.size',
                   'dialog.stylelabel.choosesize',
                   'dialog.stylelabel.choosesize',
                   'dialog.stylelabel.bold',
                   'dialog.stylelabel.italic',
                   'dialog.stylelabel.underline',
                   'dialog.stylelabel.color',
                   'dialog.stylelabel.colorpreview',
                   /*'dialog.stylelabel.choosecolor',  disabled until we get a color picker */
                   'dialog.stylelabel.bgstyle',
                   'dialog.stylelabel.bgcolor',
                   'dialog.stylelabel.bgcolorpreview',
                   /*'dialog.stylelabel.choosebgcolor',  disabled until we get a color picker */
                   'dialog.stylelabel.rotation',
                   'dialog.stylelabel.chooserotation',
                   'dialog.stylelabel.units'];
        if (label == '') {
            for (var i=0; i<ids.length; i++) {
                this.dialog.getObj(ids[i]).disabled = true;
            }
            this.dialog.getObj('dialog.stylelabel.fixedlabels').disabled = true;
            this.dialog.getObj('dialog.stylelabel.allowobscure').disabled = true;
            this.dialog.getObj('dialog.stylelabel.horizontal').disabled = true;
            this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = true;
            this.dialog.getObj('dialog.stylelabel.vertical').disabled = true;
            this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = true;
        } else {
            for (var i=0; i<ids.length; i++) {
                this.dialog.getObj(ids[i]).disabled = false;
            }
            this.setSelect('dialog.stylelabel.property', label.get('Text'));
            this.dialog.getObj('dialog.stylelabel.font').value = label.get('FontName');
            this.dialog.getObj('dialog.stylelabel.size').value = label.get('SizeY');
            this.dialog.getObj('dialog.stylelabel.bold').checked = (label.get('Bold') == 'true');
            this.dialog.getObj('dialog.stylelabel.italic').checked = (label.get('Italic') == 'true');
            this.dialog.getObj('dialog.stylelabel.underline').checked = (label.get('Underlined') == 'true');
            var color = label.get('ForegroundColor');
            this.dialog.getObj('dialog.stylelabel.color').value = color;
            this.dialog.getObj('dialog.stylelabel.colorpreview').style.backgroundColor = '#'+color.substr(2);
            this.setSelect('dialog.stylelabel.bgstyle', label.get('BackgroundStyle'));
            color = label.get('BackgroundColor');
            this.dialog.getObj('dialog.stylelabel.bgcolor').value = color;
            this.dialog.getObj('dialog.stylelabel.bgcolorpreview').style.backgroundColor = '#'+color.substr(2);
            this.setSelect('dialog.stylelabel.horizontal', label.get('HorizontalAlignment'));
            this.setSelect('dialog.stylelabel.vertical', label.get('VerticalAlignment'));
            this.dialog.getObj('dialog.stylelabel.rotation').value = label.get('Rotation');
            this.setSelect('dialog.stylelabel.units', label.get('Unit'));
        
            if (this.rule instanceof WSPointRuleType) {
                this.dialog.getObj('dialog.stylelabel.fixedlabels').disabled = false;
                this.dialog.getObj('dialog.stylelabel.allowobscure').disabled = false;
                if (this.rule.parent.get('DisplayAsText') == 'true') {
                    this.dialog.getObj('dialog.stylelabel.horizontal').disabled = false;
                    this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = false;
                    this.dialog.getObj('dialog.stylelabel.vertical').disabled = false;
                    this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = false;  
                    this.dialog.getObj('dialog.stylelabel.fixedlabels').checked = true;
                } else {
                    this.dialog.getObj('dialog.stylelabel.horizontal').disabled = true;
                    this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = true;
                    this.dialog.getObj('dialog.stylelabel.vertical').disabled = true;
                    this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = true;            
                    this.dialog.getObj('dialog.stylelabel.fixedlabels').checked = false;
                }
                if (this.rule.parent.get('AllowOverpost') == 'true') {
                    this.dialog.getObj('dialog.stylelabel.allowobscure').checked = true;
                } else {
                    this.dialog.getObj('dialog.stylelabel.allowobscure').checked = false;
                }
            } else if (this.rule instanceof WSLineRuleType) {
                this.dialog.getObj('dialog.stylelabel.fixedlabels').disabled = true;
                this.dialog.getObj('dialog.stylelabel.allowobscure').disabled = true;
                this.dialog.getObj('dialog.stylelabel.horizontal').disabled = true;
                this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = true;
                this.dialog.getObj('dialog.stylelabel.vertical').disabled = false;
                this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = false;
            } else {
                this.dialog.getObj('dialog.stylelabel.fixedlabels').disabled = true;
                this.dialog.getObj('dialog.stylelabel.allowobscure').disabled = true;
                this.dialog.getObj('dialog.stylelabel.horizontal').disabled = true;
                this.dialog.getObj('dialog.stylelabel.choosehorizontal').disabled = true;
                this.dialog.getObj('dialog.stylelabel.vertical').disabled = true;
                this.dialog.getObj('dialog.stylelabel.choosevertical').disabled = true;
            }
        }
    },
    updateRule: function() {
       this.rule.set('Label', this._rule.get('Label'));
       var b = this.dialog.getObj('dialog.stylelabel.fixedlabels').checked;
       this.rule.parent.set('DisplayAsText', b?'true':'');
       b = this.dialog.getObj('dialog.stylelabel.allowobscure').checked;
       this.rule.parent.set('AllowOverpost', b?'true':'');
    },
    getSelect: function(name) {
        var select = this.dialog.getObj(name);
        return select.options[select.selectedIndex].value;
    },
    setSelect: function(name, value) {
        var select = this.dialog.getObj(name);
        for (var i=0; i<select.options.length; i++) {
            if (select.options[i].value == value) {
                select.selectedIndex = i;
                break;
            }
        }
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
    }
});


var WSExpressionBuilderDialog = Class.create();
Object.extend(WSExpressionBuilderDialog.prototype, {
    dialog: null,
    handler: null,
    app: null,
    initialize: function( options ) {
        this.handler = options.handler;
        this.onContentLoadedCallback = options.onContentLoaded || null;
        this.app = options.app;
        var size = Position.getPageDimensions();
        var o = {
            title: 'ExpressionBuilder',
            id: 'expressionbuilderdialog',
            contentURL : 'dialogs/expressionbuilder.html',
            helpURL : 'help/dialog_expressionbuilder.html',
            width: 500,
            height: 300,
            top: (size.height-300)/2,
            left: (size.width-500)/2,
            onOpen: this.onOpen.bind(this),
            handler: this.handleDialog.bind(this),
            onContentLoaded: this.onContentLoaded.bind(this),
            buttons: ['ok','cancel'],
            resizeable: true
        };
        this.dialog = new JxDialog( o );
        this.bContentLoaded = false;
        this.bIsOpen = false;
    },
    onContentLoaded: function() {
        this.dialog.registerIds(['dialog.expression.property',
                                 'dialog.expression.expression'
                                 ], this.dialog.domObj);
                                 
        if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
        this.bContentLoaded = true;
        if (this.bIsOpen) {
            this.draw();
        }
    },
    open: function(properties, expression) {
        this.properties = properties;
        this.expression = expression;
        this.dialog.open();
    },
    close: function() {
        this.bIsOpen = false;
        var property = this.dialog.getObj('dialog.expression.property');
        while(property.options.length > 0) {
            property.options[0] = null;
        }
        
        this.dialog.close();
    },
    handleDialog: function(button, dialog) {
        /* all other buttons are handled elsewhere */
        if (button == 'ok' || button == 'cancel') {
            if (this.handler) {
                this.handler(button,this);
            }
        }
    },
    onOpen: function() {
        this.bIsOpen = true;
        if (this.bContentLoaded) {
            this.draw();
        }
    },
    draw: function() {
        var property = this.dialog.getObj('dialog.expression.property');
        if (property.options.length == 0) {
            //property.options[0] = new Option('None', '');
            for (var i=0; i<this.properties.length; i++) {
                var prop = this.properties[i];
                property.options[property.options.length] = new Option(prop, prop);
            }
        }
        this.dialog.getObj('dialog.expression.expression').value = this.expression;
    },
    getExpression: function() {
       return this.dialog.getObj('dialog.expression.expression').value;
    }
});

/**
 * WSCoordinateSystemDialog
 *
 * The coordinate system dialog allows the user to select a coordinate system
 * from those available.
 */
 var WSCoordinateSystemDialog = Class.create();
 Object.extend(WSCoordinateSystemDialog.prototype, {
     dialog: null,
     handler: null,
     app: null,
     initialize: function( options ) {
         this.handler = options.handler;
         this.onContentLoadedCallback = options.onContentLoaded || null;
         this.app = options.app;
         var size = Position.getPageDimensions();
         var o = {
             title: 'Coordinate System Selector',
             id: 'coordinatesystemdialog',
             contentURL : 'dialogs/coordinatesystem.html',
             helpURL : 'help/dialog_coordinatesystem.html',
             width: 500,
             height: 300,
             top: (size.height-300)/2,
             left: (size.width-500)/2,
             onOpen: this.onOpen.bind(this),
             handler: this.handleDialog.bind(this),
             onContentLoaded: this.onContentLoaded.bind(this),
             buttons: ['ok','cancel'],
             resizeable: true
         };
         this.dialog = new JxDialog( o );
         this.bContentLoaded = false;
         this.bIsOpen = false;
     },
     onContentLoaded: function() {
         this.dialog.registerIds(['dialog.coordinate.coordinatelist'
                                  ], this.dialog.domObj);

         if (this.onContentLoadedCallback) { this.onContentLoadedCallback(this); }
         this.bContentLoaded = true;
         var oList = this.dialog.getObj('dialog.coordinate.coordinatelist');
         Event.observe(oList, 'change', this.coordinateSystemChanged.bind(this));
         var aProjections = this.app.getCoordinateSystems();
         for (var i=0; i<aProjections.length; i++) {
             var proj = aProjections[i];
             var opt = new Option('(epsg:'+proj.epsg+') ' + proj.description, proj.wkt);
             oList.options[i] = opt;
         }
         if (this.bIsOpen) {
             this.draw();
         }
     },
     open: function(currentWKT) {
         this._szCurrentWKT = currentWKT;
         this.dialog.open();
     },
     close: function() {
         this.bIsOpen = false;
         this.dialog.close();
     },
     handleDialog: function(button, dialog) {
         /* all other buttons are handled elsewhere */
         if (button == 'ok' || button == 'cancel') {
             if (this.handler) {
                 this.handler(button,this);
             }
         }
     },
     onOpen: function() {
         this.bIsOpen = true;
         if (this.bContentLoaded) {
             this.draw();
         }
     },
     draw: function() {
         var oList = this.dialog.getObj('dialog.coordinate.coordinatelist');
         
         for (var i=0; i<oList.options.length; i++) {
             if (oList.options[i].value == this.currentWKT) {
                 oList.selectedIndex = i;
                 break;
             }
         }
     },
     coordinateSystemChanged: function() {
         var oList = this.dialog.getObj('dialog.coordinate.coordinatelist');
         this.currentWKT = oList.options[oList.selectedIndex].value;
     },
     getWKT: function() {
        return this.currentWKT;
     }
 });