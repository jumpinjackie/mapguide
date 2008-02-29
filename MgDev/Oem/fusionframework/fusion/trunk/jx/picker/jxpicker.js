/**
 * @project         Jx
 * @revision        $Id: jxpicker.js 328 2007-06-20 20:42:17Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

 Jx.addStyleSheet('picker/picker.css');


Jx.Picker = Class.create();
Jx.Picker.prototype = {
    /** selection listeners */
    sl : null,
    
    /** the div that contains the control, used to show/hide the control */
    domObj : null,
    /** the ul that contains the selectable items */
    ul : null,
    /** current selection in the list */
    currentSelection : null,
    
    /** editable? **/
    isEditable: false,
    /** constructor */
    initialize: function(options) {
        options = options || {};
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxPicker';
        this.isEditable = options.editable || false;
        
        if (this.isEditable) {
            this.domInput = document.createElement('input');
            this.domInput.type = 'text';
            Event.observe(this.domInput, 'change', this.valueChanged.bind(this));
            Event.observe(this.domInput, 'keydown', this.onKeyPress.bind(this));
        } else {
            this.domInput = document.createElement('span');
        }
        this.domInput.className = 'jxPickerInput';
        
        this.domObj.appendChild(this.domInput);
        
        this.domA = document.createElement('a');
        this.domA.href = 'javascript:void(0)';
        this.domA.className = 'jxPickerDiscloser';
        Event.observe(this.domA, 'click', this.toggle.bind(this));
        
        this.domButton = document.createElement('img');
        this.domButton.src = Jx.baseURL + 'images/disclose2.png';
        Element.addClassName(this.domButton, 'png24');
        this.domA.appendChild(this.domButton);
        this.domObj.appendChild(this.domA);
        
        if (!window.opera) {
            //insert iframeShim for IE to cover other controls in the page.
            var iframe = document.createElement('iframe');
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.domObj.appendChild(iframe);
        }
        
        this.domListDiv = document.createElement('div');
        this.domListDiv.className = 'jxPickerOptions';
        this.domListDiv.style.display = 'none';
        this.domObj.appendChild(this.domListDiv);
        this.domList = document.createElement('ul');
        this.domListDiv.appendChild(this.domList);
        this.sl = [];
    },
    
    onKeyPress: function(e) {
        var charCode = (e.charCode) ? e.charCode : ((e.keyCode) ? e.keyCode : e.which);
        if (charCode == Event.KEY_RETURN) {
            this.valueChanged();
        }
    },
    
    valueChanged: function() {
        this.processEvent(this.sl, 'selectionChanged', this);
    },
    
    /**
     * add a new item to the pick list
     *
     * @param selected {boolean} whether the item is initially selected or not
     */
    add: function(domObj, idx) {
        var li = document.createElement('li');
        var a = document.createElement('a');
        a.href="javascript:void(0)";

        if (typeof(domObj) == 'string') {
            a.innerHTML = domObj;
        } else {
            a.appendChild(domObj);
        }
        Event.observe(a, 'click', this.pick.bindAsEventListener(this));
        li.appendChild(a);

        if (arguments.length > 1 && this.domList.childNodes.length > idx) {
            this.domList.insertBefore(li, this.domList.childNodes[idx]);
        } else {
            this.domList.appendChild(li);
        }
    },
    
    remove: function(idx) {
        if (idx > 0 && idx < this.domList.childNodes.length) {
            this.domList.removeChild(this.domList.childNodes[idx]);
        }
    },
    
    /**
     * user has clicked something in the list, select it
     *
     * @param e {Event} the mouse event from the user's click
     */
    pick: function(e) {
        var target = Event.element(e);
        if (target.tagName == 'A') {
            this.currentSelection = target;
            this.setValue(this.currentSelection.childNodes[0]);
            this.valueChanged();
        }
        this.close();
    },
    setValue: function(value) {
        if (this.isEditable) {
            if (typeof(value) == 'string') {
                this.domInput.value = value;
            } else if (value.nodeType && value.nodeType == 3) {
                this.domInput.value = value.nodeValue;
            } else {
                this.domInput.value = value.innerHTML;
            }
        } else if (!this.isEditable) {
            if (typeof(value) == 'string') {
                this.domInput.innerHTML = value;
            } else if (value.nodeType && value.nodeType == 3) {
                this.domInput.innerHTML = value.nodeValue;
            } else {
                this.domInput.appendChild(value);
            }
        }
    },
    
    getValue: function() {
        value = '';
        if (this.isEditable) {
            value = this.domInput.value;
        } else {
            if (this.domInput.childNodes[0].nodeType == 3) {
                value = this.domInput.innerHTML;
            } else {
                value = this.domInput.childNodes[0];
            }
        }
        return value;
    },
    
    toggle: function() {
        if (this.domListDiv.style.display == 'block') {
            this.close();
        } else {
            this.open();
        }
    },
    
    open: function(selection) {
        if (!this.keypressListener) {
            this.keypressListener = this.keypress.bindAsEventListener(this);
        }
        this.domListDiv.style.display = 'block';
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
        this.domListDiv.style.display = 'none';
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
Object.extend(Jx.Picker.prototype, Jx.Listener.prototype);