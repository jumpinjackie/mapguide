var cmdCodes = {
    ZOOM_IN: 1,
    ZOOM_OUT: 2,
    ZOOM_RECT: 3
};

module.exports = {
    TB: cmdCodes,
    getToolbarCommand: function(toolbarRootSelector, cmd) {
        var className = null;
        switch (cmd) {
            case cmdCodes.ZOOM_IN:
                className = "zoom-in-fixed";
                break;
            case cmdCodes.ZOOM_OUT:
                className = "zoom-out-fixed";
                break;
            case cmdCodes.ZOOM_RECT:
                className = "zoom-in";
                break;
            default:
                throw new Error("Unknown command code: " + cmd);
        }
        return toolbarRootSelector + " div.jxBarScroller div.jxBarWrapper ul.jxToolbar li.jxToolItem span.jxButtonContainer a.jxButton span.jxButtonContent img." + className;
    }
};