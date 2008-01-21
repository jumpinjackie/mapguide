/**
 * @project         Jx
 * @revision        $Id: jxdialog.js 458 2008-01-09 21:24:35Z madair $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

Jx.addStyleSheet('dialog/dialog.css');

/**
 * Class: Jx.Dialog
 *
 * Extends: <Jx.ContentLoader>
 *
 * Extends: <Jx.UniqueId>
 *
 * A Jx.Dialog implements a floating dialog.  Dialogs represent a useful way
 * to present users with certain information or application controls.
 * Jx.Dialog is designed to provide the same types of features as traditional
 * operating system dialog boxes, including:
 *
 * - dialogs may be modal (user must dismiss the dialog to continue) or 
 * non-modal
 *
 * - dialogs are moveable (user can drag the title bar to move the dialog
 * around)
 *
 * - dialogs may be a fixed size or allow user resizing.
 *
 * - dialogs may have associated help content that is available as a slide-out
 * help panel on the right-hand side of the dialog.
 *
 * Jx.Dialog uses <Jx.ContentLoader> to load content into the content area
 * of the dialog.  Refer to the <Jx.ContentLoader> documentation for details
 * on content options.  Jx.Dialog also uses <Jx.ContentLoader> to load
 * help content, using non-standard options prefixed by 'help' instead of
 * 'content' (see options in initialize below).
 *
 * (code)
 * var dialog = new Jx.Dialog({});
 * (end)
 */
Jx.Dialog = Class.create();
Jx.Dialog.prototype = {
    /**
     * Property: {Function} onClose
     * 
     * if specified by the caller, this is a function to invoke
     * when the dialog closes.
     */
    onClose : null,
    /**
     * Property: {Function} onOpen
     * 
     * if specified by the caller, this is a function to invoke
     * when the dialog opens.
     */
    onOpen : null,
    /**
     * Property: {Function} onChange
     * 
     * if specified by the caller, this is a function to invoke
     * when any input element in the dialog is changed.
     */
    onChange : null,
    /**
     * Property: {String} title
     * 
     * the text to display in the title bar of the dialog.
     */
    title : null,
    /**
     * Property: {HTMLElement} content
     * 
     * the content area element of the dialog, used to load user-
     * specified content into.
     */
    content : null,
    /**
     * Property: {Object} values
     * 
     * when dialog content is loaded, the content is inspected for input
     * elements and a record of them is keep in this object for later
     * reference.
     */
    values : null,
    /**
     * Property: {Object} actions
     * 
     * when dialog content is loaded, the content is inspected for
     * input elements of type 'button' and a record of them is
     * kept in this object for later reference.
     */
    actions : null,
    /**
     * Property: {Function} handler
     * 
     * if specified by the caller, this is a function to invoke
     * when an input of type button is clicked by the user.
     */
    handler : null,
    /**
     * TODO: remove this property, it is provided by Jx.ContentLoader.
     */
    bContentLoaded : null,
    /**
     * Property: {Array} zIndex
     * 
     * class variable containing the last assigned z-index so that
     * dialogs stack correctly.
     */
    zIndex: [101],
    /**
     * Property: {Array} stack
     * 
     * class variable containing the list of visible dialogs used to
     * manage the z-index of non-modal dialogs.  This allows the user
     * to click a dialog and have it appear in front of other dialogs.
     */
    stack: [],
    /**
     * Property: {HTMLElement} blanket
     * 
     * modal dialogs prevent interaction with the rest of the application
     * while they are open, this element is displayed just under the
     * dialog to prevent the user from clicking anything.
     */
    blanket: null,
    /**
     * Property: {Boolean} modal
     * 
     * whether or not the dialog is modal.
     */
    modal: true,
    /**
     * Constructor: initialize
     *
     * Construct a new instance of Jx.Dialog
     *
     * Parameter: {Object} options
     *
     * an object containing some or all of the following options
     *
     * onChange - (optional) {Function} a function to call if the user changes
     * the value of any input in the dialog
     *
     * onClose - (optional) {Function} a function to call when the user closes
     * the dialog
     *
     * onOpen - (optional) {Function} a function to call when the user opens
     * the dialog
     *
     * onContentLoaded - (optional) {Function} a function to call when the
     * content of the dialog has been loaded, used primarily with
     * asynchronous content
     *
     * handler - (optional) {Function} a function to call when the user clicks
     * on any input of type 'button' in the dialog
     *
     * modal - (optional) {Boolean} controls whether the dialog will be modal
     * or not.  The default is to create modal dialogs.
     *
     * top - (optional) {Integer} the distance in pixels to initially place
     * the dialog from the top edge of its container.  Default is 0 unless
     * bottom is specified, in which case the top value is not used.
     *
     * bottom - (optional) {Integer} the distance in pixels to initially place
     * the dialog from the bottom edge of its container.  Default is null, which
     * means that the dialog is not positioned relative to the bottom of its
     * container.  If top and bottom are specified, top is used.
     *
     * left - (optional) {Integer} the distance in pixels to initially place
     * the dialog from the left edge of its container.
     *
     * right - (optional) {Integer} the distance in pixels to initially place
     * the dialog from the right edge of its container.  Default is null, which
     * means that the dialog is not positioned relative to the right of its
     * container.  If left and right are specified, left is used.
     *
     * width - (optional) {Integer} the initial width in pixels of the dialog.
     * The default value is 250 if not specified.
     *
     * height - (optional) {Integer} the initial height in pixels of the 
     * dialog. The default value is 250 if not specified.
     *
     * title - (optional) {String} the title of the dialog box.  "New Dialog"
     * is the default value.
     * 
     * titleHeight - (optional) {Integer} the height, in pixels, of the
     * dialog's title bar.  The default value is 22, and works with the default
     * dialog styles.  Only change this value is you are overriding the
     * default style of the dialog's title bar
     *
     * closeImg - (optional) {String} URL to an image to use for the close
     * button in the dialog title bar area.  The default value is to use
     * the close image provided by Jx.  Only override this value if you are
     * changing the default style of the dialog's title bar.
     *
     * helpID - (optional) {String} passed to <Jx.ContentLoader> for loading
     * help content.
     *
     * helpURL - (optional) {String} passed to <Jx.ContentLoader> for loading
     * help content.
     *
     * helpHTML - (optional) {String} passed to <Jx.ContentLoader> for loading
     * help content.
     *
     * help - (optional) {Object} passed to <Jx.ContentLoader> for loading
     * help content.
     *
     * contentID - (optional) {String} passed to <Jx.ContentLoader> for loading
     * dialog content.
     * 
     * contentURL - (optional) {String} passed to <Jx.ContentLoader> for loading
     * dialog content.
     * 
     * contentHTML - (optional) {String} passed to <Jx.ContentLoader> for loading
     * dialog content.
     * 
     * content - (optional) {Object} passed to <Jx.ContentLoader> for loading
     * dialog content.
     *
     * buttons - (optional) {Array} an array of strings that contain the labels
     * for buttons to create in the Action area of the dialog.
     *
     * id - (optional) {String} an HTML ID to assign to the dialog, primarily
     * used for applying CSS styles to specific dialogs
     *
     * parentObj - (optional) {HTMLElement} a reference to an HTML element that
     * the dialog is to be contained by.  The default value is for the dialog
     * to be contained by the body element.
     *
     * resizeable - (optional) {Boolean} determines whether the dialog is
     * resizeable by the user or not.  Default is false.
     *
     * imageBaseUrl - (optional) {String} the path to the various dialog edge
     * images.  If not specified, then Jx.baseURL is used.
     */
    initialize: function(options) {
        this.initUniqueId();
        this.values = {};
        this.actions = {};
        this.handler = options.handler?options.handler:null;
        if (options.onChange) {this.onChange = options.onChange;}
        if (options.onClose) {this.onClose = options.onClose;}
        if (options.onOpen) {this.onOpen = options.onOpen;}
        if (options.onContentLoaded) {this.onContentLoaded = options.onContentLoaded;}
        
        this.imageBaseUrl = options.imageBaseUrl || Jx.baseURL + "/images/";
        if (this.imageBaseUrl.slice(-1) != '/') {
            this.imageBaseUrl += '/';
        }
        this.modal = typeof options.modal == 'undefined' ? true : options.modal;
        
        var w = options.width || 250;
        var h = options.height || 250;
        var b = (typeof options.bottom != 'undefined') ? options.bottom : null;
        var r = (typeof options.right != 'undefined') ? options.right : null;
        var t = (typeof options.top != 'undefined') ? options.top : (b != null ? null : 0);
        var l = (typeof options.left != 'undefined') ? options.left : (r != null ? null : 0);
        //var t = (options.top && options.top > 0) ? options.top : 0;
        //var l = (options.left && options.left > 0) ? options.left : 0;
        
        this.blanket = document.createElement('div');
        this.blanket.className = 'jxDialogModal';
        this.blanket.style.display = 'none';
        
        if (!window.opera && this.modal) {
            var iframe = document.createElement('iframe');
            //iframe.src = 'javascript:false';
            iframe.className = 'jxDialogShim';
            iframe.scrolling = 'no';
            iframe.frameborder = 0;
            this.blanket.appendChild(iframe);
            if (options.parentObj) {
                $(options.parentObj).appendChild(this.blanket);
            } else {
                document.body.appendChild(this.blanket);            
            }        
        }
        /* create the dialog box first so we can measure it */
        var d2 = document.createElement('div');
        d2.style.width = w + 'px';
        d2.style.height = h + 'px';
        d2.className = 'jxDialog';
        
        this.title = document.createElement('div');
        this.title.className = 'jxDialogTitle';
        /* this is required for IE and Opera */
        //this.title.style.height = '22px';
        
        // It would be best to just let the CSS decide this.
        // Element.setBorderBoxSize(this.title, {height:options.titleHeight || 22});
        
        this.dragHandle = document.createElement('div');
        this.dragHandle.innerHTML = options.title || '';
        this.dragHandle.style.width = '100%';
        this.title.appendChild(this.dragHandle);
        
        /* element must be in the page to be measured */
        this.title.style.visibility = 'hidden';
        document.getElementsByTagName('BODY')[0].appendChild(this.title);
        var titleHeight = Element.getBorderBoxSize(this.title);
        document.getElementsByTagName('BODY')[0].removeChild(this.title);
        this.title.style.visibility = '';
        
        d2.appendChild(this.title);
        
        var atag = document.createElement('a');
        atag.href = 'javascript:void(0)';
        atag.className = 'jxDialogCloseButton';
        atag.onclick = this.close.bindAsEventListener(this);
        
        var close = document.createElement('img');
        if(options.closeImg) {
            close.src = options.closeImg;
        }
        else {
            close.src = this.imageBaseUrl + 'icon_close.png';
        }
        close.alt = 'Close Dialog';
        close.title = 'Close Dialog';
        atag.appendChild(close);
        this.title.appendChild(atag);
        
        if (options.helpID || options.helpHTML || options.helpURL || options.help) {
            var atag2 = document.createElement('a');
            atag2.href = 'javascript:void(0)';
            atag2.className = 'jxDialogHelpButton';
            atag2.onclick = this.toggleHelp.bindAsEventListener(this);
            //TODO: don't add help in title if it isn't needed
            var help = document.createElement('img');
            help.src = this.imageBaseUrl + 'icon_quickhelp.png';
            help.alt = 'Help';
            help.title = 'Help';
            atag2.appendChild(help);
            this.title.appendChild(atag2);
        }
        
        this.action = document.createElement('div');
        this.action.className = 'jxDialogAction';
        // It would be best to just let the CSS decide this.
        //Element.setBorderBoxSize(this.action, {height:30});
        /* element must be in the page to be measured */
        this.action.style.visibility = 'hidden';
        document.getElementsByTagName('BODY')[0].appendChild(this.action);
        var actionHeight = parseInt(Element.getStyle(this.action, 'height'));
        document.getElementsByTagName('BODY')[0].removeChild(this.action);
        this.action.style.visibility = '';
        
        var contentHeight = h - titleHeight.height - actionHeight;
        this.content = document.createElement('div');
        this.content.className = 'jxDialogContent';
        Element.setBorderBoxSize(this.content, {height: contentHeight});
        
        if (options.helpID || options.helpHTML || options.helpURL || options.help) {
            this.help = document.createElement('div');
            this.help.className = 'jxDialogHelp';
            this.help.style.display = 'none';
            this.help.isVisible = false;
            var helpOpts = {};
            helpOpts.contentID = options.helpID;
            helpOpts.content = options.help;
            helpOpts.contentURL = options.helpURL;
            helpOpts.contentHTML = options.helpHTML;
            helpOpts.onContentLoaded = this.onHelpContentLoaded.bind(this);
            this.loadContent(this.help, helpOpts);
            d2.appendChild(this.help);
        }
        
        d2.appendChild(this.content);
        
        if (options.buttons) {
            this.setButtons(options.buttons);
        }
        
        d2.appendChild(this.action);
        var contentOpts = {};
        contentOpts.contentID = options.contentID;
        contentOpts.content = options.content;
        contentOpts.contentURL = options.contentURL;
        contentOpts.contentHTML = options.contentHTML;
        contentOpts.onContentLoaded = this.onDialogContentLoaded.bind(this);
        
        this.loadContent(this.content, contentOpts);
        
        /* element must be in the page to be measured */
        d2.style.visibility = 'hidden';
        document.body.appendChild(d2);
        
        /* the outer box needs to be sized to the border box size plus margins
         * of the inner box.
         */
         
        this.dialogBoxSize = Element.getBorderBoxSize(d2);
        this.dialogBoxMargins = Element.getMarginSize(d2);
        
        var containerSize = {width: this.dialogBoxSize.width, height: this.dialogBoxSize.height};
        containerSize.width += this.dialogBoxMargins.left + this.dialogBoxMargins.right; 
        containerSize.height += this.dialogBoxMargins.top + this.dialogBoxMargins.bottom;
        
        document.body.removeChild(d2);
        d2.style.visibility = '';
        
        /* now create overall container with the correct size */
        var d = document.createElement('div');
        d.className = 'jxDialogContainer';
        d.style.display = "none";
        // this line seemes to be very wrong
        d.id = options.id;
        Element.setBorderBoxSize(d, {width:(containerSize.width), height:(containerSize.height)});
        
        d.style.position = 'absolute';
        if (t != null) {
            d.style.top = (t) + 'px';
        } else {
            d.style.bottom = (b) + 'px';
        }
        if (l != null) {
            d.style.left = (l) + 'px';
        } else {
            d.style.right = (r) + 'px';
        }

        if (options.parentObj) {
            $(options.parentObj).appendChild(d);
        } else {
            document.body.appendChild(d);            
        }

        /* Background */
        this.DTOffset = 0;
        this.DBOffset = 0;
        this.DROffset = 0;
        this.DLOffset = 0;
        this.decorationOffsets = {top:0, right: 0, bottom: 0, left: 0};
        
        /* top left */
        
        var imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgTL';
        var img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_tl.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.decorationOffsets.top += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.left += parseInt(Element.getStyle(img,'height'));

        /* top right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgTR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_tr.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.decorationOffsets.top += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.right += parseInt(Element.getStyle(img,'height'));

        /* bottom right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgBR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_br.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.decorationOffsets.bottom += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.right += parseInt(Element.getStyle(img,'height'));

        /* bottom left */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgBL';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_bl.png';
        img.className = 'png24'; /* apply png hack for IE */
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.decorationOffsets.bottom += parseInt(Element.getStyle(img,'width'));
        this.decorationOffsets.left += parseInt(Element.getStyle(img,'height'));

        /* top */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgT';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_t.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.width = containerSize.width-this.decorationOffsets.top + 'px';
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.topImg = img;

        /* bottom */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgB';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_b.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.width = containerSize.width-this.decorationOffsets.bottom + 'px';
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.bottomImg = img;

        /* left */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgL';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_l.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.height = containerSize.height-this.decorationOffsets.left + 'px';
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.leftImg = img;

        /* right */
        imgContainer = document.createElement('div');
        imgContainer.className = 'jxDialogBgR';
        img = document.createElement('img');
        img.src = this.imageBaseUrl + 'dialog_glow_r.png';
        img.className = 'png24'; /* apply png hack for IE */
        img.style.height = containerSize.height-this.decorationOffsets.right + 'px';
        imgContainer.appendChild(img);
        d.appendChild(imgContainer);
        this.rightImg = img;

        d.appendChild(d2);
        
        Event.observe(d, 'mousedown', this.mouseDown.bind(this));
        Event.observe(this.title, 'mousedown', this.mouseDown.bind(this));
        
        this.domObj = d;
        this.innerDialogObj = d2;
        
        if (options.resizeable) {
            this.resizeHandle = document.createElement('div');
            this.resizeHandle.className = 'jxDialogResize';
            this.resizeHandle.style.position = 'absolute'; //required for Draggable
            this.domObj.appendChild(this.resizeHandle);

            this.resizeHandleSize = {width:parseInt(Element.getStyle(this.resizeHandle,'width')),
                                     height:parseInt(Element.getStyle(this.resizeHandle,'height'))};

            this.resizeHandle.style.top = (containerSize.height-this.resizeHandleSize.height) + 'px';
            this.resizeHandle.style.left = (containerSize.width-this.resizeHandleSize.width) + 'px';
            new Draggable(this.resizeHandle, {starteffect: false, endeffect: false,change:this.ondrag.bind(this), zindex: 0});
        }
        
        this.bOpen = false;
    },
    /**
     * Function: mouseDown
     *
     * handle the user clicking the title area, promote the dialog to the
     * top of the stack so it is fully visible to the user.
     */
    mouseDown: function() {
        for (var i=0; i<this.stack.length; i++) {
            if (this.stack[i] == this) {
                this.stack.splice(i, 1);
                this.stack.push(this);
            }
        }
        for (var i=0; i<this.stack.length; i++) {
            this.stack[i].domObj.style.zIndex = (101 + i);
        }
    },
    /** 
     * Function: ondrag
     *
     * callback function for user-resizing of the dialog
     *
     * Parameter: obj
     *
     * a drag object that indicates what happened.
     */
    ondrag: function(obj) {
        this.mouseDown();
        
        var delta = obj.currentDelta();
        //delta is top/left of resize image.  Bottom/right of the dialog needs to be
        //adjusted for size of image (20x20) and for the shadow (6x6) resulting in
        //an additional 14 pixels from the top/left of the resize image.
        var deltaX = delta[0] + this.resizeHandleSize.width - this.dialogBoxMargins.left - this.dialogBoxMargins.right;
        var deltaY = delta[1] + this.resizeHandleSize.height - this.dialogBoxMargins.top - this.dialogBoxMargins.bottom;
        this.resize({width: deltaX, height: deltaY});
    },
    /**
     * Function: resize
     *
     * programmatically resize the dialog in either or both dimensions
     *
     * Parameter: {Object} newSize
     *
     * an object containing either or both 'width' and 'height' properties
     * that hold the new width or height in pixels
     */
    resize: function(newSize) {
        var obj = this.title.parentNode;
        var titleSize = Element.getBorderBoxSize(this.title);
        var actionSize = Element.getBorderBoxSize(this.action);
        var oldDisplay = this.domObj.style.display;
        var oldVisibility = this.domObj.style.visibility;
        if (oldDisplay == 'none') {
            this.domObj.style.visibility = 'hidden';
            this.domObj.style.display = 'block';
        }
        /* sizing logic
         *
         * we need to set the size of this.innerDialogObj to the requested size
         * then size the title, content and action area within it and expand
         * this.domObj to contain it properly.  Then we need to position the
         * images appropriately too.
         */
        if (newSize.width) {
            var outerWidth = newSize.width + this.dialogBoxMargins.left + this.dialogBoxMargins.right;
            Element.setBorderBoxSize(this.domObj, {width: outerWidth});
            /* setBorderBoxSize accommodates margins, which we don't want in this case so use outerWidth
             * and it all events out to the right number in the end
             */
            Element.setBorderBoxSize(obj, {width:outerWidth});
            Element.setBorderBoxSize(this.topImg, {width:outerWidth-this.decorationOffsets.top});
            Element.setBorderBoxSize(this.bottomImg, {width:outerWidth-this.decorationOffsets.bottom});
            var contentSize = Element.getContentBoxSize(obj);
            //Element.setBorderBoxSize(this.content, {width:contentSize.width});
        }
        if (newSize.height) {
            var outerHeight = newSize.height + this.dialogBoxMargins.top + this.dialogBoxMargins.bottom;
            Element.setBorderBoxSize(this.domObj, {height: outerHeight});
            Element.setBorderBoxSize(obj, {height:outerHeight});
            Element.setBorderBoxSize(this.leftImg, {height:outerHeight-this.decorationOffsets.left});
            Element.setBorderBoxSize(this.rightImg, {height:outerHeight-this.decorationOffsets.right});
            var contentH = newSize.height-titleSize.height-actionSize.height;
            Element.setBorderBoxSize(this.content, {height:contentH});
            if (this.help) {
                Element.setBorderBoxSize(this.help, {height:newSize.height - titleSize.height});
            }
        }
        if (oldDisplay == 'none') {
            this.domObj.style.visibility = oldVisibility;
            this.domObj.style.display = 'none';
        }
        if (this.content.resize) {
          this.content.resize();
        }
    },
    /**
     * Function: setTitle
     *
     * set the text of the dialog title.
     *
     * Parameter: {String} title
     *
     * the new title
     */
    setTitle: function( title ) {
        this.title.childNodes[0].innerHTML = title;
    },
    /**
     * Function: setButtons
     *
     * create buttons in the action area of the dialog
     *
     * Parameter: {Array} buttons
     *
     * an array of strings containing the labels to place on the
     * buttons.  One button is created for each label.
     */
    setButtons: function(buttons) {
        this.action.innerHTML = '';
        for (var i=0; i<buttons.length;i++) {
            var button = document.createElement('input');
            button.id = buttons[i];
            button.type = 'button';
            button.className = 'normalButton';
            button.name = buttons[i];
            button.value = buttons[i];
            button.onclick = this.buttonHandler.bind(this, button);
            this.action.appendChild(button);
            this.uniqueIdRefs[button.id] = button;
        }
    },
    /**
     * Function: processInputs
     * 
     * look through the DOM obj for nodes of type INPUT and register
     * for events on them.
     *
     * Parameter: {HTMLElement} obj
     *
     * the DOM obj to process
     */
    processInputs : function(obj) {
        for (var i=0;i<obj.childNodes.length; i++) {
            var node = obj.childNodes[i];
            if (node.tagName == 'INPUT' || node.tagName == 'SELECT' || node.tagName == 'TEXTAREA') {
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
    /**
     * Function: buttonHandler
     *
     * handle button events, dispatching to user handler if required.
     *
     * Parameter: {HTMLElement} input
     *
     * the input element that triggered a click event
     *
     * Parameter: {Event} event
     *
     * the event object
     */
    buttonHandler : function(input, event) {
        if (this.handler) {
            this.handler(input.value, this);
        }
    },
    /**
     * Function: onChangeHandler
     *
     * handle onchange events, dispatching to user handler if required.
     *
     * Parameter: {HTMLElement} input
     *
     * the input element that triggered an onchange event
     *
     * Parameter: {Event} event
     *
     * the event object
     */
    onChangeHandler: function(input, event) {
        if (this.onChange) {
            this.onChange(input, this);
        }
    },
    /**
     * Function: getValue
     *
     * retrieve the value of an input element in the dialog.  This is done
     * safely so depending on the type of the input.
     *
     * Parameter: {String} name
     *
     * the id of the element to get the value of
     *
     * Returns: {String} the value or an empty string.
     */
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
    /**
     * Function: setValue
     *
     * set the value of an input element in the dialog.  This is done
     * safely  depending on the type of the input.
     *
     * Parameter: {String} name
     *
     * the id of the element to set the value of
     *
     * Parameter: {String} value
     *
     * the value to set
     */
    setValue : function( name, value ) {
        if (typeof this.values[name] != 'undefined') {
            if (this.values[name].type == 'text') {
                this.values[name].value = value;
            }
        }
    },
    /**
     * Function: show
     *
     * show the dialog
     */
    show : function( ) {
        this.stack.push(this);
        if (this.modal) {
            this.blanket.style.zIndex = this.zIndex[0]++;
            this.blanket.style.visibility = 'visible';
            this.blanket.style.display = 'block';
        }
        this.domObj.style.zIndex = this.zIndex[0]++;
        Effect.Appear(this.domObj, {duration: 0.1});
        this.domObj.style.display = 'block';
        new Draggable(this.domObj, {handle:this.dragHandle, starteffect: false, endeffect: false});
        
    },
    /**
     * Function: hide
     *
     * hide the dialog
     */
    hide : function() {
        for (var i=0; i<this.stack.length; i++) {
            if (this.stack[i] == this) {
                this.stack.splice(i,1);
            }
        }
        this.zIndex[0]--;
        Effect.Fade(this.domObj, {duration: 0.3});
        //this.domObj.style.display = 'none';
        if (this.modal) {
            this.blanket.style.visibility = 'hidden';
            this.zIndex[0]--;
        }
        
    },
    /**
     * Function: open
     *
     * open the dialog.  This may be delayed depending on the 
     * asynchronous loading of dialog content.  The onOpen
     * callback function is called when the dialog actually
     * opens
     */
    open: function() {
        if (!this.bOpen) {
            this.bOpen = true;
        }
        if (this.bContentLoaded) {
            this.show();
            if (this.onOpen) this.onOpen();
        }
    },
    /**
     * Function: close
     * 
     * close the dialog and trigger the onClose callback function
     * if necessary
     */
    close: function() {
        this.bOpen = false;
        this.hide();
        if (this.onClose) this.onClose();
    },
    /**
     * Function: onDialogContentLoaded
     *
     * handle the dialog content being loaded.  This triggers
     * processing of inputs and the onContentLoaded callback
     * function (if necessary).  Also, if the dialog was previously
     * requested to be opened, this will actually open it.
     */
    onDialogContentLoaded : function() {
        this.processInputs(this.content);
        if (this.onContentLoaded) {
            this.onContentLoaded(this);
        }
        if (this.bOpen) {
            //may need to do this?
            //window.setTimeout(this.open.bind(this),1);
            this.open();
            this.bOpen = false;
        }
    },
    /**
     * Function: onHelpContentLoaded
     *
     * handle the help content being loaded.  
     */
    onHelpContentLoaded : function() {
        var img = document.createElement('img');
        img.className = 'jxDialogHelpCloseButton png24';
        img.src = this.imageBaseUrl + 'help_close.png';
        img.onclick = this.toggleHelp.bind(this);
        img.alt = 'Close Help';
        img.title = 'Close Help';
        this.help.appendChild(img);
    },
    /**
     * Function: toggleHelp
     *
     * show or hide the help panel.
     */
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
Object.extend(Jx.Dialog.prototype, Jx.UniqueId.prototype);
Object.extend(Jx.Dialog.prototype, Jx.ContentLoader.prototype);