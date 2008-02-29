/**********************************************************************
 *
 * $Id: jxtoolbar.js 370 2007-07-26 17:24:46Z fwarnock $
 *
 * purpose: Implement a toolbar control.
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
 * import stylesheets
 */
Jx.addStyleSheet('toolbar/toolbar.css');
Jx.addStyleSheet('button/button.css');

/**
 * Class: Jx.Toolbar
 *
 * A toolbar is a container object that contains other objects such as
 * buttons.  The toolbar organizes the objects it contains automatically,
 * wrapping them as necessary.  Multiple toolbars may be placed within
 * the same containing object.
 *
 * Jx.Toolbar includes CSS classes for styling the appearance of a
 * toolbar to be similar to traditional desktop application toolbars.
 *
 * There is one special object, Jx.ToolbarSeparator, that provides
 * a visual separation between objects in a toolbar.
 *
 * The following example shows how to create a Jx.Toolbar instance and place two objects in it.
 * (code)
 * //myToolbarContainer is the id of a <div> in the HTML page.
 * function myFunction() {}
 * var myToolbar = new Jx.Toolbar('myToolbarContainer');
 * 
 * var myAction = new Jx.Action(myFunction):
 * var myButton = new Jx.Button(myAction);
 *
 * var myElement = document.createElement('select');
 *
 * myToolbar.add(myButton, new Jx.ToolbarSeparator(), myElement);
 * (end)
 */
Jx.Toolbar = Class.create();
Jx.Toolbar.prototype = {
    items : null,
    domObj : null,
    bActive : false, /* from menubar */
    initialize : function(domObj, position) {
        var parent = $(domObj);
        this.domObj = document.createElement('ul');
        Element.addClassName(this.domObj,'jxToolbar');
        
        if (!Element.hasClassName(parent, 'jxToolbarContainer')) {
            Element.addClassName(parent, 'jxToolbarContainer');
            parent.appendChild(this.domObj);
            var clearer = document.createElement('div');
            clearer.className = 'jxClearer';
            parent.appendChild(clearer);                    
        } else {
            parent.insertBefore(this.domObj, parent.lastChild);
        }
        switch (position) {
            case 'top':
                Element.addClassName(parent, 'jxBarTop');
                break;
            case 'right':
                Element.addClassName(parent, 'jxBarRight');
                break;
            case 'bottom':
                Element.addClassName(parent, 'jxBarBottom');
                break;
            case 'left':
                Element.addClassName(parent, 'jxBarLeft');
                break;
            default:
                Element.addClassName(parent, 'jxBarTop');
        }
        this.deactivateWatcher = this.deactivate.bindAsEventListener(this);
        
    },
    add : function( ) {
        for (var i=0; i<arguments.length; i++) {
            var thing = arguments[i];
            thing.toolbar = this;
            if (thing.domObj) {
                thing = thing.domObj;
            }
            if (thing.tagName == 'LI') {
                if (!Element.hasClassName(thing, 'jxToolItem')) {
                    Element.addClassName(thing, 'jxToolItem');
                }
                this.domObj.appendChild(thing);
            } else {
                var item = new Jx.ToolbarItem(thing);
                this.domObj.appendChild(item.domObj);
            }
        }
    },
    /* menu functions */
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

/**
 *
 */
Jx.ToolbarItem = Class.create();
Jx.ToolbarItem.prototype = {
    domObj: null,
    initialize : function( jxThing ) {
        this.al = [];
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxToolItem';
        if (jxThing) {
            if (jxThing.domObj) {
                this.domObj.appendChild(jxThing.domObj);
                if (jxThing instanceof Jx.Tab) {
                    // this.domObj.className = 'jxTabItem';
                    Element.addClassName(this.domObj, 'jxTabItem');
                }
            } else {
                this.domObj.appendChild(jxThing);
                if (Element.hasClassName(jxThing, 'jxTab')) {
                    // this.domObj.className = 'jxTabItem';
                    Element.addClassName(this.domObj, 'jxTabItem');
                }
            }
        }
    }
};

Jx.ToolbarSeparator = Class.create();
Jx.ToolbarSeparator.prototype = {
    domObj: null,
    initialize: function() {
        this.domObj = document.createElement('li');
        this.domObj.className = 'jxToolItem';
        this.domSpan = document.createElement('span');
        this.domSpan.className = 'separator';
        this.domObj.appendChild(this.domSpan);
    }
};

