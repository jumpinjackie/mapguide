/**
 * Copyright (C) 2010 Autodesk, Inc. All rights reserved.
 */

var PreviewDialog = function(options)
{
    this.initialize(options);
};

PreviewDialog.prototype = 
{
    jxDialog : null,
    previewFrame : null,
    innerDoc : null,
    indicator : null,
    previewPicture : null,
    printPicture : null,
    topLeftXYLabel : null,
    bottomRightXYLabel : null,
    previewContainer : null,
    pictureContainer : null,
    printButton : null,
    cancelButton : null,
    loadingErrorMessage : null,
    printStyle: null,
    previewStyle: null,
    // The offset from the border of the paper, in px 
    csLabelOffset : 5,
    // The print margin, in inches
    printMargin: 0,
    mapInfo : {sessionID: "", name: ""},
    captureInfo : {topLeftCs : {x : 0, y : 0}, bottomRightCs : {x : 0, y : 0}, paperSize:{w : 0, h : 0}, scaleDenominator : 0, rotation : 0, params1 : "", params2 : ""},
    params : null,
    
    // The callback will be called right after the JxDialog content has been loaded.
    // The callback accepts 1 parameter: windowName (string)
    // Here it is used to submit the quick plot form to the preview frame.
    contentLoadedCallback : null,
    
    initialize : function(options)
    {
        this.mapInfo      = options.mapInfo;
        this.captureInfo  = options.captureInfo;
        this.params       = options.params;
        
        this.jxDialog = new Jx.Dialog(
        {
            modal: true,
            width:400,
            height: 400,
            content: '<table border="0" cellspacing="0" cellpadding="0" id="PreviewContainer">' + 
                     '	<tr>' + 
                     '		<td>' + 
                     '			<iframe id="PreviewFrame" scrolling="no" frameborder="0" style="border: 0px; width: 400px; height: 300px;" src="about:blank"></iframe>' +
                     '		</td>' +
                     '  </tr>' +
                     '</table>'
        });
        
        this.jxDialog.addEvent("open", this.jxDialogOpened.bind(this, true));
        this.jxDialog.addEvent("contentLoaded", this.jxDialogContentLoaded.bind(this));
        // Listen to the fade complete event to close the dialog
        var o = this.jxDialog.domObj.get("tween").addEvent("complete", this.closeJxDialog.bind(this));
    },
    
    open : function(contentLoadedCallback)
    {
        this.contentLoadedCallback = contentLoadedCallback;
        this.jxDialog.loadContent(this.jxDialog.content);
        this.jxDialog.open();
    },
    
    print : function()
    {
        //
        this.previewFrame.contentWindow.doPrint();
    },
    
    cancel : function()
    {
        this.indicator.style.display     = "inline";
        this.indicator.style.visibility  = "visible";
        this.indicator.setOpacity(1);
        
        this.loadingErrorMessage.fade(0);
        this.loadingErrorMessage.style.display = "none";
        // Hide the picture but don't set the 'display' style property to avoid messing the print layout up
        this.printPicture.style.height   = "0px";
        
        if (this.topLeftXYLabel && this.bottomRightXYLabel)
        {
            this.topLeftXYLabel.setOpacity(0);
            this.bottomRightXYLabel.setOpacity(0);
            this.printLabel.setOpacity(0);
            // Remove the labels
            this.topLeftXYLabel.parentNode.removeChild(this.topLeftXYLabel);
            this.topLeftXYLabel     = null;
            this.bottomRightXYLabel.parentNode.removeChild(this.bottomRightXYLabel);
            this.bottomRightXYLabel = null;
            this.printLabel.parentNode.removeChild(this.printLabel);
            this.printLabel         = null;
        }
        
        this.isClosing = true;
        this.jxDialog.domObj.fade(0);
    },
    
    jxDialogContentLoaded : function()
    {
        // Set the window name for the preview frame
        this.previewFrame = $("PreviewFrame");
        this.previewFrame.contentWindow.name = "QuickPlotPreviewFrame";
        
        // Inform the listener that the content is ready, then the Quick Plot panel can submit its data to the preview frame
        if (this.contentLoadedCallback)
        {
            this.contentLoadedCallback(this.previewFrame.contentWindow.name);
        }
    },
    
    jxDialogOpened : function(setFrameUrl)
    {
        if (this.previewInnerIsLoaded)
        {
            // Resize the preview frame according to the monitor resolution
            this.innerDoc         = this.previewFrame.contentWindow.document;
            
            // This is a workaround for JxLib3.0, which disable the getDimensions for Body.
            //var box       = $(document.body).getDimensions();
            var box =  null;
            if($("AppContainer") != null)
            {
                box =$("AppContainer").getDimensions();
            }
            else
            {
                box =$("thePage").getDimensions();
            }
            
            // Width of preview dialog = screen width * factor
            var factor    = 0.5;
            this.previewContainer = $(this.innerDoc.getElementById("PreviewContainer"));
            this.previewContainer.style.width = box.width * factor + "px";
            this.pictureContainer = $(this.innerDoc.getElementById("PictureContainer"));
            var pcBox = this.pictureContainer.getContentBoxSize();
            
            this.indicator        = $(this.innerDoc.getElementById("ProgressIndicator"));
            
            var paperSize  = this.captureInfo.paperSize;
            this.paperSize = paperSize;
            
            var ratio = paperSize.w / paperSize.h;
            
            // Resize the indicator
            this.indicator.style.width  = pcBox.width + "px";
            this.indicator.style.height = pcBox.width / ratio + "px";
            // Set a explicit size to the container
            this.pictureContainer.style.width  = this.indicator.style.width;
            this.pictureContainer.style.height = this.indicator.style.height;
            // Get the styles for the print picture
            var rules = this.innerDoc.styleSheets[1].cssRules || this.innerDoc.styleSheets[1].rules;
            this.previewStyle = rules[0];
            rules     = this.innerDoc.styleSheets[2].cssRules || this.innerDoc.styleSheets[2].rules;
            this.printStyle   = rules[0];
            
            // Reset the background
            this.indicator.style.background = "url(progress_indicator.gif) no-repeat cneter center";
            
            this.loadingErrorMessage = $(this.innerDoc.getElementById("PictureLoadingErrorMessage"));
            this.loadingErrorMessage.setOpacity(0);
            
            // Set the picture url
            var src = "GeneratePicture.php?session_id=" + this.mapInfo.sessionID +
                  "&map_name=" + this.mapInfo.name + 
                  "&print_dpi=" + this.params.printDpi + 
                  "&paper_size=" + this.captureInfo.paperSize.w + "," + this.captureInfo.paperSize.h + 
                  "&box=" + this.captureInfo.params1 +
                  "&normalized_box=" + this.captureInfo.params2 + 
                  "&scale_denominator=" + this.captureInfo.scaleDenominator + 
                  "&rotation=" + this.captureInfo.rotation +
                  "&timestamp=" + (new Date()).getTime(); 
                
            this.printPicture       = $(this.innerDoc.getElementById("PrintPicture"));
            
            // Listen to print picture onload vent
            this.printPicture.addEvent("load", this.printPictureLoaded.bind(this));
            this.printPicture.addEvent("error", this.printPictureLoadError.bind(this));
            this.printPicture.src   = src;
            
            var innerBox  = this.previewContainer.getMarginBoxSize();
            // Resize the frame according to the inner container's 
            this.previewFrame.style.width  = innerBox.width + "px";
            this.previewFrame.style.height = innerBox.height + "px";
            
            // Hide the title bar
            this.jxDialog.title.style.display  = "none";
            // Hide the chrome
            this.jxDialog.chrome.style.display = "none";
            
            // Disable the print button until the image is ready
            this.printButton           = this.innerDoc.getElementById("PrintButton");
            this.cancelButton          = this.innerDoc.getElementById("CancelButton");
            this.printButton.disabled  = true;
            this.cancelButton.disabled = true;
            
            var delta     = {x: 22, y: 43};
            var container = $("PreviewContainer");
            var size      = container.getMarginBoxSize();
            this.jxDialog.resize(size.width + delta.x, size.height + delta.y, true);
            
            this.jxDialog.domObj.fade(1);
            this.resizeIsPending = false;
            
            this.previewInnerIsLoaded = false;
        }
        else
        {
            this.jxDialog.domObj.setOpacity(0);
            this.resizeIsPending = true;
        }
    },
    
    previewInnerLoaded: function()
    {
        //HACK: There is some state within these labels that gets invalidated in subsequent
        //previews (ie. You click "Generate" on the Quick Plot panel multiple times without cancelling out the dialog).
        //So detect this invalid state and remove these labels, forcing a rebuild
        if (this.topLeftXYLabel)
        {
            var tlDoc = this.topLeftXYLabel.getDocument();
            if (tlDoc.defaultView == null)
            {
                this.topLeftXYLabel.parentNode.removeChild(this.topLeftXYLabel);
                this.topLeftXYLabel = null;
            }
        }
        
        if (this.bottomRightXYLabel)
        {
            var brDoc = this.bottomRightXYLabel.getDocument();
            if (brDoc.defaultView == null)
            {
                this.bottomRightXYLabel.parentNode.removeChild(this.bottomRightXYLabel);
                this.bottomRightXYLabel = null;
            }
        }
        
        if (this.printLabel)
        {
            var prDoc = this.printLabel.getDocument();
            if (prDoc.defaultView == null)
            {
                this.printLabel.parentNode.removeChild(this.printLabel);
                this.printLabel = null;
            }
        }
    
        this.previewInnerIsLoaded = true;
        if (this.resizeIsPending)
        {
            this.jxDialogOpened(false);
        }
    },
    
    closeJxDialog : function()
    {
        if (this.isClosing)
        {
            this.jxDialog.close();
            this.isClosing = false;
        }
    },
    
    printPictureLoaded : function()
    {
        var size = {width: parseInt(this.indicator.style.width), height: parseInt(this.indicator.style.height)};
        
        this.indicator.fade(0);
        this.indicator.style.display = "none";
        
        // Set the preview size
        this.printPicture.setOpacity(0);
        // Clear the inline style
        this.printPicture.style.width   = "";
        this.previewStyle.style.width   = size.width + "px";
        this.previewStyle.style.height  = size.height + "px";
        this.printPicture.fade(1);
        
        // Set the print size
        // NOTE: It works only with a 96 dpi device dpi
        var deviceDpi  = 96;
        var idealSize  = {width:(this.paperSize.w / 25.4 - 2 * this.printMargin) * deviceDpi, height:(this.paperSize.h / 25.4 - 2 * this.printMargin) * deviceDpi};
        // Get the size of the print frame
        var docSize    = $(this.innerDoc.body).getContentBoxSize();
        var realHeight = idealSize.height - (docSize.height - size.height);
        var realWidth  = realHeight * this.paperSize.w / this.paperSize.h;
        if (realWidth > idealSize.width)
        {
            realWidth = idealSize.width;
            realHeight = realWidth / (this.paperSize.w / this.paperSize.h);
        }
        
        this.printStyle.style.width  = realWidth + "px";
        this.printStyle.style.height = realHeight + "px";
        
        var showLabels = (this.params.showCoordinateLabels == true);
        if (showLabels)
        {
            // Create the coordinates labels
            if (!this.topLeftXYLabel)
            {
                this.topLeftXYLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.topLeftCs.x, this.captureInfo.topLeftCs.y, "TopLeftXYLabel");
                this.topLeftXYLabel.setOpacity(0);
            }
            
            if (!this.bottomRightXYLabel)
            {
                this.bottomRightXYLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.bottomRightCs.x, this.captureInfo.bottomRightCs.y, "BottomRightXYLabel");
                this.bottomRightXYLabel.setOpacity(0);
            }
        }
        if (!this.printLabel)
        {
            this.printLabel = this.createCoordinateLabel(this.pictureContainer, this.captureInfo.bottomRightCs.x, this.captureInfo.bottomRightCs.y, "PrintLabel");
        }
        
        // Set the correct positions for the labels
        var pos    = this.getContentPosition(this.pictureContainer);
        var picDim = this.pictureContainer.getContentBoxSize();
        
        var labelDim = null;
        if (showLabels) {
            this.topLeftXYLabel.style.left     = pos.left + this.csLabelOffset + "px";
            this.topLeftXYLabel.style.top      = pos.top + this.csLabelOffset + "px"; 
            labelDim = this.bottomRightXYLabel.getMarginBoxSize();
            
            this.bottomRightXYLabel.className  = "ScreenOnly";
            this.bottomRightXYLabel.style.left = pos.left + picDim.width - this.csLabelOffset - labelDim.width + "px";
            this.bottomRightXYLabel.style.top  = pos.top + picDim.height - this.csLabelOffset - labelDim.height + "px";
            
            this.topLeftXYLabel.fade(1);
            this.bottomRightXYLabel.fade(1);
        }
        labelDim = this.printLabel.getMarginBoxSize();
        this.printLabel.className          = "PrintOnly";
        this.printLabel.style.left         = pos.left + realWidth - this.csLabelOffset - labelDim.width + "px";
        this.printLabel.style.top          = pos.top + realHeight - this.csLabelOffset - labelDim.height + "px";
        
        // Enable the print button
        this.printButton.disabled  = false;
        this.cancelButton.disabled = false;
    },
    
    printPictureLoadError : function()
    {
        this.indicator.fade(0);
        this.indicator.style.display = "none";
        this.loadingErrorMessage.style.display = "inline";
        this.loadingErrorMessage.setOpacity(0);
        this.loadingErrorMessage.fade(1);
        this.cancelButton.disabled = false;
    },
    
    createCoordinateLabel: function(container, cX, cY, id)
    {
        cX = cX + "";
        cY = cY + "";
        var digits = 6;
        
        var index = cX.indexOf(".");
        if (index > -1)
        {
            cX = cX.substr(0, index + digits + 1);
        }
        
        index = cY.indexOf(".");
        if (index > -1)
        {
            cY = cY.substr(0, index + digits + 1);
        }
        
        var label = this.innerDoc.createElement("div");
        container.appendChild(label);
        label.id  = id;
        label.style.cssText = "border:solid 1px black; padding:1px; background:#F2F2F2; color:black; font-size:8pt; z-index:1000; " +
                              "position:absolute; white-space:nowrap";
        label.innerHTML     = "X: " + cX + " Y: " + cY;
        
        return $(label);
    },
    
    getContentPosition: function(element)
    {
        var offsetLeft = 0;
        var offsetTop  = 0;
        var border     = 0;
        
        while (element && element.tagName.toLowerCase() != "body" && element.tagName.toLowerCase() != "html")
        {
            offsetLeft += element.offsetLeft;
            offsetTop  += element.offsetTop;
            
            border      = parseInt(element.style.borderLeftWidth);
            if (!isNaN(border))
            {
                offsetLeft += border;
            }
            
            border      = parseInt(element.style.borderTopWidth);
            if (!isNaN(border))
            {
                offsetTop += border;
            }
            
            element     = element.offsetParent;
        }
        
        return {left: offsetLeft, top: offsetTop};
    }
};