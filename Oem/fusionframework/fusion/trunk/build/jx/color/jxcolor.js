/**********************************************************************
 *
 * $Id: jxcolor.js 409 2007-09-13 17:23:39Z pspencer $
 *
 * purpose: Implementation of a color selection panel.
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
 
Jx.addStyleSheet('color/color.css');
/**
 * Class: Jx.ColorPanel
 *
 * A Jx.ColorPanel presents a user interface for selecting colors.  Currently,
 * the user can either enter a HEX colour value or select from a palette of
 * web-safe colours.  The user can also enter an opacity value.
 *
 * A Jx.ColorPanel can be embedded anywhere in a web page by appending its
 * <Jx.ColorPanel.domObj> property to an HTML element.  However, a
 * a Jx.Button subclass  is provided ( <Jx.Button.Color> ) that embeds a
 * colour panel inside a button for easy use in toolbars.
 *
 * Colour changes are propogated via a colorChanged listener pattern.  To
 * be notified of changes in a Jx.ColorPanel, register a color change
 * listener that implements a colorChanged method.  For instance:
 *
 * (code)
 * var colorChangeListener = {
 * colorChanged: function(colorPanel) {
 *     alert('the color changed to: ' + colorPanel.color);
 *   }
 * }
 *
 * var colorPanel = new Jx.ColorPanel();
 * colorPanel.addColorChangeListener(colorChangeListener);
 *
 * document.getElementsByTagName('BODY')[0].appendChild(colorPanel.domObj);
 * (end)
 */
Jx.ColorPanel = Class.create();

Jx.ColorPanel.prototype = {
    /**
     * Property: {HTMLElement} domObj
     *
     * the HTML element representing the color panel
     */
    domObj: null,
    /**
     * Property {String} color
     * 
     * the currently selected colour, in hex format
     */
    color: null,
    /**
     * Property: {Float} alpha
     *
     * the current alpha value, between 0 (transparent) and 1 (opaque)
     */
    alpha: null,
    /**
     * Property: {Array} ccl
     * 
     * color change property listeners
     */
    ccl: null,
    /**
     * Property: {Array} hexColors
     *
     * an array of valid hex values that are used to build a web-safe
     * palette
     */
    hexColors: ['00', '33', '66', '99', 'CC', 'FF'],
    /**
     * Constructor: initialize
     *
     * initialize a new instance of Jx.ColorPicker
     *
     * Parameter: {Object} options
     *
     * an object containing a variable list of optional initialization
     * parameters.
     *
     * color - a colour to initialize the panel with, defaults to #000000
     *         (black) if not specified.
     *
     * alpha - an alpha value to initialize the panel with, defaults to 1
     *         (opaque) if not specified.
     */
    initialize: function(options) {
        
        options = options || {};
        this.keypressWatcher = this.keypressHandler.bind(this);
        
        this.ccl = [];
        
        this.color = options.color || '#000000';
        this.alpha = options.alpha || 1;
        
        this.domObj = document.createElement('div');
        this.domObj.className = 'jxColorPanel';
        
        var top = document.createElement('div');
        top.className = 'jxColorBar';
        
        var d = document.createElement('div');
        d.className = 'jxColorPreview';
        
        // var img = document.createElement('img');
        // img.src = Jx.baseURL + '/color/grid.png';        
        // d.appendChild(img);
        
        this.selectedSwatch = document.createElement('div');
        d.appendChild(this.selectedSwatch);
        
        top.appendChild(d);
        
        this.colorInputLabel = document.createElement('label');
        this.colorInputLabel.className = 'jxColorLabel';
        this.colorInputLabel.innerHTML = '#';
        top.appendChild(this.colorInputLabel);
        
        this.colorInput = document.createElement('input');
        this.colorInput.className = 'jxHexInput';
        this.colorInput.type = 'text';
        this.colorInput.maxLength = 6;
        Event.observe(this.colorInput, 'keyup', this.colorChanged.bind(this));
        Event.observe(this.colorInput, 'blur', this.colorChanged.bind(this));
        Event.observe(this.colorInput, 'change', this.colorChanged.bind(this));
        top.appendChild(this.colorInput);
        
        this.alphaLabel = document.createElement('label');
        this.alphaLabel.className = 'jxAlphaLabel';
        this.alphaLabel.innerHTML = 'alpha (%)';
        top.appendChild(this.alphaLabel);
        
        this.alphaInput = document.createElement('input');
        this.alphaInput.className = 'jxAlphaInput';
        this.alphaInput.type = 'text';
        this.alphaInput.maxLength = 3;
        Event.observe(this.alphaInput, 'keyup', this.alphaChanged.bind(this));
        top.appendChild(this.alphaInput);
        
        this.domObj.appendChild(top);
        
        var a = document.createElement('a');
        a.href="javascript:void(0)";
        a.className = 'jxColorClose';
        a.alt = 'Close';
        a.title = 'Close';
        Event.observe(a, 'click', this.hide.bind(this));
        
        img = document.createElement('img');
        img.className = 'png24';
        img.src = Jx.baseURL + 'images/close.png';
        a.appendChild(img);
        
        this.domObj.appendChild(a);
        
        d = document.createElement('div');
        d.className = 'jxClearer';
        this.domObj.appendChild(d);
        
        var swatchClick = this.swatchClick.bindAsEventListener(this);
        var swatchOver = this.swatchOver.bindAsEventListener(this);
        
        var table = document.createElement('table');
        table.className = 'jxColorGrid';
        var tbody = document.createElement('tbody');
        table.appendChild(tbody);
        for (var i=0; i<12; i++) {
            var tr = document.createElement('tr');
            for (var j=-3; j<18; j++) {
                var bSkip = false;
                var r, g, b;
                /* hacky approach to building first three columns
                 * because I couldn't find a good way to do it
                 * programmatically
                 */
                
                if (j < 0) {
                    if (j == -3 || j == -1) {
                        r = g = b = 0;
                        bSkip = true;
                    } else {
                        if (i<6) {
                            r = g = b = i;
                        } else {
                            if (i == 6) {
                                r = 5; g = 0; b = 0;
                            } else if (i == 7) {
                                r = 0; g = 5; b = 0;
                            } else if (i == 8) {
                                r = 0; g = 0; b = 5;
                            } else if (i == 9) {
                                r = 5; g = 5; b = 0;
                            } else if (i == 10) {
                                r = 0; g = 5; b = 5;
                            } else if (i == 11) {
                                r = 5; g = 0; b = 5;
                            }
                        }
                    }
                } else {
                    /* remainder of the columns are built
                     * based on the current row/column
                     */
                    r = parseInt(i/6)*3 + parseInt(j/6);
                    g = j%6;
                    b = i%6;
                }
                var bgColor = '#'+this.hexColors[r]+this.hexColors[g]+this.hexColors[b];
                
                var td = document.createElement('td');
                if (!bSkip) {
                    td.style.backgroundColor = bgColor;

                    var a = document.createElement('a');
                    if ((r > 2 && g > 2) || (r > 2 && b > 2) || (g > 2 && b > 2)) {
                        a.className = 'colorSwatch borderBlack';
                    } else {
                        a.className = 'colorSwatch borderWhite';
                    }
                    a.href = '#';
                    a.title = bgColor;
                    a.alt = bgColor;
                    a.swatchColor = bgColor;
                    
                    a.onmouseover = swatchOver;
                    a.onclick = swatchClick;

                    td.appendChild(a);
                } else {
                    var span = document.createElement('span');
                    td.className = 'emptyCell';
                    td.appendChild(span);
                }
                tr.appendChild(td);
            }
            tbody.appendChild(tr);
        }

        this.domObj.appendChild(table);
        
        var d = document.createElement('div');
        d.className = 'jxColorPreview';
        this.previewSwatch = document.createElement('div');
        d.appendChild(this.previewSwatch);
        this.previewSwatch.style.backgroundColor = this.color;
        this.domObj.appendChild(d);
        
        this.previewLabel = document.createElement('label');
        this.previewLabel.className = 'jxColorLabel';
        this.previewLabel.innerHTML = this.color;
        this.domObj.appendChild(this.previewLabel);
        
        d = document.createElement('div');
        d.className = 'jxClearer';
        this.domObj.appendChild(d);
        
        this.updateSelected();
    },
    
    /**
     * Function: swatchOver
     *
     * handle the mouse moving over a colour swatch by updating the preview
     *
     * Parameter: {Event} e
     *
     * the mousemove event object
     */
    swatchOver: function(e) {
        var a = Event.element(e);
        
        this.previewSwatch.style.backgroundColor = a.swatchColor;
        this.previewLabel.innerHTML = a.swatchColor;
    },
    
    /**
     * Function: swatchClick
     *
     * handle mouse click on a swatch by updating the color and hiding the
     * panel.
     *
     * Parameter: {Event} e
     *
     * the mouseclick event object
     */
    swatchClick: function(e) {
        var a = Event.element(e);
        
        this.color = a.swatchColor;
        this.updateSelected();
        this.hide();
    },
    
    /**
     * Function: colorChanged
     *
     * handle the user entering a new colour value manually by updating the
     * selected colour if the entered value is valid HEX.
     */
    colorChanged: function() {
        var color = this.colorInput.value;
        if (color.substring(0,1) == '#') {
            color = color.substring(1);
        }
        if (color.toLowerCase().match(/^[0-9a-f]{6}$/)) {
            this.color = '#' +color.toUpperCase();
            this.updateSelected();
        }
    },
    
    /**
     * Function: alphaChanged
     *
     * handle the user entering a new alpha value manually by updating the
     * selected alpha if the entered value is valid alpha (0-100).
     */
    alphaChanged: function() {
        var alpha = this.alphaInput.value;
        if (alpha.match(/^[0-9]{1,3}$/)) {
            this.alpha = parseFloat(alpha/100);
            this.updateSelected();
        }
    },
    
    /**
     * Function: setColor
     *
     * set the colour represented by this colour panel
     *
     * Parameter: {String} color
     *
     * the new hex color value
     */
    setColor: function( color ) {
        this.colorInput.value = color;
        this.colorChanged();
    },
    
    /**
     * Function: setAlpha
     *
     * set the alpha represented by this colour panel
     *
     * Parameter: {Integer} alpha
     *
     * the new alpha value (between 0 and 100)
     */
    setAlpha: function( alpha ) {
        this.alphaInput.value = alpha;
        this.alphaChanged();
    },
    
    /**
     * Function: updateSelected
     *
     * update the colour panel user interface based on the current
     * colour and alpha values
     */
    updateSelected: function() {
        this.selectedSwatch.style.backgroundColor = this.color;
        this.colorInput.value = this.color.substring(1);
        
        this.alphaInput.value = parseInt(this.alpha*100);
        if (this.alpha < 1) {
            this.selectedSwatch.style.opacity = this.alpha;
            this.selectedSwatch.style.filter = 'Alpha(opacity='+(this.alpha*100)+')';
        } else {
            this.selectedSwatch.style.opacity = '';
            this.selectedSwatch.style.filter = '';
        }
        this.processEvent(this.ccl,'colorChanged',this);
    },
    
    /**
     * Function: show
     *
     * show the panel
     */
    show: function() {
        this.domObj.style.display = 'block';
        Event.observe(window, 'keypress', this.keypressWatcher);
    },
    
    /**
     * Function: hide
     *
     * hide the panel
     */
    hide: function() {
        this.domObj.style.display = 'none';
        Event.stopObserving(window, 'keypress', this.keypressWatcher);
        
    },
    
    /**
     * Function: keypressHandler
     *
     * handle the user pressing a key.  If the key is the ESC key, then
     * hide the color panel
     *
     * Parameter: {Event} e
     *
     * The keypress event
     */
    keypressHandler: function(e) {
        var charCode=(e.charCode)?e.charCode:e.keyCode;
        if (charCode == Event.KEY_ESC) {
            this.hide();
        }
    },
    /**
     * Function: addColorChangeListener
     *
     * add a colour change listener, an object that has a colorChanged
     * function.
     *
     * Parameter: {Object} obj
     *
     * The colour change listener to call when the colour changes.
     */
    addColorChangeListener: function(obj){this.addListener(this.ccl, obj);},
    /**
     * Function: removeColorChangeListener
     *
     * remove a previously added colour change listener
     *
     * Parameter: {Object} obj
     *
     * The colour change listener to remove
     */
    removeColorChangeListener: function(obj) {
        this.removeListener(this.ccl, obj);
    }
};
Object.extend(Jx.ColorPanel.prototype, Jx.Listener.prototype);

/**
 * Class: Jx.Button.Color
 *
 * A <Jx.ColorPanel> wrapped up in a Jx.Button.  The button includes a
 * preview of the currently selected color.  Clicking the button opens
 * the color panel.
 *
 * Extends: <Jx.Button.Flyout>
 */
Jx.Button.Color = Class.create();
Object.extend(Jx.Button.Color.prototype, Jx.Button.Flyout.prototype);
Object.extend(Jx.Button.Color.prototype, {
    /**
     * Property: { Array } colorPanel
     *
     * By declaring this property as an array in the prototype, the
     * array is used for all instances making it a Class property.
     * A <Jx.ColorPanel> instance used by all color buttons is made the
     * first element so that only one color panel is ever created.
     */
    colorPanel: [],
    /**
     * Property: {HTMLElement} swatch
     *
     * a div used to represent the current colour in the button.
     */
    swatch: null,
    /**
     * Property: {String} color
     *
     * the current colour of this button in hex.  We have to maintain
     * it here because we share a colour panel with all other colour
     * buttons
     */
    color: null,
    /**
     * Property: {Integer} alpha
     *
     * the current alpha of this button.  We have to maintain
     * it here because we share a colour panel with all other colour
     * buttons
     */
    alpha: null,
    /**
     * Property: {Array} ccl
     * 
     * color change property listeners
     */
    ccl: null,
    /**
     * Constructor: initialize
     *
     * initialize a new colour button.
     *
     * Parameter: {Object} options
     *
     * an object containing a variable list of optional initialization
     * parameters.
     *
     * color - a colour to initialize the panel with, defaults to #000000
     *         (black) if not specified.
     *
     * alpha - an alpha value to initialize the panel with, defaults to 1
     *         (opaque) if not specified.
     */
     
    initialize: function(options) {
        options = options || {};
        options.label = options.label || '&nbsp;';
        this.color = options.color || '#000000';
        this.alpha = options.alpha || 100;
        this.ccl = [];
        
        if (this.colorPanel.length == 0) {
            this.colorPanel[0] = new Jx.ColorPanel();
        }
        var d = document.createElement('div');
        d.className = 'jxColorButtonPreview';
        
        this.selectedSwatch = document.createElement('div');
        d.appendChild(this.selectedSwatch);
        
        /* create a button with a label so it doesn't get an empty label class
           and then replace the label text with the swatch */
        Jx.Button.Flyout.prototype.initialize.apply(this, [options]);
        Element.addClassName(this.domObj.firstChild, 'jxButtonColor');
        this.domObj.firstChild.firstChild.insertBefore(d, this.domObj.firstChild.firstChild.firstChild);
        this.updateSwatch();
    },
    
    /**
     * Function: show
     * 
     * show the color panel when the user clicks the button
     */
    show: function() {
        if (this.colorPanel[0].currentButton) {
            this.colorPanel[0].currentButton.hide();
        }
        this.colorPanel[0].currentButton = this;
        this.colorPanel[0].addColorChangeListener(this);
        this.content.appendChild(this.colorPanel[0].domObj);
        this.colorPanel[0].domObj.style.display = 'block';
        Jx.Button.Flyout.prototype.show.apply(this, arguments);
        /* setting these before causes an update problem when clicking on
         * a second color button when another one is open - the color
         * wasn't updating properly
         */
        this.colorPanel[0].setColor(this.color);
        this.colorPanel[0].setAlpha(this.alpha);
    },
    
    /**
     * Function: hide
     *
     * hide the colour panel
     */    
    hide: function() {
        this.colorPanel[0].removeColorChangeListener(this);
        Jx.Button.Flyout.prototype.hide.apply(this, arguments);
        this.colorPanel[0].currentButton = null;
    },
    
    /**
     * Function: setColor
     *
     * set the colour represented by this colour panel
     *
     * Parameter: {String} color
     *
     * the new hex color value
     */
    setColor: function(color) {
        this.color = color;
        this.updateSwatch();
    },
    
    /**
     * Function: setAlpha
     *
     * set the alpha represented by this colour panel
     *
     * Parameter: {Integer} alpha
     *
     * the new alpha value (between 0 and 100)
     */
    setAlpha: function(alpha) {
        this.alpha = alpha;
        this.updateSwatch();
    },
    
    /**
     * Function: colorChanged
     *
     * colorChangeListener callback function when the user changes
     * the colour in the panel (just update the preview).
     */
    colorChanged: function(panel) {
        this.color = panel.color;
        this.alpha = panel.alpha * 100;
        this.updateSwatch();
        this.processEvent(this.ccl,'colorChanged',this);
    },
    
    updateSwatch: function() {
        this.selectedSwatch.style.backgroundColor = this.color;
        if (this.alpha < 100) {
            this.selectedSwatch.style.opacity = this.alpha/100;
            this.selectedSwatch.style.filter = 'Alpha(opacity='+(this.alpha)+')';    
        } else {
            this.selectedSwatch.style.opacity = '';
            this.selectedSwatch.style.filter = '';    
        }
    },
    /**
     * Function: addColorChangeListener
     *
     * add a colour change listener, an object that has a colorChanged
     * function.
     *
     * Parameter: {Object} obj
     *
     * The colour change listener to call when the colour changes.
     */
    addColorChangeListener: function(obj){this.addListener(this.ccl, obj);},
    /**
     * Function: removeColorChangeListener
     *
     * remove a previously added colour change listener
     *
     * Parameter: {Object} obj
     *
     * The colour change listener to remove
     */
    removeColorChangeListener: function(obj) {
        this.removeListener(this.ccl, obj);
    }
});
