/**********************************************************************
 *
 * $Id: jxbutton.js 450 2007-11-16 21:30:55Z madair $
 *
 * purpose: Implementation of a generic button widget and several
 *          useful subclasses.
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
 * Class: Jx.Button
 *
 * Jx.Button creates a clickable element in the application that is hooked
 * to an instance of Jx.Action.  When the button is clicked, it activates
 * its associated Jx.Action instance.  When the Jx.Action associated with
 * the button is enabled or disabled, the Jx.Button updates its visual
 * appearance to reflect the current state of the Jx.Action.
 *
 * Visually, a Jx.Button consists of an <A> tag that may contain either
 * an image, a label, or both (the label appears to the right of the button
 * if both are present).  The default styles for Jx.Button expect the
 * image to be 16 x 16 pixels, with a padding of 4px and a border of 1px
 * which results in an element that is 26 pixels high.  The width of the
 * button automatically accomodates the image and label as required.
 *
 * When you construct a new instance of Jx.Button, the button does not
 * automatically get inserted into the application.  Typically a button
 * is used as part of building another capability such as a Jx.Toolbar.
 * However, if you want to manually insert the button into your
 * application, you may use the domObj property of the Jx.Button instance.
 * In this case, you will use one of the DOM manipulation functions such
 * as appendChild, and pass button.domObj.  For example:
 *
 * (code)
 * var button = new Jx.Button(myAction, options);
 * var li = $('myListItem'); //obtain a reference to a DOM element
 * li.appendChild(button.domObj);
 * (end)
 *
 * To use a Jx.Button in an application, you must first create an instance
 * of Jx.Action and then pass it as the first parameter to the Jx.Button
 * constructor.  The second argument to a Jx.Button constructor is an options 
 * object allows configuring the button.
 *
 * (code)
 * Example:
 * function myFunc() { alert('you clicked me'); }
 * var action = new Jx.Action(myFunc);
 * var options = {
 *     imgPath: 'images/mybutton.png',
 *     tooltip: 'click me!',
 *     label: 'click me'
 * }
 * var button = new Jx.Button(action, options);
 * (end)
 */
 
Jx.addStyleSheet('button/button.css');

Jx.Button = Class.create();
Object.extend(Jx.Button.prototype, Jx.Listener.prototype);
Object.extend(Jx.Button.prototype, {
    /**
     * Property: {Array} al
     *
     * an array of action listeners that are attached to the button.
     */
    al: null,
    /**
     * Property: {Object} domObj
     *
     * the HTML element that is inserted into the DOM for this button
     */
    domObj: null,
    /**
     * Property: {Boolean} enabled
     *
     * whether the button is enabled or not.  This is controlled by
     * the Action object that the button is associated with.
     */
    enabled:null,
    /**
     * Function: initialize
     *
     * create a new button.
     *
     * Parameters:
     *
     * action {Object} the action to trigger for this button.
     *
     * options {Object} an object containing optional properties for this
     * button. It can have the following attributes:
     *
     * image - optional.  A string value that is the url to load the image to
     * display in this button.  The default styles size this image to 16 x 16.
     * If not provided, then the button will have no icon.
     *
     * tooltip - optional.  A string value to use as the alt/title attribute of
     * the <A> tag that wraps the button, resulting in a tooltip that appears 
     * when the user hovers the mouse over a button in most browsers.  If 
     * not provided, the button will have no tooltip.
     *
     * label - optional.  A string value that is used as a label on the button.
     *
     * disabledClass - optional.  A string value that is the name of a CSS class
     * to put on the <A> tag if the button is in a disabled state. If not 
     * provided, the default class jxDisabled is used. You may provide 
     * your own class or override jxDisabled.
     *
     * imgClass - optional.  A string value that is the name of a CSS class to
     * put on the <IMG> element.  The default value is jxButtonIcon.
     */
    initialize : function( action, options ) {
        options = options || {};
        this.al = [];
        this.action = action; //TODO move action into options
        
        var imgPath = (options.imgPath || options.image) || '';
        var tooltip = options.tooltip || '';
        var label = options.label || '';
        this.disabledClass = options.disabledClass || 'jxDisabled';
        
        this.domA = document.createElement('a');
        this.domA.className = 'jxButton';
        this.domA.href='javascript:void(0)'; //make hover effects work in IE
        this.domA.onclick = this.onclick.bindAsEventListener(this);
        this.domA.title = tooltip;
        this.domA.alt = tooltip;
        
        var span = document.createElement('span');
        span.className = 'jxButtonSpan';
        this.domA.appendChild(span);
        
        
        // if (imgPath != '') {
        //             this.domImg = document.createElement('span');
        //             this.domImg.className = 'jxButtonIcon';
        //             if (options.imageClass && options.imageClass != '') {
        //                 Element.addClassName(this.domImg, options.imageClass);
        //             }
        //             this.domImg.style.backgroundImage = "url("+imgPath+")";
        //             this.domImg.innerHTML = '&nbsp;&nbsp;';
        //             span.appendChild(this.domImg);
        //         }
        
        this.domLabel = document.createElement('span');
        this.domLabel.className = 'jxButtonContent';
        //this.domLabel.className = label != '' ? 'jxButtonLabel' : ''; //jxButtonLabel jxButtonEmptyLabel';
        this.domLabel.innerHTML = label != '' ? label : '&nbsp;';
        span.appendChild(this.domLabel);

        if (label != '') {
            Element.addClassName(this.domLabel, 'jxButtonLabel');
        }
        
        if (imgPath != '') {
            Element.addClassName(this.domLabel, 'jxButtonIcon');
            this.domLabel.style.backgroundImage = "url("+imgPath+")";
        }
        
        /* if this is an action button, then hook to the action */
        if (this.action) {
            this.action.bindTo(this);
            this.propertyChanged(this.action);            
        }
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxButtonContainer';
        this.domObj.appendChild(this.domA);
        
        if (options.halign && options.halign == 'left') {
                Element.addClassName(this.domObj, 'jxButtonContentLeft');                
        }
        if (options.valign && options.valign == 'top') {
            Element.addClassName(this.domObj, 'jxButtonContentTop');
        }
    },
    /**
     * Function: onclick
     * 
     * triggered when the user clicks the button, processes the
     * actionPerformed event
     */
    onclick : function() {
        if (this.enabled) {
            this.processEvent(this.al, 'actionPerformed', this);
        }
        return false;
    },
    /**
     * Function: addActionListener
     * 
     * add an action listener to the button
     *
     * Parameters: {Object} obj
     *
     * the object to add as an action listener
     */
    addActionListener: function(obj) { 
        this.addListener(this.al,obj); 
    },
    /**
     * Function: removeActionListener
     * 
     * remove an action listener from the button
     *
     * Parameter: {Object} obj
     *
     * the object to remove.
     */
    removeActionListener : function(obj) { 
        this.removeListener(this.al, obj);
    },
    /**
     * Function: propertyChanged
     *
     * implements the PropertyChangeListener interface
     * for handling the enabled state changing on the action
     * associated with this button
     *
     * Parameter: {Object} obj
     *
     * the action that is changing state
     */
    propertyChanged: function(obj) {
        this.enabled = obj.isEnabled();
        if (this.enabled) {
            Element.removeClassName( this.domObj, this.disabledClass );
        } else {
            Element.addClassName( this.domObj, this.disabledClass );
        }
    },
    /**
     * Function: setImage
     *
     * set the image of this button to a new image URL
     *
     * Parameter: {String} path
     *
     * the new url to use as the image for this button
     */
    setImage: function(path) {
        if (this.domImg) {
            this.domImg.src = path;
        }
    },
    /**
     * Function: setLabel
     * 
     * sets the text of the button.  Only works if a label was supplied
     * when the button was constructed
     *
     * Parameter: {String} label
     *
     * the new label for the button
     */
    setLabel: function(label) {
        if (this.domLabel) {
            this.domLabel.innerHTML = label;
        }
    },
    /**
     * Function: setTooltip
     *
     * sets the tooltip displayed by the button
     *
     * Parameter: {String} tooltip
     *
     * the new tooltip
     */
    setTooltip: function(tooltip) {
        if (this.domImg) {
            this.domImg.title = tooltip;
            this.domImg.alt = tooltip;
        }
    }
} );

/**
 * Class: Jx.Button.Flyout
 *
 * Flyout buttons expose a panel when the user clicks the button.  The
 * panel can have arbitrary content.
 *
 * The button itself does not trigger an external action.  You must provide
 * any necessary code to hook up elements in the panel to your application.
 */
Jx.Button.Flyout = Class.create();
Object.extend(Jx.Button.Flyout.prototype, Jx.Button.prototype);
Object.extend(Jx.Button.Flyout.prototype, Jx.ContentLoader.prototype);
Object.extend(Jx.Button.Flyout.prototype, {
    content: null,
    /**
     * Constructor: initialize
     *
     * construct a new instance of a flyout button.  The single options
     * argument takes the same parameters as <Jx.Button.initialize> plus
     * content loading options as per <Jx.ContentLoader>.
     *
     * Parameters: {Object} options
     *
     * an options object used to initialize the button
     */
    initialize: function(options) {
        options = options || {};
        var a = new Jx.Action(this.show.bind(this));
        Jx.Button.prototype.initialize.apply(this, [a, options]);
        Element.addClassName(this.domA, 'jxButtonFlyout');
        
        /* iframe shim to prevent scrollbars and 
           inputs from showing through the menu */
        this.iframe = document.createElement('iframe');
        this.iframe.className = 'jxMenuShim';
        this.iframe.scrolling = 'no';
        this.iframe.frameborder = 0;
        
        this.content = document.createElement('div');
        Element.addClassName(this.content, 'jxFlyout');
        this.loadContent(this.content, options);
        this.domObj.appendChild(this.content);
        this.content.style.display = 'none';
        
        this.keypressWatcher = this.keypressHandler.bindAsEventListener(this);
        this.hideWatcher = this.clickHandler.bindAsEventListener(this);
    },
    
    show: function(e) {
        if (!window.opera && !this.iframe.parentNode) {
            this.content.appendChild(this.iframe);
        }
        this.content.style.display = 'block';
        Event.observe(window, 'keypress', this.keypressWatcher);
        Event.observe(document, 'click', this.hideWatcher);
    },
    
    hide: function() {
        this.content.style.display = 'none';
        Event.stopObserving(window, 'keypress', this.keypressWatcher);    
        Event.stopObserving(document, 'click', this.hideWatcher);
    },
    /* hide flyout if the user clicks outside of the flyout */
    clickHandler: function(e) {
        var elm = Event.element(e);
        if (!Element.descendantOf(elm, this.domObj.parentNode)) {
            this.hide();
        }
    },
    /* hide flyout if the user presses the ESC key */
    keypressHandler: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.hide();
        }
    }
});

/**
 * Class: Jx.Button.Multi
 * 
 * Multi buttons are used to contain multiple buttons in a drop down list
 * where only one button is actually visible and clickable in the interface.
 * 
 * When the user clicks the active button, it performs its normal action.
 * The user may also click a drop-down arrow to the right of the button and
 * access the full list of buttons.  Clicking a button in the list causes
 * that button to replace the active button in the toolbar and performs
 * the button's regular action.
 *
 * Other buttons can be added to the Multi button using the <add> function.
 */
Jx.Button.Multi = Class.create();
Jx.Button.Multi.prototype = {
    /**
     * Property: {<Jx.Button>} activeButton
     *
     * the currently selected button
     */
    activeButton: null,
    /**
     * Property: {Array} buttons
     *
     * an array of all available buttons
     */
    buttons: null,
    /**
     * Constructor: initialize
     *
     * construct a new instance of Jx.Button.Multi.  The constructor
     * takes a single options parameter which is the same as the 
     * <Jx.Button.initialize> options object.
     *
     * Parameters: {Object} options
     *
     * an options object used to initialize the button
     */
    initialize: function(options) {
        this.buttons = [];
        this.content = document.createElement('div');
        this.tb = new Jx.Toolbar(this.content, 'right');
        this.flyout = new Jx.Button.Flyout({content: this.content, label: '&nbsp;'});
        Element.addClassName(this.flyout.domObj.firstChild, 'jxButtonMulti');
        
        this.domObj = document.createElement('div');
        //this.domObj.className = 'jxButtonMulti';
        this.buttonContainer = document.createElement('div');
        this.buttonContainer.className = 'jxButtonMultiContainer';
        this.domObj.appendChild(this.buttonContainer);
        this.domObj.appendChild(this.flyout.domObj);
    },
    /**
     * Function: add
     *
     * adds one or more buttons to the Multi button.  The first button
     * added becomes the active button initialize.  This function 
     * takes a variable number of arguments, each of which is expected
     * to be an instance of <Jx.Button>.
     *
     * Parameter: {<Jx.Button>} button
     * 
     * a <Jx.Button> instance, may be repeated in the parameter list
     */
    add: function() {
        for (var i=0; i<arguments.length; i++) {
            var theButton = arguments[i];
            var action = new Jx.Action(this.setButton.bind(this, theButton));
            var button = new Jx.Button(action, {});
            var click = button.domA.onclick;
            button.domObj = theButton.domObj.cloneNode(true);
            button.domObj.onclick = click;
            this.tb.add(button);
            if (!this.activeButton) {
                this.buttonContainer.appendChild(theButton.domObj);
                this.activeButton = theButton;
            }
        }
    },
    /**
     * Function: setActiveButton
     *
     * update the menu item to be the requested button.
     *
     * Parameter: {<Jx.Button>} button
     *
     * a <Jx.Button> instance that was added to this multi button.
     */
    setActiveButton: function(button) {
        while(this.buttonContainer.childNodes.length > 0) {
            this.buttonContainer.removeChild(this.buttonContainer.firstChild);
        }
        this.buttonContainer.appendChild(button.domObj);
    },
    /**
     * Function: setButton
     *
     * update the active button in the menu item, trigger the button's action
     * and hide the flyout that contains the buttons.
     *
     * Parameter: {<Jx.Button>} button
     *
     * The button to set as the active button
     */
    setButton: function(button) {
        this.setActiveButton(button);
        button.onclick();
        this.flyout.hide();
    }
};

/**
 * Class: Jx.Button.Picker
 * 
 * A Picker button allows the user to choose an item from a list of items.
 * The items can be any HTML element.  The chosen item is placed into the 
 * menu item.
 */
Jx.Button.Picker = Class.create();
Object.extend(Jx.Button.Picker.prototype, Jx.Button.Flyout.prototype);
Object.extend(Jx.Button.Picker.prototype, {
    /**
     * Property: {HTMLElement} ul
     *
     * the UL element that contains the items to pick from
     */
    ul: null,
    /**
     * Property {HTMLElement} selectedItem
     *
     * the currently selected item
     */
    selectedItem: null,
    /**
     * Constructor: initialize
     *
     * construct a new instance of <Jx.Button.Picker>
     */
    initialize: function() {
        Jx.Button.Flyout.prototype.initialize.apply(this, arguments);
        this.ul = document.createElement('ul');
        this.content.appendChild(this.ul);
        Element.removeClassName(this.domLabel, 'jxButtonEmptyLabel');
    },
    /**
     * Function: add
     *
     * adds one or more items to the picker, passed as separate arguments.
     *
     * Parameter: {Object} item
     *
     * can be present one or more times in the argument list.  The item
     * to be added can be a Jx object with a domObj property, a string,
     * or an HTML element reference.
     */
    add: function() {
        for (var i=0; i<arguments.length; i++) {
            var thing = arguments[i];
            if (thing.domObj) {
                thing = thing.domObj;
            }
            var li = document.createElement('li');
            var a = document.createElement('a');
            
            li.appendChild(a);
            if (typeof(thing) == 'string') {
                a.innerHTML = thing;
            } else {
                a.appendChild(thing);
            }
            this.ul.appendChild(li);
            
            if (!this.selectedItem) {
                this.updateButton(a);
            }
        }
    },
    /**
     * Function: clickHandler
     *
     * handle the user selecting an item in the list
     *
     * Parameter: {Event} e
     *
     * the event object associated with the click event
     */
    clickHandler: function(e) {
        var elm = Event.element(e);
        if (!Element.descendantOf(elm, this.domObj.parentNode)) {
            this.hide();
            return;
        }
        var a = Event.findElement(e, 'A');
        if (a && Element.descendantOf(a, this.ul)) {
            this.updateButton(a);
            this.hide();
        }
    },
    /**
     * Function: updateButton
     *
     * updates the button to contain the picked item
     *
     * Parameter: {HTMLElement} a
     *
     * the A tag that the user clicked on
     */
    updateButton: function(a) {
        while(this.domLabel.childNodes.length > 0) {
            this.domLabel.removeChild(this.domLabel.firstChild);
        }
        this.domLabel.appendChild(a.firstChild.cloneNode(true));
        this.selectedItem = a.firstChild;
    }
    
});