/**********************************************************************
 *
 * $Id: jx.js,v 1.106 2006/06/16 20:48:18 zak Exp $
 *
 * purpose: general purpose GUI components based on Prototype and 
 *          scriptaculous.
 *
 * author: Paul Spencer (pspencer@dmsolutions.ca)
 *
 **********************************************************************
 *
 * Copyright (c) 2005, DM Solutions Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************/ 
/**
 *  applyPNGFilter(o)
 *
 *  Applies the PNG Filter Hack for IE browsers when showing 24bit PNG's
 *
 *  var o = object (this png element in the page)
 *
 * The filter is applied using a nifty feature of IE that allows javascript to
 * be executed as part of a CSS style rule - this ensures that the hack only
 * gets applied on IE browsers
 */
function applyPNGFilter(o)  {
   var t="images/a_pixel.png";
   if( o.src != t ) {
       var s=o.src;
       o.src = t;
       o.runtimeStyle.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='"+s+"',sizingMethod='scale')";
   }
};

var jxTraceDiv = null;
var JxCount = 0;

/**
 * trace function for debugging.  Set global jxTraceDiv to be a div
 * in which to dump trace messages and then call this function
 * @param msg {String} the message to write in the trace.
 */
function jxTrace(msg) {
    if (jxTraceDiv || window.console) {
        var d = document.createElement('div');
        if (JxCount%2) {
            d.style.backgroundColor = '#ffc';
        }
        d.innerHTML = JxCount + ":" + msg;
        if (jxTraceDiv) {
            $(jxTraceDiv).insertBefore(d,$(jxTraceDiv).firstChild);
        } else if(window.console) {
            window.console.log(JxCount+":"+msg);
        }
        JxCount ++;
    }
};

/**
 * @class JxListenerBase is a mix-in class that performs common listener functions
 * for objects that support listeners.  It is intended to be added to
 * existing classes using the following syntax:
 *
 * Object.extends( MyClass.prototype, JxListenerBase.prototype)
 *
 * The JxListenerBase class functions provide support for managing a list of
 * listeners (add, remove) and dispatching events to listeners (processEvent)
 *
 * @author Paul Spencer
 */
var JxListenerBase = Class.create();
JxListenerBase.prototype = {
    /**
     * add a listener to the provided list
     * @param {Array} l the array of listeners to add the listener to
     * @param {Object} o the object to add as a listener
     */
    addListener: function (l,o) {l.push(o)},
    /**
     * remove a listener from the provided list
     * @param {Array} l the array of listeners to remove the listener from
     * @param {Object} o the object to remove as a listener
     */
    removeListener: function(l,o) {
        for(var i=0;i<l.length;i++) {if (l[i] == o) {l.splice(i,1); break;}}},
    /**
     * call each listener with a given method and event
     * @param {Array} l the array of listeners to call
     * @param {String} f the name of a function to call on each listener
     * @param {Object} e an object to pass to each listener
     */
    processEvent: function(l,f,e){l.each(function(o){if (o[f]) o[f](e)})}
};

/**
 * JxUniqueId is used to assign unique ids to selected elements
 * This is used to solve a problem where multiple external html
 * fragments are loaded into the DOM via ajax at runtime.  It is
 * not always possible to ensure that every element has a unique
 * id.  This is not a problem if you are using id for CSS styling
 * but if you are using it to access elements using $() then
 * you may get unexpected results.
 * JxUniqueId is a mix-in class.  Extend an existing class to
 * enable it to handle unique ids.  Register the ids that you
 * want to be unique and then get a reference to those objects
 * through the interface exposed by this class
 * The class retrieves the elements by id by walking a dom object
 * and retains references to each of the actual DOM objects
 * you have registered.
 */
var JxUniqueId = Class.create();
JxUniqueId.prototype = {
    uniqueIdRefs: null,
    initUniqueId: function() { 
        //jxTrace('JxUniqueId::initUniqueId()');
        this.uniqueIdRefs = [];
    },
    deregisterIds: function() {
        this.uniqueIdRefs.length = 0;
    },
    registerIds: function (aIds, domObj) {
        //jxTrace('JxUniqueId::registerIds('+aIds.toString()+') domObj id is ' + domObj.id);
        if (aIds.indexOf(domObj.id) != -1) {
            this.uniqueIdRefs[domObj.id] = domObj;
            //jxTrace('JxUniqueId found ' + domObj.id);
        }
        for (var i=0; i<domObj.childNodes.length; i++) {
            this.registerIds(aIds, domObj.childNodes[i]);
        }
    },
    getObj: function(id) {
        return this.uniqueIdRefs[id] || null;
    }
};

var JxEdgeConstraint = Class.create();
/**
 * JxConstraint represents a link between the edges of two DOM elements
 * The links are represented using JxEdge instances and can be used to
 * bind the edge of one element to another element for automating
 * resizing of page elements based on user changes such as page
 * resizing or drag-drop resizing.
 */
JxEdgeConstraint.prototype = {
    from: null,
    to: null,
    a: false,
    initialize : function(from, to) { 
        this.from = from;
        this.to = to;
        JxConstraintManager.add(this);
        this.apply();
        },
    apply: function() { 
            this.to.set(this.from.get());
        }
};

var JxEdge = Class.create();
/**
 * JxEdge represents the edge of a DOM element.  It can be used to get and set
 * the appropriate edge.  It is an abstract class, use a concrete sub-class.
 */
JxEdge.prototype = {
    /* the object this is an edge of */
    of: null,
    /* constructor, shared by sub-classes */
    initialize : function(of){ if (of) {this.of=$(of);}},
    /* internal function to set an edge used by sub-classes */
    _set: function(property, amount) {
        //prevent problems in IE if width or height < 0 due to invalid layout
        amount = Element.getNumber(amount);
        if (property == 'height' || property == 'width') {
            if (amount < 0) {
                amount = 0; 
            }
            var o = {};
            o[property] = amount;
            Element.setBorderBoxSize(this.of,o);
        } else {
            this.of.style[property] = amount + 'px';
        }
    }
};

var JxLeftEdge = Class.create();
/**
 * JxLeftEdge represents the left edge of a DOM element
 */
JxLeftEdge.prototype = Object.extend( new JxEdge(), {
    set: function(amount) { this._set('left', amount); },
    get: function() { return Position.positionedOffset(this.of)[0];}
});

var JxTopEdge = Class.create();
/**
 * JxTopEdge represents the top edge of a DOM element
 */
JxTopEdge.prototype = Object.extend(new JxEdge(), {
    set: function(amount) { this._set('top', amount); },
    get: function() { return Position.positionedOffset(this.of)[1];}
});

var JxRightEdge = Class.create();
/**
 * JxRightEdge represents the right edge of a DOM element
 */
JxRightEdge.prototype = Object.extend(new JxEdge(), {
    set: function(amount) { 
        if (this.of.jxSizeConstraint && this.of.jxSizeConstraint.width != 'auto') {
            this._set('left', amount-this.of.jxSizeConstraint.width);
        } else { 
            this._set('width', amount-Position.positionedOffset(this.of)[0]); 
        }
    },
    get: function() { return Position.positionedOffset(this.of)[0] +
                              Element.getBorderBoxSize(this.of).width;}
});

var JxBottomEdge = Class.create();
/**
 * JxBottomEdge represents the bottom edge of a DOM element
 */
JxBottomEdge.prototype = Object.extend(new JxEdge(), {
    set: function(amount) { 
        if (this.of.jxSizeConstraint && this.of.jxSizeConstraint.height != 'auto') {
            this._set('top', amount-this.of.jxSizeConstraint.height);
        } else { 
            this._set('height', amount-Position.positionedOffset(this.of)[1]); 
        }
    },
    get: function() { return Position.positionedOffset(this.of)[1] + 
                              Element.getBorderBoxSize(this.of).height; }
});

/**
 * JxPageLeftEdge represents the left edge of the page.  It is a an object
 * that is used by all constraints so you don't need to create a new one.
 */
var JxPageLeftEdge = {
    set: function(amount) { return false; },
    get: function() { return 0;}
};

/**
 * JxPageTopEdge represents the top edge of the page.  It is a an object
 * that is used by all constraints so you don't need to create a new one.
 */
var JxPageTopEdge = {
    set: function(amount) { return false; },
    get: function() { return 0;}
};

/**
 * JxPageRightEdge represents the right edge of the page.  It is a an object
 * that is used by all constraints so you don't need to create a new one.
 */
var JxPageRightEdge = {
    set: function(amount) { return false; },
    get: function() { return Position.getPageDimensions().width;}
};

/**
 * JxPageBottomEdge represents the bottom edge of the page.  It is a an object
 * that is used by all constraints so you don't need to create a new one.
 */
var JxPageBottomEdge ={
    set: function(amount) { return false; },
    get: function() { return Position.getPageDimensions().height; }
};

/**
 * JxSizeConstraint applies a size constraint to a DOM element.  Size
 * constraints are used to set a fixed width or height on an element.  Note
 * that setting a size constraint does not actually change the size of the
 * element, just how the constraint manager views it.
 */
var JxSizeConstraint = {
    apply: function( element, w, h ) { $(element).jxSizeConstraint = { width : w, height: h}; }
};

/*
 * create a global layout manager that manages all the constraints
 *
 * by default, it binds to the window.onresize event, but you can call apply() directly
 * if you know the layout has changed.
 */
if (!window.JxConstraintManager) {
  var JxConstraintManager = new Object();
};

Object.extend( JxConstraintManager, JxListenerBase.prototype);

/**
 * JxConstraintManager is a global singleton that takes care of applying
 * constraints registered with it when the window size changes.  Application
 * of constraints can also be triggered as a response to some internal event
 * such as a split view (see JxSplitPane).
 */
Object.extend( JxConstraintManager, {
    constraints: [],
    wsl: [],
    initialize: function() {
    //jxTrace('JxConstraintManager::initialize()');
        window.onresize = this.onresize.bindAsEventListener(this);
        this.apply();
    },
    add: function(c){
        this.constraints.push(c)
    },
    apply: function(){
        //jxTrace('JxConstraintManager::apply()');
        for (var i=0; i<this.constraints.length; i++) {
            this.constraints[i].apply();
        }
    },
    addWindowSizeListener : function( o ) {
        this.addListener(this.wsl,o);
    },
    removeWindowSizeListener : function( o ) {
        this.removeListener(this.wsl,o);
    },
    onresize : function() {
        this.apply();
        this.processEvent(this.wsl,'windowSizeChanged',this);
    }
});


//add getPageDimensions to the Position object so we can set constraints on the edge of the page.
Object.extend( Position, { 
    getPageDimensions: function() {
        var w,h = 0;

        if (self.innerHeight) { // all except Explorer
            w = self.innerWidth;
            h = self.innerHeight;
        } else if (document.documentElement && document.documentElement.clientHeight) {
            // Explorer 6 Strict Mode
            w = document.documentElement.clientWidth;
            h = document.documentElement.clientHeight;
        } else if (document.body) { // other Explorers
            w = document.body.clientWidth;
            h = document.body.clientHeight;
        }

        return { width : w, height : h };
    }
} );

var JxSplitter = Class.create();
/**
 * abstract base class for creating a splitter within an element
 * Use JxHorizontalSplitter or JxVerticalSplitter
 */
JxSplitter.prototype = {
    elements: null,
    initialize: function(element, barPosition) {
        //jxTrace('JxSplitter::initialize()');
        element = $(element);
        var o1 = this.prepareElement();
        var bar = this.prepareElement();
        this.setBarPosition(bar, barPosition);
        bar.title = 'drag this bar to resize';
        bar.style.cursor = 'move';
        bar.style.lineHeight = '1px'; // for IE, horizontal bars
        var o2 = this.prepareElement();
        element.appendChild(o1);
        element.appendChild(bar);
        element.appendChild(o2);
        this.establishConstraints(element, o1, bar, o2);
        this.elements = [o1, bar, o2];
    },
    prepareElement: function(){
        //jxTrace('JxSplitter::prepareElement()');
        var o = document.createElement('div');
        o.style.position = 'absolute';
        return o;
    }
};

var JxHorizontalSplitter = Class.create();
/**
 * Create a horizontally split pane.
 */
Object.extend( Object.extend(JxHorizontalSplitter.prototype, JxSplitter.prototype), {
    establishConstraints: function( element, o1, bar, o2 ) {
        //jxTrace('JxHorizontalSplitter::establishConstraints()');
        //set up the left/right splitter
        new JxEdgeConstraint( new JxTopEdge(element), new JxTopEdge(bar));
        new JxEdgeConstraint( new JxTopEdge(element), new JxTopEdge(o1));
        new JxEdgeConstraint( new JxTopEdge(element), new JxTopEdge(o2));
        new JxEdgeConstraint( new JxBottomEdge(element), new JxBottomEdge(bar));
        new JxEdgeConstraint( new JxBottomEdge(element), new JxBottomEdge(o1));
        new JxEdgeConstraint( new JxBottomEdge(element), new JxBottomEdge(o2));
        new JxEdgeConstraint( new JxLeftEdge(element), new JxLeftEdge(o1));
        new JxEdgeConstraint( new JxLeftEdge(bar), new JxRightEdge(o1));
        new JxEdgeConstraint( new JxRightEdge(bar), new JxLeftEdge(o2));
        new JxEdgeConstraint( new JxRightEdge(element), new JxRightEdge(o2));
        new Draggable(bar, { constraint: 'horizontal' , 
                             change : JxConstraintManager.apply.bind(JxConstraintManager),
                             starteffect : function(element) { 
                                    element.style.backgroundColor = '#eee';
                                    },
                             endeffect : function(element) {
                                 element.style.backgroundColor = '';
                             }
        });
    },
    setBarPosition: function(bar, position) {
      //jxTrace(bar.id + ': set bar position to: ' + position);
      bar.style.left = position+"px";
    }
});

var JxVerticalSplitter = Class.create();
/**
 * Create a vertically split pane
 */
Object.extend( Object.extend(JxVerticalSplitter.prototype, JxSplitter.prototype), {
    establishConstraints: function( element, o1, bar, o2 ) {
        //jxTrace('JxVerticalSplitter::establishConstraints()');
        //set up the top/bottom splitter
        new JxEdgeConstraint( new JxLeftEdge(element), new JxLeftEdge(o1));
        new JxEdgeConstraint( new JxLeftEdge(element), new JxLeftEdge(bar));
        new JxEdgeConstraint( new JxLeftEdge(element), new JxLeftEdge(o2));
        new JxEdgeConstraint( new JxRightEdge(element), new JxRightEdge(o1));
        new JxEdgeConstraint( new JxRightEdge(element), new JxRightEdge(bar));
        new JxEdgeConstraint( new JxRightEdge(element), new JxRightEdge(o2));
        new JxEdgeConstraint( new JxTopEdge(element), new JxTopEdge(o1));
        new JxEdgeConstraint( new JxTopEdge(bar), new JxBottomEdge(o1));
        new JxEdgeConstraint( new JxBottomEdge(bar), new JxTopEdge(o2));
        new JxEdgeConstraint( new JxBottomEdge(element), new JxBottomEdge(o2));
        new Draggable(bar, { constraint: 'vertical' , change : JxConstraintManager.apply.bind(JxConstraintManager),
                             starteffect : function(element) { 
                                    element.style.backgroundColor = '#eee';
                                    },
                             endeffect : function(element) {
                                 element.style.backgroundColor = '';
                             }
        });
    },
    setBarPosition: function(bar, position) {bar.style.top = position+"px";}
});

var JxAction = Class.create();
/**
 * JxAction is an abstract base class that provides a mechanism to separate
 * the user interface code from the implementation code for a particular
 * piece of functionality.
 *
 */
JxAction.prototype = {
    pcl: null,
    enabled : null,
    initialize: function(f) {
        this.pcl = [];
        this.enabled = true;
        this.actionPerformed = f;
    },
    addPropertyChangeListener: function(o){this.addListener(this.pcl, o);},
    removePropertyChangeListener: function(o) {
        this.removeListener(this.pcl, o);
    },
    isEnabled: function() {return this.enabled;},
    setEnabled: function(b){
        /* prevent unnecessary propogation of propertyChanged */
        if (this.enabled == b) {
            return;
        }
        this.enabled = b;
        this.processEvent(this.pcl,'propertyChanged',this);
    },
    bindTo : function( item ) {
        this.addPropertyChangeListener(item);
        item.addActionListener(this);
    },
    unbindFrom: function(item) {
        this.removePropertyChangeListener(item);
        item.removeActionListener(this);
    },
    actionPerformed : function(o) { alert('actionPerformed'); }
};
Object.extend(JxAction.prototype, JxListenerBase.prototype);

var JxMenuItem = Class.create();
Object.extend(JxMenuItem.prototype, JxListenerBase.prototype);
Object.extend(JxMenuItem.prototype, {
    al: null,
    domObj: null,
    parent: null,
    enabled: false,
    initialize: function(label, action, bSeparator) {
        this.initializeItem(label, bSeparator);
        action.bindTo(this);
        this.propertyChanged(action);
    },
    initializeItem: function(label, bSeparator) { 
        this.label = label;
        this.al = [];
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxMenuItem';
        var elm = document.createElement('a');
        if (bSeparator) {
            elm.className = 'separator';
        }
        elm.href = '#';
        Event.observe(elm, 'click', this.processActionEvent.bindAsEventListener(this), true);
        Event.observe(elm, 'mouseover' ,this.onmouseover.bindAsEventListener(this), true);
        elm.innerHTML = label;
        this.domObj.appendChild(elm);
    },
    setParent: function(o) {
        this.parent = o;
    },
    hide: function() {},
    show: function() {},
    addActionListener: function(o) { this.addListener(this.al,o); },
    removeActionListener : function(o) { this.removeListener(this.al, o) },
    processActionEvent: function(e) { 
        //jxTrace('jxMenuItem::processActionEvent()');
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this);
            this.parent.deactivate();
        }
        //return Event.stop(e);
    },
    propertyChanged: function(o) {
        this.enabled = o.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj.childNodes[0], 'jxDisabled' );
        } else {
            Element.addClassName( this.domObj.childNodes[0], 'jxDisabled' );
        }
    },
    onmouseover: function(e) {
        var target = Event.element(e);
        this.parent.setVisibleItem(this);
    }
});

var JxMenu = Class.create();
Object.extend(JxMenu.prototype, JxMenuItem.prototype);
Object.extend(JxMenu.prototype, {
    subMenu: null,
    parent: null,
    visibleItem: null,
    items: null,
    initialize: function(label, bSeparator) { 
        this.open = false;
        this.items = [];
        this.initializeItem(label, bSeparator);
        this.domObj.childNodes[0].className = 'jxSubMenuIndicator';
        
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxDialogShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxSubMenu';
        this.subDomObj.style.display = 'none';
        
        this.domObj.appendChild(this.subDomObj);
    },
    setParent: function(o) {
        this.parent = o;
    },
    show: function() {
        if (this.open) return;
        jxTrace('JxMenu::show');
        this.open = true;
        this.subDomObj.style.display = 'block';
        if (!window.opera) {
            this.subDomObj.childNodes[0].appendChild(this.iframe);
        }
        this.setActive(true);
    },
    hide: function() {
        if (!this.open) return;
        jxTrace('JxMenu::hide');
        this.open = false;
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide();
        }
        this.subDomObj.style.display = 'none';
        if (!window.opera) {
            this.subDomObj.childNodes[0].removeChild(this.iframe);
        }
        this.visibleItem = null;
    },
    add : function() {
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
            this.show() 
        }
        return Event.stop(e)
    },
    deactivate: function() {
        if (this.parent) {
            this.parent.deactivate();            
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

var JxMenuBar = Class.create();
JxMenuBar.prototype = {
    bActive : false,
    visibleItem: null,
    items: null,
    initialize : function(domObj) {
        this.items = [];
        this.domObj = document.createElement('ul');
        this.domObj.className = 'jxMenuBar';
        $(domObj).appendChild(this.domObj);
        this.deactivateWatcher = this.deactivate.bindAsEventListener(this);
    },
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            item.domObj.className = 'jxMenuBarItem';
            item.domObj.childNodes[0].className = 'jxMenuBarLink';
            if (item.domObj.childNodes.length > 0) {
                item.domObj.childNodes[1].className = 'jxMenu';
                //item.domObj.childNodes[1].childNodes[1].className = 'jxMenu';
            }
            this.domObj.appendChild(item.domObj);
        }
    },
    deactivate: function(e) {
        for (var i=0; i<this.items.length; i++) {
            this.items[i].hide();
        }
        this.setActive(false);
    },
    isActive: function() { 
        return this.bActive; 
    },
    setActive: function(b) { 
        this.bActive = b;
        if (this.bActive) {
            Event.observe(document, 'click', this.deactivateWatcher);
        } else {
            Event.stopObserving(document, 'click', this.deactivateWatcher);
        }
    },
    setVisibleItem: function(o) {
        if (this.visibleItem && this.visibleItem.hide && this.visibleItem != o) {
            this.visibleItem.hide();
        }
        this.visibleItem = o;
        if (this.isActive()) {
            this.visibleItem.show();
        }
    }
};

var JxContextMenu = Class.create();
JxContextMenu.prototype = {
    domObj : null,
    items : null,
    initialize : function(id) {

        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxDialogShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.items = [];
        this.domObj = document.createElement('ul');
        this.domObj.className = 'jxMenu';
        this.domObj.style.display = 'none';
        
        document.getElementsByTagName('BODY')[0].appendChild(this.domObj);
        this.hideWatcher = this.hide.bind(this);
    },
    add : function() {
        for (var i=0; i<arguments.length; i++) {
            var item = arguments[i];
            this.items.push(item);
            item.setParent(this);
            this.domObj.appendChild(item.domObj);
        }
    },
    actionPerformed : function(o) {this.hide();},
    show : function(e) {
        var target = Event.element(e);
        Event.stop(e);
        this.domObj.style.left = Event.pointerX(e) + "px";
        this.domObj.style.top = Event.pointerY(e) + "px";
        //Effect.Appear(this.domObj, 0.1)
        this.domObj.style.display = 'block';
        if (!window.opera) {
            this.domObj.childNodes[0].appendChild(this.iframe);
        }
        Event.observe(document, 'click', this.hideWatcher, true);
    },
    hide : function() {
        Event.stopObserving(document, 'click', this.hideWatcher, true);
        Effect.Fade(this.domObj, 0.1);
    },
    bindTo : function(o) {
        o = $(o);
        o.oncontextmenu = this.show.bindAsEventListener(this);
    },
    deactivate: function() {
        this.hide();
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

var JxDialog = Class.create();
JxDialog.prototype = {
    onClose : null,
    onOpen : null,
    onChange : null,
    title : null,
    content : null,
    values : null,
    actions : null,
    handler : null,
    bContentLoaded : null,
    zIndex: [101],
    blanket: null,
    initialize: function(options) {
        //jxTrace('JxDialog:initialize('+(options.title||'no title')+')');
        this.initUniqueId();
        this.values = {};
        this.actions = {};
        this.handler = options.handler?options.handler:null;
        if (options.onChange) {this.onChange = options.onChange;}
        if (options.onClose) {this.onClose = options.onClose;}
        if (options.onOpen) {this.onOpen = options.onOpen;}
        if (options.onContentLoaded) {this.onContentLoaded = options.onContentLoaded;}
        
        var w = options.width || 250;
        var h = options.height || 250;
        var t = (options.top && options.top > 0) ? options.top : 0;
        var l = (options.left && options.left > 0) ? options.left : 0;
        
        this.blanket = document.createElement('div');
        this.blanket.className = 'jxDialogModal';
        this.blanket.style.display = 'none';
        
        if (!window.opera) {
            var iframe = document.createElement('iframe');
            //iframe.src = 'javascript:false';
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.blanket.appendChild(iframe);
            document.body.appendChild(this.blanket);
        }

        /* create the dialog box first so we can measure it */
        var d2 = document.createElement('div');
        d2.style.position = 'absolute';
        d2.style.top = '6px';
        d2.style.left = '6px';
        d2.style.width = w + 'px';
        d2.style.height = h + 'px';
        d2.className = 'jxDialog';
        
        this.title = document.createElement('div');
        this.title.className = 'jxDialogTitle';
        /* this is required for IE and Opera */
        //this.title.style.height = '22px';
        Element.setBorderBoxSize(this.title, {height:22});
        
        var span = document.createElement('span');
        span.innerHTML = options.title || 'Dialog Title';
        this.title.appendChild(span);
        
        /* element must be in the page to be measured */
        this.title.style.visibility = 'hidden';
        document.getElementsByTagName('BODY')[0].appendChild(this.title);
        var titleHeight = Element.getBorderBoxSize(this.title);
        document.getElementsByTagName('BODY')[0].removeChild(this.title);
        this.title.style.visibility = 'visible';
        
        d2.appendChild(this.title);
        
        var atag = document.createElement('a');
        atag.href = '#';
        atag.className = 'jxDialogCloseButton';
        atag.onclick = this.close.bindAsEventListener(this);
        
        var close = document.createElement('img');
        close.src = 'images/icon_close.png';
        close.alt = 'Close Dialog';
        close.title = 'Close Dialog';
        atag.appendChild(close);
        this.title.appendChild(atag);
        
        if (options.helpID || options.helpContent || options.helpURL) {
            var atag2 = document.createElement('a');
            atag2.href = '#';
            atag2.className = 'jxDialogHelpButton';
            atag2.onclick = this.toggleHelp.bindAsEventListener(this);

            //TODO: don't add help in title if it isn't needed
            var help = document.createElement('img');
            help.src = 'images/icon_quickhelp.png';
            help.alt = 'Help';
            help.title = 'Help';
            atag2.appendChild(help);
            this.title.appendChild(atag2);
        }
        
        this.action = document.createElement('div');
        this.action.className = 'jxDialogAction';
        Element.setBorderBoxSize(this.action, {height:30});

        /* element must be in the page to be measured */
        this.action.style.visibility = 'hidden';
        document.getElementsByTagName('BODY')[0].appendChild(this.action);
        var actionHeight = Element.getBorderBoxSize(this.action);
        document.getElementsByTagName('BODY')[0].removeChild(this.action);
        this.action.style.visibility = 'visible';
        
        var contentHeight = h - titleHeight.height - actionHeight.height;
        this.content = document.createElement('div');
        this.content.className = 'jxDialogContent';
        Element.setBorderBoxSize(this.content, {height: contentHeight});
        
        if (options.helpID || options.helpContent || options.helpURL) {
            this.help = document.createElement('div');
            this.help.className = 'jxDialogHelp';
            this.help.style.display = 'none';
            this.help.isVisible = false;
            if (options.helpID) {
                var c = $(options.helpID);
                if (c.parentNode) { 
                    c.parentNode.removeChild(c); 
                }
                this.help.appendChild(c);
                var img = document.createElement('img');
                img.className = 'jxDialogHelpCloseButton png24';
                img.src = 'images/help_close.png';
                img.onclick = this.toggleHelp.bind(this);
                img.alt = 'Close Help';
                img.title = 'Close Help';
                this.help.appendChild(img);
            } else if (options.helpURL) {
                var ts = 'ts=' + (new Date()).getTime();
                var a = new Ajax.Request( options.helpURL, 
                    Object.extend({method:'get',onComplete:this.helpLoaded.bind(this),
                                   parameters: ts}));
            } else if (options.helpHTML) {
                this.help.innerHTML = options.helpHTML;
                var img = document.createElement('img');
                img.className = 'jxDialogHelpCloseButton png24';
                img.src = 'images/help_close.png';
                img.onclick = this.toggleHelp.bind(this);
                img.alt = 'Close Help';
                img.title = 'Close Help';
                this.help.appendChild(img);
            }
            d2.appendChild(this.help);
        }
        
        d2.appendChild(this.content);
        
        if (options.buttons) {
            this.setButtons(options.buttons);
        }
        
        d2.appendChild(this.action);
        
        if (options.contentID) {
            var c = $(options.contentID);
            if (c.parentNode) { 
                c.parentNode.removeChild(c); 
            }
            this.content.appendChild(c);
            this.processInputs(this.content);
            this.bContentLoaded = true;
        } else if (options.contentURL) {
            this.bContentLoaded = false;
            var ts = 'ts=' + (new Date()).getTime();
            var a = new Ajax.Request( options.contentURL, 
                Object.extend({method:'get',onComplete:this.contentLoaded.bind(this),
                               parameters: ts}));
        } else if (options.contentHTML) {
            this.content.innerHTML = options.contentHTML;
            this.processInputs(this.content);
            this.bContentLoaded = true;
        }
        
        /* element must be in the page to be measured */
        d2.style.visibility = 'hidden';
        document.body.appendChild(d2);
        var boxSize = Element.getBorderBoxSize(d2);
        document.body.removeChild(d2);
        d2.style.visibility = 'visible';
        
        /* now create overall container with the correct size */
        var d = document.createElement('div');
        d.style.position = 'absolute';
        d.style.top = (t - 6) + 'px';
        d.style.left = (l - 6) + 'px';
        d.style.display = "none";
        d.id = options.id || options.title;
        Element.setContentBoxSize(d, {width:(boxSize.width+12), height:(boxSize.height+12)});
        
        /* drop shadows */
        /* top left */
        var img = document.createElement('img');
        img.src = 'images/dialog_glow_tl.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.top = '0px';
        img.style.left = '0px';
        d.appendChild(img);
        /* top right */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_tr.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.top = '0px';
        img.style.right = '0px';
        d.appendChild(img);
        /* bottom right */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_br.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.bottom = '0px';
        img.style.right = '0px';
        d.appendChild(img);
        /* bottom left */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_bl.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.bottom = '0px';
        img.style.left = '0px';
        d.appendChild(img);
        /* top */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_t.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = boxSize.width-12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.top = '0px';
        img.style.left = '12px';
        d.appendChild(img);
        this.topImg = img;
        /* bottom */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_b.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = boxSize.width-12;
        img.height = 12;
        img.style.position = 'absolute';
        img.style.bottom = '0px';
        img.style.left = '12px';
        d.appendChild(img);
        this.bottomImg = img;
        /* left */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_l.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12;
        img.height = boxSize.height-12;
        img.style.position = 'absolute';
        img.style.top = '12px';
        img.style.left = '0px';
        d.appendChild(img);
        this.leftImg = img;
        /* right */
        img = document.createElement('img');
        img.src = 'images/dialog_glow_r.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.width = 12
        img.height = boxSize.height-12;
        img.style.position = 'absolute';
        img.style.top = '12px';
        img.style.right = '0px';
        d.appendChild(img);
        this.rightImg = img;
        d.appendChild(d2);
        
        document.body.appendChild(d);
        this.domObj = d;
        
        if (options.resizeable) {
            this.resizeImage = document.createElement('img');
            this.resizeImage.className = 'jxDialogResize';
            this.resizeImage.style.position = 'absolute'; //required for Draggable
            this.resizeImage.style.top = (boxSize.height-15) + 'px';
            this.resizeImage.style.left = (boxSize.width-15) + 'px';
            this.resizeImage.src = 'images/dialog_resize.png';
            this.domObj.appendChild(this.resizeImage);
            new Draggable(this.resizeImage, {starteffect: false, endeffect: false,change:this.ondrag.bind(this)});            
        }
        
        this.bOpen = false;
    },
    ondrag: function(obj) {
        var delta = obj.currentDelta();
        //delta is top/left of resize image.  Bottom/right of the dialog needs to be
        //adjusted for size of image (20x20) and for the shadow (6x6) resulting in
        //an additional 14 pixels from the top/left of the resize image.
        var deltaX = delta[0] + 15;
        var deltaY = delta[1] + 15;
        var obj = this.title.parentNode;
        Element.setBorderBoxSize(obj, {width:deltaX, height:deltaY});
        var titleSize = Element.getBorderBoxSize(this.title);
        var actionSize = Element.getBorderBoxSize(this.action);
        Element.setBorderBoxSize(this.content, {height:deltaY-titleSize.height-actionSize.height-2, width:deltaX-2});
        Element.setBorderBoxSize(this.domObj, {height:deltaY + 12, width:deltaX + 12});
        
        Element.setBorderBoxSize(this.help, {height:deltaY - titleSize.height});
        
        Element.setBorderBoxSize(this.topImg, {width:deltaX-12});
        Element.setBorderBoxSize(this.bottomImg, {width:deltaX-12});
        Element.setBorderBoxSize(this.leftImg, {height:deltaY-12});
        Element.setBorderBoxSize(this.rightImg, {height:deltaY-12});
        //jxTrace('ondrag() '+ delta.toString());
    },
    setTitle: function( title ) {
        this.title.childNodes[0].innerHTML = title;
    },
    setButtons: function(buttons) {
        this.action.innerHTML = '';
        for (var i=0; i<buttons.length;i++) {
            var button = document.createElement('input');
            button.type = 'button';
            button.className = 'normalButton';
            button.name = buttons[i];
            button.value = buttons[i];
            button.onclick = this.buttonHandler.bind(this, button);
            this.action.appendChild(button);
        }
        
    },
    processInputs : function(o) {
        for (var i=0;i<o.childNodes.length; i++) {
            var node = o.childNodes[i];
            if (node.tagName == 'INPUT') {
                if (node.type == 'button') {
                    this.actions[node.id] = node;
                    node.onclick = this.buttonHandler.bind(this, node);
                } else {
                    this.values[node.id] = node;
                    if (this.onChange) {
                        node.onchange = this.onChangeHandler.bind(this, node);
                    }
                }
            } else {
                if (node.childNodes) {
                    this.processInputs(node);
                }
            }
        }
    },
    buttonHandler : function(input, event) {
        if (this.handler) {
            this.handler(input.value, this);
        }
    },
    onChangeHandler: function(input, event) {
        if (this.onChangeHandler) {
            this.onChangeHandler(input, this);
        }
    },
    getValue : function( name ) {
        var result = '';
        var input = this.values[name];
        if (input) {
            switch (input.tagName) {
                case 'INPUT': 
                    result = input.value;
                    break;
                case 'SELECT':
                    result = input.options[input.selectedIndex].value;
            }
        }
        return result;
    },
    setValue : function( name, value ) {
        if (typeof this.values[name] != 'undefined') {
            if (this.values[name].type == 'text') {
                this.values[name].value = value;
            }
        }
    },
    show : function( ) {
        this.blanket.style.zIndex = this.zIndex[0]++;
        this.domObj.style.zIndex = this.zIndex[0]++;
        Effect.Appear(this.domObj, {duration: 0.1});
        this.blanket.style.display = 'block';
        //this.domObj.style.display = 'block';
        new Draggable(this.domObj, {handle:this.title, starteffect: false, endeffect: false});
        
    },
    hide : function() {
        this.zIndex[0]--;
        this.zIndex[0]--;
        Effect.Fade(this.domObj, {duration: 0.3});
        this.blanket.style.display = 'none';
        
    },
    open: function() {
        if (!this.bOpen) {
            this.bOpen = true;
        }
        if (this.bContentLoaded) {
            this.show();
            if (this.onOpen) this.onOpen();
        }
    },
    close: function() {
        this.bOpen = false;
        this.hide();
        if (this.onClose) this.onClose();
    },
    contentLoaded : function(r) {
        //jxTrace('JxDialog.contentLoaded() - '+this.title.firstChild.textContent);
        this.bContentLoaded = true;
        this.content.innerHTML = r.responseText;
        this.processInputs(this.content);
        if (this.onContentLoaded) {
            this.onContentLoaded();
        }
        if (this.bOpen) {
            //may need to do this?
            //window.setTimeout(this.open.bind(this),1);
            this.open();
            this.bOpen = false;
        }
    },
    helpLoaded : function(r) {
        this.help.innerHTML = r.responseText;
        var img = document.createElement('img');
        img.className = 'jxDialogHelpCloseButton png24';
        img.src = 'images/help_close.png';
        img.onclick = this.toggleHelp.bind(this);
        img.alt = 'Close Help';
        img.title = 'Close Help';
        this.help.appendChild(img);
    },
    toggleHelp: function() { 
        if (this.help.isVisible) {
            Effect.Fade(this.help, {duration: 0.3});
        } else {
            var actionSize = Element.getBorderBoxSize(this.action);
            var contentSize = Element.getBorderBoxSize(this.content);
            Element.setBorderBoxSize(this.help, {height:contentSize.height+actionSize.height});
            Effect.Appear(this.help, {duration: 0.3});
        }
        this.help.isVisible = !this.help.isVisible;
    }
};
Object.extend(JxDialog.prototype, JxUniqueId.prototype);
/**
 * JxTreeItem is an item in a tree.  An item is a leaf node that has
 * no children.
 *
 * JxTreeItem supports selection listeners.  When an item in the tree
 * is selected by the user, listeners are called by invoking the
 * selectionChanged method on the listener and passing the tree item
 * object that was selected.  The application is responsible for
 * changing the style of the selected item in the tree and for
 * tracking selection if that is important.
 * 
 * When the selection changes, the mouse event that triggered the
 * selection change is passed to the listener as a lastEvent
 * member of the tree item object.  You can use this to determine
 * if the item was clicked, double-clicked, right clicked etc.
 */
var JxTreeItem = Class.create();
JxTreeItem.prototype = {
    data : null,
    domObj : null,
    parent : null,
    currentClassName : null,
    onClick : null,
    sl : null,
    enabled : null,
    contextMenu : null,
    initialize : function( options ) {
        //jxTrace('JxTreeItem::initialize()');
        this.initializeItem(options);
    },
    initializeItem: function(options) {
        //jxTrace('JxTreeItem::initializeItem()');
        this.sl = [];
        var label = options.label || 'new node';
        this.data = options.data || null;
        this.contextMenu = options.contextMenu || null;
        
        this.imgNode = options.imgNode || 'images/tree_none.png';
        this.imgIcon = options.imgIcon || 'images/tree_page.png';
                
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxTreeNode';
        this.currentClassName = 'jxTreeNode';
      
        var elm = document.createElement('img');
        elm.src = this.imgNode;
        this.domObj.appendChild(elm);
        
        elm = document.createElement('img');
        elm.src = this.imgIcon;
        this.domObj.appendChild(elm);
      
        elm = document.createElement('a');
        elm.href = '#';
        elm.onclick = this.selected.bindAsEventListener(this);
        elm.ondblclick = this.selected.bindAsEventListener(this);
        elm.oncontextmenu = this.showMenu.bindAsEventListener(this);
        elm.innerHTML = label;
        this.domObj.appendChild(elm);
        
        //this.update(false);
        //TODO: this could cause a memory leak in remove
        this.domObj.jxTreeItem = this;
        this.domObj.childNodes[2].jxTreeItem = this;
        
        this.onClick = options.onClick || null;
        this.enabled = options.enabled?options.enabled:true;
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
    },
    finalize: function() { this.finalizeItem(); },
    finalizeItem: function() {  
        this.domObj.childNodes[2].onclick = null;
        this.domObj.childNodes[2].ondblclick = null;
        this.domObj.childNodes[2].oncontextmenu = null;
        this.contextMenu = null;
        this.onClick = null;
        for (var i=this.sl.length; i>=0; i--) {
            this.removeSelectionListener(this.sl[i]);
        }
        this.parent = null;
    },
    clone : function(node) {
    //jxTrace('JxTreeItem::clone()');
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgNode : this.imgNode,
                        imgIcon : this.imgIcon
                        };
        var item = new JxTreeItem(options);
        return item;
    },
    update : function(bDescend) {
        //jxTrace('JxTreeItem::update('+bDescend+')');
        if (arguments.length > 1) {
      bLast = arguments[1];
    } else {
          bLast = (this.parent && this.parent.isLastNode(this));
    }
        
        if (bLast) {
            this.domObj.className = 'jxTreeNodeLast';
        } else {
            this.domObj.className = 'jxTreeNode';
        }
    },
    selected : function(e) {
        //jxTrace('JxTreeItem::clicked()');
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
    },
    showMenu: function(e) {
        this.lastEvent = e?e:event;
        this.processEvent(this.sl,'selectionChanged',this);
        if (this.contextMenu) {
            this.contextMenu.show(this.lastEvent);
        }
    },
    addSelectionListener: function(o){this.addListener(this.sl, o);},
    removeSelectionListener: function(o) {this.removeListener(this.sl, o);},
    getName : function() { return this.domObj.childNodes[2].innerHTML; },
    propertyChanged : function(o) {
        this.enabled = o.isEnabled();
        if (this.enabled) {
            Element.removeClassName(this.domObj, 'jxDisabled');
        } else {
            Element.addClassName(this.domObj, 'jxDisabled');
        }
    }
};
Object.extend(JxTreeItem.prototype, JxListenerBase.prototype);

var JxTreeFolder = Class.create();
Object.extend(JxTreeFolder.prototype, JxTreeItem.prototype);
Object.extend(JxTreeFolder.prototype, {
    subDomObj : null,
    nodes : null,
    isOpen : false,
    dl : null,
    initialize : function( options ) {
    //jxTrace('JxTreeFolder::initialize() - '+options.data.rid);
        this.initializeFolder(options);
    },
    initializeFolder : function(options) {
    //jxTrace('JxTreeFolder::initializeFolder() - '+options.data.rid);  
        this.initializeItem(options);
        
        this.imgTreePlus = options.imgTreePlus || 'images/tree_plus.png';
        this.imgTreeMinus = options.imgTreeMinus || 'images/tree_minus.png';
        
        this.imgTreeFolder = options.imgTreeFolder || 'images/tree_folder.png';
        this.imgTreeFolderOpen = options.imgTreeFolderOpen || 'images/tree_folder_open.png';
        
        this.domObj.childNodes[0].src = this.imgTreePlus;
        this.domObj.childNodes[1].src = this.imgTreeFolder;
        
        this.domObj.childNodes[0].onclick = this.clicked.bindAsEventListener(this);
                
        this.nodes = [];
        this.subDomObj = document.createElement('ul');
        this.domObj.appendChild(this.subDomObj);
        this.subDomObj.className = 'jxTree';
        this.isOpen = options.isOpen || false;
        this.dl = [];
        if (this.isOpen) {
            this.expand();
        } else {
            this.collapse();
        }
    //this.makeDroppable();
    },
    finalize: function() {
        this.finalizeFolder();
        this.finalizeItem();
    },
    finalizeFolder: function() {
        this.domObj.childNodes[0].onclick = null;
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.nodes[i].finalize();
            this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes.pop();
        }
        for (var i=this.dl.length-1; i>=0; i--) {
            this.removeDisclosureListener(this.dl[i]);
        }
    },
    makeDroppable : function() {
    //jxTrace('JxTreeItem::makeDroppable()');
        if (!document.JxTreeCurrentTimeout) document.JxTreeCurrentTimeout = null;
        Droppables.add(this.domObj.childNodes[2], {
            greedy : true,
            onHover : function(draggable, droppable, overlap) { 
                var dropped = droppable.parentNode.jxTreeItem;
                if (document.JxTreeCurrentTimeout && 
                    document.currentDroppable != droppable) {
                    window.clearTimeout(document.JxTreeCurrentTimeout);
                    document.JxTreeCurrentTimeout = null;
                }
                if (!document.JxTreeCurrentTimeout && 
                    dropped.nodes && 
                    !dropped.isOpen)
                {
                    document.JxTreeCurrentTimeout = window.setTimeout(dropped.expand.bind(dropped), 700);
                }
                
                if (!document.currentHighlight != droppable) {
                    if (droppable == draggable)
                    {
                        Droppables.remove(droppable);
                    }
                    if (document.currentDroppable && 
                        document.currentDroppable != droppable) {
                        Element.removeClassName(document.currentDroppable, 
                                                'jxTreeDropzone');
                    }
                    
                    Element.addClassName(droppable, 'jxTreeDropzone');
                    document.currentDroppable = droppable;
                }
            },
            onDrop : function(draggable, droppable) {
                var dragged = draggable.jxTreeItem;
                var dropped = droppable.parentNode.jxTreeItem;

                Element.removeClassName(droppable, 
                                            'jxTreeDropzone');
                if (dropped.nodes) {
                    dragged.parent.remove(dragged);
                    dropped.append(dragged);
                    dropped.expand();
                } else if (dropped.parent != null) {
                    dragged.parent.remove(dragged);
                    dropped.parent.insert(dragged,dropped);
                }
            }
        });
    },
    clone : function(node) {
    //jxTrace('JxTreeFolder::clone() - '+this.data.rid);
        var options = { label : this.domObj.childNodes[2].innerHTML, 
                        data : this.data,
                        onClick : this.onClick,
                        imgTreePlus : this.imgTreePlus,
                        imgTreeMinus : this.imgTreeMinus,
                        imgTreeFolder : this.imgTreeFolder,
                        imgTreeFolderOpen : this.imgTreeFolderOpen
                        };
        var node = new JxTreeFolder(options);
        for (var i=0;i<this.nodes.length;i++) {
            node.append(this.nodes[i].clone());
        }
        return node;
    },
    isLastNode : function(node) {
        if (this.nodes.length == 0) {
            return false;
        } else {
            return this.nodes[this.nodes.length-1] == node;
        }
    },
    update : function(bDescend) {
        /* avoid update if not attached to tree yet */
        if (!this.parent) return;
        var bLast = false;
        //jxTrace('JxTreeFolder::update('+bDescend+','+arguments[1]+') - '+this.data.rid);
        if (arguments.length > 1) {
            bLast = arguments[1];
        } else {
            bLast = (this.parent && this.parent.isLastNode(this));
        }
        
        if (bLast) {
            this.domObj.className = 'jxTreeNodeLast';
            this.subDomObj.className = 'jxTree';
        } else {
            this.domObj.className = 'jxTreeNode';
            this.subDomObj.className = 'jxTree jxTreeNest';
        }
        
        if (this.isOpen) {
            this.domObj.childNodes[0].src = this.imgTreeMinus;
            this.domObj.childNodes[1].src = this.imgTreeFolderOpen;
        } else {
            this.domObj.childNodes[0].src = this.imgTreePlus;
            this.domObj.childNodes[1].src = this.imgTreeFolder;
        }
        
        if (this.nodes && bDescend) {
            //this.nodes.each(function(n){n.update(true);});
            for(var i=0; i<this.nodes.length; i++) {
                this.nodes[i].update(false, i==this.nodes.length-1);
            }
        }
    },
    /**
     * append a node at the end of the sub-tree
     * @param node {Object} the node to append.
     */
    append : function( node ) {
        //jxTrace('JxTreeFolder::append() - ' + node.data.szPath);
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);
    },
    /**
     * insert a node after refNode.  If refNode is null, insert at beginning
     * @param node {Object} the node to insert
     * @param refNode {Object} the node to insert before
     */
    insert : function( node, refNode ) {
    //jxTrace('JxTreeFolder::insert()');
        node.parent = this;
        //if refNode is not supplied, insert at the beginning.
        if (!refNode) {
            this.nodes.unshift(node);
            //sanity check to make sure there is actually something there
            if (this.subDomObj.childNodes.length ==0) {
                this.subDomObj.appendChild(node.domObj);
            } else {
                this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[0]);                
            }
        } else {
            //walk all nodes looking for the ref node.  Track if it actually
            //happens so we can append if it fails.
            var b = false;
            for(var i=0;i<this.nodes.length;i++) {
                if (this.nodes[i] == refNode) {
                    //increment to append after ref node.  If this pushes us
                    //past the end, it'll get appended below anyway
                    i = i + 1;
                    if (i < this.nodes.length) {
                        this.nodes.splice(i, 0, node);
                        this.subDomObj.insertBefore(node.domObj, this.subDomObj.childNodes[i]);
                        b = true;
                        break;
                    }
                }
            }
            //if the node wasn't inserted, it is because refNode didn't exist
            //and so the fallback is to just append the node.
            if (!b) {
                this.nodes.push(node); 
                this.subDomObj.appendChild(node.domObj); 
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    /**
     * remove the specified node from the tree
     * @param node {Object} the node to remove
     */
    remove : function(node) {
        //jxTrace('JxTreeFolder::remove()');
        node.parent = null;
        for(var i=0;i<this.nodes.length;i++) {
            if (this.nodes[i] == node) {
                this.nodes.splice(i, 1);
                this.subDomObj.removeChild(this.subDomObj.childNodes[i]);
                break;
            }
        }
        this.update(true);
        //if (this.parent)
        //    this.parent.update(true);        
    },
    /**
     * handle the user clicking on this folder by expanding or
     * collapsing it.
     * @param e {Event} the event object
     */
    clicked : function(e) {
        //jxTrace('JxTreeFolder::clicked()');
        e = e?e:event;
        
        if (this.isOpen) {
            this.collapse();
        } else {
            this.expand();
        }
    },
    expand : function() {
        //jxTrace('JxTreeFolder::expand()');
        this.isOpen = true;
        this.subDomObj.style.display = 'block';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);    
    },
    collapse : function() {
        //jxTrace('JxTreeFolder::collapse()');
        this.isOpen = false;
        this.subDomObj.style.display = 'none';
        this.update(true);
        this.processEvent(this.dl, 'disclosed', this);
    },
    addDisclosureListener: function(o){this.addListener(this.dl, o);},
    removeDisclosureListener: function(o) {this.removeListener(this.dl, o);},
    findChild : function(path) {
        //jxTrace('JxTreeFolder::findChild()');
        //path is empty - we are asking for this node
        if (path.length == 0)
            return this;
        
        //path has only one thing in it - looking for something in this folder
        if (path.length == 1)
        {
            for (var i=0; i<this.nodes.length; i++)
            {
                if (this.nodes[i].getName() == path[0])
                    return this.nodes[i];
            }
            return null;
        }
        //path has more than one thing in it, find a folder and descend into it    
        var childName = path.shift();
        for (var i=0; i<this.nodes.length; i++)
        {
            if (this.nodes[i].getName() == childName && this.nodes[i].findChild)
                return this.nodes[i].findChild(path);
        }
        return null;
    }
});

var JxTree = Class.create();
Object.extend( JxTree.prototype, JxTreeFolder.prototype );
Object.extend( JxTree.prototype, {
    initialize : function( id ) {
        //jxTrace('JxTree::initialize()');
        this.subDomObj = document.createElement('ul');
        this.subDomObj.className = 'jxTreeRoot';
        $(id).appendChild(this.subDomObj);
        this.nodes = [];
        this.isOpen = true;
    },
    finalize: function() { 
        this.clear(); 
        this.subDomObj.parentNode.removeChild(this.subDomObj); 
    },
    clear: function() {
        for (var i=this.nodes.length-1; i>=0; i--) {
            this.nodes[i].finalize();
            this.subDomObj.removeChild(this.nodes[i].domObj);
            this.nodes.pop();
        }
    },
    update : function(bDescend) {
        //jxTrace('JxTree::update('+bDescend+')');
        var bLast = true;
        if (this.subDomObj)
        {
            if (bLast) {
                Element.removeClassName(this.subDomObj, 'jxTreeNest');
            } else {
                Element.addClassName(this.subDomObj, 'jxTreeNest');
            }
        }
        if (this.nodes && bDescend)
            this.nodes.each(function(n){n.update(false);});
    },
    /**
     * append a node at the end of the sub-tree
     * @param node {Object} the node to append.
     */
    append : function( node ) {
        //jxTrace('JxTree::append() - ' + node.data.szPath);
        node.parent = this;
        this.nodes.push(node);
        this.subDomObj.appendChild( node.domObj );
        this.update(true);        
    }
});

/**
 * @class JxSnapMananger
 * JxSnapManager manages a set of JxSnapPanel instances inside a
 * DOM element provided by the application.  
 */
var JxSnapManager = Class.create();
JxSnapManager.prototype = {
    panels : null,
    height : null,
    /* constructor, shared by sub-classes */
    initialize : function(domObj){ 
        //jxTrace('JxSnapManager::initialize()');
        this.domObj = $(domObj);
        this.panels = [];
    },
    /**
     * destruct this snap manager and all its panels 
     */
    finalize : function() {
        this.panels.each(function(p){p.finalize()});
        this.panels.length = 0;
    },
    add : function(){
        //jxTrace('JxSnapManager::add()');
        for (var i=0; i<arguments.length; i++) {
            var panel = arguments[i];
            this.panels.push(panel);
            panel.manager = this;
            this.domObj.appendChild(panel.domObj);
            if (this.panels.length > 1) {
                panel.makeDraggable();
            }
        }
        this.distributeSpace();
  },
  minimize: function(o) {
    //jxTrace('JxSnapManager::minimize()');
    var idx = this.getPanelIndex(o);
    var space = o.getContentHeight();
    var b = false;
    if (idx == 0) {
      b = this.minimizeDown(idx, space);
      if (!b) {
        b = this.minimizeUp(idx, space);
      }
    } else {
      b = this.minimizeUp(idx, space);
      if (!b) {
        b = this.minimizeDown(idx, space);
      }
    }
    if (b) {
      o.setContentHeight(0);
    } else {
      if (idx > 0) {
        this.open(this.panels[idx - 1]);
      }
    }
  },
  maximize: function(o) {
    //jxTrace('JxSnapManager::maximize()');
    for (var i=0; i<this.panels.length; i++) {
      if (this.panels[i] == o) {
        this.panels[i].setState('open');
        this.panels[i].content.style.display = 'block';
      } else {
        this.panels[i].setContentHeight(0);
      }
    }
    this.distributeSpace();
  },
  open : function(o) {
    //jxTrace('JxSnapManager::open()');
    var idx = this.getPanelIndex(o);
    var n = -1;
    if (idx==0) {
      n = this.openDown(idx, o);
      if (n == -1) {
        n = this.openUp(idx, o);
      }
    } else {
      n = this.openUp(idx, o);
      if (n == -1) {
        n = this.openDown(idx, o);
      }
    }
    if (n > 0) {
      var h = o.getContentHeight();
      //jxTrace('JxSnapManager::open() adding '+n+' to existing height of '+h);
      o.setContentHeight(h+n);
    }
  },
  dragPanel : function( o, amount ) {
    //jxTrace('JxSnapManager::dragPanel(o, '+amount+')');
    var idx = this.getPanelIndex(o);
    /* first panel cannot be dragged */
    if (idx == 0) {
      return;
    }
    /* */
    var allocation = 0;
    if (amount < 0) {
      allocation = this.getSpaceFromAbove(idx, Math.abs(amount));
      if (allocation > 0) {
        var h = o.getContentHeight();
        //jxTrace('JxSnapManager::dragPanel() adding '+allocation+' to existing height of '+h);
        o.setContentHeight(h+allocation);
      }
    } else if (amount > 0 ) {
      allocation = this.getSpaceFromBelow(idx, amount);
      if (allocation > 0) {
        var panelIdx = idx-1;
        /*for (var i=panelIdx; i>=0; i--) {
          if (this.panels[i].getState() == 'open') {
            panelIdx = i;
            break;
          }
        }*/
        var h = this.panels[panelIdx].getContentHeight();
        this.panels[panelIdx].setContentHeight(h+allocation);
      }
    }
  },
  getSpaceFromAbove : function(idx,amount) {
    var result = 0;
    for (var i=idx-1; i>=0; i--) {
      if (this.panels[i].getState() == 'open') {
        var h = this.panels[i].getContentHeight();
        if (h >= amount) {
          result = amount;
        } else {
          result = h;
        }
        //jxTrace('JxSnapManager::getSpaceFromAbove(): h='+h+', result=' + result);
        this.panels[i].setContentHeight(h-result);
        break;
      }
    }
    return result;
  },
  
  getSpaceFromBelow : function(idx, amount) {
    var result = 0;
    for (var i=idx; i<this.panels.length; i++) {
      if (this.panels[i].getState() == 'open') {
        var h = this.panels[i].getContentHeight();
        if (h >= amount) {
          result = amount;
        } else {
          result = h;
        }
        //jxTrace('JxSnapManager::getSpaceFromBelow(): h='+h+', result='+result);
        this.panels[i].setContentHeight(h-result);
        break;
      }
    }
    return result;
  },
  /**
   * return the height of the DOM object that the manager is running within
   * @param {boolean} bRecalculate force recalculation of height if true
   * @return {Integer} the height of the container
   */
  getHeight : function() {
    //jxTrace('JxSnapManager::getHeight()');
    if (!this.height || arguments[0]) {
      var size = Element.getContentBoxSize(this.domObj);
      this.height = size.height;
    }
    //jxTrace('JxSnapManager::getHeight() returning ' + this.height);
    return this.height;
  },
  /**
   * called by the owner of this object when the container is resized
   */
  resize : function() {
      //jxTrace('JxSnapManager::resize()');
    var previousHeight = this.height;
    var newHeight = this.getHeight(true);
    var diff = newHeight - previousHeight;
      //jxTrace('JxSnapManager::resize() diff is ' + diff);
    if (diff < 0) {

      /* page is shrinking, remove space from the bottom up */
      diff = Math.abs(diff);
      /* call repeatedly until we have removed enough space */
      while(diff) {
          var h = this.getSpaceFromAbove(this.panels.length, diff);
          if (h == 0) {
              diff = 0; //prevent infinite loop
          } else {
            diff -= h
          }
      }
    } else if (diff > 0) {
      /* page is expanding, add space to the bottom most if possible
         first calculate the height of all the titles.  If the new
         height is not at least that much, there is nothing to do.
       */
      var titleHeight = 0;
      for (var i=0; i<this.panels.length; i++) {
          titleHeight += this.panels[i].getTitleHeight();
      }
      if (newHeight > titleHeight) {
          /* there is some space to allocate to the bottom most panel
             but we may not be able to allocate all the space because of
             the height required for titles.  Adjust if necessary.
           */
          if (previousHeight <= titleHeight) {
              diff = newHeight - titleHeight;
          }
          /* finally allocate the remaining space to the last panel */
          var o = this.panels[this.panels.length-1];
          var h = o.getContentHeight();
          o.setContentHeight(h+diff);
      }
    }
  },
  minimizeUp : function(idx, space) {
    //jxTrace('JxSnapManager::minimizeUp()');
    var b = false;
    if (idx > 0) {
        var o = this.panels[idx-1];
        var h = o.getContentHeight();
        o.setContentHeight(space+h);
        b = true;
      }
    //jxTrace('JxSnapManager::minimizeUp('+idx+','+space+') returning ' + b);
    return b;
  },
  minimizeDown : function(idx, space) {
    //jxTrace('JxSnapManager::minimizeDown()');
    var b = false;
    if (idx <this.panels.length -1) {
      for (var i=idx+1; i<this.panels.length; i++) {
        if (this.panels[i].getState() == 'open') {
          var h = this.panels[i].getContentHeight();
          this.panels[i].setContentHeight(space+h);
          b = true;
          break;
        }
      }
    }
    //jxTrace('JxSnapManager::minimizeDown('+idx+','+space+') returning ' + b);
    return b;
  },
  openUp : function(idx, o) {
    //jxTrace('JxSnapManager::openUp()');
    var n = -1;
    if (idx > 0) {
      for (var i=idx-1; i>=0; i--) {
        if (this.panels[i].getState() == 'open') {
          n = this.panels[i].getContentHeight();
          this.panels[i].setContentHeight(0);
          break;
        }
      }
    }     
    //jxTrace('JxSnapManager::openUp('+idx+',o) returning ' + n);
    return n;
  },
  openDown : function(idx, o) {
    //jxTrace('JxSnapManager::openDown()');
    var n = -1;
    if (idx <this.panels.length -1) {
      for (var i=idx+1; i<this.panels.length; i++) {
        if (this.panels[i].getState() == 'open') {
          n = this.panels[i].getContentHeight();
          this.panels[i].setContentHeight(0);
          break;
        }
      }
    } 
    //jxTrace('JxSnapManager::openDown('+idx+',o) returning ' + n);
    return n;
  },
  getPanelIndex: function(o) {
    //jxTrace('JxSnapManager::getPanelIndex()');
    var idx = -1;
    for (var i=0; i<this.panels.length; i++) {
      if (this.panels[i] == o) {
        idx = i;
        break;
      }
    }
    //jxTrace('JxSnapManager::getPanelIndex() returning ' + idx);
    return idx;
  },
  distributeSpace : function() {
    //jxTrace('JxSnapManager::distributeSpace() has ' + this.panels.length + ' panels' );
    var titleHeight = 0;
    var nOpen = 0;
    for (var i=0; i<this.panels.length; i++) {
      titleHeight += this.panels[i].getTitleHeight();
      //jxTrace('panel state ' + i + ':' + this.panels[i].getState());
      if (this.panels[i].getState() == 'open') {
        nOpen ++;
      }
    }
    var remainingHeight = this.getHeight() - titleHeight;
    //jxTrace('JxSnapManager::distributeSpace() remainingHeight:' + remainingHeight + ', nOpen:'+nOpen);
    for (var i=0; i<this.panels.length; i++) {
      if (this.panels[i].getState() == 'open') {
        var theHeight = parseInt(remainingHeight/nOpen);
        //jxTrace('JxSnapManager::distributeSpace() space for idx ' + i + ' theHeight:' + theHeight);
          this.panels[i].setContentHeight(theHeight);
        nOpen --;
        remainingHeight -= theHeight;
        //jxTrace('JxSnapManager::distributeSpace() remainingHeight:' + remainingHeight + ', nOpen:'+nOpen);
      } else {
        //jxTrace('JxSnapManager::distributeSpace() no space allocated for idx ' + i);
      }
    }
  },
  getTop: function(o) {
    var idx = this.getPanelIndex(o);
    var t = 0;
    for (var i=0; i<idx; i++) {
      t = t + this.panels[i].getTitleHeight() + this.panels[i].getContentHeight();
    }
    return t;
  }
};

var JxSnapPanel = Class.create();
/**
 * JxSnapPanel
 */
JxSnapPanel.prototype = {
    /** 
     * the JxSnapManager that this panel is managed by.  This is set by the
     * JxSnapManager when this panel is added to it.
     */
    manager : null,
    /** the DOM object that holds the label in the title bar. */
    labelObj : null,
    /** the DOM object that holds the button objects in the title bar. */
    // buttonObj : null,
    /** the state of this panel */
    state : 'open',
    /** the cached heights of this panel */
    contentHeight : null,
    titleHeight : null,
    /* track the busy state of this panel - used to control a 'loading' image */
    busyCount : null,
    bContentReady : null,
    onContentReady : null,
    /** 
     * @constructor
     * Initialize a new JxSnapPanel instance
     */
    initialize : function(options){
        //jxTrace("JxSnapPanel::initialize('"+options.label+"')");
        this.initUniqueId();
        this.domObj = document.createElement('div');
    
        this.title = document.createElement('div');
        this.title.className = "snapTitle";
        //TODO: Opera is broken because it doesn't report the height of the
        //title bars at all unless set through javascript
        //this is a hack until we can figure out from css what the height is
        this.title.style.height = '22px';
        Event.observe(this.title, 'dblclick', this.maximize.bindAsEventListener(this));
        this.domObj.appendChild(this.title);
    
        this.content = document.createElement('div');
        this.content.className = "snapContent";
        this.content.innerHTML = options.content?options.content:'';
        this.domObj.appendChild(this.content);
    
        this.labelObj = document.createElement('span');
        this.labelObj.className = 'snapLabel';
        this.labelObj.innerHTML = options.label?options.label:'empty';
    
        var a, img;
        if (options.helpCallback) {
            a = document.createElement('a');
            a.className = 'snapHelp';
            a.href = '#';
            Event.observe(a, 'click', options.helpCallback);
            img = document.createElement('img');
            img.src = "images/icon_quickhelp.png";
            img.alt = 'Help on this panel';
            img.title = 'Help on this panel';
            a.appendChild(img);
            this.title.appendChild(a);
            this.minimizeButton = a;
        }
    
        a = document.createElement('a');
        a.className = 'snapMinimize';
        a.href = '#';
        Event.observe(a, 'click', this.minimize.bindAsEventListener(this));
        var img = document.createElement('img');
        img.src = "images/icon_minimize.png";
        img.alt = 'Minimize Panel';
        img.title = 'Minimize Panel';
        a.appendChild(img);
        this.title.appendChild(a);
    
        a = document.createElement('a');
        a.className = 'snapMaximize';
        a.href = '#';
        Event.observe(a, 'click', this.maximize.bindAsEventListener(this));
        img = document.createElement('img');
        img.src = "images/icon_maximize.png";
        img.alt = 'Maximize Panel';
        img.title = 'Maximize Panel';
        a.appendChild(img);
        this.title.appendChild(a);
    
        a = document.createElement('a');
        a.className = 'snapLoading';
        a.href = '#';
        img = document.createElement('img');
        img.src = 'images/loading.gif';
        img.className = 'snapLoading';
        a.alt = 'Reload';
        a.title = 'Reload Panel';
        a.appendChild(img);
        this.loadingObj = {};
        this.loadingObj.link = a;
        this.loadingObj.img = img;
        this.title.appendChild(this.loadingObj.link);
        this.title.appendChild(this.labelObj);

        this.busyCount = 0;
        this.bContentReady = false;
    
        this.setState(options.state?options.state:this.state);
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
    makeDraggable : function() {    
        new Draggable(this.title,  {revert:true,
          reverteffect: function(element, top_offset, left_offset) {
            element.style.top = '';
            element.style.left = '';
            element.style.position = 'relative';
              },
              starteffect: false, 
              endeffect: false,
            constraint:'vertical',
            ghosting: true,
            change:this.ondrag.bind(this)});
        Draggables.addObserver(this);
    },
    onStart : function(event, obj, event) {
        //jxTrace('<b>JxSnapPanel::onStart()</b>');
        this.dragOffset = null;
    },
    ondrag : function(obj) {
        var t = this.manager.getTop(this);
        //jxTrace('JxSnapPanel::ondrag(): top:' + t + ' delta: ' + obj.currentDelta()[1]);

        if (this.manager) {
          this.manager.dragPanel(this, obj.currentDelta()[1]-t);
        }

        /*
        if (!this.dragOffset) {
          this.dragOffset = obj.currentDelta()[1];
        }
        var amount = obj.currentDelta()[1] - this.dragOffset;
        this.dragOffset = obj.currentDelta()[1];
        //jxTrace( 'JxSnapPanel::ondrag() offset=' + amount);
        */
    },
    minimize : function () {
        //jxTrace('JxSnapPanel::minimize()');
        if (this.manager) {
          this.manager.minimize(this);
        }
    },
    maximize : function () {
        //jxTrace('JxSnapPanel::maximize()');
        if (this.manager) {
          this.manager.maximize(this);
        }
    },
    open : function () {
        //jxTrace('JxSnapPanel::open()');
        if (this.manager) {
          this.manager.open(this);
        }
    },
    setContent : function (html) {
        //jxTrace('JxSnapPanel::setContent()');
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
        var ts = (new Date()).getTime();
        url = url + 'ts='+ts;
        var a = new Ajax.Request( url, 
            Object.extend({method:'get', onComplete:this.contentLoaded.bind(this)}));
    },
    contentLoaded: function(r) {
        this.content.innerHTML = r.responseText;
        this.bContentReady = true;
        this.setBusy(false);
        if (this.onContentReady) {
            window.setTimeout(this.onContentReady.bind(this),1);
        }
    },
    getTitleHeight : function() {
        //jxTrace('JxSnapPanel::getTitleHeight()');
        if (!this.titleHeight) {
            var size = Element.getBorderBoxSize(this.title);
            this.titleHeight = size.height;
        }
        //jxTrace('JxSnapPanel::getTitleHeight() returning ' + this.titleHeight);
        return  this.titleHeight;
    },
    getContentHeight : function() {
      //jxTrace('JxSnapPanel::getContentHeight()');
      if (!this.contentHeight) {
          if (this.state == 'open') {
              var size = Element.getBorderBoxSize(this.content);
              this.contentHeight = size.height;     
          }
      }
      //jxTrace('JxSnapPanel::getContentHeight() returning ' + this.contentHeight);
      return  this.contentHeight;
    },
  setContentHeight : function(n) {
    //jxTrace('JxSnapPanel::setContentHeight('+n+')');
    this.contentHeight = n;
    if (n == 0) {
      this.setState('closed');
    } else {
      this.setState('open');
    }
    Element.setBorderBoxSize(this.content, {height:n});
  },
  setState : function(state) {
    //jxTrace('JxSnapPanel::setState("'+state+'")');
    this.state = state;
    if (this.state == 'closed') {
      this.contentHeight = 0;
      this.content.style.display = 'none';
    } else {
      this.content.style.display = 'block';
    }
  },
  getState : function() {
      //jxTrace('JxSnapPanel::getState() returning '+ this.state);
      return this.state;
  },
  setBusy : function(b) {
      this.busyCount += b?1:-1;
      this.loadingObj.img.style.visibility = (this.busyCount>0)?'visible':'hidden';
  }
};
Object.extend(JxSnapPanel.prototype, JxUniqueId.prototype);
/**
 *
 */
var JxToolbar = Class.create();
JxToolbar.prototype = {
    items : null,
    domObj : null,
    initialize : function(domObj) {
        this.domObj = document.createElement('ul');
        this.domObj.className = 'jxToolBar';
        $(domObj).appendChild(this.domObj);
    },
    add : function( ) {
        for (var i=0; i<arguments.length; i++) {
            arguments[i].toolbar = this;
            this.domObj.appendChild(arguments[i].domObj);
        }
    }
};

/**
 *
 */
var JxToolbarItem = Class.create();
Object.extend(JxToolbarItem.prototype, JxListenerBase.prototype);
Object.extend(JxToolbarItem.prototype, {
    al: null,
    domObj: null,
    toolbar: null,
    enabled:null,
    initialize : function( imgPath, label, action, bSeparator ) {
        this.al = [];
        this.domObj = document.createElement('li');
        if (bSeparator) {
            this.domObj.className = 'separator';
        }
        var a = document.createElement('a');
        a.href='#'; //make hover effects work in IE
        a.onclick = this.onclick.bindAsEventListener(this);
        var img = document.createElement('img');
        img.src = imgPath;
        img.title = label;
        img.alt = label;
        this.domObj.appendChild(a);
        a.appendChild(img);
        action.bindTo(this);
        this.propertyChanged(action);
    },
    onclick : function() {
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this); 
        }
    },
    addActionListener: function(o) { 
        this.addListener(this.al,o); 
    },
    removeActionListener : function(o) { 
        this.removeListener(this.al, o) 
    },
    propertyChanged: function(o) {
        this.enabled = o.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj, 'jxDisabled' );
        } else {
            Element.addClassName( this.domObj, 'jxDisabled' );
        }
    }
} );

var JxTabBar = Class.create();
JxTabBar.prototype = {
    tabs : null,
    contents : null,
    tabBar : null,
    tabBox : null,
    sl: null,
    initialize : function(domObj) {
        if (arguments[1]) {
            this.tabContentBox = $(arguments[1]);
        } else {
            this.tabContentBox = $(domObj);
        }
        this.tabBox = $(domObj);
        this.tabBar = document.createElement('ul');
        this.tabBar.className = 'tabBar';
        this.tabBox.appendChild(this.tabBar);
        this.sl = [];
    },
    addTab : function() {
        for (var i=0; i<arguments.length; i++) {
            var tab = arguments[i];
            tab.addSelectionListener(this);
            this.tabBar.appendChild(tab.domObj);
            this.tabContentBox.appendChild(tab.content);
            if (!this.activeTab) {
                this.setActiveTab(tab);
            }
        }
    },
    removeTab : function(tab) {
        //TODO
    },
    setActiveTab: function(tab) {
        if (this.activeTab) {
            Element.removeClassName(this.activeTab.domObj.childNodes[0], 'tabActive');
            Element.removeClassName(this.activeTab.content, 'tabContentActive');
        }
        this.activeTab = tab;
        Element.addClassName(this.activeTab.domObj.childNodes[0], 'tabActive');
        Element.addClassName(this.activeTab.content, 'tabContentActive');
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
Object.extend(JxTabBar.prototype, JxListenerBase.prototype);

var JxTab = Class.create();
JxTab.prototype = {
    domObj: null,
    content: null,
    name: null,
    sl: null,
    initialize : function(name, options) {
        options = options || {};
        this.name = name;
        this.content = document.createElement('div');
        this.content.className = 'tabContent';
        this.onContentReady = options.onContentReady || null;
        if (options.contentHTML) {
          this.content.innerHTML = options.contentHTML;
        } else if (options.content) {
          this.content = options.content;
        } else if (options.contentURL) {
            var ts = 'ts=' + (new Date()).getTime();
            
            var a = new Ajax.Request( options.contentURL, 
                Object.extend({method:'get',onComplete:this.contentLoaded.bind(this), parameters: ts}));
        }
        this.sl = [];
        this.domObj = document.createElement('li');
        var a = document.createElement('a');
        a.innerHTML = name;
        a.href="#";
        a.onclick = this.clicked.bind(this);
        this.domObj.appendChild(a);
    },
    contentLoaded: function(r) {
      this.content.innerHTML = r.responseText;
      if (this.onContentReady) {
          /* need to use setTimeout to give IE a chance to render the content */
          window.setTimeout(this.onContentReady.bind(this),1);
      }
    },
    clicked: function() {
        this.processEvent(this.sl, 'selectionChanged', this);
    },
    addSelectionListener: function(o) {
        this.addListener(this.sl, o);
    },
    removeSelectionListener: function(o) {
        this.removeListener(this.sl, o);
    }
};
Object.extend(JxTab.prototype, JxListenerBase.prototype);

var JxPicker = Class.create();
JxPicker.prototype = {
    /** selection listeners */
    sl : null,
    /** the div that contains the control, used to show/hide the control */
    domObj : null,
    /** the ul that contains the selectable items */
    ul : null,
    /** current selection in the list */
    currentSelection : null,
    /** constructor */
    initialize: function(obj) {
        this.domObj = document.createElement('div');
        this.domObj.className = 'stylePickerContainer';
        
        if (!window.opera) {
            //insert iframeShim for IE to cover other controls in the page.
            var iframe = document.createElement('iframe');
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.domObj.appendChild(iframe);
        }
        
        var d2 = document.createElement('div');
        d2.className = 'stylePicker';
        this.ul = document.createElement('ul');
        d2.appendChild(this.ul);
        
        this.domObj.appendChild(d2);
        this.domObj.style.display = 'none';
        obj.appendChild(this.domObj);
        this.sl = [];        
    },
    /**
     * add a new item to the pick list
     *
     * @param text {String} the html to display in the list
     * @param value {String} the value to pass to the selection listener
     * @param backgroundImage {String} a background image for the item,
     *        applied using a CSS background-image style
     * @param selected {boolean} whether the item is initially selected or not
     */
    add: function(text, value, backgroundImage, selected) {
        if (selected && this.currentSelection) {
            Element.removeClassName(this.currentSelection, 'selected');
        }
        var li = document.createElement('li');
        var a = document.createElement('a');
        a.href="#";
        if (selected) {
            a.className = 'selected';
            this.currentSelection = a;
        }
        if (backgroundImage && backgroundImage != '') {
            a.style.backgroundImage = backgroundImage;
        }
        a.name = value;
        a.innerHTML = text;
        Event.observe(a, 'click', this.pick.bindAsEventListener(this));
        li.appendChild(a);
        this.ul.appendChild(li);
    },
    /**
     * user has clicked something in the list, select it
     *
     * @param e {Event} the mouse event from the user's click
     */
    pick: function(e) {
        var target = Event.element(e);
        if (target.name) {
            if (this.currentSelection && this.currentSelection != target) {
                Element.removeClassName(this.currentSelection, 'selected');
            }
            this.currentSelection = target;
            this.processEvent(this.sl, 'selectionChanged', this, target.name);
        }
    },
    open: function(selection) {
        if (!this.keypressListener) {
            this.keypressListener = this.keypress.bindAsEventListener(this);
        }
        this.domObj.style.display = 'block';
        Event.observe(document, 'keypress', this.keypressListener);
        //Effect.SlideDown(this.domObj, 0.2);
    },
    keypress: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.close();
        }
    },
    close: function() {
        this.domObj.style.display = 'none';
        Event.stopObserving(document, 'keypress', this.keypressListener);
        //Effect.SlideUp(this.domObj, 0.2);
    },
    addSelectionListener: function(o) {
        this.addListener(this.sl, o);
    },
    removeSelectionListener: function(o) {
        this.removeListener(this.sl, o);
    },
    getSelection: function() {
        return this.currentSelection ? this.currentSelection.name : '';
    }
};
Object.extend(JxPicker.prototype, JxListenerBase.prototype);

Object.extend( Element, {
    /** inspired by dojotoolkit style.js */
    getBoxSizing : function(elem) {
      //jxTrace('Element.getBoxSizing()');
      var result = 'content-box';
      elem = $(elem);
      if (elem.currentStyle || window.opera) { 
          var cm = document["compatMode"];
          //jxTrace('compatMode:'+cm);
          if (cm == "BackCompat" || cm == "QuirksMode") { 
              result = 'border-box'; 
          } else {
              result = 'content-box'; 
        }
      } else {
          //jxTrace('no compatMode');
          
          if (arguments.length == 0) {
              node = document.documentElement; 
          }
          var sizing = Element.getStyle(elem, "-moz-box-sizing");
          if (!sizing) { 
              sizing = Element.getStyle(elem, "box-sizing"); 
          }
          result = (sizing ? sizing : 'content-box');
      }
      //jxTrace('Element.getBoxSizing() returning ' + result);
      return result;
    },
    getContentBoxSize : function(elem) {
      elem = $(elem);
      //jxTrace('Element.getContentBoxSize()');
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      var padding = Element.getPaddingSize(elem);
      var border = Element.getBorderSize(elem);
      w = w - padding.left - padding.right - border.left - border.right;
      h = h - padding.bottom - padding.top - border.bottom - border.top;
      //jxTrace(elem.id + ': Element.getContentBoxSize() returning width:' + w + ', height:' +h);
      return {width: w, height: h};
    },
    getBorderBoxSize: function(elem) {
      elem = $(elem);
      //jxTrace(elem.id + ': Element.getBorderBoxSize()');
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      /*
      if (!elem.currentStyle ) {
        var padding = Element.getPaddingSize(elem);
        var border = Element.getBorderSize(elem);
        w = w + padding.left + padding.right + border.left + border.right;    
      }
      */
      //jxTrace(elem.id + ': Element.getBorderBoxSize() width:' + w + ', height:' +h);
      return {width: w, height: h}; 
    },
    setContentBoxSize : function(elem, size) {
        //jxTrace(elem.id + ': setContentBoxSize('+size.width+','+size.height+')');
      elem = $(elem);
      if (Element.getBoxSizing(elem) == 'border-box') {
        var padding = Element.getPaddingSize(elem);
        var border = Element.getBorderSize(elem);
        if (typeof size.width != 'undefined') {
          var width = (size.width + padding.left + padding.right + border.left + border.right);
          if (width < 0) {
            width = 0;
          }
          elem.style.width = width + 'px';
        }
        if (typeof size.height != 'undefined') {
          var height = (size.height + padding.top + padding.bottom + border.top + border.bottom);
          if (height < 0) {
            height = 0;
          }
          elem.style.height = height + 'px';
        }
      } else {
        if (typeof size.width != 'undefined') {
          //jxTrace('Element.setBorderBoxSize() setting width to ' + size.width);
          elem.style.width = size.width + 'px';
        }
        if (typeof size.height != 'undefined') {
          //jxTrace('Element.setBorderBoxSize() setting height to ' + size.height);
          elem.style.height = size.height + 'px';
        }
      }

    },
    setBorderBoxSize : function(elem, size) {
      elem = $(elem);
      //jxTrace(elem.id + ': Element.setBorderBoxSize(elem, '+size.width + ','+size.height+')' );
      if (Element.getBoxSizing(elem) == 'content-box') {
        //jxTrace('Element.setBorderBoxSize() using content-model sizing');
        var padding = Element.getPaddingSize(elem);
        var border = Element.getBorderSize(elem);
        if (typeof size.width != 'undefined') {
          var width = (size.width - padding.left - padding.right - border.left - border.right);
          if (width < 0) {
            width = 0;
          }
          //jxTrace('Element.setBorderBoxSize() setting width to ' + width);
          elem.style.width = width + 'px';
        }
        if (typeof size.height != 'undefined') {
          var height = (size.height - padding.top - padding.bottom - border.top - border.bottom);
          if (height < 0) {
            height = 0;
          }
          //jxTrace('Element.setBorderBoxSize() setting height to ' + height);
          elem.style.height = height + 'px';
        }
      } else {
        //jxTrace('Element.setBorderBoxSize() using box-model sizing');
        if (typeof size.width != 'undefined' && size.width >= 0) {
          //jxTrace('Element.setBorderBoxSize() setting width to ' + size.width);
          elem.style.width = size.width + 'px';
        }
        if (typeof size.height != 'undefined' && size.height >= 0) {
          //jxTrace('Element.setBorderBoxSize() setting height to ' + size.height);
          elem.style.height = size.height + 'px';
        }
      }
    },
    getPaddingSize : function (elem) {
      elem = $(elem);
      var bShow = false;
      var oldV, oldP;
      if (elem.style.display == 'none') {
        bShow = true;
        oldV = elem.style.visibility;
        oldP = elem.style.position;
        elem.style.position = 'absolute';
        elem.style.visibility = 'hidden';
        elem.style.display = 'block';
      }
      var l = Element.getNumber(Element.getStyle(elem, 'padding-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'padding-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'padding-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'padding-bottom'));
  
      if (bShow) {
        elem.style.display = 'none';
        elem.style.visibility = oldV;
        elem.style.position = oldP;
      }
      //jxTrace( elem.id + ': padding: left:' + l + ', top:' + t + ', right:' + r + ', bottom:' + b );
      return {left:l, top:t, right: r, bottom: b};
    },

    getBorderSize : function(elem) {
      elem = $(elem);
      var bShow = false;
      var oldV,oldP;
      if (elem.style.display == 'none') {
        bShow = true;
        oldV = elem.style.visibility;
        oldP = elem.style.position;
        elem.style.position = 'absolute';
        elem.style.visibility = 'hidden';
        elem.style.display = 'block';
      }
      var l = Element.getNumber(Element.getStyle(elem, 'border-left-width'));
      var t = Element.getNumber(Element.getStyle(elem, 'border-top-width'));
      var r = Element.getNumber(Element.getStyle(elem, 'border-right-width'));
      var b = Element.getNumber(Element.getStyle(elem, 'border-bottom-width'));
      
      if (bShow) {
        elem.style.display = 'none';
        elem.style.visibility = oldV;
        elem.style.position = oldP;
      }
      //jxTrace( elem.id + ': border: left:' + l + ', top:' + t + ', right:' + r + ', bottom:' + b  );
      return {left:l, top:t, right: r, bottom: b};
    },

    getMarginSize : function(elem) {
      elem = $(elem);
      var bShow = false;
      var oldV,oldP;
      if (elem.style.display == 'none') {
        bShow = true;
        oldV = elem.style.visibility;
        oldP = elem.style.position;
        elem.style.position = 'absolute';
        elem.style.visibility = 'hidden';
        elem.style.display = 'block';
      }
      var l = Element.getNumber(Element.getStyle(elem, 'margin-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'margin-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'margin-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'margin-bottom'));
      if (bShow) {
        elem.style.display = 'none';
        elem.style.visibility = oldV;
        elem.style.position = oldP;
      }
      //jxTrace( 'margin: left:' + l + ', top:' + t + ', right:' + r + ', bottom:' + b );
      return {left:l, top:t, right: r, bottom: b};
    },
    getNumber: function(n) {
      var result = n==null||isNaN(parseInt(n))?0:parseInt(n);
      //jxTrace('Element.getNumber('+n+') returning ' + result);
      return result;
    },
    getInsideWindowWidth: function() {
        if (window.innerWidth) {
            return window.innerWidth;
        } else if (document.compatMode && document.compatMode.indexOf("CSS1") >= 0) {
            // measure the html element's clientWidth
            return document.body.parentElement.clientWidth;
        } else if (document.body && document.body.clientWidth) {
            return document.body.clientWidth;
        }
        return 0; 
    },
    getInsideWindowHeight: function() {
        if (window.innerHeight) {
            return window.innerHeight;
        } else if (document.compatMode && document.compatMode.indexOf("CSS1") >= 0) {
            // measure the html element's clientHeight
            return document.body.parentElement.clientHeight;
        } else if (document.body && document.body.clientHeight) {
            return document.body.clientHeight;
        }
        return 0; 
    } 
} );


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
        //jxTrace('DomNode::initialize() - ' + this.nodeName + ' (' + this.textContent + ') with ' + this.attributes.length + ' attributes and with ' + this.childNodes.length + ' child nodes.');
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
        //jxTrace('processing node .' + this.nodeName + '.');
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
};