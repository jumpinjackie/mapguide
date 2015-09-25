var settings = require("../settings");
var helper = require("../helper");
var wl = require("./weblayout");

module.exports = {
    tags: ["ajax", "legend"],
    beforeEach: function(browser) {
        helper.setDefaultBrowserSize(browser);
    },
    'LEGEND001 - Toggle Group': function (client) {
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
            .frame("mapFrame")
                .waitForElementVisible("#LegendCtrl", 5000, true)
                .frame("LegendCtrl")
                    .waitForElementPresent("frame[name='legendUiFrame']", 5000, true)
                    .frame("legendUiFrame")
                        .useXpath()
                            //Base Map group checkbox node
                            .moveToElement("//div[@id='Tree']/div[1]/table/tbody/tr[1]/td[2]/img", 10, 10)
                            .mouseButtonClick(0)
                            .pause(3000) //Breathe for rendering
                            //Snapshot and baseline compare
                            .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND001 - 001 - Base Map Group turned off"))
                            .mouseButtonClick(0)
                            .pause(3000) //Breathe for rendering
                            //Snapshot and baseline compare
                            .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND001 - 002 - Base Map Group turned back on"))
                        .useCss()
                    .frame(null)
                .frame(null)
            .frame(null)
            .end();
    },
    'LEGEND002 - Toggle Layer': function (client) {
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
            .frame("mapFrame")
                .waitForElementVisible("#LegendCtrl", 5000, true)
                .frame("LegendCtrl")
                    .waitForElementPresent("frame[name='legendUiFrame']", 5000, true)
                    .frame("legendUiFrame")
                        .useXpath()
                            //Hydrograpy checkbox node
                            .moveToElement("//div[@id='Tree']/div[1]/div[1]/div[2]/table[1]/tbody/tr[1]/td[2]/img", 10, 10)
                            .mouseButtonClick(0)
                            .pause(3000) //Breathe for rendering
                            //Snapshot and baseline compare
                            .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND002 - 001 - Hydrography turned off"))
                            .mouseButtonClick(0)
                            .pause(3000) //Breathe for rendering
                            //Snapshot and baseline compare
                            .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND002 - 002 - Hydrography turned back on"))
                        .useCss()
                    .frame(null)
                .frame(null)
            .frame(null)
            .end();
    },
    'LEGEND003 - Show invisible layers context menu': function(client) {
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
            .frame("mapFrame")
                .waitForElementVisible("#LegendCtrl", 5000, true)
                .frame("LegendCtrl")
                    .waitForElementPresent("frame[name='legendUiFrame']", 5000, true)
                    .frame("legendUiFrame")
                        .useXpath()
                            //Hydrograpy checkbox node
                            .moveToElement("//div[@id='Tree']/div[1]/div[1]/div[2]/table[1]/tbody/tr[1]/td[2]/img", 10, 10)
                            .mouseButtonClick(2)
                        .useCss()
                        .waitForElementVisible("#legendUi", 1000, true)
                            .waitForElementVisible("#Legend", 1000, true)
                                .waitForElementVisible("#TblLegend", 1000, true)
                                .useXpath()
                                    .waitForElementPresent("//table[@id='TblLegend']/tbody/tr/td/span[contains(text(), 'Show invisible layers')]", 1000, true)
                                    .moveToElement("//table[@id='TblLegend']/tbody/tr/td/span[contains(text(), 'Show invisible layers')]", 10, 10)
                                    //Snapshot and baseline compare
                                    .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND003 - 001 - Legend context menu"))
                                    .mouseButtonClick(0)
                                    .pause(2000) //Breathe to let legend update happen
                                    //Snapshot and baseline compare
                                    .saveScreenshot(helper.screenshotName(client, "Ajax - LEGEND003 - 002 - Invisible layers shown"))
                                .useCss()
                    .frame(null)
                .frame(null)
            .frame(null)
            .end();
    }
}