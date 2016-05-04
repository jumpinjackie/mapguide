var settings = require("../settings");
var helper = require("../helper");
var wl = require("./weblayout");

module.exports = {
    tags: ["ajax", "ui"],
    beforeEach: function(browser) {
        helper.setDefaultBrowserSize(browser);
    },
    'UI001 - Context Menu': function (client) {
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
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 250, 250)
                .mouseButtonClick(2)
                .waitForElementVisible("#CtxMenu", 1000, true)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - UI001 - 001 - Context Menu"))
            .end();
    },
    'UI002 - Task Pane Toggle': function (client) {
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
            .waitForElementPresent("#taskBar", 5000, true)
            .frame("taskBar")
                .waitForElementVisible("#btnClose", 1000, true)
                .moveToElement("#btnClose", 10, 10)
                .mouseButtonClick(0)
                .pause(1500) //Breathe, let the toggle and re-render take effect
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - UI002 - 001 - Task Pane hidden"))
            .frame(null)
            .frame("tbFrame")
                .waitForElementVisible("#divOpenTaskbarBtn", 1000, true)
                    .moveToElement("#divOpenTaskbarBtn", 10, 10)
                    .mouseButtonClick(0)
                .pause(1500) //Breathe, let the toggle and re-render take effect
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - UI002 - 002 - Task Pane shown"))
            .frame(null)
            .end();
    },
    'UI003 - Pane Collapsing': function (client) {
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
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .waitForElementVisible("#legendIcon", 1000, true)
                .waitForElementVisible("#propIcon", 1000, true)
                    .click("#legendIcon")
                    .pause(500) //breathe to let the toggle happen
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - UI003 - 001 - Legend Collapsed"))
                    .click("#propIcon")
                    .pause(500) //breathe to let the toggle happen
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - UI003 - 002 - Legend and Properties Collapsed"))
            .frame(null)
            .end();
    },
    'UI004 - Pane Toggling': function (client) {
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
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .waitForElementVisible("#LayerCaption", 1000, true)
                .waitForElementVisible("#PropertyCaption", 1000, true)
                    .moveToElement("#LayerCaption", 10, 10)
                        .mouseButtonClick(2)
                        .waitForElementVisible("#InfoMenu1", 1000, true)
                        .moveToElement("#InfoMenu1", 10, 10) //Layers
                            .mouseButtonClick(0)
                            .pause(250) //Breathe and let the toggle happen
                            .saveScreenshot(helper.screenshotName(client, "Ajax - UI004 - 001 - Legend hidden"))
                    .moveToElement("#PropertyCaption", 10, 10)
                        .mouseButtonClick(2)
                        .waitForElementVisible("#InfoMenu2", 1000, true)
                        .moveToElement("#InfoMenu2", 10, 10) //Layers
                            .mouseButtonClick(0)
                            .pause(250) //Breathe and let the toggle happen
                            .saveScreenshot(helper.screenshotName(client, "Ajax - UI004 - 002 - Properties hidden"))
            .frame(null)
            .end();
    }
};