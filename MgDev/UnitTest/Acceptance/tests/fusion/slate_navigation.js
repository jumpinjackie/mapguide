var settings = require("../settings");
var helper = require("../helper");
var appDef = require("./appdef");

module.exports = {
    tags: ["fusion", "slate", "navigation"],
    beforeEach: function(browser) {
        helper.setDefaultBrowserSize(browser);
    },
    'SLATE_NAV001 - Zoom In': function(client) {
        var funcName = arguments.callee.name;
        client.url(settings.getFusionUrl("slate"))
            .waitForElementVisible("div.olMapViewport", 8000, true)
            .waitForElementVisible("img.olTileImage", 8000, true)
            .waitForElementVisible("#Navigator", 2000, true)
            .waitForElementVisible("#TaskPane_IFRAME", 2000, true)
            .frame("TaskPane_IFRAME")
                .waitForElementVisible("h1", 3000, true)
                .assert.containsText("h1", "Task Pane Help")
            .frame(null)
            .moveToElement(appDef.getToolbarCommand("#ToolbarSecondary", appDef.TB.ZOOM_IN), 10, 10)
            .mouseButtonClick(0)
            .pause(3000) //Let rendering breathe
            .saveScreenshot(helper.screenshotName(client, "Fusion - " + funcName + " - 001 - Zoom In clicked"))
            .end();
    },
    'SLATE_NAV002 - Zoom Out': function(client) {
        var funcName = arguments.callee.name;
        client.url(settings.getFusionUrl("slate"))
            .waitForElementVisible("div.olMapViewport", 8000, true)
            .waitForElementVisible("img.olTileImage", 8000, true)
            .waitForElementVisible("#Navigator", 2000, true)
            .waitForElementVisible("#TaskPane_IFRAME", 2000, true)
            .frame("TaskPane_IFRAME")
                .waitForElementVisible("h1", 3000, true)
                .assert.containsText("h1", "Task Pane Help")
            .frame(null)
            .moveToElement(appDef.getToolbarCommand("#ToolbarSecondary", appDef.TB.ZOOM_OUT), 10, 10)
            .mouseButtonClick(0)
            .pause(3000) //Let rendering breathe
            .saveScreenshot(helper.screenshotName(client, "Fusion - " + funcName + " - 001 - Zoom Out clicked"))
            .end();
    },
    'SLATE_NAV003 - Zoom Rect': function(client) {
        var funcName = arguments.callee.name;
        client.url(settings.getFusionUrl("slate"))
            .waitForElementVisible("div.olMapViewport", 8000, true)
            .waitForElementVisible("img.olTileImage", 8000, true)
            .waitForElementVisible("#Navigator", 2000, true)
            .waitForElementVisible("#TaskPane_IFRAME", 2000, true)
            .frame("TaskPane_IFRAME")
                .waitForElementVisible("h1", 3000, true)
                .assert.containsText("h1", "Task Pane Help")
            .frame(null)
            .moveToElement(appDef.getToolbarCommand("#ToolbarSecondary", appDef.TB.ZOOM_RECT), 10, 10)
            .mouseButtonClick(0)
            .pause(3000) //Let rendering breathe
            .saveScreenshot(helper.screenshotName(client, "Fusion - " + funcName + " - 001 - Zoom Rect clicked"))
            .end();
    }
};