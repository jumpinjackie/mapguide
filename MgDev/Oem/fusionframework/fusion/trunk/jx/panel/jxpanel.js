/**
 * @project         Jx
 * @revision        $Id: jxpanel.js 432 2007-10-11 20:29:53Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */
 Jx.addStyleSheet('panel/panel.css');

Jx.PanelManager = Class.create();
Jx.PanelManager.prototype = {
    panels: null,
    height: null,
    
    initialize: function(domObj, panels) {
        this.domObj = $(domObj);
        this.panels = panels;
        var d = document.createElement('div');
        d.style.position = 'absolute';
        new Jx.Layout(d, {minHeight:0,maxHeight:0,height:0});
        var elements = [d];
        for (var i=0; i<this.panels.length; i++) {
            elements.push(this.panels[i].domObj);
        }
        this.splitter = new Jx.Splitter(this.domObj, {splitInto: panels.length+1,
                                                     layout: 'vertical',
                                                     elements: elements });
        for (var i=0; i<this.panels.length; i++) {
            this.splitter.bars[i].appendChild(panels[i].title);
            this.splitter.bars[i].style.height = Element.getBorderBoxSize(panels[i].title).height + 'px';
            Element.removeClassName(this.splitter.bars[i], 'jxSplitterBar');
            Element.addClassName(this.splitter.bars[i], 'jxPanelBar');
            panels[i].manager = this;
        }
    },
    
    /**
     * Maximize the panel, taking up all available space (taking into
     * consideration any minimum or maximum values)
     */
    maximizePanel: function(panel) {
        var h = Element.getContentBoxSize(this.domObj).height;
        
        var t = 0;
        for (var i=1; i<this.splitter.elements.length; i++) {
            var p = this.splitter.elements[i];
            t += Element.getBorderBoxSize(p.leftBar).height;
            if (p !== panel.domObj) {
                p.jxLayout.resize({top: t, height: p.jxLayout.options.minHeight, bottom: null});                    
                t += p.jxLayout.options.minHeight;
                p.rightBar.style.top = t + 'px';
            } else {
                break;
            }
        }
        
        b = h;
        for (var i=this.splitter.elements.length - 1; i > 0; i--) {
            p = this.splitter.elements[i];
            if (p !== panel.domObj) {
                b -= p.jxLayout.options.minHeight;
                p.jxLayout.resize({top: b, height: p.jxLayout.options.minHeight, bottom: null});
                b -= Element.getBorderBoxSize(p.leftBar).height;
                p.leftBar.style.top = b + 'px';
                
            } else {
                break;
            }
        }
        panel.domObj.jxLayout.resize({top: t, height:b - t, bottom: null});
    }
};
Jx.Panel = Class.create();
/**
 * Jx.Panel
 */
Jx.Panel.prototype = {
    /** the DOM object that holds the label in the title bar. */
    labelObj : null,
    /** the DOM object that holds the button objects in the title bar. */
    // buttonObj : null,
    /** the state of this panel */
    state : 'open',
    /* track the busy state of this panel - used to control a 'loading' image */
    busyCount : null,
    bContentReady : null,
    onContentReady : null,
    
    /** 
     * @constructor
     * Initialize a new Jx.Panel instance
     *
     * Options:
     *
     * label - String, the title of the Jx Panel
     * toolbar - element to use as the toolbar
     * menubar - element to use as the menu
     * content - element to use as the content. A content area is created
     *           if none is provided.  Otherwise, the content element is moved
     *           in the DOM
     * statusbar - element to use as the statusbar
     * helpCallback - function to call when the user clicks the contextual help button
     * state - initial state of the panel (open or closed)
     */
    initialize : function(options){
        //console.log("Jx.Panel::initialize('"+options.label+"')");
        this.initUniqueId();
        
        /* set up the title object */
        this.title = document.createElement('div');
        this.title.className = "jxPanelTitle";
        //TODO: Opera is broken because it doesn't report the height of the
        //title bars at all unless set through javascript
        //this is a hack until we can figure out from css what the height is
        this.title.style.height = '22px';
        
        this.labelObj = document.createElement('span');
        this.labelObj.className = 'jxPanelLabel';
        this.labelObj.innerHTML = options.label?options.label:'empty';
    
        this.imageBaseUrl = options.imageBaseUrl || Jx.baseURL + 'images/';
        
        var a, img;
        if (options.helpCallback) {
            a = document.createElement('a');
            a.className = 'jxPanelHelp';
            a.href = 'javascript:void(0)';
            Event.observe(a, 'click', options.helpCallback);
            img = document.createElement('img');
            img.src = this.imageBaseUrl + "help.png";
            img.alt = 'Help on this panel';
            img.title = 'Help on this panel';
            a.appendChild(img);
            this.title.appendChild(a);
        }
    
        a = document.createElement('a');
        a.className = 'jxPanelMaximize';
        a.href = 'javascript:void(0)';
        Event.observe(a, 'click', this.maximize.bindAsEventListener(this));
        img = document.createElement('img');
        img.src = this.imageBaseUrl + "maximize.png";
        img.alt = 'Maximize Panel';
        img.title = 'Maximize Panel';
        a.appendChild(img);
        this.title.appendChild(a);
    
        a = document.createElement('a');
        a.className = 'jxPanelLoading';
        a.href = 'javascript:void(0)';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + "loading.gif";
        img.alt = 'Loading Panel';
        img.title = 'Loading Panel';
        a.appendChild(img);
        this.loadingObj = {};
        this.loadingObj.link = a;
        this.loadingObj.img = img;
        this.title.appendChild(this.loadingObj.link);
        this.title.appendChild(this.labelObj);
        
        Event.observe(this.title, 'dblclick', this.maximize.bindAsEventListener(this));
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxPanel';
        
        this.jxLayout = new Jx.Layout(this.domObj, options.constraint || {});
        this.jxLayout.addSizeChangeListener(this);
        
        var top = 0;
        var bottom = 0;
        if (options.menubar) {
            this.menubar = options.menubar;
            this.domObj.appendChild(options.menubar);
            var h = Element.getBorderBoxSize(options.menubar).height;
            new Jx.Layout(this.menubar, {top: top, height:h});
            top += h;
        }
        if (options.toolbar) {
            this.toolbar = options.toolbar;
            this.domObj.appendChild(options.toolbar);
            var h = Element.getBorderBoxSize(options.toolbar).height;
            new Jx.Layout(this.toolbar, {top:top, height: h});
        }
        
        if (options.statusbar) {
            this.statusbar = options.statusbar;
            this.domObj.appendChild(options.statusbar);
            var h = Element.getBorderBoxSize(options.statusbar).height;
            new Jx.Layout(this.statusbar, {bottom: bottom, height: h, top: null});
            bottom += h;
        }
        this.content = document.createElement('div');
        Element.addClassName(this.content, 'jxPanelContent');
        new Jx.Layout(this.content, {top: top, bottom: bottom});
        this.domObj.appendChild(this.content);
        this.loadContent(this.content, options);
        
        this.busyCount = 0;
        this.bContentReady = false;
    },
    setLabel: function(s) {
        this.labelObj.innerHTML = s;
    },
    getLabel: function() {
        return this.labelObj.innerHTML;
    },
    finalize: function() {
        this.domObj = null;
        this.deregisterIds();
    },
    maximize: function() {
        if (this.manager) {
            this.manager.maximizePanel(this);
        }
    },
    setContent : function (html) {
        //console.log('Jx.Panel::setContent()');
        this.content.innerHTML = html;
        this.bContentReady = true;
    },
    setContentURL : function (url) {
        this.bContentReady = false;
        this.setBusy(true);
        if (arguments[1]) {
            this.onContentReady = arguments[1];
        }
        if (url.indexOf('?') == -1) {
            url = url + '?';
        }
        //var ts = (new Date()).getTime();
        //url = url + 'ts='+ts;
        var opts = { method: 'get',
                     onComplete:this.panelContentLoaded.bind(this),
                     requestHeaders: ['If-Modified-Since', 'Sat, 1 Jan 2000 00:00:00 GMT']};
        var a = new Ajax.Request( url, opts);
    },
    panelContentLoaded: function(r) {
        this.content.innerHTML = r.responseText;
        this.bContentReady = true;
        this.setBusy(false);
        if (this.onContentReady) {
            window.setTimeout(this.onContentReady.bind(this),1);
        }
    },
    setBusy : function(b) {
        this.busyCount += b?1:-1;
        this.loadingObj.img.style.visibility = (this.busyCount>0)?'visible':'hidden';
    },
    sizeChanged: function() {
        var top = 0;
        var bottom = 0;
        if (this.menubar) {
            this.menubar.style.height = '';
            var size = Element.getBorderBoxSize(this.menubar);
            this.menubar.resize({height:size.height});
            top += size.height;
        }
        if (this.toolbar) {
            this.toolbar.style.height = '';
            var size = Element.getBorderBoxSize(this.toolbar);
            this.toolbar.resize({height:size.height});
            top += size.height;
        }
        if (this.statusbar) {
            this.statusbar.style.height = '';
            var size = Element.getBorderBoxSize(this.statusbar);
            this.statusbar.resize({height:size.height});
            bottom += size.height;
        }
        if (top || bottom) {
            this.content.resize({bottom: bottom, top: top});
        }               
    }
};
Object.extend(Jx.Panel.prototype, Jx.UniqueId.prototype);
Object.extend(Jx.Panel.prototype, Jx.ContentLoader.prototype);