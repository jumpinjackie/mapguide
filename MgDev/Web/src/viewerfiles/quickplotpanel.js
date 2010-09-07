function getMapFrame()
{
    if (targetType == 1)
    {
        // In task pane
        return parent.parent.mapFrame;
    }
    else if (targetType == 2)
    {
        // In a new window
        return opener.parent.parent.mapFrame;
    }
    else if (targetType == 3)
    {
        // In a specified frame. The administrator needs to implement this method
        // according to the nested level of this frame
    }
}


function restoreUI()
{
    setAdvancedOptionsUI(false);
    
    // Read the last used options
    lastPaperSize = Cookie.read("QuickPlotLastUsedPaperSize");
    lastScale     = Cookie.read("QuickPlotLastUsedScaling");
    lastDPI       = Cookie.read("QuickPlotLastUsedDPI");
    
    if (lastPaperSize != null)
    {
        document.getElementById("PaperList").value   = lastPaperSize;
    }
    
    if (lastScale != null)
    {
        document.getElementById("ScalingList").value = lastScale;
    }
    
    if (lastDPI != null)
    {
        document.getElementById("DPIList").value     = lastDPI;
    }
    
    // Set the string resource
    var mapCapturer = getMapFrame().mapCapturer;
    mapCapturer.resolutionWarning = captureWarningMessage;
}

function cleanup()
{
    // Remove the map capture box
    var mapCapturer = getMapFrame().mapCapturer;
    if (mapCapturer)
    {
        mapCapturer.deactivate();
    }
    
    // Detach the dialog closed callback to prevent calling a function from an unloaded document
    // which will lead to a runtime error
    var dialog = getMapFrame().quickPlotPreviewDialog;
    if (dialog)
    {
        dialog.closedCallback = null;
    }

    // Close the preview dialog if it is opened.
    getMapFrame().endPlotPreview();
}

function setAdvancedOptionsUI(enabled)
{
    document.getElementById("AdvancedOptionsCheckBox").checked = enabled;
    document.getElementById("PaperList").disabled   = !enabled;
    document.getElementById("ScalingList").disabled = !enabled;
    document.getElementById("DPIList").disabled     = !enabled;
    
    var mapCapturer = getMapFrame().mapCapturer;
    
    if (enabled)
    {
        mapCapturer.activate();
        drawCaptureBox();
    }
    else
    {
        mapCapturer.deactivate();
    }
}

function updateUI(enabled)
{
    var doc   = $(document);
    var cover = $(doc.getElementById("ModalDialogCover"));
    
    if (cover == null)
    {
        cover = doc.createElement("div");
        cover = $(cover);
        cover.setOpacity(0);
        doc.body.appendChild(cover);
        cover.id = "ModalDialogCover";
        cover.style.display    = "block";
        cover.style.position   = "absolute";
        cover.style.left       = "0px";
        cover.style.top        = "0px";
        cover.style.zIndex     = 1000;
        cover.style.background = "black";
        cover.style.opacity    = 0;
        cover.style.visibility = "hidden";
    }
    
    // Set a proper size to the cover
    var box = $(doc.body).getDimensions();
    cover.style.width  = box.width + "px";
    cover.style.height = box.height + "px";
    
    if (enabled)
    {
        cover.fade(0);
    }
    else
    {
        cover.fade(0.2);
        doc.getElementById("GeneratePlotButton").blur();
    }
}

function generatePlot()
{
    var mapCapturer = getMapFrame().mapCapturer;

    if (!advancedOptionsOn())
    {
        // Get paper size. Use the last used paper size by default
        mapCapturer.setSize(getPaperSize(), getScale());
    }
    
    getMapFrame().previewQuickPlot(onPreviewDialogContentLoaded, onPreviewDialogOpened, onPreviewDialogClosed, getPrintDpi(), rendererName);
}

function onPreviewDialogContentLoaded(windowName)
{
    // Save the advanced options to a cookie
    var cookieDuration = 365;
    Cookie.write("QuickPlotLastUsedPaperSize", document.getElementById("PaperList").value, {duration:cookieDuration});
    Cookie.write("QuickPlotLastUsedScaling", document.getElementById("ScalingList").value, {duration:cookieDuration});
    Cookie.write("QuickPlotLastUsedDPI", document.getElementById("DPIList").value, {duration:cookieDuration});

    // Submit the form
    var form = document.getElementById("Form1");
    form.target = windowName;
    form.submit();
}

function onPreviewDialogOpened()
{
    updateUI(false);
}

function onPreviewDialogClosed()
{
    updateUI(true);
}

function advancedOptionsOn()
{
    var o = document.getElementById("AdvancedOptionsCheckBox");
    if (o && o.checked)
    {
        return true;
    }
    
    return false;
}

function getPaperSize()
{
    var value = document.getElementById("PaperList").value.split(",");
    var size = {width: parseFloat(value[0]), height: parseFloat(value[1])};

    if (!advancedOptionsOn())
    {
        // Calculate the paper size to make sure it has a same ratio with the viweport
        var paperRatio = size.width / size.height;
        var w          = getMapFrame().window;
        var viewRatio  = w.mapDevW / w.mapDevH;

        if (paperRatio > viewRatio)
        {
            size.width  = size.height * viewRatio;
        }
        else
        {
            size.height = size.width / viewRatio;
        }
    }

    return size;
}

function getScale()
{
    var scale = 0;
    if (advancedOptionsOn())
    {
        scale = document.getElementById("ScalingList").value;
    }
    else
    {
        var map        = getMapFrame();
        var paperSize  = getPaperSize();
        var viewerSize = {width: map.extX2 - map.extX1, height: map.extY1 - map.extY2};
        var factor     = map.GetMetersPerUnits();
        
        if (paperSize.width / paperSize.height > viewerSize.width / viewerSize.height)
        {
            scale = viewerSize.height * factor * 1000 / paperSize.height;
        } 
        else
        {
            scale = viewerSize.width * factor * 1000 / paperSize.width;
        }
    }

    scale = parseInt(scale);
    // Set the value to a hidden field so that it could be sent by POST method
    // We cannot rely on the ScalingList.value because we have to handle also the viewport print 
    document.getElementById("ScaleDenominator").value = scale;
    
    return scale;
}

function getPrintDpi()
{
    return document.getElementById("DPIList").value;
}

function drawCaptureBox()
{
    var mapCapturer = getMapFrame().mapCapturer;
    mapCapturer.setSize(getPaperSize(), getScale());
}

/*
Script: Cookie.js
    Class for creating, loading, and saving browser Cookies.

License:
    MIT-style license.

Credits:
    Based on the functions by Peter-Paul Koch (http://quirksmode.org).
*/

var Cookie = function(key, options)
{
    this.initialize(key, options);
};

Cookie.prototype = 
{
    options: {
        path: false,
        domain: false,
        duration: false,
        secure: false,
        document: document
    },

    initialize: function(key, options){
        this.key = key;
        
        if (options != null)
        {
            for (var key in this.options)
            {
                if (options[key] != null)
                {
                    this.options[key] = options[key];
                }
            }
        }
    },

    write: function(value){
        value = encodeURIComponent(value);
        if (this.options.domain) value += '; domain=' + this.options.domain;
        if (this.options.path) value += '; path=' + this.options.path;
        if (this.options.duration){
            var date = new Date();
            date.setTime(date.getTime() + this.options.duration * 24 * 60 * 60 * 1000);
            value += '; expires=' + date.toGMTString();
        }
        if (this.options.secure) value += '; secure';
        this.options.document.cookie = this.key + '=' + value;
        return this;
    },

    read: function(){
        var escapedKey = this.key.replace(/([-.*+?^${}()|[\]\/\\])/g, '\\$1');
        var value = this.options.document.cookie.match('(?:^|;)\\s*' + escapedKey + '=([^;]*)');
        return (value) ? decodeURIComponent(value[1]) : null;
    },

    dispose: function(){
        new Cookie(this.key, $merge(this.options, {duration: -1})).write('');
        return this;
    }

};

Cookie.write = function(key, value, options){
    return new Cookie(key, options).write(value);
};

Cookie.read = function(key){
    return new Cookie(key).read();
};

Cookie.dispose = function(key, options){
    return new Cookie(key, options).dispose();
};
