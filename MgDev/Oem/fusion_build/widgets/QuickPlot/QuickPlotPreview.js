/**
 * Copyright (C) 2010 Autodesk, Inc. All rights reserved.
 */

function innerLoaded()
{
    if (parent.Fusion)
    {
        var widget = parent.Fusion.getWidgetsByType("QuickPlot")[0];
        var disclaimer = widget.disclaimer;
        document.getElementById("legalNotice").innerHTML = disclaimer;
        widget.previewInnerLoaded();
    }
}

function printIt()
{
    parent.Fusion.getWidgetsByType("QuickPlot")[0].printPreview();
}

function cancelPreview()
{
    parent.Fusion.getWidgetsByType("QuickPlot")[0].cancelPreview();
}

function doPrint()
{
    window.focus();
    window.print();
}
