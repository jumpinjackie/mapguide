/**
 * @project         Jx
 * @revision        $Id: jxtab.js 457 2007-12-10 21:51:53Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

Jx.addStyleSheet('tab/tabs.css');
Jx.addStyleSheet('tabs/tabs_ie.css', true);

Jx.TabSet = Class.create();
Jx.TabSet.prototype = {
    domObj : null,
    sl: null,
    initialize : function(domObj) {
        this.domObj = $(domObj);
        if (!Element.hasClassName(this.domObj, 'jxTabSetContainer')) {
            Element.addClassName(this.domObj, 'jxTabSetContainer');
        }
        this.sl = [];
    },
    /*
     * NOT REQUIRED ???
     */
    sizeChanged: function() { 
      this.resizeTabBox(); 
    },
    resizeTabBox: function() {
    
        var parentSize = Element.getContentBoxSize(this.domObj.parentNode);
        Element.setBorderBoxSize(this.domObj, {width: parentSize.width, height: parentSize.height});
        // this is a bullshit hack for IE.  We need to set the tab content height
        // for IE when the tabs are in a snap panel, otherwise the tab content
        // doesn't collapse with the panel and no scrollbars appear.  This only
        // affects the height.  In fact, setting the width breaks tab placement
        
        for (var i=0; i<this.domObj.childNodes.length; i++) {
            // don't try to set the height on a text node 
            if (this.domObj.childNodes[i].nodeType == 3) {
                 continue;
             }
            Element.setBorderBoxSize(this.domObj.childNodes[i], {height: parentSize.height});
            if (this.domObj.childNodes[i].resize) {
                this.domObj.childNodes[i].resize();
            }
        }
    },
    
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var tab = arguments[i];
            tab.addSelectionListener(this);
            this.domObj.appendChild(tab.content);
            if (!this.activeTab) {
                this.setActiveTab(tab);
            }
        }
    },
    remove : function(tab) {
        //TODO
    },
    setActiveTab: function(tab) {
        if (this.activeTab) {
            Element.removeClassName(this.activeTab.domObj, 'tabActive');
            Element.removeClassName(this.activeTab.content, 'tabContentActive');
        }
        this.activeTab = tab;
        Element.addClassName(this.activeTab.domObj, 'tabActive');
        Element.addClassName(this.activeTab.content, 'tabContentActive');
        if (this.activeTab.content.resize) {
          this.activeTab.content.resize();
        }
    },
    selectionChanged: function(tab) {
        this.setActiveTab(tab);
        this.processEvent(this.sl, 'selectionChanged', tab);
    },
    addSelectionListener: function(o) {
        this.addListener(this.sl, o);
    },
    removeSelectionListener: function(o) {
        this.removeListener(this.sl, o);
    }
};
Object.extend(Jx.TabSet.prototype, Jx.Listener.prototype);

Jx.Tab = Class.create();
Jx.Tab.prototype = {
    domObj: null,
    content: null,
    name: null,
    sl: null,
    initialize : function(name, options) {
        this.sl = [];
        options = options || {};
        this.name = name;
        this.content = document.createElement('div');
        this.content.className = 'tabContent';
        this.loadContent(this.content, options);
        var a = new Jx.Action(this.clicked.bind(this));
        var b = new Jx.Button(a, {label: name});
        this.domObj = b.domA;
        // rename the element from jxButton to jxTab
        // Element.removeClassName(this.domObj, 'jxButton');
        Element.addClassName(this.domObj, 'jxTab');
        new Jx.Layout(this.content, {position: 'relative'});
        //this.content.resize = this.resize.bind(this);
    },
    clicked: function() {
        this.processEvent(this.sl, 'selectionChanged', this);
        this.domObj.childNodes[0].blur();
    },
    addSelectionListener: function(o) {
        this.addListener(this.sl, o);
    },
    removeSelectionListener: function(o) {
        this.removeListener(this.sl, o);
    }
};
Object.extend(Jx.Tab.prototype, Jx.Listener.prototype);
Object.extend(Jx.Tab.prototype, Jx.ContentLoader.prototype);

Jx.TabBox = Class.create();
Jx.TabBox.prototype = {
    panel: null,
    tabBar: null,
    tabSet: null,
    initialize : function(domObj, position) {
        var parent = $(domObj);
        position = position || 'top';
        //this.panel = new Jx.Panel(parent);
        var tabBarDiv = document.createElement('div');
        parent.appendChild(tabBarDiv);
        this.tabBar = new Jx.Toolbar(tabBarDiv, position);
        this.tabSet = new Jx.TabSet(parent);
        switch (position) {
            case 'top':
                Element.addClassName(parent, 'jxTabBoxTop');
                break;
            case 'bottom':
                Element.addClassName(parent, 'jxTabBoxBottom');
                break;
            case 'left':
                Element.addClassName(parent, 'jxTabBoxLeft');
                Element.addClassName(tabBarDiv, 'verticalToolbar');
                break;
            case 'right':
                Element.addClassName(parent, 'jxTabBoxRight');
                Element.addClassName(tabBarDiv, 'verticalToolbar');
                break;
        }
        this.sl = [];
    },
    /* TODO: shouldn't be needed ??? */
    sizeChanged: function() { this.tabSet.sizeChanged(); },
    
    add : function() { 
        this.tabBar.add.apply(this.tabBar, arguments); 
        this.tabSet.add.apply(this.tabSet, arguments); 
    },
    remove : function(tab) { /* TODO */ }
};
Object.extend(Jx.TabBox.prototype, Jx.Listener.prototype);
