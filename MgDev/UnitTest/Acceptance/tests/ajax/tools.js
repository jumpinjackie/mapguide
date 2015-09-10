var settings = require("../settings");
var helper = require("../helper");
var wl = require("./weblayout");
var cmds = require("../ajaxcmds");
var util = require("util");

module.exports = {
    tags: ["ajax", "tools"],
    beforeEach: function(browser) {
        helper.setDefaultBrowserSize(browser);
    },
    'TOOLS001 - Select': function(client) {
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
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 001 - Zoom Rectangle clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 002 - Zoom Rectangle drag box"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 003 - Zoom Rectangle result"))
            .frame(null)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.SELECT), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.SELECT), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.SELECT))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 004 - Select clicked"))
                .useCss()
            .frame(null)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 360, 360)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 005 - Select drag box"))
                .mouseButtonUp(0)
                .pause(1000) //Breathe for rendering
                .waitForElementVisible("#PropertyPaneToolbar", 1000, true)
                    .assert.containsText("#selLayers option:nth-child(1)", "Parcels", "Expected selected layer dropdown present with entry 'Parcels'")
                    .assert.containsText("#selFeature option:nth-child(1)", "1", "Expected selected feature dropdown present with entry '1'")
                .frame("PropertyCtrl")
                    .waitForElementVisible("#Grid", 1000, true)
                    .assert.elementPresent("#Grid tbody tr.GridCell", "Expected property palette to have attributes of selected feature")
                .frame(null)
            .frame("mapFrame")
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 006 - Select result"))
                .moveToElement("#btnZoomSelectedFeature", 10, 10)
                .mouseButtonClick(0)
                .pause(3000) //Breathe for rendering
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS001 - 007 - Zoom to selected feature result"))
            .frame(null)
            .end();
    },
    'TOOLS002 - Buffer': function(client) {
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
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 001 - Zoom Rectangle clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 002 - Zoom Rectangle drag box"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 003 - Zoom Rectangle result"))
            .frame(null)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.SELECT), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.SELECT), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.SELECT))
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 004 - Select clicked"))
                .useCss()
            .frame(null)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 360, 360)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 005 - Select drag box"))
                .mouseButtonUp(0)
                .pause(1000) //Breathe for rendering
                .waitForElementVisible("#PropertyPaneToolbar", 1000, true)
                    .assert.containsText("#selLayers option:nth-child(1)", "Parcels", "Expected selected layer dropdown present with entry 'Parcels'")
                    .assert.containsText("#selFeature option:nth-child(1)", "1", "Expected selected feature dropdown present with entry '1'")
                .frame("PropertyCtrl")
                    .waitForElementVisible("#Grid", 1000, true)
                    .assert.elementPresent("#Grid tbody tr.GridCell", "Expected property palette to have attributes of selected feature")
                .frame(null)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 006 - Select result"))
            .frame(null)
            .frame("tbFrame")
                .useXpath()
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.BUFFER), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.BUFFER), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.BUFFER))
                .useCss()
            .frame(null)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .waitForElementPresent("frame[name='taskPaneFrame']", 5000, true)
                .frame("taskPaneFrame")
                    .waitForElementVisible("table.RegText", 5000, true)
                    .assert.containsText("table.RegText td.Title", "Create a Buffer")
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 007 - Buffer UI"))
                    .execute(function() {
                        try {
                            var sel = document.querySelector("select[name='UNITS']");
                            for (var i = 0; i < sel.options.length; i++) {
                                if (sel.options[i].value == "me") {
                                    sel.selectedIndex = i;
                                    return "me";
                                    break;
                                }
                            }
                            return null;
                        } catch (e) {
                            return e;
                        }
                    }, function(result) {
                        if (typeof(result.value) == 'string') {
                            console.log("Set units to: " + result.value);
                        } else {
                            console.log("Failed to select unit type: " + result.value);
                        }
                    })
                    .click("#distance")
                    .keys(client.Keys.BACK_SPACE)
                    .setValue("#distance", 100)
                    .moveToElement("#Done", 10, 10)
                    .mouseButtonClick(0)
                    .pause(3000) //Breathe to let buffer do its thing
                    .waitForElementVisible("table.RegText p:nth-child(2)", 1000, true)
                        .assert.containsText("table.RegText p:nth-child(2)", "1 buffer feature was created.")
                        //Snapshot and baseline compare
                        .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS002 - 008 - Buffer result"))
                .frame(null)
            .frame(null)
            .end();
    },
    'TOOLS003 - Measure': function(client) {
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
                    .waitForElementVisible(wl.getToolbarCommand(wl.toolbar.MEASURE), 1000, true)
                    .moveToElement(wl.getToolbarCommand(wl.toolbar.MEASURE), 10, 10)
                    .click(wl.getToolbarCommand(wl.toolbar.MEASURE))
                .useCss()
            .frame(null)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .frame("taskPaneFrame")
                    .waitForElementVisible("table", 5000, true)
                    .assert.containsText("table td.Title", "Measure Distance")
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS003 - 001 - Measure UI"))
                    .execute(function() {
                        try {
                            var sel = document.querySelector("select[name='UNITS']");
                            for (var i = 0; i < sel.options.length; i++) {
                                if (sel.options[i].value == "m") {
                                    sel.options[i].selected = true;
                                    return "m";
                                    break;
                                }
                            }
                            return null;
                        } catch (e) {
                            return e;
                        }
                    }, function(result) {
                        if (typeof(result.value) == 'string') {
                            console.log("Set units to: " + result.value);
                        } else {
                            console.log("Failed to select unit type: " + result.value);
                        }
                    })
                    //FIXME: The labels will still probably still show the default units as we set the select programmatically
                    //and not through browser controlled actions
                .frame(null)
            .frame(null)
            .frame("mapFrame")
                .moveToElement("#map", 250, 250)
                .mouseButtonClick(0)
                .pause(1000) //Breathe to let measure results render
                .moveToElement("#map", 265, 260)
                .mouseButtonClick(0)
                .pause(1000) //Breathe to let measure results render
                .moveToElement("#map", 270, 240)
                .mouseButtonClick(0)
                .pause(1000) //Breathe to let measure results render
                .moveToElement("#map", 240, 250)
                .mouseButtonClick(0)
                .pause(1000) //Breathe to let measure results render
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS003 - 002 - Measure result"))
            .frame(null)
            //NOTE: For nested frames, you have to navigate down each frame individually. This is a selenium problem
            .frame("taskFrame")
                .frame("taskPaneFrame")
                    .useXpath()
                        .moveToElement("//input[contains(@onclick, 'ClearMeasure()')]", 10, 10)
                        .mouseButtonClick(0)
                        .pause(2000) //Breathe to let selection clear
                    .useCss()
                    //Snapshot and baseline compare
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS003 - 003 - Measure cleared"))
                .frame(null)
            .frame(null)
            .end();
    },
    'TOOLS004 - Radius Select': function(client) {
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
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 001 - Zoom Rectangle clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 002 - Zoom Rectangle drag box"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 003 - Zoom Rectangle result"))
                //Flyout context menu
                .moveToElement("#map", 250, 250)
                .mouseButtonClick(2)
                .waitForElementVisible("#CtxMenu", 1000, true)
                    .waitForElementVisible("tr[name='Sub:1']", 1000, true)
                        .moveToElement("tr[name='Sub:1']", 10, 10)
                        .moveToElement("#FlyoutDiv31", 10, 10) //This is the select radius context menu command. Assuming deterministic DOM element generation here.
                            .assert.containsText("#FlyoutDiv3T1", "Select Radius") //But check just in case
                            .click("#FlyoutDiv3T1", function() { console.log("Clicked 'Select Radius' context menu item"); })
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 004 - Select Radius circle"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 005 - Select Radius result"))
                .waitForElementVisible("#PropertyPaneToolbar", 1000, true)
                    .assert.containsText("#selLayers option:nth-child(1)", "Parcels", "Expected selected layer dropdown present with entry 'Parcels'")
                    .assert.containsText("#selFeature option:nth-child(1)", "1", "Expected selected feature dropdown present with entry '1'")
                .frame("PropertyCtrl")
                    .waitForElementVisible("#Grid", 1000, true)
                    .assert.elementPresent("#Grid tbody tr.GridCell", "Expected property palette to have attributes of selected feature")
                .frame(null)
            .frame(null)
            .end();
    }
    //Can't test polygon select yet as nightwatch.js doesn't appear to allow us to perform [ctrl + click]
    //needed to finish polygon digitization
    /*
    'TOOLS005 - Polygon Select': function(client) {
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
                    .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 001 - Zoom Rectangle clicked"))
                .useCss()
            .frame(null)
            .waitForElementPresent("#mapFrame", 5000, true)
            .frame("mapFrame")
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 002 - Zoom Rectangle drag box"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 003 - Zoom Rectangle result"))
                //Flyout context menu
                .moveToElement("#map", 250, 250)
                .mouseButtonClick(2)
                .waitForElementVisible("#CtxMenu", 1000, true)
                    .waitForElementVisible("tr[name='Sub:1']", 1000, true)
                        .moveToElement("tr[name='Sub:1']", 10, 10)
                        .moveToElement("#FlyoutDiv32", 10, 10) //This is the select radius context menu command. Assuming deterministic DOM element generation here.
                            .assert.containsText("#FlyoutDiv3T2", "Select Polygon") //But check just in case
                            .click("#FlyoutDiv3T2", function() { console.log("Clicked 'Select Polygon' context menu item"); })
                .moveToElement("#map", 350, 350)
                .mouseButtonDown(0)
                .moveToElement("#map", 400, 400)
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 004 - Select Radius circle"))
                .mouseButtonUp(0)
                .pause(3000) //Breathe for rendering
                //Snapshot and baseline compare
                .saveScreenshot(helper.screenshotName(client, "Ajax - TOOLS004 - 005 - Select Radius result"))
                .waitForElementVisible("#PropertyPaneToolbar", 1000, true)
                    .assert.containsText("#selLayers option:nth-child(1)", "Parcels", "Expected selected layer dropdown present with entry 'Parcels'")
                    .assert.containsText("#selFeature option:nth-child(1)", "1", "Expected selected feature dropdown present with entry '1'")
                .frame("PropertyCtrl")
                    .waitForElementVisible("#Grid", 1000, true)
                    .assert.elementPresent("#Grid tbody tr.GridCell", "Expected property palette to have attributes of selected feature")
                .frame(null)
            .frame(null)
            .end();
    }
    */
};