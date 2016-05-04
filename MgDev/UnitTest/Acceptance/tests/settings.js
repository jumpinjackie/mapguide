var settings = {
    host: "localhost",
    port: process.env.MG_PORT || 80,
    vdir: "mapguide",
    defaultWebLayout: "Library://Samples/Sheboygan/Layouts/SheboyganPhp.WebLayout",
    getAjaxViewerUrl: function(webLayout) {
        return "http://" + this.host + ":" + this.port + "/" + this.vdir + "/mapviewerajax/?USERNAME=Anonymous&WEBLAYOUT=" + (webLayout || this.defaultWebLayout);
    },
    getDefaultFlexLayout: function(template) {
        switch (template) {
            case "slate":
                return "Library://Samples/Sheboygan/FlexibleLayouts/Slate.ApplicationDefinition";
            default:
                return "";
        }
    },
    getFusionUrl: function(template, layout) {
        return "http://" + this.host + ":" + this.port + "/" + this.vdir + "/fusion/templates/mapguide/" + template + "/index.html?ApplicationDefinition=" + (layout || this.getDefaultFlexLayout(template));
    }
};
module.exports = settings;