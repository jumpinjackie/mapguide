/**
 * @project         Jx
 * @revision        $Id: jxmenu.js 409 2007-09-13 17:23:39Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

 Jx.addStyleSheet('menu/menu.css');
 Jx.addStyleSheet('button/button.css');


Jx.MenuItem = Class.create();
Object.extend(Jx.MenuItem.prototype, Jx.Listener.prototype);
Object.extend(Jx.MenuItem.prototype, {
    al: null,
    domObj: null,
    parent: null,
    enabled: false,
    button: null,
    initialize: function(action, options) {
        this.initializeItem(options);
        action.bindTo(this);
        this.propertyChanged(action);
    },
    initializeItem: function(options) { 
        if (!options.image) {
            options.image = Jx.baseURL + 'images/a_pixel.png';
        }
        if (!options.label) {
            options.label = '&nbsp;';
        }
        this.label = options.label || '&nbsp;';
        this.image = options.image || null;
        
        this.al = [];
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxMenuItem';
        
        /* menu items are buttons without the jxButton class */
        var action = new Jx.Action(this.processActionEvent.bindAsEventListener(this))
        this.button = new Jx.Button(action, options);
        //Element.removeClassName(this.button.domA, 'jxButton');
        
        Event.observe(this.button.domObj, 'mouseover', this.onmouseover.bindAsEventListener(this), true);
        
        this.domObj.appendChild(this.button.domObj);
    },
    setParent: function(o) {
        this.parent = o;
    },
    hide: function() {},
    show: function() {},
    addActionListener: function(o) { this.addListener(this.al,o); },
    removeActionListener : function(o) { this.removeListener(this.al, o); },
    processActionEvent: function(e) { 
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this);
            if (this.parent && this.parent.deactivate) {
                this.parent.deactivate(e);
            }
        }
    },
    propertyChanged: function(o) {
        this.enabled = o.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj.childNodes[0].childNodes[0], 'jxDisabled' );
        } else {
            Element.addClassName( this.domObj.childNodes[0].childNodes[0], 'jxDisabled' );
        }
    },
    onmouseover: function(e) {
        var target = Event.element(e);
        if (this.parent && this.parent.setVisibleItem) {
            this.parent.setVisibleItem(this);
        }
        this.show();
    }
});

Jx.MenuSeparator = Class.create();
Object.extend(Jx.MenuSeparator.prototype, {
    domObj: null,
    parent: null,
    initialize: function() {
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxMenuItem';
        var span = document.createElement('span');
        span.className = 'jxMenuSeparator';
        span.innerHTML = '&nbsp;';
        this.domObj.appendChild(span);
    },
    hide: function() {},
    show: function() {},
    setParent: function(o) {
        this.parent = o;
    }
});

Jx.SubMenu = Class.create();
Object.extend(Jx.SubMenu.prototype, Jx.MenuItem.prototype);
Object.extend(Jx.SubMenu.prototype, {
    subMenu: null,
    parent: null,
    visibleItem: null,
    items: null,
    initialize: function(options) { 
        this.open = false;
        this.items = [];
        this.initializeItem(options);
        Element.addClassName(this.domObj.childNodes[0].childNodes[0], 'jxButtonSubMenu');
        
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxSubMenu';
        this.subDomObj.style.display = 'none';
        
        // this.button.domObj.appendChild(this.subDomObj);
        this.domObj.appendChild(this.subDomObj);
    },
    setParent: function(o) {
        this.parent = o;
    },
    show: function() {
        if (this.open || this.items.length == 0) {
            return;
        }

        this.open = true;
        this.subDomObj.style.display = 'block';
        if (!window.opera) {
            this.subDomObj.childNodes[0].appendChild(this.iframe);
            var size = Element.getBorderBoxSize(this.subDomObj);
            this.iframe.style.width = size.width + "px";
            this.iframe.style.height = size.height + "px";
            
        }
        this.setActive(true);
    },
    hide: function() {
        if (!this.open) {
            return;
        }
        this.open = false;
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide();
        }
        this.subDomObj.style.display = 'none';
        if (!window.opera && this.iframe.parentNode) {
            this.subDomObj.childNodes[0].removeChild(this.iframe);
        }
        this.visibleItem = null;
    },
    add : function() { /* menu */
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            this.subDomObj.appendChild(item.domObj);
        }
    },
    insertBefore: function(newItem, targetItem) {
        var bInserted = false;
        for (var i=0; i<this.items.length; i++) {
            if (this.items[i] == targetItem) {
                this.items.splice(i, 0, newItem);
                this.subDomObj.insertBefore(newItem.domObj, targetItem.domObj);
                bInserted = true;
                break;
            }
        }
        if (!bInserted) {
            this.add(newItem);
        }
    },
    remove: function(item) {
        for (var i=0; i<this.items.length; i++) {
            if (this.items[i] == item) {
                this.items.splice(i,1);
                this.subDomObj.removeChild(item.domObj);
                break;
            }
        }
    },
    processActionEvent: function(e) { 
        if (this.open) { 
            this.hide(); 
        } else { 
            this.show();
        }
        return Event.stop(e);
    },
    deactivate: function(e) {
        if (this.parent) {
            this.parent.deactivate(e);            
        }
    },
    isActive: function() { 
        if (this.parent) {
            return this.parent.isActive();
        } else {
            return false;
        }
    },
    setActive: function(b) { 
        if (this.parent && this.parent.setActive) {
            this.parent.setActive(b);
        }
    },
    setVisibleItem: function(o) {
        if (this.visibleItem != o) {
            if (this.visibleItem && this.visibleItem.hide) {
                this.visibleItem.hide();
            }
            this.visibleItem = o;
            this.visibleItem.show();
        }
    }
});

Jx.Menu = Class.create();
Jx.Menu.prototype = {
    domObj : null,
    buttonObj : null,
    subDomObj : null,
    items : null,
    menus : [],
    initialize : function(options) {
        /* stores menu items and sub menus */
        this.items = [];
        
        /* iframe shim to prevent scrollbars and 
           inputs from showing through the menu */
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        /* the DOM element that holds the actual menu */
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxMenu';
        this.subDomObj.style.display = 'none';
        
        /* if options are passed, make a button inside an LI so the
           menu can be embedded inside a toolbar */
        if (options) {
            this.domObj = document.createElement('li');
            
            var action = new Jx.Action(this.show.bind(this));
            this.button = new Jx.Button(action, options);
            Element.addClassName(this.button.domObj.firstChild, 'jxButtonMenu');
            this.domObj.appendChild(this.button.domObj);
        
            Event.observe(this.domObj, 'mouseover', this.onMouseOver.bindAsEventListener(this));
            //this.button.domObj.appendChild(this.subDomObj);
            this.domObj.appendChild(this.subDomObj);
        }
        
        /* pre-bind the hide function for efficiency */
        this.hideWatcher = this.hide.bindAsEventListener(this);
    },
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            this.subDomObj.appendChild(item.domObj);
        }
    },
    deactivate: function() { this.hide(); },
    actionPerformed : function(o) {this.hide();},
    onMouseOver: function(e) {
        if (this.menus[0] && this.menus[0] != this) {
            this.show(e);
        }
    },
    hide: function(e) {
        if (e) {
            var root = Event.findElement(e, 'LI');
            if (root == this.domObj) {
                return;
            }
        }
        if (this.menus[0] && this.menus[0] == this) {
            this.menus[0] = null;
        }
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide(e);
        }
        Event.stopObserving(document, 'click', this.hideWatcher, true);
        this.subDomObj.style.display = 'none';  
    },
    show : function(e) {
        if (this.menus[0] && this.menus[0] != this) {
            this.menus[0].hide(e);
        }
        if (this.items.length ==0) {
            return;
        }
        this.menus[0] = this;
        this.subDomObj.style.display = 'block';
        this.subDomObj.style.visibility = 'visible';
        
        if (!window.opera) {
            this.subDomObj.childNodes[0].appendChild(this.iframe);
            var size = Element.getContentBoxSize(this.subDomObj);
            this.iframe.style.width = size.width + "px";
            this.iframe.style.height = size.height + "px";
        }
        Event.stop(e);
        /* fix bug in IE that closes the menu as it opens because of bubbling */
        Event.observe(document, 'click', this.hideWatcher, true);
    },
    setVisibleItem: function(o) {
        if (this.visibleItem != o) {
            if (this.visibleItem && this.visibleItem.hide) {
                this.visibleItem.hide();
            }
            this.visibleItem = o;
            this.visibleItem.show();
        }
    }
};

Jx.ContextMenu = Class.create();
Object.extend(Jx.ContextMenu.prototype, Jx.Menu.prototype);
Object.extend(Jx.ContextMenu.prototype, {
    initialize : function(id) {
        Jx.Menu.prototype.initialize.apply(this, []);
        document.getElementsByTagName('BODY')[0].appendChild(this.subDomObj);
        if ($(id)) {
            $(id).oncontextmenu = this.show.bindAsEventListener(this);;
        }
    },
    show : function(e) {
        this.subDomObj.style.left = Event.pointerX(e) + "px";
        this.subDomObj.style.top = Event.pointerY(e) + "px";
        Jx.Menu.prototype.show.apply(this, [e]);
    },
    setVisibleItem: function(o) {
        if (this.visibleItem != o) {
            if (this.visibleItem && this.visibleItem.hide) {
                this.visibleItem.hide();
            }
            this.visibleItem = o;
            this.visibleItem.show();
        }    
    }
});