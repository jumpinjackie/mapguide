/**
 * @project         Jx
 * @revision        $Id: jxsplitter.js 431 2007-10-11 18:06:43Z fwarnock $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */
 
/* a Jx.Splitter creates two or more containers within a parent container
 * and provides user control over the size of the containers.  The split
 * can be made horizontally or vertically.
 *
 * A horizontal split creates containers that divide the space horizontally
 * with vertical bars between the containers.  A vertical split divides
 * the space vertically and creates horizontal bars between the containers.
 *
 * When constructing a splitter object, you can specify the following
 * options:
 *
 * splitInto - integer, the number of containers to create.  The default value
 * is 2 if splitInto is not provided or is less than 2.
 *
 * layout - string, one of 'horizontal' or 'vertical'.  Default is 'horizontal'.
 *
 * containerOptions - array, an array of Jx.LayoutObj objects that are used
 * to initialize the containers.  The array itself is optional, as are indexes
 * within the array.  For any container that does not have an associated
 * option in this array, there is a default Jx.LayoutObj that provides the
 * default behaviour - left, top, right and bottom are 0, width and height are
 * null.  This array can be used to create containers that have minimum and/or
 * maximum sizes.
 */
 
Jx.Splitter = Class.create();
/**
 * abstract base class for creating a splitter within an element
 * Use JxHorizontalSplitter or JxVerticalSplitter
 */
Jx.Splitter.prototype = {
    domObj: null,
    elements: null,
    bars: null,
    firstUpdate: true,
    initialize: function(domObj, options) {
        options = options || {};  
        
        this.domObj = $(domObj);
        this.domObj.style.overflow = 'hidden';
        if (this.domObj.jxLayout) {
            this.domObj.jxLayout.addSizeChangeListener(this);
        }
       
        this.elements = [];
        this.bars = [];
        
        var nSplits = options.splitInto || (options.elements ? options.elements.length : 2);
        
        var aContainerOpts = options.containerOptions || [];

        for (var i=0; i<nSplits; i++) {
            this.elements[i] = options.elements ? options.elements[i] : this.prepareElement();
            this.domObj.appendChild(this.elements[i]);
            if (!this.elements[i].jxLayout) {
                new Jx.Layout(this.elements[i], aContainerOpts[i]);
            }
        }
        
        for (var i=1; i<nSplits; i++) {
            this.bars[i-1] = this.prepareBar();
            this.bars[i-1].leftSide = this.elements[i-1];
            this.bars[i-1].rightSide = this.elements[i];
            this.elements[i-1].rightBar = this.bars[i-1];
            this.elements[i].leftBar = this.bars[i-1];
            this.domObj.appendChild(this.bars[i-1]);
        }

        this.layout = options.layout || 'horizontal';
        this.establishConstraints();
        
        if (options.snappers) {
            for (var i=0; i<options.snappers.length; i++) {
                if (options.snappers[i]) {
                    new Jx.Splitter.Snapper(options.snappers[i], this.elements[i], this);
                }
            }
        }
    },
    prepareElement: function(options){
        var o = document.createElement('div');
        o.style.position = 'absolute';
        o.leftBar = null;
        o.rightBar = null;
        return o;
    },
    
    prepareBar: function() {
        var o = document.createElement('div');
        o.className = 'jxSplitterBar';
        o.style.position = 'absolute';
        o.title = 'drag this bar to resize';
        o.style.lineHeight = '1px'; // for IE, horizontal bars
        o.splitterObj = this;
        return o;
    },
    
    establishConstraints: function() {
        if (this.layout == 'horizontal') {
            for (var i=0; i< this.bars.length; i++) {
                this.bars[i].style.top = '0px';
                this.bars[i].style.height = '100%';
                new Draggable(this.bars[i], { constraint: 'horizontal' , 
                                     starteffect : function(element) { 
                                            element.style.backgroundColor = '#eee';
                                            },
                                     endeffect : function(element) {
                                         element.style.backgroundColor = '';
                                     }
                });
            }
        } else {
            for (var i=0; i< this.bars.length; i++) {
                this.bars[i].style.left = '0px';
                this.bars[i].style.width = '100%';
                new Draggable(this.bars[i], { constraint: 'vertical' , 
                                     starteffect : function(element) { 
                                            element.style.backgroundColor = '#eee';
                                            },
                                     endeffect : function(element) {
                                         element.style.backgroundColor = '';
                                     }
                });
            }
        }
        Draggables.addObserver(this);
    },
    
    /* a bar has been moved */
    onEnd: function(eventName, obj, event) {
        if (obj.element.splitterObj != this) {
            return;
        }
        if (this.layout == 'horizontal') {
            this.dragHorizontal(eventName, obj, event);
        } else {
            this.dragVertical(eventName, obj, event);
        }
    },
    
    dragHorizontal: function(eventName, obj, event) {
        var leftEdge = parseInt(obj.element.style.left);
        var leftSide = obj.element.leftSide;
        var rightSide = obj.element.rightSide;
        
        /* process right side first */
        var rsLeft, rsWidth, rsRight;
        
        if (!obj.element.size) {
            obj.element.size = Element.getBorderBoxSize(obj.element);
        }
        var barSize = obj.element.size;
        rsLeft = leftEdge + barSize.width;
        
        var parentSize = Element.getContentBoxSize(this.domObj);
        
        if (rightSide.jxLayout.options.width != null) {
            rsWidth = rightSide.jxLayout.options.width + rightSide.jxLayout.options.left - rsLeft;
            rsRight = parentSize.width - rsLeft - rsWidth;
        } else {
            rsWidth = parentSize.width - rightSide.jxLayout.options.right - rsLeft;
            rsRight = rightSide.jxLayout.options.right;
        }
        
        /* enforce constraints on right side */
        if (rsWidth < 0) {
            rsWidth = 0;
        }
        
        if (rsWidth < rightSide.jxLayout.options.minWidth) {
            rsWidth = rightSide.jxLayout.options.minWidth;
        }
        if (rightSide.jxLayout.options.maxWidth >= 0 && rsWidth > rightSide.jxLayout.options.maxWidth) {
            rsWidth = rightSide.jxLayout.options.maxWidth;
        }
        
        rsLeft = parentSize.width - rsRight - rsWidth;
        leftEdge = rsLeft - barSize.width;
        
        /* process left side */
        var lsLeft, lsWidth;
        lsLeft = leftSide.jxLayout.options.left;
        lsWidth = leftEdge - lsLeft;
        
        /* enforce constraints on left */
        if (lsWidth < 0) {
            lsWidth = 0;
        }
        if (lsWidth < leftSide.jxLayout.options.minWidth) {
            lsWidth = leftSide.jxLayout.options.minWidth;
        }
        if (leftSide.jxLayout.options.maxWidth >= 0 && 
            lsWidth > leftSide.jxLayout.options.maxWidth) {
            lsWidth = leftSide.jxLayout.options.maxWidth;
        }
        
        /* update the leftEdge to accomodate constraints */
        if (lsLeft + lsWidth != leftEdge) {
            /* need to update right side, ignoring constraints because left side
               constraints take precedence (arbitrary decision)
             */
            leftEdge = lsLeft + lsWidth;
            var delta = leftEdge + barSize.width - rsLeft;
            rsLeft += delta;
            rsWidth -= delta; 
        }
        
        /* put bar in its final location based on constraints */
        obj.element.style.left = leftEdge + 'px';
        
        /* update leftSide positions */
        if (leftSide.jxLayout.options.width == null) {
            var parentSize = Element.getContentBoxSize(this.domObj);
            leftSide.jxLayout.resize({right: parentSize.width - lsLeft-lsWidth});
        } else {
            leftSide.jxLayout.resize({width: lsWidth});
        }
        
        /* update rightSide position */
        if (rightSide.jxLayout.options.width == null) {
            rightSide.jxLayout.resize({left:rsLeft});
        } else {
            rightSide.jxLayout.resize({left: rsLeft, width: rsWidth});
        }
    },
    
    /* a bar has been moved */
    dragVertical: function(eventName, obj, event) {
        /* top edge of the bar */
        var topEdge = parseInt(obj.element.style.top);
        
        /* the containers on either side of the bar */
        var topSide = obj.element.leftSide;
        var bottomSide = obj.element.rightSide;
        
        /* measure the bar and parent container for later use */
        if (!obj.element.size) {
            obj.element.size = Element.getBorderBoxSize(obj.element);
        }
        var barSize = obj.element.size;
        var parentSize = Element.getContentBoxSize(this.domObj);

        /* process top side first */
        var bsTop, bsHeight, bsBottom;
        
        /* top edge of bottom side is the top edge of bar plus the height of the bar */
        bsTop = topEdge + barSize.height;
        
        if (bottomSide.jxLayout.options.height != null) {
            /* bottom side height is fixed */
            bsHeight = bottomSide.jxLayout.options.height + bottomSide.jxLayout.options.top - bsTop;
            bsBottom = parentSize.height - bsTop - bsHeight;
        } else {
            /* bottom side height is not fixed. */
            bsHeight = parentSize.height - bottomSide.jxLayout.options.bottom - bsTop;
            bsBottom = bottomSide.jxLayout.options.bottom;
        }
        
        /* enforce constraints on bottom side */
        if (bsHeight < 0) {
            bsHeight = 0;
        }
        
        if (bsHeight < bottomSide.jxLayout.options.minHeight) {
            bsHeight = bottomSide.jxLayout.options.minHeight;
        }
        
        if (bottomSide.jxLayout.options.maxHeight >= 0 && bsHeight > bottomSide.jxLayout.options.maxHeight) {
            bsHeight = bottomSide.jxLayout.options.maxHeight;
        }
        
        /* recalculate the top of the bottom side in case it changed
           due to a constraint.  The bar may have moved also.
         */
        bsTop = parentSize.height - bsBottom - bsHeight;
        topEdge = bsTop - barSize.height;
                
        /* process left side */
        var tsTop, tsHeight;
        tsTop = topSide.jxLayout.options.top;
        tsHeight = topEdge - tsTop;
                        
        /* enforce constraints on left */
        if (tsHeight < 0) {
            tsHeight = 0;
        }
        if (tsHeight < topSide.jxLayout.options.minHeight) {
            tsHeight = topSide.jxLayout.options.minHeight;
        }
        if (topSide.jxLayout.options.maxHeight >= 0 && 
            tsHeight > topSide.jxLayout.options.maxHeight) {
            tsHeight = topSide.jxLayout.options.maxHeight;
        }
        
        /* update the topEdge to accomodate constraints */
        if (tsTop + tsHeight != topEdge) {
            /* need to update right side, ignoring constraints because left side
               constraints take precedence (arbitrary decision)
             */
            topEdge = tsTop + tsHeight;
            var delta = topEdge + barSize.height - bsTop;
            bsTop += delta;
            bsHeight -= delta; 
        }
        
        /* put bar in its final location based on constraints */
        obj.element.style.top = topEdge + 'px';
        
        /* update topSide positions */
        if (topSide.jxLayout.options.height == null) {
            topSide.jxLayout.resize({bottom: parentSize.height - tsTop-tsHeight});
        } else {
            topSide.jxLayout.resize({height: tsHeight});
        }
        
        /* update bottomSide position */
        if (bottomSide.jxLayout.options.height == null) {
            bottomSide.jxLayout.resize({top:bsTop});
        } else {
            bottomSide.jxLayout.resize({top: bsTop, height: bsHeight});
        }
    },
    
    sizeChanged: function() {
        if (this.layout == 'horizontal') {
            this.horizontalResize();
        } else {
            this.verticalResize();
        }
    },
    
    horizontalResize: function() {
        var availableSpace = Element.getContentBoxSize(this.domObj).width;
        var overallWidth = availableSpace;

        for (var i=0; i<this.bars.length; i++) {
            var bar = this.bars[i];
            if (!bar.size) {
                bar.size = Element.getBorderBoxSize(bar);
            }
            availableSpace -= bar.size.width;
        }

        var nVariable = 0;
        var jxo;
        for (var i=0; i<this.elements.length; i++) {
            var e = this.elements[i];
            jxo = e.jxLayout.options;
            if (jxo.width != null) {
                availableSpace -= parseInt(jxo.width);
            } else {
                var w = 0;
                if (jxo.right != 0 || 
                    jxo.left != 0) {
                    w = Element.getBorderBoxSize(e).width;
                }
                
                availableSpace -= w;
                nVariable++;
            }
        }

        if (nVariable == 0) { /* all fixed */
            /* stick all available space in the last one */
            availableSpace += jxo.width;
            jxo.width = null;
            nVariable = 1;
        }

        var amount = parseInt(availableSpace / nVariable);
        /* account for rounding errors */
        var remainder = availableSpace % nVariable;

        var currentPosition = 0;

        for (var i=0; i<this.elements.length; i++) {
             var e = this.elements[i];
             var jxl = e.jxLayout;
             var jxo = jxl.options;
             if (jxo.width != null) {
                 jxl.resize({left: currentPosition});
                 currentPosition += jxo.width;
             } else {
                 var a = amount;
                 if (nVariable == 1) {
                     a += remainder;
                 }
                 nVariable--;
                 
                 var w = 0;
                 if (jxo.right != 0 || jxo.left != 0) {
                     w = Element.getBorderBoxSize(e).width + a;
                 } else {
                     w = a;
                 }
                 
                 if (w < 0) {
                     if (nVariable > 0) {
                         amount = amount + w/nVariable;
                     }
                     w = 0;
                 }
                 if (w < jxo.minWidth) {
                     if (nVariable > 0) {
                         amount = amount + (w - jxo.minWidth)/nVariable;
                     }
                     w = jxo.minWidth;
                 }
                 if (jxo.maxWidth >= 0 && w > jxo.maxWidth) {
                     if (nVariable > 0) {
                         amount = amount + (w - jxo.maxWidth)/nVariable;
                     }
                     w = e.options.maxWidth;
                 }
                 
                 var r = overallWidth - currentPosition - w;
                 jxl.resize({left: currentPosition, right: r});
                 currentPosition += w;
             }
             if (e.rightBar) {
                 e.rightBar.style.left = currentPosition + 'px';
                 currentPosition += e.rightBar.size.width;
             }
         }
    },
    
    verticalResize: function() { 
        var availableSpace = Element.getContentBoxSize(this.domObj).height;
        var overallHeight = availableSpace;

        for (var i=0; i<this.bars.length; i++) {
            var bar = this.bars[i];
            if (!bar.size) {
                bar.size = Element.getBorderBoxSize(bar);
            }
            availableSpace -= bar.size.height;
        }

        var nVariable = 0;
        
        var jxo;
        for (var i=0; i<this.elements.length; i++) {
            var e = this.elements[i];
            jxo = e.jxLayout.options;
            if (jxo.height != null) {
                availableSpace -= parseInt(jxo.height);
            } else {
                var h = 0;
                if (jxo.bottom != 0 || jxo.top != 0) {
                    h = Element.getBorderBoxSize(e).height;
                }
                
                availableSpace -= h;
                nVariable++;
            }
        }

        if (nVariable == 0) { /* all fixed */
            /* stick all available space in the last one */
            availableSpace += jxo.height;
            jxo.height = null;
            nVariable = 1;
        }

        var amount = parseInt(availableSpace / nVariable);
        /* account for rounding errors */
        var remainder = availableSpace % nVariable;

        var currentPosition = 0;

        for (var i=0; i<this.elements.length; i++) {
             var e = this.elements[i];
             var jxl = e.jxLayout;
             var jxo = jxl.options;
             if (jxo.height != null) {
                 jxl.resize({top: currentPosition});
                 currentPosition += jxo.height;
             } else {
                 var a = amount;
                 if (nVariable == 1) {
                     a += remainder;
                 }
                 nVariable--;
                 
                 var h = 0;
                 if (jxo.bottom != 0 || 
                     jxo.top != 0) {
                     h = Element.getBorderBoxSize(e).height + a;
                 } else {
                     h = a;
                 }
                 
                 if (h < 0) {
                     if (nVariable > 0) {
                         amount = amount + h/nVariable;
                     }
                     h = 0;
                 }
                 if (h < jxo.minHeight) {
                     if (nVariable > 0) {
                         amount = amount + (h - jxo.minHeight)/nVariable;
                     }
                     h = jxo.minHeight;
                 }
                 if (jxo.maxHeight >= 0 && h > jxo.maxHeight) {
                     if (nVariable > 0) {
                         amount = amount + (h - jxo.maxHeight)/nVariable;
                     }
                     h = jxo.maxHeight;
                 }
                 
                 var r = overallHeight - currentPosition - h;
                 jxl.resize({top: currentPosition, bottom: r});
                 currentPosition += h;
             }
             if (e.rightBar) {
                 e.rightBar.style.top = currentPosition + 'px';
                 currentPosition += e.rightBar.size.height;
             }
         }
    }
};

Jx.Splitter.Snapper = Class.create();
Jx.Splitter.Snapper.prototype = {
    snapper: null,
    element: null,
    splitter: null,
    layout: 'vertical',
    initialize: function(snapper, element, splitter) {
        this.snapper = snapper;
        this.element = element;
        element.jxLayout.addSizeChangeListener(this);
        this.splitter = splitter;
        this.layout = splitter.layout; 
        var jxo = this.element.jxLayout.options;
        var size = Element.getBorderBoxSize(this.element);
        if (this.layout == 'vertical') {
            this.originalSize = size.height;
            this.minimumSize = jxo.minHeight ? jxo.minHeight : 0;
        } else {
            this.originalSize = size.width;
            this.minimumSize = jxo.minWidth ? jxo.minWidth : 0;
        }
        
        Event.observe(snapper, 'click', this.toggleElement.bind(this));
    },
    
    toggleElement: function() {
        var size = Element.getBorderBoxSize(this.element);
        var newSize = {};
        if (this.layout == 'vertical') {
            if (size.height == this.minimumSize) {
                newSize.height = this.originalSize;
            } else {
                this.originalSize = size.height;
                newSize.height = this.minimumSize;
            }
        } else {
            if (size.width == this.minimumSize) {
                newSize.width = this.originalSize;
            } else {
                this.originalSize = size.width;
                newSize.width = this.minimumSize;
            }
        }
        this.element.jxLayout.resize(newSize);
        this.splitter.sizeChanged();
    },
    
    sizeChanged: function() {
        var size = Element.getBorderBoxSize(this.element);
        if (this.layout == 'vertical') {
            if (size.height == this.minimumSize) {
                Element.addClassName(this.snapper, 'jxSnapClosed');
                Element.removeClassName(this.snapper, 'jxSnapOpened');
            } else {
                Element.addClassName(this.snapper, 'jxSnapOpened');
                Element.removeClassName(this.snapper, 'jxSnapClosed');
            }
        } else {
            if (size.width == this.minimumSize) {
                Element.addClassName(this.snapper, 'jxSnapClosed');
                Element.removeClassName(this.snapper, 'jxSnapOpened');
            } else {
                Element.addClassName(this.snapper, 'jxSnapOpened');
                Element.removeClassName(this.snapper, 'jxSnapClosed');
            }
        }
    }
};