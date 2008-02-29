/**********************************************************************
 *
 * $Id: jxcore.js 500 2008-02-15 19:51:57Z madair $
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
 * Class: Jx
 * Jx is a global singleton object that contains the entire Jx library
 * within it.  All Jx functions, attributes and classes are accessed
 * through the global Jx object.  Jx should not create any other
 * global variables, if you discover that it does then please report
 * it as a bug
 */
 
/* LEAVE THIS SEMI-COLON, IT PREVENTS PROBLEMS WITH COMPRESSION */
;

/* firebug console supressor for IE/Safari/Opera */
if (!("console" in window) || !("firebug" in console)) {
    var names = ["log", "debug", "info", "warn", "error", "assert", "dir", "dirxml",
    "group", "groupEnd", "time", "timeEnd", "count", "trace", "profile", "profileEnd"];

    window.console = {};
    for (var i = 0; i < names.length; ++i) {
        window.console[names[i]] = function() {};
    }
}
/* inspired by extjs, removes css image flicker and related problems in IE 6 */
var ua = navigator.userAgent.toLowerCase();
var isIE = ua.indexOf("msie") > -1,
    isIE7 = ua.indexOf("msie 7") > -1;
if(isIE && !isIE7) {
    try {
        document.execCommand("BackgroundImageCache", false, true);
    } catch(e) {}
}

/* Setup global namespace
 * If jxcore is loaded by jx.js, then the namespace and baseURL are
 * already established
 */
if (typeof Jx == 'undefined') {
    var Jx = {};
    /**
     * Property: {Boolean} COMBINED_CSS
     * controls whether Jx uses a single, combined CSS file or
     * individual ones.  The combined version is used automatically
     * if the combined or compressed version of Jx is used,
     * otherwise separate ones are used.
     */ 
    Jx.COMBINED_CSS = true;
    var aScripts = document.getElementsByTagName('SCRIPT');
    for (var i=0; i<aScripts.length; i++) {
        var s = aScripts[i].src;
        //only check for lib/jx because we were loaded by 
        var n = s.indexOf('lib/jx');
        if (n != -1) {
            /**
             * Property: {String} baseURL
             * This is the URL that Jx was loaded from, it is 
             * automatically calculated from the script tag
             * src property that included Jx.
             */ 
            Jx.baseURL = s.substring(0,n);
            break;
        }
    }
} 

/**
 * Property: {Object} importRules
 *
 * an object containing a list of CSS files to be included
 * to make the loaded Jx components function correctly
 */
Jx.importRules = {};
/**
 * Property: {Object} importRules
 *
 * an object containing a list of CSS files to be included
 * to make the loaded Jx components function correctly
 */
Jx.importRulesIE = {};

/**
 * Function: addStyleSheet
 *
 * Individual components of Jx call this function to get their
 * style sheets imported at run time.
 *
 * Parameters:
 *
 *   styleSheet {String} the relative path to the CSS file (relative
 *              to <Jx.baseURL>).
 *
 *   ieOnly {Boolean} if true, then the style sheet is only loaded 
 *          if the browser is Internet Explorer.
 */
Jx.addStyleSheet = function(styleSheet, ieOnly) { 
    if (ieOnly) {
        this.importRulesIE[styleSheet] = styleSheet; 
    } else {
        this.importRules[styleSheet] = styleSheet; 
    }
};

/* everything needs reset.css */
Jx.addStyleSheet('reset.css');

/**
 * Function: applyPNGFilter
 *
 * Static method that applies the PNG Filter Hack for IE browsers
 * when showing 24bit PNG's.  Used automatically for img tags with
 * a class of png24.
 *
 * The filter is applied using a nifty feature of IE that allows javascript to
 * be executed as part of a CSS style rule - this ensures that the hack only
 * gets applied on IE browsers.
 *
 * Parameters:
 *
 *   object {Object} the object (img) to which the filter needs to be applied.
 */
Jx.applyPNGFilter = function(o)  {
   var t=Jx.baseURL + "images/a_pixel.png";
   if( o.src != t ) {
       var s=o.src;
       o.src = t;
       o.runtimeStyle.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='"+s+"',sizingMethod='scale')";
   }
};

Jx.imgQueue = [];   //The queue of images to be loaded
Jx.imgLoaded = {};  //a hash table of images that have been loaded and cached
Jx.imagesLoading = 0; //counter for number of concurrent image loads 

/**
 * Method: addToImgQueue
 *
 * request that an image be set to a DOM IMG element src attribute.  This puts 
 * the image into a queue and there are private methods to manage that queue
 * and limit image loading to 2 at a time.
 */
Jx.addToImgQueue = function(obj) {
    if (Jx.imgLoaded[obj.src]) {
        //if this image was already requested (i.e. it's in cache) just set it directly
        obj.domElement.src = obj.src;
    } else {
        //otherwise stick it in te queue
        Jx.imgQueue.push(obj);
        Jx.imgLoaded[obj.src] = true;
    }
    //start the queue management process
    Jx.checkImgQueue();
};

/**
 * Method: checkImgQueue
 *
 * An internal method that ensures no more than 2 images are loading at a time.
 */
Jx.checkImgQueue = function() {
    //console.log(this.imgQueue.length+":"+this.imagesLoading);
    while (Jx.imagesLoading < 2 && Jx.imgQueue.length > 0) {
        Jx.loadNextImg();
    }
};

/**
 * Method: loadNextImg
 *
 * An internal method actually populate the DOM element with the image source.
 */
Jx.loadNextImg = function() {
    var obj = Jx.imgQueue.shift();
    if (obj) {
        ++Jx.imagesLoading;
        obj.domElement.onload = function(){--Jx.imagesLoading; Jx.checkImgQueue();};
        obj.domElement.onerror = function(){--Jx.imagesLoading; Jx.checkImgQueue();};
        obj.domElement.src = obj.src;
    }
}; 


/**
  * Class: Jx.Listener
 *
 * Jx.Listener is a mix-in class that performs common listener functions
 * for objects that support listeners.  It is intended to be added to
 * existing classes using the following syntax:
 *
 * (code)
 * Object.extends( MyClass.prototype, Jx.Listener.prototype)
 * (end)
 *
 * The Jx.Listener class functions provide support for managing a list of
 * listeners (add, remove) and dispatching events to listeners (processEvent).
 */
Jx.Listener = Class.create();
Jx.Listener.prototype = {
    /**
     * Function: addListener
     *
     * add a listener to the provided list.
     *
     * Parameters:
     *
     *   list {Array} the array of listeners to add the listener to.
     *
     *   object {Object} the object to add as a listener.
     */
    addListener: function (list,obj) {list.push(obj);},
    /**
     * Function: removeListener
     *
     * remove a listener from the provided list.
     *
     * Parameters:
     *
     * list {Array} the array of listeners to remove the listener from.
     *
     * obj {Object} the object to remove as a listener.
     */
    removeListener: function(list,obj) {
        for(var i=0;i<list.length;i++) {if (list[i] == obj) {list.splice(i,1); break;}}},
    /**
     * Function: processEvent
     *
     * call each listener with a given method and event.
     *
     * Parameters:
     *
     *   list {Array} the array of listeners to call.
     *
     *   fnName {String} the name of a function to call on each listener.
     *
     *   obj {Object} an object to pass to each listener.
     */
    processEvent: function(list,fnName,obj){list.each(function(o){if (o[fnName]) {o[fnName](obj);}});}
};

/**
 * Class: Jx.UniqueId
 *
 * Jx.UniqueId is used to assign unique ids to selected elements
 * This is used to solve a problem where multiple external html
 * fragments are loaded into the DOM via ajax at runtime.  It is
 * not always possible to ensure that every element has a unique
 * id.  This is not a problem if you are using id for CSS styling
 * but if you are using it to access elements using $() then
 * you may get unexpected results.
 *
 * Jx.UniqueId is a mix-in class.  Extend an existing class to
 * enable it to handle unique ids.  Register the ids that you
 * want to be unique and then get a reference to those objects
 * through the interface exposed by this class.
 *
 * The class retrieves the elements by id by walking a dom object
 * and retains references to each of the actual DOM objects
 * you have registered.
 */
Jx.UniqueId = Class.create();
Jx.UniqueId.prototype = {
    /**
     * Property: {Array} uniqueIdRefs
     *
     * an array of references obtained from by registering ids
     */
    uniqueIdRefs: null,
    /**
     * Function: initUniqueId
     * 
     * initialize the UniqueId object.  This must be called prior to
     * calling the <registerIds> function.  Typically, it is called
     * in the constructor of an object that includes Jx.UniqueId.
     */
    initUniqueId: function() { 
        this.uniqueIdRefs = [];
    },
    /**
     * Function: deregisterIds
     *
     * removes all registered ids
     */
    deregisterIds: function() {
        this.uniqueIdRefs.length = 0;
    },
    /**
     * Function: registerIds
     *
     * searches the domObj for each of the ids passed in and
     * obtains a unique reference to them so that subsequent
     * calls to <getObj> will return the right object.
     *
     * Parameters:
     *
     * aIds {Array} an array of strings containing ids of DOM elements
     *      to register.
     *
     * domObj {Object} an HTML element reference to search for unique
     *        ids within
     */
    registerIds: function (aIds, domObj) {
        if (aIds.indexOf(domObj.id) != -1) {
            this.uniqueIdRefs[domObj.id] = domObj;
        }
        for (var i=0; i<domObj.childNodes.length; i++) {
            this.registerIds(aIds, domObj.childNodes[i]);
        }
    },
    /**
     * Function: getObj
     *
     * return an object by id if it was previously registered
     *
     * Parameters:
     * 
     * id {String} the original registered id to get the DOM object for
     *
     * Returns:
     *
     * {Object} returns an object or null if the requested id did not
     * exist in the original DOM object or if the id was not registered.
     */
    getObj: function(id) {
        return this.uniqueIdRefs[id] || null;
    }
};

/**
 * Class: Jx.Action
 *
 * Jx.Action is a utility class that provides a mechanism to separate
 * the user interface code from the implementation code for a particular
 * piece of functionality.  A Jx.Action is used primarily as the basis for
 * clickable UI elements such as Jx.Button and Jx.MenuItem that need to
 * execute a particular function when the user clicks them.  The Jx.Action
 * includes a mechanism for controlling the state of the action, allowing
 * an application to enable or disable an action at any time.  A single
 * Jx.Action may be used with multiple buttons and menu items, allowing 
 * the application to easily keep various user interface elements
 * synchronized without having to explicitly maintain all of them.
 *
 * A new instance of Jx.Action is created by passing a function object
 * to the constructor.  The function object may be a function name or
 * the result of using the Prototype bind() function.
 *
 * For example:
 *
 * (code)
 * //Example 1:
 * //use a function reference directly
 * function myFunction() { alert('my function'); }
 * var action = new Jx.Action(myFunction);
 *
 * //Example 2:
 * //use a function bound to an object through bind()
 * var myClass = Class.create();
 * myClass.prototype = { 
 *   initialize: function() {this.x = 1;}, 
 *   getX: function() {alert(this.x); }
 * };
 *
 * var myInstance = new myClass();
 * var action = new Jx.Action(myInstance.getX.bind(myInstance));
 *
 * (end)
 *
 * To enable or disable a Jx.Action (and consequently update any associated
 * buttons or menu items), use the setEnabled([true|false]) method.
 *
 * For example:
 *
 * (code)
 * //disable an action
 * action.setEnabled(false);
 * (end)
 */
Jx.Action = Class.create();
Jx.Action.prototype = {
    /**
     * Property: {Array} pcl
     * an array of property change listeners attached to this action
     */
    pcl: null,
    /**
     * Property: {Boolean} enabled
     *
     * whether the action (and its associated interface object) is
     * currently enabled or not.  This is controlled through the
     * setEnabled function
     */
    enabled : null,
    /**
     * Constructor: initialize
     * 
     * construct a new instance of Jx.Action that invokes a function
     * when activated
     *
     * Parameter: {Function} f
     *
     * the function that this action triggers
     */
    initialize: function(f) {
        this.pcl = [];
        this.enabled = true;
        this.actionPerformed = f;
    },
    /**
     * Function: addPropertyChangeListener
     *
     * add a property change listener to this action.  When the enabled
     * state of the action changes, all property change listeners are
     * notified through their propertyChanged method.
     *
     * Parameter: {Object} obj
     *
     * the object to notify of property changes
     */
    addPropertyChangeListener: function(obj){this.addListener(this.pcl, obj);},
    /**
     * Function: removePropertyChangeListener
     *
     * remove a property change listener from this action.
     *
     * Parameter: {Object} obj
     *
     * the property change listener to remove.
     */
    removePropertyChangeListener: function(obj) {
        this.removeListener(this.pcl, obj);
    },
    /**
     * Function: isEnabled
     * 
     * return whether the action is currently enabled or not.
     *
     * Return: {Boolean}
     */
    isEnabled: function() {return this.enabled;},
    /**
     * Function: setEnabled
     *
     * set the state of this action.
     *
     * Parameter: {Boolean} b
     *
     * a boolean value to set enabled state of this action to.
     */
    setEnabled: function(b){
        /* prevent unnecessary propogation of propertyChanged */
        if (this.enabled == b) {
            return;
        }
        this.enabled = b;
        this.processEvent(this.pcl,'propertyChanged',this);
    },
    /**
     * Function: bindTo
     *
     * convenience function to bind an item to this action.  This
     * adds the item as a property change listener to the action
     * and adds the action as an ActionListener to the item.
     *
     * Parameter: {Object} item
     *
     * the object to bind the action to.
     */
    bindTo : function( item ) {
        this.addPropertyChangeListener(item);
        item.addActionListener(this);
    },
    /**
     * Function: unbindFrom
     *
     * convenience function to undo a binding between an object and
     * this action.
     *
     * Parameter: {Object} item
     *
     * the object to unbind from this action.
     */
    unbindFrom: function(item) {
        this.removePropertyChangeListener(item);
        item.removeActionListener(this);
    },
    /**
     * Function: actionPerformed
     *
     * placeholder function to conform to the ActionListener interface.
     *
     * Parameter: {Object} obj
     *
     * the object that performed the action.
     */
    actionPerformed : function(obj) { alert('actionPerformed'); }
};
Object.extend(Jx.Action.prototype, Jx.Listener.prototype);

/**
 * Class: Element
 *
 * Element is a global object provided by the prototype.js library.  The
 * functions documented here are extensions to the Element object provided
 * by Jx to make cross-browser compatibility easier to achieve.
 */
Object.extend( Element, {
    /**
     * Function: getBoxSizing
     *
     * return the box sizing of an element, one of 'content-box' or 
     *'border-box'.
     *
     * Parameter: {Object} elem
     *
     * the element to get the box sizing of.
     *
     * Return: {String} the box sizing of the element.
     */
    getBoxSizing : function(elem) {
      var result = 'content-box';
      elem = $(elem);
      if (elem.currentStyle || window.opera) { 
          var cm = document["compatMode"];
          if (cm == "BackCompat" || cm == "QuirksMode") { 
              result = 'border-box'; 
          } else {
              result = 'content-box'; 
        }
      } else {
          if (arguments.length == 0) {
              node = document.documentElement; 
          }
          var sizing = Element.getStyle(elem, "-moz-box-sizing");
          if (!sizing) { 
              sizing = Element.getStyle(elem, "box-sizing"); 
          }
          result = (sizing ? sizing : 'content-box');
      }
      return result;
    },
    /**
     * Function: getContentBoxSize
     *
     * return the size of the content area of an element.  This is the size of
     * the element less margins, padding, and borders.
     *
     * Parameter: {Object} elem
     *
     * the element to get the content size of.
     *
     * Return: {Object} an object with two properties, width and height, that
     * are the size of the content area of the measured element.
     */
    getContentBoxSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      var padding = Element.getPaddingSize(elem);
      var border = Element.getBorderSize(elem);
      Element.toggleMeasurable(elem);
      w = w - padding.left - padding.right - border.left - border.right;
      h = h - padding.bottom - padding.top - border.bottom - border.top;
      return {width: w, height: h};
    },
    /**
     * Function: getBorderBoxSize
     *
     * return the size of the border area of an element.  This is the size of
     * the element less margins.
     *
     * Parameter: {Object} elem
     *
     * the element to get the border sizing of.
     *
     * Return: {Object} an object with two properties, width and height, that
     * are the size of the border area of the measured element.
     */
    getBorderBoxSize: function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var w = elem.offsetWidth;
      var h = elem.offsetHeight;
      Element.toggleMeasurable(elem);
      return {width: w, height: h}; 
    },
    /**
     * Function: setContentBoxSize
     *
     * set either or both of the width and height of an element to
     * the provided size.  This function ensures that the content
     * area of the element is the requested size and the resulting
     * size of the element may be larger depending on padding and
     * borders.
     *
     * Parameter: {Object} elem
     *
     * the element to set the content area of.
     *
     * Parameter: {Object} size
     *
     * an object with a width and/or height property that is the size to set
     * the content area of the element to.
     */
    setContentBoxSize : function(elem, size) {
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
                elem.style.width = size.width + 'px';
            }
            if (typeof size.height != 'undefined') {
                elem.style.height = size.height + 'px';
            }
        }
    },
    /**
     * Function: setBorderBoxSize
     *
     * set either or both of the width and height of an element to
     * the provided size.  This function ensures that the border
     * size of the element is the requested size and the resulting
     * content areaof the element may be larger depending on padding and
     * borders.
     *
     * Parameter: {Object} elem
     *
     * the element to set the border size of.
     *
     * Parameter: {Object} size
     *
     * an object with a width and/or height property that is the size to set
     * the content area of the element to.
     */
    setBorderBoxSize : function(elem, size) {
      elem = $(elem);
      if (Element.getBoxSizing(elem) == 'content-box') {
        var padding = Element.getPaddingSize(elem);
        var border = Element.getBorderSize(elem);
        var margin = Element.getMarginSize(elem);
        if (typeof size.width != 'undefined') {
          var width = (size.width - padding.left - padding.right - border.left - border.right - margin.left - margin.right);
          if (width < 0) {
            width = 0;
          }
          elem.style.width = width + 'px';
        }
        if (typeof size.height != 'undefined') {
          var height = (size.height - padding.top - padding.bottom - border.top - border.bottom - margin.top - margin.bottom);
          if (height < 0) {
            height = 0;
          }
          elem.style.height = height + 'px';
        }
      } else {
        if (typeof size.width != 'undefined' && size.width >= 0) {
          elem.style.width = size.width + 'px';
        }
        if (typeof size.height != 'undefined' && size.height >= 0) {
          elem.style.height = size.height + 'px';
        }
      }
    },
    /**
     * Function: getPaddingSize
     *
     * returns the padding for each edge of an element
     *
     * Parameter: elem
     *
     * The element to get the padding for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated padding values.
     */
    getPaddingSize : function (elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'padding-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'padding-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'padding-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'padding-bottom'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Function: getBorderSize
     *
     * returns the border size for each edge of an element
     *
     * Parameter: elem
     *
     * The element to get the borders for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated border values.
     */
    getBorderSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'border-left-width'));
      var t = Element.getNumber(Element.getStyle(elem, 'border-top-width'));
      var r = Element.getNumber(Element.getStyle(elem, 'border-right-width'));
      var b = Element.getNumber(Element.getStyle(elem, 'border-bottom-width'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Function: getMarginSize
     *
     * returns the margin size for each edge of an element
     *
     * Parameter: elem
     *
     * The element to get the margins for.
     *
     * Return: {Object} an object with properties left, top, right and bottom
     * that contain the associated margin values.
     */
    getMarginSize : function(elem) {
      elem = $(elem);
      Element.toggleMeasurable(elem);
      var l = Element.getNumber(Element.getStyle(elem, 'margin-left'));
      var t = Element.getNumber(Element.getStyle(elem, 'margin-top'));
      var r = Element.getNumber(Element.getStyle(elem, 'margin-right'));
      var b = Element.getNumber(Element.getStyle(elem, 'margin-bottom'));
      Element.toggleMeasurable(elem);
      return {left:l, top:t, right: r, bottom: b};
    },
    /**
     * Function: getNumber
     *
     * safely parse a number and return its integer value.  A NaN value 
     * returns 0.  CSS size values are also parsed correctly.
     *
     * Parameter: {Mixed} n
     *
     * the string or object to parse.
     *
     * Return: {Integer} the integer value that the parameter represents
     */
    getNumber: function(n) {
      var result = n==null||isNaN(parseInt(n))?0:parseInt(n);
      return result;
    },
    /**
     * Function: getPageDimensions
     *
     * return the dimensions of the browser client area.
     *
     * Return: {Object} an object containing a width and height property 
     * that represent the width and height of the browser client area.
     */
    getPageDimensions: function() {
        return {width: Element.getInsideWindowWidth(), height: Element.getInsideWindowHeight()};
    },
    /**
     * Function: getInsideWindowWidth
     *
     * returns the width of the browser client area
     *
     * Return: {Integer} the width of the browser client area
     */
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
    /**
     * Function: getInsideWindowHeight
     *
     * returns the height of the browser client area
     *
     * Return: {Integer} the height of the browser client area
     */
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
    },
    /**
     * Function: toggleMeasurable
     *
     * toggles an element's display style property so it can be
     * measured.  If the element has display: none, it is
     * changed to display: block and made temporarily visible
     * so that it can be measured.  Calling this function
     * a second time with the same element will revert the
     * changes.  This allows an element to be measured in
     * various ways.
     *
     * Parameter: {Object} elem
     *
     * the element to measure.
     */
    toggleMeasurable: function(elem) {
        if (Element.getStyle(elem, 'display') == 'none') {
            elem.old = {};
            elem.old.display = elem.style.display;
            elem.old.visibility = elem.style.visibility;
            elem.old.position = elem.style.position;
            elem.style.position = 'absolute';
            elem.style.visibility = 'hidden';
            elem.style.display = 'block';
        } else {
            if (elem.old) {
                elem.style.display = elem.old.display;
                elem.style.visibility = elem.old.visibility;
                elem.style.position = elem.old.position;
                elem.old = null;
            }
        }
    }
} );

/**
 * Class: Jx.ContentLoader
 * 
 * ContentLoader is a mix-in class that provides a consistent
 * mechanism for other Jx controls to load content in one of
 * four different ways:
 *
 * o using an existing element, by id
 *
 * o using an existing element, by object reference
 *
 * o using an HTML string
 *
 * o using a URL to get the content remotely
 */
Jx.ContentLoader = Class.create();
Jx.ContentLoader.prototype = {
    /**
     * Property: bContentLoaded
     *
     * tracks the load state of the content, specifically useful
     * in the case of remote content.
     */ 
    bContentLoaded: false,
    /**
     * Function: contentLoaded
     * 
     * callback function that handles remote content
     *
     * Parameter: element
     *
     * the element to put the content into
     *
     * Parameter: options
     *
     * the options that were passed to loadContent originally, only
     * used to get the optional onContentLoaded callback function.
     *
     * Parameter: r
     *
     * the XmlHttpRequest object that has the content.
     */
    contentLoaded: function(element, options, r) { 
        element.innerHTML = r.responseText;
        this.bContentLoaded = true;
        if (options.onContentLoaded) {
            options.onContentLoaded();
        }
    },
    /**
     * Function: contentLoadFailed
     * 
     * callback function that handles failure to load remote content
     *
     * Parameter: options
     *
     * the options that were passed to loadContent originally, only
     * used to get the optional onContentLoadedFailed callback function.
     *
     * Parameter: r
     *
     * the XmlHttpRequest object that has the failure code
     */
    contentLoadFailed: function(options, r) {
        this.bContentLoaded = false;
        if (options.onContentLoadFailed) {
            options.onContentLoadFailed();
        }
    },
    /**
     * Function: loadContent
     *
     * triggers loading of content based on parameters passed in the
     * options parameter.  The options parameter can have the following
     * attributes:
     *
     * content - an HTMLElement object, it is appended to the element.
     *
     * contentID - a string containing the ID of an HTML element to append
     * to the element.
     * 
     * contentHTML - a string containing HTML that is inserted as the innerHTML
     * of the element.
     *
     * contentURL - a string containing a URL to some content.  The content is
     * loaded using an XmlHttpRequest and inserted into the element when it is
     * loaded.  The url *must* be to the same domain as the current page,
     * otherwise the same domain policy will prevent loading of the content.
     *
     * onContentLoaded - a function object that is called when the content has
     * been successfully loaded.  Primarily useful when using the contentURL
     * method of loading content.
     *
     * onContentLoadFailed - a function object that is called if the content
     * fails to load, primarily useful when using the contentURL method of
     * loading content.
     *
     * Parameter: element
     *
     * the element to insert the content into
     *
     * Parameter: options
     *
     * an object containing the attributes indicating what content to load.
     */     
    loadContent: function(element, options) {
        options = options || {};
        element = $(element);
        if (options.content) {
            element.appendChild(options.content);
            this.bContentLoaded = true;
        } else if (options.contentID) {
            var c = $(options.contentID);
            if (c) {
                element.appendChild(c);
                this.bContentLoaded = true;                
            }
        } else if (options.contentURL) {
            this.bContentLoaded = false;
            var ts = '';//'ts=' + (new Date()).getTime();
            var a = new Ajax.Request( options.contentURL, 
                Object.extend({method:'get',
                               onSuccess:this.contentLoaded.bind(this, element, options), 
                               onFailure: this.contentLoadFailed.bind(this, options),
                               requestHeaders: ['If-Modified-Since', 'Sat, 1 Jan 2000 00:00:00 GMT'],
                               parameters: ts}));
        } else if (options.contentHTML) {
            element.innerHTML = options.contentHTML;
            this.bContentLoaded = true;
        } else {
            this.bContentLoaded = true;
        }
        if (this.bContentLoaded && options.onContentLoaded) {
            options.onContentLoaded();
        }
    }
};