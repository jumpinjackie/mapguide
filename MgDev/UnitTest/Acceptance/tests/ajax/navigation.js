var settings = require("../settings");
var helper = require("../helper");
var wl = require("./weblayout");

module.exports = {
    tags: ["ajax", "navigation"],
    beforeEach: function(browser) {
        helper.setDefaultBrowserSize(browser);
    },
    'NAV001 - Zoom In': function (client) {
        client.url(settings.getAjaxViewerUrl())
            .waitForElementPresent("#taskFrame", 5000, true)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .waitForElementPresent("frame[name='taskPaneFrame']", 5000, true)
                .frame("taskPaneFrame")
                    .waitForElementVisible("table.RegText", 5000, true)
                    .assert.containsText("table.RegText td.Title", "Overview")
                .frame(null)
            .frame(null)
            .waitForElementPresent("#tbFrame", 5000, true)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.ZOOM_IN), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.ZOOM_IN), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.ZOOM_IN))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - NAV001 - 001 - Zoom In clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 250, 250)
                .click("#map")
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - NAV001 - 002 - Zoom In result"))
            .end();
    },
    'NAV002 - Zoom Out': function (client) {
        client.url(settings.getAjaxViewerUrl())
            .waitForElementPresent("#taskFrame", 5000, true)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .waitForElementPresent("frame[name='taskPaneFrame']", 5000, true)
                .frame("taskPaneFrame")
                    .waitForElementVisible("table.RegText", 5000, true)
                    .assert.containsText("table.RegText td.Title", "Overview")
                .frame(null)
            .frame(null)
            .waitForElementPresent("#tbFrame", 5000, true)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.ZOOM_OUT), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.ZOOM_OUT), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.ZOOM_OUT))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - NAV002 - 001 - Zoom Out clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 250, 250)
                .click("#map")
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - NAV002 - 002 - Zoom Out clicked"))
            .end();
    },
    'NAV003 - Zoom Rectangle': function (client) {
        client.url(settings.getAjaxViewerUrl())
            .waitForElementPresent("#taskFrame", 5000, true)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .waitForElementPresent("frame[name='taskPaneFrame']", 5000, true)
                .frame("taskPaneFrame")
                    .waitForElementVisible("table.RegText", 5000, true)
                    .assert.containsText("table.RegText td.Title", "Overview")
                .frame(null)
            .frame(null)
            .waitForElementPresent("#tbFrame", 5000, true)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.ZOOM_RECT), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.ZOOM_RECT), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.ZOOM_RECT))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - NAV003 - 001 - Zoom Rectangle clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 250, 250)
                .mouseButtonDown(0)
                .moveToElement("#map", 350, 350)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - NAV003 - 002 - Zoom Rectangle drag box"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - NAV003 - 003 - Zoom Rectangle result"))
            .end();
    },
    'NAV004 - Pan': function (client) {
        client.url(settings.getAjaxViewerUrl())
            .waitForElementPresent("#taskFrame", 5000, true)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .waitForElementPresent("frame[name='taskPaneFrame']", 5000, true)
                .frame("taskPaneFrame")
                    .waitForElementVisible("table.RegText", 5000, true)
                    .assert.containsText("table.RegText td.Title", "Overview")
                .frame(null)
            .frame(null)
            .waitForElementPresent("#tbFrame", 5000, true)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.PAN), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.PAN), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.PAN))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - NAV004 - 001 - Pan clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 250, 250)
                .mouseButtonUp(0)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - NAV004 - 002 - Pan result"))
            .end();
    }
}