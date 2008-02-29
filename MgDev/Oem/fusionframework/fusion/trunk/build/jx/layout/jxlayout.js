/**
 * @project         Jx
 * @revision        $Id: jxlayout.js 413 2007-09-19 17:21:56Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

/**
 * Class: Jx.Layout
 *
 * Jx.Layout is used to provide more flexible layout options for applications
 *
 * Jx.Layout wraps an existing DOM element (typically a div) and provides
 * extra functionality for sizing that element within its parent and sizing
 * elements contained within it that have a 'resize' function attached to them.
 *
 * To create a Jx.Layout, pass the element or id plus an options object to
 * the constructor:
 *
 * (code)
 * var myContainer = new Jx.Layout('myDiv', options);
 * (end)
 *
 * Options can be passed to the Jx.Layout as an object with some, all, or none
 * of the following properties:
 *
 * position - how to position the element, either 'absolute' or 'relative'.
 *      The default (if not passed) is 'absolute'.  When using
 *      'absolute' positioning, both the width and height are
 *      controlled by Jx.Layout.  If 'relative' positioning is used
 *      then only the width is controlled, allowing the height to
 *      be controlled by its content.
 * left - the distance (in pixels) to maintain the left edge of the element
 *      from its parent element.  The default value is 0.  If this is set
 *      to 'null', then the left edge can be any distance from its parent
 *      based on other parameters.
 * right - the distance (in pixels) to maintain the right edge of the element
 *      from its parent element.  The default value is 0.  If this is set
 *      to 'null', then the right edge can be any distance from its parent
 *      based on other parameters.
 * top - the distance (in pixels) to maintain the top edge of the element
 *      from its parent element.  The default value is 0.  If this is set
 *      to 'null', then the top edge can be any distance from its parent
 *      based on other parameters.
 * bottom - the distance (in pixels) to maintain the bottom edge of the element
 *      from its parent element.  The default value is 0.  If this is set
 *      to 'null', then the bottom edge can be any distance from its parent
 *      based on other parameters.
 * width - the width (in pixels) of the element.  The default value is null.
 *      If this is set to 'null', then the width can be any value based on
 *      other parameters.
 * height - the height (in pixels) of the element.  The default value is null.
 *      If this is set to 'null', then the height can be any value based on
 *      other parameters.
 * minWidth - the minimum width that the element can be sized to.  The default
 *      value is 0.
 * minHeight - the minimum height that the element can be sized to.  The
 *      default value is 0.
 * maxWidth - the maximum width that the element can be sized to.  The default
 *      value is -1, which means no maximum.
 * maxHeight - the maximum height that the element can be sized to.  The
 *      default value is -1, which means no maximum.
 */
 
Jx.Layout = Class.create();
Jx.Layout.prototype = {
    scl: null,
    initialize: function(element, options) {
        options = options || {};
        this.options = new Jx.Constraint(options);
        this.domObj = $(element);
        this.domObj.resize = this.resize.bind(this);
        this.domObj.style.position = this.options.position;
        this.domObj.jxLayout = this;

        if (this.domObj.parentNode && this.domObj.parentNode.tagName == 'BODY') {
            Event.observe(window, 'resize', this.windowResize.bind(this));
        }
        
        this.scl = [];
    },
    
    windowResize: function() {
        if (this.resizeTimer) {
            window.clearTimeout(this.resizeTimer);
            this.resizeTimer = null;
        }
        this.resizeTimer = window.setTimeout(this.resize.bind(this), 250);
    },
    
    resize: function(options) {
        this.resizeTimer = null;
        if (options) {
            for (var i in options) {
                this.options[i] = options[i];
            }
        }
        //console.log('Jx.Layout.resize '+this.domObj.id);
        var parentSize;
        if (this.domObj.parentNode.tagName == 'BODY') {
            parentSize = Element.getPageDimensions();
        } else {
            parentSize = Element.getContentBoxSize(this.domObj.parentNode);
        }
        var l, t, w, h;
        
        /* calculate left and width */
        if (this.options.left != null) {
            /* fixed left */
            l = this.options.left;
            if (this.options.right == null) {
                /* variable right */
                if (this.options.width == null) {
                    //console.log( 'fixed left, variable right, variable width' );
                    /* variable right and width
                     * set right to min, stretch width */
                    w = parentSize.width - l;
                    if (w < this.options.minWidth ) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        w = this.options.maxWidth;
                    }
                } else {
                    //console.log( 'fixed left, variable right, fixed width' );
                    /* variable right, fixed width
                     * use width
                     */
                    w = this.options.width;
                }
            } else {
                /* fixed right */
                if (this.options.width == null) {
                    //console.log( 'fixed left, fixed right, variable width' );
                    
                    /* fixed right, variable width
                     * stretch width
                     */
                    w = parentSize.width - l - this.options.right;
                    if (w < this.options.minWidth) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        w = this.options.maxWidth;
                    }
                } else {
                    //console.log( 'fixed left, fixed right, fixed width' );
                    
                    /* fixed right, fixed width
                     * respect left and width, allow right to stretch
                     */
                    w = this.options.width;
                }
            }
            
        } else {
            if (this.options.right == null) {
                if (this.options.width == null) {
                    //console.log( 'variable left, variable right, variable width' );
                    
                    /* variable left, width and right
                     * set left, right to min, stretch width
                     */
                     l = 0;
                     w = parentSize.width;
                     if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                         l = l + parseInt(w - this.options.maxWidth)/2;
                         w = this.options.maxWidth;
                     }
                } else {
                    //console.log( 'variable left, variable right, fixed width' );
                    
                    /* variable left, fixed width, variable right
                     * distribute space between left and right
                     */
                    w = this.options.width;
                    l = parseInt((parentSize.width - w)/2);
                    if (l < 0) {
                        l = 0;
                    }
                }
            } else {
                if (this.options.width != null) {
                    //console.log( 'variable left, fixed right, fixed width' );
                    
                    /* variable left, fixed width, fixed right
                     * left is calculated directly
                     */
                    w = this.options.width;
                    l = parentSize.width - w - this.options.right;
                    if (l < 0) {
                        l = 0;
                    }
                } else {
                    //console.log( 'variable left, fixed right, variable width' );
                    
                    /* variable left and width, fixed right
                     * set left to min value and stretch width
                     */
                    l = 0;
                    w = parentSize.width - this.options.right;
                    if (w < this.options.minWidth) {
                        w = this.options.minWidth;
                    }
                    if (this.options.maxWidth >= 0 && w > this.options.maxWidth) {
                        l = w - this.options.maxWidth - this.options.right;
                        w = this.options.maxWidth;                        
                    }
                }
            }
        }
        
        /* calculate the top and height */
        if (this.options.top != null) {
            /* fixed top */
            t = this.options.top;
            if (this.options.bottom == null) {
                /* variable bottom */
                if (this.options.height == null) {
                    //console.log( 'fixed top, variable bottom, variable height' );
                    
                    /* variable bottom and height
                     * set bottom to min, stretch height */
                    h = parentSize.height - t;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        h = this.options.maxHeight;
                    }
                } else {
                    //console.log( 'fixed top, variable bottom, fixed height' );
                    
                    /* variable bottom, fixed height
                     * stretch height
                     */
                    h = this.options.height;
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        t = h - this.options.maxHeight;
                        h = this.options.maxHeight;
                    }
                }
            } else {
                /* fixed bottom */
                if (this.options.height == null) {
                    //console.log( 'fixed top, fixed bottom, variable height' );
                    
                    /* fixed bottom, variable height
                     * stretch height
                     */
                    h = parentSize.height - t - this.options.bottom;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        h = this.options.maxHeight;
                    }                
                } else {
                    //console.log( 'fixed top, fixed bottom, fixed height' );
                    
                    /* fixed bottom, fixed height
                     * respect top and height, allow bottom to stretch
                     */
                    h = this.options.height;
                }
            }
            
        } else {
            if (this.options.bottom == null) {
                if (this.options.height == null) {
                    //console.log( 'variable top, variable bottom, variable height' );
                    
                    /* variable top, height and bottom
                     * set top, bottom to min, stretch height
                     */
                     t = 0;
                     h = parentSize.height;
                     if (h < this.options.minHeight) {
                         h = this.options.minHeight;
                     }
                     if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                         t = parseInt((parentSize.height - this.options.maxHeight)/2);
                         h = this.options.maxHeight;
                     }
                } else {
                    //console.log( 'variable top, variable bottom, fixed height' );
                    
                    /* variable top, fixed height, variable bottom
                     * distribute space between top and bottom
                     */
                    h = this.options.height;
                    t = parseInt((parentSize.height - h)/2);
                    if (t < 0) {
                        t = 0;
                    }
                }
            } else {
                if (this.options.height != null) {
                    //console.log( 'variable top, fixed bottom, fixed height' );
                    
                    /* variable top, fixed height, fixed bottom
                     * top is calculated directly
                     */
                    h = this.options.height;
                    t = parentSize.height - h - this.options.bottom;
                    if (t < 0) {
                        t = 0;
                    }
                } else {
                    //console.log( 'variable top, fixed bottom, variable height' );
                    
                    /* variable top and height, fixed bottom
                     * set top to min value and stretch height
                     */
                    t = 0;
                    h = parentSize.height - this.options.bottom;
                    if (h < this.options.minHeight) {
                        h = this.options.minHeight;
                    }
                    if (this.options.maxHeight >= 0 && h > this.options.maxHeight) {
                        t = parentSize.height - this.options.maxHeight - this.options.bottom;
                        h = this.options.maxHeight;
                    }
                }
            }
        }
        
        this.domObj.style.position = this.options.position;
        if (this.options.position == 'absolute') {
            var padding = Element.getPaddingSize(this.domObj.parentNode);
        
            this.domObj.style.left = (l+padding.left) + 'px';
            this.domObj.style.top = (t+padding.top) + 'px';
            Element.setBorderBoxSize(this.domObj, {width: w, height: h} );
        } else {
            var sizeOpts = {width: w};
            if (this.options.height) {
                sizeOpts.height = this.options.height;
            }
            Element.setBorderBoxSize(this.domObj, sizeOpts);
        }
        
        
        for (var i=0; i<this.domObj.childNodes.length; i++) {
            var c = this.domObj.childNodes[i];
            if (c.resize) {
                c.resize();
            }
        }
        this.processEvent(this.scl,'sizeChanged',this);
    },
    addSizeChangeListener: function(o){this.addListener(this.scl, o);},
    removeSizeChangeListener: function(o) {
        this.removeListener(this.scl, o);
    }
};

Object.extend(Jx.Layout.prototype, Jx.Listener.prototype);


Jx.Constraint = Class.create();
Jx.Constraint.prototype = {
    position: 'absolute',
    left: 0,
    right: 0,
    top: 0,
    bottom: 0,
    width: null,
    height: null,
    minWidth: 0,
    minHeight: 0,
    maxWidth: -1,
    maxHeight: -1,
    initialize: function(o) {
        for( var i in o ) {
            this[i] = o[i];
        }
    }
};